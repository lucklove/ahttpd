#include "UnitTest.hh"
#include "AsyncWrapper.hh"

TEST_CASE(async_wrapper_test)
{
    asyncWrap([](auto callback)
    {   
        callback(1);
    }).then([](auto callback1, auto callback2, int x)
    {
        TEST_CHECK(x == 1);
        callback1(2);
        callback2(3, 4); 
    }).then([](auto callback, int x)
    {
        TEST_CHECK(x == 2);
        callback(5);
    }, [](auto callback, int y, int z)
    {
        TEST_CHECK(y == 3);
        TEST_CHECK(z == 4);
        callback(6);
    }).then([](int x)
    {
        TEST_CHECK(x == 5 || x == 6);
    }).apply();
}
