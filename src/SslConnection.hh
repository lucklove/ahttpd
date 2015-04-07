#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <asio/ssl.hpp>
#include <iostream>
#include <iostream>
#include "connection.hh"

#include "log.hh"

class Server;

class SslConnection : public Connection
{
public:
	explicit SslConnection(asio::io_service& service, 
		asio::ssl::context& context)
  		: Connection(service), socket_(service, context), ssl_shutdown_timer_(service)
	{}
	~SslConnection() {}
	void stop() override;
	socket_t socket() override { return socket_t{ &socket_ }; }
	asio::ip::tcp::socket& nativeSocket() override { return socket_.next_layer(); }
	void async_handshake(std::function<void(const asio::error_code& e)> handle);
	void async_connect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr, bool)> handler) override;
private:
	asio::ssl::stream<asio::ip::tcp::socket> socket_;
	asio::deadline_timer ssl_shutdown_timer_;
	void stopNextLayer(const asio::error_code& ec);
};
