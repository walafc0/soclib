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
 * Maintainers: nipo
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "ississ2.h"
#include "mips.h"
#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_simhelper.h"
#include "vci_pi_initiator_wrapper.h"
#include "vci_pi_target_wrapper.h"
#include "pibus_bcu.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

	// Mapping table

	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(0), true));
  	maptab.add(Segment("loc0" , LOC0_BASE , LOC0_SIZE , IntTab(0), true));

	maptab.add(Segment("tty"  , TTY_BASE , TTY_SIZE , IntTab(1), false));
	maptab.add(Segment("simhelper", SIMHELPER_BASE, SIMHELPER_SIZE, IntTab(2), false));

	// Signals
	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
   
	sc_signal<bool> signal_mips0_it0("signal_mips0_it0"); 
	sc_signal<bool> signal_mips0_it1("signal_mips0_it1"); 
	sc_signal<bool> signal_mips0_it2("signal_mips0_it2"); 
	sc_signal<bool> signal_mips0_it3("signal_mips0_it3"); 
	sc_signal<bool> signal_mips0_it4("signal_mips0_it4"); 
	sc_signal<bool> signal_mips0_it5("signal_mips0_it5");

	soclib::caba::Pibus pibus("pibus");

	sc_signal<bool> req("req");
	sc_signal<bool> gnt("gnt");
	sc_signal<bool> sel0("sel0");
	sc_signal<bool> sel1("sel1");
	sc_signal<bool> sel2("sel2");
  
	soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");

	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_simhelper("signal_vci_simhelper");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");

	// Components

	soclib::caba::VciXcacheWrapper<vci_param, soclib::common::IssIss2<soclib::common::MipsElIss> > 
            mips0("mips0", 0,maptab,0,1,8,4,1,8,4);
	soclib::common::Loader 
            loader("soft/bin.soft");
	soclib::caba::VciRam<vci_param> 
            vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> 
            vcitty("vcitty",	IntTab(1), maptab, "vcitty0", NULL);
	soclib::caba::VciSimhelper<vci_param> 
            simhelper("vcisimhelper", IntTab(2), maptab);
	soclib::caba::PibusBcu 
            bcu("bcu", maptab, 1, 3, 100);
	soclib::caba::VciPiInitiatorWrapper<vci_param> 
            cache_wrapper("cache_wrapper");
	soclib::caba::VciPiTargetWrapper<vci_param> 
            multiram_wrapper("multiram_wrapper");
	soclib::caba::VciPiTargetWrapper<vci_param> 
            tty_wrapper("tty_wrapper");
	soclib::caba::VciPiTargetWrapper<vci_param> 
            simhelper_wrapper("simhelper_wrapper");

	//	Net-List
 
	mips0.p_clk(signal_clk);  
	vcimultiram0.p_clk(signal_clk);
	vcitty.p_clk(signal_clk);
	simhelper.p_clk(signal_clk);
	cache_wrapper.p_clk(signal_clk);
	multiram_wrapper.p_clk(signal_clk);
	tty_wrapper.p_clk(signal_clk);
	simhelper_wrapper.p_clk(signal_clk);
	bcu.p_clk(signal_clk);

	mips0.p_resetn(signal_resetn);  
	vcimultiram0.p_resetn(signal_resetn);
	vcitty.p_resetn(signal_resetn);
	simhelper.p_resetn(signal_resetn);
	cache_wrapper.p_resetn(signal_resetn);
	multiram_wrapper.p_resetn(signal_resetn);
	tty_wrapper.p_resetn(signal_resetn);
	simhelper_wrapper.p_resetn(signal_resetn);
	bcu.p_resetn(signal_resetn);
  
	mips0.p_irq[0](signal_mips0_it0); 
	mips0.p_irq[1](signal_mips0_it1); 
	mips0.p_irq[2](signal_mips0_it2); 
	mips0.p_irq[3](signal_mips0_it3); 
	mips0.p_irq[4](signal_mips0_it4); 
	mips0.p_irq[5](signal_mips0_it5); 
	mips0.p_vci(signal_vci_m0);

	vcimultiram0.p_vci(signal_vci_vcimultiram0);

	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_mips0_it0); 

	simhelper.p_vci(signal_vci_simhelper);

	cache_wrapper.p_gnt(gnt);
	cache_wrapper.p_req(req);
	cache_wrapper.p_pi(pibus);
	cache_wrapper.p_vci(signal_vci_m0);

	multiram_wrapper.p_sel(sel0);
	multiram_wrapper.p_pi(pibus);
	multiram_wrapper.p_vci(signal_vci_vcimultiram0);

	tty_wrapper.p_sel(sel1);
	tty_wrapper.p_pi(pibus);
	tty_wrapper.p_vci(signal_vci_tty);

	simhelper_wrapper.p_sel(sel2);
	simhelper_wrapper.p_pi(pibus);
	simhelper_wrapper.p_vci(signal_vci_simhelper);

	bcu.p_req[0](req);
	bcu.p_gnt[0](gnt);
	bcu.p_sel[0](sel0);
	bcu.p_sel[1](sel1);
	bcu.p_sel[2](sel2);

	bcu.p_pi(pibus);

	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;

#ifndef SOCVIEW
	sc_start();
#else
	debug();
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
