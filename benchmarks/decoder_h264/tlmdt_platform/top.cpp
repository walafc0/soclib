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
#include <sys/timeb.h>
#include <iostream>
#include <cstdlib>
#include <cstdarg>

#include "cpu_type.h"
#include "soclib_addresses.h"

#include "gdbserver.h"
#include "iss_memchecker.h"

#include "mapping_table.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"
#include "vci_xcache_wrapper.h"
#include "iss2_simhelper.h"
#include "vci_icu.h"
#include "vci_blackhole.h"	// required for TLMDT implementation
#include "vci_timer.h"
#include "vci_simhelper.h"

#ifdef CONFIG_FRAMEBUFFER
#	include "vci_framebuffer.h"
#	warning FRAMEBUFFER added on the platform
#endif

#ifdef CONFIG_DUMP_STATS
#       include "tlmdt_cache_statdumper.h"
#       warning TLM-DT stat dumper added on the platform
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
//typedef soclib::common::Iss2Simhelper<iss_t> complete_iss_t;
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
  const size_t icu_n_irq = 2;
  int nb_target = 0;
  struct timeb initial, final;

  // Define VCI parameters
  typedef soclib::tlmdt::VciParams<uint32_t, uint32_t> vci_param;

  const char *kernel = default_kernel;
  if ( argc > 1 ) {
    kernel = argv[1];
  }

  /*************************************
     Mapping Table
  *************************************/
  soclib::common::Loader       loader (kernel,"block0.iso@0x68200000:D");
  soclib::common::MappingTable maptabp(32, IntTab(8), IntTab(8), CACHABILITY_MASK);
  
#if defined(CPU_ppc)
  maptabp.add(Segment("boot",  SEG_BOOT_ADDR, SEG_BOOT_SIZE, IntTab(nb_target), true));
#elif defined(CPU_arm)
  maptabp.add(Segment("boot",  SEG_BOOT_ADDR, SEG_BOOT_SIZE, IntTab(nb_target), true));
#elif defined(CPU_mips32el)
  maptabp.add(Segment("boot",  SEG_BOOT_ADDR, SEG_BOOT_SIZE, IntTab(nb_target), true));
#endif
  
  maptabp.add(Segment("text",  SEG_ROM_ADDR, SEG_ROM_SIZE, IntTab(nb_target), false));
  
  maptabp.add(Segment("tty",     SEG_TTY_ADDR,  SEG_TTY_SIZE,  IntTab(++nb_target), false));
  maptabp.add(Segment("mem",   SEG_TEXT_ADDR, SEG_TEXT_SIZE, IntTab(++nb_target), false));
  maptabp.add(Segment("icu0",    SEG_ICU_ADDR,  SEG_ICU_SIZE,  IntTab(++nb_target), false));  
  maptabp.add(Segment("ramdisk", SEG_DISK_ADDR, SEG_DISK_SIZE, IntTab(++nb_target), false));
#ifdef CONFIG_FRAMEBUFFER
  maptabp.add(Segment("fbuffer", SEG_BUFF_ADDR, SEG_BUFF_SIZE, IntTab(++nb_target), false));
#endif
  maptabp.add(Segment("timer",   SEG_TIMER_ADDR,SEG_TIMER_SIZE,IntTab(++nb_target), false));
  maptabp.add(Segment("simhelper",SEG_SIMHELPER_ADDR,SEG_SIMHELPER_SIZE,IntTab(++nb_target), false));


#if defined(USE_GDB_SERVER) && defined(USE_MEMCHECKER)
  soclib::common::GdbServer<soclib::common::IssMemchecker<iss_t> >::set_loader(loader);
#endif
#if defined(USE_MEMCHECKER)
  soclib::common::IssMemchecker<iss_t>::init(maptabp, loader, "tty,icu,fbuffer,timer");
#elif defined(USE_GDB_SERVER)
  soclib::common::GdbServer<iss_t>::set_loader(loader);
#endif

  /*************************************
     Components
  *************************************/
  nb_target = 0;
  soclib::tlmdt::VciXcacheWrapper<vci_param, complete_iss_t > *procs[NCPU];
  for ( size_t i = 0; i<NCPU; ++i ) {
    std::ostringstream o;
    o << "proc" << i;
    procs[i] = new soclib::tlmdt::VciXcacheWrapper<vci_param, complete_iss_t >
      (o.str().c_str(), i, maptabp, IntTab(i), 4, 64, CACHE_LINE_SIZE, 4, 64, CACHE_LINE_SIZE);
      //(o.str().c_str(), i, maptabp, IntTab(i), 1,8,4, 1,8,4);
      //(o.str().c_str(), i, IntTab(i), maptabp, 4,64,16, 4,64,16, 500 * UNIT_TIME);
  }

  soclib::tlmdt::VciRam<vci_param>      rom("rom", IntTab(nb_target)  , maptabp, loader);
  soclib::tlmdt::VciMultiTty<vci_param> vcitty("vcitty",IntTab(++nb_target), maptabp, "vcitty0", NULL);
  soclib::tlmdt::VciRam<vci_param>      ram("ram", IntTab(++nb_target), maptabp, loader);
  soclib::tlmdt::VciIcu<vci_param>      vciicu("vciicu", IntTab(++nb_target), maptabp, icu_n_irq);
  soclib::tlmdt::VciRam<vci_param>      vciramdisk("vciramdisk", IntTab(++nb_target), maptabp, loader);
