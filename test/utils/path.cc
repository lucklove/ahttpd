#include <boost/test/unit_test.hpp>
#include "utils.hh"

using namespace ahttpd;

BOOST_AUTO_TEST_CASE(path_test)
{
    BOOST_CHECK(isPathMatch("/", ""));
    BOOST_CHECK(isPathMatch("www.example.com", ""));
    BOOST_CHECK(isPathMatch("/123", ""));
    BOOST_CHECK(isPathMatch("/", "/"));
    BOOST_CHECK(isPathMatch("/1/2/3/", "/1/2"));
    BOOST_CHECK(isPathMatch("/1/2/3", "/1/2/"));
    BOOST_CHECK(isPathMatch("/1/2/3/", "/1/2/3"));
    BOOST_CHECK(isPathMatch("/1/2/3", "/1/2/3/"));

    
    BOOST_CHECK(!isPathMatch("/1/2/3/", "/1/2/4"));
    BOOST_CHECK(!isPathMatch("/1/2/3/", "/1/2/3/4"));
    BOOST_CHECK(!isPathMatch("", "/"));
}
