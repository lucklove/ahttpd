#pragma once

#include <string>
#include <asio.hpp>
#include <functional>
#include "ptrs.hh"
#include "log.hh"

namespace asio {
namespace ssl {
class context;
}
}

class Client {
public:
	Client(asio::io_service& io_service);

	~Client();

	void request(const std::string& method, const std::string& url,
		std::function<void(ResponsePtr, bool)> res_handler,
		std::function<void(RequestPtr, bool)> req_handler =
			[](RequestPtr req, bool good) {}
	);
private:
	asio::io_service& service_;
	asio::ssl::context* ssl_context_;
};
	
