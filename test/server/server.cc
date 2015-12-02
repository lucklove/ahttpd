#include "UnitTest.hh"
#include "server.hh"
#include "client.hh"

using namespace ahttpd;

struct TestServer : RequestHandler {
    TestServer(const std::string& msg) : msg_(msg) {}
    void handleRequest(RequestPtr req, ResponsePtr res) override {
        TEST_CHECK(req->connection() == nullptr);
        TEST_CHECK(res->connection() == nullptr);
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

TEST_CASE(deliver_test)
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
    TEST_CHECK(req->getQueryString() == "test");
    req->setQueryString("");
    req->setPath("/test/subdir");
    server.deliverRequest(req);
    TEST_CHECK(req->getQueryString() == "test subdir");
    req->setQueryString("");
    req->setPath("/test/sub");
    server.deliverRequest(req);
    TEST_CHECK(req->getQueryString() == "test");
}

TEST_CASE(http_test)
{
/*
    std::stringstream config("{\"http port\": \"8888\"}");
    Server server(config);
    Client c{server.service()};
    c.request("GET", "http://localhost:8888/something_not_exist", [&](ResponsePtr res) {
        TEST_CHECK(res->getStatus() == Response::Not_Found);
        server.stop();
    });
    server.run();
*/
}
/*
TEST_CASE(https_test)
{
    std::stringstream config("{"
        "\"https port\":\"9999\","
        "\"verify file\":\"certificate/server.csr\","
        "\"certificate chain file\":\"certificate/server.crt\","
        "\"private key\":\"certificate/server.key\","
        "\"tmp dh file\":\"certificate/server.dh\""
    "}");
    Server server(config);
    Client c{server.service()};
    c.request("GET", "https://localhost:9999/something_not_exist", [&](ResponsePtr res) {
        TEST_REQUIRE(res);
        TEST_CHECK(res->getStatus() == Response::Not_Found);
        server.stop();
    });
    server.run();
}
*/
TEST_CASE(chunked_body_test)
{
Log("DEBUG") << __LINE__;
    std::stringstream config("{\"http port\": \"8888\"}");
Log("DEBUG") << __LINE__;
    Server server(config);
Log("DEBUG") << __LINE__;
    auto chunked_test = std::make_shared<ChunkedTestServer>();
Log("DEBUG") << __LINE__;
    server.addHandler("/chunked", chunked_test.get());
Log("DEBUG") << __LINE__;
    std::this_thread::sleep_for(std::chrono::seconds(1));        /**< 等待server开始监听 */
Log("DEBUG") << __LINE__;
    Client c{server.service()};
Log("DEBUG") << __LINE__;
    c.request("GET", "http://localhost:8888/chunked", [&](ResponsePtr res) {
Log("DEBUG") << __LINE__;
        TEST_REQUIRE(res);
        std::stringstream ss;
        ss << res->out().rdbuf();
        TEST_CHECK(ss.str() == "this is a chunked body");
        server.stop();
    });
Log("DEBUG") << __LINE__;
    server.run();
Log("DEBUG") << __LINE__;
}    
