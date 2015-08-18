/***************************************************************************
   Cpu.cpp  -  functional simulator of St231
 ***************************************************************************/

/* *****************************************************************************
                                    BSD LICENSE
********************************************************************************
Copyright (c) 2006, INRIA
Authors: Sylvain Girbal (sylvain.girbal@inria.fr)

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
#include <cpu_tlmt.hh>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>

#include "debug_switch.h"

//#define SOCLIB_DEBUG

using namespace std;

namespace st231
{

// === CONSTRUCTORS & DESTRUCTOR ==============================================

/**
 * \brief Constructor
 */
CPU_tlmt::CPU_tlmt() : CPU()
{ load_counter = 0;
  symtab = SymbolTable(mapfile);
  fetch_cia = 0;
  something_new_to_dump = true;
  cycle=0;
  update_debug_prefix();
}

/**
 * \brief Destructor
 */
CPU_tlmt::~CPU_tlmt()
{ cerr << debug_prefix << "end." << endl;
}

// ============================================================================

void CPU_tlmt::ReadMemoryBuffer(uint32_t addr, void *buffer, uint32_t size)
{ MemoryOp *memop = new MemoryOp(addr, size, /*dest=*/gpr_n[current_operation->loadstore_target_register(this)], load_counter, /*aligned=*/true, /*signed=*/false);
  LoadStoreQueue.push(memop);
  load_counter++;
}

// write "size" bytes to "addr"
void CPU_tlmt::WriteMemoryBuffer(uint32_t addr, const void *buffer, uint32_t size)
{ uint32_t *value  = (uint32_t*)buffer;
  MemoryOp *memop = new MemoryOp(addr, size, *value, load_counter);
  LoadStoreQueue.push(memop);
  load_counter++;
}

void CPU_tlmt::FetchSubOp(uint32_t addr, uint32_t &dest, int32_t &counter)
{ if(addr==0)
  { cerr << std::dec << debug_prefix << "main returned with exitcode " << gpr_n[16]<< endl;
//    dump_registers(cerr);
    exit(0);
  }
  MemoryOp *memop = new MemoryOp(addr, 4, dest, counter, /*aligned=*/true, /*signed=*/false);
  FetchQueue.push(memop);
}

void CPU_tlmt::bundle_commit()
{ cia = nia;
  memcpy((void *)gpr_c, (void *)gpr_n, 4 * NB_REGS);
  memcpy(gpb_c, gpb_n, NB_BITREGS);
}

// === SOCLIB INTERFACES ======================================================

void CPU_tlmt::SoclibReset() 
{ Reset();
  soclib_exception = 0;
  cia = symtab["main"];
  nia = symtab["main"];
  fetch_cia = cia;
  FetchStage=NULL;
  DecodeStage=NULL;
  ExecuteStage=NULL;
  something_new_to_dump = true;
  cycle=0;
  update_debug_prefix();
}

// ----------------------------------------------------------------------------

uint32_t CPU_tlmt::IsBusy() 
{ //TODO ?
  return 0;
}

// ----------------------------------------------------------------------------

void CPU_tlmt::NullStep(uint32_t time_passed) 
{
//#ifdef SOCLIB_DEBUG
//  cerr << "+++ NullStep(" << time_passed << ")" << endl;
//#endif
  for(uint32_t i = 0; i < time_passed; i++)
  { update_debug_prefix();
    StepCycle();
    cycle++;
  }
}

// ----------------------------------------------------------------------------

void CPU_tlmt::Step() 
{
//#ifdef SOCLIB_DEBUG
//  cerr << "+++ Step()" << endl;
//#endif
  update_debug_prefix();
  StepCycle();
  cycle++;
}

// ----------------------------------------------------------------------------

