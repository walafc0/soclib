ADD_OBJS= stdlib.o stdio.o dhry_1.o dhry_2.o
ADD_CFLAGS=-DTIME -O3 #  -DUSE_REG
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk
