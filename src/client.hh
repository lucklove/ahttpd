#pragma once

#include <string>
#include <functional>
#include <mutex>
#include "ptrs.hh"
#include "log.hh"
#include "cookie.hh"

namespace ahttpd {

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
	void enableCookie() {
		enable_cookie_ = true;
	}
	void disableCookie() {
		cookie_jar_.clear();
		enable_cookie_ = false;
	}
	void clearCookie() {
		cookie_jar_.clear();
	}
private:
	void add_cookie_to_request(RequestPtr req, const std::string& scheme, const std::string& host);
	void add_cookie_to_cookie_jar(ResponsePtr res, const std::string& host);
	boost::asio::io_service& service_;
	std::shared_ptr<boost::asio::io_service> service_holder_;
	boost::asio::ssl::context* ssl_context_;
	std::vector<response_cookie_t> cookie_jar_;
	bool enable_cookie_{};
	std::mutex cookie_mutex_{};
};

}	/**< namespace ahttpd */
