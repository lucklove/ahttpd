#include "fcgi.hh"
#include "TcpConnection.hh"
#include "request.hh"
#include "response.hh"
#include <string>

#define STACK_BUFF_SIZE (128 * 1024)

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
 * Number of bytes in a FCGI_Header.  Future versions of the protocol
 * will not reduce this number.
 */
#define FCGI_HEADER_LEN  8

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
	if(status != FCGI_REQUEST_COMPLETE)
		res->setStatus(500);
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
		Log("LINE DEBUG") << line;
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
read_record(ConnectionPtr conn, ResponsePtr res)
{
	int need_read = 8 - conn->readBuffer().in_avail();
	need_read = need_read < 0 ? 0 : need_read;
	conn->asyncRead(boost::asio::transfer_at_least(need_read),
		[=](const boost::system::error_code& err, size_t n){
			if(err) {
				Log("DEBUG") << __FILE__ << ":" << __LINE__;
				Log("ERROR") << err;
				res->setStatus(Response::Internal_Server_Error);
				return;					
			}
			std::istream in(&conn->readBuffer());
			in.ignore();			/**< version */
			unsigned char type;
			in >> type;
			in.ignore(2);			/**< requestID */
			unsigned char content_length1;
			unsigned char content_length0;
			unsigned char padding_length;
			in >> content_length1;
			in >> content_length0;
			in >> padding_length;
			in.ignore();			/**< reserved */
			int need_read = (content_length1 << 8) + 
				content_length0 + padding_length - conn->readBuffer().in_avail();
			need_read = need_read < 0 ? 0 : need_read;
			conn->asyncRead(boost::asio::transfer_exactly(need_read),
				[=](const boost::system::error_code &err, size_t n) {
					if(err) {
						Log("ERROR") << err;
						res->setStatus(Response::Internal_Server_Error);
						return;
					}
					std::istream in(&conn->readBuffer());
					if((content_length1 << 8) + content_length0 > STACK_BUFF_SIZE - 1) {
						res->setStatus(500);
						return;
					}
					char buff[STACK_BUFF_SIZE] = { 0 };
					in.read(buff, (content_length1 << 8) + content_length0);
					in.ignore(padding_length);
					switch(type) {
						case FCGI_END_REQUEST:
							end_request(buff[4], res);
							break;
						case FCGI_STDOUT:
							Log("DEBUG") << "FCGI_STDOUT";
							res->out() << buff;
							break;
						case FCGI_STDERR:
							Log("DEBUG") << "FCGI_STDERR";
							Log("WARNING") << buff;
							break;
						default:
							Log("DEBUG") << "UNKONW TYPE";
							res->setStatus(500);
					}
					if(type != FCGI_END_REQUEST)
						read_record(conn, res);
				}		
			);
		}
	);
}
}

void fcgi(boost::asio::io_service& service, const std::string& host, 
	const std::string& port, RequestPtr req, ResponsePtr res)
{
	TcpConnectionPtr conn = std::make_shared<TcpConnection>(service);
	conn->asyncConnect(host, port, [=](ConnectionPtr conn) {
		if(!conn) {
			res->setStatus(Response::Internal_Server_Error);
			return;
		}

		conn->asyncWrite(begin_request());

		conn->asyncWrite(record(FCGI_PARAMS, key_value("REQUEST_METHOD", req->getMethod())));
		conn->asyncWrite(record(FCGI_PARAMS, key_value("SCRIPT_FILENAME", req->getPath())));
		conn->asyncWrite(record(FCGI_PARAMS, key_value("QUERY_STRING", req->getQueryString())));
		conn->asyncWrite(record(FCGI_PARAMS, key_value("SERVER_PROTOCOL", req->getVersion())));
		conn->asyncWrite(record(FCGI_PARAMS, ""));

		conn->asyncWrite(record(FCGI_STDIN, ""));
		
		read_record(conn, res);
	});
}
