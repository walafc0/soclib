OBJS = main.o uputs.o reset.o stdlib.o
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk
