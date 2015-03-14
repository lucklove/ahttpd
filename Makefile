all:
	g++ main.cc server.cc connection.cc request.cc RequestHandler.cc parser.cc response.cc package.cc -std=c++11 -lboost_system -lpthread
test:
	g++ test.cc server.cc connection.cc request.cc RequestHandler.cc parser.cc response.cc package.cc -std=c++11 -lboost_system -lpthread
clean:
	rm -f *.o a.out
