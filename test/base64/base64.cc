#include <boost/test/unit_test.hpp>
#include <string>
#include <random>
#include "base64.hh"
#include <iostream>

using namespace ahttpd;

BOOST_AUTO_TEST_CASE(base64_test)
{
    BOOST_CHECK(Base64::decode(Base64::encode("")) == "");    
    BOOST_CHECK(Base64::decode(Base64::encode("aA")) == "aA");    
    BOOST_CHECK(Base64::decode(Base64::encode("01234")) == "01234");
    BOOST_CHECK(Base64::decode(Base64::encode("0123")) == "0123");
    BOOST_CHECK(Base64::decode(Base64::encode("hello world")) == "hello world");
}

BOOST_AUTO_TEST_CASE(base64_random_test)
{
    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<unsigned char> c(0,255);
    for(size_t i = 0; i < 1000; ++i) {
        std::string text;
        text.resize(i);
        for(size_t j = 0; j < i; ++j)
            text[j] = c(e);
        BOOST_CHECK(Base64::decode(Base64::encode(text)) == text);
    }
}
