#include "parser.hh"
#include "server.hh"
#include "log.hh"
#include "utils.hh"
#include "connection.hh"
#include <regex>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace {
template<typename Pac_t, typename Handle_t>
void
parse_headers(Pac_t pac, Handle_t handler)
{
	pac->connection()->async_read_until("\n",
		[=](const boost::system::error_code& err, size_t) {
			if(err) {
				handler(nullptr);
				return;
			}
			static const std::regex key_val_reg("([[:print:]]+): ([[:print:]]*)");
			std::smatch results;
			std::istream in(&pac->connection()->readBuffer());
			std::string line;
			getline(in, line);
			if(line == "\r" || line == "") {	/**< 头部最后的\r\n" */
				handler(pac);
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
		[=](const boost::system::error_code& err, size_t) {
			if(err) {
				Log("DEBUG") << __FILE__ << ":" << __LINE__;
				Log("ERROR") << err.message();
				handler(nullptr);
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
				pac->connection()->async_read(boost::asio::transfer_exactly(need_read),
					[=](const boost::system::error_code& err, size_t n) {
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
							handler(nullptr);
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
					handler(nullptr);
					return;
				}
				char buf[length];
				in.read(buf, length);
				pac->out().write(buf, length);	
			}

			pac->connection()->async_read_until("\n",
				[=](const boost::system::error_code& err, size_t n) {
					if(err) {
						Log("DEBUG") << __FILE__ << ":" << __LINE__;
						Log("ERROR") << err.message();
						if(length != 0)
						handler(nullptr);
						return;
					}
					std::istream in(&pac->connection()->readBuffer());
					std::string ignore;
					getline(in, ignore);
					if(length == 0) {
						handler(pac);
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
			handler(pac);
		}
	} else {
		size_t length = boost::lexical_cast<size_t>(*h);
		if(length == 0) {
			handler(pac);
			return;
		}

		size_t already_read = pac->connection()->readBuffer().in_avail();
		int need_read = length - already_read;

		if(need_read < 0) {
			handler(nullptr);
			return;
		}

		if(already_read)
			pac->out() << &pac->connection()->readBuffer();

		if(need_read == 0) {
			handler(pac);
			return;
		}

		pac->connection()->async_read(boost::asio::transfer_exactly(need_read),
			[=](const boost::system::error_code& err, size_t n) {
				if(err || static_cast<int>(n) != need_read) {	/**< 避免警告 */
					handler(nullptr);
					return;
				}
				pac->out() << &pac->connection()->readBuffer();
				handler(pac);
			}
		);
	}
}

void
parse_request_first_line(RequestPtr req, std::function<void(RequestPtr)> handler)
{
	req->connection()->async_read_until("\n", 
		[req, handler](const boost::system::error_code& err, size_t n) {
			if(err) {
				handler(nullptr);
				return;
			}
			std::istream in(&req->connection()->readBuffer());
			std::string line;
			getline(in, line);
			StringTokenizer first_line_st(line);
			std::string url;
			StringTokenizer url_st;

			if(!first_line_st.hasMoreTokens())
				goto bad_request;
			req->setMethod(first_line_st.nextToken());

			if(!first_line_st.hasMoreTokens())
				goto bad_request;
			url = first_line_st.nextToken();
			if(!urlDecode(url) || url[0] != '/')
				goto bad_request;
			url_st = StringTokenizer(url, '?');
			req->setPath(url_st.nextToken());
			if(url_st.hasMoreTokens())
				req->setQueryString(url_st.nextToken());

			if(!first_line_st.hasMoreTokens())
				goto bad_request;
			req->setVersion(first_line_st.nextToken());

			handler(req);
			return;

			bad_request:
				Log("WARNING") << "Bad request";
				handler(nullptr);
		}
	);

}

void
parse_response_first_line(ResponsePtr res, std::function<void(ResponsePtr)> handler)
{
	res->connection()->async_read_until("\n", 
		[=](const boost::system::error_code& err, size_t n) {
			if(err) {
				Log("DEBUG") << __FILE__ << ":" << __LINE__;
				Log("ERROR") << err.message();
				handler(nullptr);
				return;
			}
			std::smatch results;
			static const std::regex first_line_reg("([[:print:]]*) ([0-9]*) (((?!\r)[[:print:] ])*)");
			std::istream in(&res->connection()->readBuffer());
			std::string line;
			getline(in, line);
			if(std::regex_search(line, results, first_line_reg)) {
				res->setVersion(results.str(1));
				res->setStatus(boost::lexical_cast<Response::status_t>(results.str(2)));
				res->setMessage(results.str(3));
				handler(res);
			} else {
				handler(nullptr);
			}
		}
	);

}

}

#define PARSE(package, pac)						\
do {									\
	parse_##package##_first_line(pac,				\
		[=](decltype(pac) pac) {				\
			if(!pac) {					\
				handler(nullptr);			\
				return;					\
			}						\
			parse_headers(pac,				\
				[=](decltype(pac) pac) {		\
					if(!pac) {			\
						handler(nullptr);	\
						return;			\
					}				\
					pac->parseCookie();		\
					parse_body(pac, handler);	\
				}					\
			);						\
		}							\
	);								\
} while(0)
		
void
parseRequest(RequestPtr req, std::function<void(RequestPtr)> handler)
{
	PARSE(request, req);
}

void
parseResponse(ResponsePtr res, std::function<void(ResponsePtr)> handler)
{
	PARSE(response, res);
}
