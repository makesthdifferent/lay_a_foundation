CC = g++
CXXFLAGS=-lpthread -g -std=c++11
target:main.cpp threadpool.o
	$(CC) main.cpp threadpool.o -o Demo $(CXXFLAGS)
threadpool.o:threadpool.cc
	$(CC) -c threadpool.cc $(CXXFLAGS)
clean:
	rm *.o