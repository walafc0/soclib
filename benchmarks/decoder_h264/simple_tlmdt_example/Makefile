SIMULATION_ARGS=$(KERNEL)
SOFT=$(KERNEL)
NO_SOFT=1
SOCLIB?=$(shell soclib-cc --getpath)
SIMULATOR_BINARY=platform_h264.x
ARCH=mips32el

APP_DIR=$(shell pwd)/decoder_h264_clone/decoder_soclib
KERNEL=mutekh/kernel-soclib-mips32el.out

export SOCLIB

all: $(KERNEL) block0.iso

help:
	@echo "You may either build:"
	@echo "  $(SIMULATOR_BINARY): only the simulator"
	@echo "  $(KERNEL): only the mutekh kernel"

mutekh:
	mkdir -p $@

$(KERNEL): mutekh
	rm $(APP_DIR)/src/conf.h 
	ln -s $(APP_DIR)/soft_config/conf_3proc_qcif.h $(APP_DIR)/src/conf.h
	$(MAKE) \
	   -f $(MUTEKH_DIR)/Makefile \
	   CONF=$(APP_DIR)/config \
	   BUILD_DIR=$(shell pwd)/mutekh \
	   SRC_DIR=$(MUTEKH_DIR) -j 3 \
	   BUILD=soclib-mips32el:pf_decoder_h264

doc: mutekh
	$(MAKE) \
	   -f $(MUTEKH_DIR)/Makefile \
	   CONF=$(APP_DIR)/config \
	   BUILD_DIR=$(shell pwd)/mutekh \
	   SRC_DIR=$(MUTEKH_DIR) -j 3 doc


block0.iso: disk
	mkisofs -f -R -m .svn -V volname -o $@ $<


.PHONY: $(KERNEL)

include $(SOCLIB)/soclib/platform/topcells/all.mk
