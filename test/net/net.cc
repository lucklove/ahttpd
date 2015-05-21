#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include <mutex>
#include <queue>
#include "buffer.hh"
#include "net.hh"
#include "log.hh"

BOOST_AUTO_TEST_CASE(tcp_connect_test)
{
	boost::asio::io_service service;
	TcpConnect(service, "www.example.com", "80",
		[](ConnectionPtr conn) {
			BOOST_REQUIRE(conn);
		}
	);
	TcpConnect(service, "localhost", "12345",
		[](ConnectionPtr conn) {
			BOOST_CHECK(!conn);
		}
	);
	service.run();
}

BOOST_AUTO_TEST_CASE(ssl_connect_test)
{
	boost::asio::io_service service;
	SslConnect(service, "www.example.com", "443",
		[](ConnectionPtr conn) {
			BOOST_REQUIRE(conn);
		}
	);
	SslConnect(service, "localhost", "12345",
		[](ConnectionPtr conn) {
			BOOST_CHECK(!conn);
		}
	);
	service.run();
}
/*
struct MockConnection : Connection {
	MockConnection(boost::asio::io_service& service, const std::string& to_send,
		const std::string& to_recv) : Connection(service), send_(to_send), recv_(to_recv)
	{}
	std::string send_;
	std::string recv_;
	boost::asio::ip::tcp::socket socket_;
	bool stoped_{};
	void stop() override { stoped_ = true; }
	bool stoped() override { return stoped_; }
	virtual boost::asio::ip::tcp::socket& nativeSocket() { return socket_; }
	void async_read(std::function<size_t(const boost::system::error_code &, size_t)> completion,
		std::function<void(const boost::system::error_code &, size_t)> handler) override {
		std::ostream out(&readBuffer());
		out << recv_;		
	}
	void async_write(const std::string& msg, 
		std::function<void(const boost::system::error_code&, size_t)> handler) override {
		std::ostream out(&readBuffer());
		out << recv_;		
	}
};

BOOST_AUTO_TEST_CASE(tunnel_test)
{
	Log("DEBUG") << sizeof(Connection);
	boost::asio::io_service service;
	tunnel(std::make_shared<Connection>(service), std::make_shared<Connection>(service));		
}
*/
