#include "UnitTest.hh"
#include "utils.hh"

using namespace ahttpd;

TEST_CASE(string_tokenizer_test)
{
    StringTokenizer st("token1, token2", ',', ' ');
    TEST_REQUIRE(st.hasMoreTokens());
    TEST_CHECK(st.nextToken() == "token1");
    TEST_REQUIRE(st.hasMoreTokens());
    TEST_CHECK(st.nextToken() == "token2");
    TEST_CHECK(!st.hasMoreTokens());
    st = StringTokenizer("=", '=');
    TEST_CHECK(!st.hasMoreTokens());
}
