#include <boost/test/unit_test.hpp>
#include "server.hh"
#include "client.hh"

struct TestServer : RequestHandler {
	TestServer(const std::string& msg) : msg_(msg) {}
	void handleRequest(RequestPtr req, ResponsePtr res) override {
		BOOST_CHECK(req->connection() == nullptr);
		BOOST_CHECK(res->connection() == nullptr);
		req->setQueryString(msg_);
	}
	std::string msg_;
};

struct ChunkedTestServer : RequestHandler {
	void handleRequest(RequestPtr req, ResponsePtr res) override {
		res->out() << "this ";
		res->flush();
		res->out() << "is ";
		res->flush();
		res->out() << "a ";
		res->flush();
		res->out() << "chunked ";
		res->flush();
		res->out() << "body";
	}
};
	
BOOST_AUTO_TEST_CASE(deliver_test)
{
	std::stringstream config("{}");
	Server server(config);
	auto test = std::make_shared<TestServer>("test");
	auto test_subdir = std::make_shared<TestServer>("test subdir");
	server.addHandler("/test/subdir", test_subdir.get());
	server.addHandler("/test", test.get());
	RequestPtr req = std::make_shared<Request>(nullptr);
	req->setMethod("GET");
	req->setPath("/test");
	server.deliverRequest(req);
	BOOST_CHECK(req->getQueryString() == "test");
	req->setQueryString("");
	req->setPath("/test/subdir");
	server.deliverRequest(req);
	BOOST_CHECK(req->getQueryString() == "test subdir");
	req->setQueryString("");
	req->setPath("/test/sub");
	server.deliverRequest(req);
	BOOST_CHECK(req->getQueryString() == "test");
}

BOOST_AUTO_TEST_CASE(http_test)
{
	std::stringstream config("{\"http port\": \"8888\"}");
	Server server(config);
	server.enqueue([&]{
		std::this_thread::sleep_for(std::chrono::seconds(1));		/**< 等待server开始监听 */
		Client c;
		c.request("GET", "http://localhost:8888/something_not_exist", [&](ResponsePtr res) {
			BOOST_CHECK(res->getStatus() == Response::Not_Found);
			server.stop();
		});
		c.apply();
	});
	server.run();
}

/*
 * FIXME: 在加载密钥文件时需要输入密码，无法在trivas CI上使用
BOOST_AUTO_TEST_CASE(https_test)
{
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
		c.request("GET", "https://localhost:9999/something_not_exist", [&](ResponsePtr res) {
			BOOST_CHECK(res->getStatus() == Response::Not_Found);
			server.stop();
		});
		c.apply();
	});
	server.run();
}
*/

BOOST_AUTO_TEST_CASE(chunked_body_test)
{
	std::stringstream config("{\"http port\": \"8888\"}");
	Server server(config);
	server.addHandler("/chunked", new ChunkedTestServer());
	server.enqueue([&]{
		std::this_thread::sleep_for(std::chrono::seconds(1));		/**< 等待server开始监听 */
		Client c;
		c.request("GET", "http://localhost:8888/chunked", [&](ResponsePtr res) {
			std::stringstream ss;
			ss << res->out().rdbuf();
			BOOST_CHECK(ss.str() == "this is a chunked body");
			server.stop();
		});
		c.apply();
	});
	server.run();
}	
