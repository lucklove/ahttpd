#include <boost/test/unit_test.hpp>
#include "response.hh"

BOOST_AUTO_TEST_CASE(response_cookie_test)
{
	Response res(nullptr);
	time_t t = time(nullptr);
	res.setCookie(response_cookie_t().setKey("key1").setVal("val1").setSecure());
	res.setCookie(response_cookie_t().setKey("key2").setVal("val2").setExpires(t));
	auto hs = res.getHeaders("Set-Cookie");
	BOOST_CHECK(hs.size() == 2);
	res.parseCookie();
	auto cookie_jar = res.cookieJar();
	BOOST_REQUIRE(cookie_jar.size() == 2);
	BOOST_CHECK(cookie_jar[0].key == "key1");
	BOOST_CHECK(cookie_jar[0].val == "val1");
	BOOST_CHECK(cookie_jar[0].secure == true);
	BOOST_CHECK(cookie_jar[0].httponly == false);
	BOOST_CHECK(cookie_jar[1].key == "key2");
	BOOST_CHECK(cookie_jar[1].val == "val2");
	BOOST_CHECK(cookie_jar[1].expires == t);
}
