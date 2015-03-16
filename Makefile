CXX = g++

RM = rm -f

CXXFLAGS = -Wall -std=c++14

LDFLAGS = -lboost_system -lpthread -lssl -lcrypto

TEST_OBJS = main.o server.o request.o RequestHandler.o parser.o response.o package.o TcpConnection.o SslConnection.o log.o

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $*.cc

all: test

test: $(TEST_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(TEST_OBJS)

clean:
	$(RM) *.o test
