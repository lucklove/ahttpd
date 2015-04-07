#include "request.hh"
#include "connection.hh"

void
Request::flush()
{

	if(connection() == nullptr)
		return;
	if(!chunked()) {        /**< 说明是第一次手动调用flush */
		/** 由于是第一次调用，说明第一行并未发送 */
		setChunked();
		if(query_ == "") {
			Log("NOTE") << method_ << " " << path_ << " " << version_;
			connection()->async_write(method_ + " " + path_ + " " + version_ + "\r\n");
		} else {
			Log("NOTE") << method_ << " " << path_ << "?" << query_ << " " << version_;
			connection()->async_write(method_ + " " + path_ + "?" + query_ + " " + version_ + "\r\n");
		}
	}
	flushPackage();
}

Request::~Request()
{

	if(connection() == nullptr)
		return;
	try {
		if(!chunked()) {
			if(query_ == "") {
				Log("NOTE") << method_ << " " << path_ << " " << version_;
				connection()->async_write(method_ + " " + path_ + " " + version_ + "\r\n");
			} else {
				Log("NOTE") << method_ << " " << path_ << "?" << query_ << " " << version_;
				connection()->async_write(method_ + " " + path_ + "?" + query_ + " " + version_ + "\r\n");
			}
		} else {
			flushPackage();
			connection()->async_write("0\r\n\r\n");
		}
		flushPackage();
	} catch(std::exception& e) {
		fprintf(stderr, "%s\n", e.what());
	}
}
		
