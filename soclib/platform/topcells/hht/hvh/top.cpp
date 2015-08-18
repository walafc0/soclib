
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "hht_initiator_from_text.h"
#include "vci_cori_config_initiator.h"
#include "vci_ciro_config_initiator.h"
#include "vci_hht_cori_bridge.h"
#include "vci_hht_ciro_bridge.h"
#include "hht_testelg_target.h"
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
	// Signal connecting ciro to cori
	soclib::caba::VciSignals<vci_param> s_vci_io("s_vci_io");
	// Signals connecting the config initiators
	soclib::caba::VciSignals<vci_param> s_vci_cori_config("s_vci_cori_config");
	soclib::caba::VciSignals<vci_param> s_vci_ciro_config("s_vci_ciro_config");
	// Signals connecting ciro to initiator
	soclib::caba::HhtSignals<hht_param> 		s_ciro2init("s_ciro2init");
	// Signals connecting cori to target
	soclib::caba::HhtSignals<hht_param> 		s_cori2targ("s_cori2targ");
	
	
	// Components
	printf("Initializing components\n");
	soclib::caba::HhtInitiatorFromText<hht_param> 		hht_init("hht_init");
	soclib::caba::VciCoriConfigInitiator<vci_param> 	vci_cori_config("vci_cori_config");
	soclib::caba::VciCiroConfigInitiator<vci_param> 	vci_ciro_config("vci_ciro_config");
	soclib::caba::VciHhtCoriBridge<vci_param, hht_param> 	cori_bridge("cori_bridge");
	soclib::caba::VciHhtCiroBridge<vci_param, hht_param> 	ciro_bridge("ciro_bridge");
	soclib::caba::HhtTestelgTarget<hht_param> 				hht_targ("hht_targ");
	
	//	Net-List
	printf("Initializing net-list\n");
	// All common signals
	hht_init.p_clk(signal_clk);
	hht_init.p_resetn(signal_resetn);
	hht_targ.p_clk(signal_clk);
	hht_targ.p_resetn(signal_resetn);
	vci_cori_config.p_clk(signal_clk);
	vci_cori_config.p_resetn(signal_resetn);
	vci_ciro_config.p_clk(signal_clk);
	vci_ciro_config.p_resetn(signal_resetn);
	cori_bridge.p_clk(signal_clk);
	cori_bridge.p_resetn(signal_resetn);
	ciro_bridge.p_clk(signal_clk);
	ciro_bridge.p_resetn(signal_resetn);
	
	// Linking components together
	vci_cori_config.p_vci(s_vci_cori_config);
	vci_ciro_config.p_vci(s_vci_ciro_config);
	
	cori_bridge.p_vci_io(s_vci_io);
	cori_bridge.p_vci_config(s_vci_cori_config);
	ciro_bridge.p_vci_io(s_vci_io);
	ciro_bridge.p_vci_config(s_vci_ciro_config);
	
	cori_bridge.p_hht(s_cori2targ);
	hht_targ.p_hht(s_cori2targ);
	
	ciro_bridge.p_hht(s_ciro2init);
	hht_init.p_hht(s_ciro2init);
	
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
	
	if (hht_init.start_send("/root/Desktop/Recherche/SystemC/hht/platforms/hvh/hht_cmd.txt",false))
		printf("Opening HHT commands file\n");
	else
		printf("Error: HHT commands file missing\n");
		
	for (int i = 0; i < ncycles ; i+=1)
	{
		sc_start(sc_core::sc_time(1, SC_NS));
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
