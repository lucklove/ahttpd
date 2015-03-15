CXX = g++

RM = rm -f

CXXFLAGS = -std=c++11 -Wall

LDFLAGS = -lboost_system -lpthread -lssl -lcrypto

TEST_OBJS = main.o server.o connection.o request.o RequestHandler.o parser.o response.o package.o TcpConnection.o SslConnection.o

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $*.cc

all: test

test:
	$(CXX) $(LDFLAGS) -o $@ $(TEST_OBJS)

clean:
	$(RM) *.o test
