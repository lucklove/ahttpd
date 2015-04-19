#pragma once

#include <string>
#include <functional>
#include <memory>
#include "ptrs.hh"

namespace boost { namespace asio { namespace ssl {
class context;
}
class io_service;
}
}

class Mail {
public:
	Mail(boost::asio::io_service& io_service, const std::string& username, 
		const std::string& password, const std::string& server, 
		const std::string& port = "smtp", bool use_ssl = false);
	Mail(const std::string& username, const std::string& password, const std::string& server,
		const std::string& port = "smtp", bool use_ssl = false);
	~Mail();

	void apply();
	void send(const std::string& to_addr, const std::string& subject, const std::string& body,
		std::function<void(bool)> handler = [](bool){});
private:
	boost::asio::io_service& service_;
	std::shared_ptr<boost::asio::io_service> service_holder_;
	boost::asio::ssl::context* ssl_context_ = nullptr;
	std::string username_;
	std::string password_;
	std::string server_;
	std::string port_;
	void sayHello(ConnectionPtr conn, std::function<void(bool)> handler);
	void loginRequest(ConnectionPtr conn, std::function<void(bool)> handler);
	void sendUser(ConnectionPtr conn, std::function<void(bool)> handler);
	void sendPass(ConnectionPtr conn, std::function<void(bool)> handler);
	void mailFrom(ConnectionPtr conn, std::function<void(bool)> handler);
	void rcptTo(const std::string& to_addr, ConnectionPtr conn, std::function<void(bool)> handler);
	void sendData(const std::string& subject, const std::string& to_addr,
		const std::string& body, ConnectionPtr conn, std::function<void(bool)> handler);	
};
/*
bool sendMail(const std::string& from, const std::string& to,
	const std::string& password, const std::string& server,
	const std::string& port, const std::string& subject, const std::string& body);*/
