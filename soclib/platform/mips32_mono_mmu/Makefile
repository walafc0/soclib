ARCH=mips32el
SIMULATION_ARGS=100000
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
SOCLIB_CC_ADD_ARGS=-j8
include $(SOCLIB)/soclib/platform/topcells/all.mk
