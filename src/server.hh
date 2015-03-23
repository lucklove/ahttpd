#pragma once

#include <asio.hpp>
#include <string>
#include <istream>
#include "RequestHandler.hh"
#include "request.hh"
#include "response.hh"
#include "ThreadPool.hh"
#include "ptrs.hh"

namespace asio {
namespace ssl {
class context; 
}
}

class Server {
public:
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	explicit Server(asio::io_service& service,
			const std::string& http_port = "",
			const std::string& https_port = "",
			size_t thread_pool_size = 10);
	~Server();
	void run(size_t thread_number = 1);

	void addHandler(const std::string& path, RequestHandler* handle) {
		request_handler_.addSubHandler(path, handle);
	}

	void deliverRequest(RequestPtr req, ResponsePtr rep) {
		request_handler_.handleRequest(req, rep);
	}

	asio::io_service& service() {
		return service_;
	}
	
	void post(const std::function<void(void)>& func) {
		service_.post(func);
	}
	
	template<typename _fCallable, typename... _tParams>
	auto enqueue(_fCallable&& f, _tParams&&... args) {
		return thread_pool_.enqueue(std::forward<_fCallable>(f), std::forward<_tParams>(args)...);
	}
private:
	asio::io_service& service_;

	asio::signal_set signals_;

	asio::ip::tcp::acceptor tcp_acceptor_;

	asio::ip::tcp::acceptor ssl_acceptor_;

	RequestHandler request_handler_;

	TcpConnectionPtr new_tcp_connection_;

	SslConnectionPtr new_ssl_connection_;

	asio::ssl::context* ssl_context_;

	size_t thread_pool_size_;
	
	ThreadPool thread_pool_;

	void startAccept();
	void do_await_stop();
	void handleTcpAccept(const asio::error_code& ec);
	void handleSslAccept(const asio::error_code& ec);
	void handleRequest(RequestPtr req);
};
