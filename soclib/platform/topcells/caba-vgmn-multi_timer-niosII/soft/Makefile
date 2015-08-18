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

SOFT_IMAGE=bin.soft
OBJS= reset.o exceptionProcessing.o softwareException.o system.o stdio.o stdlib.o main.o  

include $(SOCLIB)/utils/conf/soft_flags.mk

INTERFACE_CFLAGS:=$(shell cd $(PLATFORM_DIR) && soclib-cc -p $(PLATFORM_DESC) $(SOCLIB_CC_ADD_ARGS) --embedded-cflags)

CC_PREFIX=$($(ARCH)_CC_PREFIX)
CC = $(CC_PREFIX)gcc
AS = $(CC_PREFIX)as
LD = $(CC_PREFIX)ld
OBJDUMP = $(CC_PREFIX)objdump

CFLAGS=-Wall  -I. $(INTERFACE_CFLAGS) $(ADD_CFLAGS) $(DEBUG_CFLAGS) $($(ARCH)_CFLAGS) -ggdb -DNIOS

MAY_CLEAN=$(shell test -r arch_stamp && (test "$(ARCH)" = "$$(cat /dev/null arch_stamp)" || echo clean))

default: clean $(SOFT_IMAGE)

$(SOFT_IMAGE): ldscript $(MAY_CLEAN) arch_stamp $(OBJS)
	$(LD) $($(ARCH)_LDFLAGS) -T $(filter %ldscript,$^) -o $@ $(filter %.o,$^)

arch_stamp:
	echo $(ARCH) > $@

%.o: %.s
	$(AS) $< -o $@

%.o : %.c
	$(CC) -o $@ $(CFLAGS) -c $<

clean :
	-rm -f $(SOFT_IMAGE) $(OBJS) arch_stamp
