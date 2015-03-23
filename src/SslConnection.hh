#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <asio/ssl.hpp>
#include <iostream>
#include "connection.hh"
#include <iostream>

class Server;

/// Represents a single connection from a client.
class SslConnection : public Connection
{
public:
	using ssl_socket_t = asio::ssl::stream<asio::ip::tcp::socket>;

	explicit SslConnection(asio::io_service& service, 
		asio::ssl::context& context)
  		: socket_(service, context), ssl_shutdown_timer_(service)
	{}

	void stop() override;

	void async_read_until(const std::string& delim, 
		const std::function<void(const asio::error_code &, size_t)>& handler) override;

	void async_read(result_of_t<decltype(&asio::transfer_exactly)(size_t)> completion,
		const std::function<void(const asio::error_code &, size_t)>& handler) override {
			asio::async_read(socket_, readBuffer(), completion, handler);
	}

	void async_write(const std::function<
		void(const asio::error_code&, size_t)>& handler) override;

	asio::ip::tcp::socket& socket() { return socket_.next_layer(); }
	void async_handshake(const std::function<void(const asio::error_code& e)>& handle);

private:
	ssl_socket_t socket_;
	asio::deadline_timer ssl_shutdown_timer_;
	void stopNextLayer(const asio::error_code& ec);
};
