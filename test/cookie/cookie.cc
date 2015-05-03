#include <boost/test/unit_test.hpp>
#include "cookie.hh"

BOOST_AUTO_TEST_CASE(request_cookie_single_test)
{
	auto cookies = parseRequestCookie("user=username");
	BOOST_REQUIRE(cookies.size());
	BOOST_CHECK(cookies.size() == 1);
	BOOST_CHECK(cookies[0].key == "user");
	BOOST_CHECK(cookies[0].val == "username");
}

BOOST_AUTO_TEST_CASE(request_cookie_muti_test)
{
	auto cookies = parseRequestCookie("user=username;pass=password; addr=address");
	BOOST_REQUIRE(cookies.size());
	BOOST_CHECK(cookies.size() == 3);
	BOOST_CHECK(cookies[0].key == "user");
	BOOST_CHECK(cookies[0].val == "username");
	BOOST_CHECK(cookies[1].key == "pass");
	BOOST_CHECK(cookies[1].val == "password");
	BOOST_CHECK(cookies[2].key == "addr");
	BOOST_CHECK(cookies[2].val == "address");
}

BOOST_AUTO_TEST_CASE(response_cookie_single_test)
{
	auto cookie = parseResponseCookie("token; something else");
	BOOST_CHECK(cookie.key == "token");
	BOOST_CHECK(cookie.val == "");
	cookie = parseResponseCookie("user=username");
	BOOST_CHECK(cookie.key == "user");
	BOOST_CHECK(cookie.val == "username");
	cookie = parseResponseCookie("user=username; expires=Sat, 02-May-2015 23:38:25 GMT; secure; "
		"httponly;domain=www.example.com; path=/example");
	BOOST_CHECK(cookie.key == "user");
	BOOST_CHECK(cookie.val == "username");
	BOOST_CHECK(cookie.expires == gmtToTime("Sat, 02-May-2015 23:38:25 GMT"));
	BOOST_CHECK(cookie.domain == "www.example.com");
	BOOST_CHECK(cookie.path == "/example");
	BOOST_CHECK(cookie.secure == true);
	BOOST_CHECK(cookie.httponly == true);
}

BOOST_AUTO_TEST_CASE(response_cookie_muti_test)
{
	std::vector<std::string> cookie_headers;
	cookie_headers.push_back("user=username1");
	cookie_headers.push_back("user=username2; expires=Sat, 02-May-15 23:38:25 GMT;domain=www.example.com;");
	cookie_headers.push_back("user=username3; expires=Sat, 02-May-15 23:38:25 GMT;");
	auto cookies = parseResponseCookie(cookie_headers);
	BOOST_REQUIRE(cookies.size() == 3);
	BOOST_CHECK(cookies[0].key == "user");
	BOOST_CHECK(cookies[0].val == "username1");
	BOOST_CHECK(cookies[1].val == "username2");
	BOOST_CHECK(cookies[1].expires == gmtToTime("Sat, 02-May-2015 23:38:25"));
	BOOST_CHECK(cookies[1].domain == "www.example.com");
	BOOST_CHECK(cookies[2].expires == gmtToTime("Sat, 02-May-2015 23:38:25"));
}
