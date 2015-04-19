#define BOOST_TEST_MODULE client
#include <boost/test/included/unit_test.hpp>

#include "client.hh"
#include "response.hh"

BOOST_AUTO_TEST_CASE(client_test)
{
	Client c;
	c.request("GET", "http://www.example.com",
		[](ResponsePtr res, bool good) {
			BOOST_REQUIRE(good);
			BOOST_CHECK(res->status() == Response::ok);
		}
	);
	c.request("GET", "https://www.example.com",	
		[](ResponsePtr res, bool good) {
			BOOST_REQUIRE(good);
			BOOST_CHECK(res->status() == Response::ok);
		}
	);
	c.run();
}
