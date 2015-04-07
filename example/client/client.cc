#include "client.hh"
#include "response.hh"
#include "request.hh"
#include <iostream>

int 
main(int argc, char* argv[])
{
	asio::io_service io_service;

	Client c(io_service);
	if(argc < 3) {
		std::cout << "useage: client method url [body]" << std::endl;
		return 0;
	}
	try {
		c.request(argv[1], argv[2], 
			[](auto res, bool good) {
				if(good) {
					std::cout << res->out().rdbuf();
				} else {
					std::cout << "bad" << std::endl;
				}
			},
			[=](auto req, bool good) {
				if(good) {
					if(argc == 4)
						req->out() << argv[3];
				}
			}
		);
		io_service.run();
	} catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
  	return 0;
}
