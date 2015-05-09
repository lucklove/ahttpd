#include <boost/test/unit_test.hpp>
#include "utils.hh"

BOOST_AUTO_TEST_CASE(url_test)
{
	std::string url("你+好");
	url = urlEncode(url);
	BOOST_CHECK(urlDecode(url));	
	BOOST_CHECK(url == "你+好");
	url = "%0G";
	BOOST_CHECK(!urlDecode(url));
	url = "%0F";
	BOOST_CHECK(urlDecode(url));
}
