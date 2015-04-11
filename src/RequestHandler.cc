#include "RequestHandler.hh"
#include "response.hh"
#include "request.hh"
#include "server.hh"
#include "log.hh"
#include "connection.hh"
#include <fstream>
#include <sstream>
#include <string>

RequestHandler::RequestHandler(Server *server)
	: server_(server)
{}

void 
RequestHandler::handleRequest(RequestPtr req, ResponsePtr rep)
{
	if(url_decode(req->path()) && url_decode(req->query())) {
		try {
	 	 	if(!deliverRequest(req, rep))
				rep->status() = Response::not_found;
		} catch(std::exception& e) {
			Log("WARNING") << "EXCEPTION FROM USER HANDLER: " << e.what();
		}
  	} else {
		Log("WARNING") << "Bad request";
		rep->status() = Response::not_found;
	}
}

bool
RequestHandler::deliverRequest(RequestPtr req, ResponsePtr rep)
{
	std::tuple<std::string, RequestHandler*> best;

	for(auto handler : sub_handlers_) {
		size_t cmp_size = std::get<0>(handler).size();
		if(req->path().size() >= cmp_size && 
			req->path().substr(0, cmp_size) == std::get<0>(handler))
			best = handler;	
	}

	if(std::get<1>(best) == nullptr)
		return false;

	std::get<1>(best)->handleRequest(req, rep);
	return true;
}

bool 
RequestHandler::url_decode(std::string& in_out)
{
	std::string out;
	out.reserve(in_out.size());
	for(std::size_t i = 0; i < in_out.size(); ++i) {
		if(in_out[i] == '%') {
      			if(i + 3 <= in_out.size()) {
        			int value = 0;
        			std::istringstream is(in_out.substr(i + 1, 2));
        			if(is >> std::hex >> value) {
          				out += static_cast<char>(value);
          				i += 2;
        			} else {
          				return false;
        			}
      			} else {
        			return false;
      			}
    		} else if (in_out[i] == '+') {
      			out += ' ';
    		} else {
      			out += in_out[i];
    		}
  	}
	in_out = out;
  	return true;
}
