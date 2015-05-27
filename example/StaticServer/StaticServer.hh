#pragma once

#include "ahttpd.hh"

class StaticServer : public RequestHandler {
public:
	StaticServer(Server *s) : server_(s) {}
	StaticServer(Server *s, const std::string& root) : server_(s), doc_root(root) {}
        void handleRequest(RequestPtr req, ResponsePtr res) override;
private:
	Server *server_;
        const std::string doc_root = ".";
};
