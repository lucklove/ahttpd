#include <boost/test/unit_test.hpp>
#include "AsyncWrapper.hh"

BOOST_AUTO_TEST_CASE(async_wrapper_test)
{
    asyncWrap([](auto callback)
    {   
        callback(1);
    }).then([](auto callback1, auto callback2, int x)
    {
        BOOST_CHECK(x == 1);
        callback1(2);
        callback2(3, 4); 
    }).then([](int x)
    {   
        BOOST_CHECK(x == 2);
    }, [](int y, int z)
    {
        BOOST_CHECK(y == 3); 
        BOOST_CHECK(z == 4); 
    }).apply();
}
