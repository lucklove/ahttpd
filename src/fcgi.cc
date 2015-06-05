#include "fcgi.hh"
#include "TcpConnection.hh"
#include "request.hh"
#include "response.hh"
#include <string>
#include <boost/lexical_cast.hpp>

#define STACK_BUFF_SIZE (128 * 1024)

namespace ahttpd {

struct FCGI_Header {
	unsigned char version;
	unsigned char type;
	unsigned char requestIdB1;
	unsigned char requestIdB0;
	unsigned char contentLengthB1;
	unsigned char contentLengthB0;
	unsigned char paddingLength;
	unsigned char reserved;
};

std::string
key_value(const std::string& name, const std::string& value)
{
	std::string ret;
	if(name.size() > 0xFF) {
		ret.push_back(((name.size() >> 24) & 0xFF) | 0x80); 
		ret.push_back((name.size() >> 16) & 0xFF); 
		ret.push_back((name.size() >> 8) & 0xFF); 
		ret.push_back(name.size() & 0xFF); 
	} else {
		ret.push_back(name.size()); 
	}
	if(value.size() > 0xFF) {
		ret.push_back(((value.size() >> 24) & 0xFF) | 0x80); 
		ret.push_back((value.size() >> 16) & 0xFF); 
		ret.push_back((value.size() >> 8) & 0xFF); 
		ret.push_back(value.size() & 0xFF); 
	} else {
		ret.push_back(value.size()); 
	}
	return ret + name + value;
}

/**
 * Value for version component of FCGI_Header
 */
#define FCGI_VERSION_1           1

/**
 * Values for type component of FCGI_Header
 */
#define FCGI_BEGIN_REQUEST       1
#define FCGI_ABORT_REQUEST       2
#define FCGI_END_REQUEST         3
#define FCGI_PARAMS              4
#define FCGI_STDIN               5
#define FCGI_STDOUT              6
#define FCGI_STDERR              7
#define FCGI_DATA                8
#define FCGI_GET_VALUES          9
#define FCGI_GET_VALUES_RESULT  10
#define FCGI_UNKNOWN_TYPE       11
#define FCGI_MAXTYPE (FCGI_UNKNOWN_TYPE)

/**
 * Values for role component of FCGI_BeginRequestBody
 */
#define FCGI_RESPONDER  1

/**
 * Values for protocolStatus component of FCGI_EndRequestBody
 */
#define FCGI_REQUEST_COMPLETE 0
#define FCGI_CANT_MPX_CONN    1
#define FCGI_OVERLOADED       2
#define FCGI_UNKNOWN_ROLE     3


/**
 * Value for requestId component of FCGI_Header
 */
#define FCGI_NULL_REQUEST_ID     0

namespace {
std::string
begin_request()
{
	std::string ret;
	ret.reserve(16);
	ret.push_back(FCGI_VERSION_1);
	ret.push_back(FCGI_BEGIN_REQUEST);

	/**
 	 * \note 不使用requestID, 因为我们不复用连接.
 	 */ 
	ret.push_back(0);
	ret.push_back(FCGI_NULL_REQUEST_ID);

	/** content length: 即FCGI_BeginRequestBody */
	ret.push_back(0);
	ret.push_back(8);

	ret.push_back(0);				/**< padding length */
	ret.push_back(0);				/**< reserved */
		
	/** FCGI_BeginRequestBody */

	/**
 	 * \note ROLE固定为RESPONDER
 	 */
	ret.push_back(0);
	ret.push_back(FCGI_RESPONDER);

	ret.push_back(0);				/**< flags */

	/** reserved */
	ret.push_back(0);
	ret.push_back(0);
	ret.push_back(0);
	ret.push_back(0);
	ret.push_back(0);
		
	return ret;
};

std::string
record(unsigned char type, const std::string& content)
{
	std::string ret;
	ret.push_back(FCGI_VERSION_1);
	ret.push_back(type);

	/**
	 * \note 不使用requestID, 因为我们不复用连接.
 	 */ 
	ret.push_back(0);
	ret.push_back(FCGI_NULL_REQUEST_ID);

