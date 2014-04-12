include $(ALELIB_DIR)/conf/variables

SUBDIRS = mesh util io shape_functions quadrature

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

all: msg subdirs build ok_msg

msg:
	echo $(ALELIB_DIR)

$(SUBDIRS):
	$(MAKE) all -C $@
	
clean:
	$(ALE_RM) `find . -iname *.o`
	$(ALE_RM) `find . -iname *.d`
	
# pay attention to the wildcard depth.
build: subdirs
	ar -cr libalelib.a $(wildcard */*.o) $(wildcard */*/*.o) $(wildcard */*/*/*.o)
	mv -f libalelib.a $(ALE_LIBS_DIR)
	ranlib $(ALE_LIBS_DIR)/libalelib.a

ok_msg: build
	@echo
	@echo successfully compiled
	@echo
	
	
	
