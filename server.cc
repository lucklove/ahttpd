// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#include "server.hh"
#include "parser.hh"
#include <csignal>
#include <utility>
#include <iostream>

Server::Server(boost::asio::io_service& service, const std::string& port)
	: service_(service), signals_(service), acceptor_(service), 
	 socket_(service), request_handler_(this)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
	signals_.add(SIGINT);
	signals_.add(SIGTERM);
#if defined(SIGQUIT)
	signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

	do_await_stop();

	// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver(service_);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({"127.0.0.1", port});
	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();

	do_accept();
}

void 
Server::run()
{
  	service_.run();
}

void 
Server::do_accept()
{
	std::cout << "添加accept..." << std::endl;
	acceptor_.async_accept(socket_,
		[this](boost::system::error_code ec) {
			if(!acceptor_.is_open())
				return;
			std::cout << "accepted" << std::endl;	
		       	if(!ec) {
				std::cout << "connection start" << std::endl;
				ConnectionPtr conn = 
					std::make_shared<Connection>(
						std::move(socket_)
					);
				RequestPtr req = 
					std::make_shared<Request>(this, conn);
				parseRequest(req, [](RequestPtr req, bool good) {
					if(good) {
						std::cout << "deliverSelf" << std::endl;
						req->deliverSelf();
					} else {
						std::cout << "not good" << std::endl;
						req->connection()->stop();
					}
				});
			}
/*			connection_manager_.start(
				std::make_shared<Connection>(
		              	std::move(socket_), 
				connection_manager_, 
		              	request_handler_, this));
*/
	        	do_accept();
	        }
	);
}

void 
Server::do_await_stop()
{
	signals_.async_wait(
      		[this](boost::system::error_code /*ec*/, int /*signo*/) {
        		acceptor_.close();
			std::cout << "关闭中..." << std::endl;
      		}
	);
}
