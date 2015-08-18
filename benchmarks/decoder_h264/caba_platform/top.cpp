/*****************************************************************************
  Filename : top.cc 

  Authors:
  Fabien Colas-Bigey THALES COM - AAL, 2009

  Copyright (C) THALES COMMUNICATIONS
 
  This code is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.
   
  This code is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.
   
  You should have received a copy of the GNU General Public License along
  with this code (see file COPYING).  If not, see
  <http://www.gnu.org/licenses/>.
  
  This License does not grant permission to use the name of the copyright
  owner, except only as required above for reproducing the content of
  the copyright notice.
*****************************************************************************/


/****************************************************************************
  Macros and configuration of the platform
 ****************************************************************************/

/****************************************************************************
  Include section
 ****************************************************************************/
#include <systemc>
#include <sys/time.h>
#include <iostream>
#include <cstdlib>
#include <cstdarg>

#include "cpu_type.h"
#include "soclib_addresses.h"

#include "gdbserver.h"
#include "iss_memchecker.h"

#include "mapping_table.h"
#include "vci_rom.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"
#include "vci_xcache_wrapper.h"
#include "vci_xicu.h"
#include "vci_simhelper.h"

#ifdef CONFIG_FRAMEBUFFER
#	include "vci_framebuffer.h"
#	warning FRAMEBUFFER added on the platform
#endif
#ifdef CONFIG_TIMER
#	include "vci_timer.h"
#	warning Timer added on the platform
#endif


/****************************************************************************
  Global variables and structures
 ****************************************************************************/
#if defined(CPU_mips32el)
#   include "mips32.h"
#   warning Using a Mips32
typedef soclib::common::Mips32ElIss iss_t;
const char *default_kernel = "mutekh/kernel-soclib-mips32el.out";
#elif defined(CPU_ppc)
#   include "ppc405.h"
#   warning Using a PPC405
typedef soclib::common::Ppc405Iss iss_t;
const char *default_kernel = "mutekh/kernel-soclib-ppc.out";
#elif defined(CPU_arm)
#   include "arm.h"
#   warning Using an ARM
typedef soclib::common::ArmIss iss_t;
const char *default_kernel = "mutekh/kernel-soclib-arm.out";
#endif /* End of CPU switches */

#if defined(USE_GDB_SERVER) && defined(USE_MEMCHECKER)
#   include "gdbserver.h"
#   include "iss_memchecker.h"
typedef soclib::common::GdbServer<
  soclib::common::IssMemchecker<
    iss_t> > complete_iss_t;
#elif defined(USE_MEMCHECKER)
#   include "iss_memchecker.h"
typedef soclib::common::IssMemchecker<iss_t> complete_iss_t;
#elif defined(USE_GDB_SERVER)
#   include "gdbserver.h"
typedef soclib::common::GdbServer<iss_t> complete_iss_t;
#else
typedef iss_t complete_iss_t;
#endif


/****************************************************************************
  Sub Main functions
 ****************************************************************************/
