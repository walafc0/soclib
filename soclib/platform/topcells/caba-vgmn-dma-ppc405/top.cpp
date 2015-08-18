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
#include "ppc405.h"
#include "vci_xcache_wrapper.h"
#include "vci_simhelper.h"
#include "vci_dma.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_framebuffer.h"
#include "vci_vgmn.h"

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

	soclib::common::MappingTable maptab(32, IntTab(4), IntTab(4), 0x00300000);

	maptab.add(Segment("ppc_boot", PPC_BOOT_BASE, PPC_BOOT_SIZE, IntTab(5), false));
	maptab.add(Segment("ppc_special", PPC_SPECIAL_BASE, PPC_SPECIAL_SIZE, IntTab(5), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(5), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
  
	maptab.add(Segment("loc0" , LOC0_BASE , LOC0_SIZE , IntTab(1), true));
  
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));
	maptab.add(Segment("simhelper", SIMHELPER_BASE, SIMHELPER_SIZE, IntTab(3), false));

	maptab.add(Segment("fb", FB_BASE, FB_SIZE, IntTab(4), false));

	maptab.add(Segment("dma", DMA_BASE, DMA_SIZE, IntTab(0), false));

	// Signals

	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
   
	sc_signal<bool> signal_ppc0_it0("signal_ppc0_it0"); 
	sc_signal<bool> signal_ppc0_it1("signal_ppc0_it1"); 

	soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");

	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
	soclib::caba::VciSignals<vci_param> signal_vci_simhelper("signal_vci_simhelper");
	soclib::caba::VciSignals<vci_param> signal_vci_vcifb("signal_vci_vcifb");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");

	soclib::caba::VciSignals<vci_param> signal_vci_dmai("signal_vci_dmai");
	soclib::caba::VciSignals<vci_param> signal_vci_dmat("signal_vci_dmat");

	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 

	// Components

	soclib::caba::VciXcacheWrapper<vci_param,soclib::common::Ppc405Iss> cache0("cache0", 0, maptab,IntTab(1),1,8,4,1,8,4);

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(5), maptab, loader);
	soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", NULL);
	soclib::caba::VciSimhelper<vci_param> simhelper("vcitsimhelper", IntTab(3), maptab);
	soclib::caba::VciFrameBuffer<vci_param> vcifb("vcifb", IntTab(4), maptab, FB_WIDTH, FB_HEIGHT); 

	soclib::caba::VciDma<vci_param> vcidma("vcidma", maptab, IntTab(0), IntTab(0), 1<<(vci_param::K-1)); 

	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 2, 6, 2, 8);

	//	Net-List
 
	cache0.p_clk(signal_clk);
	vcimultiram0.p_clk(signal_clk);
	vcimultiram1.p_clk(signal_clk);
	vcifb.p_clk(signal_clk);
	simhelper.p_clk(signal_clk);
	vcidma.p_clk(signal_clk);
  
	cache0.p_resetn(signal_resetn);
	vcimultiram0.p_resetn(signal_resetn);
	vcimultiram1.p_resetn(signal_resetn);
	vcifb.p_resetn(signal_resetn);
	simhelper.p_resetn(signal_resetn);
	vcidma.p_resetn(signal_resetn);
  
	cache0.p_irq[0](signal_ppc0_it0);
	cache0.p_irq[1](signal_ppc0_it1); 
        
	cache0.p_vci(signal_vci_m0);

	vcimultiram0.p_vci(signal_vci_vcimultiram0);

	simhelper.p_vci(signal_vci_simhelper);
  
	vcifb.p_vci(signal_vci_vcifb);
  
	vcimultiram1.p_vci(signal_vci_vcimultiram1);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_tty_irq0); 

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

	vcidma.p_vci_target(signal_vci_dmat);
	vcidma.p_vci_initiator(signal_vci_dmai);
	vcidma.p_irq(signal_ppc0_it0);

	vgmn.p_to_initiator[0](signal_vci_dmai);
	vgmn.p_to_initiator[1](signal_vci_m0);

	vgmn.p_to_target[0](signal_vci_dmat);
	vgmn.p_to_target[1](signal_vci_vcimultiram1);
	vgmn.p_to_target[2](signal_vci_tty);
	vgmn.p_to_target[3](signal_vci_simhelper);
	vgmn.p_to_target[4](signal_vci_vcifb);
	vgmn.p_to_target[5](signal_vci_vcimultiram0);

	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;

#ifdef SOCVIEW
	debug();
#else
	sc_start();
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
