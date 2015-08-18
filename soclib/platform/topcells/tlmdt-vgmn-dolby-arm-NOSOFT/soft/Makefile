ADD_OBJS = stdlib.o stdio.o exception.o system.o main.o

ADD_LDFLAGS = 
export ADD_LDFLAGS

SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk
