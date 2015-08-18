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
 * Copyright (c) TelecomParisTECH
 *         Tarik Graba <tarik.graba@telecom-paristech.fr>, 2010
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 *
 * $Id$
 *
 * History:
 * - 2010-04-19
 *   Tarik Graba : Ref. platform for the BE WB lm32
 */


#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "iss2_simhelper.h"
#include "lm32.h"

#include "wb_signal.h"
#include "wb_xcache_wrapper.h"
#include "wb_interco.h"
#include "wb_slave_vci_initiator_wrapper.h"

#include "vci_ram.h"
#include "vci_multi_tty.h"


#include "segmentation.h"

int _main(int argc, char *argv[])
{
    using namespace sc_core;
    // Avoid repeating these everywhere
    using soclib::common::IntTab;
    using soclib::common::Segment;

    // Define our VCI parameters
    typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

    // Define our WB parameters
    typedef soclib::caba::WbParams<32,32> wb_param;

    // Mapping table

    soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x20000000);

    maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
    maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
    maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));

    // Signals

#ifdef SOCVIEW
    sc_clock	signal_clk("signal_clk");
#else
    // modelsim fails to simulate designs where clock
    // signals have no explicit paeriod
    sc_time     clk_period(10,SC_NS);
    sc_clock	signal_clk("signal_clk",clk_period);
#endif
    sc_signal<bool> signal_resetn("signal_resetn");

    // unconnected irq signal
    sc_signal<bool> uncon_lm32_it ;
    // test irq
    sc_signal<bool> test_lm32_it ;

    soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
    soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
    soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");

    // WB interconnect signals
    soclib::caba::WbSignal<wb_param> signal_wb_lm32("signal_wb_lm32");
    soclib::caba::WbSignal<wb_param> signal_wb_ram0("signal_wb_ram0");
    soclib::caba::WbSignal<wb_param> signal_wb_ram1("signal_wb_ram1");
    soclib::caba::WbSignal<wb_param> signal_wb_tty("signal_wb_tty");

    sc_signal<bool> signal_tty_irq0("signal_tty_irq0");
    sc_signal<bool> signal_tty_irq1("signal_tty_irq1");
    sc_signal<bool> signal_tty_irq2("signal_tty_irq2");
    sc_signal<bool> signal_tty_irq3("signal_tty_irq3");

    // Components
    // lm32 real cache configuration can be:
    // Ways 1 or 2
    // Sets 128,256,512 or 1024
    // Bytes per line 4, 8 or 16
    // Here we have 2 way, 128 set and 8 bytes per set
    // To simulate a cache less processor these parameters should be
    // changed to 1,1,4
    // LM32Iss template parameter lEndianInterface = 'false' => Big endian interface,
    soclib::caba::WbXcacheWrapper<wb_param, soclib::common::Iss2Simhelper<soclib::common::LM32Iss <false> > >
        lm32("lm32", 0, maptab,IntTab(0), 2,128,8, 2,128,8);

    soclib::common::Loader loader("soft/bin.soft");

    soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
    soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
    soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", NULL);

    // WB interconnect
    //                                           sc_name    maptab  masters slaves
    soclib::caba::WbInterco<wb_param> wbinterco("wbinterco",maptab, 1,       3    );

    ////////////////////////////////////////////////////////////
    /////////////////// WB<->VCI Wrappers //////////////////////
    ////////////////////////////////////////////////////////////

    // The WbSlaveVciInitiatorWrapper defaults behaviour converts endianess from wb BE to vci LE
    // To keep the same endianess add "false" as constructor argument
    // When using this wrapper with vci vgmn consider adding the mapping table and the index IntTab

    // ram0
    soclib::caba::WbSlaveVciInitiatorWrapper<vci_param, wb_param> ram0_w ("ram0_w") ;
    ram0_w.p_clk               (signal_clk);
    ram0_w.p_resetn            (signal_resetn);
    ram0_w.p_vci               (signal_vci_vcimultiram0);
    ram0_w.p_wb                (signal_wb_ram0);

    // ram1
    soclib::caba::WbSlaveVciInitiatorWrapper<vci_param, wb_param> ram1_w ("ram1_w") ;
    ram1_w.p_clk               (signal_clk);
    ram1_w.p_resetn            (signal_resetn);
    ram1_w.p_vci               (signal_vci_vcimultiram1);
    ram1_w.p_wb                (signal_wb_ram1);


    // tty
    soclib::caba::WbSlaveVciInitiatorWrapper<vci_param, wb_param> tty_w ("tty_w") ;
    tty_w.p_clk               (signal_clk);
    tty_w.p_resetn            (signal_resetn);
    tty_w.p_vci               (signal_vci_tty);
    tty_w.p_wb                (signal_wb_tty);

    ////////////////////////////////////////////////////////////
    ///////////////////// WB Net List //////////////////////////
    ////////////////////////////////////////////////////////////

    wbinterco.p_clk(signal_clk);
    wbinterco.p_resetn(signal_resetn);

    wbinterco.p_from_master[0](signal_wb_lm32);

    wbinterco.p_to_slave[0](signal_wb_ram0);
    wbinterco.p_to_slave[1](signal_wb_ram1);
    wbinterco.p_to_slave[2](signal_wb_tty);

    lm32.p_clk(signal_clk);
    lm32.p_resetn(signal_resetn);
    // LM32 irq are normally active low
    // but the iss checks for active high irqs to be easier with SocLib
    lm32.p_irq[0] (signal_tty_irq0);
    lm32.p_irq[1] (test_lm32_it);
    for (int i=2; i<32; i++)
        lm32.p_irq[i] (uncon_lm32_it);

    lm32.p_wb(signal_wb_lm32);

    ////////////////////////////////////////////////////////////
    //////////////////// VCI Net List //////////////////////////
    ////////////////////////////////////////////////////////////

    vcimultiram0.p_clk(signal_clk);
    vcimultiram0.p_resetn(signal_resetn);
    vcimultiram0.p_vci(signal_vci_vcimultiram0);

    vcimultiram1.p_clk(signal_clk);
    vcimultiram1.p_resetn(signal_resetn);
    vcimultiram1.p_vci(signal_vci_vcimultiram1);

    vcitty.p_clk(signal_clk);
    vcitty.p_resetn(signal_resetn);
    vcitty.p_vci(signal_vci_tty);
    vcitty.p_irq[0](signal_tty_irq0);

    ////////////////////////////////////////////////////////////
    //////////////////Simulation Starts Here////////////////////
    ////////////////////////////////////////////////////////////

    sc_start(sc_core::sc_time(0, SC_NS));
    signal_resetn = false;
    sc_start(sc_core::sc_time(1, SC_NS));
    signal_resetn = true;

    std::cout << maptab;

    uncon_lm32_it = false;
    test_lm32_it = false;

#ifdef SOCVIEW
    debug();
#else
    // some code to test irqs
    for (int i = 0; i< 9000; i++)
    sc_start(clk_period);
    test_lm32_it = true;
    for (int i = 0; i< 10; i++)
    sc_start(clk_period);
    test_lm32_it = false;

    sc_start();
#endif
    return EXIT_SUCCESS;
}

int sc_main (int argc, char *argv[])
{
    return _main(argc, argv);
}
