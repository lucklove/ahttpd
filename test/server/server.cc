#include <boost/test/unit_test.hpp>
#include "server.hh"
#include "client.hh"

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

BOOST_AUTO_TEST_CASE(http_test)
{
	std::stringstream config("{\"http port\": \"8888\"}");
	Server server(config);
	server.enqueue([&]{
		std::this_thread::sleep_for(std::chrono::seconds(1));		/**< 等待server开始监听 */
		Client c;
		c.request("GET", "http://localhost:8888/something_not_exist", [&](auto res) {
			BOOST_CHECK(res->status() == Response::not_found);
			server.stop();
		});
		c.apply();
	});
	server.run();
}

BOOST_AUTO_TEST_CASE(https_test)
{
/*
	std::stringstream config("{"
		"\"https port\":\"9999\","
		"\"verify file\":\"certificate/server.csr\","
		"\"certificate chain file\":\"certificate/server.crt\","
		"\"private key\":\"certificate/server.key\","
		"\"tmp dh file\":\"certificate/server.dh\""
	"}");
	Server server(config);
	server.enqueue([&]{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		Client c;
		c.request("GET", "https://localhost:9999/something_not_exist", [&](auto res) {
			BOOST_CHECK(res->status() == Response::not_found);
			server.stop();
		});
		c.apply();
	});
	server.run();
*/
}
