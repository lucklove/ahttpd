#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include "TcpConnection.hh"
#include "SslConnection.hh"

BOOST_AUTO_TEST_CASE(tcp_connection_test)
{
	boost::asio::io_service service;
	ConnectionPtr conn = std::make_shared<TcpConnection>(service);
	conn->asyncConnect("www.example.com", "80",
		[](ConnectionPtr conn) {
			BOOST_REQUIRE(conn);
			conn->stop();
			conn->stop();			/**< 测试stop状态 */
		}
	);
	service.run();
}

BOOST_AUTO_TEST_CASE(ssl_connection_test)
{
	boost::asio::io_service service;
	boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);
	ConnectionPtr conn = std::make_shared<SslConnection>(service, ssl_context);
	conn->asyncConnect("www.example.com", "443",
		[](ConnectionPtr conn) {
			BOOST_CHECK(conn);
			conn->stop();
			conn->stop();
		}
	);
	service.run();
}
