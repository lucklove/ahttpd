#include <boost/test/unit_test.hpp>
#include "server.hh"

struct TestServer : RequestHandler {
	void handleRequest(RequestPtr req, ResponsePtr res) override {
		BOOST_CHECK(req->connection() == nullptr);
		BOOST_CHECK(res->connection() == nullptr);
		req->query() = "ok";
	}
};

BOOST_AUTO_TEST_CASE(deliver_test)
{
	std::stringstream config("{\"http port\": \"8888\"}");
	Server server(config);
	server.addHandler("/test", new TestServer());
	RequestPtr req = std::make_shared<Request>(nullptr);
	req->path() = "/test";
	server.deliverRequest(req);
	BOOST_CHECK(req->query() == "ok");
	req->query() = "";
	req->path() = "/test/subdir";
	server.deliverRequest(req);
	BOOST_CHECK(req->query() == "ok");
}
