#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <boost/asio.hpp>
#include "buffer.hh"

class ConnectionManager;
class Server;

/// Represents a single connection from a client.
class Connection
  : public std::enable_shared_from_this<Connection>
{
private:
        template<typename _type>
        using result_of_t = typename std::result_of<_type>::type;

public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  /// Construct a connection with the given socket.
  explicit Connection(boost::asio::ip::tcp::socket socket);
//      ConnectionManager& manager, RequestHandler& handler, Server* server);

  /// Stop all asynchronous operations associated with the connection.
  void stop();

	buffer_t& buffer() { return buffer_; }
	void async_read_until(const std::string& delim, 
		std::function<void(const boost::system::error_code &, size_t)> handler);

	void async_read(result_of_t<decltype(&boost::asio::transfer_exactly)(size_t)> completion,
		std::function<void(const boost::system::error_code &, size_t)> handler) {
		boost::asio::async_read(socket_, buffer_, completion, handler);
	}

private:
  /// Perform an asynchronous read operation.
  void do_read();

  /// Perform an asynchronous write operation.
  void do_write();

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  Server *server_;

  buffer_t buffer_;

};

using ConnectionPtr = std::shared_ptr<Connection>;
