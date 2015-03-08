all:
	g++ *.cc -std=c++11 -lboost_system -lpthread
clean:
	rm -f *.o a.out
