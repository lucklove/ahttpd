#include "UnitTest.hh"
#include "utils.hh"

using namespace ahttpd;

TEST_CASE(gmt_test)
{
    time_t now = time(nullptr);
    TEST_CHECK(gmtToTime(gmtTime(now)) == now);
}
