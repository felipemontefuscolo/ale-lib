include $(ALELIB_DIR)/conf/variables

SUBDIRS = mesh util io shape_functions quadrature dof_mapper

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

all: msg subdirs build

msg:
	echo $(ALELIB_DIR)

$(SUBDIRS):
	$(MAKE) all -C $@
	
clean:
	$(ALE_RM) `find . -iname *.o`
	$(ALE_RM) `find . -iname *.d`
	$(ALE_RM) `find . -iname *.a`
	
# pay attention to the wildcard depth.
build: subdirs
	## Linux Users
	#ar -cr libalelib.a $(wildcard */*.o) $(wildcard */*/*.o) $(wildcard */*/*/*.o)
	#mv -f libalelib.a $(ALE_LIBS_DIR)
	#ranlib $(ALE_LIBS_DIR)/libalelib.a
	## MAC USERS
	libtool -static -o libalelib.a $(wildcard */*.o) $(wildcard */*/*.o) $(wildcard */*/*/*.o)
	mv -f libalelib.a $(ALE_LIBS_DIR)
	
