/***************************************************************************
   Cpu.hh  -  functional simulator of St231
 ***************************************************************************/

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

#ifndef __ST231_CPU_HH__
#define __ST231_CPU_HH__

// standard libs
#include <inttypes.h>
#include <iostream>
#include <iostream>
#include <stdio.h>

//#include "st231_isa.hh"

// genIssLib

#ifndef SOCLIB

  #include <generic/memory/memory_interface.hh>
  #include <plugins/debug/instruction_level_debug_interface.hh>
  #include <generic/bus/bus_interface.hh>
  #include <utils/registers/simple_register_interface.hh>
  #include <utils/endian/endian.hh>
  #include <generic/memory/endian_interface.hh>
  #include <utils/arithmetic/arithmetic.hh>
  #include <utils/services/service.hh>
  #include <generic/memory/memory_interface.hh>
  #include <plugins/loader/loader_interface.hh>
  #include <utils/debug/symbol_table_interface.hh>

#endif

// st231
#include "ctrlregdefs.h"
#include "exception.hh"

#ifndef SOCLIB
  #include "st231_simoscall.hh"
#endif

namespace st231{

// Exception code

//************************************************************************************************************
// STF: Les macros sont a remplacer par des static const pour éviter les conflits (et la possibilité d'avoir
// plusieurs instances. ==> static const dans le namespace.

#define STBUS_IC_EXCEPTION    	(1<<0)   
#define STBUS_DC_EXCEPTION    	(1<<1)
#define EXTERN_INT_EXCEPTION  	(1<<2)
#define IBREAK_EXCEPTION      	(1<<3)
#define ITLB_EXCEPTION        	(1<<4)
#define SBREAK_EXCEPTION      	(1<<5)
#define ILL_INST_EXCEPTION    	(1<<6)
#define SYSCALL_EXCEPTION     	(1<<7)
#define DBREAK_EXCEPTION      	(1<<8)
#define MISALIGNED_EXCEPTION  	(1<<9)
#define CREG_NO_MAPPING       	(1<<10)
#define CREG_ACCESS_VIOLATION 	(1<<11)
#define DTLB_EXCEPTION        	(1<<12)
#define SDI_EXCEPTION         	(1<<13)

// operation mode
#define PSW_USER_MODE         	(cpu->ReadMemory32(PSW) & 0x00FF0000) //????

// specific maros for immediate instruction
#define IMM_SEL_MASK(x) 	((x) & (0x7f<<23))
#define IMML_MASK 		((0x2a<<23))
#define IMMR_MASK 		((0x2b<<23))
#define IMM_MASK 		0x7fffff
#define IMM(x) 			((x)&IMM_MASK)

// stop bit
#define STOP_BIT 		(1<<31)

// Number of registers
#define NB_REGS 		64
#define NB_BITREGS 		8

#ifndef SOCLIB

