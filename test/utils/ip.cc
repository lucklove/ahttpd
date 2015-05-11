#include <boost/test/unit_test.hpp>
#include "utils.hh"

BOOST_AUTO_TEST_CASE(ipv4_test)
{
	BOOST_CHECK(isValidIPAddress("192.168.1.1"));
	BOOST_CHECK(isValidIPAddress("0.0.0.0"));
	BOOST_CHECK(isValidIPAddress("255.255.255.255"));

	BOOST_CHECK(!isValidIPAddress("192.168.1."));
	BOOST_CHECK(!isValidIPAddress("256.168.1.1"));
	BOOST_CHECK(!isValidIPAddress("192.168.1.-1"));
	BOOST_CHECK(!isValidIPAddress("192.168.1.1.1"));
	BOOST_CHECK(!isValidIPAddress("192.168.1"));
	BOOST_CHECK(!isValidIPAddress("192.168.1.a"));
	BOOST_CHECK(!isValidIPAddress("www.baidu.com"));
}

BOOST_AUTO_TEST_CASE(ipv6_test)
{
	BOOST_CHECK(isValidIPAddress("CDCD:910A:2222:5498:8475:1111:3900:2020"));
	BOOST_CHECK(isValidIPAddress("CDCD:910A:2222:5498:8475:1111:3900:"));
	BOOST_CHECK(isValidIPAddress(":910A:2222:5498:8475:1111:3900:2020"));
	BOOST_CHECK(isValidIPAddress("1030::C9B4:FF12:48AA:1A2B"));
	BOOST_CHECK(isValidIPAddress("2000:0:0:0:0:0:0:1"));
	BOOST_CHECK(isValidIPAddress("2001:DB8:2de::e13"));
	BOOST_CHECK(isValidIPAddress("AD80::ABAA:0000:00C2:0002"));

	BOOST_CHECK(!isValidIPAddress("AD80::ABAA::00C2:0002"));
	BOOST_CHECK(!isValidIPAddress("AD80::ABAA:0000:00C2:0002:"));
	BOOST_CHECK(!isValidIPAddress("www.baidu.com"));
}
