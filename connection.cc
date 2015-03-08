// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#include "connection.hh"
#include <utility>
#include <vector>
#include "ConnectionManager.hh"
#include "RequestHandler.hh"

namespace http {
namespace server {

Connection::Connection(boost::asio::ip::tcp::socket socket,
    ConnectionManager& manager, RequestHandler& handler)
  : socket_(std::move(socket)),
    connection_manager_(manager),
    request_handler_(handler)
{
}

void Connection::start()
{
  do_read();
}

void Connection::stop()
{
  socket_.close();
}

void Connection::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(boost::asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          RequestParser::result_type result;
          std::tie(result, std::ignore) = request_parser_.parse(
              request_, buffer_.data(), buffer_.data() + bytes_transferred);

          if (result == RequestParser::good)
          {
            request_handler_.handleRequest(request_, reply_);
            do_write();
          }
          else if (result == RequestParser::bad)
          {
            reply_ = reply::stock_reply(reply::bad_request);
            do_write();
          }
          else
          {
            do_read();
          }
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
          connection_manager_.stop(shared_from_this());
        }
      });
}

void Connection::do_write()
{
  auto self(shared_from_this());
  boost::asio::async_write(socket_, reply_.to_buffers(),
      [this, self](boost::system::error_code ec, std::size_t)
      {
        if (!ec)
        {
          // Initiate graceful connection closure.
          boost::system::error_code ignored_ec;
          socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
            ignored_ec);
        }

        if (ec != boost::asio::error::operation_aborted)
        {
          connection_manager_.stop(shared_from_this());
        }
      });
}

} // namespace server
} // namespace http
