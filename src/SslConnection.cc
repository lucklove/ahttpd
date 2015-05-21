#include "SslConnection.hh"
#include "ptrs.hh"
#include <utility>
#include <boost/asio.hpp>

void 
SslConnection::stop()
{
	std::unique_lock<std::mutex> lck(stop_mutex_);
	if(stoped_)
		return;
	stoped_ = true;
	SslConnectionPtr sft = std::dynamic_pointer_cast<SslConnection>(shared_from_this());
	socket_.async_shutdown(
		std::bind(&SslConnection::stopNextLayer, sft, std::placeholders::_1));
}

void 
SslConnection::stopNextLayer(const boost::system::error_code& ec)
{
	try {
		if(nativeSocket().is_open()) {
			boost::system::error_code ignored_ec;
			nativeSocket().cancel();
			nativeSocket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			nativeSocket().close();
		}
	} catch(boost::system::system_error& e) {
		Log("DEBUG") << __FILE__ << ":" << __LINE__;
		Log("ERROR") << e.what();
	}
}		

/**
 * \brief 作为服务端握手
 */ 
void
SslConnection::async_handshake(std::function<void (boost::system::error_code const&)> handler)
{
	socket_.async_handshake(boost::asio::ssl::stream_base::server, 
		[handler, ptr = shared_from_this()](const boost::system::error_code& e) {
			handler(e);
		}
	);
}
	
void 
SslConnection::async_connect(const std::string& host, const std::string& port,
	std::function<void(ConnectionPtr)> handler)
{
	low_level_connect(host, port, 
		[this, handler, ptr = shared_from_this()](ConnectionPtr conn) {
		if(conn) {
			socket_.set_verify_mode(boost::asio::ssl::verify_peer);
			socket_.set_verify_callback([](bool, boost::asio::ssl::verify_context&) { 
				/** XXX: NOT SAFE */
				return true; 
			});
			socket_.async_handshake(boost::asio::ssl::stream_base::client,
				[=, ptr = ptr](const boost::system::error_code& e) {
					if(e) {
						handler(nullptr);
					} else {
						handler(conn);
					}
				}
			);
		} else {
			Log(__FILE__) << __LINE__;
			handler(nullptr);
		}
	});
}
