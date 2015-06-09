#pragma once

#include <ahttpd.hh>

class WebServer : public ahttpd::RequestHandler {
public:
	WebServer(ahttpd::Server *s, const std::string& root) : server_(s), doc_root(root) {
		if(doc_root[doc_root.size()-1] == '/')
			doc_root.resize(doc_root.size() - 1);
	}
        void handleRequest(ahttpd::RequestPtr req, ahttpd::ResponsePtr res) override;
private:
	ahttpd::Server *server_;
        std::string doc_root;
};
