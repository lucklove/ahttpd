#pragma once

#include "server.hh"

class StaticServer : public RequestHandler {
public:
        using RequestHandler::RequestHandler;
        void handleRequest(RequestPtr req, ResponsePtr res) override;
private:
        const std::string doc_root = ".";
};
