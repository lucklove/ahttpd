#include "TcpConnection.hh"
#include <utility>
#include <asio.hpp>
#include "log.hh"
#include "ptrs.hh"

void 
TcpConnection::stop()
{
	socket_.close();
}

void 
TcpConnection::async_read(result_of_t<decltype(&asio::transfer_exactly)(size_t)> completion,
	const std::function<void(const asio::error_code &, size_t)>& handler) 
{
	enqueueRead([=, ptr = shared_from_this()] {
		asio::async_read(socket_, readBuffer(), completion, 
			[this, handler, ptr](const asio::error_code& e, size_t n) {
				dequeueRead();
				handler(e, n); 
			}
		);
	});
	doRead();
}

void 
TcpConnection::async_read_until(const std::string& delim, 
	const std::function<void(const asio::error_code &, size_t)>& handler)
{
	enqueueRead([=, ptr = shared_from_this()] {
		asio::async_read_until(socket_, readBuffer(), delim, 
			[this, handler, ptr](const asio::error_code& e, size_t n) {
				dequeueRead();
				handler(e, n);
			}
		);
	});
	doRead();
}
	
void 
TcpConnection::async_write(const std::function<
	void(const asio::error_code&, size_t)>& handler)
{
	enqueueWrite([=, ptr = shared_from_this()] {
		asio::async_write(socket_, writeBuffer(), 
			[this, handler, ptr](const asio::error_code& e, size_t n) {
				dequeueWrite();
				handler(e, n);
			}
		);
	});
	doWrite();
}
