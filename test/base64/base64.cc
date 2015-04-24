#include <boost/test/unit_test.hpp>
#include <string>
#include "base64.hh"

BOOST_AUTO_TEST_CASE(base64_test)
{
	BOOST_CHECK(Base64::decode(Base64::encode("")) == "");	
	BOOST_CHECK(Base64::decode(Base64::encode("aA")) == "aA");	
	BOOST_CHECK(Base64::decode(Base64::encode("01234")) == "01234");
	BOOST_CHECK(Base64::decode(Base64::encode("0123")) == "0123");
	BOOST_CHECK(Base64::decode(Base64::encode("hello world")) == "hello world");
}	
