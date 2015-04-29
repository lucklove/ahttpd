#pragma once

#include <string>
#include <istream>
#include "RequestHandler.hh"
#include "request.hh"
#include "response.hh"
#include "ThreadPool.hh"
#include "ptrs.hh"

namespace boost { namespace asio {
class io_service;
}
}

class ServerImpl;

class Server {
public:
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	explicit Server(std::istream& config,
			size_t thread_pool_size = 10);

	explicit Server(boost::asio::io_service& service,
			std::istream& config,
			size_t thread_pool_size = 10);
	~Server();

	void run(size_t thread_number = 1);

	void stop();

	void addHandler(const std::string& path, RequestHandler* handle) {
		handle->setServer(this);
		request_handler_.addSubHandler(path, handle);
	}

	void deliverRequest(RequestPtr req) {
		auto res = std::make_shared<Response>(req->connection());
		req->discardConnection();
		if(req->keepAlive())
			res->addHeader("Connection", "Keep-alive");
		request_handler_.handleRequest(req, res);
	}

	boost::asio::io_service& service() {
		return service_;
	}
	
	template<typename _fCallable, typename... _tParams>
	auto enqueue(_fCallable&& f, _tParams&&... args) {
		return thread_pool_.enqueue(std::forward<_fCallable>(f), std::forward<_tParams>(args)...);
	}
private:
	std::shared_ptr<ServerImpl> pimpl_;
	boost::asio::io_service& service_;
	std::shared_ptr<boost::asio::io_service> service_holder_;
	RequestHandler request_handler_;
	size_t thread_pool_size_;
	ThreadPool thread_pool_;
	void startAccept();
	void do_await_stop();
	void handleRequest(RequestPtr req);
};
