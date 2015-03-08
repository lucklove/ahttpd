// RequestHandler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>
#include <memory>
#include <vector>

namespace http {
namespace server {

struct reply;
struct Request;
class Server;
class RequestHandler;

using RequestHandlerPtr = std::shared_ptr<RequestHandler>;

/// The common handler for all incoming requests.
class RequestHandler
{
public:
  RequestHandler(const RequestHandler&) = delete;
  RequestHandler& operator=(const RequestHandler&) = delete;

  explicit RequestHandler(Server *server);

  /// Handle a request and produce a reply.
  virtual void handleRequest(Request& req, reply& rep);
  bool deliverRequest(Request& req, reply& rep);
  void addSubHandler(const std::string& path, RequestHandlerPtr handler) {
	sub_handlers_.push_back(std::make_tuple(path, handler));
  }

private:
  Server *server_;

  std::vector<std::tuple<const std::string, RequestHandlerPtr>> sub_handlers_;
  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);

};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP
