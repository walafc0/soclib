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
#include "mips32.h"
#include "vci_xcache_wrapper.h"
#include "vci_block_device.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_simhelper.h"
#include "vci_vgmn.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,9,32,1,1,1,8,1,1,1> vci_param;

	// Mapping table

	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(1), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(1), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(1), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(2), true));
  
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(3), false));
	maptab.add(Segment("bd", BD_BASE, BD_SIZE, IntTab(0), false));
	maptab.add(Segment("simhelper", SIMHELPER_BASE, SIMHELPER_SIZE, IntTab(4), false));

	// Signals

	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
   
	sc_signal<bool> signal_mips0_it0("signal_mips0_it0"); 
	sc_signal<bool> signal_mips0_it1("signal_mips0_it1"); 
	sc_signal<bool> signal_mips0_it2("signal_mips0_it2"); 
	sc_signal<bool> signal_mips0_it3("signal_mips0_it3"); 
	sc_signal<bool> signal_mips0_it4("signal_mips0_it4"); 
	sc_signal<bool> signal_mips0_it5("signal_mips0_it5");

	soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");

	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
	soclib::caba::VciSignals<vci_param> signal_vci_vcibd("signal_vci_vcibd");
	soclib::caba::VciSignals<vci_param> signal_vci_vcibd_i("signal_vci_vcibd_i");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");

	soclib::caba::VciSignals<vci_param> signal_vci_vcish("signal_vci_vcish");

	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 

	// Components

	soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Mips32ElIss> cache0("cache0", 0, maptab,IntTab(1),1, 8,4,1, 8,4);

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(1), maptab, loader);
	soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(2), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty", IntTab(3), maptab, "vcitty0", NULL);
	soclib::caba::VciBlockDevice<vci_param> vcibd("vcibd", maptab, IntTab(0), IntTab(0), "test.bin");

	soclib::caba::VciSimhelper<vci_param> vcisimhelper("vcisimhelper",	IntTab(4), maptab);
	
	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 2, 5, 2, 8);

	//	Net-List
 
	cache0.p_clk(signal_clk);
	vcimultiram0.p_clk(signal_clk);
	vcimultiram1.p_clk(signal_clk);
	vcibd.p_clk(signal_clk);
	vcisimhelper.p_clk(signal_clk);
  
	cache0.p_resetn(signal_resetn);
	vcimultiram0.p_resetn(signal_resetn);
	vcimultiram1.p_resetn(signal_resetn);
	vcibd.p_resetn(signal_resetn);
	vcisimhelper.p_resetn(signal_resetn);
  
	cache0.p_irq[0](signal_mips0_it0); 
	cache0.p_irq[1](signal_mips0_it1); 
	cache0.p_irq[2](signal_mips0_it2); 
	cache0.p_irq[3](signal_mips0_it3); 
	cache0.p_irq[4](signal_mips0_it4); 
	cache0.p_irq[5](signal_mips0_it5); 
	cache0.p_vci(signal_vci_m0);

	vcimultiram0.p_vci(signal_vci_vcimultiram0);

	vcisimhelper.p_vci(signal_vci_vcish);

	vcibd.p_vci_target(signal_vci_vcibd);
	vcibd.p_vci_initiator(signal_vci_vcibd_i);
	vcibd.p_irq(signal_mips0_it0); 
  
	vcimultiram1.p_vci(signal_vci_vcimultiram1);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_tty_irq0); 

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

	vgmn.p_to_initiator[0](signal_vci_vcibd_i);
	vgmn.p_to_initiator[1](signal_vci_m0);

	vgmn.p_to_target[0](signal_vci_vcibd);
	vgmn.p_to_target[1](signal_vci_vcimultiram0);
	vgmn.p_to_target[2](signal_vci_vcimultiram1);
	vgmn.p_to_target[3](signal_vci_tty);
	vgmn.p_to_target[4](signal_vci_vcish);


	sc_start(sc_time(0, SC_FS));
	signal_resetn = false;

	sc_start(sc_time(1, SC_FS));
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
