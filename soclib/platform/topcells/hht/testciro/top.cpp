
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "hht_initiator_from_text.h"
#include "vci_ciro_config_initiator.h"
#include "vci_testelg_target.h"
#include "vci_hht_ciro_bridge.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	
	// Define VCI and HHT parameters
	typedef soclib::caba::VciParams<4,7,40,2,1,1,14,1,1,1> vci_param;
	typedef soclib::caba::HhtParam<64,32> hht_param;
	
	// Signals
	printf("Initializing signals\n");
	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
	// Signals connecting target to ciro 
	soclib::caba::VciSignals<vci_param> 		s_vci_io_targ("s_vci_io_targ");
	soclib::caba::VciSignals<vci_param> 		s_vci_ciro_config("s_vci_ciro_config");
	// Signals connecting ciro to initiator
	soclib::caba::HhtSignals<hht_param> 		s_hht("s_hht");
	
	// Components
	printf("Initializing components\n");
	soclib::caba::VciTestelgTarget<vci_param> 				vci_io_targ("vci_io_targ");
	soclib::caba::VciCiroConfigInitiator<vci_param> 	vci_ciro_config("vci_ciro_config");
	soclib::caba::VciHhtCiroBridge<vci_param, hht_param> 	ciro_bridge("ciro_bridge");
	soclib::caba::HhtInitiatorFromText<hht_param> 		hht_init("hht_init");
	
	//	Net-List
	printf("Initializing net-list\n");
	// All common signals
	vci_io_targ.p_clk(signal_clk);
	vci_io_targ.p_resetn(signal_resetn);
	vci_ciro_config.p_clk(signal_clk);
	vci_ciro_config.p_resetn(signal_resetn);
	ciro_bridge.p_clk(signal_clk);
	ciro_bridge.p_resetn(signal_resetn);
	hht_init.p_clk(signal_clk);
	hht_init.p_resetn(signal_resetn);
	
	// Linking components together
	vci_io_targ.p_vci(s_vci_io_targ);
	vci_ciro_config.p_vci(s_vci_ciro_config);
	
	ciro_bridge.p_vci_io(s_vci_io_targ);
	ciro_bridge.p_vci_config(s_vci_ciro_config);
	
	ciro_bridge.p_hht(s_hht);
	hht_init.p_hht(s_hht);
	
	// Simulation
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
	
	if (hht_init.start_send("/root/Desktop/Recherche/SystemC/hht/platforms/testciro/hht_cmd.txt",false))
		printf("Opening HHT commands file\n");
	else
		printf("Error: HHT commands file missing\n");
	printf("Cycle\t");
	printf("ctrlPCI         \t");
	printf("ctrlNPCI        \t");
	//printf("ROtoCI          \t");
	printf("vciCI           \t");
	printf("vciRO           \t");
	printf("ctrlRO          \t");
	printf("\n");
	
	for (int i = 0; i < ncycles ; i+=1)
	{
		sc_start(sc_core::sc_time(1, SC_NS));
		
		// Displays important fifos
		if (ciro_bridge.f_ctrlPCI.has_put || 
			ciro_bridge.f_ctrlNPCI.has_put || 
		//	ciro_bridge.f_ROtoCI.has_put || 
			ciro_bridge.f_vciCI.has_put || 
			ciro_bridge.f_vciRO.has_put || 
			ciro_bridge.f_ctrlRO.has_put ||
			false)
		{
			printf("%d\t",i);
			ciro_bridge.f_ctrlPCI.has_written.print	(ciro_bridge.f_ctrlPCI.has_put);
			ciro_bridge.f_ctrlNPCI.has_written.print(ciro_bridge.f_ctrlNPCI.has_put);
			//ciro_bridge.f_ROtoCI.has_written.print	(ciro_bridge.f_ROtoCI.has_put);
			ciro_bridge.f_vciCI.has_written.print	(ciro_bridge.f_vciCI.has_put);
			ciro_bridge.f_vciRO.has_written.print	(ciro_bridge.f_vciRO.has_put);
			ciro_bridge.f_ctrlRO.has_written.print	(ciro_bridge.f_ctrlRO.has_put);
			printf("\n");
		}	
		if (ciro_bridge.f_dataPCI.has_put  ||
			ciro_bridge.f_dataNPCI.has_put ||
			ciro_bridge.f_dataRO.has_put ||
			false)
		{
			printf("%d\t",i);
			if (ciro_bridge.f_dataPCI.has_put)	printf("data(%.8X)\t",(int)ciro_bridge.f_dataPCI.has_written); 	else printf("                \t");
			if (ciro_bridge.f_dataNPCI.has_put)	printf("data(%.8X)\t",(int)ciro_bridge.f_dataNPCI.has_written);	else printf("                \t");
			//ciro_bridge.f_ROtoCI.has_written.print(false);
			ciro_bridge.f_vciCI.has_written.print(false);
			ciro_bridge.f_vciRO.has_written.print(false);
			if (ciro_bridge.f_dataRO.has_put)	printf("data(%.8X)\t",(int)ciro_bridge.f_dataRO.has_written);		else printf("                \t");
			printf("\n");
		}
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
