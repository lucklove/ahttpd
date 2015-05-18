[![Build Status](https://travis-ci.org/lucklove/ahttpd.svg?branch=master)](https://travis-ci.org/lucklove/ahttpd)
[![Coverage Status](https://coveralls.io/repos/lucklove/ahttpd/badge.svg?branch=master)](https://coveralls.io/r/lucklove/ahttpd?branch=master)
app server框架
===================

##当前特性
- http/https server
- http/https client
- smtp client
- cookie

##依赖
- boost
- C++1y  
- libssl(https需要)  

##编译-安装
- cd ahttpd  
- mkdir build  
- cd build  
- cmake ..  
- make  
- sudo make install

###注意:
若在运行时出现如下错误:  
error while loading shared libraries: libahttpd.so: cannot open shared object file: No such file or directory  
请确保libahttpd.so所在目录在ld的搜索路径中,对于linux系统, libahttpd.so所在的目录一般为/usr/local/lib  
若发现改目录不在ld的搜索路径中,以下步骤可让libahttpd.so被ld找到:  
- 修改/etc/ld.so.conf, 加入一行/usr/local/lib  
- 执行sudo ldconfig更新配置  

###gcc-4.8 workaround：
经测试在gcc-4.8下能编译通过,但是运行时抛出std::regex_error异常, 执行`sh gcc-4.8-fix.sh`可修复问题  

##示例
```c++
#include "server.hh"
struct TestHandler : public RequestHandler {
	void handleRequest(RequestPtr req, ResponsePtr rep) override {
		rep->out() << "hello world!" << std::endl;
	}
};

int
main(int argc, char *argv[])
{
	std::stringstream config("{\"http port\":\"8888\"}");
	Server server(config);
	server.addHandler("/", new TestHandler());
	server.run();
}
```
##example目录中的示例:

|   名称     |         描述                |             访问地址                 |           备注                    |
|------------|-----------------------------|--------------------------------------|-----------------------------------|
| HelloWorld | 向客户端发送hello world	   | http://127.0.0.1:8888/HelloWorld     |		             	      |
|   echo     | 显示客户端请求包的详细信息  | http://127.0.0.1:8888/echo           |	              		      |
| HttpsTest  | https的示例		   | https://127.0.0.1:9999/HttpsTest     |  需要输入创建密钥时的密码         |
|   client   | 异步客户端示例	           |				          |				      |
|StaticServer| 静态文件服务器              | http://127.0.0.1:8888/[xxx]          | 其中的小游戏来自github用户kig)    |
|   mail     | 使用smtp发送邮件		   |					  |				      |
|   proxy    | http/https代理服务器	   | http://127.0.0.1:8888		  | 				      |  

http代理服务器地址: 115.28.32.115:8888 

##人丑bug多，欢迎找茬
