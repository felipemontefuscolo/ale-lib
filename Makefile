# check for ALELIB_DIR
ifeq "" "$(wildcard ${ALELIB_DIR})"
$(error variable ALELIB_DIR was not defined or is an invalid directory)
endif 

include $(ALELIB_DIR)/conf/variables

.PHONY: src

all: src ok_msg

$(ALELIB_DIR)/conf/variables:


src: $(ALELIB_DIR)/conf/variables Makefile
	$(MAKE) all -C Alelib/src
	
#clean:
#	$(MAKE) clean -C Alelib/src

clean:
	$(ALE_RM) `find . -iname *.o`
	$(ALE_RM) `find . -iname *.d`
	$(ALE_RM) `find . -iname *.a`

	
ok_msg: src
	@echo
	@echo successfully compiled
	@echo
	
