

#define NB_CYCLES 16 
#define NB_SAMPLES 64

#include <iostream>
#include <cstdlib>

#include "fifo_ports.h"
#include "mapping_table.h"
#include "mips.h"
#include "vci_vgmn.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_xcache_wrapper.h"
#include "ississ2.h"
#include "vci_locks.h"
#include "mwmr_controller.h"
#include "vci_mwmr_controller.h"

#include "synchronization.h"

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

	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
	
	maptab.add(Segment("tty", TTY_BASE, TTY_SIZE  , IntTab(2), false));

	maptab.add(Segment("mwmr0", MWMR_BASE , MWMR_SIZE , IntTab(3), false));
	maptab.add(Segment("mwmr_ram", MWMRd_BASE , MWMRd_SIZE , IntTab(4), false));	
	
	  
	maptab.add(Segment("locks" , LOCKS_BASE , LOCKS_SIZE , IntTab(5), false));
	/*maptab.add(Segment("loc1" , LOC1_BASE , LOC1_SIZE , IntTab(5), true));
	maptab.add(Segment("loc2" , LOC2_BASE , LOC2_SIZE , IntTab(6), true));
	maptab.add(Segment("loc3" , LOC3_BASE , LOC3_SIZE , IntTab(7), true));*/
	
  // Signals

	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
   
	sc_signal<bool> signal_mips0_it0("signal_mips_it0"); 
	sc_signal<bool> signal_mips0_it1("signal_mips_it1"); 
	sc_signal<bool> signal_mips0_it2("signal_mips_it2"); 
	sc_signal<bool> signal_mips0_it3("signal_mips_it3"); 
	sc_signal<bool> signal_mips0_it4("signal_mips_it4"); 
	sc_signal<bool> signal_mips0_it5("signal_mips_it5");
        
	soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");
	soclib::caba::VciSignals<vci_param> signal_mwmr0_target("signal_mwmr0_target")    ;
	soclib::caba::VciSignals<vci_param> signal_mwmr0_initiator("signal_mwmr0_initiator")    ;
	soclib::caba::FifoSignals<uint32_t> signal_fifo_to_ctrl("signal_fifo_to_ctrl");
        soclib::caba::FifoSignals<uint32_t> signal_fifo_from_ctrl("signal_fifo_from_ctrl");
	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram2("signal_vci_vcimultiram2");
	
	soclib::caba::VciSignals<vci_param> signal_vci_locks("signal_vci_locks");
	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 

	// Components

	typedef soclib::common::IssIss2<soclib::common::MipsElIss> iss_t;
	soclib::caba::VciXcacheWrapper<vci_param, iss_t > mips0("mips0", 0,maptab,IntTab(0),1,8,4,1,8,4);

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
	soclib::caba::VciRam<vci_param> vcimultiram2("vcimultiram2", IntTab(4), maptab, loader);

	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty", IntTab(2), maptab, "vcitty0", NULL);
	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab,2,6,2,8);
	soclib::caba::VciLocks<vci_param> vcilocks("vcilocks", IntTab(5), maptab);

	/////////////////////////////////////////////////////////////////////////////
	// MWMR AND COPROCESSOR 0
	/////////////////////////////////////////////////////////////////////////////

	//soclib::caba::VciMwmrController<vci_param> mwmr0("mwmr0", maptab, IntTab(3), IntTab(3), plaps, fifo_to_coproc_depth, fifo_from_coproc_depth, n_to_coproc, n_from_coproc, n_config, n_status, use_llsc);
	soclib::caba::VciMwmrController<vci_param> mwmr0("mwmr0", maptab, IntTab(1), IntTab(3), 64, 8, 8, 1, 1, 0, 0, false);	
	soclib::caba::Synchronization<vci_param, NB_SAMPLES> synchronization("synchronization", NB_CYCLES);
	
	//	Net-List
	mips0.p_clk(signal_clk);  
	vcimultiram0.p_clk(signal_clk);
	vcimultiram1.p_clk(signal_clk);
	vcimultiram2.p_clk(signal_clk);

	
	mips0.p_resetn(signal_resetn);  
	vcimultiram0.p_resetn(signal_resetn);
	vcimultiram1.p_resetn(signal_resetn);
	vcimultiram2.p_resetn(signal_resetn);
	
	mips0.p_irq[0](signal_mips0_it0); 
	mips0.p_irq[1](signal_mips0_it1); 
	mips0.p_irq[2](signal_mips0_it2); 
	mips0.p_irq[3](signal_mips0_it3); 
	mips0.p_irq[4](signal_mips0_it4); 
	mips0.p_irq[5](signal_mips0_it5);
	
	mips0.p_vci(signal_vci_m0);
	
	vcimultiram0.p_vci(signal_vci_vcimultiram0);
	vcimultiram1.p_vci(signal_vci_vcimultiram1);
	vcimultiram2.p_vci(signal_vci_vcimultiram2);
	
	vcitty.p_clk(signal_clk);
        vcitty.p_resetn(signal_resetn);
        vcitty.p_vci(signal_vci_tty);
        vcitty.p_irq[0](signal_tty_irq0);

	mwmr0.p_clk(signal_clk);
	mwmr0.p_resetn(signal_resetn);
	mwmr0.p_vci_initiator(signal_mwmr0_initiator);
	mwmr0.p_vci_target(signal_mwmr0_target);
	mwmr0.p_from_coproc[0](signal_fifo_to_ctrl);
	mwmr0.p_to_coproc[0](signal_fifo_from_ctrl);
	
	synchronization.p_clk(signal_clk);
	synchronization.p_resetn(signal_resetn);
	synchronization.p_to_ctrl(signal_fifo_to_ctrl);
	synchronization.p_from_ctrl(signal_fifo_from_ctrl);

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);
        vgmn.p_to_initiator[1](signal_mwmr0_initiator);
	vgmn.p_to_initiator[0](signal_vci_m0);
	
	vgmn.p_to_target[/*5*/3](signal_mwmr0_target);
	vgmn.p_to_target[0](signal_vci_vcimultiram0);
	vgmn.p_to_target[1](signal_vci_vcimultiram1);
	vgmn.p_to_target[/*2*/4](signal_vci_vcimultiram2);
	vgmn.p_to_target[/*3*/2](signal_vci_tty);
	vgmn.p_to_target[/*4*/5](signal_vci_locks);
	
	vcilocks.p_clk(signal_clk);
        vcilocks.p_resetn(signal_resetn);
	vcilocks.p_vci(signal_vci_locks);
	
	// Trace
	sc_trace_file *my_trace_file;
	my_trace_file = sc_create_vcd_trace_file ("system_trace");
	
	sc_trace(my_trace_file, signal_clk, "CLK");
        sc_trace(my_trace_file, signal_resetn, "RESETN");

	signal_vci_m0.trace(my_trace_file, "vci_cache");
	signal_vci_vcimultiram2.trace(my_trace_file, "vci_mwmr_data");
	signal_vci_locks.trace(my_trace_file, "vci_locks");	

	/////////////////////////////////////////////////////////////////////////////
	// START
	/////////////////////////////////////////////////////////////////////////////
	int ncycles;
#ifndef SOCVIEW
	if (argc == 2) {
		ncycles = std::atoi(argv[1]);
	} else {
		std::cerr
			<< std::endl
			<< "The number of simulation cycles must "
			   "be defined in the command line"
			<< std::endl;
		exit(1);
	}

	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;

	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;

	for (int i = 0; i < ncycles ; i+=10000) {
		sc_start(sc_core::sc_time(100000, SC_NS));
		std::cout << "Time elapsed: "<<i<<" cycles." << std::endl;
	}
	// TRACE
	sc_close_vcd_trace_file (my_trace_file);

	return EXIT_SUCCESS;
	sc_start();
#else
	ncycles = 1;
	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;

	debug();
	return EXIT_SUCCESS;
#endif
	
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

