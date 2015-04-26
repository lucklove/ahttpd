#include <boost/test/unit_test.hpp>
#include "request.hh"

BOOST_AUTO_TEST_CASE(request_header_test)
{
	Request req(nullptr);
	BOOST_CHECK(req.headerMap().size() == 0);
	req.addHeader("key1", "val1");
	req.addHeader("key2", "val2");
	req.addHeader("key1", "val3");
	BOOST_CHECK(req.headerMap().size() == 3);
	req.delHeader("key1");
	BOOST_CHECK(req.headerMap().size() == 1);
	BOOST_CHECK(!req.getHeader("key1"));
	BOOST_CHECK(*req.getHeader("key2") == "val2");
}

BOOST_AUTO_TEST_CASE(request_cookie_test)
{
}
