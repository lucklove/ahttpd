// Server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include "connection.hh"
#include "ConnectionManager.hh"
#include "RequestHandler.hh"
#include "request.hh"
#include "reply.hh"

namespace http {
namespace server {

/// The top-level class of the HTTP Server.
class Server {
public:
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	/// Construct the Server to listen on the specified TCP address and port, and
	/// serve up files from the given directory.
	explicit Server(const std::string& address, const std::string& port);

	/// Run the Server's io_service loop.
	void run();

	void addHandler(const std::string& path, RequestHandlerPtr handle) {
		request_handler_.addSubHandler(path, handle);
	}

	void reDeliverRequest(Request& req, reply& rep) {
		request_handler_.handleRequest(req, rep);
	}	
private:

	/// Perform an asynchronous accept operation.
	void do_accept();

	/// Wait for a request to stop the Server.
	void do_await_stop();

	/// The io_service used to perform asynchronous operations.
	boost::asio::io_service io_service_;

	/// The signal_set is used to register for process termination notifications.
	boost::asio::signal_set signals_;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor acceptor_;

	/// The connection manager which owns all live connections.
	ConnectionManager connection_manager_;

	/// The next socket to be accepted.
	boost::asio::ip::tcp::socket socket_;

	/// The handler for all incoming requests.
	RequestHandler request_handler_;
};

} // namespace Server
} // namespace http

#endif // HTTP_SERVER_HPP
