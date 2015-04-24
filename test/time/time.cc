#include <boost/test/unit_test.hpp>
#include "utils.hh"

BOOST_AUTO_TEST_CASE(gmt_test)
{
	time_t now = time(nullptr);
	BOOST_CHECK(gmtToTime(gmtTime(now)) == now);
}
