#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <asio.hpp>
#include <iostream>
#include "buffer.hh"
#include "log.hh"

class Server;

/// Represents a single connection from a client.
class Connection
	: public std::enable_shared_from_this<Connection>
{
protected:
        template<typename _type>
        using result_of_t = typename std::result_of<_type>::type;

  	Server *server_;
public:
	Connection(const Connection&) = delete;
	Connection& operator=(const Connection&) = delete;
	Connection() = default;	

	virtual ~Connection() {};

	virtual void stop() = 0;

	buffer_t& readBuffer() { return read_buffer_; }
	buffer_t& writeBuffer() { return write_buffer_; }

	virtual void async_read_until(const std::string& delim, 
		std::function<void(const asio::error_code &, size_t)> handler) = 0;

	virtual void async_read(result_of_t<decltype(&asio::transfer_exactly)(size_t)> completion,
		std::function<void(const asio::error_code &, size_t)> handler) = 0;

	virtual void async_write(std::function<
		void(const asio::error_code&, size_t)> handler) = 0;

private:
	buffer_t read_buffer_;
	buffer_t write_buffer_;
};
