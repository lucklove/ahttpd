#include <boost/test/unit_test.hpp>
#include "request.hh"

BOOST_AUTO_TEST_CASE(request_header_test)
{
	Request req(nullptr);
	BOOST_CHECK(req.getHeaderMap().size() == 0);
	req.addHeader("key1", "val1");
	req.addHeader("key2", "val2");
	req.addHeader("key1", "val3");
	BOOST_CHECK(req.getHeaderMap().size() == 3);
	req.delHeader("key1");
	BOOST_CHECK(req.getHeaderMap().size() == 1);
	BOOST_CHECK(!req.getHeader("key1"));
	BOOST_CHECK(*req.getHeader("key2") == "val2");
}

BOOST_AUTO_TEST_CASE(request_cookie_test)
{
	Request req(nullptr);
	req.setCookie({"key1", "val1"});
	req.setCookie({"key2", "val2"});
	const std::string* ptr = req.getHeader("Cookie");
	BOOST_REQUIRE(ptr);
	BOOST_CHECK(*ptr == "key1=val1; key2=val2");
	req.parseCookie();
	ptr = req.getCookieValue("key1");
	BOOST_REQUIRE(ptr);
	BOOST_CHECK(*ptr == "val1");
	ptr = req.getCookieValue("key2");	
	BOOST_REQUIRE(ptr);
	BOOST_CHECK(*ptr == "val2");
	ptr = req.getCookieValue("key3");
	BOOST_CHECK(ptr == nullptr);
}

BOOST_AUTO_TEST_CASE(basic_auth_test)
{
	Request req(nullptr);
	req.basicAuth("ahttpd:ahttpd");
	BOOST_CHECK(req.basicAuthInfo() == "ahttpd:ahttpd");
}

BOOST_AUTO_TEST_CASE(param_test)
{
	Request req(nullptr);
	req.parseParams(urlEncode("你好") + "=" + "Hello");
	BOOST_CHECK(req.getParamValue("你好") == "Hello");
	req.parseParams("你好=Hello");
	BOOST_CHECK(req.getParamValue("你好") == "Hello");
	req.parseParams("subject=你好");
	BOOST_CHECK(req.getParamValue("subject") == "你好");
}
