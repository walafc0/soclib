################################################################################
#                                   BSD LICENSE
################################################################################
#  Copyright (c) 2008, INRIA
#  All rights reserved.
#  Authors: Sylvain Girbal
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

GENISSLIB := genisslib

all: cpu/st231_isa.cpp cpu/st231_isa.hh

# --- ISA related --------------------------------------------------------------

isa:
	svn export https://unisim.org/svn/projects/st231/trunk/src/isa

#genisslib:
#	svn co https://unisim.org/svn//public/tools/GenISSLib/trunk genisslib

#$(GENISSLIB): genisslib/Makefile
#	$(MAKE) -C genisslib

#genisslib/Makefile: genisslib
#	cd genisslib; ./configure

cpu/st231_isa.cpp cpu/st231_isa.hh: isa cpu isa/st231.isa $(wildcard isa/*.isa)
	$(GENISSLIB) -Iisa -w 32 -o cpu/st231_isa isa/st231.isa

# --- CPU related --------------------------------------------------------------

cpu:
	svn export https://unisim.org/svn/projects/st231/trunk/src/functional_simulator cpu

# --- cleans -------------------------------------------------------------------

clean:
	rm -rf st231_isa.cpp st231_isa.hh

# ------------------------------------------------------------------------------
