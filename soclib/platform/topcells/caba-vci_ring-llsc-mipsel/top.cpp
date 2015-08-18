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
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: abdelmalek.si-merabet@lip6.fr nipo
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "mips.h"
#include "vci_xcache_wrapper.h"
#include "ississ2.h"
#include "vci_simple_ram.h"
#include "vci_multi_tty.h"
#include "vci_simhelper.h"
#include "vci_simple_ring_fast.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1> vci_param;

	// Mapping table
	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(0), true));
  
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(1), false));

	maptab.add(Segment("simhelper"  , SIMHELPER_BASE  , SIMHELPER_SIZE  , IntTab(2), false));

	// Signals

	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
   
	sc_signal<bool> signal_mips0_it0("signal_mips0_it0"); 
	sc_signal<bool> signal_mips0_it1("signal_mips0_it1"); 
	sc_signal<bool> signal_mips0_it2("signal_mips0_it2"); 
	sc_signal<bool> signal_mips0_it3("signal_mips0_it3"); 
	sc_signal<bool> signal_mips0_it4("signal_mips0_it4"); 
	sc_signal<bool> signal_mips0_it5("signal_mips0_it5");
  
	sc_signal<bool> signal_mips1_it0("signal_mips1_it0"); 
	sc_signal<bool> signal_mips1_it1("signal_mips1_it1"); 
	sc_signal<bool> signal_mips1_it2("signal_mips1_it2"); 
	sc_signal<bool> signal_mips1_it3("signal_mips1_it3"); 
	sc_signal<bool> signal_mips1_it4("signal_mips1_it4"); 
	sc_signal<bool> signal_mips1_it5("signal_mips1_it5");
  
	sc_signal<bool> signal_mips2_it0("signal_mips2_it0"); 
	sc_signal<bool> signal_mips2_it1("signal_mips2_it1"); 
	sc_signal<bool> signal_mips2_it2("signal_mips2_it2"); 
	sc_signal<bool> signal_mips2_it3("signal_mips2_it3"); 
	sc_signal<bool> signal_mips2_it4("signal_mips2_it4"); 
	sc_signal<bool> signal_mips2_it5("signal_mips2_it5");

	sc_signal<bool> signal_mips3_it0("signal_mips3_it0"); 
	sc_signal<bool> signal_mips3_it1("signal_mips3_it1"); 
	sc_signal<bool> signal_mips3_it2("signal_mips3_it2"); 
	sc_signal<bool> signal_mips3_it3("signal_mips3_it3"); 
	sc_signal<bool> signal_mips3_it4("signal_mips3_it4"); 
	sc_signal<bool> signal_mips3_it5("signal_mips3_it5");

	soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");
	soclib::caba::VciSignals<vci_param> signal_vci_m1("signal_vci_m1");
	soclib::caba::VciSignals<vci_param> signal_vci_m2("signal_vci_m2");
	soclib::caba::VciSignals<vci_param> signal_vci_m3("signal_vci_m3");

	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_simhelper("signal_vci_simhelper");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");

	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 

	// Components

	typedef soclib::common::IssIss2<soclib::common::MipsElIss> iss_t;
	soclib::caba::VciXcacheWrapper<vci_param, iss_t> mips0("mips0", 0, maptab,IntTab(0),1,8,4,1,8,4);
	soclib::caba::VciXcacheWrapper<vci_param, iss_t> mips1("mips1", 1, maptab,IntTab(1),1,8,4,1,8,4);
	soclib::caba::VciXcacheWrapper<vci_param, iss_t> mips2("mips2", 2, maptab,IntTab(2),1,8,4,1,8,4);
	soclib::caba::VciXcacheWrapper<vci_param, iss_t> mips3("mips3", 3, maptab,IntTab(3),1,8,4,1,8,4);

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciSimpleRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(1), maptab, "vcitty0", NULL);
	soclib::caba::VciSimhelper<vci_param> vcisimhelper("vcisimhelper",	IntTab(2), maptab);
	
        // Ring
        soclib::caba::VciSimpleRingFast<vci_param, 40, 33> 
	ring("ring",maptab, IntTab(), 2, 4, 3);

	//	Net-List
	mips0.p_clk(signal_clk);  
	mips1.p_clk(signal_clk);  
	mips2.p_clk(signal_clk);  
	mips3.p_clk(signal_clk);  
	vcimultiram0.p_clk(signal_clk);
  
	mips0.p_resetn(signal_resetn);  
	mips1.p_resetn(signal_resetn);  
	mips2.p_resetn(signal_resetn);  
	mips3.p_resetn(signal_resetn);  
	vcimultiram0.p_resetn(signal_resetn);
  
	mips0.p_irq[0](signal_mips0_it0); 
	mips0.p_irq[1](signal_mips0_it1); 
	mips0.p_irq[2](signal_mips0_it2); 
	mips0.p_irq[3](signal_mips0_it3); 
	mips0.p_irq[4](signal_mips0_it4); 
	mips0.p_irq[5](signal_mips0_it5); 
  
	mips1.p_irq[0](signal_mips1_it0); 
	mips1.p_irq[1](signal_mips1_it1); 
	mips1.p_irq[2](signal_mips1_it2); 
	mips1.p_irq[3](signal_mips1_it3); 
	mips1.p_irq[4](signal_mips1_it4); 
	mips1.p_irq[5](signal_mips1_it5); 
  
	mips2.p_irq[0](signal_mips2_it0); 
	mips2.p_irq[1](signal_mips2_it1); 
	mips2.p_irq[2](signal_mips2_it2); 
	mips2.p_irq[3](signal_mips2_it3); 
	mips2.p_irq[4](signal_mips2_it4); 
	mips2.p_irq[5](signal_mips2_it5); 
  
	mips3.p_irq[0](signal_mips3_it0); 
	mips3.p_irq[1](signal_mips3_it1); 
	mips3.p_irq[2](signal_mips3_it2); 
	mips3.p_irq[3](signal_mips3_it3); 
	mips3.p_irq[4](signal_mips3_it4); 
	mips3.p_irq[5](signal_mips3_it5); 
        
	mips0.p_vci(signal_vci_m0);
	mips1.p_vci(signal_vci_m1);
	mips2.p_vci(signal_vci_m2);
	mips3.p_vci(signal_vci_m3);

	vcimultiram0.p_vci(signal_vci_vcimultiram0);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_tty_irq0); 

	vcisimhelper.p_clk(signal_clk);
	vcisimhelper.p_resetn(signal_resetn);
	vcisimhelper.p_vci(signal_vci_simhelper);

	// ring clock/reset signals
	ring.p_clk(signal_clk);
	ring.p_resetn(signal_resetn);
	
	// ring connection
        ring.p_to_initiator[0](signal_vci_m0);
        ring.p_to_initiator[1](signal_vci_m1);
	ring.p_to_initiator[2](signal_vci_m2);
        ring.p_to_initiator[3](signal_vci_m3);

        ring.p_to_target[0](signal_vci_vcimultiram0);
	ring.p_to_target[1](signal_vci_tty);
	ring.p_to_target[2](signal_vci_simhelper);

	//----------
	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;
	sc_start();
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
