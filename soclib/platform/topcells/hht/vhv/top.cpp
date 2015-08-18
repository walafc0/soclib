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

#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "vci_initiator_from_text.h"
#include "vci_cori_config_initiator.h"
#include "vci_ciro_config_initiator.h"
#include "vci_hht_cori_bridge.h"
#include "vci_hht_ciro_bridge.h"
#include "vci_testelg_target.h"
int _main(int argc, char *argv[])
{
	using namespace sc_core;
	
	// Define VCI & HHT parameters
	typedef soclib::caba::VciParams<4,7,40,2,1,1,14,1,1,1> vci_param;
	typedef soclib::caba::HhtParam<64,32> hht_param;
	
	// Signals
	printf("Initializing signals\n");
	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
	// Signals connecting initiator to cori and target to ciro
	soclib::caba::VciSignals<vci_param> 		s_vci_io_init("s_vci_io_init");
	soclib::caba::VciSignals<vci_param> 		s_vci_io_targ("s_vci_io_targ");
	// Signals connection the config initiators
	soclib::caba::VciSignals<vci_param> 		s_vci_cori_config("s_vci_cori_config");
	soclib::caba::VciSignals<vci_param> 		s_vci_ciro_config("s_vci_ciro_config");
	// Signals connecting cori to ciro
	soclib::caba::HhtSignals<hht_param> 		s_hht("s_hht");
	
	// Components
	printf("Initializing components\n");
	soclib::caba::VciInitiatorFromText<vci_param> 			vci_io_init("vci_io_init");
	soclib::caba::VciTestelgTarget<vci_param> 				vci_io_targ("vci_io_targ");
	soclib::caba::VciCoriConfigInitiator<vci_param> 		vci_cori_config("vci_cori_config");
	soclib::caba::VciCiroConfigInitiator<vci_param> 		vci_ciro_config("vci_ciro_config");
	soclib::caba::VciHhtCoriBridge<vci_param, hht_param> 	cori_bridge("cori_bridge");
	soclib::caba::VciHhtCiroBridge<vci_param, hht_param> 	ciro_bridge("ciro_bridge");
	
	//	Net-List
	printf("Initializing net-list\n");
	// All common signals
	vci_io_init.p_clk(signal_clk);
	vci_io_init.p_resetn(signal_resetn);
	vci_io_targ.p_clk(signal_clk);
	vci_io_targ.p_resetn(signal_resetn);
	vci_cori_config.p_clk(signal_clk);
	vci_cori_config.p_resetn(signal_resetn);
	vci_ciro_config.p_clk(signal_clk);
	vci_ciro_config.p_resetn(signal_resetn);
	cori_bridge.p_clk(signal_clk);
	cori_bridge.p_resetn(signal_resetn);
	ciro_bridge.p_clk(signal_clk);
	ciro_bridge.p_resetn(signal_resetn);
	
	// Linking components together
	vci_io_init.p_vci(s_vci_io_init);
	vci_io_targ.p_vci(s_vci_io_targ);
	vci_cori_config.p_vci(s_vci_cori_config);
	vci_ciro_config.p_vci(s_vci_ciro_config);
	
	cori_bridge.p_vci_io(s_vci_io_init);
	cori_bridge.p_vci_config(s_vci_cori_config);
	ciro_bridge.p_vci_io(s_vci_io_targ);
	ciro_bridge.p_vci_config(s_vci_ciro_config);
	
	cori_bridge.p_hht(s_hht);
	ciro_bridge.p_hht(s_hht);
	
	// Simulation
	int ncycles;
	if (argc == 2) {
		ncycles = std::atoi(argv[1]);
	} else {
		std::cerr
			<< std::endl << "The number of simulation cycles must be defined in the command line" << std::endl;
		exit(1);
	}

	FILE *file_vciCO=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/vhv/log/vciCO.txt","w");
	FILE *file_vciCI=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/vhv/log/vciCI.txt","w");
	if (file_vciCO==0)
		printf("Error creating vciCO log file\n");
	if (file_vciCI==0)
		printf("Error creating vciCI log file\n");
	FILE *file_vciRO=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/vhv/log/vciRO.txt","w");
	FILE *file_vciRI=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/vhv/log/vciRI.txt","w");
	if (file_vciRO==0)
		printf("Error creating vciRO log file\n");
	if (file_vciRI==0)
		printf("Error creating vciRI log file\n");

	printf("Starting simulation\n");
	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;
	
	if (vci_io_init.start_send("/root/Desktop/Recherche/SystemC/hht/platforms/vhv/vci_cmd.txt",false))
		printf("Opening VCI commands file\n");
	else
		printf("Error: VCI commands file missing\n");
	printf("Cycle\t");
	printf("vciCO           \t");
	printf("ctrlNPCO        \t");
	printf("ctrlPCO         \t");
	printf("vciCI           \t");
	printf("vciRO           \t");
	printf("ctrlRI          \t");
	printf("vciRI           \t");
	printf("\n");
	for (int i = 0; i < ncycles ; i+=1)
	{
	    sc_start(sc_core::sc_time(1, SC_NS));
		//std::cin.get(); // Effectue une pause
		// Affichage des fifos importantes
		if (cori_bridge.f_vciCO.has_put |//&& cori_bridge.f_vciCO.has_written.eop==1||
			cori_bridge.f_ctrlNPCO.has_put ||
			cori_bridge.f_ctrlPCO.has_put ||
			ciro_bridge.f_vciCI.has_put |//&& ciro_bridge.f_vciCI.has_written.eop==1 ||
			ciro_bridge.f_vciRO.has_put |//&& ciro_bridge.f_vciRO.has_written.reop==1 ||
			cori_bridge.f_ctrlRI.has_put ||
			cori_bridge.f_vciRI.has_put ||// && cori_bridge.f_vciRI.has_written.reop==1 || 
			false)
		{
			printf("%d\t",i);
			cori_bridge.f_vciCO.has_written.print	(cori_bridge.f_vciCO.has_put );// && cori_bridge.f_vciCO.has_written.eop==1);
			cori_bridge.f_ctrlNPCO.has_written.print(cori_bridge.f_ctrlNPCO.has_put);
			cori_bridge.f_ctrlPCO.has_written.print	(cori_bridge.f_ctrlPCO.has_put);
			ciro_bridge.f_vciCI.has_written.print	(ciro_bridge.f_vciCI.has_put );// && ciro_bridge.f_vciCI.has_written.eop==1);
			ciro_bridge.f_vciRO.has_written.print	(ciro_bridge.f_vciRO.has_put  );//&& ciro_bridge.f_vciRO.has_written.reop==1);
			cori_bridge.f_ctrlRI.has_written.print	(cori_bridge.f_ctrlRI.has_put);
			cori_bridge.f_vciRI.has_written.print	(cori_bridge.f_vciRI.has_put  );//&& cori_bridge.f_vciRI.has_written.reop==1);
			printf("\n");
		}	
		if (//cori_bridge.f_dataNPCO.has_put || 
			//cori_bridge.f_dataPCO.has_put || 
			//cori_bridge.f_dataRI.has_put ||
			false)
		{
			printf("%d\t",i);
			cori_bridge.f_vciCO.has_written.print(false);
			if (cori_bridge.f_dataNPCO.has_put)	printf("data(%.8X)     \t",(int)cori_bridge.f_dataNPCO.has_written);	else printf("                \t");
			if (cori_bridge.f_dataPCO.has_put)	printf("data(%.8X)     \t",(int)cori_bridge.f_dataPCO.has_written);	else printf("                \t");
			ciro_bridge.f_vciCI.has_written.print(false);
			ciro_bridge.f_vciRO.has_written.print(false);
			if (cori_bridge.f_dataRI.has_put)	printf("data(%.8X)     \t",(int)cori_bridge.f_dataRI.has_written);		else printf("                \t");
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
