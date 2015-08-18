/////////////////////////////////////////////////////////////////////////////////
//                                   BSD LICENSE
/////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2008, INRIA
//  All rights reserved.
//  Authors: Sylvain Girbal
//
//  Redistribution and use in source and binary forms, with or without modification,
//  are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright notice, this
//     list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   - Neither the name of the INRIA nor the names of its contributors may be used
//     to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED.
//  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
//  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////

#include "st231.hh"

namespace soclib { namespace common {

namespace {

static inline std::string mkname(uint32_t no)
{
    char tmp[32];
    snprintf(tmp, 32, "arm966es_iss%d", (int)no);
    return std::string(tmp);
}

}

typedef st231::CPU_tlmt inherited_CPU;

ST231iss::ST231iss(uint32_t ident) 
  : inherited_CPU(/*this*/)
  , Iss(mkname(ident), ident) 
{ 
}

ST231iss::~ST231iss() {}

/************************************************************************/
/* Methods required by the ISS Wrapper                                  */
/************************************************************************/

// ISS <-> Wrapper API
void ST231iss::reset() 
{ // reinitialize variables to its initial state
  inherited_CPU::SoclibReset();
}

uint32_t ST231iss::isBusy() 
{ return inherited_CPU::IsBusy();
}

void ST231iss::step() 
{ inherited_CPU::Step();
}

void ST231iss::nullStep( uint32_t time_passed) 
{ inherited_CPU::NullStep(time_passed);
}

void ST231iss::getInstructionRequest(bool &req, uint32_t &addr) const 
{ inherited_CPU::GetInstructionRequest(req, addr);
}

void ST231iss::setInstruction(bool error, uint32_t val) 
{ inherited_CPU::SetInstruction(error, val);
}

void ST231iss::getDataRequest(bool &req, enum DataAccessType &type, uint32_t &addr, uint32_t &data) const 
{ bool is_read;
	int size;
	inherited_CPU::GetDataRequest(req, is_read, size, addr, data);
	if(req) {
		if(is_read) {
			switch(size) {
			case 1:
				type = READ_BYTE;
				break;
			case 2:
				type = READ_HALF;
				break;
			case 4:
				type = READ_WORD;
				break;
			default:
				std::cerr << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": ERROR: invalid read size (" << size << ")" << std::endl;
				exit(-1);
				break;
			}
		} else {
			switch(size) {
			case 1:
				type = WRITE_BYTE;
				break;
			case 2:
				type = WRITE_HALF;
				break;
			case 4:
				type = WRITE_WORD;
				break;
			default:
				std::cerr << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ": ERROR: invalid write size (" << size << ")" << std::endl;
				exit(-1);
				break;
			}
		}
	}
}

void ST231iss::setDataResponse(bool error, uint32_t rdata) 
{ inherited_CPU::SetDataResponse(error, rdata);
}

void ST231iss::setWriteBerr() 
{ inherited_CPU::SetWriteBerr();
}

void ST231iss::setIrq(uint32_t irq) 
{ inherited_CPU::SetIrq(irq);
}

unsigned int ST231iss::getDebugRegisterCount() const 
{ return inherited_CPU::GetDebugRegisterCount();
}

uint32_t ST231iss::getDebugRegisterValue(unsigned int reg) const 
{ return inherited_CPU::GetDebugRegisterValue(reg);
}

void ST231iss::setDebugRegisterValue(unsigned int reg, uint32_t value) 
{ inherited_CPU::SetDebugRegisterValue(reg, value);
}

size_t ST231iss::getDebugRegisterSize(unsigned int reg) const 
{ return inherited_CPU::GetDebugRegisterSize(reg);
}

uint32_t ST231iss::getDebugPC() const 
{ return GetDebugPC();
}

void ST231iss::setDebugPC(uint32_t pc) 
{ inherited_CPU::SetDebugPC(pc);
}

void ST231iss::setICacheInfo( size_t line_size, size_t assoc, size_t n_lines ) 
{ inherited_CPU::SetICacheInfo(line_size, assoc, n_lines);
}

void ST231iss::setDCacheInfo( size_t line_size, size_t assoc, size_t n_lines ) 
{ inherited_CPU::SetDCacheInfo(line_size, assoc, n_lines);
}

}
}

