#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "mapping_table.h"
#include "mips32.h"
#include "iss2.h"
#include "vci_ram.h"
#include "vci_simple_ram.h"
#include "vci_multi_tty.h"
#include "vci_xicu.h"
#include "vci_simhelper.h"
#include "vci_vgmn.h"
#include "vci_logger.h"
#include "vci_dma.h"

#define USE_SIMPLE_RAM
#define USE_VCACHE
//#define USE_LOGGER_ON_L1
//#define USE_LOGGER_ON_RAM



#ifdef USE_VCACHE
#include "vci_vcache_wrapper.h"
#else
#include "vci_xcache_wrapper.h"
#endif

/////////////////////////////////////////
//   Devices mapped segments 
/////////////////////////////////////////
#define     TTY_BASE    0xd0200000
#define     TTY_SIZE    0x00000010

#define     XICU_BASE    0xd8200000
#define     XICU_SIZE    0x00001000

#define     EXIT_BASE    0xe0000000
#define     EXIT_SIZE    0x00000010

#define     DMA_BASE     0xe8000000
#define     DMA_SIZE     0x00000014

////////////////////////////////////////


//////////////////////////////////////////
//       ROM mapped segments
//////////////////////////////////////////

#define    BOOT_BASE       0xbfc00000
#define    BOOT_SIZE       0x00400000
//////////////////////////////////////////

//////////////////////////////////////////
//                RAM
//////////////////////////////////////////

#define     RAM_BASE    0x00000000
#define     RAM_SIZE    0x00400000
//////////////////////////////////////////

//////////////////////////////////////////
//    Application mapped segments
//////////////////////////////////////////

////////////////////////////////////////
//	Reserved segments
////////////////////////////////////////

#define PROC0_BASE	0x01200000
#define PROC0_SIZE	0x00000008

////////////////////////////////////////

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,4,1> vci_param;

	// Mapping table

	soclib::common::MappingTable maptab(32, IntTab(6), IntTab(6), 0xF0000000);

	maptab.add(Segment("ram" , RAM_BASE , RAM_SIZE , IntTab(0), true));

	maptab.add(Segment("boot", BOOT_BASE, BOOT_SIZE, IntTab(1), true));
	
	maptab.add(Segment("exit", EXIT_BASE, EXIT_SIZE, IntTab(2), false));

	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(3), false));
	maptab.add(Segment("xicu" , XICU_BASE , XICU_SIZE , IntTab(4), false));
	maptab.add(Segment("dma", DMA_BASE, DMA_SIZE, IntTab(5), false));
	
	// Signals

	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
   
	sc_signal<bool> signal_mips0_it0("signal_mips0_it0"); 
	sc_signal<bool> signal_mips0_it1("signal_mips0_it1"); 
	sc_signal<bool> signal_mips0_it2("signal_mips0_it2"); 
	sc_signal<bool> signal_mips0_it3("signal_mips0_it3"); 
	sc_signal<bool> signal_mips0_it4("signal_mips0_it4"); 
	sc_signal<bool> signal_mips0_it5("signal_mips0_it5");

	soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");

	soclib::caba::VciSignals<vci_param> signal_vci_exit("signal_vci_exit");
	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_xicu("signal_vci_xicu");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
	soclib::caba::VciSignals<vci_param> signal_vci_rom("signal_vci_rom");
	soclib::caba::VciSignals<vci_param> signal_vci_dmai("signal_vci_dmai");
	soclib::caba::VciSignals<vci_param> signal_vci_dmat("signal_vci_dmat");


	sc_signal<bool> signal_xicu_irq0("signal_xicu_irq0");
	sc_signal<bool> signal_xicu_irq1("signal_xicu_irq1");
	sc_signal<bool> signal_xicu_irq2("signal_xicu_irq2");
	sc_signal<bool> signal_xicu_irq3("signal_xicu_irq3");
	sc_signal<bool> signal_xicu_irq4("signal_xicu_irq4");
	sc_signal<bool> signal_xicu_irq5("signal_xicu_irq5");
	sc_signal<bool> signal_xicu_irq6("signal_xicu_irq6");

	// Components
#ifdef USE_VCACHE
	soclib::caba::VciVcacheWrapper<vci_param, soclib::common::Mips32ElIss > cache0("mips0", 0,maptab,IntTab(0),8, 8, 4, 256, 4, 4, 256, 4, 4, 4, 1000000, 0, false);
#else
	soclib::caba::VciXcacheWrapper<vci_param, soclib::common::Mips32ElIss > cache0("mips0", 0,maptab,IntTab(0),4,64,16,4,64,16);
#endif
	//soclib::common::Loader loader("kernel-soclib.bin", "soft.bin");
	soclib::common::Loader loader("test.elf");

