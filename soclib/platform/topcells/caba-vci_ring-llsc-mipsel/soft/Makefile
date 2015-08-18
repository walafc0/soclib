ADD_OBJS= stdlib.o stdio.o globals.o
ADD_CFLAGS=-mips2
SIMULATION_ARGS=
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk
