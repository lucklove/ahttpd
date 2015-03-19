#http server异步框架#

##当前特性:##
http/https支持

##依赖##
1.asio
2.C++1y
3.openssl(https需要)

##编译##
cd shttpd && mkdir build && cd build && cmake .. && make

##运行##
./test
启动过程中会要求输入PEM密码(生成ssl密钥文件时的密码)，此时输入lucklove

##测试地址##
http://127.0.0.1:8888/test
https://127.0.0.1:9999/test
会返回当前请求的方法，路径，headers, body等

##示例##
main.cc即为一个示例，继承RequestHandler并定义自己的handleRequest方法即可
