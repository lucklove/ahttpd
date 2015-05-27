#include <boost/test/unit_test.hpp>
#include "server.hh"
#include "fcgi.hh"

struct TestServer : RequestHandler {
	TestServer(Server *s) : server_(s) {}
	void handleRequest(RequestPtr req, ResponsePtr res) override {
		fcgi(server_->service(), "127.0.0.1", "9000", req, res);
	}
	Server *server_;
};

BOOST_AUTO_TEST_CASE(fcgi_test)
{
	std::stringstream config("{\"http port\": \"8888\"}");
	Server server(config);
	auto test = std::make_shared<TestServer>(&server);
	server.addHandler("/", test.get());
	server.run();
}	
