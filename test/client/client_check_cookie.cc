#include "UnitTest.hh"
#include "server.hh"
#define private public
#include "client.hh"

using namespace ahttpd;

struct SetCookie : RequestHandler {
    void handleRequest(RequestPtr req, ResponsePtr res) {
        for(auto c : cookies)
            res->setCookie(c);    
    }
    std::vector<response_cookie_t> cookies;
};

TEST_CASE(client_cookie_simple_test)
{
/*
    std::stringstream config("{\"http port\":\"8888\"}");
    Server s(config);
    auto set_cookie = std::make_shared<SetCookie>();
    s.addHandler("/", set_cookie.get());
    Client c(s.service());
    c.enableCookie();
    set_cookie->cookies.push_back(response_cookie_t().setKey("key1").setVal("val1"));
    c.request("GET", "http://localhost:8888", [&](ResponsePtr res) {
        auto cookie_jar = c.cookie_jar_;
        TEST_REQUIRE(cookie_jar.size());
        TEST_CHECK(cookie_jar[0].key == "key1");
        TEST_CHECK(cookie_jar[0].val == "val1");
        TEST_CHECK(cookie_jar[0].expires == 0);
        TEST_CHECK(cookie_jar[0].domain == "localhost");
        TEST_CHECK(cookie_jar[0].path == "/");
        TEST_CHECK(cookie_jar[0].secure == false);
        TEST_CHECK(cookie_jar[0].httponly == false);
        s.stop();
    });
    s.run();
*/
}
