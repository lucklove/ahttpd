#include "package.hh"
#include "connection.hh"

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
Package::flushPackage()
{
	std::stringstream send_buf;

	if(!send_started_) {		/**< 如果heander被发送，则设置为true */
		if(chunked_) {
			addHeader("Transfer-Encoding", "chunked");
		} else {
			auto h = getHeader("Content-Length");
			if(h == nullptr) {
				addHeader("Content-Length", to_string(contentLength()));
			}
		}
//		add_cookie();		/**< 将cookie写入header */	
		for(auto&& h : headerMap())
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
	connection()->async_write(send_buf.str());
}