void CPU_tlmt::StepCycle() 
{
#ifdef SOCLIB_DEBUG
//  cerr << debug_prefix << "\e[1;31mStepCycle()\e[0m" << endl;
  smart_dump(cerr);
#endif

  if(ExecuteStage)
  { if(!ExecuteStage->executed())
    { //cerr << "execute:" << *ExecuteStage << endl;
      ExecuteStage->Execute();
      if(load_counter==0) // No load / store pending
      { delete ExecuteStage;
        ExecuteStage = NULL;
      }
      something_new_to_dump = true;
    }
    else 
    { // Instruction already executed, with a pending load/store
      if(load_counter==0)
      { delete ExecuteStage;
        ExecuteStage = NULL;
        something_new_to_dump = true;
      }
      else
      { //cerr << debug_prefix << load_counter << " pending load/stores." << endl;
      }
    }
  }

  if(DecodeStage)
  { if(!DecodeStage->decoded())
    { DecodeStage->Decode();
      //cerr << "decoded: " << *DecodeStage << endl;
      something_new_to_dump = true;
    }
    if(DecodeStage->decoded() && !ExecuteStage)
    { ExecuteStage = DecodeStage;
      DecodeStage = NULL;
      something_new_to_dump = true;
    }
  }

  if(!FetchStage)
  { // if the fetch stage is empty, start a new fetch
    if(!DecodeStage && !ExecuteStage)
    { FetchStage = new Bundle(cia,this);
      FetchStage->Fetch();
      something_new_to_dump = true;
    }
  }
  else // FetchedStage not null
  { // Fetch stage is full, move to decode when VLIW word fully fetched
    if(FetchStage->fetched() && !DecodeStage)
    { DecodeStage = FetchStage;
      FetchStage = NULL;
      something_new_to_dump = true;
    }
  }
}

// ----------------------------------------------------------------------------

void CPU_tlmt::GetInstructionRequest(bool &req, uint32_t &addr) const 
{ // check if the instruction on the fetch queue has been requested,
  // if not then return the address to fetch
  if(FetchQueue.empty()) 
  { req = false;
    return;
  }
  MemoryOp *memop = FetchQueue.front();
  if(memop->GetSize()!=4)
  { cerr << "unexpected fetch size" << endl;
    exit(-1);
  }
  if(memop->GetType()!=MemoryOp::READ) 
  { cerr << "unexpected fetch type" << endl;
    exit(-1);
  }
  addr = memop->GetAddress();
  req = true;
//#ifdef SOCLIB_DEBUG
//  cerr << debug_prefix << "GetInstructionRequest @" << hex << addr << dec<< endl;
//#endif
}

// ----------------------------------------------------------------------------

void CPU_tlmt::SetInstruction(bool error, uint32_t val) 
{ /* set the instruction opcode using val, now it could be a good moment to
   *   decode the instruction */
  if(error) {
    // TODO: what to do in case of a bus error
    cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
      << "): an error was received. What to do?" << endl;
    exit(-1);
  }
  if(FetchQueue.empty()) 
  { cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
         << "Received instruction in response that was not expected" << endl;
    exit(-1);
  }
  MemoryOp *memop = FetchQueue.front();
  FetchQueue.pop();
  memop->dest = val; // dest is now a reference to the requesting bundle slot.
  memop->counter++;
  delete(memop);
  something_new_to_dump = true;
#ifdef SOCLIB_DEBUG
//  cerr << debug_prefix << "SetInstruction val = 0x" << hex << val << dec << ")" << endl;
#endif
}

// ----------------------------------------------------------------------------

void CPU_tlmt::GetDataRequest(bool &reg, bool &is_read, int &size, uint32_t &addr, uint32_t &data) const
{ MemoryOp *memop = NULL;
  reg = false;
  /* check if the instruction in the execute queue has any load/store to
   * be done, if so, execute it. */
  if(LoadStoreQueue.empty()) 
  { reg = false;
    return;
  }
#ifdef SOCLIB_DEBUG
//  cerr << debug_prefix << "GetDataRequest" << endl;
#endif
  memop = LoadStoreQueue.front();
  reg = true;
  size = memop->GetSize();
  addr = memop->GetAddress();
  switch(memop->GetType()) 
  { case MemoryOp::READ:
    case MemoryOp::PREFETCH:
      is_read = true;
      break;
    case MemoryOp::WRITE:
      is_read = false;
      data = memop->GetWriteValue();
      break;
  }
#ifdef SOCLIB_DEBUG
  cerr << "    memory operation ready:" << endl;
  cerr << "    - " << (is_read?"read":"write") << endl;
  cerr << "    - size = " << size << endl;
  cerr << "    - address = 0x" << std::hex << addr << std::dec << endl;
  if(!is_read) cerr << "    - data = " << hex << data << dec << endl;
#endif
}

// ----------------------------------------------------------------------------

