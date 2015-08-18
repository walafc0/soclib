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

#include <generic/memory/MyMemEmulator.hh>
#include <plugins/loader/elf_loader.hh>
#include <utils/services/service.hh>                 /* USEFULL */
#include <plugins/loader/linux_loader.hh>
#include <utils/debug/symbol_table.hh>               /* USEFULL */

#include <iostream>
#include <sys/times.h>
#include <getopt.h>
#include <stdlib.h>
#include "st231_simoscall.hh"
#include "st231_isa.hh"
#include "debug_switch.h"

using namespace std;
using namespace st231;
using full_system::generic::memory::MyMemEmulator;
using full_system::plugins::loader::Elf32Loader;
using full_system::utils::debug::SymbolTable;
using full_system::utils::services::ServiceManager;

void help(char *prog_name)
{
	cerr << "Usage  : " << prog_name << " [<options>] <program> [program arguments]" << endl << endl;
	cerr << "       'program' is an ELF32 statically linked Linux binary" << endl;
	cerr << "       'program' can be a Linux kernel if option '-k' is used" << endl << endl;
	cerr << "Options:" << endl;
	cerr << "-d         : inline-debugger" << endl;
	cerr << "-i <count> : execute <count> instructions then exit" << endl << endl;
	cerr << "-h         : displays this help" << endl;
}

#if 0
bool debug_enabled;

void EnableDebug()
{
	debug_enabled = true;
}

void DisableDebug()
{
	debug_enabled = false;
}
#endif 


int main(int argc, char *argv[], char **envp)
{

	/* option analyse section : */
	// In this first vertion emulator, only three options are used:
	// - d : define inline-debugger
	// - i : define the maximum number of instructions to simulate
	// - h : print how to use the emulator 
	static struct option long_options[] = {
	{"inline-debugger", no_argument, 0, 'd'},
	{"help", no_argument, 0, 'h'},
	{"max:inst", required_argument, 0, 'i'},
	{0, 0, 0, 0}
	};

	int c;
	bool use_inline_debugger = false;
	uint64_t maxinst = 0;

	// the structs for simoscall
	struct xst_simoscall_callbacks * simoscallbacks = (struct xst_simoscall_callbacks *) malloc(sizeof(struct xst_simoscall_callbacks) );
	simoscallbacks->mem_read_fn  = &(simoscall_mem_read);
	simoscallbacks->mem_write_fn = &(simoscall_mem_write);
	simoscallbacks->reg_read_fn  = &(simoscall_reg_read);
	simoscallbacks->reg_write_fn = &(simoscall_reg_write);
	simoscallbacks->exit_fn      = &(simoscall_exit);
	xst_simoscall_target_def_t * targer_def = (xst_simoscall_target_def_t *) malloc(sizeof(xst_simoscall_target_def_t));
	targer_def->interface = XST_SIMOSCALL_ABI_ST231,XST_SIMOSCALL_ABI_ST231;
	targer_def->endianess = xst_simoscall_target_little_endian;
	xst_simoscall_t * handler=NULL;

	while((c = getopt_long (argc, argv, "dhi:", long_options, 0)) != -1)
	{
		switch(c)
		{
			case 'd':
				use_inline_debugger = true;
				break;
			case 'h':
				help(argv[0]);
				return 0;
			case 'i':
				maxinst = strtoull(optarg, 0, 0);
				break;
		}
	}

	if(optind >= argc)
	{
		help(argv[0]);
		return 0;
	}

	char *filename = argv[optind];
	int sim_argc = argc - optind;
	char **sim_argv = argv + optind;
	char **sim_envp = envp;

	if(!filename)
	{
		help(argv[0]);
		return 0;
	}


	/* component instance section                                                      */
	/*  -- In this first vertion emulator, only five components (cpu, memory, loader,  */
	/*     bus, symbole table) are used                                                */
	/*  -- In the next vertion, we will add the Debugger and Os ...                    */

	// creating the memory and configuring it 
	MyMemEmulator mymem("MyMemorySystem", 0);
	(mymem)["memory.org"] = 0x00000000UL;
	(mymem)["memory.bytesize"] = (uint32_t)-1;

	// creating the cpu and configuring it 
	st231::CPU *cpu = new st231::CPU("cpu",&handler);
	if(maxinst)
	{
		(*cpu)["max-inst"] = maxinst;
	}
	// creating the loader and configuring it 
	Elf32Loader *elf32_loader = new Elf32Loader("elf32-loader");
	(*elf32_loader)["filename"] = filename;

	// creating the symbole table 
	SymbolTable<uint32_t> *symbol_table = new SymbolTable<uint32_t>("symbol_table");

	// connecte the components
	//cpu->memory >> bus->memory;
	//(*bus)(memory);
	
	cpu->memory >> mymem.memory_export;
	cpu->bus_interface >> mymem.bus_export;
	elf32_loader->memory >> mymem.memory_export;
	elf32_loader->symbol_table >> symbol_table->build_exp;
	cpu->elfloader >> elf32_loader->exp;
	cpu->symbol_table >> symbol_table->lookup_exp;

	/* simulation section                                                      */
	if(ServiceManager::Setup())
	{
// -------------------------------------------------------------------------------------

// --- buggy version: stating at _start as it should -----------------------------------
//cpu->SetNia(elf32_loader->GetEntryPoint());
// -------------------------------------------------------------------------------------



		cerr << "Starting simulation at user privilege level " << endl;

		struct tms time_start, time_stop;
		double ratio;
		clock_t utime;
		double spent_time;
	
		times(&time_start);
                //EnableDebug();

		// initialize the simoscall
		xst_simoscall_init(simoscallbacks,targer_def,&handler);
                cpu->Setup();
		int i=0;
		do {
		   cpu->BundleStep();
		   i++;
		} while(1);

		cerr << "Simulation finished with " <<i <<"cycles" << endl;

		times(&time_stop);
	}
	else
	{
		cerr << "Can't start simulation because of previous errors" << endl;
	}

	if(handler)
		xst_simoscall_destroy(handler);
	if(elf32_loader) delete elf32_loader;
	if(symbol_table) delete symbol_table;

	if(cpu) delete cpu;

	return 0;
}
