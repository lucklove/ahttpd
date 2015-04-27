#include "client.hh"
#include "response.hh"
#include "request.hh"
#include <iostream>

int 
main(int argc, char* argv[])
{
	Client c;
	if(argc < 3) {
		std::cout << "useage: client method url [body]" << std::endl;
		return 0;
	}
	try {
		c.request(argv[1], argv[2], 
			[](auto res) {
				if(res) {
					std::cout << res->out().rdbuf();
				} else {
					std::cout << "bad" << std::endl;
				}
			},
			[=](auto req) {
				if(req) {
					if(argc == 4)
						req->out() << argv[3];
				}
			}
		);
		c.apply();
	} catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
  	return 0;
}
