#include "package.hh"
#include "connection.hh"
#include <boost/lexical_cast.hpp>

namespace ahttpd {

Package::~Package()
{
}

void
Package::flushPackage()
{
	std::stringstream send_buf;

	if(!send_started_) {		/**< 如果heander被发送，则设置为true */
		if(chunked_) {
			addHeader("Transfer-Encoding", "chunked");
		} else {
			auto h = getHeader("Content-Length");
			if(h == nullptr) {
				addHeader("Content-Length", ::boost::lexical_cast<std::string>(contentLength()));
			}
		}
		for(auto h : getHeaderMap())
			send_buf << h.name << ": " << h.value << "\r\n";
		send_buf << "\r\n";
		send_started_ = true;
	}

	if(body.rdbuf()->in_avail()) {
		if(chunked_) {
			send_buf << std::hex << contentLength() << "\r\n";
			send_buf << body.rdbuf() << "\r\n";
		} else {
			send_buf << body.rdbuf();
		}	
	}

	connection()->asyncWrite(send_buf.str());
}

const char*
Package::connectionType()
{
	return connection_->type();
}
}	/**< namespace ahttpd */
