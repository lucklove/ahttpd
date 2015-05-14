#include "response.hh"
#include "server.hh"
#include "log.hh"
#include "connection.hh"
#include <string>
#include <exception>
#include <cstdio>

#define PARSE_STATUS(ststus)						\
	switch (status) {						\
		case Response::Continue:				\
    			return Continue;				\
		case Response::Switching:				\
    			return Switching;				\
		case Response::Processing:				\
    			return Processing;				\
		case Response::Ok:					\
    			return Ok;					\
  		case Response::Created:					\
    			return Created;					\
  		case Response::Accepted:				\
    			return Accepted;				\
  		case Response::Non_Authoritative_Information:		\
    			return Non_Authoritative_Information;		\
  		case Response::No_Content:				\
    			return No_Content;				\
  		case Response::Reset_Content:				\
    			return Reset_Content;				\
  		case Response::Partial_Content:				\
    			return Partial_Content;				\
  		case Response::Muti_Status:				\
    			return Muti_Status;				\
  		case Response::Multiple_Choices:			\
    			return Multiple_Choices;			\
  		case Response::Moved_Permanently:			\
    			return Moved_Permanently;			\
  		case Response::Moved_Temporarily:			\
    			return Moved_Temporarily;			\
  		case Response::See_Other:				\
    			return See_Other;				\
  		case Response::Not_Modified:				\
    			return Not_Modified;				\
  		case Response::Bad_Request:				\
    			return Bad_Request;				\
  		case Response::Unauthorized:				\
    			return Unauthorized;				\
  		case Response::Forbidden:				\
   	 		return Forbidden;				\
  		case Response::Not_Found:				\
    			return Not_Found;				\
  		case Response::Internal_Server_Error:			\
    			return Internal_Server_Error;			\
  		case Response::Not_Implemented:				\
    			return Not_Implemented;				\
  		case Response::Bad_Gateway:				\
    			return Bad_Gateway;				\
  		case Response::Service_Unavailable:			\
    			return Service_Unavailable;			\
  		default:						\
    			return Internal_Server_Error;			\
  	}

namespace status_strings {

const char Continue[] =
	"HTTP/1.1 100 Continue";
const char Switching[] =
	"HTTP/1.1 101 Continue";
const char Processing[] =
	"HTTP/1.1 102 Continue";
const char Ok[] =
 	"HTTP/1.1 200 OK";
const char Created[] =
	"HTTP/1.1 201 Created";
const char Accepted[] =
	"HTTP/1.1 202 Accepted";
const char Non_Authoritative_Information[] =
	"HTTP/1.1 203 Non-Authoritative Information";
const char No_Content[] =
	"HTTP/1.1 204 No Content";
const char Reset_Content[] =
	"HTTP/1.1 205 Reset Content";
const char Partial_Content[] =
	"HTTP/1.1 206 Partial Content";
const char Muti_Status[] =
	"HTTP/1.1 207 Muti Status";
const char Multiple_Choices[] =
	"HTTP/1.1 300 Multiple Choices";
const char Moved_Permanently[] =
	"HTTP/1.1 301 Moved Permanently";
const char Moved_Temporarily[] =
	"HTTP/1.1 302 Moved Temporarily";
const char See_Other[] =
	"HTTP/1.1 303 See Other";
const char Not_Modified[] =
	"HTTP/1.1 304 Not Modified";
const char Bad_Request[] =
	"HTTP/1.1 400 Bad Request";
const char Unauthorized[] =
	"HTTP/1.1 401 Unauthorized";
const char  Payment_Required[] =
	"HTTP/1.1 402 Payment Required";
const char Forbidden[] =
	"HTTP/1.1 403 Forbidden";
const char Not_Found[] =
	"HTTP/1.1 404 Not Found";
const char Internal_Server_Error[] =
	"HTTP/1.1 500 Internal Server Error";
const char Not_Implemented[] =
	"HTTP/1.1 501 Not Implemented";
const char Bad_Gateway[] =
	"HTTP/1.1 502 Bad Gateway";
const char Service_Unavailable[] =
	"HTTP/1.1 503 Service Unavailable";

std::string
status_head(Response::status_t status)
{
	PARSE_STATUS(status);
}

} // namespace status_strings

