include $(ALELIB_DIR)/conf/variables

RM	  = rm

CPPSOURCES = $(wildcard *.cpp)
OBJS	= ${SRCS:.cpp=.o}

all: $(CPPSOURCES:.cpp=.o)

%.o: %.cpp
	$(ALE_CXX) -c $< $(ALE_CXXFLAGS) -o $@
	
clean:
	$(ALE_RM) *.o *~
	
remade:
	$(MAKE) clean
	$(MAKE)

-include $(CPPSOURCES:.cpp=.d)

%.d: %.cpp
	$(ALE_CXX) $< -MM -MT '$*.o $*.d ' -M $(ALE_CXXFLAGS)
