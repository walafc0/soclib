ADD_OBJS = stdio.o stdlib.o 
OBJS = crt0.o main.o trap.o exception.o system.o $(ADD_OBJS)
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk
