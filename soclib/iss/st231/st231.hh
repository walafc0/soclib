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

#ifndef _SOCLIB_ST231_ISS_H_
#define _SOCLIB_ST231_ISS_H_

#include "iss.h" 
#include "cpu/st231_isa.hh"
#include "cpu/cpu_tlmt.hh"

namespace soclib {
namespace common {

class ST231iss
  : protected st231::CPU_tlmt
  , public soclib::common::Iss 
{ 
 private:
  typedef uint32_t address_t;

 public:
  static const int n_irq = 2;

  ST231iss(uint32_t ident);
  ~ST231iss();

  /************************************************************************/
  /* Methods required by the ISS Wrapper                            START */
  /************************************************************************/

  void reset();
  uint32_t isBusy();
  void step();
  void nullStep( uint32_t time_passed = 1 );
  void getInstructionRequest(bool &req, uint32_t &addr) const;
  void setInstruction(bool error, uint32_t val);
  void getDataRequest(bool &req, enum DataAccessType &type, uint32_t &addr, uint32_t &data) const;
  void setDataResponse(bool error, uint32_t rdata);
  void setWriteBerr();
  void setIrq(uint32_t irq);

  /************************************************************************/
  /* Methods required by the ISS Wrapper (Debugger)                       */
  /************************************************************************/

  // processor internal registers access API, used by
  // debugger. Register numbering must match gdb packet order.
  unsigned int getDebugRegisterCount() const;
  uint32_t getDebugRegisterValue(unsigned int reg) const;
  void setDebugRegisterValue(unsigned int reg, uint32_t value);
  size_t getDebugRegisterSize(unsigned int reg) const;
  uint32_t getDebugPC() const;
  void setDebugPC(uint32_t);
  void setICacheInfo( size_t line_size, size_t assoc, size_t n_lines );
  void setDCacheInfo( size_t line_size, size_t assoc, size_t n_lines );

};

} // end of namespace common
} // end of namespace soclib

#endif // _SOCLIB_ST231_ISS_H_