#ifdef USE_SIMPLE_RAM
	soclib::caba::VciSimpleRam<vci_param>  vcimultiram0("ram", IntTab(0), maptab, loader);
	soclib::caba::VciSimpleRam<vci_param> rom("rom", IntTab(1), maptab, loader);
#else
	soclib::caba::VciRam<vci_param> vcimultiram0("ram", IntTab(0), maptab, loader);
	soclib::caba::VciRam<vci_param> rom("rom", IntTab(1), maptab, loader);
#endif	  

	soclib::caba::VciSimhelper<vci_param> vciexit("vciexit",	IntTab(2), maptab);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(3), maptab, "vcitty0", NULL);
	soclib::caba::VciXicu<vci_param> vcixicu("vcixicu", maptab, IntTab(4), 1 /* npti */, 3 /* nhwi */, 0 /* nwti */, 6 /* nirq */);
	soclib::caba::VciDma<vci_param> vcidma("vcidma", maptab, IntTab(1), IntTab(5), (1<<(vci_param::K-1)));

#ifdef USE_LOGGER_ON_RAM
	soclib::caba::VciLogger<vci_param> vcilogger("vcilogger", maptab); 
#endif

#ifdef USE_LOGGER_ON_L1
	soclib::caba::VciLogger<vci_param> vcilogger2("vcilogger", maptab); 
#endif
	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 2, 6, 2, 8);
	//	Net-List

	cache0.p_clk(signal_clk);

	vcimultiram0.p_clk(signal_clk);
	rom.p_clk(signal_clk);

	cache0.p_resetn(signal_resetn);

	vcimultiram0.p_resetn(signal_resetn);
	rom.p_resetn(signal_resetn);

	cache0.p_irq[0](signal_mips0_it0); 
	cache0.p_irq[1](signal_mips0_it1); 
	cache0.p_irq[2](signal_mips0_it2); 
	cache0.p_irq[3](signal_mips0_it3); 
	cache0.p_irq[4](signal_mips0_it4); 
	cache0.p_irq[5](signal_mips0_it5); 
          
	cache0.p_vci(signal_vci_m0);

	vcimultiram0.p_vci(signal_vci_vcimultiram0);

#ifdef USE_LOGGER_ON_RAM
	vcilogger.p_clk(signal_clk);
	vcilogger.p_resetn(signal_resetn);
	vcilogger.p_vci(signal_vci_vcimultiram0);
#endif

#ifdef USE_LOGGER_ON_L1
	vcilogger2.p_clk(signal_clk);
	vcilogger2.p_resetn(signal_resetn);
	vcilogger2.p_vci(signal_vci_m0);
#endif

	rom.p_vci(signal_vci_rom);
	
	vciexit.p_clk(signal_clk);
	vciexit.p_resetn(signal_resetn);
	vciexit.p_vci(signal_vci_exit);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_xicu_irq0); 

	vcixicu.p_clk(signal_clk);
	vcixicu.p_resetn(signal_resetn);
	vcixicu.p_vci(signal_vci_xicu);
	vcixicu.p_hwi[0](signal_xicu_irq0);
	vcixicu.p_hwi[1](signal_xicu_irq1);
	vcixicu.p_hwi[2](signal_xicu_irq2);
	vcixicu.p_irq[0](signal_mips0_it0);
	vcixicu.p_irq[1](signal_mips0_it1);
	vcixicu.p_irq[2](signal_mips0_it2);
	vcixicu.p_irq[3](signal_mips0_it3);
	vcixicu.p_irq[4](signal_mips0_it4);
	vcixicu.p_irq[5](signal_mips0_it5);

	vcidma.p_clk(signal_clk);
	vcidma.p_resetn(signal_resetn);
	vcidma.p_vci_target(signal_vci_dmat);
	vcidma.p_vci_initiator(signal_vci_dmai);
	vcidma.p_irq(signal_xicu_irq1);

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

	vgmn.p_to_initiator[0](signal_vci_m0);
	vgmn.p_to_initiator[1](signal_vci_dmai);

	vgmn.p_to_target[0](signal_vci_vcimultiram0);
	vgmn.p_to_target[1](signal_vci_rom);
	vgmn.p_to_target[2](signal_vci_exit);
	vgmn.p_to_target[3](signal_vci_tty);
	vgmn.p_to_target[4](signal_vci_xicu);
	vgmn.p_to_target[5](signal_vci_dmat);

	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;

	sc_start(1000000);

	std::cerr << "simulation time out" << std::endl;

	return EXIT_FAILURE;
}


int sc_main (int argc, char *argv[])
{
  try {
    _main(argc, argv);
    
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "Unknown exception occured" << std::endl;
    throw;
  }
  return 1;
}
