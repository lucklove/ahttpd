#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include "TcpConnection.hh"
#include "SslConnection.hh"

BOOST_AUTO_TEST_CASE(connection_test)
{
	boost::asio::io_service service;
	boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);
	ConnectionPtr conn = std::make_shared<TcpConnection>(service);
	conn->async_connect("www.example.com", "80",
		[](ConnectionPtr conn, bool good) {
			BOOST_CHECK(good);
		}
	);
	conn = std::make_shared<SslConnection>(service, ssl_context);
	conn->async_connect("www.example.com", "443",
		[](ConnectionPtr conn, bool good) {
			BOOST_CHECK(good);
		}
	);
	service.run();
}
