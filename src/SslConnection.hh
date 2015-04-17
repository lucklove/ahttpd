#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <iostream>
#include "connection.hh"

class Server;

class SslConnection : public Connection
{
public:
	explicit SslConnection(boost::asio::io_service& service, boost::asio::ssl::context& context)
  		: Connection(service), socket_(service, context), ssl_shutdown_timer_(service)
	{}
	~SslConnection() {}

	void stop() override;
	socket_t socket() override { return socket_t{ &socket_ }; }
	boost::asio::ip::tcp::socket& nativeSocket() override { return socket_.next_layer(); }
	void async_handshake(std::function<void(const boost::system::error_code& e)> handle);
	void async_connect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr, bool)> handler) override;
private:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
	boost::asio::deadline_timer ssl_shutdown_timer_;
	void stopNextLayer(const boost::system::error_code& ec);
};
