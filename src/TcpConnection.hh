#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <boost/asio.hpp>
#include "connection.hh"

class Server;

class TcpConnection : public Connection
{
public:
	explicit TcpConnection(boost::asio::io_service& service)
  		: Connection(service), socket_(service), resolver_(service)
	{}

	void stop() override { 
		boost::system::error_code ignored_ec;
		nativeSocket().cancel();
		nativeSocket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		socket_.close(); 
	}

	socket_t socket() override { return socket_t{ &socket_ }; }
	boost::asio::ip::tcp::socket& nativeSocket() override { return socket_; }

private:
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::resolver resolver_;
};
