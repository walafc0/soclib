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
 * Copyright (C) IRISA/INRIA, 2007
 *         Francois Charot <charot@irisa.fr>
 *
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "iss2_simhelper.h"
#include "niosII.h"
#include "vci_xcache_wrapper.h"
#include "vci_timer.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_locks.h"
#include "vci_vgmn.h"

//#define USE_GDB_SERVER

#ifdef USE_GDB_SERVER
#include "gdbserver.h"
#endif

#include "segmentation.h"

#define SEGTYPEMASK 0x00300000

int _main(int argc, char *argv[]) {
  using namespace sc_core;
  // Avoid repeating these everywhere
  using soclib::common::IntTab;
  using soclib::common::Segment;

  // Define our VCI parameters
  typedef soclib::caba::VciParams<4,6,32,1,1,1,8,1,1,1> vci_param;

  // Mapping table
  soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

  maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
  maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
  maptab.add(Segment("text", TEXT_BASE, TEXT_SIZE, IntTab(0), true));

  maptab.add(Segment("data", DATA_BASE, DATA_SIZE, IntTab(1), true));

  maptab.add(Segment("loc0", LOC0_BASE, LOC0_SIZE, IntTab(1), true));
  maptab.add(Segment("loc1", LOC1_BASE, LOC1_SIZE, IntTab(1), true));
  maptab.add(Segment("loc2", LOC2_BASE, LOC2_SIZE, IntTab(1), true));
  maptab.add(Segment("loc3", LOC3_BASE, LOC3_SIZE, IntTab(1), true));

  maptab.add(Segment("tty", TTY_BASE, TTY_SIZE, IntTab(2), false));
  maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(3), false));
  maptab.add(Segment("locks", LOCKS_BASE, LOCKS_SIZE, IntTab(4), false));

  // Signals
  sc_clock signal_clk("signal_clk");
  sc_signal<bool> signal_resetn("signal_resetn");

  sc_signal<bool> signal_nios2_irq00("signal_nios2_irq0");
  sc_signal<bool> signal_nios2_irq0[32];

  sc_signal<bool> signal_nios2_irq10("signal_nios2_irq1");
  sc_signal<bool> signal_nios2_irq1[32];

  sc_signal<bool> signal_nios2_irq20("signal_nios2_irq2");
  sc_signal<bool> signal_nios2_irq2[32];

  sc_signal<bool> signal_nios2_irq30("signal_nios2_irq3");
  sc_signal<bool> signal_nios2_irq3[32];

  soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");
  soclib::caba::VciSignals<vci_param> signal_vci_m1("signal_vci_m1");
  soclib::caba::VciSignals<vci_param> signal_vci_m2("signal_vci_m2");
  soclib::caba::VciSignals<vci_param> signal_vci_m3("signal_vci_m3");

  soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
  soclib::caba::VciSignals<vci_param>
    signal_vci_vciram0("signal_vci_vciram0");
  soclib::caba::VciSignals<vci_param>
    signal_vci_vcitimer("signal_vci_vcitimer");
  soclib::caba::VciSignals<vci_param>
    signal_vci_vcilocks("signal_vci_vcilocks");
  soclib::caba::VciSignals<vci_param>
    signal_vci_vciram1("signal_vci_vciram1");

  sc_signal<bool> signal_tty_irq0("signal_tty_irq0");
  sc_signal<bool> signal_tty_irq1("signal_tty_irq1");
  sc_signal<bool> signal_tty_irq2("signal_tty_irq2");
  sc_signal<bool> signal_tty_irq3("signal_tty_irq3");

  // Components

#ifdef USE_GDB_SERVER
  // uncomment this line if you want processors frozen at boot
#   warning Using a NIOS II
#   warning Using GDB
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::GdbServer<soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > > nios20("nios20", 0, maptab, IntTab(0), 4,1,8, 4,1,8);
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::GdbServer<soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > > nios21("nios21", 1, maptab, IntTab(1), 4,1,8, 4,1,8);
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::GdbServer<soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > > nios22("nios22", 2, maptab, IntTab(2), 4,1,8, 4,1,8);
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::GdbServer<soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > > nios23("nios23", 3, maptab, IntTab(3), 4,1,8, 4,1,8);

