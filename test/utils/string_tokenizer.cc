#include <boost/test/unit_test.hpp>
#include "utils.hh"

using namespace ahttpd;

BOOST_AUTO_TEST_CASE(string_tokenizer_test)
{
	StringTokenizer st("token1, token2", ',', ' ');
	BOOST_REQUIRE(st.hasMoreTokens());
	BOOST_CHECK(st.nextToken() == "token1");
	BOOST_REQUIRE(st.hasMoreTokens());
	BOOST_CHECK(st.nextToken() == "token2");
	BOOST_CHECK(!st.hasMoreTokens());
	st = StringTokenizer("=", '=');
	BOOST_CHECK(!st.hasMoreTokens());
}