namespace stock_replies {

const char Continue[] =
	"<html>"
	"<head><title>Continue</title></head>"
	"<body><h1>100 Continue</h1></body>"
	"</html>";
const char Switching[] =
	"<html>"
	"<head><title>Switching</title></head>"
	"<body><h1>101 Switching</h1></body>"
	"</html>";
const char Processing[] =
	"<html>"
	"<head><title>Processing</title></head>"
	"<body><h1>102 Processing</h1></body>"
	"</html>";
const char Ok[] = "";
const char Created[] =
	"<html>"
	"<head><title>Created</title></head>"
	"<body><h1>201 Created</h1></body>"
	"</html>";
const char Accepted[] =
	"<html>"
	"<head><title>Accepted</title></head>"
	"<body><h1>202 Accepted</h1></body>"
	"</html>";
const char Non_Authoritative_Information[] =
	"<html>"
	"<head><title>Non-Authoritative Information</title></head>"
	"<body><h1>203 Non-Authoritative Information</h1></body>"
	"</html>";
const char No_Content[] =
	"<html>"
	"<head><title>No Content</title></head>"
	"<body><h1>204 Content</h1></body>"
	"</html>";
const char Reset_Content[] =
	"<html>"
	"<head><title>Reset Content</title></head>"
	"<body><h1>205 Reset Content</h1></body>"
	"</html>";
const char Partial_Content[] =
	"<html>"
	"<head><title>Partial Content</title></head>"
	"<body><h1>206 Partial Content</h1></body>"
	"</html>";
const char Muti_Status[] =
	"<html>"
	"<head><title>Muti Status</title></head>"
	"<body><h1>207 Muti Status</h1></body>"
	"</html>";
const char Multiple_Choices[] =
	"<html>"
	"<head><title>Multiple Choices</title></head>"
	"<body><h1>300 Multiple Choices</h1></body>"
	"</html>";
const char Moved_Permanently[] =
	"<html>"
	"<head><title>Moved Permanently</title></head>"
	"<body><h1>301 Moved Permanently</h1></body>"
	"</html>";
const char Moved_Temporarily[] =
	"<html>"
	"<head><title>Moved Temporarily</title></head>"
	"<body><h1>302 Moved Temporarily</h1></body>"
	"</html>";
const char See_Other[] =
	"<html>"
	"<head><title>See Other</title></head>"
	"<body><h1>303 See Other</h1></body>"
	"</html>";
const char Not_Modified[] =
	"<html>"
	"<head><title>Not Modified</title></head>"
	"<body><h1>304 Not Modified</h1></body>"
	"</html>";
const char Bad_Request[] =
	"<html>"
	"<head><title>Bad Request</title></head>"
	"<body><h1>400 Bad Request</h1></body>"
	"</html>";
const char Unauthorized[] =
	"<html>"
	"<head><title>Unauthorized</title></head>"
	"<body><h1>401 Unauthorized</h1></body>"
	"</html>";
const char Payment_Required[] =
	"<html>"
	"<head><title>Payment Required</title></head>"
	"<body><h1>402 Payment Required</h1></body>"
	"</html>";
const char Forbidden[] =
	"<html>"
	"<head><title>Forbidden</title></head>"
	"<body><h1>403 Forbidden</h1></body>"
	"</html>";
const char Not_Found[] =
	"<html>"
	"<head><title>Not Found</title></head>"
	"<body><h1>404 Not Found</h1></body>"
	"</html>";
const char Internal_Server_Error[] =
	"<html>"
	"<head><title>Internal Server Error</title></head>"
	"<body><h1>500 Internal Server Error</h1></body>"
	"</html>";
const char Not_Implemented[] =
	"<html>"
	"<head><title>Not Implemented</title></head>"
	"<body><h1>501 Not Implemented</h1></body>"
	"</html>";
const char Bad_Gateway[] =
	"<html>"
	"<head><title>Bad Gateway</title></head>"
	"<body><h1>502 Bad Gateway</h1></body>"
	"</html>";
const char Service_Unavailable[] =
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
			if(status_ != Ok && in().rdbuf()->in_avail() == 0)
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
