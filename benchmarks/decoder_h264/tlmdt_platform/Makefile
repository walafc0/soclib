include config.mk

SIMULATION_ARGS=$(KERNEL)
SOFT=$(KERNEL)
NO_SOFT=1
SOCLIB?=$(shell soclib-cc --getpath)
SIMULATOR_BINARY=platform_h264.x
# No need to change this, the soclib part does not build software.
# Change config.mk if you want.
ARCH=mips32el
export SOCLIB

all: $(SIMULATOR_BINARY) $(KERNEL) block0.iso

help:
	@echo "You may either build:"
	@echo "  $(SIMULATOR_BINARY): only the simulator"
	@echo "  $(KERNEL): only the mutekh kernel"

$(SIMULATOR_BINARY): cpu_type.h

cpu_type.h:
	echo "#ifndef __CPUCONFIG_H__" > $@
	echo "#define __CPUCONFIG_H__" >> $@
	echo "#define CPU_$(CPU)" >> $@
	echo "#define VIDEO_TYPE_$(FORMAT)" >> $@
	if [ $(GDB_SERVER) = "YES" ]; then echo "#define USE_GDBSERVER" >> $@; fi
	if [ $(MEM_CHECKER) = "YES" ] ; then echo "#define USE_MEMCHECKER" >> $@ ; fi
	if [ $(TIMER) = "YES" ] ; then echo "#define CONFIG_TIMER" >> $@ ; fi
	if [ $(FRAMEBUFFER) = "YES" ] ; then echo "#define CONFIG_FRAMEBUFFER" >> $@ ; fi
	if [ $(DBF) = "YES" ] ; then echo "#define CONFIG_DB_FILTER" >> $@ ; fi
	if [ $(DUMP_STATS) = "YES" ] ; then echo "#define CONFIG_DUMP_STATS" >> $@ ; fi
	echo "#define NCPU $(NB_PROC)" >> $@
	echo "#define CACHE_LINE_SIZE $(CACHE_LINE_SIZE)" >> $@
	echo "#endif" >> $@

mutekh:
	mkdir -p $@

$(KERNEL): mutekh
	rm $(APP_DIR)/src/conf.h 
	ln -s $(APP_DIR)/soft_config/conf_$(NB_PROC)proc_$(FORMAT).h $(APP_DIR)/src/conf.h
	$(MAKE) \
	   -f $(MUTEKH_DIR)/Makefile \
	   CONF=$(APP_DIR)/$(CONFIG) \
	   BUILD_DIR=$(shell pwd)/mutekh \
	   SRC_DIR=$(MUTEKH_DIR) -j 3 \
	   BUILD=$(BUILD)

doc: mutekh
	$(MAKE) \
	   -f $(MUTEKH_DIR)/Makefile \
	   CONF=$(APP_DIR)/$(CONFIG) \
	   BUILD_DIR=$(shell pwd)/mutekh \
	   SRC_DIR=$(MUTEKH_DIR) -j 3 doc


block0.iso: disk
	mkisofs -f -R -m .svn -V volname -o $@ $<


.PHONY: $(KERNEL)

include $(SOCLIB)/soclib/platform/topcells/all.mk
