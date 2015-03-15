#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include "connection.hh"
#include <iostream>

class Server;

/// Represents a single connection from a client.
class SslConnection : public Connection
{
public:
	using ssl_socket_t = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

	virtual ~SslConnection() { std::cout << "ssl connection 析构" << std::endl; }

	explicit SslConnection(boost::asio::io_service& service, 
		boost::asio::ssl::context& context)
  		: socket_(service, context), ssl_shutdown_timer_(service)
	{ std::cout << "ssl connection start" << std::endl; }

	void stop() override;

	void async_read_until(const std::string& delim, 
		std::function<void(const boost::system::error_code &, size_t)> handler) override;

	void async_read(result_of_t<decltype(&boost::asio::transfer_exactly)(size_t)> completion,
		std::function<void(const boost::system::error_code &, size_t)> handler) override {
			boost::asio::async_read(socket_, buffer(), completion, handler);
	}

	void async_write(std::function<
		void(const boost::system::error_code&, size_t)> handler) override;

	boost::asio::ip::tcp::socket& socket() { return socket_.next_layer(); }
private:
	ssl_socket_t socket_;
	boost::asio::deadline_timer ssl_shutdown_timer_;
	void stopNextLayer(const boost::system::error_code& ec);
};

using SslConnectionPtr = std::shared_ptr<SslConnection>;
