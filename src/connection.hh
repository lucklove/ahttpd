#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <mutex>
#include <queue>
#include <iostream>
#include <tuple>
#include "buffer.hh"
#include "log.hh"
#include "ptrs.hh"

class Connection : public std::enable_shared_from_this<Connection> {
protected:
	enum class socket_type { ordinary, ssl };
	struct socket_t {
		socket_t(boost::asio::ip::tcp::socket* ordinary_socket_) 
			: type(socket_type::ordinary), ordinary_socket(ordinary_socket_)
		{}
		socket_t(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* ssl_socket_)
			: type(socket_type::ssl), ssl_socket(ssl_socket_)
		{}
		socket_type type;
		union {
			boost::asio::ip::tcp::socket* ordinary_socket;
			boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* ssl_socket;
		};
	};

public:
	Connection(const Connection&) = delete;
	Connection& operator=(const Connection&) = delete;
	Connection(boost::asio::io_service& service) 
		: service_(service), resolver_(service) {}	

	virtual ~Connection() {};

	virtual void stop() = 0; 

	virtual bool stoped() = 0; 

	buffer_t& readBuffer() { return read_buffer_; }

	virtual void async_connect(const std::string& host, const std::string& port,
		std::function<void(ConnectionPtr)> handler) = 0;

	virtual void async_read_until(const std::string& delim, 
		std::function<void(const boost::system::error_code &, size_t)> handler);

	virtual void async_read(std::function<size_t(const boost::system::error_code &, size_t)> completion,
		std::function<void(const boost::system::error_code &, size_t)> handler);

	virtual void async_write(const std::string& msg,
		std::function<void(const boost::system::error_code&, size_t)> handler =
			[](const boost::system::error_code& e, size_t n) {
				if(e) {
					Log("DEBUG") << __FILE__ << ":" << __LINE__;
					Log("ERROR") << "WRITE ERROR:" << e.message() << ", "
						<< n << " bytes writen";
				}
			}
	);

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
	boost::asio::io_service& service_;
	boost::asio::ip::tcp::resolver resolver_;
	buffer_t read_buffer_;
	std::queue<std::tuple<std::function<void()>, bool>> read_queue_;
	std::queue<std::tuple<std::function<void()>, bool>> write_queue_;
	std::mutex read_queue_mutex_{};
	std::mutex write_queue_mutex_{};
	virtual socket_t socket() = 0;
};
