#include "parser.hh"
#include "server.hh"
#include "buffer.hh"
#include <regex>
#include <asio.hpp>
#include "log.hh"

namespace {

inline size_t 
to_size(const std::string& s)
{
	std::stringstream ss(s);
	size_t ret = 0;
	ss >> ret;
	return ret;
}

void
parse_headers(RequestPtr req, std::function<void(RequestPtr, bool)> handler)
{
	req->connection()->async_read_until("\r\n\r\n",
		[=](const asio::error_code& err, size_t) {
			if(err) {
				handler(req, false);
				return;
			}
			static const std::regex key_val_reg("([[:print:]]+): ([[:print:]]*)");
			std::smatch results;
			std::istream in(&req->connection()->buffer());
			std::string line;
			while(getline(in, line)) {
				if(line == "\r")	/**< 头部最后的\r\n" */
					break;
				if(std::regex_search(line, results, key_val_reg)) {
					req->addHeader(results.str(1), results.str(2));
				} else {
					assert(0);
				}
			}
			handler(req, true);
		}
	);				
}

void
parse_body(RequestPtr req,
	std::function<void(RequestPtr, bool)> handler)
{
	auto h = req->getHeader("Content-Length");
	if(h.size() == 0) {
		handler(req, true);
	} else {
		size_t length = to_size(h[0]);
		if(length == 0) {
			handler(req, false);
			return;
		}
		size_t already_read = req->connection()->buffer().in_avail();
		int need_read = length - already_read;

		if(need_read < 0) {
			handler(req, false);
			return;
		}

		if(already_read)
			req->out() << &req->connection()->buffer();

		if(need_read == 0) {
			handler(req, true);
			return;
		}
		req->connection()->async_read(asio::transfer_exactly(need_read),
			[=](const asio::error_code& err, size_t n) {
				if(err || static_cast<int>(n) != need_read) {	/**< 避免警告 */
					handler(req, false);
					return;
				}
				req->out() << &req->connection()->buffer();
				handler(req, true);
			}
		);
	}
}

void
parse_request_first_line(RequestPtr req, std::function<void(RequestPtr, bool)> handler)
{
	req->connection()->async_read_until("\r\n", 
		[=](const asio::error_code& err, size_t n) {
			if(err) {
				handler(req, false);
				return;
			}
			std::smatch results;
			static const std::regex first_line_reg(
				"(GET|POST|PUT|DELETE) (/[[:print:]]*) (HTTP/1.1|HTTP/1.0)");
			std::istream in(&req->connection()->buffer());
			std::string line;
			getline(in, line);
			if(std::regex_search(
				line, results, first_line_reg)) {
				req->setMethod(results.str(1));
				req->setUri(results.str(2));
				req->setVersion(results.str(3));
				handler(req, true);
				return;
			} else {
				handler(req, false);
			}
		}
	);

}

}

void
parseRequest(RequestPtr req,
	std::function<void(RequestPtr, bool)> handler)
{
	parse_request_first_line(req, 
		[=](RequestPtr req, bool good) {
			if(!good) {
				handler(req, good);
				return;
			}
			parse_headers(req,
				[=](RequestPtr req, bool good) {
					if(!good) {
						handler(req, good);
						return;
					}				
					parse_body(req, handler);
				}
			);
		}
	);	
						
}
