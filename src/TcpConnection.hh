#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <asio.hpp>
#include "connection.hh"

class Server;

class TcpConnection : public Connection
{
public:
	explicit TcpConnection(asio::io_service& service)
  		: socket_(service)
	{}

	void stop() override;

	void async_read_until(const std::string& delim, 
		std::function<void(const asio::error_code &, size_t)> handler) override;

	void async_read(result_of_t<decltype(&asio::transfer_exactly)(size_t)> completion,
		std::function<void(const asio::error_code &, size_t)> handler) override {
			asio::async_read(socket_, buffer(), completion, handler);
	}

	void async_write(std::function<
		void(const asio::error_code&, size_t)> handler) override;

	asio::ip::tcp::socket& socket() { return socket_; }
private:
	asio::ip::tcp::socket socket_;
};
