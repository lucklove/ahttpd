#include "SslConnection.hh"
#include "ptrs.hh"
#include <utility>
#include <asio.hpp>

void 
SslConnection::stop()
{
	ssl_shutdown_timer_.expires_from_now(boost::posix_time::seconds(1));
	auto a = shared_from_this();
	SslConnectionPtr sft = std::dynamic_pointer_cast<SslConnection>(shared_from_this());
	ssl_shutdown_timer_.async_wait(
		std::bind(&SslConnection::stopNextLayer, sft, std::placeholders::_1));
	socket_.async_shutdown(
		std::bind(&SslConnection::stopNextLayer, sft, std::placeholders::_1));
}

void 
SslConnection::stopNextLayer(const asio::error_code& ec)
{
	ssl_shutdown_timer_.cancel();
	if(ec) {
		/**< TODO:记录错误信息 */
	}
	
	try {
		if(socket().is_open()) {
			asio::error_code ignored_ec;
			socket().shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
			socket().close();
		}
	} catch(asio::system_error& e) {
		/**< TODO:记录错误信息 */
	}
}		

void
SslConnection::async_handshake(const std::function<void (asio::error_code const&)>& handler)
{
	socket_.async_handshake(asio::ssl::stream_base::server, 
		[handler, ptr = shared_from_this()](const asio::error_code& e) {
			handler(e);
		}
	);
}

void 
SslConnection::async_read(result_of_t<decltype(&asio::transfer_exactly)(size_t)> completion,
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
SslConnection::async_read_until(const std::string& delim, 
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
SslConnection::async_write(const std::function<
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
