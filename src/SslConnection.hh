#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <mutex>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <iostream>
#include "TcpConnection.hh"

class Server;

class SslConnection : public TcpConnection {
private:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::deadline_timer ssl_shutdown_timer_;
	void stopNextLayer(const boost::system::error_code& ec);
	bool stoped_{};
	std::mutex stop_mutex_;

public:
	explicit SslConnection(boost::asio::io_service& service, boost::asio::ssl::context& context)
  		: TcpConnection(service), socket_(service, context), resolver_(service), ssl_shutdown_timer_(service)
	{}
	~SslConnection() {}

	void stop() override;

	/**
 	 * \note 非线程安全
 	 */ 
	bool stoped() override { return stoped_; }

	boost::asio::ip::tcp::socket& nativeSocket() override { return socket_.next_layer(); }

	void asyncConnect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr)> handler) override;

	void asyncHandshake(std::function<void(const boost::system::error_code& e)> handle);
private:

	void async_read_until(const std::string& delim, 
		std::function<void(const boost::system::error_code &, size_t)> handler) override {
		boost::asio::async_read_until(socket_, readBuffer(), delim, handler);
	}

	void async_read(
		std::function<size_t(const boost::system::error_code &, size_t)> completion,
		std::function<void(const boost::system::error_code &, size_t)> handler) override {
		boost::asio::async_read(socket_, readBuffer(), completion, handler);
	}

	void async_write(const std::string& msg,
		std::function<void(const boost::system::error_code&, size_t)> handler) override {
		boost::asio::async_write(socket_, boost::asio::buffer(msg), handler);
	}
};
