CXX=g++
#CXX=clang++
CPPFLAGS=-g3 -gdwarf-2 -Wall -std=c++98 -Wextra -DDEBUG -I. -pedantic

test: test.cpp Array/array.hpp Makefile
	$(CXX) $(CPPFLAGS) test.cpp -o test

clean:
	rm -f test


