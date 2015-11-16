#include <boost/test/unit_test.hpp>
#include "request.hh"

using namespace ahttpd;

BOOST_AUTO_TEST_CASE(auth_test)
{
    Request req(nullptr);
    req.basicAuth("ahttpd:ahttpd");
    BOOST_CHECK(req.basicAuthInfo() == "ahttpd:ahttpd");
    req.addHeader("Proxy-Authorization", "Basic YWh0dHBkOmFodHRwZA==");
    BOOST_CHECK(req.proxyAuthInfo() == "ahttpd:ahttpd");
}
