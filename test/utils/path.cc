#include "UnitTest.hh"
#include "utils.hh"

using namespace ahttpd;

TEST_CASE(path_test)
{
    TEST_CHECK(isPathMatch("/", ""));
    TEST_CHECK(isPathMatch("www.example.com", ""));
    TEST_CHECK(isPathMatch("/123", ""));
    TEST_CHECK(isPathMatch("/", "/"));
    TEST_CHECK(isPathMatch("/1/2/3/", "/1/2"));
    TEST_CHECK(isPathMatch("/1/2/3", "/1/2/"));
    TEST_CHECK(isPathMatch("/1/2/3/", "/1/2/3"));
    TEST_CHECK(isPathMatch("/1/2/3", "/1/2/3/"));

    
    TEST_CHECK(!isPathMatch("/1/2/3/", "/1/2/4"));
    TEST_CHECK(!isPathMatch("/1/2/3/", "/1/2/3/4"));
    TEST_CHECK(!isPathMatch("", "/"));
}
