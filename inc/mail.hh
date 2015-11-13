#pragma once

#include <string>
#include <functional>
#include <memory>
#include "ptrs.hh"
#include "package.hh"

namespace ahttpd {

class MailPkg : public Package {
public:
	MailPkg(ConnectionPtr conn) : Package(conn) {}
	void flush() {
		flushPackage();
	}
	~MailPkg();
};

/**
 * \brief 邮件类，用于通过smtp发送邮件
 */ 
class Mail {
public:
	/**
 	 * \brief ctor
 	 * \param io_service 用于和其他组件共享同一个io_service
 	 * \param username 邮件用户名, 如xxx@example.com
 	 * \param server smtp服务器，如smtp.example.com
 	 * \param port smtp服务器端口
 	 * \param use_ssl 是否使用ssl，默认为false
 	 */ 
	Mail(boost::asio::io_service& io_service, const std::string& username, 
		const std::string& server, const std::string& port = "smtp", bool use_ssl = false);

	/**
 	 * \brief ctor
 	 * \param username 邮件用户名, 如xxx@example.com
 	 * \param server smtp服务器，如smtp.example.com
 	 * \param port smtp服务器端口
 	 * \param use_ssl 是否使用ssl，默认为false
 	 */ 
	Mail(const std::string& username, const std::string& server, 
		const std::string& port = "smtp", bool use_ssl = false);

	Mail& pass(const std::string& password) {
		password_ = password;
		return *this;
	}

	/**
 	 * \biref dtor
 	 */
	~Mail();

	/**
 	 * \brief 执行io_service::run
 	 * \note 若和其他组件共用io_service则可在其他地方执行io_service::run而不必调用此函数
 	 */ 
	void apply();

	/**
 	 * \brief 异步发送邮件
 	 * \param to_addr 对端邮件地址，如xxx@example.com
 	 * \param subject 邮件主题
 	 * \param body 邮件主体
 	 * \param handler 异步handler，若成功则以true为参数调用handler，否则false
 	 */ 
	Mail& send(const std::string& to_addr, 
		std::function<void(MailPkgPtr)> send_handler,
		std::function<void(bool)> result_handler = [](bool){});


private:
	boost::asio::io_service& service_;
	std::shared_ptr<boost::asio::io_service> service_holder_;
	boost::asio::ssl::context* ssl_context_ = nullptr;
	std::string username_{};
	std::string password_{};
	std::string server_;
	std::string port_;
	void sayHello(ConnectionPtr conn, std::function<void(bool)> handler);
	void loginRequest(ConnectionPtr conn, std::function<void(bool)> handler);
	void sendUser(ConnectionPtr conn, std::function<void(bool)> handler);
	void sendPass(ConnectionPtr conn, std::function<void(bool)> handler);
	void mailFrom(ConnectionPtr conn, std::function<void(bool)> handler);
	void rcptTo(const std::string& to_addr, ConnectionPtr conn, std::function<void(bool)> handler);
	void sendData(ConnectionPtr conn, 
		std::function<void(MailPkgPtr)> send_handler,
		std::function<void(bool)> handler);	
};

}	/**< namespace ahttpd */
