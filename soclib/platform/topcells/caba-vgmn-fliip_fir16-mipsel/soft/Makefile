ADD_OBJS= stdlib.o stdio.o
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk

main.c : image.dat

image.dat : image.dat.gz
	gzip -dc $< > $@
