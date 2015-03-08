// connection.hh
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "reply.hh"
#include "request.hh"
#include "RequestHandler.hh"
#include "RequestParser.hh"

namespace http {
namespace server {

class ConnectionManager;

/// Represents a single connection from a client.
class Connection
  : public std::enable_shared_from_this<Connection>
{
public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  /// Construct a connection with the given socket.
  explicit Connection(boost::asio::ip::tcp::socket socket,
      ConnectionManager& manager, RequestHandler& handler);

  /// Start the first asynchronous operation for the connection.
  void start();

  /// Stop all asynchronous operations associated with the connection.
  void stop();

private:
  /// Perform an asynchronous read operation.
  void do_read();

  /// Perform an asynchronous write operation.
  void do_write();

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// The manager for this connection.
  ConnectionManager& connection_manager_;

  /// The handler used to process the incoming request.
  RequestHandler& request_handler_;

  /// Buffer for incoming data.
  std::array<char, 8192> buffer_;

  /// The incoming request.
  Request request_;

  /// The parser for the incoming request.
  RequestParser request_parser_;

  /// The reply to be sent back to the client.
  reply reply_;
};

typedef std::shared_ptr<Connection> connection_ptr;

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP
