/***************************************************************************
   Cpu.cpp  -  functional simulator of St231
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
#include <st231_isa.hh>
#include <cpu.hh>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug_switch.h"

namespace st231
{
using namespace std;

#ifndef SOCLIB
  using full_system::plugins::debug::DBG_STEP;
  using full_system::plugins::debug::DBG_KILL;
  using full_system::plugins::debug::DBG_RESET;
  using full_system::plugins::debug::DebugCommand;
  using full_system::generic::bus::BC_GLOBAL;
  using full_system::generic::bus::BC_NONE;
  using full_system::generic::bus::BS_OK;
  using full_system::generic::bus::BS_ERROR;
  using full_system::generic::bus::CS_MISS;
  using full_system::generic::bus::CacheStatus;
#endif

#define XST_SC_MAX_BUFFER 	1024
#define syscall_num		gpr_c[16]
#define syscall_arg0		gpr_c[17]
#define syscall_arg1		gpr_c[18]
#define syscall_arg2		gpr_c[19]
#define syscall_arg3		gpr_c[20]
#define syscall_arg4		gpr_c[21]

#ifdef SOCLIB
CPU::CPU() 
{ Reset();
}
#else
CPU::CPU(const char *name, Object *parent) 
  : Object(name, parent)
  , Client<LoaderInterface<uint32_t>/*, CPURegistersInterface*/>(name, parent)
  , Client<SymbolTableLookupInterface<uint32_t> >(name, parent)
  , Client<MemoryInterface<uint32_t> >(name, parent)
  , Client<BusInterface<uint32_t> >(name, parent)
  , elfloader("elfloader", this)
  , symbol_table("symbol-table", this)
  , memory("memory", this)
  , bus_interface("bus_interface", this)
{ this->handler = NULL;
  Reset();
}

CPU::CPU(const char *name, xst_simoscall_t **handler, Object *parent)
  : Object(name, parent)
  , Client<LoaderInterface<uint32_t>/*, CPURegistersInterface*/>(name, parent)
  , Client<SymbolTableLookupInterface<uint32_t> >(name, parent)
  , Client<MemoryInterface<uint32_t> >(name, parent)
  , Client<BusInterface<uint32_t> >(name, parent)
  , elfloader("elfloader", this)
  , symbol_table("symbol-table", this)
  , memory("memory", this)
  , bus_interface("bus_interface", this)
{ this->handler = handler;
  Reset();
}
#endif


CPU::~CPU()
{
}

#ifndef SOCLIB

bool CPU::ReadMemory(uint32_t addr, void *buffer, uint32_t size)
{
	if(memory)
		return memory->ReadMemory(addr, buffer, size);
	return false;
}

bool CPU::WriteMemory(uint32_t addr, const void *buffer, uint32_t size)
{
	if(memory)
		return memory->WriteMemory(addr, buffer, size);
	return false;
}

#endif

#ifdef SOCLIB
void CPU::ReadMemoryBuffer(uint32_t addr, void *buffer, uint32_t size)
{ cerr << "ReadMemoryBuffer!" << endl;
  exit(1);
}
// write "size" bytes to "addr"
void CPU::WriteMemoryBuffer(uint32_t addr, const void *buffer, uint32_t size)
{ cerr << "WriteMemoryBuffer!" << endl;
  exit(1);
}
#else
// read "size" bytes from "addr" to "buffer"
void CPU::ReadMemoryBuffer(uint32_t addr, void *buffer, uint32_t size)
{ full_system::generic::bus::CacheStatus cs;
  bus_interface->BusRead(addr, buffer, size,full_system::generic::bus::BC_GLOBAL, cs);
}
// write "size" bytes to "addr"
void CPU::WriteMemoryBuffer(uint32_t addr, const void *buffer, uint32_t size)
{ bus_interface->BusWrite(addr, buffer, size, BC_GLOBAL);
}
#endif

#ifdef SOCLIB
void CPU::Fetch(void *buffer, uint32_t addr, uint32_t size)
{ cerr << "Fetch!" << endl;
  exit(1);
}
#else
void CPU::Fetch(void *buffer, uint32_t addr, uint32_t size)
{ CacheStatus cs;
  bus_interface->BusRead(addr, buffer, size, BC_NONE, cs);
}
#endif

void CPU::Reset()
{
	current_operation = NULL;
	bus_cycle = 0;
	instruction_counter = 0;

	// initialization of pc register
	cia = 0x00000000;
	nia = 0x00000000;

	// initialisation of bank registers
	memset(gpr_c, 0, 4 * NB_REGS);
	memset(gpr_n, 0, 4 * NB_REGS);
	memset(gpb_c, 0, NB_BITREGS);
	memset(gpb_n, 0, NB_BITREGS); 

	// initialization of Stack pointer register(r12)
	gpr_c[12] = 0x8700000;
	gpr_n[12] = 0x8700000;

	// initialization of syscall address
	syscall_addr = 0xFFFFFFFF;

	// initialistaion of Data Break Point
	DBreakAddr = -1;
}

#ifndef SOCLIB
void CPU::Setup()
{
	// initialization of pc register
	cia = symbol_table->FindSymbolByName("main")->GetAddress();
	nia = symbol_table->FindSymbolByName("main")->GetAddress();

	// initialization of link register(r63)
	gpr_c[63] = symbol_table->FindSymbolByName("_exit")->GetAddress();
	gpr_n[63] = symbol_table->FindSymbolByName("_exit")->GetAddress();

	// initialization of syscall address
        // __dotsyscall : the symbol in the binary file used to specify
        //                the simulation trap point. The simulation will use this symbol to detect
        //                when it should call simoscall_emulate.
	syscall_addr = symbol_table->FindSymbolByName("__dotsyscall")->GetAddress();

	// text segment boundary
	text_segment_start = elfloader->GetTextBase();
	text_segment_end   = elfloader->GetTextBase() + elfloader->GetTextSize();
}

void CPU::BundleStep()
{
	Operation * operation[4];
	uint32_t immregs[4]={0,0,0,0};
	int i = 0;

        nb_bundle ++;
	// decode the instructions of a bundle
	for(; i<4 ; )
	{
		operation[i] = Decode(nia);
		CodeType op = operation[i]->GetEncoding();

		if(IMM_SEL_MASK(op)==IMML_MASK) 
		{  immregs[i-1] = IMM(op);
		}
		else if(IMM_SEL_MASK(op)==IMMR_MASK)
		{  immregs[i+1] = IMM(op);
		}
		if(op & STOP_BIT)
		{  nia += 4;
	           break;
		}

		i++;
		nia += 4;
        }

        if(i==4)
        { cerr << __FUNCTION__ << ": Bundle without any stop bit at address " << hex << nia << dec << endl;
          exit(-1);
        }

        #if INST_DEBUG
        cout <<"cia = " << hex << cia << dec << endl;
        #endif

	// check the syscalls
	// all the syscalls of ST231 start from the same address
	// for the syscalls, the reg16 is used for the call number
	// and the reg17~reg22 used
        if (cia == syscall_addr )
        {
        	if(i>4) 
        	{
        	  cerr<< "Illegal dotsyscall function - must be a nop" <<endl;
        	  exit(1);
        	}
        	simoscall_emulation();
        	//xiss_uint32_t slice_id;
        	//xiss_uint32_t thread_id;
        	//xst_simoscall_emulate((void *)this, *handler,slice_id,thread_id);
        }
        
        else
	{
		// execute the instructions of a bundle
		for(int j=0; j<=i; j++)
		{      
		        imm = immregs[j];	
			current_operation = operation[j];
			operation[j]->execute(this);
                        #if INST_DEBUG
                        cout<<dec<<"B "<<nb_bundle<<dec<<" :";
			operation[j]->disasm(this,cout);
	                cout << endl;
                        #endif
		}
        }
        #if INST_DEBUG
	cout <<";;" << endl<<endl;
        #endif

	cia = nia;

	
	memcpy((void *)gpr_c, (void *)gpr_n, 4 * NB_REGS);
	memcpy(gpb_c, gpb_n, NB_BITREGS); 
}
#endif

