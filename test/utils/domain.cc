#include <boost/test/unit_test.hpp>
#include "utils.hh"

using namespace ahttpd;

BOOST_AUTO_TEST_CASE(domain_test)
{
	BOOST_CHECK(isDomainMatch("www.example.com", "example.com"));
	BOOST_CHECK(isDomainMatch("www.example.com", ".example.com"));
	BOOST_CHECK(isDomainMatch(".example.com", "example.com"));
	BOOST_CHECK(isDomainMatch(".example.com", ".example.com"));
	BOOST_CHECK(isDomainMatch("example.com", "example.com"));
	BOOST_CHECK(isDomainMatch("example.com", ".example.com"));

	BOOST_CHECK(!isDomainMatch("example.com", "www.example.com"));
	BOOST_CHECK(!isDomainMatch("example.com", "xample.com"));
	BOOST_CHECK(!isDomainMatch("example.com", "example."));
}
