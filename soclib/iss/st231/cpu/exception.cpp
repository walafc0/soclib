/* *****************************************************************************
                                    BSD LICENSE
********************************************************************************
Copyright (c) 2006, INRIA
Authors: Zheng LI (zheng.x.li@inria.fr)

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 - Neither the name of the INRIA nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************** */

#include <st231_isa.hh>
#include <unistd.h>

namespace st231 {

//using full_system::utils::debug::Symbol;
	
SystemResetException::SystemResetException()
{
}

const char * SystemResetException::what () const throw ()
{
	return "system reset exception";
}

MemException::MemException(const char *name, uint32_t addr)
{
	stringstream sstr;
	this->addr = addr;
	sstr.setf(ios::right | ios::hex | ios::showbase);
	sstr << "Mem " << name << " exception at " << hex << addr;
	what_str = sstr.str();
}

MemException::~MemException() throw()
{
}

const char * MemException::what () const throw ()
{
	return what_str.c_str();
}

uint32_t MemException::GetAddr() const
{
	return addr;
}

IllInstException::IllInstException()
{
}

const char * IllInstException::what () const throw ()
{
	return "Illegal instruction exception";
}

DBreakException::DBreakException(uint32_t addr) : MemException("Access to the data Break point", addr)
{
}

CRegAccessViolationException::CRegAccessViolationException(uint32_t addr) : MemException("Control register access violation", addr)
{
}

CRegNoMappingException::CRegNoMappingException(uint32_t addr) : MemException("Control register non mapping", addr)
{
}

MisAlignedException::MisAlignedException(uint32_t addr) : MemException("Address not aligned", addr)
{
}

DTLBException::DTLBException(uint32_t addr) : MemException("DTLB Error", addr)
{
}

} // end of namespace st231
