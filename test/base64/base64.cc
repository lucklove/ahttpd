#include "UnitTest.hh"
#include <string>
#include <random>
#include "base64.hh"
#include <iostream>

using namespace ahttpd;

TEST_CASE(base64_test)
{
    TEST_CHECK(Base64::decode(Base64::encode("")) == "");    
    TEST_CHECK(Base64::decode(Base64::encode("aA")) == "aA");    
    TEST_CHECK(Base64::decode(Base64::encode("01234")) == "01234");
    TEST_CHECK(Base64::decode(Base64::encode("0123")) == "0123");
    TEST_CHECK(Base64::decode(Base64::encode("hello world")) == "hello world");
}

TEST_CASE(base64_random_test)
{
    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<unsigned char> c(0,255);
    for(size_t i = 0; i < 1000; ++i) 
    {
        std::string text;
        text.resize(i);
        for(size_t j = 0; j < i; ++j)
            text[j] = c(e);
        TEST_CHECK(Base64::decode(Base64::encode(text)) == text);
    }
}