#ifdef CONFIG_FRAMEBUFFER
#	ifdef VIDEO_TYPE_cif
  soclib::tlmdt::VciFrameBuffer<vci_param> vciframebuffer("vciframebuffer", IntTab(++nb_target), maptabp, 352, 288, 420);
#	warning Using CIF video size
#	elif defined(VIDEO_TYPE_qcif)
  soclib::tlmdt::VciFrameBuffer<vci_param> vciframebuffer("vciframebuffer", IntTab(++nb_target), maptabp, 176, 144, 420);
#	warning Using QCIF video size
#	else
#	error No video type (CIF or QCIF) configuration defined
#	endif
#endif
  
  soclib::tlmdt::VciTimer<vci_param> vcitimer("vcittimer", IntTab(++nb_target), maptabp, 1);
  soclib::tlmdt::VciSimhelper<vci_param> vcisimhelper("vcicimhelper", IntTab(++nb_target), maptabp);

  soclib::tlmdt::VciVgmn vgmn("vgmn",maptabp, NCPU, ++nb_target, 2, 8);
  

  /*************************************
     Netlits
  *************************************/
  nb_target = 0;

   // Proc
  for ( size_t i = 0; i<NCPU; ++i )
    procs[i]->p_vci(*vgmn.p_to_initiator[i]);

  // ROM
  (*vgmn.p_to_target[nb_target])(rom.p_vci);
  
  // TTY
  (*vcitty.p_irq[0])(*vciicu.p_irq_in[1]);
  (*vgmn.p_to_target[++nb_target])(vcitty.p_vci);

  // RAM
  (*vgmn.p_to_target[++nb_target])(ram.p_vci);

  // ICU
  vciicu.p_irq(*procs[0]->p_irq[0]);
  (*vgmn.p_to_target[++nb_target])(vciicu.p_vci);

  // RAMDISK
  (*vgmn.p_to_target[++nb_target])(vciramdisk.p_vci);

  // Framebuffer
#ifdef CONFIG_FRAMEBUFFER
  (*vgmn.p_to_target[++nb_target])(vciframebuffer.p_vci);
#endif
  
  // TIMER
  (*vcitimer.p_irq[0])(*vciicu.p_irq_in[0]);
  (*vgmn.p_to_target[++nb_target])(vcitimer.p_vci);
    
  // SIMHELPER
  (*vgmn.p_to_target[++nb_target])(vcisimhelper.p_vci);
  

  /////////////////////////////////////////////////////////////////////////////
  // VciBlackhole Initiator
  /////////////////////////////////////////////////////////////////////////////
  // !!!!!!! TO BE MODIFIED IF PROCESSOR IS NOT A MIPS, ADAP DEFINES  
  soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > *fake_initiator[NCPU];
  
  // Because only processor 0 is connected to ICU output IRQ : this might need to be changed
  fake_initiator[0] = new soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> >("fake0", soclib::common::Mips32ElIss::n_irq-1);
  for(int irq=0; irq<soclib::common::Mips32ElIss::n_irq-1; irq++)
    (*fake_initiator[0]->p_socket[irq])(*procs[0]->p_irq[irq+1]);
  
  for (int i=1 ; i < NCPU ; i++) {
    std::ostringstream fake_name;
    fake_name << "fake" << i;
    fake_initiator[i] = new soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> >((fake_name.str()).c_str(), soclib::common::Mips32ElIss::n_irq);
    
    for(int irq=0; irq<soclib::common::Mips32ElIss::n_irq; irq++){
      (*fake_initiator[i]->p_socket[irq])(*procs[i]->p_irq[irq]);
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // VciBlackhole Target Tagged
  /////////////////////////////////////////////////////////////////////////////
  // soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> > *fake_target_tagged;
  
//   fake_target_tagged = new soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> >("fake_target_tagged", NCPU);
  
//   for(int i=0; i<NCPU; i++){
//     (*vcitty.p_irq[i])(*fake_target_tagged->p_socket[i]);
//   }
  

  /////////////////////////////////////////////////////////////////////////////
  // SIMULATION
  /////////////////////////////////////////////////////////////////////////////
  ftime(&initial);
  std::cout <<"Starts simulation now" <<endl;

#ifdef CONFIG_DUMP_STATS
  soclib::tlmdt::CacheStatdumper<soclib::tlmdt::VciXcacheWrapper<vci_param, complete_iss_t> > stats(procs[0]);
#endif

  sc_core::sc_start();  // start the simulation
  ftime(&final);

  std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl << std::endl;

  /*
  for (int i=0 ; i<n_rams; i++) {
    ram[i]->print_stats();
  }
  vcitty.print_stats();
  */
  
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
