#include "response.hh"

#include <iostream>
#include <sstream>

int
main(int argc, char *argv[])
{
	Response *rep = new Response(nullptr, nullptr);
	rep->out() << "hello";
	std::cout << "handle.................." << std::endl;
	rep->out() << "it's TestHander::handleRequest" << std::endl;
	rep->out() << "method: " << std::endl;
	rep->out() << "uri: " << std::endl;
	rep->out() << "version: " << std::endl;
	std::cout << "--------------" << rep->contentLength() << std::endl;
}
