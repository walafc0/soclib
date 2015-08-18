ADD_OBJS = stdio.o stdlib.o system.o 
OBJS = boot.o crt0.o ram_init.o main.o $(ADD_OBJS)
SOCLIB?=$(shell soclib-cc --getpath)
LGCC_FILE = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
lm32_LDFLAGS += -L$(dir $(LGCC_FILE))

export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/soft.mk
