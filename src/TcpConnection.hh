#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <mutex>
#include <boost/asio.hpp>
#include "connection.hh"

class Server;

class TcpConnection : public Connection
{
public:
	explicit TcpConnection(boost::asio::io_service& service)
  		: Connection(service), socket_(service), resolver_(service)
	{}

	void stop() override {
		std::unique_lock<std::mutex> lck(stop_mutex_);
		if(stoped_)
			return;
		stoped_ = true;
		boost::system::error_code ignored_ec;
		nativeSocket().cancel();
		nativeSocket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		socket_.close(); 
	}

	socket_t socket() override { return socket_t{ &socket_ }; }
	boost::asio::ip::tcp::socket& nativeSocket() override { return socket_; }

private:
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::resolver resolver_;
	bool stoped_;
	std::mutex stop_mutex_;
};
