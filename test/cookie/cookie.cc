#include "UnitTest.hh"
#include "cookie.hh"

using namespace ahttpd;

TEST_CASE(request_cookie_single_test)
{
    auto cookies = parseRequestCookie("user=username");
    TEST_REQUIRE(cookies.size());
    TEST_CHECK(cookies.size() == 1);
    TEST_CHECK(cookies[0].key == "user");
    TEST_CHECK(cookies[0].val == "username");
}

TEST_CASE(request_cookie_muti_test)
{
    auto cookies = parseRequestCookie("user=username;pass=password; addr=address");
    TEST_REQUIRE(cookies.size());
    TEST_CHECK(cookies.size() == 3);
    TEST_CHECK(cookies[0].key == "user");
    TEST_CHECK(cookies[0].val == "username");
    TEST_CHECK(cookies[1].key == "pass");
    TEST_CHECK(cookies[1].val == "password");
    TEST_CHECK(cookies[2].key == "addr");
    TEST_CHECK(cookies[2].val == "address");
}

TEST_CASE(response_cookie_single_test)
{
    auto cookie = parseResponseCookie("token; something else");
    TEST_CHECK(cookie.key == "token");
    TEST_CHECK(cookie.val == "");
    cookie = parseResponseCookie("user=username");
    TEST_CHECK(cookie.key == "user");
    TEST_CHECK(cookie.val == "username");
    cookie = parseResponseCookie("user=username; expires=Sat, 02-May-2015 23:38:25 GMT; secure; "
        "httponly;domain=www.example.com; path=/example");
    TEST_CHECK(cookie.key == "user");
    TEST_CHECK(cookie.val == "username");
    TEST_CHECK(cookie.expires == gmtToTime("Sat, 02-May-2015 23:38:25 GMT"));
    TEST_CHECK(cookie.domain == "www.example.com");
    TEST_CHECK(cookie.path == "/example");
    TEST_CHECK(cookie.secure == true);
    TEST_CHECK(cookie.httponly == true);
}

TEST_CASE(response_cookie_muti_test)
{
    std::vector<std::string> cookie_headers;
    cookie_headers.push_back("user=username1");
    cookie_headers.push_back("user=username2; expires=Sat, 02-May-15 23:38:25 GMT;domain=www.example.com;");
    cookie_headers.push_back("user=username3; expires=Sat, 02-May-15 23:38:25 GMT;");
    auto cookies = parseResponseCookie(cookie_headers);
    TEST_REQUIRE(cookies.size() == 3);
    TEST_CHECK(cookies[0].key == "user");
    TEST_CHECK(cookies[0].val == "username1");
    TEST_CHECK(cookies[1].val == "username2");
    TEST_CHECK(cookies[1].expires == gmtToTime("Sat, 02-May-2015 23:38:25"));
    TEST_CHECK(cookies[1].domain == "www.example.com");
    TEST_CHECK(cookies[2].expires == gmtToTime("Sat, 02-May-2015 23:38:25"));
}
