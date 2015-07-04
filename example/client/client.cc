#include "ahttpd.hh"
#include <iostream>

using namespace ahttpd;

int 
main(int argc, char* argv[])
{
	Client c;
	c.enableCookie();
	if(argc != 3) {
		std::cout << "useage: client method url" << std::endl;
		return 0;
	}
	try {
		c.request(argv[1], argv[2], 
			[](ResponsePtr res) {
				if(res) {
					std::cout << res->out().rdbuf();
				} else {
					Log("ERROR") << "bad response";
				}
			},
			[=](RequestPtr req) {
				if(req) {
					std::cout << "Input http body, end with [^D]:" << std::endl;
					req->out() << std::cin.rdbuf();
				}
			}
		);
		c.apply();
	} catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
  	return 0;
}
