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
 * Maintainers: nipo yang
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "iss2_simhelper.h"
#include "mips32.h"
#include "vci_vcache_wrapper.h"
//#include "vci_vcache_wrapper2.h"
#include "vci_timer.h"
#include "vci_simple_ram.h"
#include "vci_multi_tty.h"
#include "vci_locks.h"
#include "vci_vgmn.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,8,32,1,1,1,14,4,4,1> vci_param;

	// Mapping table

	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
 
	maptab.add(Segment("loc0" , LOC0_BASE , LOC0_SIZE , IntTab(1), true));
	maptab.add(Segment("loc1" , LOC1_BASE , LOC1_SIZE , IntTab(1), true));
	maptab.add(Segment("loc2" , LOC2_BASE , LOC2_SIZE , IntTab(1), true));
	maptab.add(Segment("loc3" , LOC3_BASE , LOC3_SIZE , IntTab(1), true));
 
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));
	maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(3), false));
	maptab.add(Segment("ptba",  PTD_ADDR  , TAB_SIZE  , IntTab(4), true));
	maptab.add(Segment("locks", LOCKS_BASE, LOCKS_SIZE, IntTab(5), false));
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
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
	soclib::caba::VciSignals<vci_param> signal_vci_vcitimer("signal_vci_vcitimer");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultipagt("signal_vci_vcimultipagt");
	soclib::caba::VciSignals<vci_param> signal_vci_vcilocks("signal_vci_vcilocks");

	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 
	sc_signal<bool> signal_tty_irq1("signal_tty_irq1"); 
	sc_signal<bool> signal_tty_irq2("signal_tty_irq2"); 
	sc_signal<bool> signal_tty_irq3("signal_tty_irq3"); 

	// Components

	soclib::caba::VciVCacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache0("cache0", 0,maptab,IntTab(0),4,16,4,16,4,64,16,4,64,16,16);
	soclib::caba::VciVCacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache1("cache1", 1,maptab,IntTab(1),4,16,4,16,4,64,16,4,64,16,16);
	soclib::caba::VciVCacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache2("cache2", 2,maptab,IntTab(2),4,16,4,16,4,64,16,4,64,16,16);
	soclib::caba::VciVCacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache3("cache3", 3,maptab,IntTab(3),4,16,4,16,4,64,16,4,64,16,16);

