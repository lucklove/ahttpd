#include "server.hh"
#include "parser.hh"
#include "TcpConnection.hh"
#include "SslConnection.hh"
#include "log.hh"
#include <thread>
#include <vector>
#include <csignal>
#include <utility>
#include <random>
#include <ctime>
#include <asio/ssl.hpp>
#include <cstring>

namespace {

std::string
generateId(size_t max_size)
{
	std::string ret;
	std::default_random_engine e(time(nullptr));
	std::uniform_int_distribution<char> c('0', '9');
	size_t length = e();
	length %= max_size;
	for(size_t i = 0; i < length; ++i)
		ret.push_back(c(e));
	return ret;
}	

}

Server::Server(asio::io_service& service, 
	const std::string& http_port, const std::string& https_port,
	size_t thread_pool_size)
	: service_(service), signals_(service), 
	tcp_acceptor_(service), ssl_acceptor_(service), request_handler_(this),
	thread_pool_size_(thread_pool_size), thread_pool_(thread_pool_size)
{
	signals_.add(SIGINT);
	signals_.add(SIGTERM);
#if defined(SIGQUIT)
	signals_.add(SIGQUIT);
#endif

	do_await_stop();

	if(http_port != "") {
		asio::ip::tcp::resolver resolver(service_);
		asio::ip::tcp::endpoint endpoint = *resolver.resolve({"0.0.0.0", http_port});
		tcp_acceptor_.open(endpoint.protocol());
		tcp_acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		tcp_acceptor_.bind(endpoint);
		tcp_acceptor_.listen();
		new_tcp_connection_.reset(new TcpConnection(service_));
	}

	if(https_port != "") {
		int sslOptions = asio::ssl::context::default_workarounds
			| asio::ssl::context::no_sslv2
			| asio::ssl::context::single_dh_use;
		ssl_context_ = new asio::ssl::context(service, asio::ssl::context::sslv23);
		ssl_context_->set_options(sslOptions);
		ssl_context_->set_verify_mode(asio::ssl::context::verify_none);
		ssl_context_->load_verify_file("server.csr");
		ssl_context_->use_certificate_chain_file("server.crt");
		ssl_context_->use_private_key_file("server.key",
			 asio::ssl::context::pem);
		ssl_context_->use_tmp_dh_file("server.dh");
		SSL_CTX *native_ctx = ssl_context_->native_handle();
		std::string sessionId = generateId(SSL_MAX_SSL_SESSION_ID_LENGTH);
		SSL_CTX_set_session_id_context(native_ctx,
			reinterpret_cast<const unsigned char *>(sessionId.c_str()), sessionId.size());
		asio::ip::tcp::resolver resolver(service_);
		asio::ip::tcp::endpoint ssl_endpoint = *resolver.resolve({"0.0.0.0", https_port});
		ssl_acceptor_.open(ssl_endpoint.protocol());
		ssl_acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		ssl_acceptor_.bind(ssl_endpoint);
		ssl_acceptor_.listen();
		new_ssl_connection_.reset(new SslConnection(service_, *ssl_context_));
	}

	startAccept();
}

Server::~Server() {}

void 
Server::run(size_t thread_number)
{
	if(!thread_number)
		return;
	std::vector<std::thread> threads;
	for(size_t i = 0; i < thread_number; ++i) {
		threads.push_back(std::thread([this] {
			service_.run();
		}));
	}
	for(auto&& th : threads)
		th.join();
}

void
Server::handleRequest(RequestPtr req)
{
	assert(req->connection() != nullptr);
	parseRequest(req, [this](RequestPtr req, bool good) {
		if(good) {
			if(req->keepAlive()) {
				RequestPtr new_req = std::make_shared<Request>(req->connection());
				handleRequest(new_req);
			}
			thread_pool_.wait_to_enqueue([this](auto&&) {
				if(thread_pool_.size_unlocked() > thread_pool_size_)
					Log("WARNING") << "thread pool overload";
			}, &Server::deliverRequest, this, req);
		} else {
			req->connection()->stop();
			req->discardConnection();
		}
	});
}

void 
Server::handleTcpAccept(const asio::error_code& ec)
{
	if(!ec) {
		RequestPtr req = std::make_shared<Request>(new_tcp_connection_);
		handleRequest(req);
		new_tcp_connection_.reset(new TcpConnection(service_));
		tcp_acceptor_.async_accept(new_tcp_connection_->nativeSocket(),
			std::bind(&Server::handleTcpAccept, 
				this, std::placeholders::_1));
	} else {
		if(ec != asio::error::operation_aborted)
			Log("ERROR") << ec.message();
	}
}

void 
Server::handleSslAccept(const asio::error_code& ec)
{
	if(!ec) {
		new_ssl_connection_->async_handshake([this](const asio::error_code& e) {
			if(e) {
				Log("ERROR") << e.message();
			} else {
				RequestPtr req = std::make_shared<Request>(new_ssl_connection_);
				handleRequest(req);
			}
			new_ssl_connection_.reset(new SslConnection(service_, *ssl_context_));
			ssl_acceptor_.async_accept(new_ssl_connection_->nativeSocket(),
				std::bind(&Server::handleSslAccept, this, std::placeholders::_1));
		});
	} else {
		if(ec != asio::error::operation_aborted)
			Log("ERROR") << ec.message();
	}
}
			
void 
Server::startAccept()
{
	if(new_tcp_connection_) {
		tcp_acceptor_.async_accept(new_tcp_connection_->nativeSocket(),
			std::bind(&Server::handleTcpAccept, this, std::placeholders::_1));
	}
	if(new_ssl_connection_) {
		ssl_acceptor_.async_accept(new_ssl_connection_->nativeSocket(),
			std::bind(&Server::handleSslAccept, this, std::placeholders::_1));
	}
}	

void 
Server::do_await_stop()
{
	signals_.async_wait([this](asio::error_code /*ec*/, int /*signo*/) {
		Log("NOTE") << "BYE";
		service_.stop();		/**< XXX:简单粗暴 */
      	});
}
