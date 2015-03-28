#include <iostream>
#include <string>
#include "server.hh"

struct TestHandler : public RequestHandler {
	using RequestHandler::RequestHandler;

	void handleRequest(RequestPtr req, ResponsePtr rep) override {
		rep->out() << "it's TestHander::handleRequest" << std::endl;
		rep->out() << "method: " << req->method() << std::endl;
		rep->out() << "path: " << req->path() << std::endl;
		rep->out() << "query: " << req->query() << std::endl;
		rep->out() << "version: " << req->version() << std::endl;
		for(auto&& h : req->headerMap())
			rep->out() << h.name << ": " << h.value << std::endl;

		if(req->in().rdbuf()->in_avail())	/**< 判断是否有数据，[重要]*/
			rep->out() << req->in().rdbuf();
	}
};

int 
main(int argc, char* argv[])
{
	try {
		asio::io_service io_service;
		Server server(io_service, "8888", "9999");
		server.addHandler("/test", new TestHandler(&server));
		server.run(10);		/**< 给io_service 10个线程 */
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
