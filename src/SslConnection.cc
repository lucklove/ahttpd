#include "SslConnection.hh"
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
SslConnection::async_read_until(const std::string& delim, 
	std::function<void(const asio::error_code &, size_t)> handler)
{
	asio::async_read_until(socket_, buffer(), delim, handler);
}
	
void 
SslConnection::async_write(std::function<
	void(const asio::error_code&, size_t)> handler)
{
	asio::async_write(socket_, buffer(), std::bind(
		[handler](const asio::error_code& e, size_t n, ConnectionPtr) {
			handler(e, n);
		}, 
		std::placeholders::_1, std::placeholders::_2, shared_from_this())); /**< 防止过早析构 */
}

void
SslConnection::async_handshake(std::function<void (asio::error_code const&)> handle)
{
	socket_.async_handshake(asio::ssl::stream_base::server, handle);
}
