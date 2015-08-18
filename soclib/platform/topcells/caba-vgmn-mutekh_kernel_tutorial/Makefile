KERNEL=mutekh/kernel-soclib-mips.out
SIMULATION_ARGS=$(KERNEL)
NO_SOFT=1
SOCLIB?=$(shell soclib-cc --getpath)
SIMULATOR_BINARY=system.x
# never mind about the ARCH below, we dont use the software-building
# feature of this makefile anyway
ARCH=mips32el
export SOCLIB

all: $(SIMULATOR_BINARY) block0.iso

block0.iso:
	touch $@

CONFIG=config_soclib_mipsel
APP=$(MUTEKH_DIR)/examples/hello

.PHONY: $(KERNEL)

include $(SOCLIB)/soclib/platform/topcells/all.mk

