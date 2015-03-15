// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#include "TcpConnection.hh"
#include <utility>
#include <vector>
#include <boost/asio.hpp>

void 
TcpConnection::stop()
{
	socket_.close();
}

void 
TcpConnection::async_read_until(const std::string& delim, 
	std::function<void(const boost::system::error_code &, size_t)> handler)
{
	boost::asio::async_read_until(socket_, buffer(), delim, handler);
}
	
void 
TcpConnection::async_write(std::function<
	void(const boost::system::error_code&, size_t)> handler)
{
	boost::asio::async_write(socket_, buffer(), std::bind(
		/** XXX:是否需要保证connection不过早析构? */
		[handler](const boost::system::error_code& e, size_t n) {
			handler(e, n);
		}, 
		std::placeholders::_1, std::placeholders::_2));
}
