include $(ALELIB_DIR)/conf/variables

CPPSOURCES = $(wildcard *.cpp)

SUBDIRS = shape_types

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

#all: $(warning In a prerequisite list)$(CPPSOURCES:.cpp=.o) subdirs
all: $(CPPSOURCES:.cpp=.o) subdirs

$(SUBDIRS):
	$(MAKE) all -C $@

%.o: %.cpp $(ALELIB_DIR)/conf/variables
#	$(warning In Objs Script)
	$(ALE_CXX) -c $< $(ALE_CXXFLAGS) -o $@
	
clean:
	$(ALE_RM) *.o *~
	
remade:
	$(MAKE) clean
	$(MAKE)

-include $(CPPSOURCES:.cpp=.d)



