#pragma once

#include <string>
#include <functional>
#include "ptrs.hh"
#include "log.hh"

namespace boost { namespace asio { namespace ssl {
class context;
}
class io_service;
}
}

class Client {
public:
	Client(boost::asio::io_service& io_service);
	Client();
	~Client();

	void request(const std::string& method, const std::string& url,
		std::function<void(ResponsePtr)> res_handler,
		std::function<void(RequestPtr)> req_handler =
			[](RequestPtr req) {}
	);

	void apply();
private:
	boost::asio::io_service& service_;
	std::shared_ptr<boost::asio::io_service> service_holder_;
	boost::asio::ssl::context* ssl_context_;
};
