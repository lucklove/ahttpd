#include "client.hh"
#include "response.hh"
#include "request.hh"
#include <iostream>

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
					req->basicAuth("632541058@qq.com:123456");
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
