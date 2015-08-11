# define boost directory
#BOOST_DIR=
CXX=g++
#CXX=clang++
#CPPFLAGS= -O3 -march=native -mtune=native --fast-math -DNDEBUG -Wall -std=c++98 -Wextra -I.. -I$(BOOST_DIR) -pedantic
CPPFLAGS= -O3 -march=native -mtune=native  -DNDEBUG -Wall -std=c++98 -Wextra -I.. -I$(BOOST_DIR) -pedantic
#CPPFLAGS= -Wall -std=c++98 -Wextra -I.. -I$(BOOST_DIR) -pedantic

ifeq "" "$(wildcard $(BOOST_DIR))"
$(error variable BOOST_DIR was not defined or is an invalid directory)
endif 

test: test.cpp ../Array/array.hpp Makefile
	$(CXX) $(CPPFLAGS) test.cpp -o test

clean:
	rm -f test


