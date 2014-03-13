ALELIB_DIR=${HOME}/Dropbox/cl-simu2/ale-lib
export ALELIB_DIR

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
	