void CPU::cpy_cpu_state(CPU *another_cpu)
{
    for(int i=0;i<NB_REGS; i++)
    { 
        gpr_c[i] = another_cpu->GetGPR_C(i);
        gpr_n[i] = another_cpu->GetGPR_N(i);
    }
    for(int i=0;i<NB_BITREGS; i++)
    { 
        gpb_c[i] = another_cpu->GetGPB_C(i);
        gpb_n[i] = another_cpu->GetGPB_N(i);
    }
    cia = another_cpu->GetCia();
    nia = another_cpu->GetNia();

    // TODO : the control registers 
}

void CPU::invalidate_icache(void)
{
#if 0 
#ifdef FAST_PRGINS
  invalidate_cycle_count++;
  /* Afer 4billion or so prgins, we need to trash the
   * the whole thing the slow way
   */
  if(invalidate_cycle_count==0) {
    memset(icache,0,MEMORY_SIZE);
  }
#else
    memset(icache,0,MEMORY_SIZE);
#endif

  /* Reset pointer to start of block */
  icache_next=0;
#endif
}

#ifndef SOCLIB
void CPU::sim_syscall(void)
{

	switch(syscall_num) 
	{
		case 6:
			sim_fstat();
			break;
		case 10:
			sim_open();
			break;
		case 11:
			sim_read();
			break;
		case 17:
			sim_write();
			break;
		case 1:
			sim_exit();
			break;
		case 104:
			//sim_printf();
			cout<< "sim printf" <<endl;
			break;
		default:
			printf("unimplemented sim_syscall - number %d\n",syscall_num);
			break;
	}
}

void CPU::sim_exit(void)
{
	int exit_num = syscall_arg0;
	cout << "End of simulation " << endl;
	exit(exit_num);
}

void CPU::sim_read(void)
{
	int fd = syscall_arg0;
	uint32_t sim_addr = syscall_arg1;
	int count = syscall_arg2;

	// template buffer to store the value from Host
	char *buf = (char *) calloc(1, count);

	// read from Host
	gpr_n[16] = read(fd,buf,count);
	// write to simulator
	WriteMemoryBuffer(sim_addr,buf,count);

	free(buf);
}

void CPU::sim_write(void)
{
	int fd = syscall_arg0;
	uint32_t sim_addr = syscall_arg1;
	int count = syscall_arg2;

	// template buffer to store the value from simulator
	char *buf = (char *) calloc(1, count);

	// read from simulator
	ReadMemoryBuffer(sim_addr,buf,count);

	// write to Host
	gpr_n[16] = write(fd,buf,count);

	free(buf);
}

void CPU::sim_open(void)
{
	char filename[XST_SC_MAX_BUFFER];

	uint32_t sim_addr = syscall_arg0;
	int sflag = syscall_arg1 & 0xFFFF;
	int mode = syscall_arg2;

	// read file name from simulator's memory
	ReadMemoryBuffer(sim_addr,filename,XST_SC_MAX_BUFFER);

	// open file
	gpr_n[16] = open(filename, sflag, mode);
} 

void CPU::sim_fstat(void)
{
	struct stat *sts;

	int fd = syscall_arg0;
	uint32_t buf_addr = syscall_arg1;

	sts = (struct stat *) malloc(sizeof(struct stat));
	gpr_c[16] = fstat(fd,sts);

	if(gpr_n[16]=0)
		WriteMemoryBuffer(buf_addr,sts,sizeof(struct stat));
	free(sts);
} 
#endif

} // end of namespace st231
