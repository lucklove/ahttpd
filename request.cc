#include "request.hh"
#include "response.hh"
#include "server.hh"

void 
Request::deliverSelf() 
{
	server_->deliverRequest(shared_from_this(), 
		std::make_shared<Response>(server_, connection()));
}
