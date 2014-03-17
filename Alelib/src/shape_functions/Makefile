
CXXFLAGS=-Wall -Wextra -pedantic
OBJECTS=$(wildcard *.cc)
OBJECTS+=$(wildcard shape_types/*.cc)
HEADERS=$(wildcard *.hpp)
HEADERS+=$(wildcard shape_types/*.hpp)


.PHONY : clean

#test: $(OBJECTS) $(HEADERS) clean
#	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

test: parametric_pts.hpp test.cc
	$(CXX) $(CXXFLAGS) test.cc -o test

clean:
	rm -f test

