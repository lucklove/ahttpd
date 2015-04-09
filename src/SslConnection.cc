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
		Log("ERROR") << ec.message();
	}
	
	try {
		if(nativeSocket().is_open()) {
			asio::error_code ignored_ec;
			nativeSocket().cancel();
			nativeSocket().shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
			nativeSocket().close();
		}
	} catch(asio::system_error& e) {
		Log("ERROR") << e.what();
	}
}		

/**
 * \brief 作为服务端握手
 */ 
void
SslConnection::async_handshake(std::function<void (asio::error_code const&)> handler)
{
	socket_.async_handshake(asio::ssl::stream_base::server, 
		[handler, ptr = shared_from_this()](const asio::error_code& e) {
			handler(e);
		}
	);
}
	
void 
SslConnection::async_connect(const std::string& host, const std::string& port,
	std::function<void(ConnectionPtr, bool)> handler)
{
	Connection::async_connect(host, port, 
		[this, handler, ptr = shared_from_this()](ConnectionPtr conn, bool good) {
		if(good) {
			socket_.set_verify_mode(asio::ssl::verify_peer);
			socket_.set_verify_callback([](bool, asio::ssl::verify_context&) { 
				/** XXX: NOT SAFE */
				return true; 
			});
			socket_.async_handshake(asio::ssl::stream_base::client,
				[=, ptr = ptr](const asio::error_code& e) {
					if(e) {
						handler(conn, false);
					} else {
						handler(conn, true);
					}
				}
			);
		} else {
			Log(__FILE__) << __LINE__;
			handler(conn, false);
		}
	});
}
