#include "UnitTest.hh"
#include "utils.hh"

using namespace ahttpd;

TEST_CASE(domain_test)
{
    TEST_CHECK(isDomainMatch("www.example.com", "example.com"));
    TEST_CHECK(isDomainMatch("www.example.com", ".example.com"));
    TEST_CHECK(isDomainMatch(".example.com", "example.com"));
    TEST_CHECK(isDomainMatch(".example.com", ".example.com"));
    TEST_CHECK(isDomainMatch("example.com", "example.com"));
    TEST_CHECK(isDomainMatch("example.com", ".example.com"));

    TEST_CHECK(!isDomainMatch("example.com", "www.example.com"));
    TEST_CHECK(!isDomainMatch("example.com", "xample.com"));
    TEST_CHECK(!isDomainMatch("example.com", "example."));
}
