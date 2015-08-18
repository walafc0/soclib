#include <iostream>
#include <systemc>
#include <tlmt>
#include "vci_ports.h"
#include "fifo_ports.h"
#include "mapping_table.h"
#include "vci_vgmn.h"
#include "vci_simple_initiator.h"
#include "vci_ram.h"
#include "vci_mwmr_controller.h"
#include "coprocessor.h"
#include "segmentation.h"

#include <sys/timeb.h>

int sc_main(int argc, char **argv)
{
  struct timeb initial, final;

  ftime(&initial);

  tlmt_core::tlmt_time simulationTime;
  if(argc>1)
    simulationTime = tlmt_core::tlmt_time(atoi(argv[1]));
  else
    simulationTime = tlmt_core::tlmt_time(10000);

  //typedef soclib::tlmt::VciParams<uint32_t,uint32_t> vci_param;
  typedef soclib::tlmt::VciParams<uint32_t,uint32_t,4> vci_param;

  // Avoid repeating these everywhere
  using soclib::common::IntTab;
  using soclib::common::Segment;

  // Mapping table
  soclib::common::MappingTable maptab(32, IntTab(12), IntTab(12), 0x00300000);

  maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
  maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
  maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));

  maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));

  maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));
  maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(3), false));

  maptab.add(Segment("loc0" , LOC0_BASE , LOC0_SIZE , IntTab(4), true));
  maptab.add(Segment("loc1" , LOC1_BASE , LOC1_SIZE , IntTab(5), true));
  maptab.add(Segment("loc2" , LOC2_BASE , LOC2_SIZE , IntTab(6), true));
  maptab.add(Segment("loc3" , LOC3_BASE , LOC3_SIZE , IntTab(7), true));

  maptab.add(Segment("test0" , TEST0_BASE , TEST0_SIZE , IntTab(8), true));
  maptab.add(Segment("test1" , TEST1_BASE , TEST1_SIZE , IntTab(9), true));
  maptab.add(Segment("test2" , TEST2_BASE , TEST2_SIZE , IntTab(10), true));
  maptab.add(Segment("test3" , TEST3_BASE , TEST3_SIZE , IntTab(11), true));
  maptab.add(Segment("test4" , TEST4_BASE , TEST4_SIZE , IntTab(12), true));
  maptab.add(Segment("test5" , TEST5_BASE , TEST5_SIZE , IntTab(13), true));
  maptab.add(Segment("test6" , TEST6_BASE , TEST6_SIZE , IntTab(14), true));
  maptab.add(Segment("test7" , TEST7_BASE , TEST7_SIZE , IntTab(15), true));

  soclib::tlmt::VciVgmn<vci_param> vgmn(3,3,maptab,10);

  /////////////////////////////////////////////////////////////////////////////
  // SIMPLE INITIATOR 
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmt::VciSimpleInitiator<vci_param>  i0( "init0", 0,  100, simulationTime);

  i0.p_vci(vgmn.m_RspArbCmdRout[0]->p_vci);

  /////////////////////////////////////////////////////////////////////////////
  // RAM
  /////////////////////////////////////////////////////////////////////////////
  soclib::common::Loader loader("soft/bin.soft");

  soclib::tlmt::VciRam<vci_param> ram0("ram0", 0, IntTab(0), maptab, loader);

  vgmn.m_CmdArbRspRout[0]->p_vci(ram0.p_vci);

  /////////////////////////////////////////////////////////////////////////////
  // MWMR AND COPROCESSOR 0
  /////////////////////////////////////////////////////////////////////////////
  uint32_t read_fifo_depth = 0;
  uint32_t write_fifo_depth = 48;
  uint32_t n_read_channel = 0;
  uint32_t n_write_channel = 1;
  uint32_t n_config = 3;
  uint32_t n_status = 3;

  soclib::tlmt::VciMwmrController<vci_param> mwmr0("mwmr0", maptab, IntTab(1), IntTab(1), read_fifo_depth, write_fifo_depth, n_read_channel, n_write_channel, n_config, n_status);

  mwmr0.p_vci_initiator(vgmn.m_RspArbCmdRout[1]->p_vci);
  vgmn.m_CmdArbRspRout[1]->p_vci(mwmr0.p_vci_target);

  soclib::tlmt::Coprocessor<vci_param> coprocessor0("coprocessor0",0,(read_fifo_depth/4), (write_fifo_depth/4), n_read_channel, n_write_channel, n_config, n_status);

  for(uint32_t i=0; i<n_read_channel; i++)
    (*mwmr0.p_read_fifo[i])(*coprocessor0.p_read_fifo[i]);

  for(uint32_t i=0; i<n_write_channel; i++)
    (*mwmr0.p_write_fifo[i])(*coprocessor0.p_write_fifo[i]);

  for(uint32_t i=0; i<n_config; i++)
    (*mwmr0.p_config[i])(*coprocessor0.p_config[i]);

  for(uint32_t i=0; i<n_status; i++)
    (*mwmr0.p_status[i])(*coprocessor0.p_status[i]);


  /////////////////////////////////////////////////////////////////////////////
  // MWMR AND COPROCESSOR 1
  /////////////////////////////////////////////////////////////////////////////
  read_fifo_depth = 48;
  write_fifo_depth = 0;
  n_read_channel = 1;
  n_write_channel = 0;
  n_config = 3;
  n_status = 3;

  soclib::tlmt::VciMwmrController<vci_param> mwmr1("mwmr1", maptab, IntTab(2), IntTab(2), read_fifo_depth, write_fifo_depth, n_read_channel, n_write_channel, n_config, n_status);

  mwmr1.p_vci_initiator(vgmn.m_RspArbCmdRout[2]->p_vci);
  vgmn.m_CmdArbRspRout[2]->p_vci(mwmr1.p_vci_target);

  soclib::tlmt::Coprocessor<vci_param> coprocessor1("coprocessor1",1,(read_fifo_depth/4), (write_fifo_depth/4), n_read_channel, n_write_channel, n_config, n_status);

  for(uint32_t i=0; i<n_read_channel; i++)
    (*mwmr1.p_read_fifo[i])(*coprocessor1.p_read_fifo[i]);

  for(uint32_t i=0; i<n_write_channel; i++)
    (*mwmr1.p_write_fifo[i])(*coprocessor1.p_write_fifo[i]);

  for(uint32_t i=0; i<n_config; i++)
    (*mwmr1.p_config[i])(*coprocessor1.p_config[i]);

  for(uint32_t i=0; i<n_status; i++)
    (*mwmr1.p_status[i])(*coprocessor1.p_status[i]);


  /////////////////////////////////////////////////////////////////////////////
  // START
  /////////////////////////////////////////////////////////////////////////////
  sc_core::sc_start();

  ftime(&final);

  std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl;

  return 0;
}

