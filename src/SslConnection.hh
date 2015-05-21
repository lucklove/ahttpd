#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <mutex>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <iostream>
#include "connection.hh"

class Server;

class SslConnection : public Connection {
public:
	explicit SslConnection(boost::asio::io_service& service, boost::asio::ssl::context& context)
  		: Connection(service), socket_(service, context), resolver_(service), ssl_shutdown_timer_(service)
	{}
	~SslConnection() {}

	void stop() override;

	/**
 	 * \note 非线程安全
 	 */ 
	bool stoped() override { return stoped_; }

	boost::asio::ip::tcp::socket& nativeSocket() { return socket_.next_layer(); }
	void async_handshake(std::function<void(const boost::system::error_code& e)> handle);
	void async_connect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr)> handler) override;
	void low_level_connect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr)> handler) {
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
						std::bind(&SslConnection::handle_connect, this, std::placeholders::_1, 
							++endpoint_iterator, handler, ptr));
				}
			}
		);
	}
private:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::deadline_timer ssl_shutdown_timer_;
	void stopNextLayer(const boost::system::error_code& ec);
	bool stoped_{};
	std::mutex stop_mutex_;
	socket_t socket() override { return socket_t{ &socket_ }; }
	void handle_connect(const boost::system::error_code& err, 
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator, 
		std::function<void(ConnectionPtr)> handler,
		ConnectionPtr ptr) {			/**< 防止过早的析构 */
		if(!err) {
			handler(ptr);
		} else if(endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
			nativeSocket().close();
			boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
			nativeSocket().async_connect(endpoint, std::bind(&SslConnection::handle_connect, this,
            			std::placeholders::_1, ++endpoint_iterator, handler, ptr));
		} else {
			Log("ERROR") << "connect faild";
			handler(nullptr);
		}
	}
};
