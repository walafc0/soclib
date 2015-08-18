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
 * File : top.cc
 * Date : 05/05/2007
 * Author :  Francois Charot
 * 
 * This architecture is ISS2 API compliant.
 * It contains:
 *  - 1 VCI Generic Micro Network (1 initiators /5 targets)
 *  - 1 NIOS2 processor with External VciXcacheWrapper Data/Instruction cache (old Xcache)
 *  - 2 VCI target VCI RAM
 *  - 1 VCI target TTY display
 *  - 1 VCI Timer
 *  - 1 VCI framebuffer
 *
 *
 *********************************************************************
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
  //   typedef soclib::caba::VciParams<4,1,32,1,1,1,8,1,1,1> vci_param;
  typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

  // Mapping table
  soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

  maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
  maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
  maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
  maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
  
  //  maptab.add(Segment("loc0" , LOC0_BASE , LOC0_SIZE , IntTab(1), true));
  
  maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));
  maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(3), false));

  maptab.add(Segment("fb", FB_BASE, FB_SIZE, IntTab(4), false));

  // Signals
  sc_clock		signal_clk("signal_clk");
  sc_signal<bool> signal_resetn("signal_resetn");
   
  soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");

  soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
  soclib::caba::VciSignals<vci_param> signal_vci_vciram0("signal_vci_vciram0");
  soclib::caba::VciSignals<vci_param> signal_vci_vcitimer("signal_vci_vcitimer");
  soclib::caba::VciSignals<vci_param> signal_vci_vcifb("signal_vci_vcifb");
  soclib::caba::VciSignals<vci_param> signal_vci_vciram1("signal_vci_vciram1");

  sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 

  sc_signal<bool>        signal_nios2_irq[32]; 
  
  // Components
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > nios2("nios2", 0, maptab,IntTab(0),  1,8,4, 1,8,4);
#   warning Using a NIOS II
  soclib::common::Loader loader("soft/bin.soft");
  soclib::caba::VciRam<vci_param> vciram0("vciram0", IntTab(0), maptab, loader);
  soclib::caba::VciRam<vci_param> vciram1("vciram1", IntTab(1), maptab, loader);
  soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", NULL);
  soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(3), maptab, 1);
  soclib::caba::VciFrameBuffer<vci_param> vcifb("vcifb", IntTab(4), maptab, FB_WIDTH, FB_HEIGHT, FB_MODE); 

  soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 1, 5, 2, 8);

  //	Net-List

  nios2.p_clk(signal_clk);  
  vciram0.p_clk(signal_clk);
  vciram1.p_clk(signal_clk);
  vcifb.p_clk(signal_clk);
  vcitimer.p_clk(signal_clk);
  
  nios2.p_resetn(signal_resetn);  
  vciram0.p_resetn(signal_resetn);
  vciram1.p_resetn(signal_resetn);
  vcifb.p_resetn(signal_resetn);
  vcitimer.p_resetn(signal_resetn);
  
  for (int i = 0; i<32; i++)
    nios2.p_irq[i]      (signal_nios2_irq[i]);

  nios2.p_vci(signal_vci_m0);

  vciram0.p_vci(signal_vci_vciram0);

  vcitimer.p_vci(signal_vci_vcitimer);
  vcitimer.p_irq[0](signal_nios2_irq[0]); 
  
  vcifb.p_vci(signal_vci_vcifb);
  
  vciram1.p_vci(signal_vci_vciram1);

  vcitty.p_clk(signal_clk);
  vcitty.p_resetn(signal_resetn);
  vcitty.p_vci(signal_vci_tty);
  vcitty.p_irq[0](signal_tty_irq0); 

  vgmn.p_clk(signal_clk);
  vgmn.p_resetn(signal_resetn);

  vgmn.p_to_initiator[0](signal_vci_m0);

  vgmn.p_to_target[0](signal_vci_vciram0);
  vgmn.p_to_target[1](signal_vci_vciram1);
  vgmn.p_to_target[2](signal_vci_tty);
  vgmn.p_to_target[3](signal_vci_vcitimer);
  vgmn.p_to_target[4](signal_vci_vcifb);

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

