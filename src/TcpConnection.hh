#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <mutex>
#include <boost/asio.hpp>
#include "connection.hh"

class Server;

class TcpConnection : public Connection {
public:
	explicit TcpConnection(boost::asio::io_service& service)
  		: socket_(service), resolver_(service)
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

	/**
 	 * \note 非线程安全
 	 */ 
	bool stoped() override { return stoped_; }
	
	void asyncConnect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr)> handler) override {

		boost::asio::ip::tcp::resolver::query query(host, port);

		resolver_.async_resolve(query,
			[=, ptr = shared_from_this()](const boost::system::error_code& err,
				boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
				if(err) {
					Log("DEBUG") << __FILE__ << ":" << __LINE__;
					Log("ERROR") << err.message();
					handler(nullptr);
				} else {
					boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
					nativeSocket().async_connect(endpoint,
						std::bind(&TcpConnection::handle_connect, this, std::placeholders::_1, 
							++endpoint_iterator, handler, ptr));
				}
			}
		);
	}		

	virtual boost::asio::ip::tcp::socket& nativeSocket() { return socket_; }

protected:
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

private:
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::resolver resolver_;
	bool stoped_{};
	std::mutex stop_mutex_;
	void handle_connect(const boost::system::error_code& err, 
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator, 
		std::function<void(ConnectionPtr)> handler,
		ConnectionPtr ptr) {			/**< 防止过早的析构 */
		if(!err) {
			handler(ptr);
		} else if(endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
			nativeSocket().close();
			boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
			nativeSocket().async_connect(endpoint, std::bind(&TcpConnection::handle_connect, this,
            			std::placeholders::_1, ++endpoint_iterator, handler, ptr));
		} else {
			Log("ERROR") << "connect faild";
			handler(nullptr);
		}
	}
};
