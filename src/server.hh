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

/**
 * \brief 
 * 	Server类，服务器主要类
 */ 
class Server {
public:
	/** 不允许复制 */
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	/**
 	 * \biref ctor
 	 * \param config流，内容为json格式的配置信息
 	 * \param thread_pool_size Server内部的线程池大小
 	 */ 
	explicit Server(std::istream& config,
			size_t thread_pool_size = 10);

	/**
 	 * \biref ctor
 	 * \param service 
 	 * 	io_service对象的引用，可由此让Server和其他代码共用一个service
 	 * \param config流，内容为json格式的配置信息
 	 * \param thread_pool_size Server内部的线程池大小
 	 */ 
	explicit Server(boost::asio::io_service& service,
			std::istream& config,
			size_t thread_pool_size = 10);
	~Server();

	/**
 	 * \brief 启动Server
 	 * \param thread_number 分配给io_service的线程数
 	 */ 
	void run(size_t thread_number = 1);

	/** 停止Server */
	void stop();

	/**
 	 * \brief 对感兴趣的路径进行监听
 	 * \param path 目标路径
 	 * \param handler RequestHandler对象的指针
 	 * \example
 	 * 	Server s(conofig);
 	 * 	s.addHandler("/", main_handler);
 	 */ 
	void addHandler(const std::string& path, RequestHandler* handle) {
		request_handler_.addSubHandler(path, handle);
	}

	/**
 	 * \brief 转发请求到request handler.
 	 * \param req 解析完后的请求，其path指向请求路径
 	 */ 
	void deliverRequest(RequestPtr req) {
		auto res = std::make_shared<Response>(req->connection());
		req->discardConnection();
		request_handler_.handleRequest(req, res);
		if(req->keepAlive() && !res->getHeader("Connection"))
			res->addHeader("Connection", "keep-alive");
	}

	/**
 	 * \brief 获取io_service引用(共享service给其他代码使用
 	 * \example
 	 * 	Server s(config);
 	 * 	s.run();
 	 * 	---------另一处的代码----------
 	 * 	Client c(s.service());
 	 * 	c.request(...);
 	 */ 	
	boost::asio::io_service& service() {
		return service_;
	}
	
	/** 
 	 * \brief 加入一个任务到线程池
 	 * \param f 描述任务的函数
 	 * \param args 任务函数的参数
 	 * \return std::future
 	 */ 
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
	void handleRequest(ConnectionPtr req);
};
