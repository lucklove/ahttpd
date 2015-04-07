#include "client.hh"
#include "log.hh"
#include "parser.hh"
#include "response.hh"
#include "request.hh"
#include "SslConnection.hh"
#include "TcpConnection.hh"
#include <regex>
#include <asio/ssl.hpp>

Client::Client(asio::io_service& service)
 	:service_(service) 
{
	ssl_context_ = new asio::ssl::context(asio::ssl::context::sslv23);
	ssl_context_->set_default_verify_paths();
}
	
Client::~Client() { delete ssl_context_; }

void
Client::request(const std::string& method, const std::string& url,
	std::function<void(ResponsePtr, bool)> res_handler,
	std::function<void(RequestPtr, bool)> req_handler)
{
	std::string scheme = "http";
	std::string host;
	std::string path = "/";
	static const std::regex url_reg("((http|https)(://))?(((?![/\\?])[[:print:]])*)([[:print:]]+)?");
	std::smatch results;
	if(std::regex_search(url, results, url_reg)) {
		if(results[2].matched) 
			scheme = results.str(2);
		host = results.str(4);
		if(results[6].matched)
			path = results.str(6);
		if(path[0] != '/')
			path = "/" + path;
		std::string port = scheme;
		static const std::regex host_port_reg("(((?!:)[[:print:]])*)(:([0-9]+))?");
		if(std::regex_search(host, results, host_port_reg)) {
			host = results.str(1);
			if(results[4].matched)
				port = results.str(4);
		}
		ConnectionPtr connection;
		if(scheme == "http") {
			connection = std::make_shared<TcpConnection>(service_);
		} else if(scheme == "https") {
			connection = std::make_shared<SslConnection>(service_, *ssl_context_);
		}

		connection->async_connect(host, port, [=](ConnectionPtr conn, bool good) {
			if(good) {
				auto req = std::make_shared<Request>(conn);
				auto res = std::make_shared<Response>(conn);
				req->method() = method;
				auto pos = path.find("?");
				if(pos == path.npos) {
					req->path() = path;
				} else {
					req->path() = path.substr(0, pos); 
					req->query() = path.substr(pos + 1, path.size());
				}
				req->version() = "HTTP/1.1";
				req->addHeader("Host", host);
				req->addHeader("Connection", "close");
				req_handler(req, true);
				parseResponse(res, [=](ResponsePtr res, bool good) {
					res->discardConnection();
					if(good) {
						res_handler(res, true);
					} else {
						Log(__FILE__) << __LINE__;
						res_handler(res, false);
					}
				});
			} else {
				req_handler(nullptr, false);
				res_handler(nullptr, false);
			}
		});
	} else {
		res_handler(nullptr, false);
	}
}
