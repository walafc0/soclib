ARCH = mips32el
PLATFORM_DESC=hard/platform_desc
SIMULATOR_BINARY=hard/system.x
SOCLIB_CC_ADD_ARGS=-I$(PWD)

SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB

include $(SOCLIB)/soclib/platform/topcells/all.mk

$(PLATFORM_DESC) : arch.py
	python arch.py --mode generate
