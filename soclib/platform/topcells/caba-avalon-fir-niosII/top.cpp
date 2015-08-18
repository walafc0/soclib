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
 * File : top.cpp
 * Date : 20/11/2008
 * Author :  Francois Charot
 * 
 * Copyright : IRISA
 *
 * This architecture contains:
 *  - 1 Avalon interconnect (1 initiators /4 targets)
 *  - 1 NIOS2 processor with VCI cache (Data/Instruction)
 *  - 1 VCI target VCI RAM
 *  - 1 VCI target TTY display
 *  - 1 VCI target TIMER
 * use of the ISSISS2 wrapper
 *********************************************************************
 */

#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "iss2_simhelper.h"
#include "niosII.h"
#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_timer.h"
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

#define SEGTYPEMASK 0x00300000

int SIMULATION_END = 0; 

int _main (int argc, char *argv[])
{

  using namespace sc_core;
  // Avoid repeating these everywhere
  using soclib::common::IntTab;
  using soclib::common::Segment;


  const int nb_master = 1;
  const int nb_slave = 3;


  // Define our VCI parameters
  typedef soclib::caba::VciParams<4,9,32,1,1,1,8,1,1,1> vci_param;
	
  // Define our AVALON parameters
  typedef soclib::caba::AvalonParams<32,32,8> avalon_param;
		
		
  // Mapping table
  soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

  maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
  maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
  maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));

  maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(0), true));

  maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(1), false));
  maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(2), false));

  ///////////////////////////////////////////////////////
  //	SIGNALS DECLARATION
  //////////////////////////////////////////////////////

  sc_clock		signal_clk("signal_clk");
  sc_signal<bool> signal_resetn("signal_resetn");

  sc_signal<bool> signal_nios2_it0("signal_nios2_it0"); 
  sc_signal<bool> signal_nios2_it1("signal_nios2_it1"); 
  sc_signal<bool> signal_nios2_it2("signal_nios2_it2"); 
  sc_signal<bool> signal_nios2_it3("signal_nios2_it3"); 
  sc_signal<bool> signal_nios2_it4("signal_nios2_it4"); 
  sc_signal<bool> signal_nios2_it5("signal_nios2_it5");

  soclib::caba::VciSignals<vci_param>	signal_vci_m0("signal_vci_m0");  
  soclib::caba::VciSignals<vci_param>	signal_vci_t0("signal_vci_t0");
  soclib::caba::VciSignals<vci_param>	signal_vci_t1("signal_vci_t1");

  sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 
  sc_signal<bool> signal_nios2_irq[32]; 
  sc_signal<bool> signal_nios2_irq00("signal_nios2_irq0");

  soclib::caba::VciSignals<vci_param>  vci_initiator_bus("vci_initiator_bus");

  //Address_Width,Data_Width,Burstcount_Width
	
  soclib::caba::AvalonSignals< avalon_param>  avalonbus1("avalonbus1");
  soclib::caba::AvalonSignals< avalon_param>  avalonbus2("avalonbus2");
  soclib::caba::AvalonSignals< avalon_param>  avalonbus3("avalonbus3");

  soclib::caba::AvalonSignals< avalon_param>  wrapper_switch("wrapper_switch");
	
	
  soclib::caba::VciSignals<vci_param>  vci_target_bus1("vci_target_bus1");
  soclib::caba::VciSignals<vci_param>  vci_target_bus2("vci_target_bus2");
  soclib::caba::VciSignals<vci_param>  vci_target_bus3("vci_target_bus3");


  /////////////////////////////////////////////////////////
  //	INSTANCIATED  COMPONENTS
  /////////////////////////////////////////////////////////

#ifdef USE_GDB_SERVER
#   warning Using a NIOS II
#   warning Using GDB
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::GdbServer<soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > > nios2("nios2", 0, maptab, IntTab(0), 2,128,16, 2,128,16);

#else
#   warning Using a NIOS II
  soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > nios2("nios2", 0, maptab,IntTab(0),   1,128,16, 1,128,16);
