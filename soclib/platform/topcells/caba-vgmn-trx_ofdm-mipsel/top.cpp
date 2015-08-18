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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors : Ivan MIRO-PANADES
 * 
 * History :
 *
 * Comment :
 *
 */

//#define SOCVIEW
//#define _TRACEFILE
#define _CLK_PERIOD 1000   // Expressed in ps

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "mips.h"
#include "vci_xcache_wrapper.h"
#include "ississ2.h"
#include "vci_ram.h"
#include "vci_locks.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"

//MWMR
#include "mwmr_controller.h"
#include "vci_mwmr_controller.h"

//TRX_OFDM
#include "trx_ofdm.h"
#include "fifo_signals.h"

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

	maptab.add(Segment("reset"    , RESET_BASE, RESET_SIZE, IntTab(0), true));
	maptab.add(Segment("excep"    , EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text"     , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
	maptab.add(Segment("data"     , DATA_BASE , DATA_SIZE , IntTab(1), true));
  
	maptab.add(Segment("tty"      , TTY_BASE  , TTY_SIZE  , IntTab(2), false));
	maptab.add(Segment("mwmr_ctrl", MWMR_BASE , MWMR_SIZE , IntTab(3), false)); //MWMR controller
	maptab.add(Segment("mwmr_ram" , MWMRd_BASE, MWMRd_SIZE, IntTab(4), false)); //RAM memory associated for the MWMR FIFOs 
	maptab.add(Segment("locks"    , LOCKS_BASE, LOCKS_SIZE, IntTab(5), false)); //Locks memory


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
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram2("signal_vci_vcimultiram2");
	soclib::caba::VciSignals<vci_param> signal_vci_mwmr_initiator("signal_vci_mwmr_initiator");
	soclib::caba::VciSignals<vci_param> signal_vci_mwmr_target("signal_vci_mwmr_target");
	soclib::caba::VciSignals<vci_param> signal_vci_locks("signal_vci_locks");

	soclib::caba::FifoSignals<uint32_t>  signal_mwmr_from_coproc0("signal_mwmr_from_coproc0");
	soclib::caba::FifoSignals<uint32_t>  signal_mwmr_from_coproc1("signal_mwmr_from_coproc1");
	soclib::caba::FifoSignals<uint32_t>  signal_mwmr_to_coproc0("signal_mwmr_to_coproc0");
	soclib::caba::FifoSignals<uint32_t>  signal_mwmr_to_coproc1("signal_mwmr_to_coproc1");
	soclib::caba::FifoSignals<uint32_t>  signal_mwmr_config("signal_mwmr_config");
	sc_signal<uint32_t> signal_p_config[6];
	sc_signal<uint32_t> signal_p_status[6];

	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 

	// Components

	typedef soclib::common::IssIss2<soclib::common::MipsElIss> iss_t;
	soclib::caba::VciXcacheWrapper<vci_param, iss_t > mips0("mips0", 0,maptab,IntTab(0),1,8,4,1,8,4);

	soclib::common::Loader loader("soft/bin.soft");
	soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
	soclib::caba::VciRam<vci_param> vcimultiram2("vcimultiram2", IntTab(4), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", NULL);
	soclib::caba::VciLocks<vci_param> vcilocks("vcilocks", IntTab(5), maptab);


	////////////////////////////////////////////////
	// MWMR CONTROLLER
	////////////////////////////////////////////////
    //VciMwmrController(name,MappingTable, IntTab &srcid, IntTab &tgtid, plaps, fifo_to_coproc_depth, fifo_from_coproc_depth, n_to_coproc, n_from_coproc, n_config, n_status, use_llsc );

	soclib::caba::VciMwmrController<vci_param>  mwmr_ctrl("mwmr_ctrl", maptab, IntTab(1), IntTab(3), 64, 8, 8, 3, 2, 6, 6, false);


	////////////////////////////////////////////////
	// TRX_OFDM
	////////////////////////////////////////////////
    soclib::caba::trx_ofdm trx_ofdm("trx_ofdm",_CLK_PERIOD);

    // vgmn(name, MappingTable, nb_initiat, nb_target, min_latency, fifo_depth)
	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 2, 6, 2, 8);

	//	Net-List
	mips0.p_clk(signal_clk);  
	vcimultiram0.p_clk(signal_clk);
	vcimultiram1.p_clk(signal_clk);
	vcimultiram2.p_clk(signal_clk);
  
	mips0.p_resetn(signal_resetn);  
	vcimultiram0.p_resetn(signal_resetn);
	vcimultiram1.p_resetn(signal_resetn);
	vcimultiram2.p_resetn(signal_resetn);

  /* Processor management */
	mips0.p_irq[0](signal_mips0_it0); 
	mips0.p_irq[1](signal_mips0_it1); 
	mips0.p_irq[2](signal_mips0_it2); 
	mips0.p_irq[3](signal_mips0_it3); 
	mips0.p_irq[4](signal_mips0_it4); 
	mips0.p_irq[5](signal_mips0_it5); 
        
  /* Processor cache */
	mips0.p_vci(signal_vci_m0);

	vcimultiram0.p_vci(signal_vci_vcimultiram0);
	vcimultiram1.p_vci(signal_vci_vcimultiram1);
	vcimultiram2.p_vci(signal_vci_vcimultiram2);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_tty_irq0); 

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

	vgmn.p_to_initiator[0](signal_vci_m0);
	vgmn.p_to_initiator[1](signal_vci_mwmr_initiator);

	vgmn.p_to_target[0](signal_vci_vcimultiram0);
	vgmn.p_to_target[1](signal_vci_vcimultiram1);
	vgmn.p_to_target[2](signal_vci_tty);
	vgmn.p_to_target[3](signal_vci_mwmr_target);
	vgmn.p_to_target[4](signal_vci_vcimultiram2);
	vgmn.p_to_target[5](signal_vci_locks);


    //MWMR
	mwmr_ctrl.p_clk(signal_clk);
	mwmr_ctrl.p_resetn(signal_resetn);
	mwmr_ctrl.p_from_coproc[0](signal_mwmr_from_coproc0);   //Data OUT
	mwmr_ctrl.p_from_coproc[1](signal_mwmr_from_coproc1);   //Data OUT
	mwmr_ctrl.p_to_coproc[0](signal_mwmr_to_coproc0);       //Data IN
	mwmr_ctrl.p_to_coproc[1](signal_mwmr_to_coproc1);       //Data IN
	mwmr_ctrl.p_to_coproc[2](signal_mwmr_config);           //Configuration FIFO port
	mwmr_ctrl.p_vci_initiator(signal_vci_mwmr_initiator);   //VCI port
	mwmr_ctrl.p_vci_target(signal_vci_mwmr_target);
    for (int i=0; i<6; i++) {
	    mwmr_ctrl.p_config[i](signal_p_config[i]);          //Configuration registers
	    mwmr_ctrl.p_status[i](signal_p_status[i]);          //Status register
    }

    //TRX_OFDM
    trx_ofdm.p_clk(signal_clk);
	trx_ofdm.p_resetn(signal_resetn);
    trx_ofdm.p_from_MWMR[0](signal_mwmr_to_coproc0);
    trx_ofdm.p_from_MWMR[1](signal_mwmr_to_coproc1);
    trx_ofdm.p_to_MWMR[0](signal_mwmr_from_coproc0);
    trx_ofdm.p_to_MWMR[1](signal_mwmr_from_coproc1);
    trx_ofdm.p_core_config(signal_mwmr_config);
    for (int i=0; i<6; i++) {
	    trx_ofdm.p_config[i](signal_p_config[i]);          //Configuration registers
    }
    for (int i=0; i<5; i++) {
	    trx_ofdm.p_status[i](signal_p_status[i]);          //Status register
    }

    ///LOCKS
    vcilocks.p_clk(signal_clk);
	vcilocks.p_resetn(signal_resetn);
    vcilocks.p_vci(signal_vci_locks);


#ifdef _TRACEFILE
    // open trace file
    sc_trace_file *my_trace_file;
    my_trace_file = sc_create_vcd_trace_file ("system_trace");

    //// chronogrammes signaux CK et NRESET
    sc_trace(my_trace_file, signal_clk,         "CK");
    sc_trace(my_trace_file, signal_resetn,      "NRESET");

    signal_vci_m0.trace(my_trace_file, "vci_cache");
    signal_vci_vcimultiram2.trace(my_trace_file, "vci_mwmr_data");
    signal_vci_locks.trace(my_trace_file, "vci_locks");
 #endif

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

	for (int i = 0; i < ncycles ; ) {
		sc_start(sc_core::sc_time(10000, SC_NS));
        i+=10000;
		std::cout << "Time elapsed: "<<i<<" cycles." << std::endl;
	}
    sleep(5);
#ifdef _TRACEFILE
	sc_close_vcd_trace_file(my_trace_file);
 #endif
    return EXIT_SUCCESS;
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
            
// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
