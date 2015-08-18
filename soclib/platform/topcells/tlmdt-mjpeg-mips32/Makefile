ARCH=mips32el
SIMULATOR_BINARY?=simulation.x
PLATFORM_DESC?=platform_desc
SOCLIB_CC_ARGS?=-p $(PLATFORM_DESC)
SOCLIB_CC=soclib-cc
SOCLIB?=$(shell soclib-cc --getpath)
N_INITS?=1
export SOCLIB

ifneq ($(SOCLIB_CC_MODE),)
SOCLIB_CC_ADD_ARGS+= -m $(SOCLIB_CC_MODE)
endif
ifneq ($(SOCLIB_CC_TYPE),)
SOCLIB_CC_ADD_ARGS+= -t $(SOCLIB_CC_TYPE)
endif

export ARCH

include $(SOCLIB)/utils/conf/soft_flags.mk

CC=$($(ARCH)_CC_PREFIX)gcc
HAS_CC:=$(shell which $(CC) 2>&1 > /dev/null && echo ok)
ifneq ($(HAS_CC),ok)
NO_SOFT=1
NO_TEST=No compiler for $(ARCH)
endif

all: test_soclib $(SIMULATOR_BINARY) soft/bin$(N_INITS)proc.soft

test_soclib:
	@test -z "$(SOCLIB)" && (\
	echo "You must have soclib-cc in your $$PATH" ; exit 1 ) || exit 0
	@test ! -z "$(SOCLIB)"

$(SIMULATOR_BINARY): $(PLATFORM_DESC)
	$(SOCLIB_CC) -P $(SOCLIB_CC_ARGS) $(SOCLIB_CC_ADD_ARGS) -o $@

soft/bin$(N_INITS)proc.soft:
	$(MAKE) -C soft

clean: 
	$(SOCLIB_CC) -P $(SOCLIB_CC_ARGS) $(SOCLIB_CC_ADD_ARGS) -x -o $(SIMULATOR_BINARY)
	@rm -rf repos
	@rm -rf *~
	@rm -rf semantic*
	@rm -rf \#*

soft_clean:
	$(MAKE) -C soft clean


.PHONY: $(SIMULATOR_BINARY)