int _main(int argc, char *argv[])
{
  /*************************************
     Internal declaration
  *************************************/
  // Avoid repeating these everywhere
  using namespace sc_core;
  using soclib::common::IntTab;
  using soclib::common::Segment;
  const size_t xicu_n_irq = 2;
  int nb_target = 0;

  // Define VCI parameters
  typedef soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1> vci_param;

  const char *kernel = default_kernel;
  if ( argc > 1 ) {
    kernel = argv[1];
  }

  /*************************************
     Mapping Table
  *************************************/
  soclib::common::Loader loader(kernel,"block0.iso@0x68200000:D");
  soclib::common::MappingTable maptabp(32, IntTab(8), IntTab(8), CACHABILITY_MASK);
  
#if defined(CPU_ppc)
  maptabp.add(Segment("boot",  SEG_BOOT_ADDR, SEG_BOOT_SIZE, IntTab(0), true));
#elif defined(CPU_arm)
  maptabp.add(Segment("boot",  SEG_BOOT_ADDR, SEG_BOOT_SIZE, IntTab(0), true));
#elif defined(CPU_mips32el)
  maptabp.add(Segment("boot",  SEG_BOOT_ADDR, SEG_BOOT_SIZE, IntTab(0), true));
#endif
  
  maptabp.add(Segment("text",  SEG_ROM_ADDR, SEG_ROM_SIZE, IntTab(0), true));
  //maptabp.add(Segment("rodata",SEG_RODATA_ADDR, SEG_RODATA_SIZE, IntTab(0), true));

  maptabp.add(Segment("tty",     SEG_TTY_ADDR,  SEG_TTY_SIZE,  IntTab(++nb_target), false));
  maptabp.add(Segment("mem",     SEG_TEXT_ADDR, SEG_TEXT_SIZE, IntTab(++nb_target), false));
  maptabp.add(Segment("xicu",    SEG_ICU_ADDR,  SEG_ICU_SIZE,  IntTab(++nb_target), false));
  maptabp.add(Segment("ramdisk", SEG_DISK_ADDR, SEG_DISK_SIZE, IntTab(++nb_target), false));
#ifdef CONFIG_FRAMEBUFFER
  maptabp.add(Segment("fbuffer", SEG_BUFF_ADDR, SEG_BUFF_SIZE, IntTab(++nb_target), false));
#endif
#ifdef CONFIG_TIMER
  maptabp.add(Segment("timer",   SEG_TIMER_ADDR,SEG_TIMER_SIZE,IntTab(++nb_target), false));
#endif
  maptabp.add(Segment("simhelper",SEG_SIMHELPER_ADDR,SEG_SIMHELPER_SIZE,IntTab(++nb_target), false));


#if defined(USE_GDB_SERVER) && defined(USE_MEMCHECKER)
  soclib::common::GdbServer<
  soclib::common::IssMemchecker<
  iss_t> >::set_loader(loader);
#endif
#if defined(USE_MEMCHECKER)
  soclib::common::IssMemchecker<
  iss_t>::init(maptabp, loader, "tty,xicu,fbuffer,timer");
#elif defined(USE_GDB_SERVER)
  soclib::common::GdbServer<iss_t>::set_loader(loader);
#endif

  
  /*************************************
     CABA Signals
  *************************************/
  sc_clock	  signal_clk("clk");
  sc_signal<bool> signal_resetn("resetn");

  sc_signal<bool> signal_proc_it[NCPU][iss_t::n_irq]; 
   
  soclib::caba::VciSignals<vci_param> signal_vci_proc[NCPU];

  soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
  soclib::caba::VciSignals<vci_param> signal_vci_xicu("signal_vci_xicu");

  soclib::caba::VciSignals<vci_param> signal_vci_ram("vci_tgt_ram");
  soclib::caba::VciSignals<vci_param> signal_vci_rom("vci_tgt_rom");

  soclib::caba::VciSignals<vci_param> signal_vci_ramdisk("signal_vci_ramdisk");
  
  sc_signal<bool> signal_xicu_irq[xicu_n_irq];

  soclib::caba::VciSignals<vci_param> signal_vci_simhelper("signal_vci_simhelper");

  
  /*************************************
     Components
  *************************************/
  nb_target = 0;
  soclib::caba::VciXcacheWrapper<vci_param, complete_iss_t > *procs[NCPU];
  for ( size_t i = 0; i<NCPU; ++i ) {
    std::ostringstream o;
    o << "proc" << i;
    procs[i] = new soclib::caba::VciXcacheWrapper<vci_param, complete_iss_t >
      (o.str().c_str(), i, maptabp, IntTab(i),4,64,CACHE_LINE_SIZE, 4,64,CACHE_LINE_SIZE);
  }

  soclib::caba::VciRom<vci_param> rom("rom", IntTab(nb_target), maptabp, loader);
  soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(++nb_target), maptabp, "vcitty0", NULL);
  soclib::caba::VciRam<vci_param> ram("ram", IntTab(++nb_target), maptabp, loader);
  soclib::caba::VciXicu<vci_param> vciicu("vciicu", maptabp,IntTab(++nb_target), NCPU, xicu_n_irq, NCPU, NCPU);
  soclib::caba::VciRam<vci_param> vciramdisk("vciramdisk", IntTab(++nb_target), maptabp, loader);

#ifdef CONFIG_FRAMEBUFFER
#	ifdef VIDEO_TYPE_cif
	soclib::caba::VciFrameBuffer<vci_param> vciframebuffer("vciframebuffer", IntTab(++nb_target), maptabp, 352, 288);
#	warning Using CIF video size
#	elif defined(VIDEO_TYPE_qcif)
	soclib::caba::VciFrameBuffer<vci_param> vciframebuffer("vciframebuffer", IntTab(++nb_target), maptabp, 176, 144);
