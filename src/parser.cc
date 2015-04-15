#include "parser.hh"
#include "server.hh"
#include "log.hh"
#include "connection.hh"
#include <regex>
#include <asio.hpp>
#include <iostream>

namespace {

inline size_t 
to_size(const std::string& s)
{
	std::stringstream ss(s);
	size_t ret = 0;
	ss >> ret;
	return ret;
}

template<typename Pac_t, typename Handle_t>
void
parse_headers(Pac_t pac, Handle_t handler)
{
	pac->connection()->async_read_until("\n",
		[=](const asio::error_code& err, size_t) {
			if(err) {
				handler(pac, false);
				return;
			}
			static const std::regex key_val_reg("([[:print:]]+): ([[:print:]]*)");
			std::smatch results;
			std::istream in(&pac->connection()->readBuffer());
			std::string line;
			getline(in, line);
			if(line == "\r" || line == "") {	/**< 头部最后的\r\n" */
				handler(pac, true);
				return;
			}
			if(std::regex_search(line, results, key_val_reg))
				pac->addHeader(results.str(1), results.str(2));
			parse_headers(pac, handler);
		}
	);				
}

template<typename Pac_t, typename Handle_t>
void
read_chunked_body(Pac_t pac, Handle_t handler)
{
	pac->connection()->async_read_until("\n",
		[=](const asio::error_code& err, size_t) {
			if(err) {
				Log("DEBUG") << __FILE__ << ":" << __LINE__;
				Log("ERROR") << err.message();
				handler(pac, false);
				return;
			}
			std::istream in(&pac->connection()->readBuffer());
			size_t length = 0;
			std::string ignore;
			in >> std::hex >> length;
			getline(in, ignore);	/**< 比如A3\r\n, 读取A3后扔掉\r\n */

			size_t already_read = pac->connection()->readBuffer().in_avail();
			int need_read = length - already_read;
			if(need_read > 0) {
				pac->connection()->async_read(asio::transfer_exactly(need_read),
					[=](const asio::error_code& err, size_t n) {
						if(err || static_cast<int>(n) != need_read /**< 避免警告 */
							|| length > 1024 * 1024) {
							if(err) {
								Log("DEBUG") << __FILE__ << ":" << __LINE__;
								Log("ERROR") << err.message();
							}
							if(static_cast<int>(n) != need_read)
								Log("ERROR") << "BUFFER HUNGERY";
							if(length > 1024 * 1024)
								Log("ERROR") << "BUFFER OVERFLOW";
							handler(pac, false);
							return;
						}
						char buf[length];
						std::istream in(&pac->connection()->readBuffer());
						in.read(buf, length);
						pac->out().write(buf, length);
				});
			} else if(length > 0) {
				if(length > 1024 * 1024) {
					Log("ERROR") << "BUFFER OVERFLOW";
					handler(pac, false);
					return;
				}
				char buf[length];
				in.read(buf, length);
				pac->out().write(buf, length);	
			}

			pac->connection()->async_read_until("\n",
				[=](const asio::error_code& err, size_t n) {
					if(err) {
						Log("DEBUG") << __FILE__ << ":" << __LINE__;
						Log("ERROR") << err.message();
						if(length != 0)
						handler(pac, false);
						return;
					}
					std::istream in(&pac->connection()->readBuffer());
					std::string ignore;
					getline(in, ignore);
					if(length == 0) {
						handler(pac, true);
						return;
					}
				}
			);
			if(length)
				read_chunked_body(pac, handler);
		}
	);	
}

template<typename Pac_t, typename Handle_t>
void
parse_body(Pac_t pac, Handle_t handler)
{
	auto h = pac->getHeader("Content-Length");
	if(h == nullptr) {
		auto h = pac->getHeader("Transfer-Encoding");
		if(h != nullptr && strcasecmp(h->c_str(), "chunked")  == 0) {
			read_chunked_body(pac, handler);
		} else {
			handler(pac, true);
		}
	} else {
		size_t length = to_size(*h);
		if(length == 0) {
			handler(pac, true);
			return;
		}

		size_t already_read = pac->connection()->readBuffer().in_avail();
		int need_read = length - already_read;

		if(need_read < 0) {
			handler(pac, false);
			return;
		}

		if(already_read)
			pac->out() << &pac->connection()->readBuffer();

		if(need_read == 0) {
			handler(pac, true);
			return;
		}

		pac->connection()->async_read(asio::transfer_exactly(need_read),
			[=](const asio::error_code& err, size_t n) {
				if(err || static_cast<int>(n) != need_read) {	/**< 避免警告 */
					handler(pac, false);
					return;
				}
				pac->out() << &pac->connection()->readBuffer();
				handler(pac, true);
			}
		);
	}
}

void
parse_request_first_line(RequestPtr req, std::function<void(RequestPtr, bool)> handler)
{
	req->connection()->async_read_until("\n", 
		[req, handler](const asio::error_code& err, size_t n) {
			if(err) {
				handler(req, false);
				return;
			}
			std::smatch results;
			static const std::regex first_line_reg(
				"(GET|POST|PUT|DELETE) (/((?!\\?)[[:print:]])*)"
				"[\\?]?([[:print:]]*)? (HTTP/1.1|HTTP/1.0)");
			std::istream in(&req->connection()->readBuffer());
			std::string line;
			getline(in, line);
			if(std::regex_search(
				line, results, first_line_reg)) {
				req->method() = results.str(1);
				req->path() = results.str(2);
				if(results[4].matched)
					req->query() = results.str(4);
				req->version() = results.str(5);
				handler(req, true);
			} else {
				handler(req, false);
			}
		}
	);

}

void
parse_response_first_line(ResponsePtr res, std::function<void(ResponsePtr, bool)> handler)
{
	res->connection()->async_read_until("\n", 
		[=](const asio::error_code& err, size_t n) {
			if(err) {
				Log("DEBUG") << __FILE__ << ":" << __LINE__;
				Log("ERROR") << err.message();
				handler(res, false);
				return;
			}
			std::smatch results;
			static const std::regex first_line_reg("([[:print:]]*) ([0-9]*) (((?!\r)[[:print:] ])*)");
			std::istream in(&res->connection()->readBuffer());
			std::string line;
			getline(in, line);
			if(std::regex_search(line, results, first_line_reg)) {
				res->version() = results.str(1);
				res->status() = static_cast<Response::status_t>(to_size(results.str(2)));
				res->message() = results.str(3);
				handler(res, true);
			} else {
				handler(res, false);
			}
		}
	);

}

}

void
parseRequest(RequestPtr req, std::function<void(RequestPtr, bool)> handler)
{
	parse_request_first_line(req, 
		[=](RequestPtr req, bool good) {
			if(!good) {
				handler(req, false);
				return;
			}
			parse_headers(req,
				[=](RequestPtr req, bool good) {
					if(!good) {
						handler(req, false);
						return;
					}				
					parse_body(req, handler);
				}
			);
		}
	);	
						
}

void
parseResponse(ResponsePtr res, std::function<void(ResponsePtr, bool)> handler)
{
	parse_response_first_line(res, 
		[=](ResponsePtr res, bool good) {
			if(!good) {
				handler(res, false);
				return;
			}
			parse_headers(res,
				[=](ResponsePtr res, bool good) {
					if(!good) {
						handler(res, false);
						return;
					}				
					parse_body(res, handler);
				}
			);
		}
	);	
}
