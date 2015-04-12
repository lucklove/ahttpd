all: server client

server:
	clang++ -Wall -g -std=c++1y JsonRpcServer.cc -o JsonRpcServer -lshttpd -I/usr/local/include/shttpd

client:
	clang++ -Wall -g -std=c++1y JsonRpcServer.cc -o JsonRpcServer -lshttpd -I/usr/local/include/shttpd

clean:
	rm JsonRpcServer
