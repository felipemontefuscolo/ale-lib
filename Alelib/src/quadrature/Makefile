include $(ALELIB_DIR)/conf/variables

CPPSOURCES = $(wildcard *.cpp)

SUBDIRS =

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

#all: $(warning In a prerequisite list)$(CPPSOURCES:.cpp=.o) subdirs
all: $(CPPSOURCES:.cpp=.o)

$(SUBDIRS):
	$(MAKE) all -C $@

%.o: %.cpp
#	$(warning In Objs Script)
	$(ALE_CXX) -c $< $(ALE_CXXFLAGS) -o $@
	
clean:
	$(ALE_RM) *.o *~
	
remade:
	$(MAKE) clean
	$(MAKE)

-include $(CPPSOURCES:.cpp=.d)

%.d: %.cpp
	$(ALE_CXX) $< -MM -MT '$*.o $*.d ' -MD $(ALE_CXXFLAGS)