void CPU_tlmt::SetDataResponse(bool error, uint32_t rdata) 
{ /* set the data using rdata, to the register indicated by the instruction
   * in the execute queue. */
  if(error) 
  { cerr << "TODO(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): " 
         << "Received a data response with an error. rdata=" << rdata << endl;
    exit(-1);
  }
  if(LoadStoreQueue.empty()) 
  { cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
         << "Received data in response that was not expected" << endl;
    exit(-1);
  }
#ifdef SOCLIB_DEBUG
  cerr << debug_prefix << "SetDataResponse" << endl;
#endif
  MemoryOp *memop = LoadStoreQueue.front();
  LoadStoreQueue.pop();
  switch(memop->GetType()) {
    case MemoryOp::WRITE:
      memop->counter--;
      break;
    case MemoryOp::READ:
    { //int dest = memop->GetTargetReg();
      //SetGPR_N(dest, rdata);
      memop->dest = rdata; // dest is now a reference to the target register.
      memop->counter--;
    } break;
    default:
      cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
           << "Unhandled swith case !" << endl;
      exit(-1);
  }
  delete(memop);
  something_new_to_dump = true;
}

void CPU_tlmt::SetWriteBerr() 
{ //cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
  //     << "Writing to an invalid address" << endl;
  // exit(-1);
}

// ----------------------------------------------------------------------------

void CPU_tlmt::SetIrq(uint32_t irq) 
{ // Checks the irq range, if out of range, then display error and stop.
  // If irq received then set the correspondent exception flag (exception member variable).
  // List of avalaible interrupts:
  // - irq 1 (IRQ_IRQ) == normal IRQ
  // - irq 2 (FIQ_IRQ) == fast IRQ
  if(irq) 
  { if(irq == IRQ_IRQ) 
    { soclib_exception |= IRQ_EXCEPTION;
    } 
    else if(irq == FIQ_IRQ) 
    { soclib_exception |= FIQ_EXCEPTION;
    } 
    else 
    { cerr << "ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
           << "Received unknow interruption type (irq = 0x" << std::hex << irq << std::dec << ")" << endl;
      exit(-1);
    }
  }
}

// ----------------------------------------------------------------------------

unsigned int CPU_tlmt::GetDebugRegisterCount() const 
{ return NB_REGS;
}

uint32_t CPU_tlmt::GetDebugRegisterValue(unsigned int reg) const 
{ return GetGPR_C(reg);
}

void CPU_tlmt::SetDebugRegisterValue(unsigned int reg, uint32_t value) 
{ SetGPR_N(reg, value);
}

size_t CPU_tlmt::GetDebugRegisterSize(unsigned int reg) const 
{ // TODO 
  return 0;
}

uint32_t CPU_tlmt::GetDebugPC() const 
{ // TODO
  return 0;
}

void CPU_tlmt::SetDebugPC(uint32_t new_pc) 
{ // TODO
}

void CPU_tlmt::SetICacheInfo( size_t line_size, size_t assoc, size_t n_lines ) 
{ // TODO
}

void CPU_tlmt::SetDCacheInfo( size_t line_size, size_t assoc, size_t n_lines )
{ // TODO
}

void CPU_tlmt::smart_dump(ostream &os)
{ if(something_new_to_dump)
  { os << *this;
    something_new_to_dump = false;
  }
}

void CPU_tlmt::dump_registers(std::ostream &os)
{ os << debug_prefix << "C: ";
  for(int i=0; i<NB_REGS; i++)
  { os <<" R" << i << "=" << gpr_c[i] << " ";
  }
  os << "\n" << debug_prefix << "N: ";
  for(int i=0; i<NB_REGS; i++)
  { os <<" R" << i << "=" << gpr_n[i] << " ";
  }
  os << endl;
}

ostream & operator<<(ostream &os, const CPU_tlmt &cpu)
{ os << cpu.debug_prefix << "F: ";
  if(cpu.FetchStage) cpu.FetchStage->dump_fetch(os);
  else os << "-";
  os << "\n" << cpu.debug_prefix << "D: ";
  if(cpu.DecodeStage) cpu.DecodeStage->dump_decode(os);
  else os << "-";
  os << "\n" << cpu.debug_prefix << "E: ";
  if(cpu.ExecuteStage) cpu.ExecuteStage->dump_execute(os);
  else os << "-";
  os << endl;
  return os;
}

void CPU_tlmt::update_debug_prefix()
{ stringstream ss;
  ss << left << setw(8) << cycle;
  debug_prefix = ss.str();
}

} // end of namespace st231
