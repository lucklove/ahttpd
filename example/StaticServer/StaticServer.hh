#pragma once

#include "ahttpd.hh"

class StaticServer : public RequestHandler {
public:
	StaticServer(Server *s, const std::string& root) : server_(s), doc_root(root) {
		if(doc_root[doc_root.size()-1] == '/')
			doc_root.resize(doc_root.size() - 1);
	}
        void handleRequest(RequestPtr req, ResponsePtr res) override;
private:
	Server *server_;
        std::string doc_root;
};