  using full_system::generic::memory::MemoryInterface;
  using full_system::generic::bus::BusInterface;
  using full_system::generic::memory::endian_interface::endian_type;
  using full_system::generic::memory::endian_interface::E_BIG_ENDIAN;
  using namespace full_system::utils::endian;
  using full_system::utils::services::Client;
  using full_system::utils::services::ServiceImport;
  using full_system::utils::services::Object;
  using full_system::plugins::loader::LoaderInterface;
  using full_system::utils::debug::SymbolTableLookupInterface;
  using full_system::generic::bus::BC_GLOBAL;

#endif

using namespace std;

class CPU 
  : public Decoder
#ifndef SOCLIB
  , public Client<LoaderInterface<uint32_t> >
  , public Client<SymbolTableLookupInterface<uint32_t> >
  , public Client<MemoryInterface<uint32_t> >
  , public Client<BusInterface<uint32_t> >
#endif
{public:
#ifndef SOCLIB
	ServiceImport<LoaderInterface<uint32_t>/*, CPURegistersInterface*/> elfloader;
	ServiceImport<MemoryInterface<uint32_t> > memory;
	ServiceImport<SymbolTableLookupInterface<uint32_t> > symbol_table;
	ServiceImport<BusInterface<uint32_t> > bus_interface;
#endif

	uint32_t text_segment_start;
	uint32_t text_segment_end;

 protected:
	uint32_t cia;
	uint32_t nia;
	uint32_t gpr_c[NB_REGS];
	uint32_t gpr_n[NB_REGS];
	uint8_t gpb_c[NB_BITREGS];
	uint8_t gpb_n[NB_BITREGS];

  int nb_bundle;
  uint32_t DBreakAddr;
  uint32_t exception; /* An exception has occurred by somebody */
  // HOST SysCall address (__dotsyscall)
  uint32_t syscall_addr;

#ifndef SOCLIB
  //used for simoscall
  xst_simoscall_t ** handler;
#endif

	// extended immediate value : used for immediate instructions
	// this value is set by the "imml" and "immr" instructions
	uint32_t imm;

        // compute the L/S effective address for cycle-level simulation 
	uint32_t effective_address;


 protected:
  uint64_t instruction_counter;
  uint64_t bus_cycle;

 public:
  Operation *current_operation;
  // constructor and destructor
#ifdef SOCLIB
  CPU();
#else
  CPU(const char *name, Object *parent = 0);
  CPU(const char *name, xst_simoscall_t **handler, Object *parent=0);
#endif

  virtual ~CPU();

  // CPU operations
  virtual void Reset(); 
  void BundleStep();
#ifndef SOCLIB
  void Setup();
#endif

  // for pc
  inline uint32_t GetCia(){return cia;}
  inline uint32_t GetNia(){return nia;}
  inline void SetCia(uint32_t pc){cia = pc;}
  inline void SetNia(uint32_t pc)
  { //if( (pc<text_segment_start) || (pc>text_segment_end) )
    //{ cerr << hex << "Supicious branch jumping out of text section (to @" << pc << ") at @" << cia << dec << " !" << endl;
    //  exit(-1);
    //}
    nia = pc;
  }

#ifndef SOCLIB
	// for syscall
	inline void SetSyscallAddr(uint32_t ea){syscall_addr = ea;};
	inline uint32_t GetSyscallAddr(){ return syscall_addr;};
        void simoscall_emulation(){
               xiss_uint32_t slice_id = 0;
               xiss_uint32_t thread_id = 0;
               xst_simoscall_emulate((void *)this, *handler,slice_id,thread_id);
	};
#endif

  // for Ifetch 
  virtual void Fetch(void *buffer, uint32_t addr, uint32_t size);

#ifndef SOCLIB
  /* Memory interface */
  virtual bool ReadMemory(uint32_t addr, void *buffer, uint32_t size);
  virtual bool WriteMemory(uint32_t addr, const void *buffer, uint32_t size);
#endif

       /* Methods for accessing various registers */
        inline int32_t GetGPR_C(int n) const { return gpr_c[n]; }
        inline int32_t GetGPR_N(int n) const { return gpr_n[n]; }
        inline int8_t GetGPB_C(int n) const { if(gpb_c[n])return 1; return 0; }
        inline int8_t GetGPB_N(int n) const { if(gpb_n[n])return 1; return 0; }
        inline void SetGPR_N(int n, uint32_t value) { 
          if(n==0) {
            //cout<<"Warning R0 === 0 "<<endl; 
            return;
          }
          gpr_n[n] = value; 
        }
        inline void SetGPR_C(int n, uint32_t value) { 
          if(n==0) { return; }
          gpr_c[n] = value; 
        }
        inline void SetGPB_N(int n, uint8_t value) { if(value)  gpb_n[n] = 1; else  gpb_n[n] = 0; }

	// for Icache
	void invalidate_icache(void);

	// for exception
        inline uint32_t GetException(void) { return exception; }
        inline void SetException(int value) { exception = value; }
	inline void invalidate_last_tlb_cache();
	inline void ThrowDBreak(uint32_t address);
	inline void ThrowCRegAccessViolation(uint32_t address);
	inline void ThrowIllInst();
	inline void ThrowCRegNoMapping(uint32_t address);
        inline void ThrowMisAligned_Trap(uint32_t address);
        inline void ThrowDTLB(uint32_t address);
	inline void HandleException(const DBreakException & exc);
	inline void HandleException(const CRegAccessViolationException & exc);
	inline void HandleException(const IllInstException & exc);
	inline void HandleException(const CRegNoMappingException & exc);
	inline void HandleException(const MisAlignedException & exc);
	inline void HandleException(const DTLBException & exc);
  // for memory acces
  virtual void ReadMemoryBuffer(uint32_t addr, void *buffer, uint32_t size);
  virtual void WriteMemoryBuffer(uint32_t addr, const void *buffer, uint32_t size);
	inline bool UndefinedCReg(uint32_t address);
	inline uint32_t CRegIndex(uint32_t address);
	inline bool CRegReadAccessViolation(uint32_t index);
        // compute the L/S effective address for cycle-level simulation 
	inline void SetEA(uint32_t ea) { effective_address = ea; }
        inline uint32_t GetEA() { return effective_address; }

        inline bool MisAligned8(uint32_t a);
        inline bool MisAligned16(uint32_t a);
        inline bool MisAligned32(uint32_t a);
        inline bool TLB_ENABLE();
        inline bool MultiMapping(uint32_t a);
        inline bool NoTranslation(uint32_t a);
        inline bool ReadAccessViolation(uint32_t a);


	inline void ReadCheckCReg(uint32_t address);
	inline void ReadCheckMemory8(uint32_t address);
	inline void ReadCheckMemory16(uint32_t address);
	inline void ReadCheckMemory32(uint32_t address);

	inline int32_t ReadCReg(uint32_t address);
	inline int8_t ReadMemory8(uint32_t address);
	inline int16_t ReadMemory16(uint32_t address);
	inline int32_t ReadMemory32(uint32_t address);

	inline void DisReadCheckMemory8(uint32_t address);
	inline void DisReadCheckMemory16(uint32_t address);
	inline void DisReadCheckMemory32(uint32_t address);
	inline int8_t DisReadMemory8(uint32_t address);
	inline int16_t DisReadMemory16(uint32_t address);
	inline int32_t DisReadMemory32(uint32_t address);
	inline void WriteCheckCReg(uint32_t address);
	inline void WriteCheckMemory8(uint32_t address);
	inline void WriteCheckMemory16(uint32_t address);
	inline void WriteCheckMemory32(uint32_t address);
	inline void WriteMemory8(uint32_t address,int32_t value);
	inline void WriteMemory16(uint32_t address,int32_t value);
	inline void WriteMemory32(uint32_t address,int32_t value);
	inline void WrirteCReg(uint32_t address,int32_t value);

	// for tlb
	inline void PrefetchCheckMemory(uint32_t address);
	inline void PrefetchMemory(uint32_t address);
	inline void PurgeAddressCheckMemory(uint32_t address);
	inline void PurgeAddress(uint32_t address);
	inline void PurgeSet(uint32_t address);
	inline void PurgeInsPg(uint32_t address);

	// for psw
	inline void PswClr(uint32_t value);
	inline void PswSet(uint32_t value);
	inline bool IsDBreakHit(uint32_t address);
	inline bool IsCRegSpace(uint32_t address);
	inline bool IsDBreakEnable();

	// for immediate instructions
        inline void setImm(int32_t i);
	inline int32_t Imm(int32_t op2); 

#ifndef SOCLIB
  // for syscalls
  void sim_syscall(void);
  void sim_exit(void);
  void sim_fstat(void);
  void sim_open(void);
  void sim_read(void);
  void sim_write(void);
#endif

  void cpy_cpu_state(CPU *another_cpu);
  inline void updateRegs();
};

inline void CPU::invalidate_last_tlb_cache()
{
//	last_itlb=last_dtlb=NULL;
}

inline bool CPU::IsDBreakEnable()
{ return false;
}

inline bool CPU::IsDBreakHit(uint32_t address)
{
	// Result is TRUE if address will trigger a data breakpoint,otherwise it is FALSE
	if( IsDBreakEnable() && address == DBreakAddr )
		return true;
	return false;
}

inline bool CPU::IsCRegSpace(uint32_t address)
{
	if(address >= CTRL_REG_START) //&& (address < CTRL_REG_START + CTRL_REG_SIZE) )
		return true;
	return false;
}

inline void CPU::ThrowDBreak(uint32_t address)
{
	throw DBreakException(address);
}

inline void CPU::ThrowCRegAccessViolation(uint32_t address)
{
	throw CRegAccessViolationException(address);
}

inline void CPU::ThrowIllInst()
{
	throw IllInstException();
}

inline void CPU::ThrowCRegNoMapping(uint32_t address)
{
	throw CRegNoMappingException(address);
}


inline void CPU::HandleException(const DBreakException & exc)
{ 
#ifdef SOCLIB
  cerr << exc.what() << endl;
#else
  cerr << Object::GetName() << ": " << exc.what() << endl;
#endif
}

inline void CPU::HandleException(const CRegAccessViolationException & exc)
{ 
#ifdef SOCLIB
  cerr << exc.what() << endl;
#else
  cerr << Object::GetName() << ": " << exc.what() << endl;
#endif

}

inline void CPU::HandleException(const IllInstException & exc)
{
#ifdef SOCLIB
  cerr << exc.what() << endl;
#else
  cerr << Object::GetName() << ": " << exc.what() << endl;
#endif

}

inline void CPU::HandleException(const CRegNoMappingException & exc)
{
#ifdef SOCLIB
  cerr << exc.what() << endl;
#else
  cerr << Object::GetName() << ": " << exc.what() << endl;
#endif
}

inline void CPU::HandleException(const MisAlignedException & exc)
{ 
#ifdef SOCLIB
  cerr << exc.what() << endl;
#else
  cerr << Object::GetName() << ": " << exc.what() << endl;
#endif
}

inline void CPU::HandleException(const DTLBException & exc)
{ 
#ifdef SOCLIB
  cerr << exc.what() << endl;
#else
  cerr << Object::GetName() << ": " << exc.what() << endl;
#endif
}

// to complete
inline bool CPU::UndefinedCReg(uint32_t address)
{ return false;
}

// to complete
inline uint32_t CPU::CRegIndex(uint32_t address)
{ return 0;
}

// to complete
inline bool CPU::CRegReadAccessViolation(uint32_t index)
{ return false;
}

// to complete
inline void CPU::ReadCheckCReg(uint32_t address)
{
        if(UndefinedCReg(address))
		ThrowCRegNoMapping(address);

	uint32_t index = CRegIndex(address);
	if(CRegReadAccessViolation(index))
		ThrowCRegAccessViolation(address);
}
	
//to complete
inline bool CPU::MisAligned8(uint32_t a)
{
	return false;
}

//to complete
inline bool CPU::MisAligned16(uint32_t a)
{
	return false;
}

//to complete
inline bool CPU::MisAligned32(uint32_t a)
{
	return false;
}

//to complete
inline bool CPU::TLB_ENABLE()
{
	return false;
}

inline void CPU::ThrowMisAligned_Trap(uint32_t address)
{
	throw MisAlignedException(address);
}

//to complete
inline bool CPU::MultiMapping(uint32_t a)
{
	return false;
}

//to complete
inline bool CPU::NoTranslation(uint32_t a)
{
	return false;
}

//to complete
inline bool CPU::ReadAccessViolation(uint32_t a)
{
	return false;
}

inline void CPU::ThrowDTLB(uint32_t address)
{
	throw DTLBException(address);
}

inline void CPU::ReadCheckMemory8(uint32_t a)
{
	if (MisAligned8(a))
		ThrowMisAligned_Trap(a);
	if (TLB_ENABLE()){
		if(NoTranslation(a) || MultiMapping(a) || ReadAccessViolation(a)){
			ThrowDTLB(a);
		}
	}

}
	
inline void CPU::ReadCheckMemory16(uint32_t a)
{
	if (MisAligned16(a))
		ThrowMisAligned_Trap(a);
	if (TLB_ENABLE()){
		if(NoTranslation(a) || MultiMapping(a) || ReadAccessViolation(a)){
			ThrowDTLB(a);
		}
	}
}

inline void CPU::ReadCheckMemory32(uint32_t a)
{
	if (MisAligned32(a))
		ThrowMisAligned_Trap(a);
	if (TLB_ENABLE()){
		if(NoTranslation(a) || MultiMapping(a) || ReadAccessViolation(a)){
			ThrowDTLB(a);
		}
	}
}
	
	
inline int32_t CPU::ReadCReg(uint32_t address)
{ return 0; // ?????????????????????????????????????????????????
}

inline int8_t CPU::ReadMemory8(uint32_t address)
{
	uint8_t value;
	ReadMemoryBuffer(address, &value, 1);
	return value;
}

inline int16_t CPU::ReadMemory16(uint32_t address)
{
	uint16_t value;
	ReadMemoryBuffer(address, &value, 2);
	return value;
}
	
inline int32_t CPU::ReadMemory32(uint32_t address)
{
	uint32_t value;
	ReadMemoryBuffer(address, &value, 4);
	return value;
}	

inline void CPU::DisReadCheckMemory8(uint32_t address)
{
}
	
inline void CPU::DisReadCheckMemory16(uint32_t address)
{
}
	
inline void CPU::DisReadCheckMemory32(uint32_t address)
{
}
	
inline int8_t CPU::DisReadMemory8(uint32_t address)
{
	uint8_t value;
	ReadMemoryBuffer(address, &value, 1);
	return value;
}
	
inline int16_t CPU::DisReadMemory16(uint32_t address)
{
	uint16_t value;
	ReadMemoryBuffer(address, &value, 2);
	return value;
}
	
inline int32_t CPU::DisReadMemory32(uint32_t address)
{
	uint32_t value;
	ReadMemoryBuffer(address, &value, 4);
	return value;
}
	
inline void CPU::WriteCheckCReg(uint32_t address)
{}
	
inline void CPU::WriteCheckMemory8(uint32_t address)
{}
	
inline void CPU::WriteCheckMemory16(uint32_t address)
{}
	
inline void CPU::WriteCheckMemory32(uint32_t address)
{}
	
inline void CPU::WriteMemory8(uint32_t ea,int32_t value)
{
	WriteMemoryBuffer(ea, &value, 1);
}
	
inline void CPU::WriteMemory16(uint32_t ea,int32_t value)
{
	WriteMemoryBuffer(ea, &value, 2);
}
	
inline void CPU::WriteMemory32(uint32_t ea,int32_t value)
{
	WriteMemoryBuffer(ea, &value, 4);
}
	
inline void CPU::WrirteCReg(uint32_t address,int32_t value)
{}
	
	
inline void CPU::PrefetchCheckMemory(uint32_t address)
{}
	
inline void CPU::PrefetchMemory(uint32_t address)
{}
	
inline void CPU::PurgeAddressCheckMemory(uint32_t address)
{}
	
inline void CPU::PurgeAddress(uint32_t address)
{}
	
inline void CPU::PurgeSet(uint32_t address)
{}
	
inline void CPU::PurgeInsPg(uint32_t address)
{}
	
inline void CPU::PswClr(uint32_t value)
{}
	
inline void CPU::PswSet(uint32_t value)
{}
	
inline void CPU::setImm(int32_t i)
{  imm = i;
}

inline int32_t CPU::Imm(int32_t op2) 
{  if(imm)
   {  op2 |= imm<<9 ;
      imm = 0;
   }
   else if (op2 & (1<<8))
   {  op2 |= (0x7fffff<<9);
   }
   return op2;
}

inline int32_t SignEx9to32(int32_t op2) 
{  if (op2 & (1<<8))
   {  op2 |= (0x7fffff<<9);
   }
   return op2;
}

inline int32_t SignEx23to32(int32_t op2) 
{  if (op2 & (1<<22))
   {  op2 |= (0x1ff<<23);
   }
   return op2;
}

inline void CPU::updateRegs()
{
    memcpy((void *)gpr_c, (void *)gpr_n, 4 * NB_REGS);
    memcpy(gpb_c, gpb_n, NB_BITREGS);
}

} // end of namespace st231

#endif
