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
 * Maintainers: dimitri.refauvelet@etu.upmc.fr
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "mips.h"
#include "ississ2.h"
#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_timer.h"
#include "vci_multi_tty.h"
#include "vci_simhelper.h"
#include "vci_vgmn.h"
#include "stuck_at_fault_signal.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,6,32,1,1,1,8,1,1,1> vci_param;

	// Mapping table
	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(0), true));
	
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(1), false));

	maptab.add(Segment("simhelper"  , SIMHELPER_BASE  , SIMHELPER_SIZE  , IntTab(2), false));
        maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(3), false));


	// Signals

	sc_clock	signal_clk("signal_clk");
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
	soclib::caba::VciSignals<vci_param> signal_vci_vcitimer("signal_vci_vcitimer");

	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 


	sc_signal<bool> signal_mips3_it0_fault("signal_mips3_it0_fault");
	// Components
	soclib::caba::StuckAtFaultSignal<bool> fault("fault", false, true);

	soclib::caba::VciXcacheWrapper<vci_param, soclib::common::IssIss2<soclib::common::MipsElIss> > mips0("mips0", 0, maptab,IntTab(0), 4,1,8, 4,1,8);
	soclib::caba::VciXcacheWrapper<vci_param, soclib::common::IssIss2<soclib::common::MipsElIss> > mips1("mips1", 1, maptab,IntTab(1), 4,1,8, 4,1,8);
	soclib::caba::VciXcacheWrapper<vci_param, soclib::common::IssIss2<soclib::common::MipsElIss> > mips2("mips2", 2, maptab,IntTab(2), 4,1,8, 4,1,8);
	soclib::caba::VciXcacheWrapper<vci_param, soclib::common::IssIss2<soclib::common::MipsElIss> > mips3("mips3", 3, maptab,IntTab(3), 4,1,8, 4,1,8);

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(1), maptab, "vcitty0", NULL);
	soclib::caba::VciSimhelper<vci_param> vcisimhelper("vcisimhelper",	IntTab(2), maptab);
	soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(3), maptab, 4);

	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 4, 4, 2, 8);
	
	//	Net-List
 
	mips0.p_clk(signal_clk);  
	mips1.p_clk(signal_clk);  
	mips2.p_clk(signal_clk);  
	mips3.p_clk(signal_clk);  
	vcimultiram0.p_clk(signal_clk);
        vcitimer.p_clk(signal_clk);
  
	mips0.p_resetn(signal_resetn);  
	mips1.p_resetn(signal_resetn);  
	mips2.p_resetn(signal_resetn);  
	mips3.p_resetn(signal_resetn);  
	vcimultiram0.p_resetn(signal_resetn);
        vcitimer.p_resetn(signal_resetn);
  
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
  
	fault.p_in(signal_mips3_it0);
	fault.p_out(signal_mips3_it0_fault);
	mips3.p_irq[0](signal_mips3_it0_fault); 
	//mips3.p_irq[0](signal_mips3_it0); 
	mips3.p_irq[1](signal_mips3_it1); 
	mips3.p_irq[2](signal_mips3_it2); 
	mips3.p_irq[3](signal_mips3_it3); 
	mips3.p_irq[4](signal_mips3_it4); 
	mips3.p_irq[5](signal_mips3_it5); 
        
	mips0.p_vci(signal_vci_m0);
	mips1.p_vci(signal_vci_m1);
	mips2.p_vci(signal_vci_m2);
	mips3.p_vci(signal_vci_m3);

	vcitimer.p_vci(signal_vci_vcitimer);
        vcitimer.p_irq[0](signal_mips0_it0);
        vcitimer.p_irq[1](signal_mips1_it0);
        vcitimer.p_irq[2](signal_mips2_it0);
	vcitimer.p_irq[3](signal_mips3_it0);


	vcimultiram0.p_vci(signal_vci_vcimultiram0);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_tty_irq0); 

	vcisimhelper.p_clk(signal_clk);
	vcisimhelper.p_resetn(signal_resetn);
	vcisimhelper.p_vci(signal_vci_simhelper);

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

	vgmn.p_to_initiator[0](signal_vci_m0);
	vgmn.p_to_initiator[1](signal_vci_m1);
	vgmn.p_to_initiator[2](signal_vci_m2);
	vgmn.p_to_initiator[3](signal_vci_m3);

	vgmn.p_to_target[0](signal_vci_vcimultiram0);
	vgmn.p_to_target[1](signal_vci_tty);
	vgmn.p_to_target[2](signal_vci_simhelper);
	vgmn.p_to_target[3](signal_vci_vcitimer);


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
