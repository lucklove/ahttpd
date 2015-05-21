#pragma once

#include "ahttpd.hh"

class StaticServer : public RequestHandler {
public:
	StaticServer() = default;
	StaticServer(const std::string& root) : doc_root(root) {}
        void handleRequest(RequestPtr req, ResponsePtr res) override;
private:
        const std::string doc_root = ".";
};
