#include <boost/test/unit_test.hpp>
#include <string>
#include "base64.hh"

BOOST_AUTO_TEST_CASE(base64_test)
{
	BOOST_CHECK(Base64::decode(Base64::encode("")) == "");	
	BOOST_CHECK(Base64::decode(Base64::encode("aA")) == "aA");	
	BOOST_CHECK(Base64::decode(Base64::encode("0")) == "0");
}	
