#pragma once

#include <string>
#include <functional>
#include "ptrs.hh"
#include "log.hh"

namespace asio { namespace ssl {
class context;
}
class io_service;
}

class Client {
public:
	Client(asio::io_service& io_service);
	Client();

	~Client();

	void request(const std::string& method, const std::string& url,
		std::function<void(ResponsePtr, bool)> res_handler,
		std::function<void(RequestPtr, bool)> req_handler =
			[](RequestPtr req, bool good) {}
	);

	void run();
private:
	asio::io_service& service_;
	std::shared_ptr<asio::io_service> service_holder_;
	asio::ssl::context* ssl_context_;
};
	
