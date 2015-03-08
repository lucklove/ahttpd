// RequestHandler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#include "RequestHandler.hh"
#include <fstream>
#include <sstream>
#include <string>
#include "MimeTypes.hh"
#include "reply.hh"
#include "request.hh"
#include "server.hh"

#include <iostream>

namespace http {
namespace server {

RequestHandler::RequestHandler(Server *server)
	: server_(server)
{
}

void 
RequestHandler::handleRequest(Request& req, reply& rep)
{
  	// Decode url to path.
	std::string request_path;
 
	if(!url_decode(req.uri, request_path)) {
 		rep = reply::stock_reply(reply::bad_request);
    		return;
  	}

  	if(!deliverRequest(req, rep)) {
		rep = reply::stock_reply(reply::not_found);
		return;
  	}
}

bool
RequestHandler::deliverRequest(Request& req, reply& rep)
{
	std::tuple<std::string, RequestHandlerPtr> best;
	
	for(auto handler : sub_handlers_) {
		size_t cmp_size = std::get<0>(handler).size();
		if(req.uri.size() >= cmp_size && 
			req.uri.substr(0, cmp_size) == std::get<0>(handler))
			best = handler;	
	}

	if(std::get<1>(best) == nullptr)
		return false;

	std::get<1>(best)->handleRequest(req, rep);
	return true;
}

bool 
RequestHandler::url_decode(const std::string& in, std::string& out)
{
	out.clear();
	out.reserve(in.size());
	for(std::size_t i = 0; i < in.size(); ++i) {
		if(in[i] == '%') {
      			if(i + 3 <= in.size()) {
        			int value = 0;
        			std::istringstream is(in.substr(i + 1, 2));
        			if(is >> std::hex >> value) {
          				out += static_cast<char>(value);
          				i += 2;
        			} else {
          				return false;
        			}
      			} else {
        			return false;
      			}
    		} else if (in[i] == '+') {
      			out += ' ';
    		} else {
      			out += in[i];
    		}
  	}
  	return true;
}

} // namespace server
} // namespace http
