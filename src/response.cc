#include "response.hh"
#include "server.hh"
#include "log.hh"
#include "connection.hh"
#include <string>

#define PARSE_STATUS(ststus)				\
	switch (status) {				\
		case Response::ok:			\
    			return ok;			\
  		case Response::created:			\
    			return created;			\
  		case Response::accepted:		\
    			return accepted;		\
  		case Response::no_content:		\
    			return no_content;		\
  		case Response::multiple_choices:	\
    			return multiple_choices;	\
  		case Response::moved_permanently:	\
    			return moved_permanently;	\
  		case Response::moved_temporarily:	\
    			return moved_temporarily;	\
  		case Response::not_modified:		\
    			return not_modified;		\
  		case Response::bad_request:		\
    			return bad_request;		\
  		case Response::unauthorized:		\
    			return unauthorized;		\
  		case Response::forbidden:		\
   	 		return forbidden;		\
  		case Response::not_found:		\
    			return not_found;		\
  		case Response::internal_server_error:	\
    			return internal_server_error;	\
  		case Response::not_implemented:		\
    			return not_implemented;		\
  		case Response::bad_gateway:		\
    			return bad_gateway;		\
  		case Response::service_unavailable:	\
    			return service_unavailable;	\
  		default:				\
    			return internal_server_error;	\
  	}

namespace status_strings {

const char ok[] =
 	"HTTP/1.1 200 OK";
const char created[] =
	"HTTP/1.1 201 Created";
const char accepted[] =
	"HTTP/1.1 202 Accepted";
const char no_content[] =
	"HTTP/1.1 204 No Content";
const char multiple_choices[] =
	"HTTP/1.1 300 Multiple Choices";
const char moved_permanently[] =
	"HTTP/1.1 301 Moved Permanently";
const char moved_temporarily[] =
	"HTTP/1.1 302 Moved Temporarily";
const char not_modified[] =
	"HTTP/1.1 304 Not Modified";
const char bad_request[] =
	"HTTP/1.1 400 Bad Request";
const char unauthorized[] =
	"HTTP/1.1 401 Unauthorized";
const char forbidden[] =
	"HTTP/1.1 403 Forbidden";
const char not_found[] =
	"HTTP/1.1 404 Not Found";
const char internal_server_error[] =
	"HTTP/1.1 500 Internal Server Error";
const char not_implemented[] =
	"HTTP/1.1 501 Not Implemented";
const char bad_gateway[] =
	"HTTP/1.1 502 Bad Gateway";
const char service_unavailable[] =
	"HTTP/1.1 503 Service Unavailable";

std::string
status_head(Response::status_t status)
{
	PARSE_STATUS(status);
}

} // namespace status_strings

namespace stock_replies {

const char ok[] = "";
const char created[] =
  "<html>"
  "<head><title>Created</title></head>"
  "<body><h1>201 Created</h1></body>"
  "</html>";
const char accepted[] =
  "<html>"
  "<head><title>Accepted</title></head>"
  "<body><h1>202 Accepted</h1></body>"
  "</html>";
const char no_content[] =
  "<html>"
  "<head><title>No Content</title></head>"
  "<body><h1>204 Content</h1></body>"
  "</html>";
const char multiple_choices[] =
  "<html>"
  "<head><title>Multiple Choices</title></head>"
  "<body><h1>300 Multiple Choices</h1></body>"
  "</html>";
const char moved_permanently[] =
  "<html>"
  "<head><title>Moved Permanently</title></head>"
  "<body><h1>301 Moved Permanently</h1></body>"
  "</html>";
const char moved_temporarily[] =
  "<html>"
  "<head><title>Moved Temporarily</title></head>"
  "<body><h1>302 Moved Temporarily</h1></body>"
  "</html>";
const char not_modified[] =
  "<html>"
  "<head><title>Not Modified</title></head>"
  "<body><h1>304 Not Modified</h1></body>"
  "</html>";
const char bad_request[] =
  "<html>"
  "<head><title>Bad Request</title></head>"
  "<body><h1>400 Bad Request</h1></body>"
  "</html>";
const char unauthorized[] =
  "<html>"
  "<head><title>Unauthorized</title></head>"
  "<body><h1>401 Unauthorized</h1></body>"
  "</html>";
const char forbidden[] =
  "<html>"
  "<head><title>Forbidden</title></head>"
  "<body><h1>403 Forbidden</h1></body>"
  "</html>";
const char not_found[] =
  "<html>"
  "<head><title>Not Found</title></head>"
  "<body><h1>404 Not Found</h1></body>"
  "</html>";
const char internal_server_error[] =
  "<html>"
  "<head><title>Internal Server Error</title></head>"
  "<body><h1>500 Internal Server Error</h1></body>"
  "</html>";
const char not_implemented[] =
  "<html>"
  "<head><title>Not Implemented</title></head>"
  "<body><h1>501 Not Implemented</h1></body>"
  "</html>";
const char bad_gateway[] =
  "<html>"
  "<head><title>Bad Gateway</title></head>"
  "<body><h1>502 Bad Gateway</h1></body>"
  "</html>";
const char service_unavailable[] =
  "<html>"
  "<head><title>Service Unavailable</title></head>"
  "<body><h1>503 Service Unavailable</h1></body>"
  "</html>";

std::string
status_body(Response::status_t status)
{
	PARSE_STATUS(status);
}

} // namespace stock_replies

namespace {

template<typename T>
std::string
to_string(T&& some_thing)
{
        std::string ret;
        std::stringstream s(ret);
        s << some_thing;
        s >> ret;
        return ret;
}

}

void
Response::flush()
{
	std::ostream send_buf(&connection()->buffer());
		
	/** 发送回应包 */
	if(status_) {	/**< 如果heander被发送，则设置为0 */
		if(status_ != ok && in().rdbuf()->in_avail() == 0)
			out() << stock_replies::status_body(status_);
		auto h = getHeader("Content-Length");
		if(h.size() == 0) {
			addHeader("Content-Length", to_string(contentLength()));
		}
		send_buf << status_strings::status_head(status_) << "\r\n";
		for(auto&& h : headerMap())
			send_buf << h.name << ": " << h.value << "\r\n";
		send_buf << "\r\n";
		status_ = header_already_send;
	}

	if(in().rdbuf()->in_avail())
		send_buf << in().rdbuf();
	
	connection()->async_write([](const asio::error_code& e, size_t n) {
			if(e) {
				/** TODO:记录错误 */
				Log("ERROR") << "connection写入错误:" << e.message();
			} else {
				/** TODO:记录成功 */
			}
		}
	);
}

/** FIXME: 不能保证noexcept */			
Response::~Response() 
{
	flush();
}
