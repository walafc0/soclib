SIMULATION_ARGS=$(KERNEL)
SOFT=$(KERNEL)
SOCLIB?=$(shell soclib-cc --getpath)
SIMULATOR_BINARY=system.x
# No need to change this, the soclib part does not build software.
# Change config.mk if you want.
ARCH=mips32el
export SOCLIB

all: $(KERNEL) block0.iso platform.dtb

.PHONY: $(KERNEL) config.h FORCE

FORCE:

include $(SOCLIB)/soclib/platform/topcells/all.mk

block0.iso: iso_contents FORCE
	mkisofs -f -R -V volname -o $@ $<

platform.dtb: platform.dts
	dtc -O dtb platform.dts -o platform.dtb

