OBJS=boot.o it.o system.o main.o
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk
