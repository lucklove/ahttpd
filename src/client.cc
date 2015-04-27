#include "client.hh"
#include "parser.hh"
#include "response.hh"
#include "request.hh"
#include "SslConnection.hh"
#include "TcpConnection.hh"
#include <regex>
#include <boost/asio/ssl.hpp>

Client::Client(boost::asio::io_service& service)
 	:service_(service) 
{
	ssl_context_ = new boost::asio::ssl::context(boost::asio::ssl::context::sslv23);
	ssl_context_->set_default_verify_paths();
}
	
Client::Client()
 	: Client(*(new boost::asio::io_service()))
{
	service_holder_.reset(&service_);
}

Client::~Client() { delete ssl_context_; }

void
Client::apply()
{
	service_.run();
}

void
Client::request(const std::string& method, const std::string& url,
	std::function<void(ResponsePtr)> res_handler,
	std::function<void(RequestPtr)> req_handler)
{
	static const std::regex url_reg("((http|https)(://))?((((?!@)[[:print:]])*)@)?"
		"(((?![/\\?])[[:print:]])*)([[:print:]]+)?");	/**< http://user:pass@server:port/path?query */
	std::smatch results;
	if(std::regex_search(url, results, url_reg)) {
		std::string scheme = "http";
		if(results[2].matched) 
			scheme = results.str(2);	

		std::string auth{};
		if(results[5].matched)
			auth = results.str(5);

		std::string host = results.str(7);

		std::string path = "/";
		if(results[9].matched)
			path = results.str(9);
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

		connection->async_connect(host, port, [=](ConnectionPtr conn) {
			if(conn) {
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
				if(auth != "")
					req->basicAuth(auth);

				req_handler(req);
				parseResponse(res, [=](ResponsePtr response) {
					res->discardConnection();
					if(response) {
						res_handler(res);
					} else {
						res_handler(nullptr);
					}
				});
			} else {
				req_handler(nullptr);
				res_handler(nullptr);
			}
		});
	} else {
		res_handler(nullptr);
	}
}
