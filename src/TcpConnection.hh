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

	void stop() override { socket_.close(); }
	socket_t socket() override { return socket_t{ &socket_ }; }
	asio::ip::tcp::socket& nativeSocket() { return socket_; }
private:
	asio::ip::tcp::socket socket_;
};
