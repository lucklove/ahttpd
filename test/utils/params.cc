#include <boost/test/unit_test.hpp>
#include "utils.hh"
#include "params.hh"

BOOST_AUTO_TEST_CASE(param_test)
{
	auto map = parseParams("key1=val1&=&key2=val2&key3=&=");
	BOOST_REQUIRE(map.size() == 3);
	BOOST_CHECK(map["key1"] == "val1");
	BOOST_CHECK(map["key2"] == "val2");
	BOOST_CHECK(map["key3"] == "");
}