#else
#   warning Using a NIOS II
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > nios20("nios20", 0, maptab,IntTab(0),  4,1,8, 4,1,8);
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > nios21("nios21", 1, maptab,IntTab(1),  4,1,8, 4,1,8); 
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > nios22("nios22", 2, maptab,IntTab(2),  4,1,8, 4,1,8); 
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > nios23("nios23", 3, maptab,IntTab(3),  4,1,8, 4,1,8); 
#endif

  soclib::common::Loader loader("soft/bin.soft");
  soclib::caba::VciRam<vci_param> vciram0("vciram0", IntTab(0), maptab, loader);
  soclib::caba::VciRam<vci_param> vciram1("vciram1", IntTab(1), maptab, loader);
  soclib::caba::VciMultiTty<vci_param> vcitty("vcitty", IntTab(2), maptab, "vcitty0", "vcitty1", "vcitty2", "vcitty3", NULL);
  soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(3), maptab, 4);
  soclib::caba::VciLocks<vci_param> vcilocks("vcilocks", IntTab(4), maptab);

  soclib::caba::VciVgmn<vci_param> vgmn("vgmn", maptab, 4, 5, 2, 8);

  //	Net-List
  nios20.p_clk(signal_clk);
  nios21.p_clk(signal_clk);
  nios22.p_clk(signal_clk);
  nios23.p_clk(signal_clk);

  vciram0.p_clk(signal_clk);
  vciram1.p_clk(signal_clk);
  vcilocks.p_clk(signal_clk);
  vcitimer.p_clk(signal_clk);
  vgmn.p_clk(signal_clk);
  vcitty.p_clk(signal_clk);

  nios20.p_resetn(signal_resetn);
  nios21.p_resetn(signal_resetn);
  nios22.p_resetn(signal_resetn);
  nios23.p_resetn(signal_resetn);

  vciram0.p_resetn(signal_resetn);
  vciram1.p_resetn(signal_resetn);
  vcilocks.p_resetn(signal_resetn);
  vcitimer.p_resetn(signal_resetn);
  vgmn.p_resetn(signal_resetn);
  vcitty.p_resetn(signal_resetn);

  for (int i = 1; i<32; i++)
    nios20.p_irq[i](signal_nios2_irq0[i]);
  nios20.p_irq[0](signal_nios2_irq00);

  for (int i = 1; i<32; i++)
    nios21.p_irq[i](signal_nios2_irq1[i]);
  nios21.p_irq[0](signal_nios2_irq10);

  for (int i = 1; i<32; i++)
    nios22.p_irq[i](signal_nios2_irq2[i]);
  nios22.p_irq[0](signal_nios2_irq20);

  for (int i = 1; i<32; i++)
    nios23.p_irq[i](signal_nios2_irq3[i]);
  nios23.p_irq[0](signal_nios2_irq30);

  nios20.p_vci(signal_vci_m0);
  nios21.p_vci(signal_vci_m1);
  nios22.p_vci(signal_vci_m2);
  nios23.p_vci(signal_vci_m3);

  vciram0.p_vci(signal_vci_vciram0);

  vcitimer.p_vci(signal_vci_vcitimer);
  vcitimer.p_irq[0](signal_nios2_irq00);
  vcitimer.p_irq[1](signal_nios2_irq10);
  vcitimer.p_irq[2](signal_nios2_irq20);
  vcitimer.p_irq[3](signal_nios2_irq30);

  vcilocks.p_vci(signal_vci_vcilocks);

  vciram1.p_vci(signal_vci_vciram1);

  vcitty.p_vci(signal_vci_tty);
  vcitty.p_irq[0](signal_tty_irq0);
  vcitty.p_irq[1](signal_tty_irq1);
  vcitty.p_irq[2](signal_tty_irq2);
  vcitty.p_irq[3](signal_tty_irq3);

  vgmn.p_to_initiator[0](signal_vci_m0);
  vgmn.p_to_initiator[1](signal_vci_m1);
  vgmn.p_to_initiator[2](signal_vci_m2);
  vgmn.p_to_initiator[3](signal_vci_m3);

  vgmn.p_to_target[0](signal_vci_vciram0);
  vgmn.p_to_target[1](signal_vci_vciram1);
  vgmn.p_to_target[2](signal_vci_tty);
  vgmn.p_to_target[3](signal_vci_vcitimer);
  vgmn.p_to_target[4](signal_vci_vcilocks);

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
