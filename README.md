http server异步框架
===================

##当前特性
http/https支持

##依赖
1.asio  
2.C++1y  
3.openssl(https需要)  

##编译
cd shttpd && mkdir build && cd build && cmake .. && make  

##运行
./test  
启动过程中会要求输入PEM密码(生成ssl密钥文件时的密码),此时输入lucklove  

##测试地址
http://127.0.0.1:8888/test  
https://127.0.0.1:9999/test  
会返回当前请求的方法，路径，headers, body等

##示例
```c++
#include "server.hh"
struct TestHandler : public RequestHandler {
	using RequestHandler::RequestHandler;
	void handleRequest(RequestPtr req, ResponsePtr rep) override {
		rep->out() << "hello world!" << std::endl;
	}
};

int
main(int argc, char *argv[])
{
	asio::io_service io_service;
	Server server(io_service, "8888");
	server.addHandler("/", new TestHandler(&server));
	server.run();
}
```
更多例子见example