#	warning Using QCIF video size
#	else
#	error No video type (CIF or QCIF) configuration defined
#	endif
#endif
#ifdef CONFIG_TIMER
  soclib::caba::VciTimer<vci_param> vcitimer("vcittimer", IntTab(++nb_target), maptabp, 1);
#endif
  soclib::caba::VciSimhelper<vci_param> vcisimhelper("vcisimhelper", IntTab(++nb_target), maptabp);

  soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptabp, NCPU, ++nb_target, 2, 8);



  /*************************************
     Netlits
  *************************************/
  nb_target = 0;

  for ( size_t i = 0; i<NCPU; ++i ) {
    for ( size_t irq = 0; irq < iss_t::n_irq; ++irq )
      procs[i]->p_irq[irq](signal_proc_it[i][irq]); 
    procs[i]->p_clk(signal_clk);  
    procs[i]->p_resetn(signal_resetn);  
    procs[i]->p_vci(signal_vci_proc[i]);
  }

  rom.p_clk(signal_clk);
  rom.p_resetn(signal_resetn);
  rom.p_vci(signal_vci_rom);

  ram.p_clk(signal_clk);
  ram.p_resetn(signal_resetn);
  ram.p_vci(signal_vci_ram);

  vciicu.p_clk(signal_clk);
  vciicu.p_resetn(signal_resetn);
  vciicu.p_vci(signal_vci_xicu);
  // Input IRQs
  for ( size_t i = 0; i<xicu_n_irq; ++i )
    vciicu.p_hwi[i](signal_xicu_irq[i]);
  // Output IRQs to processor
  for ( size_t i = 0; i<NCPU; ++i )
    vciicu.p_irq[i](signal_proc_it[i][0]);

  vcitty.p_clk(signal_clk);
  vcitty.p_resetn(signal_resetn);
  vcitty.p_vci(signal_vci_tty);
  vcitty.p_irq[0](signal_xicu_irq[0]); 

  vciramdisk.p_clk(signal_clk);
  vciramdisk.p_resetn(signal_resetn);
  vciramdisk.p_vci(signal_vci_ramdisk);
  
#ifdef CONFIG_FRAMEBUFFER
  soclib::caba::VciSignals<vci_param> signal_vci_framebuffer("signal_vci_framebuffer");
  vciframebuffer.p_clk(signal_clk);
  vciframebuffer.p_resetn(signal_resetn);
  vciframebuffer.p_vci(signal_vci_framebuffer);
#endif

#ifdef CONFIG_TIMER
  soclib::caba::VciSignals<vci_param> signal_vci_vcitimer("signal_vci_vcitimer");

  vcitimer.p_clk(signal_clk);
  vcitimer.p_resetn(signal_resetn);
  vcitimer.p_irq[0](signal_xicu_irq[1]);

  vcitimer.p_vci(signal_vci_vcitimer);
#endif

  vcisimhelper.p_clk(signal_clk);
  vcisimhelper.p_resetn(signal_resetn);
  vcisimhelper.p_vci(signal_vci_simhelper);
  
  
  vgmn.p_clk(signal_clk);
  vgmn.p_resetn(signal_resetn);

  for ( size_t i = 0; i<NCPU; ++i )
    vgmn.p_to_initiator[i](signal_vci_proc[i]);

  vgmn.p_to_target[nb_target](signal_vci_rom);
  vgmn.p_to_target[++nb_target](signal_vci_tty);
  vgmn.p_to_target[++nb_target](signal_vci_ram);
  vgmn.p_to_target[++nb_target](signal_vci_xicu);
  vgmn.p_to_target[++nb_target](signal_vci_ramdisk);
#ifdef CONFIG_FRAMEBUFFER
  vgmn.p_to_target[++nb_target](signal_vci_framebuffer);
#endif
#ifdef CONFIG_TIMER
  vgmn.p_to_target[++nb_target](signal_vci_vcitimer);
#endif
  vgmn.p_to_target[++nb_target](signal_vci_simhelper);

  sc_start(sc_core::sc_time(0, SC_NS));
  signal_resetn = false;

  sc_start(sc_core::sc_time(1, SC_NS));
  signal_resetn = true;


  sc_start();

  printf("End of simulation\n");

  /**********************************/
  /* Printing simulation statistics */
  /**********************************/
  // for (int i=0 ;i<NCPU ;i++) {
//     std::cout <<"***********************" <<endl;
//     procs[i]->print_stats();
//   }
//   std::cout <<"***********************" <<endl;
//   vgmn.print_stats();

//   std::cout <<"***********************" <<endl;
//   ram.print_stats();

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
