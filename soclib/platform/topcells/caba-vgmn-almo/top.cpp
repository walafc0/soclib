/*
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Dimitri Refauvelet <dimitri.refauvelet@lip6.fr>, 2009
 *
 * Maintainers: Dimitri Refauvelet
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "mips32.h"

#define CONFIG_GDB_SERVER
#if defined(CONFIG_GDB_SERVER)
# include "gdbserver.h"
#endif

#include "vci_vcache_wrapper2.h"
#include "vci_simple_ram.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"

#include "segmentation.h"

typedef soclib::caba::VciParams<4,8,32,1,1,1,14,4,4,1> vci_param;

typedef soclib::common::Mips32ElIss ProcessorIss;
typedef soclib::common::GdbServer<ProcessorIss> Processor;
typedef soclib::caba::VciVCacheWrapper2<vci_param, Processor > Cache;

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Mapping table

	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(0), true));
 
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(1), false));

	// Signals

	sc_clock	signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
	sc_signal<bool> signal_cpu_it[Processor::n_irq];
  
	soclib::caba::VciSignals<vci_param> signal_vci_cpu("signal_vci_m0");
	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram("signal_vci_vcimultiram");

	// Components
	Cache cache("cache", 0,maptab,IntTab(0),4,16,4,16,4,64,16,4,64,16,16);

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciSimpleRam<vci_param> vcimultiram("vcimultiram", IntTab(0), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(1), maptab, "vcitty0", NULL);

	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 1, 2, 2, 8);

	//	Net-List
	cache.p_clk(signal_clk);
	vcimultiram.p_clk(signal_clk);
 
	cache.p_resetn(signal_resetn);
	vcimultiram.p_resetn(signal_resetn);

	cache.p_vci(signal_vci_cpu);

	for(int i = 0 ; i < Processor::n_irq ; i++)
	  cache.p_irq[i](signal_cpu_it[i]); 

	vcimultiram.p_vci(signal_vci_vcimultiram);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_cpu_it[0]);

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

	vgmn.p_to_initiator[0](signal_vci_cpu);

	vgmn.p_to_target[0](signal_vci_vcimultiram);
	vgmn.p_to_target[1](signal_vci_tty);

	int ncycles;
	if ( argv[1] == NULL )
	  ncycles = 100000000;
	else
	  ncycles = std::atoi(argv[1]);

	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;

#ifdef SOCVIEW
	debug();
#else
	//sc_start();
	for (int i = 0; i < ncycles ; i+=1000) {
		sc_start(sc_core::sc_time(1000, SC_NS));
	}

#endif
	return EXIT_SUCCESS;
}

int sc_main (int argc, char *argv[])
{
	try {
		return _main(argc, argv);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	} catch (...) {
		std::cout << "Unknown exception occured" << std::endl;
		throw;
	}
	return 1;
}
