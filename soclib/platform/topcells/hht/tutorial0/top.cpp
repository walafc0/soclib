/* -*- c++ -*-
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
 *         Etienne Le Grand <etilegr@hotmail.com>, 2009
 */

#include <stdio.h>
#include <iostream>
#include <cstdlib>

////////////////////////////////////////////////////////////////////
// Part 1 : Include files                                         //
////////////////////////////////////////////////////////////////////

#include "mapping_table.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "vci_cori_config_initiator.h"
#include "vci_ciro_config_initiator.h"
#include "vci_hht_cori_bridge.h"
#include "vci_hht_ciro_bridge.h"

#include "mips32.h"
#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

        ////////////////////////////////////////////////////////////////////
        // Part 2 : Mapping table                                         //
        ////////////////////////////////////////////////////////////////////

	// Define VCI & HHT parameters
	typedef soclib::caba::VciParams<4,6,32,1,1,1,8,1,1,1> vci_param;
	typedef soclib::caba::HhtParam<64,32> hht_param;
	
	// Mapping table
	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(0), true));
  
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(1), false));

        ////////////////////////////////////////////////////////////////////
        // Part 3 : signals                                               //
        ////////////////////////////////////////////////////////////////////

	sc_clock	signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
   
	sc_signal<bool> signal_mips0_it0("signal_mips0_it0"); 
	sc_signal<bool> signal_mips0_it1("signal_mips0_it1"); 
	sc_signal<bool> signal_mips0_it2("signal_mips0_it2"); 
	sc_signal<bool> signal_mips0_it3("signal_mips0_it3"); 
	sc_signal<bool> signal_mips0_it4("signal_mips0_it4"); 
	sc_signal<bool> signal_mips0_it5("signal_mips0_it5");

	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 

	soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");
		
	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");

	soclib::caba::VciSignals<vci_param> s_vci_io_init("s_vci_io_init");
	soclib::caba::VciSignals<vci_param> s_vci_io_targ("s_vci_io_targ");
	soclib::caba::VciSignals<vci_param> s_vci_cori_config("s_vci_cori_config");
	soclib::caba::VciSignals<vci_param> s_vci_ciro_config("s_vci_ciro_config");
	
	soclib::caba::HhtSignals<hht_param> s_hht("s_hht");
        ////////////////////////////////////////////////////////////////////
        // Part 4 : instances                                             //
        ////////////////////////////////////////////////////////////////////

	soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Mips32ElIss > cache0("cache0", 0, maptab,IntTab(0), 4,1,8, 4,1,8); 

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(1), maptab, "vcitty0", NULL);
	
	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 1, 2, 2, 8);

	soclib::caba::VciCoriConfigInitiator<vci_param> vci_cori_config("vci_cori_config");
	soclib::caba::VciCiroConfigInitiator<vci_param> vci_ciro_config("vci_ciro_config");
	soclib::caba::VciHhtCoriBridge<vci_param, hht_param> cori_bridge("cori_bridge");
	soclib::caba::VciHhtCiroBridge<vci_param, hht_param> ciro_bridge("ciro_bridge");
	
        ////////////////////////////////////////////////////////////////////
        // Part 5 : netlist                                               //
        ////////////////////////////////////////////////////////////////////

	cache0.p_clk(signal_clk);
	cache0.p_resetn(signal_resetn);
	cache0.p_irq[0](signal_mips0_it0); 
	cache0.p_irq[1](signal_mips0_it1); 
	cache0.p_irq[2](signal_mips0_it2); 
	cache0.p_irq[3](signal_mips0_it3); 
	cache0.p_irq[4](signal_mips0_it4); 
	cache0.p_irq[5](signal_mips0_it5); 
	cache0.p_vci(signal_vci_m0);
	
	vci_cori_config.p_clk(signal_clk);
	vci_cori_config.p_resetn(signal_resetn);
	vci_ciro_config.p_clk(signal_clk);
	vci_ciro_config.p_resetn(signal_resetn);
	cori_bridge.p_clk(signal_clk);
	cori_bridge.p_resetn(signal_resetn);
	ciro_bridge.p_clk(signal_clk);
	ciro_bridge.p_resetn(signal_resetn);
	

	vci_cori_config.p_vci(s_vci_cori_config);
	vci_ciro_config.p_vci(s_vci_ciro_config);
	cori_bridge.p_vci_io(s_vci_io_init);
	cori_bridge.p_vci_config(s_vci_cori_config);
	ciro_bridge.p_vci_io(s_vci_io_targ);
	ciro_bridge.p_vci_config(s_vci_ciro_config);
	
	cori_bridge.p_hht(s_hht);
	ciro_bridge.p_hht(s_hht);
	
	vcimultiram0.p_clk(signal_clk);
	vcimultiram0.p_resetn(signal_resetn);
	vcimultiram0.p_vci(s_vci_io_targ);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_irq[0](signal_tty_irq0); 
	vcitty.p_vci(signal_vci_tty);

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);
	vgmn.p_to_initiator[0](signal_vci_m0);
	vgmn.p_to_target[0](s_vci_io_init);
	vgmn.p_to_target[1](signal_vci_tty);

	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;

   	FILE *file_vciCO=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/tutorial0/log/vciCO.txt","w");
	FILE *file_vciCI=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/tutorial0/log/vciCI.txt","w");
	if (file_vciCO==0)
		printf("Error creating vciCO log file\n");
	if (file_vciCI==0)
		printf("Error creating vciCI log file\n");
	FILE *file_vciRO=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/tutorial0/log/vciRO.txt","w");
	FILE *file_vciRI=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/tutorial0/log/vciRI.txt","w");
	if (file_vciRO==0)
		printf("Error creating vciRO log file\n");
	if (file_vciRI==0)
		printf("Error creating vciRI log file\n");
		////////////////////////////////////////////////////////////////////
        // Part 6 : simulate                                              //
        ////////////////////////////////////////////////////////////////////

	int ncycles;
	if (argc == 2) {
		ncycles = std::atoi(argv[1]);
	} else {
		std::cerr
			<< std::endl << "The number of simulation cycles must be defined in the command line" << std::endl;
		exit(1);
	}

	printf("Starting simulation\n");
	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;
	
	printf("Cycle\tvciCO           \tctrlNPCO        \tctrlPCO         \tvciCI           \tvciRO           \tctrlRI          \tvciRI           \t\n");
	for (int i = 0; i < ncycles ; i+=1)
	{
		sc_start(sc_core::sc_time(1, SC_NS));
		// Displays important fifos
		if (cori_bridge.f_vciCO.has_put && cori_bridge.f_vciCO.has_written.eop==1||
			cori_bridge.f_ctrlNPCO.has_put ||
			cori_bridge.f_ctrlPCO.has_put ||
			ciro_bridge.f_vciCI.has_put && ciro_bridge.f_vciCI.has_written.eop==1 ||
			ciro_bridge.f_vciRO.has_put && ciro_bridge.f_vciRO.has_written.reop==1 ||
			cori_bridge.f_ctrlRI.has_put ||
			cori_bridge.f_vciRI.has_put && cori_bridge.f_vciRI.has_written.reop==1 || 
			false)
		{
			printf("%d\t",i);
			cori_bridge.f_vciCO.has_written.print	(cori_bridge.f_vciCO.has_put && cori_bridge.f_vciCO.has_written.eop==1);
			cori_bridge.f_ctrlNPCO.has_written.print(cori_bridge.f_ctrlNPCO.has_put);
			cori_bridge.f_ctrlPCO.has_written.print	(cori_bridge.f_ctrlPCO.has_put);
			ciro_bridge.f_vciCI.has_written.print	(ciro_bridge.f_vciCI.has_put && ciro_bridge.f_vciCI.has_written.eop==1);
			ciro_bridge.f_vciRO.has_written.print	(ciro_bridge.f_vciRO.has_put && ciro_bridge.f_vciRO.has_written.reop==1);
			cori_bridge.f_ctrlRI.has_written.print	(cori_bridge.f_ctrlRI.has_put);
			cori_bridge.f_vciRI.has_written.print	(cori_bridge.f_vciRI.has_put && cori_bridge.f_vciRI.has_written.reop==1);
			printf("\n");
		}	
		if (//cori_bridge.f_dataNPCO.has_put || 
			//cori_bridge.f_dataRI.has_put ||
			false)
		{
			printf("%d\t",i);
			cori_bridge.f_vciCO.has_written.print(false);
			if (cori_bridge.f_dataNPCO.has_put)	printf("data(%.8X)\t",(int)cori_bridge.f_dataNPCO.has_written);	else printf("                \t");
			ciro_bridge.f_vciCI.has_written.print(false);
			ciro_bridge.f_vciRO.has_written.print(false);
			if (cori_bridge.f_dataRI.has_put)	printf("data(%.8X)\t",(int)cori_bridge.f_dataRI.has_written);		else printf("                \t");
			cori_bridge.f_vciRI.has_written.print(false);
			printf("\n");
		}	
		if (cori_bridge.f_vciCO.has_put) cori_bridge.f_vciCO.has_written.fprint (file_vciCO);
		if (ciro_bridge.f_vciCI.has_put) ciro_bridge.f_vciCI.has_written.fprint (file_vciCI);
		if (ciro_bridge.f_vciRO.has_put) ciro_bridge.f_vciRO.has_written.fprint (file_vciRO);
		if (cori_bridge.f_vciRI.has_put) cori_bridge.f_vciRI.has_written.fprint (file_vciRI);
	}	
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
