################################################################################
#                                   BSD LICENSE
################################################################################
#  Copyright (c) 2008, INRIA
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification,
#  are permitted provided that the following conditions are met:
#
#   - Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
#   - Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#   - Neither the name of the INRIA nor the names of its contributors may be used
#     to endorse or promote products derived from this software without specific
#     prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED.
#  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
#  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
#  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
################################################################################

GENISSLIB := genisslib/src/genisslib
ISADIR    := isa

SOCLIB_LIB_ST231 := ${SOCLIB}/soclib/lib/st231

# ------------------------------------------------------------------------------

SOCLIB_CC_ADD_ARGS := -v

AM_CPPFLAGS+=-D__STDC_CONSTANT_MACROS
ARCH=arm966
SIMULATION_ARGS=1320000
SOCLIB?=$(shell soclib-cc --getpath)
ifndef UNISIM
NO_TEST=External unisim library needed
endif
export SOCLIB

aaa: all

include $(SOCLIB)/soclib/platform/topcells/all.mk

# ------------------------------------------------------------------------------

isa:
#	svn export https://unisim.org/svn/projects/st231/trunk/src/isa
	svn co https://unisim.org/svn/projects/st231/trunk/src/isa

genisslib:
	svn co https://unisim.org/svn//public/tools/GenISSLib/trunk genisslib

$(GENISSLIB): genisslib/Makefile
	$(MAKE) -C genisslib

genisslib/Makefile: genisslib
	cd genisslib; ./configure

st231.cpp st231.hh: isa $(GENISSLIB) $(ISADIR)/st231.isa
	$(GENISSLIB) -I$(ISADIR) -w 32 -o st231 $(ISADIR)/st231.isa

distclean: clean
	rm -rf isa genisslib st231.cpp st231.hh simulation.x

$(SOCLIB_LIB_ST231):
	svn co https://unisim.org/svn/projects/soclib/lib-st231 $(SOCLIB_LIB_ST231)
