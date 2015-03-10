// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hh"

struct TestHandler : public RequestHandler {
	using RequestHandler::RequestHandler;

	void handleRequest(RequestPtr req, ResponsePtr rep) override {
		std::cout << "it's TestHander::handleRequest" << std::endl;
		std::cout << "method: " << req->getMethod() << std::endl;
		std::cout << "uri: " << req->getUri() << std::endl;
		std::cout << "version: " << req->getVersion() << std::endl;
		for(auto&& h : req->headerMap())
			std::cout << h.name << ": " << h.value << std::endl;
		std::string body;
		std::cout << req->in().rdbuf();
		std::cout << std::endl;
	}
};

int 
main(int argc, char* argv[])
{
	try {
		boost::asio::io_service io_service;
		Server server(io_service, "8888");
		server.addHandler("/test", RequestHandlerPtr(new TestHandler(&server)));
		server.run();
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