/*
	soclib::caba::VciVCacheWrapper2<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache0("cache0", 0,maptab,IntTab(0),4,16,4,16,4,64,16,4,64,16,16);
	soclib::caba::VciVCacheWrapper2<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache1("cache1", 1,maptab,IntTab(1),4,16,4,16,4,64,16,4,64,16,16);
	soclib::caba::VciVCacheWrapper2<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache2("cache2", 2,maptab,IntTab(2),4,16,4,16,4,64,16,4,64,16,16);
	soclib::caba::VciVCacheWrapper2<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache3("cache3", 3,maptab,IntTab(3),4,16,4,16,4,64,16,4,64,16,16);
*/

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciSimpleRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciSimpleRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", "vcitty1", "vcitty2", "vcitty3", NULL);
	soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(3), maptab, 4);
	soclib::caba::VciSimpleRam<vci_param> vcimultipagt("vcimultipagt", IntTab(4), maptab, loader);
	soclib::caba::VciLocks<vci_param> vcilocks("vcilocks", IntTab(5), maptab); 

	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 4, 6, 2, 8);

	//	Net-List
	cache0.p_clk(signal_clk);
	cache1.p_clk(signal_clk);
	cache2.p_clk(signal_clk);
	cache3.p_clk(signal_clk);
	vcimultiram0.p_clk(signal_clk);
	vcimultiram1.p_clk(signal_clk);
	vcimultipagt.p_clk(signal_clk);
	vcitimer.p_clk(signal_clk);
 	vcilocks.p_clk(signal_clk);
 
	cache0.p_resetn(signal_resetn);
	cache1.p_resetn(signal_resetn);
	cache2.p_resetn(signal_resetn);
	cache3.p_resetn(signal_resetn);
	vcimultiram0.p_resetn(signal_resetn);
	vcimultiram1.p_resetn(signal_resetn);
	vcimultipagt.p_resetn(signal_resetn);
	vcitimer.p_resetn(signal_resetn);
  	vcilocks.p_resetn(signal_resetn);

	cache0.p_vci(signal_vci_m0);
	cache0.p_irq[0](signal_mips0_it0); 
	cache0.p_irq[1](signal_mips0_it1); 
	cache0.p_irq[2](signal_mips0_it2); 
	cache0.p_irq[3](signal_mips0_it3); 
	cache0.p_irq[4](signal_mips0_it4); 
	cache0.p_irq[5](signal_mips0_it5); 

	cache1.p_vci(signal_vci_m1);
	cache1.p_irq[0](signal_mips0_it0); 
	cache1.p_irq[1](signal_mips0_it1); 
	cache1.p_irq[2](signal_mips0_it2); 
	cache1.p_irq[3](signal_mips0_it3); 
	cache1.p_irq[4](signal_mips0_it4); 
	cache1.p_irq[5](signal_mips0_it5); 
	
	cache2.p_vci(signal_vci_m2);
	cache2.p_irq[0](signal_mips0_it0); 
	cache2.p_irq[1](signal_mips0_it1); 
	cache2.p_irq[2](signal_mips0_it2); 
	cache2.p_irq[3](signal_mips0_it3); 
	cache2.p_irq[4](signal_mips0_it4); 
	cache2.p_irq[5](signal_mips0_it5); 
	
	cache3.p_vci(signal_vci_m3);
	cache3.p_irq[0](signal_mips0_it0); 
	cache3.p_irq[1](signal_mips0_it1); 
	cache3.p_irq[2](signal_mips0_it2); 
	cache3.p_irq[3](signal_mips0_it3); 
	cache3.p_irq[4](signal_mips0_it4); 
	cache3.p_irq[5](signal_mips0_it5); 

	vcimultiram0.p_vci(signal_vci_vcimultiram0);
	vcimultiram1.p_vci(signal_vci_vcimultiram1);
	vcimultipagt.p_vci(signal_vci_vcimultipagt);

	vcilocks.p_vci(signal_vci_vcilocks);

	vcitimer.p_vci(signal_vci_vcitimer);
	vcitimer.p_irq[0](signal_mips0_it0); 
	vcitimer.p_irq[1](signal_mips1_it0); 
	vcitimer.p_irq[2](signal_mips2_it0); 
	vcitimer.p_irq[3](signal_mips3_it0); 

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_tty_irq0); 
	vcitty.p_irq[1](signal_tty_irq1); 
	vcitty.p_irq[2](signal_tty_irq2); 
	vcitty.p_irq[3](signal_tty_irq3); 

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

	vgmn.p_to_initiator[0](signal_vci_m0);
	vgmn.p_to_initiator[1](signal_vci_m1);
	vgmn.p_to_initiator[2](signal_vci_m2);
	vgmn.p_to_initiator[3](signal_vci_m3);

	vgmn.p_to_target[0](signal_vci_vcimultiram0);
	vgmn.p_to_target[1](signal_vci_vcimultiram1);
	vgmn.p_to_target[2](signal_vci_tty);
	vgmn.p_to_target[3](signal_vci_vcitimer);
	vgmn.p_to_target[4](signal_vci_vcimultipagt);
	vgmn.p_to_target[5](signal_vci_vcilocks);

	if ( argv[1] == NULL )
	{
	    std::cerr << "Expect the number of cycles" << std::endl;
            return 1;
	}
	
	int ncycles = std::atoi(argv[1]);

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
		//cache0.print_stats();
		//cache1.print_stats();
		//cache2.print_stats();
		//cache3.print_stats();
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
