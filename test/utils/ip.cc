#include "UnitTest.hh"
#include "utils.hh"

using namespace ahttpd;

TEST_CASE(ipv4_test)
{
    TEST_CHECK(isValidIPAddress("192.168.1.1"));
    TEST_CHECK(isValidIPAddress("0.0.0.0"));
    TEST_CHECK(isValidIPAddress("255.255.255.255"));

    TEST_CHECK(!isValidIPAddress("192.168.1."));
    TEST_CHECK(!isValidIPAddress("256.168.1.1"));
    TEST_CHECK(!isValidIPAddress("192.168.1.-1"));
    TEST_CHECK(!isValidIPAddress("192.168.1.1.1"));
    TEST_CHECK(!isValidIPAddress("192.168.1"));
    TEST_CHECK(!isValidIPAddress("192.168.1.a"));
    TEST_CHECK(!isValidIPAddress("www.baidu.com"));
}

TEST_CASE(ipv6_test)
{
    TEST_CHECK(isValidIPAddress("CDCD:910A:2222:5498:8475:1111:3900:2020"));
    TEST_CHECK(isValidIPAddress("CDCD:910A:2222:5498:8475:1111:3900:"));
    TEST_CHECK(isValidIPAddress(":910A:2222:5498:8475:1111:3900:2020"));
    TEST_CHECK(isValidIPAddress("1030::C9B4:FF12:48AA:1A2B"));
    TEST_CHECK(isValidIPAddress("2000:0:0:0:0:0:0:1"));
    TEST_CHECK(isValidIPAddress("2001:DB8:2de::e13"));
    TEST_CHECK(isValidIPAddress("AD80::ABAA:0000:00C2:0002"));

    TEST_CHECK(!isValidIPAddress("AD80::ABAA::00C2:0002"));
    TEST_CHECK(!isValidIPAddress("AD80::ABAA:0000:00C2:0002:"));
    TEST_CHECK(!isValidIPAddress("www.baidu.com"));
}
