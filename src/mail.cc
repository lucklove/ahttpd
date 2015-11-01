#include "base64.hh"
#include "mail.hh"
#include "TcpConnection.hh"
#include "SslConnection.hh"
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include <sstream>
#include <iostream>

namespace ahttpd 
{

namespace 
{

static int
check_response_code(boost::asio::streambuf& buf, int expected_code)
{
	std::stringstream ss;
	ss << &buf;
	int code;
	ss >> code;
	if(code != expected_code) {
		Log("ERROR") << ss.str();
		return false;
	}
	return true;
}

void 
step(const std::string& message, 
	int expected_code,
	ConnectionPtr conn, 
	std::function<void(bool)> handler) 
{				
	conn->asyncWrite(message, [=](const boost::system::error_code& ec, size_t) 
    {
		if(!ec) {										
			conn->asyncReadUntil("\n", [=](const boost::system::error_code& ec, size_t) 
            {	
				if(!ec) 
                {
					handler(check_response_code(conn->readBuffer(), expected_code));
				} 
                else 
                {								
					Log("DEBUG") << __FILE__ << ":" << __LINE__;			
					Log("ERROR") << ec.message();					
					handler(false);							
				}									
			});										
		} 
        else 
        {										
			Log("DEBUG") << __FILE__ << ":" << __LINE__;
			Log("ERROR") << ec.message();					
			handler(false);									
		}											
	});
}

}

MailPkg::~MailPkg() 
{
	if(!connection())
		return;
	flushPackage();
	connection()->asyncWrite("\r\n.\r\n");	
};
	
Mail::Mail(boost::asio::io_service& io_service, const std::string& username,
	const std::string& server, const std::string& port, bool use_ssl)
	: service_(io_service), username_(username), server_(server), port_(port)
{
	if(use_ssl) 
    {
		ssl_context_ = new boost::asio::ssl::context(boost::asio::ssl::context::sslv23);
		ssl_context_->set_default_verify_paths();
	}
}

Mail::Mail(const std::string& username, const std::string& server, const std::string& port, bool use_ssl)
	: Mail(*(new boost::asio::io_service()), username, server, port, use_ssl)
{
	service_holder_.reset(&service_);
}

Mail::~Mail()
{
	if(ssl_context_)
		delete ssl_context_;
}

void Mail::apply()
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

Mail& Mail::send(const std::string& to_addr, 
	std::function<void(MailPkgPtr)> send_handler, 
	std::function<void(bool)> handler)
{
	ConnectionPtr conn;
	if(ssl_context_) 
    {
		conn = std::make_shared<SslConnection>(service_, *ssl_context_);
	} 
    else 
    {
		conn = std::make_shared<TcpConnection>(service_);
	}

	conn->asyncConnect(server_, port_, [=](ConnectionPtr conn) 
    {
		CHECK(conn);
		conn->asyncReadUntil("\n", [=](const boost::system::error_code& ec, size_t) 
        {
			if(ec) 
            {
				Log("DEBUG") << __FILE__ << ":" << __LINE__;
				Log("ERROR") << ec.message();
				handler(false);
				return;
			}
			conn->asyncReadUntil("\n", [=](const boost::system::error_code& ec, size_t) 
            {
				CHECK(check_response_code(conn->readBuffer(), 220));
				sayHello(conn, [=](bool good) 
                {
					CHECK(good);
					rcptTo(to_addr, conn, [=](bool good) 
                    {
						CHECK(good);
						sendData(conn, send_handler, [=](bool good) 
                        {
							CHECK(good);
							step("", 250, conn, handler);
						});
					});
				});
			});
		});
	});
	return *this;
}
						
void Mail::sayHello(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("HELO " + server_ + "\r\n", 250, conn, [=](bool good) 
    {
		if(good) 
        {
			if(password_.size()) 
            {
				loginRequest(conn, handler);
			} 
            else 
            {
				mailFrom(conn, handler);	
			}
		} 
        else 
        {
			handler(false);
		}
	});
}

void Mail::loginRequest(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("AUTH LOGIN\r\n", 334, conn, [=](bool good) {
		if(good) {
			sendUser(conn, handler);
		} else {
			handler(false);
		}
	});
}

void Mail::sendUser(ConnectionPtr conn, std::function<void(bool)> handler)
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

void Mail::sendPass(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step(Base64::encode(password_) + "\r\n", 235, conn, [=](bool good) {
		if(good) {
			mailFrom(conn, handler);
		} else {
			handler(false);
		}
	});
}

void Mail::mailFrom(ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("MAIL FROM: <" + username_ + ">\r\n", 250, conn, handler);
}

void Mail::rcptTo(const std::string& to_addr, ConnectionPtr conn, std::function<void(bool)> handler)
{
	step("RCPT TO: <" + to_addr + ">\r\n", 250, conn, handler);
}

void Mail::sendData(ConnectionPtr conn, 
	std::function<void(MailPkgPtr)> send_handler,
	std::function<void(bool)> res_handler)
{
	step("DATA\r\n", 354, conn, [=](bool good) {
		if(good) {
			auto mpkg = std::make_shared<MailPkg>(conn);
			send_handler(mpkg);
			res_handler(true);
		} else {
			res_handler(false);
		}
	});
}

}	/**< namespace ahttpd */
