// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#include "SslConnection.hh"
#include <utility>
#include <vector>
#include <boost/asio.hpp>

void 
SslConnection::stop()
{
	ssl_shutdown_timer_.expires_from_now(boost::posix_time::seconds(1));
	auto sft = std::dynamic_pointer_cast<SslConnection>(shared_from_this());
	ssl_shutdown_timer_.async_wait(
		std::bind(&SslConnection::stopNextLayer, sft, std::placeholders::_1));
	socket_.async_shutdown(
		std::bind(&SslConnection::stopNextLayer, sft, std::placeholders::_1));
}

void 
SslConnection::stopNextLayer(const boost::system::error_code& ec)
{
	ssl_shutdown_timer_.cancel();
	if(ec) {
		/**< TODO:记录错误信息 */
	}
	
	try {
		if(socket().is_open()) {
			boost::system::error_code ignored_ec;
			socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			socket().close();
		}
	} catch(boost::system::system_error& e) {
		/**< TODO:记录错误信息 */
	}
}		

void 
SslConnection::async_read_until(const std::string& delim, 
	std::function<void(const boost::system::error_code &, size_t)> handler)
{
	boost::asio::async_read_until(socket_, buffer(), delim, handler);
}
	
void 
SslConnection::async_write(std::function<
	void(const boost::system::error_code&, size_t)> handler)
{
	boost::asio::async_write(socket_, buffer(), std::bind(
		/** XXX:是否需要保证connection不过早析构? */
		[handler](const boost::system::error_code& e, size_t n) {
			handler(e, n);
		}, 
		std::placeholders::_1, std::placeholders::_2));
}