#endif

  soclib::common::Loader loader("soft/bin.soft");

  soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(1), maptab, "vcitty", NULL);

  soclib::caba::VciRam<vci_param> vciram0("vciram", IntTab(0), maptab, loader);

  soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(2), maptab, 1);

  soclib::caba::AvalonSwitchConfig<nb_master, nb_slave> config_switch;
  soclib::caba::AvalonSwitchFabric<nb_master, nb_slave, avalon_param>   SwitchFabric("SwitchFabric", config_switch);
  soclib::caba::VciAvalonInitiatorWrapper<vci_param, avalon_param> nios2_wapper("nios2_wapper");

  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> ram_wrapper("ram_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> tty_wrapper("tty_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> timer_wrapper("timer_wrapper");

  //////////////////////////////////////////////////////////
  //	Net-List
  //////////////////////////////////////////////////////////

  nios2.p_clk         	(signal_clk);  
  vcitty.p_clk        	(signal_clk);
  vciram0.p_clk  	(signal_clk);
  vcitimer.p_clk        (signal_clk);

  nios2.p_resetn        (signal_resetn);  
  vcitty.p_resetn       (signal_resetn);
  vciram0.p_resetn      (signal_resetn);
  vcitimer.p_resetn     (signal_resetn);

  nios2.p_irq[0]        (signal_nios2_irq00);
  for (int i = 1; i<32; i++)
    nios2.p_irq[i]      (signal_nios2_irq[i]);

  nios2.p_vci           (vci_initiator_bus);

  //connexion InitiatorWrapper & targetWrapper
  nios2_wapper.p_clk(signal_clk);
  nios2_wapper.p_resetn(signal_resetn);
  nios2_wapper.p_vci(vci_initiator_bus); 
  nios2_wapper.p_avalon(wrapper_switch);

  ram_wrapper.p_clk(signal_clk);
  ram_wrapper.p_resetn(signal_resetn);
  ram_wrapper.p_vci(vci_target_bus1); 
  ram_wrapper.p_avalon(avalonbus1);

  tty_wrapper.p_clk(signal_clk);
  tty_wrapper.p_resetn(signal_resetn);
  tty_wrapper.p_vci(vci_target_bus2); 
  tty_wrapper.p_avalon(avalonbus2);

  timer_wrapper.p_clk(signal_clk);
  timer_wrapper.p_resetn(signal_resetn);
  timer_wrapper.p_vci(vci_target_bus3); 
  timer_wrapper.p_avalon(avalonbus3);

  //connexion SwitchFabric
  SwitchFabric.p_clk(signal_clk);
  SwitchFabric.p_resetn(signal_resetn);
  SwitchFabric.p_avalon_master[0](wrapper_switch);

  SwitchFabric.p_avalon_slave[0](avalonbus1);
  SwitchFabric.p_avalon_slave[1](avalonbus2);
  SwitchFabric.p_avalon_slave[2](avalonbus3);

  vciram0.p_vci      (vci_target_bus1);
  vcitty.p_vci       (vci_target_bus2);
  vcitimer.p_vci     (vci_target_bus3);

  vcitty.p_irq[0]    (signal_tty_irq0); 

  vcitimer.p_irq[0]  (signal_nios2_irq00);

  int ncycles;
  clock_t starttime, endtime;

  //////////////////////////////////////////////////////////
  //	Traces
  //////////////////////////////////////////////////////////

#if TRACE_FILE 
  sc_trace_file *my_trace_file;
  my_trace_file = sc_create_vcd_trace_file ("system_trace");

  sc_trace(my_trace_file, SwitchFabric.to_mux[0]->sel_slave, "MUX_sel_slave");
  sc_trace(my_trace_file, SwitchFabric.to_mux[0]->p_mux_sel_master[0], "MUX_sel_master_0");
  sc_trace(my_trace_file, SwitchFabric.to_mux[0]->p_mux_sel_master[1]," MUX_sel_master_1");
  sc_trace(my_trace_file, SwitchFabric.to_mux[0]->p_mux_master[0].waitrequest, "MUX_master_waitrequest");
  sc_trace(my_trace_file, SwitchFabric.to_mux[0]->p_mux_slave[0].waitrequest, "MUX_slave_0_waitrequest");
  sc_trace(my_trace_file, SwitchFabric.to_mux[0]->p_mux_slave[1].waitrequest, "MUX_slave_1_waitrequest");


  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->r_fsm_state, "Arbiter_0_r_fsm_state");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->req_courante, "Arbiter_0_req_courante");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->requete[0], "Arbiter_0_requete[0]");


  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->r_fsm_state, "Arbiter_1_r_fsm_state");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->req_courante, "Arbiter_1_req_courante");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->requete[0], "Arbiter_1_requete[0]");

  sc_trace(my_trace_file, ram_wrapper.r_fsm_state, "ram_wrapper_fsm");
  sc_trace(my_trace_file, ram_wrapper.r_burstcount, "ram_wrapper_r_burstcount");
  sc_trace(my_trace_file, ram_wrapper.r_byteenable, "ram_wrapper.r_byteenable");
	
  sc_trace(my_trace_file, tty_wrapper.r_fsm_state, "tty_wrapper_fsm");
  sc_trace(my_trace_file, tty_wrapper.r_burstcount, "tty_wrapper_r_burstcount");
	
  sc_trace(my_trace_file, nios2_wapper.r_fsm_state, "nios2_wapper_fsm");
  sc_trace(my_trace_file, nios2_wapper.r_read_burst_count, "nios2_wapper_r_read_burst_count");	
  sc_trace(my_trace_file, nios2_wapper.r_read_burstcount, "nios2_wapper_r_read_burstcount");	
  sc_trace(my_trace_file, nios2_wapper.r_cmd, "nios2_wapper_r_cmd");	
	
  sc_trace(my_trace_file, signal_clk, "CLK");
  sc_trace(my_trace_file, signal_resetn, "RESETN");

  sc_trace(my_trace_file, signal_nios2_icache.req, "ICACHE.REQ");
  //sc_trace(my_trace_file, signal_nios2_icache.type, "ICACHE.TYPE");
  sc_trace(my_trace_file, signal_nios2_icache.adr, "ICACHE.ADR");
  sc_trace(my_trace_file, signal_nios2_icache.frz, "ICACHE.FRZ");
  sc_trace(my_trace_file, signal_nios2_icache.ins, "ICACHE.INS");
  sc_trace(my_trace_file, signal_nios2_icache.berr, "ICACHE.BERR");

  sc_trace(my_trace_file, signal_nios2_dcache.req, "DCACHE.REQ");
  sc_trace(my_trace_file, signal_nios2_dcache.type, "DCACHE.TYPE");
  sc_trace(my_trace_file, signal_nios2_dcache.adr, "DCACHE.ADR");
  sc_trace(my_trace_file, signal_nios2_dcache.frz, "DCACHE.FRZ");
  sc_trace(my_trace_file, signal_nios2_dcache.wdata, "DCACHE.WDATA");
  sc_trace(my_trace_file, signal_nios2_dcache.rdata, "DCACHE.RDATA");
  sc_trace(my_trace_file, signal_nios2_dcache.berr, "DCACHE.BERR");

  sc_trace(my_trace_file, vci_target_bus1.rspack, "TARGET.RSPACK");
  sc_trace(my_trace_file, vci_target_bus1.rspval, "TARGET.RSPVAL");
  sc_trace(my_trace_file, vci_target_bus1.rdata, "TARGET.RDATA");
  sc_trace(my_trace_file, vci_target_bus1.reop, "TARGET.REOP");
  sc_trace(my_trace_file, vci_target_bus1.rerror, "TARGET.RERROR");
  sc_trace(my_trace_file, vci_target_bus1.rsrcid, "TARGET.RSRCID");
  sc_trace(my_trace_file, vci_target_bus1.rtrdid, "TARGET.RTRDID");
  sc_trace(my_trace_file, vci_target_bus1.rpktid, "TARGET.RPKTID");

  sc_trace(my_trace_file, vci_target_bus1.cmdack, "TARGET.CMDACK");
  sc_trace(my_trace_file, vci_target_bus1.cmdval, "TARGET.CMDVAL");
  sc_trace(my_trace_file, vci_target_bus1.address, "TARGET.ADDRESS");
  sc_trace(my_trace_file, vci_target_bus1.be, "TARGET.BE");
  sc_trace(my_trace_file, vci_target_bus1.cmd, "TARGET.CMD");
  sc_trace(my_trace_file, vci_target_bus1.contig, "TARGET.CONTIG");
  sc_trace(my_trace_file, vci_target_bus1.wdata, "TARGET.WDATA");
  sc_trace(my_trace_file, vci_target_bus1.eop, "TARGET.EOP");
  sc_trace(my_trace_file, vci_target_bus1.cons, "TARGET.CONS");
  sc_trace(my_trace_file, vci_target_bus1.plen, "TARGET.PLEN");
  sc_trace(my_trace_file, vci_target_bus1.wrap, "TARGET.WRAP");
  sc_trace(my_trace_file, vci_target_bus1.cfixed, "TARGET.CFIXED");
  sc_trace(my_trace_file, vci_target_bus1.clen, "TARGET.CLEN");
  sc_trace(my_trace_file, vci_target_bus1.srcid, "TARGET.SRCID");
  sc_trace(my_trace_file, vci_target_bus1.trdid, "TARGET.TRDID");
  sc_trace(my_trace_file, vci_target_bus1.pktid, "TARGET.PKTID");


  //  VCITTY
  sc_trace(my_trace_file, vci_target_bus2.rspack, "VCITTY.RSPACK");
  sc_trace(my_trace_file, vci_target_bus2.rspval, "VCITTY.RSPVAL");
  sc_trace(my_trace_file, vci_target_bus2.rdata, "VCITTY.RDATA");
  sc_trace(my_trace_file, vci_target_bus2.reop, "VCITTY.REOP");
  sc_trace(my_trace_file, vci_target_bus2.rerror, "VCITTY.RERROR");
  sc_trace(my_trace_file, vci_target_bus2.rsrcid, "VCITTY.RSRCID");
  sc_trace(my_trace_file, vci_target_bus2.rtrdid, "VCITTY.RTRDID");
  sc_trace(my_trace_file, vci_target_bus2.rpktid, "VCITTY.RPKTID");

  sc_trace(my_trace_file, vci_target_bus2.cmdack, "VCITTY.CMDACK");
  sc_trace(my_trace_file, vci_target_bus2.cmdval, "VCITTY.CMDVAL");
  sc_trace(my_trace_file, vci_target_bus2.address, "VCITTY.ADDRESS");
  sc_trace(my_trace_file, vci_target_bus2.be, "VCITTY.BE");
  sc_trace(my_trace_file, vci_target_bus2.cmd, "VCITTY.CMD");
  sc_trace(my_trace_file, vci_target_bus2.contig, "VCITTY.CONTIG");
  sc_trace(my_trace_file, vci_target_bus2.wdata, "VCITTY.WDATA");
  sc_trace(my_trace_file, vci_target_bus2.eop, "VCITTY.EOP");
  sc_trace(my_trace_file, vci_target_bus2.cons, "VCITTY.CONS");
  sc_trace(my_trace_file, vci_target_bus2.plen, "VCITTY.PLEN");
  sc_trace(my_trace_file, vci_target_bus2.wrap, "VCITTY.WRAP");
  sc_trace(my_trace_file, vci_target_bus2.cfixed, "VCITTY.CFIXED");
  sc_trace(my_trace_file, vci_target_bus2.clen, "VCITTY.CLEN");
  sc_trace(my_trace_file, vci_target_bus2.srcid, "VCITTY.SRCID");
  sc_trace(my_trace_file, vci_target_bus2.trdid, "VCITTY.TRDID");
  sc_trace(my_trace_file, vci_target_bus2.pktid, "VCITTY.PKTID");

  //==============================Traces  VCI VciAvalonInitWrapper  
  sc_trace(my_trace_file, signal_resetn, "VCI_INIT_resetn");
  sc_trace(my_trace_file, signal_clk, "VCI_INIT_CLK");
  sc_trace(my_trace_file, vci_initiator_bus.cmd, "VCI_INIT_cmd");
  sc_trace(my_trace_file, vci_initiator_bus.cmdval, "VCI_INIT_cmdval");
  sc_trace(my_trace_file, vci_initiator_bus.eop, "VCI_INIT_eop");
  sc_trace(my_trace_file, vci_initiator_bus.address, "VCI_INIT_address");
  sc_trace(my_trace_file, vci_initiator_bus.be, "VCI_INIT_be");
  sc_trace(my_trace_file, vci_initiator_bus.wdata, "VCI_INIT_wdata");

  sc_trace(my_trace_file, vci_initiator_bus.cmdack, "VCI_INIT_cmdack");
  sc_trace(my_trace_file, vci_initiator_bus.rdata, "VCI_INIT_rdata");
  sc_trace(my_trace_file, vci_initiator_bus.contig, "VCI_INIT_contig");
  sc_trace(my_trace_file, vci_initiator_bus.wrap, "VCI_INIT_wrap");
  sc_trace(my_trace_file, vci_initiator_bus.cons, "VCI_INIT_cons");
  sc_trace(my_trace_file, vci_initiator_bus.plen, "VCI_INIT_plen");
  sc_trace(my_trace_file, vci_initiator_bus.rspack, "VCI_INIT_rspack");
  sc_trace(my_trace_file, vci_initiator_bus.rspval, "VCI_INIT_rspval");
  sc_trace(my_trace_file, vci_initiator_bus.reop, "VCI_INIT_reop");

  sc_trace(my_trace_file, vci_initiator_bus.rerror, "VCI_INIT_rerror");
  sc_trace(my_trace_file, vci_initiator_bus.rsrcid, "VCI_INIT_rsrcid");
  sc_trace(my_trace_file, vci_initiator_bus.rtrdid, "VCI_INIT_rtrdid");
  sc_trace(my_trace_file, vci_initiator_bus.rpktid, "VCI_INIT_rpktid");

  sc_trace(my_trace_file, vci_initiator_bus.clen, "VCI_INIT_clen");
  sc_trace(my_trace_file, vci_initiator_bus.srcid, "VCI_INIT_srcid");
  sc_trace(my_trace_file, vci_initiator_bus.pktid, "VCI_INIT_pktid");
  sc_trace(my_trace_file, vci_initiator_bus.trdid, "VCI_INIT_trdid");

  //==============================Traces VciAvalonInitWrapper  --  AvalonSwitchFabric

  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].reset, "AVALON_MASTER_RESET");                  // IN
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].waitrequest, "AVALON_MASTER_WAITREQUEST");     // IN
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].write , "AVALON_MASTER_WRITE");
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].read , "AVALON_MASTER_READ");                   // OUT                 // OUT
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].address , "AVALON_MASTER_ADDRESS");             // OUT
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].writedata , "AVALON_MASTER_WRITEDATA");         // OUT
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].readdata, "AVALON_MASTER_READDATA");            // IN
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].byteenable, "AVALON_MASTER_BYTEENABLE");        // OUT
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].readdatavalid, "AVALON_MASTER_READDATAVALID");  // IN
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].flush , "AVALON_MASTER_FLUSH");                 // OUT
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].burstcount , "AVALON_MASTER_BURSTCOUNT") ;      // OUT
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].endofpacket, "AVALON_MASTER_ENDOFPACKET");      // IN
  //sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].data, "AVALON_MASTER_DATA");                    // INOUT
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].irq, "AVALON_MASTER_IRQ");                      // IN
  sc_trace(my_trace_file, SwitchFabric.p_avalon_master[0].irqnumber, "AVALON_MASTER_IRQNUMBER");          // IN


  //==========================Traces AvalonSwitchFabric -- VciAvalonTargetWrapper

  sc_trace(my_trace_file, avalonbus1. reset, "AVALON_SLAVE1_RESET");                 // IN
  sc_trace(my_trace_file, avalonbus1. resetrequest, "AVALON_SLAVE1_RESETREQUEST");   // OUT
  sc_trace(my_trace_file, avalonbus1.clk, "AVALON_SLAVE1_CLK");
  sc_trace(my_trace_file, avalonbus1. waitrequest, "AVALON_SLAVE1_WAITREQUEST");     // IN
  sc_trace(my_trace_file, avalonbus1.write, "AVALON_SLAVE1_WRITE");                  // OUT
  sc_trace(my_trace_file, avalonbus1.read, "AVALON_SLAVE1_READ");                    // OUT
  sc_trace(my_trace_file, avalonbus1.chipselect, "AVALON_SLAVE1_CHIPSELECT");        // OUT
  sc_trace(my_trace_file, avalonbus1.address, "AVALON_SLAVE1_ADDRESS_1");            // OUT
  sc_trace(my_trace_file, avalonbus1.writedata, "AVALON_SLAVE1_WRITEDATA");          // OUT
  sc_trace(my_trace_file, avalonbus1.readdata, "AVALON_SLAVE1_READDATA");            // IN
  sc_trace(my_trace_file, avalonbus1.byteenable, "AVALON_SLAVE1_BYTEENABLE");        // OUT
  sc_trace(my_trace_file, avalonbus1. readdatavalid, "AVALON_SLAVE1_READDATAVALID"); // IN
  sc_trace(my_trace_file, avalonbus1. flush, "AVALON_SLAVE1_FLUSH");                 // OUT
  sc_trace(my_trace_file, avalonbus1.  burstcount, "AVALON_SLAVE1_BURSTCOUNT");      // OUT
  sc_trace(my_trace_file, avalonbus1. endofpacket, "AVALON_SLAVE1_ENDOFPACKET");     // IN
  //sc_trace(my_trace_file, avalonbus1. data, "AVALON_SLAVE_DATA");                   // INOUT
  sc_trace(my_trace_file, avalonbus1. irq, "AVALON_SLAVE1_IRQ");                     // IN
  sc_trace(my_trace_file, avalonbus1. irqnumber, "AVALON_SLAVE1_IRQNUMBER");         // IN

  //==========================Traces AvalonSwitchFabric -- VciAvalonTargetWrapper

  sc_trace(my_trace_file, avalonbus2. reset, "AVALON_SLAVE2_RESET");                 // IN
  sc_trace(my_trace_file, avalonbus2. resetrequest, "AVALON_SLAVE2_RESETREQUEST");   // OUT
  sc_trace(my_trace_file, avalonbus2.clk, "AVALON_SLAVE2_CLK");
  sc_trace(my_trace_file, avalonbus2. waitrequest, "AVALON_SLAVE2_WAITREQUEST");     // IN
  sc_trace(my_trace_file, avalonbus2.write, "AVALON_SLAVE2_WRITE");                  // OUT
  sc_trace(my_trace_file, avalonbus2.read, "AVALON_SLAVE2_READ");                    // OUT
  sc_trace(my_trace_file, avalonbus2.chipselect, "AVALON_SLAVE2_CHIPSELECT");        // OUT
  sc_trace(my_trace_file, avalonbus2.address, "AVALON_SLAVE2_ADDRESS_1");            // OUT
  sc_trace(my_trace_file, avalonbus2.writedata, "AVALON_SLAVE2_WRITEDATA");          // OUT
  sc_trace(my_trace_file, avalonbus2.readdata, "AVALON_SLAVE2_READDATA");            // IN
  sc_trace(my_trace_file, avalonbus2.byteenable, "AVALON_SLAVE2_BYTEENABLE");        // OUT
  sc_trace(my_trace_file, avalonbus2. readdatavalid, "AVALON_SLAVE2_READDATAVALID"); // IN
  sc_trace(my_trace_file, avalonbus2. flush, "AVALON_SLAVE2_FLUSH");                 // OUT
  sc_trace(my_trace_file, avalonbus2.  burstcount, "AVALON_SLAVE2_BURSTCOUNT");      // OUT
  sc_trace(my_trace_file, avalonbus2. endofpacket, "AVALON_SLAVE2_ENDOFPACKET");     // IN
  //sc_trace(my_trace_file, avalonbus1. data, "AVALON_SLAVE_DATA");                   // INOUT
  sc_trace(my_trace_file, avalonbus2. irq, "AVALON_SLAVE2_IRQ");                     // IN
  sc_trace(my_trace_file, avalonbus2. irqnumber, "AVALON_SLAVE2_IRQNUMBER");         // IN

#endif

  // Starting execution timing
  starttime = clock ();

  //#ifndef SOCVIEW
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

    //std::cout << std::endl << "ncycles " << i<< std::endl;

    if((i % 10000) == 0)  
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
	
	

#if TRACE_FILE
  sc_close_vcd_trace_file (my_trace_file);
#endif
#if WIF_FILE
  sc_close_wif_trace_file (my_trace_file);
#endif


  char buf[1];

  std::cin.getline(buf,2);
  return EXIT_SUCCESS;
  //#else
  ncycles = 1;
  sc_start(sc_core::sc_time(0, SC_NS));
  signal_resetn = false;
  sc_start(sc_core::sc_time(1, SC_NS));
  signal_resetn = true;

  //	debug();
  return EXIT_SUCCESS;
  //#endif



}

int sc_main (int argc, char *argv[])
{
  try {
    return _main(argc, argv);
  } catch (soclib::exception::Exception &e) {
    std::cout << e << std::endl;
  } catch (...) {
    std::cout << "Unknown exception occurred" << std::endl;
  }
  return 1;
}
