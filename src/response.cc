#include "response.hh"
#include "server.hh"
#include "log.hh"
#include "connection.hh"
#include <string>
#include <exception>
#include <cstdio>

#define PARSE_STATUS(ststus)						\
	switch (status) {						\
		case Response::ok:					\
    			return ok;					\
  		case Response::created:					\
    			return created;					\
  		case Response::accepted:				\
    			return accepted;				\
  		case Response::non_authoritative_information:		\
    			return non_authoritative_information;		\
  		case Response::no_content:				\
    			return no_content;				\
  		case Response::reset_content:				\
    			return reset_content;				\
  		case Response::partial_content:				\
    			return partial_content;				\
  		case Response::muti_status:				\
    			return muti_status;				\
  		case Response::multiple_choices:			\
    			return multiple_choices;			\
  		case Response::moved_permanently:			\
    			return moved_permanently;			\
  		case Response::moved_temporarily:			\
    			return moved_temporarily;			\
  		case Response::see_other:				\
    			return see_other;				\
  		case Response::not_modified:				\
    			return not_modified;				\
  		case Response::bad_request:				\
    			return bad_request;				\
  		case Response::unauthorized:				\
    			return unauthorized;				\
  		case Response::forbidden:				\
   	 		return forbidden;				\
  		case Response::not_found:				\
    			return not_found;				\
  		case Response::internal_server_error:			\
    			return internal_server_error;			\
  		case Response::not_implemented:				\
    			return not_implemented;				\
  		case Response::bad_gateway:				\
    			return bad_gateway;				\
  		case Response::service_unavailable:			\
    			return service_unavailable;			\
  		default:						\
    			return internal_server_error;			\
  	}

namespace status_strings {

const char ok[] =
 	"HTTP/1.1 200 OK";
const char created[] =
	"HTTP/1.1 201 Created";
const char accepted[] =
	"HTTP/1.1 202 Accepted";
const char non_authoritative_information[] =
	"HTTP/1.1 203 Non-Authoritative Information";
const char no_content[] =
	"HTTP/1.1 204 No Content";
const char reset_content[] =
	"HTTP/1.1 205 Reset Content";
const char partial_content[] =
	"HTTP/1.1 206 Partial Content";
const char muti_status[] =
	"HTTP/1.1 207 Muti Status";
const char multiple_choices[] =
	"HTTP/1.1 300 Multiple Choices";
const char moved_permanently[] =
	"HTTP/1.1 301 Moved Permanently";
const char moved_temporarily[] =
	"HTTP/1.1 302 Moved Temporarily";
const char see_other[] =
	"HTTP/1.1 303 See Other";
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
const char non_authoritative_information[] =
  "<html>"
  "<head><title>Non-Authoritative Information</title></head>"
  "<body><h1>203 Non-Authoritative Information</h1></body>"
  "</html>";
const char no_content[] =
  "<html>"
  "<head><title>No Content</title></head>"
  "<body><h1>204 Content</h1></body>"
  "</html>";
const char reset_content[] =
  "<html>"
  "<head><title>Reset Content</title></head>"
  "<body><h1>205 Reset Content</h1></body>"
  "</html>";
const char partial_content[] =
  "<html>"
  "<head><title>Partial Content</title></head>"
  "<body><h1>206 Partial Content</h1></body>"
  "</html>";
const char muti_status[] =
  "<html>"
  "<head><title>Muti Status</title></head>"
  "<body><h1>207 Muti Status</h1></body>"
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
const char see_other[] =
  "<html>"
  "<head><title>See Other</title></head>"
  "<body><h1>303 See Other</h1></body>"
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

#define FLUSH_FIRST_LINE()									\
do {												\
	if(msg_ == "" || version_ == "" || !status_) {						\
		connection()->async_write(status_strings::status_head(status_) + "\r\n");	\
	} else {										\
		connection()->async_write(version_ + " " +					\
			boost::lexical_cast<std::string>(status_) + " "				\
			+ version_ + "\r\n");							\
	}											\
} while(0)

void
Response::flush()
{
	if(connection() == nullptr)
		return;
	if(!chunked()) {	/**< 说明是第一次手动调用flush */
		/** 由于是第一次调用，说明第一行并未发送 */
		setChunked(); 
		FLUSH_FIRST_LINE();
	}
	flushPackage(); 
}


Response::~Response() 
{
	if(connection() == nullptr)
		return;
	try {
		if(!chunked()) {
			FLUSH_FIRST_LINE();
			if(status_ != ok && in().rdbuf()->in_avail() == 0)
				out() << stock_replies::status_body(status_);
		} else {
			flushPackage();
			connection()->async_write("0\r\n\r\n");
		}
		flushPackage();
	} catch(std::exception& e) {
		fprintf(stderr, "%s\n", e.what());
	}
}
