#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <mutex>
#include <queue>
#include <iostream>
#include <tuple>
#include "buffer.hh"
#include "log.hh"
#include "ptrs.hh"

class Server;
class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection>
{
protected:
        template<typename _type>
        using result_of_t = typename std::result_of<_type>::type;

  	Server *server_;
	enum class socket_type { ordinary, ssl };
	struct socket_t {
		socket_t(asio::ip::tcp::socket* ordinary_socket_) 
			: type(socket_type::ordinary), ordinary_socket(ordinary_socket_)
		{}
		socket_t(asio::ssl::stream<asio::ip::tcp::socket>* ssl_socket_)
			: type(socket_type::ssl), ssl_socket(ssl_socket_)
		{}
		socket_type type;
		union {
			asio::ip::tcp::socket* ordinary_socket;
			asio::ssl::stream<asio::ip::tcp::socket>* ssl_socket;
		};
	};

public:
	Connection(const Connection&) = delete;
	Connection& operator=(const Connection&) = delete;
	Connection(asio::io_service& service) 
		: service_(service), resolver_(service) {}	

	virtual ~Connection() {};

	virtual void stop() = 0; 

	buffer_t& readBuffer() { return read_buffer_; }

	virtual asio::ip::tcp::socket& nativeSocket() = 0;

	virtual void async_connect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr, bool)> handler) {
		asio::ip::tcp::resolver::query query(host, port);

		resolver_.async_resolve(query,
			[=, ptr = shared_from_this()](const asio::error_code& err,
				asio::ip::tcp::resolver::iterator endpoint_iterator) {
				if(err) {
					Log("ERROR") << err.message();
					handler(nullptr, false);
				} else {
					asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
					nativeSocket().async_connect(endpoint,
						std::bind(&Connection::handle_connect, this, std::placeholders::_1, 
							++endpoint_iterator, handler, ptr));
				}
			}
		);
	}		
	
	void async_read_until(const std::string& delim, 
		std::function<void(const asio::error_code &, size_t)> handler);

	void async_read(result_of_t<decltype(&asio::transfer_exactly)(size_t)> completion,
		std::function<void(const asio::error_code &, size_t)> handler);

	void async_write(const std::string& msg,
		std::function<void(const asio::error_code&, size_t)> handler =
			[](const asio::error_code& e, size_t n) {
				if(e) {
					Log("ERROR") << "WRITE ERROR:" << e.message() << ", "
						<< n << " bytes writen";
				}
			}
	);

	virtual socket_t socket() = 0;
protected:
	void enqueueRead(std::function<void()> read_func) {
		{
			std::unique_lock<std::mutex> lck(read_queue_mutex_);
			read_queue_.push(std::make_tuple(read_func, false));
		}
		doRead();
	}

	void dequeueRead() {
		{
			std::unique_lock<std::mutex> lck(read_queue_mutex_);
			read_queue_.pop();
		}

		doRead();
	}

	void doRead() {
		std::function<void()> func;

		{
			std::unique_lock<std::mutex> lck(read_queue_mutex_);
			if(read_queue_.size() == 0)
				return;
			auto& read_task = read_queue_.front();
			if(std::get<1>(read_task) == true)
				return;
			std::get<1>(read_task) = true;
			func = std::get<0>(read_task);
		}

		func();
	}

	void enqueueWrite(std::function<void()> write_func) {
		{
			std::unique_lock<std::mutex> lck(write_queue_mutex_);
			write_queue_.push(std::make_tuple(write_func, false));
		}
		
		doWrite();	
	}

	void dequeueWrite() {
		{
			std::unique_lock<std::mutex> lck(write_queue_mutex_);
			write_queue_.pop();
		}

		doWrite();
	}
	

	void doWrite() {
		std::function<void()> func;

		{
			std::unique_lock<std::mutex> lck(write_queue_mutex_);
			if(write_queue_.size() == 0)
				return;
			auto& write_task = write_queue_.front();
			if(std::get<1>(write_task) == true)
				return;
			std::get<1>(write_task) = true;
			func = std::get<0>(write_task);
		}

		func();
	}

private:
	asio::io_service& service_;
	asio::ip::tcp::resolver resolver_;
	buffer_t read_buffer_;
	std::queue<std::tuple<std::function<void()>, bool>> read_queue_;
	std::queue<std::tuple<std::function<void()>, bool>> write_queue_;
	std::mutex read_queue_mutex_{};
	std::mutex write_queue_mutex_{};
	void handle_connect(const asio::error_code& err, 
		asio::ip::tcp::resolver::iterator endpoint_iterator, 
		std::function<void(ConnectionPtr, bool)> handler,
		ConnectionPtr ptr) {			/**< 防止过早的析构 */
		if(!err) {
			handler(ptr, true);
		} else if(endpoint_iterator != asio::ip::tcp::resolver::iterator()) {
			nativeSocket().close();
			asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
			nativeSocket().async_connect(endpoint, std::bind(&Connection::handle_connect, this,
            			std::placeholders::_1, ++endpoint_iterator, handler, ptr));
		} else {
			Log("ERROR") << "connect faild";
			handler(nullptr, false);
		}
	}
};
