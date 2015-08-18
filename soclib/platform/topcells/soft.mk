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

SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB

ifeq ($(PLATFORM_DESC)$(PLATFORM_DIR),)
$(error You must launch the software compilation from the platform directory, or your platform_desc file wont be found.)
endif

SOFT_IMAGE=bin.soft
OBJS?=main.o exception.o system.o $(ADD_OBJS)

COMMON=$(SOCLIB)/soclib/platform/topcells/common
include $(SOCLIB)/utils/conf/soft_flags.mk

INTERFACE_CFLAGS:=$(shell cd $(PLATFORM_DIR) && soclib-cc -p $(PLATFORM_DESC) $(SOCLIB_CC_ADD_ARGS) --embedded-cflags)

VPATH=. $(COMMON)

CC_PREFIX=$($(ARCH)_CC_PREFIX)
CC = $(CC_PREFIX)gcc
AS = $(CC_PREFIX)as
LD = $(CC_PREFIX)ld
OBJDUMP = $(CC_PREFIX)objdump

CFLAGS=-Wall -O2 -I. $(ADD_CFLAGS) $(DEBUG_CFLAGS) $($(ARCH)_CFLAGS) -ggdb -I$(COMMON) $(INTERFACE_CFLAGS)
LIBGCC:=$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

MAY_CLEAN=$(shell test -r arch_stamp && (test "$(ARCH)" = "$$(cat /dev/null arch_stamp)" || echo clean))

default: clean $(SOFT_IMAGE)

$(SOFT_IMAGE): ldscript $(MAY_CLEAN) arch_stamp $(OBJS)
	$(LD) -q $($(ARCH)_LDFLAGS) $(ADD_LDFLAGS) -o $@ $(filter %.o,$^) -T $(filter %ldscript,$^) $(LIBGCC)

arch_stamp:
	echo $(ARCH) > $@

%.o: %.s
	$(AS) $< -o $@

%.o : %.c
	$(CC) -o $@ $(CFLAGS) -c $<

clean :
	-rm -f $(SOFT_IMAGE) $(OBJS) arch_stamp deps.mk $(OBJS:.o=.deps)

ifneq ($(MAKECMDGOALS),clean)
deps.mk: $(OBJS:.o=.deps)
	cat $^ /dev/null > $@

%.deps: %.c
	$(CC) $(CFLAGS) -M -MT $*.o -MF $@ $<

%.deps: %.s
	touch $@

-include deps.mk
endif
