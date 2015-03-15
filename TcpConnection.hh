#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <boost/asio.hpp>
#include <iostream>
#include "connection.hh"
#include <iostream>

class Server;

/// Represents a single connection from a client.
class TcpConnection : public Connection
{
public:
	virtual ~TcpConnection() { std::cout << "tcp connection 析构" << std::endl; }

	explicit TcpConnection(boost::asio::io_service& service)
  		: socket_(service)
	{ std::cout << "tcp connection start" << std::endl; }

	void stop() override;

	void async_read_until(const std::string& delim, 
		std::function<void(const boost::system::error_code &, size_t)> handler) override;

	void async_read(result_of_t<decltype(&boost::asio::transfer_exactly)(size_t)> completion,
		std::function<void(const boost::system::error_code &, size_t)> handler) override {
			boost::asio::async_read(socket_, buffer(), completion, handler);
	}

	void async_write(std::function<
		void(const boost::system::error_code&, size_t)> handler) override;

	boost::asio::ip::tcp::socket& socket() { return socket_; }
private:
	boost::asio::ip::tcp::socket socket_;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
