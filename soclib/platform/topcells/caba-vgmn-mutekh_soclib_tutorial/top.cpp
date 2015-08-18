
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
#include "vci_xicu.h"
#include "vci_block_device.h"

#include "arm.h"

typedef soclib::common::ArmIss iss_t;

#include "gdbserver.h"
#include "iss_memchecker.h"

typedef soclib::common::GdbServer<soclib::common::IssMemchecker<soclib::common::ArmIss> > complete_iss_t;

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;
	const size_t ncpu = 4;
	const size_t xicu_n_irq = 2;

	// Define VCI parameters
	typedef soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1> vci_param;

	if ( argc < 3 ) {
        std::cerr << "Usage: " << argv[0] << " <arm_kernel_file> debug" << std::endl;
        return 1;
	}

    const char *kernel = argv[1];

    uint32_t debug = atoi(argv[2]);

	// Mapping table
    soclib::common::Loader loader(kernel, "platform.dtb@0xe0000000:RO");

    // This puts 0x5a everywhere in the memory, this eases detection
    // of uninitialized variables bugs.
    loader.memory_default(0x5a);

    soclib::common::MappingTable maptabp(32, IntTab(8), IntTab(8), 0xf0000000);
	
	maptabp.add(Segment("boot",  0x00000000, 0x00000200, IntTab(0), true));
	maptabp.add(Segment("text",  0x60000000, 0x00100000, IntTab(0), true));
	maptabp.add(Segment("rodata",0x80000000, 0x01000000, IntTab(0), true));
	maptabp.add(Segment("fdt",   0xe0000000, 0x00001000, IntTab(0), false)); // device tree

	maptabp.add(Segment("tty",   0xd0200000, 0x00000040, IntTab(1), false));
	maptabp.add(Segment("mem",   0x7f000000, 0x01000000, IntTab(2), false));
	maptabp.add(Segment("xicu",  0xd2200000, 0x00001000, IntTab(3), false));
	maptabp.add(Segment("bdev0", 0xd1200000, 0x00000020, IntTab(4), false));

    iss_t::setBoostrapCpuId(0);      /* Only processor 0 starts execution on reset */

    soclib::common::GdbServer<
        soclib::common::IssMemchecker<
            iss_t> >::set_loader(loader);

    soclib::common::IssMemchecker<
      iss_t>::init(maptabp, loader, "tty,xicu,bdev0");

	// Signals
	sc_clock	signal_clk("clk");
	sc_signal<bool> signal_resetn("resetn");

	sc_signal<bool> signal_proc_it[ncpu][iss_t::n_irq]; 
   
	soclib::caba::VciSignals<vci_param> signal_vci_proc[ncpu];

	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_xicu("signal_vci_xicu");

	soclib::caba::VciSignals<vci_param> signal_vci_bdi[1];
	soclib::caba::VciSignals<vci_param> signal_vci_bdt[1];

	soclib::caba::VciSignals<vci_param> signal_vci_ram("vci_tgt_ram");
	soclib::caba::VciSignals<vci_param> signal_vci_rom("vci_tgt_rom");

	sc_signal<bool> signal_xicu_irq[xicu_n_irq];

	soclib::caba::VciXcacheWrapper<vci_param, complete_iss_t > *procs[ncpu];

    for ( size_t i = 0; i<ncpu; ++i ) {
        std::ostringstream o;
        o << "proc" << i;
        procs[i] = new soclib::caba::VciXcacheWrapper<vci_param, complete_iss_t >
            (o.str().c_str(), i, maptabp, IntTab(i),4,64,16, 4,64,16);
    }

	soclib::caba::VciRom<vci_param> rom("rom", IntTab(0), maptabp, loader);
#ifdef USE_LOG_CONSOLE
	soclib::caba::VciLogConsole<vci_param> vcitty("vcitty",	IntTab(1), maptabp);
#else
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(1), maptabp, "vcitty0", NULL);
#endif
	soclib::caba::VciRam<vci_param> ram("ram", IntTab(2), maptabp, loader);
	soclib::caba::VciXicu<vci_param> vciicu("vcixicu", maptabp,IntTab(3), 1, xicu_n_irq, ncpu, ncpu);
	soclib::caba::VciBlockDevice<vci_param> vcibd0("vcibd0", maptabp, IntTab(ncpu), IntTab(4), "block0.iso", 2048);
	
	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptabp, ncpu+1, 5, 2, 8);

    for ( size_t i = 0; i<ncpu; ++i ) {
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
    for ( size_t i = 0; i<ncpu; ++i )
        vciicu.p_irq[i](signal_proc_it[i][0]);

	vcibd0.p_clk(signal_clk);
	vcibd0.p_resetn(signal_resetn);
	vcibd0.p_irq(signal_xicu_irq[1]); 
	vcibd0.p_vci_target(signal_vci_bdt[0]);
	vcibd0.p_vci_initiator(signal_vci_bdi[0]);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
#ifndef USE_LOG_CONSOLE
	vcitty.p_irq[0](signal_xicu_irq[0]); 
#endif

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

    for ( size_t i = 0; i<ncpu; ++i )
        vgmn.p_to_initiator[i](signal_vci_proc[i]);
	vgmn.p_to_initiator[ncpu](signal_vci_bdi[0]);

	vgmn.p_to_target[0](signal_vci_rom);
	vgmn.p_to_target[1](signal_vci_tty);
	vgmn.p_to_target[2](signal_vci_ram);
	vgmn.p_to_target[3](signal_vci_xicu);
	vgmn.p_to_target[4](signal_vci_bdt[0]);

	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;

	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;

    for ( size_t n=1 ; 10000000 ; n++ )
    {
        sc_start( sc_time( 1 , SC_NS ) ) ;
        if ( debug )
        {
            std::cout << "***************** cycle " << std::dec << n
                << " ***********************" << std::endl;
            procs[0]->print_trace();
            signal_vci_proc[0].print_trace("signal_proc_0");
            signal_vci_rom.print_trace("signal_rom");
            signal_vci_ram.print_trace("signal_ram");
        }
    }
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
