http server异步框架
===================

##当前特性
http/https支持

##依赖
- asio  
- C++1y  
- openssl(https需要)  

##编译-安装
- cd shttpd  
- mkdir build  
- cd build  
- cmake ..  
- make  
- sudo make install

##编译示例
- cd shttpd  
- cd example  
- make  

##运行
./test  
启动过程中会要求输入PEM密码(生成ssl密钥文件时的密码),此时输入lucklove  
###注意:
若在运行时出现如下错误:  
error while loading shared libraries: libshttpd.so: cannot open shared object file: No such file or directory  
请确保libshttpd.so所在目录在ld的搜索路径中,对于linux系统, libshttpd.so所在的目录一般为/usr/local/lib  
若发现改目录不在ld的搜索路径中,以下步骤可让libshttpd.so被ld找到:  
- 修改/etc/ld.so.conf, 加入一行/usr/local/lib  
- 执行sudo ldconfig更新配置  

##测试地址
http://127.0.0.1:8888/test  
https://127.0.0.1:9999/test  
会返回当前请求的方法，路径，query, headers, body等

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

##TODU LIST
- 连接定时器: 指定时间内客户端无反应就断开
