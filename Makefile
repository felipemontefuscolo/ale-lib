# check for ALELIB_DIR
ifeq "" "$(wildcard ${ALELIB_DIR})"
$(error variable ALELIB_DIR was not defined or is an invalid directory)
endif 

include $(ALELIB_DIR)/conf/variables

.PHONY: src

all: src ok_msg

src:
	$(MAKE) all -C Alelib/src
	
clean:
	$(MAKE) clean -C Alelib/src
	
ok_msg: src
	@echo
	@echo successfully compiled
	@echo
	
