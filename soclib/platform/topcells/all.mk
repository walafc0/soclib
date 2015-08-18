#
# SOCLIB_GPL_HEADER_BEGIN
# 
# This file is part of SoCLib, GNU GPLv2.
# 
# SoCLib is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# SoCLib is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with SoCLib; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
# 
# SOCLIB_GPL_HEADER_END
#
# Copyright (c) UPMC, Lip6, SoC
#         Nicolas Pouillon <nipo@ssji.net>, 2007
#
# Maintainers: nipo

SIMULATOR_BINARY?=simulation.x
PLATFORM_DESC?=platform_desc
SOCLIB_CC_ARGS?=-p $(PLATFORM_DESC)
SOCLIB_CC=soclib-cc
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
TEST_OUTPUT=test.out

PLATFORM_DESC:=$(shell pwd)/$(PLATFORM_DESC)
PLATFORM_DIR:=$(shell pwd)
export PLATFORM_DESC
export PLATFORM_DIR

ifneq ($(SOCLIB_CC_MODE),)
SOCLIB_CC_ADD_ARGS+= -m $(SOCLIB_CC_MODE)
endif
ifneq ($(SOCLIB_CC_TYPE),)
SOCLIB_CC_ADD_ARGS+= -t $(SOCLIB_CC_TYPE)
endif

export SOCLIB_CC_ADD_ARGS

export ARCH

include $(SOCLIB)/utils/conf/soft_flags.mk

ifeq ($(ARCH), )
$(warning ARCH not defined, assuming host compiler)
CC=gcc
else
ifeq ($($(ARCH)_CC_PREFIX), )
$(warning No compiler define for $(ARCH), software wont be compiled)
CC=bad-gcc
else
CC=$($(ARCH)_CC_PREFIX)gcc
endif
endif

HAS_CC:=$(shell which $(CC) 2>&1 > /dev/null && echo ok)
ifneq ($(HAS_CC),ok)
NO_SOFT=1
NO_TEST=No compiler for $(ARCH)
$(warning $(CC) not found, software wont be compiled)
endif

ifeq ($(NO_SOFT),)
SOFT?=soft/bin.soft
endif

ifeq ($(shell test -r disabled && echo disabled),disabled)
NO_TEST=Test disabled by "disabled" file: $(shell cat disabled)
NO_SIMCOMPILE=1
endif

all: test_soclib $(SIMULATOR_BINARY) $(SOFT)

ifeq ($(NO_COMPILE),)

simcompile: $(SIMULATOR_BINARY)

else

simcompile:
	@echo "simcompile disabled"

endif

ifeq ($(NO_SOFT),)

.PHONY: $(SOFT)

$(SOFT):
	$(MAKE) -C $(dir $@) $(notdir $@)

endif

test_soclib:
	@test -z "$(SOCLIB)" && (\
	echo "You must have soclib-cc in your $$PATH" ; exit 1 ) || exit 0
	@test ! -z "$(SOCLIB)"

$(SIMULATOR_BINARY): $(PLATFORM_DESC)
	$(SOCLIB_CC) -P $(SOCLIB_CC_ARGS) $(SOCLIB_CC_ADD_ARGS) -o $@

ifdef NO_TEST

test:
	@echo "Test disabled: $(NO_TEST)"

else

test: $(TEST_OUTPUT) post_test

post_test:

$(TEST_OUTPUT): $(SIMULATOR_BINARY) $(SOFT)
	set -o pipefail ; SOCLIB_TTY=TERM ./$(SIMULATOR_BINARY) $(SIMULATION_ARGS) < /dev/null 2>&1 | tee $@

.PHONY: $(TEST_OUTPUT)

endif

clean: soft_clean
	$(SOCLIB_CC) -P $(SOCLIB_CC_ARGS) $(SOCLIB_CC_ADD_ARGS) -x -o $(SIMULATOR_BINARY)
	rm -rf repos

soft_clean: soft/clean

soft/clean:
ifeq ($(NO_SOFT),)
	$(MAKE) -C $(dir $(SOFT)) clean
endif

a.out:
	$(MAKE) -C soft

.PHONY: a.out $(SIMULATOR_BINARY)