	ret.push_back((content.size() >> 8) & 0xFF);
	ret.push_back(content.size() & 0xFF);
	ret.push_back(0);				/**< padding length */
	ret.push_back(0);				/**< reserved */
	return ret + content;
}

void
trim_line(std::string& line)
{
	if(line[line.size() -1] == '\r')
		line.resize(line.size() - 1);
}

void
end_request(unsigned char status, ResponsePtr res)
{
	if(status != FCGI_REQUEST_COMPLETE) {
		Log("DEBUG") << "UNCOMPLETE FCGI REQUEST";
		res->setStatus(500);
	}
	switch(status) {
		case FCGI_CANT_MPX_CONN:
			Log("ERROR") << "FCGI_CANT_MPX_CONN";
			return;
		case FCGI_OVERLOADED:
			Log("ERROR") << "FCGI_OVERLOADED";
			return;
		case FCGI_UNKNOWN_ROLE:
			Log("ERROR") << "FCGI_OVERLOADED";
			return;
	}
	std::string line;
	while(true) {
		if(!getline(res->in(), line) || line == "\r" || line == "")
			break;
		trim_line(line);
		auto pos = line.find(':');
		if(pos == line.npos)
			continue;
		std::string key = line.substr(0, pos);
		++pos;
		while(line[pos] == ' ' && pos < line.size())
			++pos;
		std::string value = line.substr(pos, line.size());
		res->addHeader(key, value);				
	}
	std::string* state = res->getHeader("Status");
	if(state) {
		std::stringstream ss(*state);
		short int s;
		ss >> s;
		res->setStatus(s);
	}
}

void
read_record(ConnectionPtr conn, ResponsePtr res, std::function<void(void)> finish_handler)
{
	int need_read = sizeof(FCGI_Header) - conn->readBuffer().in_avail();
	need_read = need_read < 0 ? 0 : need_read;
	conn->asyncRead(boost::asio::transfer_at_least(need_read),
		[=](const boost::system::error_code& err, size_t n){
			if(err) {
				Log("DEBUG") << __FILE__ << ":" << __LINE__;
				Log("ERROR") << err;
				res->setStatus(Response::Internal_Server_Error);
				return;					
			}

			FCGI_Header header;
			std::istream in(&conn->readBuffer());

			in.read(reinterpret_cast<char*>(&header), sizeof(header));

			int need_read = (header.contentLengthB1 << 8) + 
				header.contentLengthB0 + header.paddingLength - 
				conn->readBuffer().in_avail();

			need_read = need_read < 0 ? 0 : need_read;

			conn->asyncRead(boost::asio::transfer_exactly(need_read),
				[=](const boost::system::error_code &err, size_t n) {
					if(err) {
						Log("DEBUG") << __FILE__ << ":" << __LINE__;
						Log("ERROR") << err;
						res->setStatus(Response::Internal_Server_Error);
						return;
					}
					std::istream in(&conn->readBuffer());
					if((header.contentLengthB1 << 8) + header.contentLengthB0 > STACK_BUFF_SIZE - 1) {
						Log("DEBUG") << "BUFFER OVERFLOW";
						res->setStatus(500);
						return;
					}
					char buff[STACK_BUFF_SIZE] = { 0 };
					in.read(buff, (header.contentLengthB1 << 8) + header.contentLengthB0);
					in.ignore(header.paddingLength);
					switch(header.type) {
						case FCGI_END_REQUEST:
							end_request(buff[4], res);
							break;
						case FCGI_STDOUT:
							res->out().write(buff, 
								(header.contentLengthB1 << 8) + 
								header.contentLengthB0);
							break;
						case FCGI_STDERR:
							Log("WARNING") << buff;
							break;
						default:
							Log("DEBUG") << "unknow type";
							res->setStatus(500);
					}
					if(header.type != FCGI_END_REQUEST) {
						read_record(conn, res, finish_handler);
					} else {
						finish_handler();
					}
				}		
			);
		}
	);
}

void
add_param(ConnectionPtr conn, const std::string& key, const std::string& val)
{
	conn->asyncWrite(record(FCGI_PARAMS, key_value(key, val)));
}

void
add_param_if(RequestPtr req, ConnectionPtr conn, const std::string& header_name, const std::string& name)
{
	std::string *p = req->getHeader(header_name);
	if(p)
		conn->asyncWrite(record(FCGI_PARAMS, key_value(name, *p)));
}
}

void fcgi(boost::asio::io_service& service, const std::string& host, 
	const std::string& port, std::string doc_root, RequestPtr req, 
	ResponsePtr res, std::function<void(void)> handler)
{
	if(!doc_root.size()) {
		Log("DEBUG") << "doc_root can not be empty";
		res->setStatus(500);
		return;
	}

	if(doc_root[doc_root.size()-1] == '/')
		doc_root.resize(doc_root.size() - 1);

	TcpConnectionPtr conn = std::make_shared<TcpConnection>(service);
	conn->asyncConnect(host, port, [=](ConnectionPtr conn) {
		if(!conn) {
			res->setStatus(Response::Internal_Server_Error);
			return;
		}
		conn->asyncWrite(begin_request());
		add_param(conn, "DOCUMENT_ROOT", doc_root);
		add_param(conn, "REQUEST_METHOD", req->getMethod());
		add_param(conn, "SCRIPT_FILENAME", doc_root + req->getPath());
		add_param(conn, "SCRIPT_NAME", req->getPath());
		add_param(conn, "QUERY_STRING", req->getQueryString());
		add_param(conn, "SERVER_PROTOCOL", req->getVersion());
		add_param(conn, "GATEWAY_INTERFACE", "CGI/1.1");
		add_param(conn, "SERVER_SOFTWARE", "ahttpd");
		add_param(conn, "REQUEST_URI", req->getPath() + 
			(req->getQueryString() == "" ? "" : "?" + req->getQueryString()));
		add_param(conn, "CONTENT_LENGTH", boost::lexical_cast<std::string>(req->contentLength()));
		add_param(conn, "HTTP_CONTENT_LENGTH", boost::lexical_cast<std::string>(req->contentLength()));
		add_param_if(req, conn, "Host", "HTTP_HOST");
		add_param_if(req, conn, "Cookie", "HTTP_COOKIE");
		add_param_if(req, conn, "Content-Type", "CONTENT_TYPE");
		add_param_if(req, conn, "Content-Type", "HTTP_CONTENT_TYPE");
		add_param_if(req, conn, "Accept", "HTTP_ACCEPT");
		add_param_if(req, conn, "Connection", "HTTP_CONNECTION");
		conn->asyncWrite(record(FCGI_PARAMS, ""));

		if(req->in().rdbuf()->in_avail()) {
			std::stringstream ss;
			ss << req->in().rdbuf();
			conn->asyncWrite(record(FCGI_STDIN, ss.str()));
		}
		conn->asyncWrite(record(FCGI_STDIN, ""));
		
		read_record(conn, res, handler);
	});
}

}	/**< namespace ahttpd */
