#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include <systemc>
#include <tlmt>
#include "vci_ports.h"
#include "fifo_ports.h"
#include "mapping_table.h"
#include "vci_vgmn.h"
#include "iss.h"
#include "st231.hh"
#include "vci_xcache_wrapper.h"
#include "vci_mwmr_controller.h"
#include "fifo_reader.h"
#include "fifo_writer.h"
#include "vci_ram.h"
#include "tty.h"
#include "vci_multi_tty.h"

#include <sys/timeb.h>
#include "segmentation.h"

std::vector<std::string> stringArray(const char *first, ... )
{ std::vector<std::string> ret;
  va_list arg;
  va_start(arg, first);
  const char *s = first;
  while (s)
  { ret.push_back(std::string(s));
    s = va_arg(arg, const char *);
  };
  va_end(arg);
  return ret;
}

std::vector<int> intArray(const int length, ... )
{ int i;
  std::vector<int> ret;
  va_list arg;
  va_start(arg, length);
  for (i=0; i<length; ++i)
  { ret.push_back(va_arg(arg, int));
  };
  va_end(arg);
  return ret;
}

int sc_main(int argc, char **argv)
{ struct timeb initial, final;
  typedef soclib::tlmt::VciParams<uint32_t,uint32_t,4> vci_param;
  size_t read_depth, write_depth, n_read_channel, n_write_channel, n_config, n_status;

  size_t simulation_time = std::numeric_limits<uint32_t>::max();
  size_t dcache_size     = 1024;
  size_t icache_size     = 1024;
  size_t network_latence = 3;

  if (argc > 1)
    simulation_time = atoi(argv[1]);
  if (argc > 2)
    icache_size = atoi(argv[2]);
  if (argc > 3)
    dcache_size = atoi(argv[3]);
  if (argc > 4)
    network_latence = atoi(argv[4]);

  std::cout << "SIMULATION PARAMETERS: simulation time = " << simulation_time << " icache size = " << icache_size << " dcache size = " << dcache_size << " network latence = " << network_latence << std::endl << std::endl;

  ftime(&initial);

  // Configurator instanciateOnStack
  st231::mapfile = "soft/bin.maps";
  soclib::common::Loader loader("soft/bin.soft");
  soclib::common::MappingTable mapping_table(32, soclib::common::IntTab(8), soclib::common::IntTab(8), 0x00200000);





  // Configurator configure

  mapping_table.add(soclib::common::Segment("reset", RESET_BASE, RESET_SIZE, soclib::common::IntTab(0), false)); 
  mapping_table.add(soclib::common::Segment("text" , TEXT_BASE , TEXT_SIZE , soclib::common::IntTab(1), false));
  mapping_table.add(soclib::common::Segment("data" , DATA_BASE , DATA_SIZE , soclib::common::IntTab(1), false));
  mapping_table.add(soclib::common::Segment("tty0",  TTY_BASE  , TTY_SIZE  , soclib::common::IntTab(2), 0));

//???????????????

/*
  mapping_table.add(soclib::common::Segment("boot",        0xbfc00000,       2048, soclib::common::IntTab(1), 1));
  mapping_table.add(soclib::common::Segment("cram0",       0x10000000, 0x00100000, soclib::common::IntTab(0), 1));
  mapping_table.add(soclib::common::Segment("cram1",       0x20000000, 0x00100000, soclib::common::IntTab(1), 1));
  mapping_table.add(soclib::common::Segment("excep",       0x80000080,       2048, soclib::common::IntTab(1), 1));
  mapping_table.add(soclib::common::Segment("ramdac_ctrl", 0x71200000,        256, soclib::common::IntTab(4), 0));
  mapping_table.add(soclib::common::Segment("tg_ctrl",     0x70200000,        256, soclib::common::IntTab(3), 0));
  mapping_table.add(soclib::common::Segment("tty0",        0x90200000,         32, soclib::common::IntTab(2), 0));
  mapping_table.add(soclib::common::Segment("uram0",       0x10200000, 0x00100000, soclib::common::IntTab(0), 0));
  mapping_table.add(soclib::common::Segment("uram1",       0x20200000, 0x00100000, soclib::common::IntTab(1), 0));
*/

  /////////////////////////////////////////////////////////////////////////////
  // VCI_VGMN
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmt::VciVgmn<vci_param> vgmn(3,5,mapping_table,network_latence);

  /////////////////////////////////////////////////////////////////////////////
  // VCI_XCACHE
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmt::VciXcacheWrapper<soclib::common::ST231iss,vci_param> mips0("mips0", soclib::common::IntTab(0), mapping_table, icache_size, 8, dcache_size, 8, simulation_time);

  mips0.p_vci(vgmn.m_RspArbCmdRout[0]->p_vci);

  /////////////////////////////////////////////////////////////////////////////
  // MWMR AND COPROCESSOR RAMDAC
  /////////////////////////////////////////////////////////////////////////////
  read_depth      = 96;
  write_depth     = 0;
  n_read_channel  = 1;
  n_write_channel = 0;
  n_config        = 0;
  n_status        = 0;

  soclib::tlmt::VciMwmrController<vci_param> ramdac_ctrl("ramdac_ctrl", mapping_table, soclib::common::IntTab(2), soclib::common::IntTab(4), read_depth, write_depth, n_read_channel, n_write_channel, n_config, n_status);

  ramdac_ctrl.p_vci_initiator(vgmn.m_RspArbCmdRout[2]->p_vci);
  vgmn.m_CmdArbRspRout[4]->p_vci(ramdac_ctrl.p_vci_target);

  soclib::tlmt::FifoWriter<vci_param>  ramdac("ramdac", "soclib-pipe2fb", stringArray("soclib-pipe2fb", "48", "48", NULL),read_depth);

  (*ramdac_ctrl.p_read_fifo[0])(ramdac.p_fifo);

  /////////////////////////////////////////////////////////////////////////////
  // MWMR AND COPROCESSOR TG
  /////////////////////////////////////////////////////////////////////////////

  read_depth      = 0;
  write_depth     = 8;
  n_read_channel  = 0;
  n_write_channel = 1;
  n_config        = 0;
  n_status        = 0;

  soclib::tlmt::VciMwmrController<vci_param> tg_ctrl("tg_ctrl", mapping_table, soclib::common::IntTab(1), soclib::common::IntTab(3), read_depth, write_depth, n_read_channel, n_write_channel, n_config, n_status);

  tg_ctrl.p_vci_initiator(vgmn.m_RspArbCmdRout[1]->p_vci);
  vgmn.m_CmdArbRspRout[3]->p_vci(tg_ctrl.p_vci_target);

  soclib::tlmt::FifoReader<vci_param>  tg("tg", "bash", stringArray("bash", "-c", "while cat \"plan.jpg\" ; do true ; done", NULL),write_depth);

  (*tg_ctrl.p_write_fifo[0])(tg.p_fifo);

  /////////////////////////////////////////////////////////////////////////////
  // RAM
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmt::VciRam<vci_param> ram0("ram0", 0, soclib::common::IntTab(0), mapping_table, loader);

  vgmn.m_CmdArbRspRout[0]->p_vci(ram0.p_vci);

  /////////////////////////////////////////////////////////////////////////////
  // RAM
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmt::VciRam<vci_param> ram1("ram1", 1, soclib::common::IntTab(1), mapping_table, loader);

  vgmn.m_CmdArbRspRout[1]->p_vci(ram1.p_vci);

  /////////////////////////////////////////////////////////////////////////////
  // TTY
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmt::VciMultiTty<vci_param> vcitty("tty0",soclib::common::IntTab(2), mapping_table, "TTY0", NULL);

  vgmn.m_CmdArbRspRout[2]->p_vci(vcitty.p_vci);
  (*vcitty.p_irq[0])(mips0.p_irq[0]);

  /////////////////////////////////////////////////////////////////////////////
  // START
  /////////////////////////////////////////////////////////////////////////////
  sc_core::sc_start();

  ftime(&final);

  std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl << std::endl;

  return 0;
}

