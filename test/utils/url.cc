#include "UnitTest.hh"
#include "utils.hh"

using namespace ahttpd;

TEST_CASE(url_test)
{
    std::string url("你+好");
    url = urlEncode(url);
    TEST_CHECK(urlDecode(url));    
    TEST_CHECK(url == "你+好");
    url = urlEncode(url);
    url = urlEncode(url);
    TEST_CHECK(urlDecode(url));    
    TEST_CHECK(urlDecode(url));    
    TEST_CHECK(url == "你+好");
    url = "%0G";
    TEST_CHECK(!urlDecode(url));
    url = "%0F";
    TEST_CHECK(urlDecode(url));
}
