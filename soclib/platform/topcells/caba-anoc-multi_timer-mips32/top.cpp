/*
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
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors : Ivan MIRO-PANADES
 * 
 * Maintainers: ivan.miro-panades@cea.fr
 */
//#define _TRACEFILE

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "iss2_simhelper.h"
#include "mips32.h"
#include "vci_xcache_wrapper.h"
#include "vci_timer.h"
#include "vci_simple_ram.h"
#include "vci_multi_tty.h"
#include "vci_locks.h"
#include "vci_anoc_network.h"
#include "vci_local_crossbar.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
    using namespace sc_core;
    // Avoid repeating these everywhere
    using soclib::common::IntTab;
    using soclib::common::Segment;

    // Define our VCI parameters
    typedef soclib::caba::VciParams<4,6,32,1,1,1,12,1,1,1> vci_param;

    // Mapping table

    //VCI ADDRESS width = 32 bits, Global routing= 8 bits, Local routing = 4 bits
    soclib::common::MappingTable maptab(32, IntTab(8,4), IntTab(8,4), 0x00300000);

    maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0x00,0), true));
    maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0x00,0), true));
    maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0x00,0), true));

    maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(0x00,0), true));

    maptab.add(Segment("loc0" , LOC0_BASE , LOC0_SIZE , IntTab(0x00,0), true));
    maptab.add(Segment("loc1" , LOC1_BASE , LOC1_SIZE , IntTab(0x00,0), true));
    maptab.add(Segment("loc2" , LOC2_BASE , LOC2_SIZE , IntTab(0x00,0), true));
    maptab.add(Segment("loc3" , LOC3_BASE , LOC3_SIZE , IntTab(0x00,0), true));

    maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(0x01,0), false));
    maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(0x10,0), false));
    maptab.add(Segment("locks", LOCKS_BASE, LOCKS_SIZE, IntTab(0x11,0), false));

    // Signals

    sc_clock        signal_clk("signal_clk");
    sc_signal<bool> signal_resetn("signal_resetn");

    sc_signal<bool> signal_mips0_it0("signal_mips0_it0"); 
    sc_signal<bool> signal_mips0_it1("signal_mips0_it1"); 
    sc_signal<bool> signal_mips0_it2("signal_mips0_it2"); 
    sc_signal<bool> signal_mips0_it3("signal_mips0_it3"); 
    sc_signal<bool> signal_mips0_it4("signal_mips0_it4"); 
    sc_signal<bool> signal_mips0_it5("signal_mips0_it5");

    sc_signal<bool> signal_mips1_it0("signal_mips1_it0"); 
    sc_signal<bool> signal_mips1_it1("signal_mips1_it1"); 
    sc_signal<bool> signal_mips1_it2("signal_mips1_it2"); 
    sc_signal<bool> signal_mips1_it3("signal_mips1_it3"); 
    sc_signal<bool> signal_mips1_it4("signal_mips1_it4"); 
    sc_signal<bool> signal_mips1_it5("signal_mips1_it5");

    sc_signal<bool> signal_mips2_it0("signal_mips2_it0"); 
    sc_signal<bool> signal_mips2_it1("signal_mips2_it1"); 
    sc_signal<bool> signal_mips2_it2("signal_mips2_it2"); 
    sc_signal<bool> signal_mips2_it3("signal_mips2_it3"); 
    sc_signal<bool> signal_mips2_it4("signal_mips2_it4"); 
    sc_signal<bool> signal_mips2_it5("signal_mips2_it5");

    sc_signal<bool> signal_mips3_it0("signal_mips3_it0"); 
    sc_signal<bool> signal_mips3_it1("signal_mips3_it1"); 
    sc_signal<bool> signal_mips3_it2("signal_mips3_it2"); 
    sc_signal<bool> signal_mips3_it3("signal_mips3_it3"); 
    sc_signal<bool> signal_mips3_it4("signal_mips3_it4"); 
    sc_signal<bool> signal_mips3_it5("signal_mips3_it5");

    soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");
    soclib::caba::VciSignals<vci_param> signal_vci_m1("signal_vci_m1");
    soclib::caba::VciSignals<vci_param> signal_vci_m2("signal_vci_m2");
    soclib::caba::VciSignals<vci_param> signal_vci_m3("signal_vci_m3");

    soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
    soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
    soclib::caba::VciSignals<vci_param> signal_vci_vcitimer("signal_vci_vcitimer");
    soclib::caba::VciSignals<vci_param> signal_vci_vcilocks("signal_vci_vcilocks");

    soclib::caba::VciSignals<vci_param> signal_vci_C00_I("signal_vci_C00_I");
    soclib::caba::VciSignals<vci_param> signal_vci_C01_I("signal_vci_C01_I");
    soclib::caba::VciSignals<vci_param> signal_vci_C10_I("signal_vci_C10_I");
    soclib::caba::VciSignals<vci_param> signal_vci_C11_I("signal_vci_C11_I");

    soclib::caba::VciSignals<vci_param> signal_vci_C00_T("signal_vci_C00_T");
    soclib::caba::VciSignals<vci_param> signal_vci_C01_T("signal_vci_C01_T");
    soclib::caba::VciSignals<vci_param> signal_vci_C10_T("signal_vci_C10_T");
    soclib::caba::VciSignals<vci_param> signal_vci_C11_T("signal_vci_C11_T");

    sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 
    sc_signal<bool> signal_tty_irq1("signal_tty_irq1"); 
    sc_signal<bool> signal_tty_irq2("signal_tty_irq2"); 
    sc_signal<bool> signal_tty_irq3("signal_tty_irq3"); 

    // Components

    soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache0("cache0", 0, maptab,IntTab(0x00,0), 4,1,8, 4,1,8);
    soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache1("cache1", 1, maptab,IntTab(0x01,0), 4,1,8, 4,1,8);
    soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache2("cache2", 2, maptab,IntTab(0x10,0), 4,1,8, 4,1,8);
    soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache3("cache3", 3, maptab,IntTab(0x11,0), 4,1,8, 4,1,8);

    soclib::common::Loader loader("soft/bin.soft");
    soclib::caba::VciSimpleRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0x00,0), maptab, loader);
    soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",   IntTab(0x01,0), maptab, "vcitty0", "vcitty1", "vcitty2", "vcitty3", NULL);
    soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(0x10,0), maptab, 4);
    soclib::caba::VciLocks<vci_param> vcilocks("vcilocks",  IntTab(0x11,0), maptab); 

    soclib::caba::VciLocalCrossbar<vci_param> cluster00("cluster00",maptab, IntTab(0x00), IntTab(0x00), 1, 1 );
    soclib::caba::VciLocalCrossbar<vci_param> cluster01("cluster01",maptab, IntTab(0x01), IntTab(0x01), 1, 1 );
    soclib::caba::VciLocalCrossbar<vci_param> cluster10("cluster10",maptab, IntTab(0x10), IntTab(0x10), 1, 1 );
    soclib::caba::VciLocalCrossbar<vci_param> cluster11("cluster11",maptab, IntTab(0x11), IntTab(0x11), 1, 1 );

    soclib::caba::VciAnocNetwork<vci_param, 4, 4> anocNetwork("anocNetwork", maptab, 2, 2);

    // Net-List

    cache0.p_clk(signal_clk);
    cache1.p_clk(signal_clk);
    cache2.p_clk(signal_clk);
    cache3.p_clk(signal_clk);
    vcimultiram0.p_clk(signal_clk);
    vcilocks.p_clk(signal_clk);
    vcitimer.p_clk(signal_clk);
    cluster00.p_clk(signal_clk);
    cluster01.p_clk(signal_clk);
    cluster10.p_clk(signal_clk);
    cluster11.p_clk(signal_clk);
    anocNetwork.p_clk(signal_clk);

    cache0.p_resetn(signal_resetn);
    cache1.p_resetn(signal_resetn);
    cache2.p_resetn(signal_resetn);
    cache3.p_resetn(signal_resetn);
    vcimultiram0.p_resetn(signal_resetn);
    vcilocks.p_resetn(signal_resetn);
    vcitimer.p_resetn(signal_resetn);
    cluster00.p_resetn(signal_resetn);
    cluster01.p_resetn(signal_resetn);
    cluster10.p_resetn(signal_resetn);
    cluster11.p_resetn(signal_resetn);
    anocNetwork.p_resetn(signal_resetn);

    cache0.p_irq[0](signal_mips0_it0); 
    cache0.p_irq[1](signal_mips0_it1); 
    cache0.p_irq[2](signal_mips0_it2); 
    cache0.p_irq[3](signal_mips0_it3); 
    cache0.p_irq[4](signal_mips0_it4); 
    cache0.p_irq[5](signal_mips0_it5); 

    cache1.p_irq[0](signal_mips1_it0); 
    cache1.p_irq[1](signal_mips1_it1); 
    cache1.p_irq[2](signal_mips1_it2); 
    cache1.p_irq[3](signal_mips1_it3); 
    cache1.p_irq[4](signal_mips1_it4); 
    cache1.p_irq[5](signal_mips1_it5); 

    cache2.p_irq[0](signal_mips2_it0); 
    cache2.p_irq[1](signal_mips2_it1); 
    cache2.p_irq[2](signal_mips2_it2); 
    cache2.p_irq[3](signal_mips2_it3); 
    cache2.p_irq[4](signal_mips2_it4); 
    cache2.p_irq[5](signal_mips2_it5); 

    cache3.p_irq[0](signal_mips3_it0); 
    cache3.p_irq[1](signal_mips3_it1); 
    cache3.p_irq[2](signal_mips3_it2); 
    cache3.p_irq[3](signal_mips3_it3); 
    cache3.p_irq[4](signal_mips3_it4); 
    cache3.p_irq[5](signal_mips3_it5); 

    cache0.p_vci(signal_vci_m0);
    cache1.p_vci(signal_vci_m1);
    cache2.p_vci(signal_vci_m2);
    cache3.p_vci(signal_vci_m3);

    vcimultiram0.p_vci(signal_vci_vcimultiram0);

    vcitimer.p_vci(signal_vci_vcitimer);
    vcitimer.p_irq[0](signal_mips0_it0); 
    vcitimer.p_irq[1](signal_mips1_it0); 
    vcitimer.p_irq[2](signal_mips2_it0); 
    vcitimer.p_irq[3](signal_mips3_it0); 

    vcilocks.p_vci(signal_vci_vcilocks);

    vcitty.p_clk(signal_clk);
    vcitty.p_resetn(signal_resetn);
    vcitty.p_vci(signal_vci_tty);
    vcitty.p_irq[0](signal_tty_irq0); 
    vcitty.p_irq[1](signal_tty_irq1); 
    vcitty.p_irq[2](signal_tty_irq2); 
    vcitty.p_irq[3](signal_tty_irq3); 


    //Cluster [0,0]
    cluster00.p_to_target[0](signal_vci_vcimultiram0);
    cluster00.p_to_initiator[0](signal_vci_m0);
    cluster00.p_target_to_up(signal_vci_C00_T);
    cluster00.p_initiator_to_up(signal_vci_C00_I);
    anocNetwork.p_to_initiator[0][0](signal_vci_C00_I);
    anocNetwork.p_to_target[0][0](signal_vci_C00_T);

    //Cluster [0,1]
    cluster01.p_to_target[0](signal_vci_tty);
    cluster01.p_to_initiator[0](signal_vci_m1);
    cluster01.p_target_to_up(signal_vci_C01_T);
    cluster01.p_initiator_to_up(signal_vci_C01_I);
    anocNetwork.p_to_initiator[0][1](signal_vci_C01_I);
    anocNetwork.p_to_target[0][1](signal_vci_C01_T);

    //Cluster [1,0]
    cluster10.p_to_target[0](signal_vci_vcitimer);
    cluster10.p_to_initiator[0](signal_vci_m2);
    cluster10.p_target_to_up(signal_vci_C10_T);
    cluster10.p_initiator_to_up(signal_vci_C10_I);
    anocNetwork.p_to_initiator[1][0](signal_vci_C10_I);
    anocNetwork.p_to_target[1][0](signal_vci_C10_T);

    //Cluster [1,1]
    cluster11.p_to_target[0](signal_vci_vcilocks);
    cluster11.p_to_initiator[0](signal_vci_m3);
    cluster11.p_target_to_up(signal_vci_C11_T);
    cluster11.p_initiator_to_up(signal_vci_C11_I);
    anocNetwork.p_to_initiator[1][1](signal_vci_C11_I);
    anocNetwork.p_to_target[1][1](signal_vci_C11_T);

