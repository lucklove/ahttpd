#include "UnitTest.hh"
#include "request.hh"

using namespace ahttpd;

TEST_CASE(auth_test)
{
    Request req(nullptr);
    req.basicAuth("ahttpd:ahttpd");
    TEST_CHECK(req.basicAuthInfo() == "ahttpd:ahttpd");
    req.addHeader("Proxy-Authorization", "Basic YWh0dHBkOmFodHRwZA==");
    TEST_CHECK(req.proxyAuthInfo() == "ahttpd:ahttpd");
}
