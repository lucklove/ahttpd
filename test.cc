#include "request.hh"
#include "response.hh"
#include <iostream>
#include <string>
#include <cassert>
#include "server.hh"

int
main(int argc, char *argv[])
{
	Request t(static_cast<Server *>(nullptr), nullptr);
	std::string line;
	t.out() << "1\r\n";
	t.in() >> line;
	std::cout << line;
}
