#include "base64.hh"
#include "mail.hh"
#include "TcpConnection.hh"
#include "SslConnection.hh"
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include <sstream>
#include <iostream>

namespace {

static int
response_code(boost::asio::streambuf& buf)
{
	std::stringstream ss;
	ss << &buf;
	int code;
	ss >> code;
	return code;
}

void 
step(const std::string& message, 
	int expected_code,
	ConnectionPtr conn, 
	std::function<void(bool)> handler) 
{				
	conn->async_write(message, [=](const boost::system::error_code& ec, size_t) {
		if(!ec) {										
			conn->async_read_until("\n", [=](const boost::system::error_code& ec, size_t) {	
				if(!ec) {
					handler(response_code(conn->readBuffer()) == expected_code);
				} else {								
					Log("DEBUG") << __FILE__ << ":" << __LINE__;			
					Log("ERROR") << ec.message();					
					handler(false);							
				}									
			});										
		} else {										
			Log("DEBUG") << __LINE__;
			Log("ERROR") << ec.message();					
			handler(false);									
		}											
	});
}

}


Mail::Mail(boost::asio::io_service& io_service, const std::string& username, 
	const std::string& password, const std::string& server, const std::string& port, bool use_ssl)
	: service_(io_service), username_(username), password_(password), server_(server), port_(port)
{
	if(use_ssl) {
		ssl_context_ = new boost::asio::ssl::context(boost::asio::ssl::context::sslv23);
		ssl_context_->set_default_verify_paths();
	}
}

Mail::Mail(const std::string& username, const std::string& password, 
	const std::string& server, const std::string& port, bool use_ssl)
	: Mail(*(new boost::asio::io_service()), username, password, server, port, use_ssl)
{
	service_holder_.reset(&service_);
}

Mail::~Mail()
{
	if(ssl_context_)
		delete ssl_context_;
}

void
Mail::apply()
{
	service_.run();
	service_.reset();
}

#define CHECK(cond)						\
do {								\
	if(!(cond)) {						\
		Log("DEBUG") << __FILE__ << ":" << __LINE__;	\
		handler(false);					\
		return;						\
	}							\
} while(0)

void
Mail::send(const std::string& to_addr, const std::string& subject, 
	const std::string& body, std::function<void(bool)> handler)
{
	ConnectionPtr conn;
	CHECK(username_.find('@') != username_.npos);
	if(ssl_context_) {
		conn = std::make_shared<SslConnection>(service_, *ssl_context_);
	} else {
		conn = std::make_shared<TcpConnection>(service_);
	}
	conn->async_connect(server_, port_, [=](ConnectionPtr conn) {
		CHECK(conn);
		conn->async_read_until("\n", [=](const boost::system::error_code& ec, size_t) {
			if(ec) {
				Log("DEBUG") << __LINE__;
				Log("ERROR") << ec.message();
				handler(false);
				return;
			}
			conn->async_read_until("\n", [=](const boost::system::error_code& ec, size_t) {
				CHECK(response_code(conn->readBuffer()) == 220);
				sayHello(conn, [=](bool good) {
					CHECK(good);
					rcptTo(to_addr, conn, [=](bool good) {
						CHECK(good);
						sendData(subject, to_addr, body, conn, [=](bool good) {
							conn->stop();
							handler(good);
						});
					});
				});
			});
		});
	});
}
						

void
Mail::sayHello(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("HELO " + server_ + "\r\n", 250, conn, [=](bool good) {
		if(good) {
			loginRequest(conn, handler);
		} else {
			handler(false);
		}
	});
}

void
Mail::loginRequest(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("AUTH LOGIN\r\n", 334, conn, [=](bool good) {
		if(good) {
			sendUser(conn, handler);
		} else {
			handler(false);
		}
	});
}

void
Mail::sendUser(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step(Base64::encode(username_.substr(0, username_.find('@'))) + "\r\n", 
			334, conn, [=](bool good) {
		if(good) {
			sendPass(conn, handler);
		} else {
			handler(false);
		}
	});
}

void
Mail::sendPass(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step(Base64::encode(password_) + "\r\n", 235, conn, [=](bool good) {
		if(good) {
			mailFrom(conn, handler);
		} else {
			handler(false);
		}
	});
}

void
Mail::mailFrom(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("MAIL FROM: <" + username_ + ">\r\n", 250, conn, handler);
}

void
Mail::rcptTo(const std::string& to_addr, ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("RCPT TO: <" + to_addr + ">\r\n", 250, conn, handler);
}

void
Mail::sendData(const std::string& subject, const std::string& to_addr, 
	const std::string& body, ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("DATA\r\n", 354, conn, [=](bool good) {
		if(good) {
			step("subject: " + subject + "\r\n" + "from: " + username_ + "\r\n" +
				+ "to: " + to_addr + "\r\n\r\n" + body + "\r\n.\r\n", 250, conn, handler);
		} else {
			handler(false);
		}
	});
}
