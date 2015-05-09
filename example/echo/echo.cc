/********************************************************************************
 *  ahttpd示例程序								*
 *  向客户端返回客户请求报文中的各种信息					*
 *  by Joshua <gnu.crazier@gmail.com> at 2015-3-30				*
 ********************************************************************************/ 	

#include <iostream>
#include "server.hh"

struct EchoHandler : public RequestHandler {
	void handleRequest(RequestPtr req, ResponsePtr rep) override {
		auto auth_info = req->basicAuthInfo();
		rep->out() << "method: " << req->getMethod() << std::endl;
		rep->out() << "path: " << req->path() << std::endl;
		rep->out() << "query: " << req->query() << std::endl;
		rep->out() << "version: " << req->version() << std::endl;
		if(auth_info != "")
			rep->out() << "basic auth infomation:[" << auth_info << "]" << std::endl;
		rep->flush();
		for(auto&& h : req->headerMap())
			rep->out() << h.name << ": " << h.value << std::endl;
		rep->flush();
		if(req->in().rdbuf()->in_avail())				/**< 判断是否有数据，[重要]*/
			rep->out() << req->in().rdbuf();
	}
};

int 
main(int argc, char* argv[])
{
	try {
		std::stringstream config("{\"http port\":\"8888\"}");
		Server server(config);						/**< 在8888端口监听 */
		server.addHandler("/echo", new EchoHandler());			/**< 路径为/echo */
		server.run(10);							/**< 给io_service 10个线程 */
	} catch(std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
