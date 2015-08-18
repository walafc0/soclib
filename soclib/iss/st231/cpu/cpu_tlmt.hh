/***************************************************************************
   Cpu.hh  -  functional simulator of St231
 ***************************************************************************/

/* *****************************************************************************
                                    BSD LICENSE
********************************************************************************
Copyright (c) 2006, INRIA
Authors: Sylvain Girbal

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

#ifndef __ST231_CPU_TLMT_HH__
#define __ST231_CPU_TLMT_HH__

#include <cpu.hh>
#include <soclib_memory_op.hh>
#include <queue>
#include <bundle.hh>
#include <soclib_symtab.hh>

namespace st231{

using namespace std;

class CPU_tlmt : public CPU
{public:
  // constructor and destructor
  CPU_tlmt();
  virtual ~CPU_tlmt();

  void FetchSubOp(uint32_t addr, uint32_t &dest, int32_t &counter);
  virtual void ReadMemoryBuffer(uint32_t addr, void *buffer, uint32_t size);
  virtual void WriteMemoryBuffer(uint32_t addr, const void *buffer, uint32_t size);
  void bundle_commit();

  // SOCLIB Processor API
  void SoclibReset();
  uint32_t IsBusy();
  void Step();
  void StepCycle();
  void NullStep(uint32_t time_passed = 1);
  void GetInstructionRequest(bool &req, uint32_t &addr) const;
  void SetInstruction(bool error, uint32_t val);
  void GetDataRequest(bool &reg, bool &is_read, int &size, uint32_t &addr, uint32_t &data) const;
  void SetDataResponse(bool error, uint32_t rdata);
  void SetWriteBerr();
  void SetIrq(uint32_t irq);

  // SOCLIB Processor - debug API
  unsigned int GetDebugRegisterCount() const;
  uint32_t GetDebugRegisterValue(unsigned int reg) const;
  void SetDebugRegisterValue(unsigned int reg, uint32_t value);
  size_t GetDebugRegisterSize(unsigned int reg) const;
  uint32_t GetDebugPC() const;
  void SetDebugPC(uint32_t);
  void SetICacheInfo( size_t line_size, size_t assoc, size_t n_lines );
  void SetDCacheInfo( size_t line_size, size_t assoc, size_t n_lines );

  // Printers
  void smart_dump(std::ostream &os);
  void dump_registers(std::ostream &os);
  friend std::ostream & operator<<(std::ostream &os, const CPU_tlmt &cpu);

  std::string debug_prefix;
 private:
  uint64_t cycle;
  SymbolTable symtab;
  uint32_t fetch_cia;
  Bundle *FetchStage;
  Bundle *DecodeStage;
  Bundle *ExecuteStage;
  queue<MemoryOp*> LoadStoreQueue;
  queue<MemoryOp*> FetchQueue;
  uint32_t soclib_exception;
  int32_t load_counter;
  enum {IRQ_IRQ = 1, FIQ_IRQ = 2};
  enum {RESET_EXCEPTION = 1, UNDEFINED_INSTRUCTION_EXCEPTION = 2, SOFTWARE_INTERRUPT_EXCEPTION = 4, PREFETCH_ABORT_EXCEPTION = 8,
        DATA_ABORT_EXCEPTION = 16, IRQ_EXCEPTION = 32, FIQ_EXCEPTION = 64};
  bool something_new_to_dump;
  void update_debug_prefix();
};

}

#endif
