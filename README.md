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
- cd 某个示例的目录  
- make  

###注意:
若在运行时出现如下错误:  
error while loading shared libraries: libshttpd.so: cannot open shared object file: No such file or directory  
请确保libshttpd.so所在目录在ld的搜索路径中,对于linux系统, libshttpd.so所在的目录一般为/usr/local/lib  
若发现改目录不在ld的搜索路径中,以下步骤可让libshttpd.so被ld找到:  
- 修改/etc/ld.so.conf, 加入一行/usr/local/lib  
- 执行sudo ldconfig更新配置  

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
##example目录中的示例:

|   名称     |         描述                |             访问地址             |            备注             |
|------------|-----------------------------|----------------------------------|-----------------------------|
| HelloWorld | 向客户端发送hello world	   | http://127.0.0.1:8888/HelloWorld |				    |
|   echo     | 显示客户端请求包的详细信息  | http://127.0.0.1:8888/echo       |				    |
| HttpsTest  |  https的示例		   | https://127.0.0.1:9999/HttpsTest |  需要输入创建密钥时的密码   |

##TODU LIST
- 连接定时器: 指定时间内客户端无反应就断开
