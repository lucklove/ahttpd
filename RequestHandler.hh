#pragma once

#include <string>
#include <memory>
#include <vector>

#include "request.hh"
#include "response.hh"

struct Response;
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

  /// Handle a request and produce a response.
  virtual void handleRequest(RequestPtr req, ResponsePtr rep);
  bool deliverRequest(RequestPtr req, ResponsePtr rep);
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
