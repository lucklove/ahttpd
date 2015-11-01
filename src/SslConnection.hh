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

namespace ahttpd 
{
class Server;

class SslConnection : public TcpConnection 
{
private:
	::boost::asio::ssl::stream<::boost::asio::ip::tcp::socket> socket_;
	::boost::asio::ip::tcp::resolver resolver_;
	::boost::asio::deadline_timer ssl_shutdown_timer_;
	void stopNextLayer(const ::boost::system::error_code& ec);
	bool stoped_{};
	std::mutex stop_mutex_;

public:
	explicit SslConnection(::boost::asio::io_service& service, ::boost::asio::ssl::context& context)
  		: TcpConnection(service), socket_(service, context), resolver_(service), ssl_shutdown_timer_(service)
	{}
    ~SslConnection() override;

	void stop() override;

	/**
 	 * \note 非线程安全
 	 */ 
	bool stoped() override;
	
	const char* type() override;

	::boost::asio::ip::tcp::socket& nativeSocket() override;

	void asyncConnect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr)> handler) override;

	void asyncHandshake(std::function<void(const ::boost::system::error_code& e)> handle);

protected:
	void async_read_until(const std::string& delim, 
		std::function<void(const ::boost::system::error_code &, size_t)> handler) override;

	void async_read(
		std::function<size_t(const ::boost::system::error_code &, size_t)> completion,
		std::function<void(const ::boost::system::error_code &, size_t)> handler) override;

	void async_write(const std::string& msg,
		std::function<void(const ::boost::system::error_code&, size_t)> handler) override;
};

}	/**< namespace ahttpd */
