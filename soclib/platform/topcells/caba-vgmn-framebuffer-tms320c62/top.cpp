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
 * Copyright (C) IRISA/INRIA, 2007-2008
 *         Francois Charot <charot@irisa.fr>
 *
 * File : top.cpp
 * Date : 15/12/2008
 * Author :  Francois Charot
 *
 * Copyright : IRISA
 *
 * This architecture contains:
 *  - 1 VCI Generic Micro Network (1 initiators /5 targets)
 *  - 1 TMS320C62 with External Data/Instruction cache (old Xcache)
 *  - 2 VCI target VCI RAM
 *  - 1 VCI target TTY display
 *  - 1 VCI Timer
 *  - 1 VCI framebuffer
 *
 *********************************************************************
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "tms320c62.h"
#include "vci_xcache_wrapper.h"
#include "ississ2.h"
#include "vci_timer.h"
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
  soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

  maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));

  maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));
  maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(3), false));

  maptab.add(Segment("fb", FB_BASE, FB_SIZE, IntTab(4), false));

  // Signals
  sc_clock		signal_clk("signal_clk");
  sc_signal<bool> signal_resetn("signal_resetn");

  soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");

  soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
  soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
  soclib::caba::VciSignals<vci_param> signal_vci_vcitimer("signal_vci_vcitimer");
  soclib::caba::VciSignals<vci_param> signal_vci_vcifb("signal_vci_vcifb");
  soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");

  sc_signal<bool> signal_tty_irq0("signal_tty_irq0");

  sc_signal<bool>        signal_tms320c62_irq[32];

  // Components
  typedef soclib::common::IssIss2<soclib::common::Tms320C6xIss> iss_t;
  
  soclib::caba::VciXcacheWrapper<vci_param, iss_t > tms320c62("tms320c62", 0,maptab,IntTab(0),1,16,8,1,16,8);

  soclib::common::Loader loader("soft/bin.soft");
  soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
  soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
  soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", NULL);
  soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(3), maptab, 1);
  soclib::caba::VciFrameBuffer<vci_param> vcifb("vcifb", IntTab(4), maptab, FB_WIDTH, FB_HEIGHT);

  soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 1, 5, 2, 8);

  //	Net-List

  tms320c62.p_clk(signal_clk);
  vcimultiram0.p_clk(signal_clk);
  vcimultiram1.p_clk(signal_clk);
  vcifb.p_clk(signal_clk);
  vcitimer.p_clk(signal_clk);

  tms320c62.p_resetn(signal_resetn);
  vcimultiram0.p_resetn(signal_resetn);
  vcimultiram1.p_resetn(signal_resetn);
  vcifb.p_resetn(signal_resetn);
  vcitimer.p_resetn(signal_resetn);

  for (int i = 0; i<32; i++)
    tms320c62.p_irq[i]      (signal_tms320c62_irq[i]);

  tms320c62.p_vci(signal_vci_m0);

  vcimultiram0.p_vci(signal_vci_vcimultiram0);

  vcitimer.p_vci(signal_vci_vcitimer);
  vcitimer.p_irq[0](signal_tms320c62_irq[0]);

  vcifb.p_vci(signal_vci_vcifb);

  vcimultiram1.p_vci(signal_vci_vcimultiram1);

  vcitty.p_clk(signal_clk);
  vcitty.p_resetn(signal_resetn);
  vcitty.p_vci(signal_vci_tty);
  vcitty.p_irq[0](signal_tty_irq0);

  vgmn.p_clk(signal_clk);
  vgmn.p_resetn(signal_resetn);

  vgmn.p_to_initiator[0](signal_vci_m0);

  vgmn.p_to_target[0](signal_vci_vcimultiram0);
  vgmn.p_to_target[1](signal_vci_vcimultiram1);
  vgmn.p_to_target[2](signal_vci_tty);
  vgmn.p_to_target[3](signal_vci_vcitimer);
  vgmn.p_to_target[4](signal_vci_vcifb);

  int ncycles;
  clock_t starttime, endtime;


  // Starting execution timing
  starttime = clock ();

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

  for (int i = 0; i < ncycles ; i++) {
    sc_start(sc_core::sc_time(1, SC_NS));

    if((i % 100000) == 0)
      std::cout
	<< "Time elapsed: "<<i<<" cycles." << std::endl;
  }


  endtime = clock ();

  double simtime = (1.0 * (endtime - starttime) / CLOCKS_PER_SEC);
  //  double simcycles = sc_time_stamp().to_seconds();
  std::cout << "**" << std::endl;
  //  std::cout << "** simulation time (in seconds) " << simtime  << std::endl;
  std::cout << "** simulation time (in seconds) " << simtime  << std::endl;
  std::cout << "** simulated cycles : " << ncycles-1 << "  (" << ((ncycles-1) / simtime) << " c/s)" << std::endl;
  std::cout << "**" << std::endl;
  std::cout << "Hit ENTER to end simulation" << std::endl;

  char buf[1];

  std::cin.getline(buf,2);
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
