#include "RequestHandler.hh"
#include "response.hh"
#include "request.hh"
#include "server.hh"
#include "log.hh"
#include "connection.hh"
#include <fstream>
#include <sstream>
#include <string>

namespace {
const char *allowed_method[] = {
	"GET",
	"POST",
	"PUT",
	"DELETE"
};

bool method_implemented(const std::string& method)
{
	for(const char* m : allowed_method) {
		if(m == method)
			return true;
	}
	return false;
}
}

void 
RequestHandler::handleRequest(RequestPtr req, ResponsePtr rep)
{
	try {
		if(!method_implemented(req->getMethod())) {
			rep->status() = Response::not_implemented;
			return;
		}
		if(!deliverRequest(req, rep)) {
			rep->status() = Response::not_found;
			return;
		}
	} catch(std::exception& e) {
		Log("WARNING") << "EXCEPTION FROM USER HANDLER: " << e.what();
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
