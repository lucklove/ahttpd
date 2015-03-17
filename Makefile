CXX = clang++

RM = rm -f

CXXFLAGS = -Wall -std=c++14

LDFLAGS = -lboost_system -lpthread -lssl -lcrypto -lboost_regex -ltcmalloc

TEST_OBJS = main.o server.o request.o RequestHandler.o parser.o response.o package.o \
	TcpConnection.o SslConnection.o log.o ThreadPool.o

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $*.cc

all: test

test: $(TEST_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(TEST_OBJS)

clean:
	$(RM) *.o test