#ifdef _TRACEFILE
    // open trace file
    sc_trace_file *my_trace_file;
    my_trace_file = sc_create_vcd_trace_file ("system_trace");

    //// chronogrammes signaux CK et NRESET
    sc_trace(my_trace_file, signal_clk,    "CK");
    sc_trace(my_trace_file, signal_resetn, "NRESET");

    signal_vci_m0.trace(my_trace_file, "vci_m0");
    signal_vci_m1.trace(my_trace_file, "vci_m1");
    signal_vci_m2.trace(my_trace_file, "vci_m2");
    signal_vci_m3.trace(my_trace_file, "vci_m3");

    signal_vci_vcimultiram0.trace(my_trace_file, "vci_vcimultiram0");
    signal_vci_tty.trace(my_trace_file, "vci_tty");
    signal_vci_vcitimer.trace(my_trace_file, "vci_vcitimer");
    signal_vci_vcilocks.trace(my_trace_file, "vci_vcilocks");

    signal_vci_C00_I.trace(my_trace_file, "signal_vci_C00_I");
    signal_vci_C01_I.trace(my_trace_file, "signal_vci_C01_I");
    signal_vci_C10_I.trace(my_trace_file, "signal_vci_C10_I");
    signal_vci_C11_I.trace(my_trace_file, "signal_vci_C11_I");

    signal_vci_C00_T.trace(my_trace_file, "signal_vci_C00_T");
    signal_vci_C01_T.trace(my_trace_file, "signal_vci_C01_T");
    signal_vci_C10_T.trace(my_trace_file, "signal_vci_C10_T");
    signal_vci_C11_T.trace(my_trace_file, "signal_vci_C11_T");
#endif

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

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
