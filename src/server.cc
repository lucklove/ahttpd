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
#include <boost/asio/ssl.hpp>
#include <cstring>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace {

using boost::property_tree::read_json;
using boost::property_tree::ptree;

template<typename T>
T
parse_config(const ptree& t, const std::string& key, 
	std::function<void(std::exception& e, T&)> exception_callback = 
	[](std::exception& e, T& retval) {
		retval = T{};
	})
{
	T ret{};
	try {
		ret = t.get<T>(key);
	} catch(std::exception& e) {
		exception_callback(e, ret);
	}
	return ret;
}

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

class ServerImpl {
	friend class Server;
public:
	ServerImpl(boost::asio::io_service& service) 
		: service_(service), signals_(service), tcp_acceptor_(service), 
		ssl_acceptor_(service), ssl_context_(boost::asio::ssl::context::sslv23)
	{}
private:
	boost::asio::io_service& service_;
	boost::asio::signal_set signals_;
	boost::asio::ip::tcp::acceptor tcp_acceptor_;
	boost::asio::ip::tcp::acceptor ssl_acceptor_;
	boost::asio::ssl::context ssl_context_;
	TcpConnectionPtr new_tcp_connection_;
	SslConnectionPtr new_ssl_connection_;

	void handleTcpAccept(std::function<void(RequestPtr)> request_handler, const boost::system::error_code& ec);
	void handleSslAccept(std::function<void(RequestPtr)> request_handler, const boost::system::error_code& ec);
};

void 
ServerImpl::handleTcpAccept(std::function<void(RequestPtr)> request_handler, const boost::system::error_code& ec)
{
	if(!ec) {
		RequestPtr req = std::make_shared<Request>(new_tcp_connection_);
		request_handler(req);
		new_tcp_connection_.reset(new TcpConnection(service_));
		tcp_acceptor_.async_accept(new_tcp_connection_->nativeSocket(),
			std::bind(&ServerImpl::handleTcpAccept, this, 
			request_handler, std::placeholders::_1));
	} else {
		if(ec != boost::asio::error::operation_aborted)
			Log("ERROR") << ec.message();
	}
}

void 
ServerImpl::handleSslAccept(std::function<void(RequestPtr)> request_handler, const boost::system::error_code& ec)
{
	if(!ec) {
			new_ssl_connection_->async_handshake([=](const boost::system::error_code& e) {
			if(e) {
				Log("ERROR") << e.message();
			} else {
				RequestPtr req = std::make_shared<Request>(new_ssl_connection_);
				request_handler(req);
			}
			new_ssl_connection_.reset(new SslConnection(service_, ssl_context_));
			ssl_acceptor_.async_accept(new_ssl_connection_->nativeSocket(),
				std::bind(&ServerImpl::handleSslAccept, this, 
				request_handler, std::placeholders::_1));
		});
	} else {
		if(ec != boost::asio::error::operation_aborted)
			Log("ERROR") << ec.message();
	}
}
			
Server::Server(std::istream& config, size_t thread_pool_size)
	: Server(*(new boost::asio::io_service()), config, thread_pool_size)
{
	service_holder_.reset(&service_);	
}

