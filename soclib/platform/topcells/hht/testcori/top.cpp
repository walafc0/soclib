
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "vci_initiator_from_text.h"
#include "vci_cori_config_initiator.h"
#include "hht_testelg_target.h"
#include "vci_hht_cori_bridge.h"
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
	// Signals connecting initiator to cori 
	soclib::caba::VciSignals<vci_param> 		s_vci_io_init("s_vci_io_init");
	soclib::caba::VciSignals<vci_param> 		s_vci_cori_config("s_vci_cori_config");
	// Signals connecting cori to target
	soclib::caba::HhtSignals<hht_param> 		s_hht("s_hht");
	
	// Components
	printf("Initializing components\n");
	soclib::caba::VciInitiatorFromText<vci_param> 		vci_io_init("vci_io_init");
	soclib::caba::VciCoriConfigInitiator<vci_param> 	vci_cori_config("vci_cori_config");
	soclib::caba::VciHhtCoriBridge<vci_param, hht_param> cori_bridge("cori_bridge");
	soclib::caba::HhtTestelgTarget<hht_param> 			hht_targ("hht_targ");
	
	//	Net-List
	printf("Initializing net-list\n");
	// All common signals
	vci_io_init.p_clk(signal_clk);
	vci_io_init.p_resetn(signal_resetn);
	vci_cori_config.p_clk(signal_clk);
	vci_cori_config.p_resetn(signal_resetn);
	cori_bridge.p_clk(signal_clk);
	cori_bridge.p_resetn(signal_resetn);
	hht_targ.p_clk(signal_clk);
	hht_targ.p_resetn(signal_resetn);
	
	// Linking components together
	vci_io_init.p_vci(s_vci_io_init);
	vci_cori_config.p_vci(s_vci_cori_config);
	
	cori_bridge.p_vci_io(s_vci_io_init);
	cori_bridge.p_vci_config(s_vci_cori_config);
	
	cori_bridge.p_hht(s_hht);
	hht_targ.p_hht(s_hht);
	
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
	
	if (vci_io_init.start_send("/root/Desktop/Recherche/SystemC/hht/platforms/testcori/vci_cmd.txt",false))
		printf("Opening VCI commands file\n");
	else
		printf("Error: VCI commands file missing\n");
	printf("Cycle\t");
	printf("vciCO           \t");
	printf("ctrlCO          \t");
	printf("ctrlNPCO        \t");
	printf("ctrlPCO         \t");
	printf("ctrlRI          \t");
	printf("vciRI           \t");
	printf("\n");
	
	for (int i = 0; i < ncycles ; i+=1)
	{
		sc_start(sc_core::sc_time(1, SC_NS));
		// Displays important fifos
		if (cori_bridge.f_vciCO.has_put || 
			cori_bridge.f_ctrlCO.has_put || 
			cori_bridge.f_ctrlNPCO.has_put ||
			cori_bridge.f_ctrlPCO.has_put || 
			cori_bridge.f_ctrlRI.has_put || 
			cori_bridge.f_vciRI.has_put || 
			false)
		{
			printf("%d\t",i);
			cori_bridge.f_vciCO.has_written.print	(cori_bridge.f_vciCO.has_put);
			cori_bridge.f_ctrlCO.has_written.print	(cori_bridge.f_ctrlCO.has_put);
			cori_bridge.f_ctrlNPCO.has_written.print(cori_bridge.f_ctrlNPCO.has_put);
			cori_bridge.f_ctrlPCO.has_written.print	(cori_bridge.f_ctrlPCO.has_put);
			cori_bridge.f_ctrlRI.has_written.print	(cori_bridge.f_ctrlRI.has_put);
			cori_bridge.f_vciRI.has_written.print	(cori_bridge.f_vciRI.has_put);
			printf("\n");
		}	
		if (cori_bridge.f_dataCO.has_put  ||  
			cori_bridge.f_dataNPCO.has_put ||
			cori_bridge.f_dataPCO.has_put ||  
			cori_bridge.f_dataRI.has_put  ||
			false)
		{
			printf("%d\t",i);
			cori_bridge.f_vciCO.has_written.print(false);
			if (cori_bridge.f_dataCO.has_put)	printf("data(%.8X)     \t",(int)cori_bridge.f_dataCO.has_written); 	else printf("                \t");
			if (cori_bridge.f_dataNPCO.has_put)	printf("data(%.8X)     \t",(int)cori_bridge.f_dataNPCO.has_written);	else printf("                \t");
			if (cori_bridge.f_dataPCO.has_put)	printf("data(%.8X)     \t",(int)cori_bridge.f_dataPCO.has_written);	else printf("                \t");
			if (cori_bridge.f_dataRI.has_put)	printf("data(%.8X)     \t",(int)cori_bridge.f_dataRI.has_written);		else printf("                \t");
			cori_bridge.f_vciRI.has_written.print(false);
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
