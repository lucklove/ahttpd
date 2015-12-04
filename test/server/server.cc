#include "UnitTest.hh"
#include "server.hh"
#include "client.hh"

using namespace ahttpd;

struct TestServer : RequestHandler 
{
    TestServer(const std::string& msg) : msg_(msg) {}
    void handleRequest(RequestPtr req, ResponsePtr res) override 
    {
        TEST_CHECK(req->connection() == nullptr);
        TEST_CHECK(res->connection() == nullptr);
        req->setQueryString(msg_);
    }
    std::string msg_;
};

struct ChunkedTestServer : RequestHandler 
{
    void handleRequest(RequestPtr req, ResponsePtr res) override 
    {
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
    std::stringstream config("{\"http port\": \"8888\"}");
    Server server(config);
    Client c{server.service()};
    c.request("GET", "http://localhost:8888/something_not_exist", [&](ResponsePtr res) 
    {
        TEST_CHECK(res->getStatus() == Response::Not_Found);
        server.stop();
    });
    server.run();
}

TEST_CASE(chunked_body_test)
{
    std::stringstream config("{\"http port\": \"8888\"}");
    Server server(config);
    auto chunked_test = std::make_shared<ChunkedTestServer>();
    server.addHandler("/chunked", chunked_test.get());
    Client c{server.service()};
    c.request("GET", "http://localhost:8888/chunked", [&](ResponsePtr res) 
    {
        TEST_REQUIRE(res);
        std::stringstream ss;
        ss << res->out().rdbuf();
        TEST_CHECK(ss.str() == "this is a chunked body");
        server.stop();
    });
    server.run();
}    