Server::Server(boost::asio::io_service& service, std::istream& config, size_t thread_pool_size)
	: pimpl_(std::make_shared<ServerImpl>(service)), service_(service),
	thread_pool_size_(thread_pool_size), thread_pool_(thread_pool_size)
{
	pimpl_->signals_.add(SIGINT);
	pimpl_->signals_.add(SIGTERM);
#if defined(SIGQUIT)
	pimpl_->signals_.add(SIGQUIT);
#endif

	do_await_stop();
	ptree conf;
	read_json(config, conf);
	std::string http_port = parse_config<std::string>(conf, "http port",
		[](std::exception&, std::string&) {
			Log("NOTE") << "no http port provide in config, disable http";
		}
	);
	std::string https_port = parse_config<std::string>(conf, "https port",
		[](std::exception&, std::string&) {
			Log("NOTE") << "no https port provide in config, disable https";
		}
	);
	if(http_port != "") {
		std::string http_server = parse_config<std::string>(conf, "http server",
			[](std::exception&, std::string& server) {
				server = "0.0.0.0";
			}
		);
		boost::asio::ip::tcp::resolver resolver(service_);
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({http_server, http_port});
		pimpl_->tcp_acceptor_.open(endpoint.protocol());
		pimpl_->tcp_acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		pimpl_->tcp_acceptor_.bind(endpoint);
		pimpl_->tcp_acceptor_.listen();
		pimpl_->new_tcp_connection_.reset(new TcpConnection(service_));
	}

	if(https_port != "") {
		int sslOptions = boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use;
		pimpl_->ssl_context_.set_options(sslOptions);
		pimpl_->ssl_context_.set_verify_mode(boost::asio::ssl::context::verify_none);
		pimpl_->ssl_context_.load_verify_file(parse_config<std::string>(conf, "verify file",
			[](std::exception& e, std::string&) {
				Log("ERROR") << "can't peek verify file path in config";
				Log("ERROR") << e.what();
				abort();
			}
		)); //csr
		pimpl_->ssl_context_.use_certificate_chain_file(parse_config<std::string>(conf, "certificate chain file",
			[](std::exception& e, std::string&) {
				Log("ERROR") << "can't peek certificate chain file path in config";
				Log("ERROR") << e.what();
				abort();
			}
		));//crt
		pimpl_->ssl_context_.use_private_key_file(parse_config<std::string>(conf, "private key",
			[](std::exception& e, std::string&) {
				Log("ERROR") << "can't peek certificate private key path in config";
				Log("ERROR") << e.what();
				abort();
			}
		), boost::asio::ssl::context::pem);
		pimpl_->ssl_context_.use_tmp_dh_file(parse_config<std::string>(conf, "tmp dh file",
			[](std::exception& e, std::string&) {
				Log("ERROR") << "can't peek tmp dh file path in config";
				Log("ERROR") << e.what();
				abort();
			}
		));
		SSL_CTX *native_ctx = pimpl_->ssl_context_.native_handle();
		std::string sessionId = generateId(SSL_MAX_SSL_SESSION_ID_LENGTH);
		SSL_CTX_set_session_id_context(native_ctx,
			reinterpret_cast<const unsigned char *>(sessionId.c_str()), sessionId.size());
		std::string https_server = parse_config<std::string>(conf, "https server",
			[](std::exception&, std::string& server) {
				server = "0.0.0.0";
			}
		);
		boost::asio::ip::tcp::resolver resolver(service_);
		boost::asio::ip::tcp::endpoint ssl_endpoint = *resolver.resolve({https_server, https_port});
		pimpl_->ssl_acceptor_.open(ssl_endpoint.protocol());
		pimpl_->ssl_acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		pimpl_->ssl_acceptor_.bind(ssl_endpoint);
		pimpl_->ssl_acceptor_.listen();
		pimpl_->new_ssl_connection_.reset(new SslConnection(service_, pimpl_->ssl_context_));
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
	service_.reset();
}

void
Server::stop()
{
	Log("NOTE") << "BYE";
	service_.stop();		/**< XXX:简单粗暴 */
}

void
Server::handleRequest(RequestPtr req)
{
	assert(req->connection() != nullptr);
	parseRequest(req, [=](RequestPtr request) {
		if(request) {
			if(request->keepAlive()) {
				RequestPtr new_req = std::make_shared<Request>(request->connection());
				handleRequest(new_req);
			}
			thread_pool_.wait_to_enqueue([this](std::unique_lock<std::mutex>&) {
				if(thread_pool_.size_unlocked() > thread_pool_size_)
					Log("WARNING") << "thread pool overload";
			}, &Server::deliverRequest, this, request);
		} else {
			req->connection()->stop();
			req->discardConnection();
		}
	});
}

void 
Server::startAccept()
{
	if(pimpl_->new_tcp_connection_) {
		pimpl_->tcp_acceptor_.async_accept(pimpl_->new_tcp_connection_->nativeSocket(),
			[this](const boost::system::error_code& ec) {
				pimpl_->handleTcpAccept(
					std::bind(&Server::handleRequest, this, std::placeholders::_1),
					ec
				);
			}
		);
	}
	if(pimpl_->new_ssl_connection_) {
		pimpl_->ssl_acceptor_.async_accept(pimpl_->new_ssl_connection_->nativeSocket(),
			[this](const boost::system::error_code& ec) {
				pimpl_->handleSslAccept(
					std::bind(&Server::handleRequest, this, std::placeholders::_1),
					ec
				);
			}
		);
	}
}	

void 
Server::do_await_stop()
{
	pimpl_->signals_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/) {
		stop();
      	});
}
