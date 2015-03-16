#pragma once

#include <string>
#include <memory>
#include <vector>

#include "request.hh"
#include "response.hh"
#include "log.hh"

class Server;
class RequestHandler;

using RequestHandlerPtr = std::shared_ptr<RequestHandler>;

class RequestHandler {
public:
	RequestHandler(const RequestHandler&) = delete;
	RequestHandler& operator=(const RequestHandler&) = delete;

	explicit RequestHandler(Server *server);
	virtual void handleRequest(RequestPtr req, ResponsePtr rep);
	bool deliverRequest(RequestPtr req, ResponsePtr rep);
	void addSubHandler(const std::string& path, RequestHandlerPtr handler) {
		sub_handlers_.push_back(std::make_tuple(path, handler));
	}

	Server* server() { return server_; }
private:
	Server *server_;

	std::vector<std::tuple<const std::string, RequestHandlerPtr>> sub_handlers_;

	static bool url_decode(const std::string& in, std::string& out);
};
