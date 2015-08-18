
#define USE_GDB_SERVER
//#define USE_SIMHELPER
//#define USE_MEMCHECKER

//#define USE_LOG_CONSOLE

#include <systemc>
#include <sys/time.h>
#include <iostream>
#include <cstdlib>
#include <cstdarg>

#include "gdbserver.h"
#include "iss_memchecker.h"

#include "mapping_table.h"
#include "vci_rom.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_log_console.h"
#include "vci_vgmn.h"
#include "vci_xcache_wrapper.h"
#include "vci_icu.h"
#include "vci_timer.h"
#include "vci_block_device.h"

#include "arm.h"

typedef soclib::common::ArmIss iss_t;

#include "gdbserver.h"
#include "iss_memchecker.h"

typedef soclib::common::GdbServer<soclib::common::IssMemchecker<iss_t> > complete_iss_t;

//#include "iss2_simhelper.h"
//typedef soclib::common::Iss2Simhelper<iss_t> complete_iss_t;

int _main(int argc, char *argv[])
{
  using namespace sc_core;
  // Avoid repeating these everywhere
  using soclib::common::IntTab;
  using soclib::common::Segment;
  const size_t icu_n_irq = 4;

  // Define VCI parameters
  typedef soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1> vci_param;

  if ( argc < 2 ) {
    std::cerr << "Usage: " << argv[0] << " <arm_kernel_file>" << std::endl;
    return 1;
  }

  // Mapping table
  soclib::common::Loader loader(argv[1], "platform.dtb@0xe0000000:RO");

  // This puts 0x5a everywhere in the memory, this eases detection
  // of uninitialized variables bugs.
  //  loader.memory_default(0x5a);

  soclib::common::MappingTable maptabp(32, IntTab(8), IntTab(8), 0xf0000000);
	
  maptabp.add(Segment("boot",  0x00000000, 0x00000200, IntTab(0), true));
  maptabp.add(Segment("text",  0x60000000, 0x00100000, IntTab(0), true));
  maptabp.add(Segment("rodata",0x80000000, 0x01000000, IntTab(0), true));
  maptabp.add(Segment("fdt",   0xe0000000, 0x00001000, IntTab(0), false)); // device tree

  maptabp.add(Segment("tty",   0xd0200000, 0x00000040, IntTab(1), false));
  maptabp.add(Segment("mem",   0x7f000000, 0x01000000, IntTab(2), false));
  maptabp.add(Segment("icu",   0xd2200000, 0x00001000, IntTab(3), false));
  maptabp.add(Segment("bdev0", 0xd1200000, 0x00000020, IntTab(4), false));
  maptabp.add(Segment("timer", 0xd3200000, 0x00000020, IntTab(5), false));

  soclib::common::GdbServer<soclib::common::IssMemchecker<iss_t> >::set_loader(loader);
  soclib::common::IssMemchecker<iss_t>::init(maptabp, loader, "tty,icu,bdev0,timer");

  // Signals
  sc_clock	signal_clk("clk");
  sc_signal<bool> signal_resetn("resetn");

  sc_signal<bool> signal_proc_it[iss_t::n_irq]; 
   
  soclib::caba::VciSignals<vci_param> signal_vci_proc;

  soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
  soclib::caba::VciSignals<vci_param> signal_vci_icu("signal_vci_icu");

  soclib::caba::VciSignals<vci_param> signal_vci_bdi[1];
  soclib::caba::VciSignals<vci_param> signal_vci_bdt[1];

  soclib::caba::VciSignals<vci_param> signal_vci_timer("signal_vci_timer");

  soclib::caba::VciSignals<vci_param> signal_vci_ram("vci_tgt_ram");
  soclib::caba::VciSignals<vci_param> signal_vci_rom("vci_tgt_rom");

  sc_signal<bool> signal_icu_irq[icu_n_irq];

  soclib::caba::VciXcacheWrapper<vci_param, complete_iss_t > *procs;

  procs = new soclib::caba::VciXcacheWrapper<vci_param, complete_iss_t >("armcpu", 0, maptabp, IntTab(0),4,64,16, 4,64,16);

  soclib::caba::VciRom<vci_param> rom("rom", IntTab(0), maptabp, loader);
#ifdef USE_LOG_CONSOLE
  soclib::caba::VciLogConsole<vci_param> vcitty("vcitty",	IntTab(1), maptabp);
#else
  soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(1), maptabp, "vcitty0", NULL);
#endif
  soclib::caba::VciRam<vci_param> ram("ram", IntTab(2), maptabp, loader);
  soclib::caba::VciIcu<vci_param> vciicu("vciicu",IntTab(3), maptabp, icu_n_irq);
  soclib::caba::VciBlockDevice<vci_param> vcibd0("vcibd0", maptabp, IntTab(1), IntTab(4), "block0.iso", 2048);

  soclib::caba::VciTimer<vci_param> vcitimer("timer", IntTab(5), maptabp, 2);

  soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptabp, 1+1, 6, 2, 8);

  for ( size_t irq = 0; irq < iss_t::n_irq; ++irq )
    procs->p_irq[irq](signal_proc_it[irq]); 

  procs->p_clk(signal_clk);  
  procs->p_resetn(signal_resetn);  
  procs->p_vci(signal_vci_proc);

  rom.p_clk(signal_clk);
  rom.p_resetn(signal_resetn);
  rom.p_vci(signal_vci_rom);

  ram.p_clk(signal_clk);
  ram.p_resetn(signal_resetn);
  ram.p_vci(signal_vci_ram);

  vciicu.p_clk(signal_clk);
  vciicu.p_resetn(signal_resetn);
  vciicu.p_vci(signal_vci_icu);

  vcitimer.p_clk(signal_clk);
  vcitimer.p_resetn(signal_resetn);
  vcitimer.p_vci(signal_vci_timer);
  vcitimer.p_irq[0](signal_icu_irq[2]);
  vcitimer.p_irq[1](signal_icu_irq[3]);

  // Input IRQs
  for ( size_t i = 0; i<icu_n_irq; ++i )
    vciicu.p_irq_in[i](signal_icu_irq[i]);

  // Output IRQs to processor
  vciicu.p_irq(signal_proc_it[0]);

  vcibd0.p_clk(signal_clk);
  vcibd0.p_resetn(signal_resetn);
  vcibd0.p_irq(signal_icu_irq[1]); 
  vcibd0.p_vci_target(signal_vci_bdt[0]);
  vcibd0.p_vci_initiator(signal_vci_bdi[0]);

  vcitty.p_clk(signal_clk);
  vcitty.p_resetn(signal_resetn);
  vcitty.p_vci(signal_vci_tty);
#ifndef USE_LOG_CONSOLE
  vcitty.p_irq[0](signal_icu_irq[0]); 
#endif

  vgmn.p_clk(signal_clk);
  vgmn.p_resetn(signal_resetn);

  vgmn.p_to_initiator[0](signal_vci_proc);
  vgmn.p_to_initiator[1](signal_vci_bdi[0]);

  vgmn.p_to_target[0](signal_vci_rom);
  vgmn.p_to_target[1](signal_vci_tty);
  vgmn.p_to_target[2](signal_vci_ram);
  vgmn.p_to_target[3](signal_vci_icu);
  vgmn.p_to_target[4](signal_vci_bdt[0]);
  vgmn.p_to_target[5](signal_vci_timer);

  sc_start(sc_core::sc_time(0, SC_NS));
  signal_resetn = false;

  sc_start(sc_core::sc_time(1, SC_NS));
  signal_resetn = true;
	
  sc_start();

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

