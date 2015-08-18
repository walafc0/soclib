ADD_OBJS=stdio.o stdlib.o exception.o system.o main.o
ADD_CFLAGS=-G0
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk
