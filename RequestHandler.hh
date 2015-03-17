#pragma once

#include <string>
#include <memory>
#include <vector>

#include "request.hh"
#include "response.hh"

class Server;
class RequestHandler;

using RequestHandlerPtr = std::shared_ptr<RequestHandler>;

/**
 * \brief 请求处理类，处理Request, 生成Resposne, 后则被返回给客户端
 * \note USER API 
 */
class RequestHandler {
public:
	RequestHandler(const RequestHandler&) = delete;
	RequestHandler& operator=(const RequestHandler&) = delete;
	explicit RequestHandler(Server *server);
	virtual ~RequestHandler() {}

	/**
 	 * \brief Request处理函数
 	 * \param req 解析后的Request
 	 * \param res 将被发送的Response, 对其进行操作，以发送不同的响应
 	 */
	virtual void handleRequest(RequestPtr req, ResponsePtr res);

	/**
 	 * \brief 重新将Request提交给其他RequestHandler处理
 	 * \param req 将被提交(一般对uri做了修改)的Request
 	 * \param res 响应(也许已经发送了一部分)
 	 */ 
	bool deliverRequest(RequestPtr req, ResponsePtr res);

	/**
 	 * \brief 添加子handler, 由deliverRequest使用
 	 * \param path 子handler所处路径
 	 */
	void addSubHandler(const std::string& path, RequestHandlerPtr handler) {
		sub_handlers_.push_back(std::make_tuple(path, handler));
	}

	Server* server() { return server_; }
private:
	Server *server_;

	std::vector<std::tuple<const std::string, RequestHandlerPtr>> sub_handlers_;

	static bool url_decode(const std::string& in, std::string& out);
};
