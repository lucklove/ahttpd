#include <boost/test/unit_test.hpp>
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

BOOST_AUTO_TEST_CASE(client_cookie_simple_test)
{
    std::stringstream config("{\"http port\":\"8888\"}");
    Server s(config);
    auto set_cookie = std::make_shared<SetCookie>();
    s.addHandler("/", set_cookie.get());
    Client c(s.service());
    c.enableCookie();
    set_cookie->cookies.push_back(response_cookie_t().setKey("key1").setVal("val1"));
    c.request("GET", "http://localhost:8888", [&](ResponsePtr res) {
        auto cookie_jar = c.cookie_jar_;
        BOOST_REQUIRE(cookie_jar.size());
        BOOST_CHECK(cookie_jar[0].key == "key1");
        BOOST_CHECK(cookie_jar[0].val == "val1");
        BOOST_CHECK(cookie_jar[0].expires == 0);
        BOOST_CHECK(cookie_jar[0].domain == "localhost");
        BOOST_CHECK(cookie_jar[0].path == "/");
        BOOST_CHECK(cookie_jar[0].secure == false);
        BOOST_CHECK(cookie_jar[0].httponly == false);
        s.stop();
    });
    s.run();
}
