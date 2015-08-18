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
 * Date : 20/11/2008
 * Author :  Francois Charot
 *           Charles Wagner
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
#include "avalon_switch_config.h"
#include "avalon_switch_fabric.h"
#include "vci_avalon_initiator_wrapper.h"
#include "vci_avalon_target_wrapper.h"

#define USE_GDB_SERVER

#ifdef USE_GDB_SERVER
#include "gdbserver.h"
#endif

#define TRACE_FILE 0

#include "segmentation.h"

#define SEGTYPEMASK 0x00300000

int _main(int argc, char *argv[]) {
  using namespace sc_core;
  // Avoid repeating these everywhere
  using soclib::common::IntTab;
  using soclib::common::Segment;

  const int nb_master = 4;
  const int nb_slave = 5;

  // Define our VCI parameters
  typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

  // Define our AVALON parameters
  typedef soclib::caba::AvalonParams<32,32,8> avalon_param;

  // Mapping table
  soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

  maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
  maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
  maptab.add(Segment("text", TEXT_BASE, TEXT_SIZE, IntTab(0), true));

  maptab.add(Segment("data", DATA_BASE, DATA_SIZE, IntTab(1), true));


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

  soclib::caba::VciSignals<vci_param>  signal_vci_initiator_m0("signal_vci_initiator_m0");
  soclib::caba::VciSignals<vci_param>  signal_vci_initiator_m1("signal_vci_initiator_m1");
  soclib::caba::VciSignals<vci_param>  signal_vci_initiator_m2("signal_vci_initiator_m2");
  soclib::caba::VciSignals<vci_param>  signal_vci_initiator_m3("signal_vci_initiator_m3");

  soclib::caba::VciSignals<vci_param> signal_vci_vcitty("signal_vci_vcitty");
  soclib::caba::VciSignals<vci_param> signal_vci_vciram0("signal_vci_vciram0");
  soclib::caba::VciSignals<vci_param> signal_vci_vciram1("signal_vci_vciram1");
  soclib::caba::VciSignals<vci_param> signal_vci_vcitimer("signal_vci_vcitimer");
  soclib::caba::VciSignals<vci_param> signal_vci_vcilocks("signal_vci_vcilocks");

  sc_signal<bool> signal_tty_irq0("signal_tty_irq0");
  sc_signal<bool> signal_tty_irq1("signal_tty_irq1");
  sc_signal<bool> signal_tty_irq2("signal_tty_irq2");
  sc_signal<bool> signal_tty_irq3("signal_tty_irq3");

  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_m0("avalon_switch_wrapper_m0");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_m1("avalon_switch_wrapper_m1");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_m2("avalon_switch_wrapper_m2");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_m3("avalon_switch_wrapper_m3");

  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t0("avalon_switch_wrapper_t0");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t1("avalon_switch_wrapper_t1");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t2("avalon_switch_wrapper_t2");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t3("avalon_switch_wrapper_t3");
  soclib::caba::AvalonSignals<avalon_param>  avalon_switch_wrapper_t4("avalon_switch_wrapper_t4");

  // Components

#ifdef USE_GDB_SERVER
  typedef soclib::common::GdbServer<soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> > iss_t;
#else
  typedef soclib::common::Iss2Simhelper<soclib::common::Nios2fIss> iss_t;
#endif

  soclib::caba::VciXcacheWrapper<vci_param, iss_t> nios20("nios2_0", 0, maptab, IntTab(0), 1, 8, 4, 1, 8, 4);
  soclib::caba::VciXcacheWrapper<vci_param, iss_t> nios21("nios2_1", 1, maptab, IntTab(1), 1, 8, 4, 1, 8, 4);
  soclib::caba::VciXcacheWrapper<vci_param, iss_t> nios22("nios2_2", 2, maptab, IntTab(2), 1, 8, 4, 1, 8, 4);
  soclib::caba::VciXcacheWrapper<vci_param, iss_t> nios23("nios2_3", 3, maptab, IntTab(3), 1, 8, 4, 1, 8, 4);

  soclib::common::Loader loader("soft/bin.soft");
  soclib::caba::VciRam<vci_param> vciram0("vciram0", IntTab(0), maptab, loader);
  soclib::caba::VciRam<vci_param> vciram1("vciram1", IntTab(1), maptab, loader);
  soclib::caba::VciMultiTty<vci_param> vcitty("vcitty", IntTab(2), maptab, "vcitty0", "vcitty1", "vcitty2", "vcitty3", NULL);
  soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(3), maptab, 4);
  soclib::caba::VciLocks<vci_param> vcilocks("vcilocks", IntTab(4), maptab);

  soclib::caba::AvalonSwitchConfig<nb_master, nb_slave> config_switch;
  soclib::caba::AvalonSwitchFabric<nb_master, nb_slave, avalon_param>   SwitchFabric("SwitchFabric", config_switch);
  soclib::caba::VciAvalonInitiatorWrapper<vci_param, avalon_param> cache0_wrapper("cache0_wrapper");
  soclib::caba::VciAvalonInitiatorWrapper<vci_param, avalon_param> cache1_wrapper("cache1_wrapper");
  soclib::caba::VciAvalonInitiatorWrapper<vci_param, avalon_param> cache2_wrapper("cache2_wrapper");
  soclib::caba::VciAvalonInitiatorWrapper<vci_param, avalon_param> cache3_wrapper("cache3_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> ram0_wrapper("ram0_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> ram1_wrapper("ram1_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> tty_wrapper("tty_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> timer_wrapper("timer_wrapper");
  soclib::caba::VciAvalonTargetWrapper<vci_param, avalon_param> locks_wrapper("locks_wrapper");

  //	std::cout << "Before binding ..." << std::endl;

  //	Net-List
  nios20.p_clk(signal_clk);
  nios21.p_clk(signal_clk);
  nios22.p_clk(signal_clk);
  nios23.p_clk(signal_clk);
  vciram0.p_clk(signal_clk);
  vciram1.p_clk(signal_clk);
  vcilocks.p_clk(signal_clk);
  vcitimer.p_clk(signal_clk);
  vcitty.p_clk(signal_clk);

  nios20.p_resetn(signal_resetn);
  nios21.p_resetn(signal_resetn);
  nios22.p_resetn(signal_resetn);
  nios23.p_resetn(signal_resetn);
  vciram0.p_resetn(signal_resetn);
  vciram1.p_resetn(signal_resetn);
  vcilocks.p_resetn(signal_resetn);
  vcitimer.p_resetn(signal_resetn);
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

  nios20.p_vci(signal_vci_initiator_m0);

  cache0_wrapper.p_clk(signal_clk);
  cache0_wrapper.p_resetn(signal_resetn);
  cache0_wrapper.p_vci(signal_vci_initiator_m0); 
  cache0_wrapper.p_avalon(avalon_switch_wrapper_m0);

  nios21.p_vci(signal_vci_initiator_m1);

  cache1_wrapper.p_clk(signal_clk);
  cache1_wrapper.p_resetn(signal_resetn);
  cache1_wrapper.p_vci(signal_vci_initiator_m1); 
  cache1_wrapper.p_avalon(avalon_switch_wrapper_m1);

  nios22.p_vci(signal_vci_initiator_m2);

  cache2_wrapper.p_clk(signal_clk);
  cache2_wrapper.p_resetn(signal_resetn);
  cache2_wrapper.p_vci(signal_vci_initiator_m2); 
  cache2_wrapper.p_avalon(avalon_switch_wrapper_m2);

  nios23.p_vci(signal_vci_initiator_m3);

  cache3_wrapper.p_clk(signal_clk);
  cache3_wrapper.p_resetn(signal_resetn);
  cache3_wrapper.p_vci(signal_vci_initiator_m3); 
  cache3_wrapper.p_avalon(avalon_switch_wrapper_m3);

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
  vcitty.p_irq[1](signal_tty_irq1);
  vcitty.p_irq[2](signal_tty_irq2);
  vcitty.p_irq[3](signal_tty_irq3);

  tty_wrapper.p_clk(signal_clk);
  tty_wrapper.p_resetn(signal_resetn);
  tty_wrapper.p_vci(signal_vci_vcitty); 
  tty_wrapper.p_avalon(avalon_switch_wrapper_t2);

  vcitimer.p_vci(signal_vci_vcitimer);
  vcitimer.p_irq[0](signal_nios2_irq00);
  vcitimer.p_irq[1](signal_nios2_irq10);
  vcitimer.p_irq[2](signal_nios2_irq20);
  vcitimer.p_irq[3](signal_nios2_irq30);

  timer_wrapper.p_clk(signal_clk);
  timer_wrapper.p_resetn(signal_resetn);
  timer_wrapper.p_vci(signal_vci_vcitimer); 
  timer_wrapper.p_avalon(avalon_switch_wrapper_t3);

  vcilocks.p_vci(signal_vci_vcilocks);

  locks_wrapper.p_clk(signal_clk);
  locks_wrapper.p_resetn(signal_resetn);
  locks_wrapper.p_vci(signal_vci_vcilocks); 
  locks_wrapper.p_avalon(avalon_switch_wrapper_t4);

  SwitchFabric.p_clk(signal_clk);
  SwitchFabric.p_resetn(signal_resetn);
  SwitchFabric.p_avalon_master[0](avalon_switch_wrapper_m0);
  SwitchFabric.p_avalon_master[1](avalon_switch_wrapper_m1);
  SwitchFabric.p_avalon_master[2](avalon_switch_wrapper_m2);
  SwitchFabric.p_avalon_master[3](avalon_switch_wrapper_m3);

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
  //==============================Traces ram0wrapper  --  AvalonSwitchFabric

  sc_trace(my_trace_file, ram0_wrapper.r_address,   "ram0_wrapper_r_address"); 
  sc_trace(my_trace_file, ram0_wrapper.r_fsm_state, "ram0_wrapper_r_fsm_state"); 

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

  //==============================Traces ram1wrapper  --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.reset, "AVALON_ram1_RESET");                
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.resetrequest, "AVALON_ram1_RESETREQUEST");   
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.clk, "AVALON_ram1_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_t1. waitrequest, "AVALON_ram1_WAITREQUEST");    
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.write, "AVALON_ram1_WRITE");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.read, "AVALON_ram1_READ");                    
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.chipselect, "AVALON_ram1_CHIPSELECT");        
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.address, "AVALON_ram1_ADDRESS_1");            
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.writedata, "AVALON_ram1_WRITEDATA");          
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.readdata, "AVALON_ram1_READDATA");            
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.byteenable, "AVALON_ram1_BYTEENABLE");        
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.readdatavalid, "AVALON_ram1_READDATAVALID"); 
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.flush, "AVALON_ram1_FLUSH");                 
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.burstcount, "AVALON_ram1_BURSTCOUNT");      
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.endofpacket, "AVALON_ram1_ENDOFPACKET");     
  //sc_trace(my_trace_file, avalon_switch_wrapper_t1.data, "AVALON_ram1_DATA");                   
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.irq, "AVALON_ram1_IRQ");                    
  sc_trace(my_trace_file, avalon_switch_wrapper_t1.irqnumber, "AVALON_ram1_IRQNUMBER");         

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


  //==============================Traces vcilockswrapper  --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.reset, "AVALON_vcilocks_RESET");                
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.resetrequest, "AVALON_vcilocks_RESETREQUEST");   
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.clk, "AVALON_vcilocks_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4. waitrequest, "AVALON_vcilocks_WAITREQUEST");     
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.write, "AVALON_vcilocks_WRITE");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.read, "AVALON_vcilocks_READ");                    
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.chipselect, "AVALON_vcilocks_CHIPSELECT");       
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.address, "AVALON_vcilocks_ADDRESS_1");            
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.writedata, "AVALON_vcilocks_WRITEDATA");          
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.readdata, "AVALON_vcilocks_READDATA");            
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.byteenable, "AVALON_vcilocks_BYTEENABLE");        
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.readdatavalid, "AVALON_vcilocks_READDATAVALID");
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.flush, "AVALON_vcilocks_FLUSH");                 
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.burstcount, "AVALON_vcilocks_BURSTCOUNT");      
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.endofpacket, "AVALON_vcilocks_ENDOFPACKET");     
  //sc_trace(my_trace_file, avalon_switch_wrapper_t4.data, "AVALON_vcilocks_DATA");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.irq, "AVALON_vcilocks_IRQ");                     
  sc_trace(my_trace_file, avalon_switch_wrapper_t4.irqnumber, "AVALON_vcilocks_IRQNUMBER");         





  //==============================Traces avalon_nios20_wrapper   --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.reset, "AVALON_nios20_RESET");                
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.resetrequest, "AVALON_nios20_RESETREQUEST");   
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.clk, "AVALON_nios20_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_m0. waitrequest, "AVALON_nios20_WAITREQUEST");     
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.write, "AVALON_nios20_WRITE");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.read, "AVALON_nios20_READ");                   
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.address, "AVALON_nios20_ADDRESS");            
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.writedata, "AVALON_nios20_WRITEDATA");          
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.readdata, "AVALON_nios20_READDATA");            
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.byteenable, "AVALON_nios20_BYTEENABLE");       
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.readdatavalid, "AVALON_nios20_READDATAVALID"); 
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.flush, "AVALON_nios20_FLUSH");                 
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.burstcount, "AVALON_nios20_BURSTCOUNT");      
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.endofpacket, "AVALON_nios20_ENDOFPACKET");     
  //sc_trace(my_trace_file, avalon_switch_wrapper_m0.data, "AVALON_nios20_DATA");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.irq, "AVALON_nios20_IRQ");                     
  sc_trace(my_trace_file, avalon_switch_wrapper_m0.irqnumber, "AVALON_nios20_IRQNUMBER"); 


  //==============================Traces avalon_nios21_wrapper   --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.reset, "AVALON_nios21_RESET");                
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.resetrequest, "AVALON_nios21_RESETREQUEST");   
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.clk, "AVALON_nios21_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_m1. waitrequest, "AVALON_nios21_WAITREQUEST");     
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.write, "AVALON_nios21_WRITE");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.read, "AVALON_nios21_READ");                   
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.address, "AVALON_nios21_ADDRESS");            
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.writedata, "AVALON_nios21_WRITEDATA");          
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.readdata, "AVALON_nios21_READDATA");            
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.byteenable, "AVALON_nios21_BYTEENABLE");       
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.readdatavalid, "AVALON_nios21_READDATAVALID"); 
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.flush, "AVALON_nios21_FLUSH");                 
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.burstcount, "AVALON_nios21_BURSTCOUNT");      
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.endofpacket, "AVALON_nios21_ENDOFPACKET");     
  //sc_trace(my_trace_file, avalon_switch_wrapper_m1.data, "AVALON_nios21_DATA");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.irq, "AVALON_nios21_IRQ");                     
  sc_trace(my_trace_file, avalon_switch_wrapper_m1.irqnumber, "AVALON_nios21_IRQNUMBER");           


  //==============================Traces avalon_nios22_wrapper   --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.reset, "AVALON_nios22_RESET");                
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.resetrequest, "AVALON_nios22_RESETREQUEST");   
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.clk, "AVALON_nios22_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_m2. waitrequest, "AVALON_nios22_WAITREQUEST");     
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.write, "AVALON_nios22_WRITE");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.read, "AVALON_nios22_READ");                   
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.address, "AVALON_nios22_ADDRESS");            
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.writedata, "AVALON_nios22_WRITEDATA");          
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.readdata, "AVALON_nios22_READDATA");            
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.byteenable, "AVALON_nios22_BYTEENABLE");       
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.readdatavalid, "AVALON_nios22_READDATAVALID"); 
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.flush, "AVALON_nios22_FLUSH");                 
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.burstcount, "AVALON_nios22_BURSTCOUNT");      
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.endofpacket, "AVALON_nios22_ENDOFPACKET");     
  //sc_trace(my_trace_file, avalon_switch_wrapper_m2.data, "AVALON_nios22_DATA");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.irq, "AVALON_nios22_IRQ");                     
  sc_trace(my_trace_file, avalon_switch_wrapper_m2.irqnumber, "AVALON_nios22_IRQNUMBER");           


  //==============================Traces avalon_nios23_wrapper   --  AvalonSwitchFabric
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.reset, "AVALON_nios23_RESET");                
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.resetrequest, "AVALON_nios23_RESETREQUEST");   
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.clk, "AVALON_nios23_CLK");
  sc_trace(my_trace_file, avalon_switch_wrapper_m3. waitrequest, "AVALON_nios23_WAITREQUEST");     
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.write, "AVALON_nios23_WRITE");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.read, "AVALON_nios23_READ");                   
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.address, "AVALON_nios23_ADDRESS");            
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.writedata, "AVALON_nios23_WRITEDATA");          
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.readdata, "AVALON_nios23_READDATA");            
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.byteenable, "AVALON_nios23_BYTEENABLE");       
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.readdatavalid, "AVALON_nios23_READDATAVALID"); 
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.flush, "AVALON_nios23_FLUSH");                 
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.burstcount, "AVALON_nios23_BURSTCOUNT");      
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.endofpacket, "AVALON_nios23_ENDOFPACKET");     
  //sc_trace(my_trace_file, avalon_switch_wrapper_m3.data, "AVALON_nios23_DATA");                  
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.irq, "AVALON_nios23_IRQ");                     
  sc_trace(my_trace_file, avalon_switch_wrapper_m3.irqnumber, "AVALON_nios23_IRQNUMBER");           


  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->sel_master, "Arbiter_0_sel_master");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->req_courante,"Arbiter_0_req_courante");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->requete[0], "Arbiter_0_requete[0]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->requete[1], "Arbiter_0_requete[1]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->requete[2], "Arbiter_0_requete[2]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->requete[3], "Arbiter_0_requete[3]");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->sel_master,  "Arbiter_1_sel_master");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->r_fsm_state,  "Arbiter_1_r_fsm_state");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->req_courante,"Arbiter_1_req_courante");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->req_nouv, "Arbiter_1_req_nouv");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->requete[0],  "Arbiter_1_requete[0]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->requete[1],  "Arbiter_1_requete[1]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->requete[2],  "Arbiter_1_requete[2]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->requete[3],  "Arbiter_1_requete[3]");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->sel_master,  "Arbiter_2_sel_master");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->req_courante,"Arbiter_2_req_courante");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->requete[0],  "Arbiter_2_requete[0]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->requete[1],  "Arbiter_2_requete[1]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->requete[2],  "Arbiter_2_requete[2]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->requete[3],  "Arbiter_2_requete[3]");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->sel_master,  "Arbiter_3_sel_master");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->req_courante,"Arbiter_3_req_courante");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->requete[0],  "Arbiter_3_requete[0]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->requete[1],  "Arbiter_3_requete[1]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->requete[2],  "Arbiter_3_requete[2]");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->requete[3],  "Arbiter_3_requete[3]");


  // ARBITERS
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_master[0].chipselect, "arbiter0_master[0]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_master[1].chipselect, "arbiter0_master[1]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_master[2].chipselect, "arbiter0_master[2]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_master[3].chipselect, "arbiter0_master[3]_chipselect");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[0].chipselect, "arbiter1_master[0]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[1].chipselect, "arbiter1_master[1]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[2].chipselect, "arbiter1_master[2]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[3].chipselect, "arbiter1_master[3]_chipselect");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->p_arbiter_master[0].chipselect, "arbiter2_master[0]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->p_arbiter_master[1].chipselect, "arbiter2_master[1]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->p_arbiter_master[2].chipselect, "arbiter2_master[2]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[2]->p_arbiter_master[3].chipselect, "arbiter2_master[3]_chipselect");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->p_arbiter_master[0].chipselect, "arbiter3_master[0]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->p_arbiter_master[1].chipselect, "arbiter3_master[1]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->p_arbiter_master[2].chipselect, "arbiter3_master[2]_chipselect");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[3]->p_arbiter_master[3].chipselect, "arbiter3_master[3]_chipselect");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[0].read, "arbiter1_master[0]_read");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[0].write, "arbiter1_master[0]_write");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[1].read, "arbiter1_master[1]_read");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[1].write, "arbiter1_master[1]_write");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[2].read, "arbiter1_master[2]_read");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[2].write, "arbiter1_master[2]_write");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[3].read, "arbiter1_master[3]_read");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[1]->p_arbiter_master[3].write, "arbiter1_master[3]_write");

  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_master[0].address, "arbiter0_master[0]_address");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_master[1].address, "arbiter0_master[1]_address");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_master[2].address, "arbiter0_master[2]_address");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_master[3].address, "arbiter0_master[3]_address");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->select_master, "arbiter0_slave[0]_select_master");
  sc_trace(my_trace_file, SwitchFabric.to_arbiter[0]->p_arbiter_slave[0].address, "arbiter0_slave[0]_address");

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

  //==============================Traces  VCIRAM1  
  sc_trace(my_trace_file, signal_vci_vciram1.cmd, "vciram1_cmd");
  sc_trace(my_trace_file, signal_vci_vciram1.cmdval, "vciram1_cmdval");
  sc_trace(my_trace_file, signal_vci_vciram1.eop, "vciram1_eop");
  sc_trace(my_trace_file, signal_vci_vciram1.address, "vciram1_address");
  sc_trace(my_trace_file, signal_vci_vciram1.be, "vciram1_be");
  sc_trace(my_trace_file, signal_vci_vciram1.wdata, "vciram1_wdata");
  sc_trace(my_trace_file, signal_vci_vciram1.cmdack, "vciram1_cmdack");
  sc_trace(my_trace_file, signal_vci_vciram1.rdata, "vciram1_rdata");
  sc_trace(my_trace_file, signal_vci_vciram1.contig, "vciram1_contig");
  sc_trace(my_trace_file, signal_vci_vciram1.wrap, "vciram1_wrap");
  sc_trace(my_trace_file, signal_vci_vciram1.cons, "vciram1_cons");
  sc_trace(my_trace_file, signal_vci_vciram1.plen, "vciram1_plen");
  sc_trace(my_trace_file, signal_vci_vciram1.rspack, "vciram1_rspack");
  sc_trace(my_trace_file, signal_vci_vciram1.rspval, "vciram1_rspval");
  sc_trace(my_trace_file, signal_vci_vciram1.reop, "vciram1_reop");
  sc_trace(my_trace_file, signal_vci_vciram1.clen, "vciram1_clen");
  sc_trace(my_trace_file, signal_vci_vciram1.srcid, "vciram1_srcid");
  sc_trace(my_trace_file, signal_vci_vciram1.pktid, "vciram1_pktid");
  sc_trace(my_trace_file, signal_vci_vciram1.trdid, "vciram1_trdid");
  sc_trace(my_trace_file, signal_vci_vciram1.rtrdid, "vciram1_rtrdid");
  sc_trace(my_trace_file, signal_vci_vciram1.rsrcid, "vciram1_rsrcid");
  sc_trace(my_trace_file, signal_vci_vciram1.rpktid, "vciram1_rpktid");


  //==============================Traces  VCICACHE0  
  sc_trace(my_trace_file, signal_vci_initiator_m0.cmd, "vci_cache_m0_cmd");
  sc_trace(my_trace_file, signal_vci_initiator_m0.cmdval, "vci_cache_m0_cmdval");
  sc_trace(my_trace_file, signal_vci_initiator_m0.eop, "vci_cache_m0_eop");
  sc_trace(my_trace_file, signal_vci_initiator_m0.address, "vci_cache_m0_address");
  sc_trace(my_trace_file, signal_vci_initiator_m0.be, "vci_cache_m0_be");
  sc_trace(my_trace_file, signal_vci_initiator_m0.wdata, "vci_cache_m0_wdata");
  sc_trace(my_trace_file, signal_vci_initiator_m0.cmdack, "vci_cache_m0_cmdack");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rdata, "vci_cache_m0_rdata");
  sc_trace(my_trace_file, signal_vci_initiator_m0.contig, "vci_cache_m0_contig");
  sc_trace(my_trace_file, signal_vci_initiator_m0.wrap, "vci_cache_m0_wrap");
  sc_trace(my_trace_file, signal_vci_initiator_m0.cons, "vci_cache_m0_cons");
  sc_trace(my_trace_file, signal_vci_initiator_m0.plen, "vci_cache_m0_plen");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rspack, "vci_cache_m0_rspack");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rspval, "vci_cache_m0_rspval");
  sc_trace(my_trace_file, signal_vci_initiator_m0.reop, "vci_cache_m0_reop");
  sc_trace(my_trace_file, signal_vci_initiator_m0.clen, "vci_cache_m0_clen");
  sc_trace(my_trace_file, signal_vci_initiator_m0.srcid, "vci_cache_m0_srcid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.pktid, "vci_cache_m0_pktid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.trdid, "vci_cache_m0_trdid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rtrdid, "vci_cache_m0_rtrdid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rsrcid, "vci_cache_m0_rsrcid");
  sc_trace(my_trace_file, signal_vci_initiator_m0.rpktid, "vci_cache_m0_rpktid");


  //==============================Traces  VCICACHE1  
  sc_trace(my_trace_file, signal_vci_initiator_m1.cmd, "vci_cache_m1_cmd");
  sc_trace(my_trace_file, signal_vci_initiator_m1.cmdval, "vci_cache_m1_cmdval");
  sc_trace(my_trace_file, signal_vci_initiator_m1.eop, "vci_cache_m1_eop");
  sc_trace(my_trace_file, signal_vci_initiator_m1.address, "vci_cache_m1_address");
  sc_trace(my_trace_file, signal_vci_initiator_m1.be, "vci_cache_m1_be");
  sc_trace(my_trace_file, signal_vci_initiator_m1.wdata, "vci_cache_m1_wdata");
  sc_trace(my_trace_file, signal_vci_initiator_m1.cmdack, "vci_cache_m1_cmdack");
  sc_trace(my_trace_file, signal_vci_initiator_m1.rdata, "vci_cache_m1_rdata");
  sc_trace(my_trace_file, signal_vci_initiator_m1.contig, "vci_cache_m1_contig");
  sc_trace(my_trace_file, signal_vci_initiator_m1.wrap, "vci_cache_m1_wrap");
  sc_trace(my_trace_file, signal_vci_initiator_m1.cons, "vci_cache_m1_cons");
  sc_trace(my_trace_file, signal_vci_initiator_m1.plen, "vci_cache_m1_plen");
  sc_trace(my_trace_file, signal_vci_initiator_m1.rspack, "vci_cache_m1_rspack");
  sc_trace(my_trace_file, signal_vci_initiator_m1.rspval, "vci_cache_m1_rspval");
  sc_trace(my_trace_file, signal_vci_initiator_m1.reop, "vci_cache_m1_reop");
  sc_trace(my_trace_file, signal_vci_initiator_m1.clen, "vci_cache_m1_clen");
  sc_trace(my_trace_file, signal_vci_initiator_m1.srcid, "vci_cache_m1_srcid");
  sc_trace(my_trace_file, signal_vci_initiator_m1.pktid, "vci_cache_m1_pktid");
  sc_trace(my_trace_file, signal_vci_initiator_m1.trdid, "vci_cache_m1_trdid");
  sc_trace(my_trace_file, signal_vci_initiator_m1.rtrdid, "vci_cache_m1_rtrdid");
  sc_trace(my_trace_file, signal_vci_initiator_m1.rsrcid, "vci_cache_m1_rsrcid");
  sc_trace(my_trace_file, signal_vci_initiator_m1.rpktid, "vci_cache_m1_rpktid");



  //==============================Traces  VCICACHE2  
  sc_trace(my_trace_file, signal_vci_initiator_m2.cmd, "vci_cache_m2_cmd");
  sc_trace(my_trace_file, signal_vci_initiator_m2.cmdval, "vci_cache_m2_cmdval");
  sc_trace(my_trace_file, signal_vci_initiator_m2.eop, "vci_cache_m2_eop");
  sc_trace(my_trace_file, signal_vci_initiator_m2.address, "vci_cache_m2_address");
  sc_trace(my_trace_file, signal_vci_initiator_m2.be, "vci_cache_m2_be");
  sc_trace(my_trace_file, signal_vci_initiator_m2.wdata, "vci_cache_m2_wdata");
  sc_trace(my_trace_file, signal_vci_initiator_m2.cmdack, "vci_cache_m2_cmdack");
  sc_trace(my_trace_file, signal_vci_initiator_m2.rdata, "vci_cache_m2_rdata");
  sc_trace(my_trace_file, signal_vci_initiator_m2.contig, "vci_cache_m2_contig");
  sc_trace(my_trace_file, signal_vci_initiator_m2.wrap, "vci_cache_m2_wrap");
  sc_trace(my_trace_file, signal_vci_initiator_m2.cons, "vci_cache_m2_cons");
  sc_trace(my_trace_file, signal_vci_initiator_m2.plen, "vci_cache_m2_plen");
  sc_trace(my_trace_file, signal_vci_initiator_m2.rspack, "vci_cache_m2_rspack");
  sc_trace(my_trace_file, signal_vci_initiator_m2.rspval, "vci_cache_m2_rspval");
  sc_trace(my_trace_file, signal_vci_initiator_m2.reop, "vci_cache_m2_reop");
  sc_trace(my_trace_file, signal_vci_initiator_m2.clen, "vci_cache_m2_clen");
  sc_trace(my_trace_file, signal_vci_initiator_m2.srcid, "vci_cache_m2_srcid");
  sc_trace(my_trace_file, signal_vci_initiator_m2.pktid, "vci_cache_m2_pktid");
  sc_trace(my_trace_file, signal_vci_initiator_m2.trdid, "vci_cache_m2_trdid");
  sc_trace(my_trace_file, signal_vci_initiator_m2.rtrdid, "vci_cache_m2_rtrdid");
  sc_trace(my_trace_file, signal_vci_initiator_m2.rsrcid, "vci_cache_m2_rsrcid");
  sc_trace(my_trace_file, signal_vci_initiator_m2.rpktid, "vci_cache_m2_rpktid");


  //==============================Traces  VCICACHE3  
  sc_trace(my_trace_file, signal_vci_initiator_m3.cmd, "vci_cache_m3_cmd");
  sc_trace(my_trace_file, signal_vci_initiator_m3.cmdval, "vci_cache_m3_cmdval");
  sc_trace(my_trace_file, signal_vci_initiator_m3.eop, "vci_cache_m3_eop");
  sc_trace(my_trace_file, signal_vci_initiator_m3.address, "vci_cache_m3_address");
  sc_trace(my_trace_file, signal_vci_initiator_m3.be, "vci_cache_m3_be");
  sc_trace(my_trace_file, signal_vci_initiator_m3.wdata, "vci_cache_m3_wdata");
  sc_trace(my_trace_file, signal_vci_initiator_m3.cmdack, "vci_cache_m3_cmdack");
  sc_trace(my_trace_file, signal_vci_initiator_m3.rdata, "vci_cache_m3_rdata");
  sc_trace(my_trace_file, signal_vci_initiator_m3.contig, "vci_cache_m3_contig");
  sc_trace(my_trace_file, signal_vci_initiator_m3.wrap, "vci_cache_m3_wrap");
  sc_trace(my_trace_file, signal_vci_initiator_m3.cons, "vci_cache_m3_cons");
  sc_trace(my_trace_file, signal_vci_initiator_m3.plen, "vci_cache_m3_plen");
  sc_trace(my_trace_file, signal_vci_initiator_m3.rspack, "vci_cache_m3_rspack");
  sc_trace(my_trace_file, signal_vci_initiator_m3.rspval, "vci_cache_m3_rspval");
  sc_trace(my_trace_file, signal_vci_initiator_m3.reop, "vci_cache_m3_reop");
  sc_trace(my_trace_file, signal_vci_initiator_m3.clen, "vci_cache_m3_clen");
  sc_trace(my_trace_file, signal_vci_initiator_m3.srcid, "vci_cache_m3_srcid");
  sc_trace(my_trace_file, signal_vci_initiator_m3.pktid, "vci_cache_m3_pktid");
  sc_trace(my_trace_file, signal_vci_initiator_m3.trdid, "vci_cache_m3_trdid");
  sc_trace(my_trace_file, signal_vci_initiator_m3.rtrdid, "vci_cache_m3_rtrdid");
  sc_trace(my_trace_file, signal_vci_initiator_m3.rsrcid, "vci_cache_m3_rsrcid");
  sc_trace(my_trace_file, signal_vci_initiator_m3.rpktid, "vci_cache_m3_rpktid");




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


  //==============================Traces  VCILOCKS
  sc_trace(my_trace_file, signal_vci_vcilocks.cmd, "vci_vcilocks_cmd");
  sc_trace(my_trace_file, signal_vci_vcilocks.cmdval, "vci_vcilocks_cmdval");
  sc_trace(my_trace_file, signal_vci_vcilocks.eop, "vci_vcilocks_eop");
  sc_trace(my_trace_file, signal_vci_vcilocks.address, "vci_vcilocks_address");
  sc_trace(my_trace_file, signal_vci_vcilocks.be, "vci_vcilocks_be");
  sc_trace(my_trace_file, signal_vci_vcilocks.wdata, "vci_vcilocks_wdata");
  sc_trace(my_trace_file, signal_vci_vcilocks.cmdack, "vci_vcilocks_cmdack");
  sc_trace(my_trace_file, signal_vci_vcilocks.rdata, "vci_vcilocks_rdata");
  sc_trace(my_trace_file, signal_vci_vcilocks.contig, "vci_vcilocks_contig");
  sc_trace(my_trace_file, signal_vci_vcilocks.wrap, "vci_vcilocks_wrap");
  sc_trace(my_trace_file, signal_vci_vcilocks.cons, "vci_vcilocks_cons");
  sc_trace(my_trace_file, signal_vci_vcilocks.plen, "vci_vcilocks_plen");
  sc_trace(my_trace_file, signal_vci_vcilocks.rspack, "vci_vcilocks_rspack");
  sc_trace(my_trace_file, signal_vci_vcilocks.rspval, "vci_vcilocks_rspval");
  sc_trace(my_trace_file, signal_vci_vcilocks.reop, "vci_vcilocks_reop");
  sc_trace(my_trace_file, signal_vci_vcilocks.clen, "vci_vcilocks_clen");
  sc_trace(my_trace_file, signal_vci_vcilocks.srcid, "vci_vcilocks_srcid");
  sc_trace(my_trace_file, signal_vci_vcilocks.pktid, "vci_vcilocks_pktid");
  sc_trace(my_trace_file, signal_vci_vcilocks.trdid, "vci_vcilocks_trdid");
  sc_trace(my_trace_file, signal_vci_vcilocks.rtrdid, "vci_vcilocks_rtrdid");
  sc_trace(my_trace_file, signal_vci_vcilocks.rsrcid, "vci_vcilocks_rsrcid");
  sc_trace(my_trace_file, signal_vci_vcilocks.rpktid, "vci_vcilocks_rpktid");

  // Interruptions
  sc_trace(my_trace_file, signal_nios2_irq00, "signal_nios2_irq00");
  sc_trace(my_trace_file, signal_nios2_irq0[32], "ssignal_nios2_irq0[32]");

  sc_trace(my_trace_file, signal_nios2_irq10, "signal_nios2_irq10");
  sc_trace(my_trace_file, signal_nios2_irq1[32], "ssignal_nios2_irq1[32]");

  sc_trace(my_trace_file, signal_nios2_irq20, "signal_nios2_irq20");
  sc_trace(my_trace_file, signal_nios2_irq2[32], "ssignal_nios2_irq2[32]");

  sc_trace(my_trace_file, signal_nios2_irq30, "signal_nios2_irq30");
  sc_trace(my_trace_file, signal_nios2_irq3[32], "ssignal_nios2_irq3[32]");
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

