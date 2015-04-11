#pragma once

#include "server.hh"

class StaticServer : public RequestHandler {
public:
	using RequestHandler::RequestHandler;
	StaticServer(Server* server, const std::string& root)
		: RequestHandler(server), doc_root(root) {}
        void handleRequest(RequestPtr req, ResponsePtr res) override;
private:
        const std::string doc_root = ".";
};
