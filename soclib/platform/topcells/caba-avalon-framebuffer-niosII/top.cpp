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
 * Date : 20/11/2008
 * Author :  Francois Charot
 *           Charles Wagner
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
#include "avalon_switch_config.h"
#include "avalon_switch_fabric.h"
#include "vci_avalon_initiator_wrapper.h"
#include "vci_avalon_target_wrapper.h"

//#define USE_GDB_SERVER

#ifdef USE_GDB_SERVER
#include "gdbserver.h"
#endif

#include "segmentation.h"

#define TRACE_FILE 0

int _main(int argc, char *argv[])
{
  using namespace sc_core;
  // Avoid repeating these everywhere
  using soclib::common::IntTab;
  using soclib::common::Segment;

  const int nb_master = 1;
  const int nb_slave = 5;

  // Define our VCI parameters
  typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

  // Define our AVALON parameters
  typedef soclib::caba::AvalonParams<32,32,8> avalon_param;


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


  soclib::caba::VciSignals<vci_param>  signal_vci_initiator_m0("signal_vci_initiator_m0");

  soclib::caba::VciSignals<vci_param> signal_vci_vcitty("signal_vci_vcitty");
  soclib::caba::VciSignals<vci_param> signal_vci_vciram0("signal_vci_vciram0");
  soclib::caba::VciSignals<vci_param> signal_vci_vcitimer("signal_vci_vcitimer");
  soclib::caba::VciSignals<vci_param> signal_vci_vcifb("signal_vci_vcifb");
  soclib::caba::VciSignals<vci_param> signal_vci_vciram1("signal_vci_vciram1");

  sc_signal<bool> signal_tty_irq0("signal_tty_irq0");

  sc_signal<bool>        signal_nios2_irq[32];
  sc_signal<bool> 		 signal_nios2_irq0("signal_nios2_irq0");

  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_m0("avalon_switch_wrapper_m0");

  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t0("avalon_switch_wrapper_t0");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t1("avalon_switch_wrapper_t1");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t2("avalon_switch_wrapper_t2");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t3("avalon_switch_wrapper_t3");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t4("avalon_switch_wrapper_t4");


#if defined(USE_GDB_SERVER)
#   warning Using a NIOS II
#   warning Using GDB
  typedef soclib::common::GdbServer<soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > iss_t;
#else
#   warning Using a NIOS II
  typedef soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> iss_t;
#endif

  // Components
  soclib::caba::VciXcacheWrapper<vci_param, iss_t > nios2("nios2", 0, maptab,IntTab(0),1,128,16, 1,128,16);

  soclib::common::Loader loader("soft/bin.soft");
  soclib::caba::VciRam<vci_param> vciram0("vciram0", IntTab(0), maptab, loader);
  soclib::caba::VciRam<vci_param> vciram1("vciram1", IntTab(1), maptab, loader);
  soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", NULL);
  soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(3), maptab, 1);
  soclib::caba::VciFrameBuffer<vci_param> vcifb("vcifb", IntTab(4), maptab, FB_WIDTH, FB_HEIGHT, FB_MODE);

  soclib::caba::AvalonSwitchConfig<nb_master, nb_slave> config_switch;
  soclib::caba::AvalonSwitchFabric<nb_master, nb_slave, avalon_param>   SwitchFabric("SwitchFabric", config_switch);
  soclib::caba::VciAvalonInitiatorWrapper<vci_param, avalon_param> cache0_wrapper("cache0_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> ram0_wrapper("ram0_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> ram1_wrapper("ram1_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> tty_wrapper("tty_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> timer_wrapper("timer_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> fb_wrapper("fb_wrapper");

  //	Net-List

  nios2.p_clk(signal_clk);
  vciram0.p_clk(signal_clk);
  vciram1.p_clk(signal_clk);
  vcitty.p_clk(signal_clk);
  vcitimer.p_clk(signal_clk);
  vcifb.p_clk(signal_clk);

  nios2.p_resetn(signal_resetn);
  vciram0.p_resetn(signal_resetn);
  vciram1.p_resetn(signal_resetn);
  vcitty.p_resetn(signal_resetn);
  vcitimer.p_resetn(signal_resetn);
  vcifb.p_resetn(signal_resetn);

  for (int i = 1; i<32; i++)
    nios2.p_irq[i]      (signal_nios2_irq[i]);
  nios2.p_irq[0](signal_nios2_irq0);


  nios2.p_vci(signal_vci_initiator_m0);

  cache0_wrapper.p_clk(signal_clk);
  cache0_wrapper.p_resetn(signal_resetn);
  cache0_wrapper.p_vci(signal_vci_initiator_m0);
  cache0_wrapper.p_avalon(avalon_switch_wrapper_m0);



  vciram0.p_vci(signal_vci_vciram0);

  ram0_wrapper.p_clk(signal_clk);
  ram0_wrapper.p_resetn(signal_resetn);
  ram0_wrapper.p_vci(signal_vci_vciram0);
  ram0_wrapper.p_avalon(avalon_switch_wrapper_t0);

  vciram1.p_vci(signal_vci_vciram1);

  ram1_wrapper.p_clk(signal_clk);
  ram1_wrapper.p_resetn(signal_resetn);
  ram1_wrapper.p_vci(signal_vci_vciram1);
  ram1_wrapper.p_avalon(avalon_switch_wrapper_t1);

  vcitty.p_vci(signal_vci_vcitty);
  vcitty.p_irq[0](signal_tty_irq0);

  tty_wrapper.p_clk(signal_clk);
  tty_wrapper.p_resetn(signal_resetn);
  tty_wrapper.p_vci(signal_vci_vcitty);
  tty_wrapper.p_avalon(avalon_switch_wrapper_t2);

  vcitimer.p_vci(signal_vci_vcitimer);
  vcitimer.p_irq[0](signal_nios2_irq[0]);

  timer_wrapper.p_clk(signal_clk);
  timer_wrapper.p_resetn(signal_resetn);
  timer_wrapper.p_vci(signal_vci_vcitimer);
  timer_wrapper.p_avalon(avalon_switch_wrapper_t3);

  vcifb.p_vci(signal_vci_vcifb);

  fb_wrapper.p_clk(signal_clk);
  fb_wrapper.p_resetn(signal_resetn);
  fb_wrapper.p_vci(signal_vci_vcifb);
  fb_wrapper.p_avalon(avalon_switch_wrapper_t4);

  SwitchFabric.p_clk(signal_clk);
  SwitchFabric.p_resetn(signal_resetn);
  SwitchFabric.p_avalon_master[0](avalon_switch_wrapper_m0);

  SwitchFabric.p_avalon_slave[0](avalon_switch_wrapper_t0);
  SwitchFabric.p_avalon_slave[1](avalon_switch_wrapper_t1);
  SwitchFabric.p_avalon_slave[2](avalon_switch_wrapper_t2);
  SwitchFabric.p_avalon_slave[3](avalon_switch_wrapper_t3);
  SwitchFabric.p_avalon_slave[4](avalon_switch_wrapper_t4);


  //////////////////////////////////////////////////////////
  //	Traces
  //////////////////////////////////////////////////////////

#if TRACE_FILE
  sc_trace_file *my_trace_file;
  my_trace_file = sc_create_vcd_trace_file ("system_trace");

  sc_trace(my_trace_file, signal_clk, "signal_clk");

  sc_trace(my_trace_file, ram0_wrapper.r_fsm_state, "======ram0_r_fsm_state");
  sc_trace(my_trace_file, ram0_wrapper.r_address,   "======ram0_r_address");

  //==============================Traces ram0wrapper  --  AvalonSwitchFabric

  sc_trace(my_trace_file, avalon_switch_wrapper_t0.reset, "AVALON_ram0_RESET");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.resetrequest, "AVALON_ram0_RESETREQUEST");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.clk, "AVALON_ram0_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0. waitrequest, "AVALON_ram0_WAITREQUEST");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.write, "AVALON_ram0_WRITE");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.read, "AVALON_ram0_READ");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.chipselect, "AVALON_ram0_CHIPSELECT");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.address, "AVALON_ram0_ADDRESS_1");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.writedata, "AVALON_ram0_WRITEDATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.readdata, "AVALON_ram0_READDATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.byteenable, "AVALON_ram0_BYTEENABLE");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.readdatavalid, "AVALON_ram0_READDATAVALID");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.flush, "AVALON_ram0_FLUSH");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.burstcount, "AVALON_ram0_BURSTCOUNT");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.endofpacket, "AVALON_ram0_ENDOFPACKET");
  //sc_trace(my_trace_file, avalon_switch_wrapper_t0.data, "AVALON_ram0_DATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.irq, "AVALON_ram0_IRQ");
  sc_trace(my_trace_file, avalon_switch_wrapper_t0.irqnumber, "AVALON_ram0_IRQNUMBER");


  //============================== Traces vcittywrapper --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.reset, "AVALON_vcitty_RESET");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.resetrequest, "AVALON_vcitty_RESETREQUEST");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.clk, "AVALON_vcitty_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2. waitrequest, "AVALON_vcitty_WAITREQUEST");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.write, "AVALON_vcitty_WRITE");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.read, "AVALON_vcitty_READ");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.chipselect, "AVALON_vcitty_CHIPSELECT");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.address, "AVALON_vcitty_ADDRESS_1");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.writedata, "AVALON_vcitty_WRITEDATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.readdata, "AVALON_vcitty_READDATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.byteenable, "AVALON_vcitty_BYTEENABLE");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.readdatavalid, "AVALON_vcitty_READDATAVALID");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.flush, "AVALON_vcitty_FLUSH");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.burstcount, "AVALON_vcitty_BURSTCOUNT");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.endofpacket, "AVALON_vcitty_ENDOFPACKET");
  //sc_trace(my_trace_file, avalon_switch_wrapper_t2.data, "AVALON_vcitty_DATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.irq, "AVALON_vcitty_IRQ");
  sc_trace(my_trace_file, avalon_switch_wrapper_t2.irqnumber, "AVALON_vcitty_IRQNUMBER");


  //==============================Traces  vcitimerwrapper   --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.reset, "AVALON_vcitimer_RESET");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.resetrequest, "AVALON_vcitimer_RESETREQUEST");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.clk, "AVALON_vcitimer_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3. waitrequest, "AVALON_vcitimer_WAITREQUEST");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.write, "AVALON_vcitimer_WRITE");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.read, "AVALON_vcitimer_READ");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.chipselect, "AVALON_vcitimer_CHIPSELECT");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.address, "AVALON_vcitimer_ADDRESS_1");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.writedata, "AVALON_vcitimer_WRITEDATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.readdata, "AVALON_vcitimer_READDATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.byteenable, "AVALON_vcitimer_BYTEENABLE");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.readdatavalid, "AVALON_vcitimer_READDATAVALID");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.flush, "AVALON_vcitimer_FLUSH");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.burstcount, "AVALON_vcitimer_BURSTCOUNT");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.endofpacket, "AVALON_vcitimer_ENDOFPACKET");
  //sc_trace(my_trace_file, avalon_switch_wrapper_t3.data, "AVALON_vcitimer_DATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.irq, "AVALON_vcitimer_IRQ");
  sc_trace(my_trace_file, avalon_switch_wrapper_t3.irqnumber, "AVALON_vcitimer_IRQNUMBER");

  //==============================Traces vcifbwrapper  --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.reset, "AVALON_vcifb_RESET");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.resetrequest, "AVALON_vcifb_RESETREQUEST");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.clk, "AVALON_vcifb_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4. waitrequest, "AVALON_vcifb_WAITREQUEST");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.write, "AVALON_vcifb_WRITE");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.read, "AVALON_vcifb_READ");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.chipselect, "AVALON_vcifb_CHIPSELECT");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.address, "AVALON_vcifb_ADDRESS_1");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.writedata, "AVALON_vcifb_WRITEDATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.readdata, "AVALON_vcifb_READDATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.byteenable, "AVALON_vcifb_BYTEENABLE");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.readdatavalid, "AVALON_vcifb_READDATAVALID");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.flush, "AVALON_vcifb_FLUSH");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.burstcount, "AVALON_vcifb_BURSTCOUNT");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.endofpacket, "AVALON_vcifb_ENDOFPACKET");
  //sc_trace(my_trace_file, avalon_switch_wrapper_t4.data, "AVALON_vcifb_DATA");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.irq, "AVALON_vcifb_IRQ");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.irqnumber, "AVALON_vcifb_IRQNUMBER");

  //==============================Traces  VCICDMA
  sc_trace(my_trace_file, signal_vci_initiator_m0.cmd, "vci_dma_cmd");
  sc_trace(my_trace_file, signal_vci_initiator_m0.cmdval, "vci_dma_cmdval");
  sc_trace(my_trace_file, signal_vci_initiator_m0.eop, "vci_dma_eop");
  sc_trace(my_trace_file, signal_vci_initiator_m0.address, "vci_dma_address");
  sc_trace(my_trace_file, signal_vci_initiator_m0.be, "vci_dma_be");
  sc_trace(my_trace_file, signal_vci_initiator_m0.wdata, "vci_dma_wdata");
  sc_trace(my_trace_file, signal_vci_initiator_m0.cmdack, "vci_dma_cmdack");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rdata, "vci_dma_rdata");
  sc_trace(my_trace_file, signal_vci_initiator_m0.contig, "vci_dma_contig");
  sc_trace(my_trace_file, signal_vci_initiator_m0.wrap, "vci_dma_wrap");
  sc_trace(my_trace_file, signal_vci_initiator_m0.cons, "vci_dma_cons");
  sc_trace(my_trace_file, signal_vci_initiator_m0.plen, "vci_dma_plen");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rspack, "vci_dma_rspack");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rspval, "vci_dma_rspval");
  sc_trace(my_trace_file, signal_vci_initiator_m0.reop, "vci_dma_reop");
  sc_trace(my_trace_file, signal_vci_initiator_m0.clen, "vci_dma_clen");
  sc_trace(my_trace_file, signal_vci_initiator_m0.srcid, "vci_dma_srcid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.pktid, "vci_dma_pktid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.trdid, "vci_dma_trdid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rtrdid, "vci_dma_rtrdid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rsrcid, "vci_dma_rsrcid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rpktid, "vci_dma_rpktid");


  //==============================Traces  VCIRAM0
  sc_trace(my_trace_file, signal_vci_vciram0.cmd, "vciram0_cmd");
  sc_trace(my_trace_file, signal_vci_vciram0.cmdval, "vciram0_cmdval");
  sc_trace(my_trace_file, signal_vci_vciram0.eop, "vciram0_eop");
  sc_trace(my_trace_file, signal_vci_vciram0.address, "vciram0_address");
  sc_trace(my_trace_file, signal_vci_vciram0.be, "vciram0_be");
  sc_trace(my_trace_file, signal_vci_vciram0.wdata, "vciram0_wdata");
  sc_trace(my_trace_file, signal_vci_vciram0.cmdack, "vciram0_cmdack");
  sc_trace(my_trace_file, signal_vci_vciram0.rdata, "vciram0_rdata");
  sc_trace(my_trace_file, signal_vci_vciram0.contig, "vciram0_contig");
  sc_trace(my_trace_file, signal_vci_vciram0.wrap, "vciram0_wrap");
  sc_trace(my_trace_file, signal_vci_vciram0.cons, "vciram0_cons");
  sc_trace(my_trace_file, signal_vci_vciram0.plen, "vciram0_plen");
  sc_trace(my_trace_file, signal_vci_vciram0.rspack, "vciram0_rspack");
  sc_trace(my_trace_file, signal_vci_vciram0.rspval, "vciram0_rspval");
  sc_trace(my_trace_file, signal_vci_vciram0.reop, "vciram0_reop");
  sc_trace(my_trace_file, signal_vci_vciram0.clen, "vciram0_clen");
  sc_trace(my_trace_file, signal_vci_vciram0.srcid, "vciram0_srcid");
  sc_trace(my_trace_file, signal_vci_vciram0.pktid, "vciram0_pktid");
  sc_trace(my_trace_file, signal_vci_vciram0.trdid, "vciram0_trdid");
  sc_trace(my_trace_file, signal_vci_vciram0.rtrdid, "vciram0_rtrdid");
  sc_trace(my_trace_file, signal_vci_vciram0.rsrcid, "vciram0_rsrcid");
  sc_trace(my_trace_file, signal_vci_vciram0.rpktid, "vciram0_rpktid");


  //==============================Traces  VCITIMER
  sc_trace(my_trace_file, signal_vci_vcitimer.cmd, "vci_vcitimer_cmd");
  sc_trace(my_trace_file, signal_vci_vcitimer.cmdval, "vci_vcitimer_cmdval");
  sc_trace(my_trace_file, signal_vci_vcitimer.eop, "vci_vcitimer_eop");
  sc_trace(my_trace_file, signal_vci_vcitimer.address, "vci_vcitimer_address");
  sc_trace(my_trace_file, signal_vci_vcitimer.be, "vvci_vcitimer_be");
  sc_trace(my_trace_file, signal_vci_vcitimer.wdata, "vci_vcitimer_wdata");
  sc_trace(my_trace_file, signal_vci_vcitimer.cmdack, "vci_vcitimer_cmdack");
  sc_trace(my_trace_file, signal_vci_vcitimer.rdata, "vci_vcitimer_rdata");
  sc_trace(my_trace_file, signal_vci_vcitimer.contig, "vci_vcitimer_contig");
  sc_trace(my_trace_file, signal_vci_vcitimer.wrap, "vci_vcitimer_wrap");
  sc_trace(my_trace_file, signal_vci_vcitimer.cons, "vci_vcitimer_cons");
  sc_trace(my_trace_file, signal_vci_vcitimer.plen, "vci_vcitimer_plen");
  sc_trace(my_trace_file, signal_vci_vcitimer.rspack, "vci_vcitimer_rspack");
  sc_trace(my_trace_file, signal_vci_vcitimer.rspval, "vci_vcitimer_rspval");
  sc_trace(my_trace_file, signal_vci_vcitimer.reop, "vci_vcitimer_reop");
  sc_trace(my_trace_file, signal_vci_vcitimer.clen, "vci_vcitimer_clen");
  sc_trace(my_trace_file, signal_vci_vcitimer.srcid, "vci_vcitimer_srcid");
  sc_trace(my_trace_file, signal_vci_vcitimer.pktid, "vci_vcitimer_pktid");
  sc_trace(my_trace_file, signal_vci_vcitimer.trdid, "vci_vcitimer_trdid");
  sc_trace(my_trace_file, signal_vci_vcitimer.rtrdid, "vci_vcitimer_rtrdid");
  sc_trace(my_trace_file, signal_vci_vcitimer.rsrcid, "vci_vcitimer_rsrcid");
  sc_trace(my_trace_file, signal_vci_vcitimer.rpktid, "vci_vcitimer_rpktid");


  //==============================Traces  VCITTY
  sc_trace(my_trace_file, signal_vci_vcitty.cmd, "vci_vcitty_cmd");
  sc_trace(my_trace_file, signal_vci_vcitty.cmdval, "vci_vcitty_cmdval");
  sc_trace(my_trace_file, signal_vci_vcitty.eop, "vci_vcitty_eop");
  sc_trace(my_trace_file, signal_vci_vcitty.address, "vci_vcitty_address");
  sc_trace(my_trace_file, signal_vci_vcitty.be, "vci_vcitty_be");
  sc_trace(my_trace_file, signal_vci_vcitty.wdata, "vci_vcitty_wdata");
  sc_trace(my_trace_file, signal_vci_vcitty.cmdack, "vci_vcitty_cmdack");
  sc_trace(my_trace_file, signal_vci_vcitty.rdata, "vci_vcitty_rdata");
  sc_trace(my_trace_file, signal_vci_vcitty.contig, "vci_vcitty_contig");
  sc_trace(my_trace_file, signal_vci_vcitty.wrap, "vci_vcitty_wrap");
  sc_trace(my_trace_file, signal_vci_vcitty.cons, "vci_vcitty_cons");
  sc_trace(my_trace_file, signal_vci_vcitty.plen, "vci_vcitty_plen");
  sc_trace(my_trace_file, signal_vci_vcitty.rspack, "vci_vcitty_rspack");
  sc_trace(my_trace_file, signal_vci_vcitty.rspval, "vci_vcitty_rspval");
  sc_trace(my_trace_file, signal_vci_vcitty.reop, "vci_vcitty_reop");
  sc_trace(my_trace_file, signal_vci_vcitty.clen, "vci_vcitty_clen");
  sc_trace(my_trace_file, signal_vci_vcitty.srcid, "vci_vcitty_srcid");
  sc_trace(my_trace_file, signal_vci_vcitty.pktid, "vci_vcitty_pktid");
  sc_trace(my_trace_file, signal_vci_vcitty.trdid, "vci_vcitty_trdid");
  sc_trace(my_trace_file, signal_vci_vcitty.rtrdid, "vci_vcitty_rtrdid");
  sc_trace(my_trace_file, signal_vci_vcitty.rsrcid, "vci_vcitty_rsrcid");
  sc_trace(my_trace_file, signal_vci_vcitty.rpktid, "vci_vcitty_rpktid");


  //==============================Traces  VCIFB
  sc_trace(my_trace_file, signal_vci_vcifb.cmd, "vci_vcifb_cmd");
  sc_trace(my_trace_file, signal_vci_vcifb.cmdval, "vci_vcifb_cmdval");
  sc_trace(my_trace_file, signal_vci_vcifb.eop, "vci_vcifb_eop");
  sc_trace(my_trace_file, signal_vci_vcifb.address, "vci_vcifb_address");
  sc_trace(my_trace_file, signal_vci_vcifb.be, "vci_vcifb_be");
  sc_trace(my_trace_file, signal_vci_vcifb.wdata, "vci_vcifb_wdata");
  sc_trace(my_trace_file, signal_vci_vcifb.cmdack, "vci_vcifb_cmdack");
  sc_trace(my_trace_file, signal_vci_vcifb.rdata, "vci_vcifb_rdata");
  sc_trace(my_trace_file, signal_vci_vcifb.contig, "vci_vcifb_contig");
  sc_trace(my_trace_file, signal_vci_vcifb.wrap, "vci_vcifb_wrap");
  sc_trace(my_trace_file, signal_vci_vcifb.cons, "vci_vcifb_cons");
  sc_trace(my_trace_file, signal_vci_vcifb.plen, "vci_vcifb_plen");
  sc_trace(my_trace_file, signal_vci_vcifb.rspack, "vci_vcifb_rspack");
  sc_trace(my_trace_file, signal_vci_vcifb.rspval, "vci_vcifb_rspval");
  sc_trace(my_trace_file, signal_vci_vcifb.reop, "vci_vcifb_reop");
  sc_trace(my_trace_file, signal_vci_vcifb.clen, "vci_vcifb_clen");
  sc_trace(my_trace_file, signal_vci_vcifb.srcid, "vci_vcifb_srcid");
  sc_trace(my_trace_file, signal_vci_vcifb.pktid, "vci_vcifb_pktid");
  sc_trace(my_trace_file, signal_vci_vcifb.trdid, "vci_vcifb_trdid");
  sc_trace(my_trace_file, signal_vci_vcifb.rtrdid, "vci_vcifb_rtrdid");
  sc_trace(my_trace_file, signal_vci_vcifb.rsrcid, "vci_vcifb_rsrcid");
  sc_trace(my_trace_file, signal_vci_vcifb.rpktid, "vci_vcifb_rpktid");
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

