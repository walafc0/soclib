
//#define USE_GDB_SERVER
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

#include "niosII.h"
#   warning Using a NIOS II
typedef soclib::common::Nios2fIss iss_t;

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
#elif defined(USE_SIMHELPER)
#   include "iss2_simhelper.h"
    typedef soclib::common::Iss2Simhelper<iss_t> complete_iss_t;
#else
    typedef iss_t complete_iss_t;
#endif

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

	if ( argc < 2 ) {
        std::cerr << "Usage: " << argv[0] << " <niosII_kernel_file>" << std::endl;
        return 1;
	}

    const char *kernel = argv[1];

	// Mapping table
    soclib::common::Loader loader(kernel);

    // This puts 0x5a everywhere in the memory, this eases detection
    // of uninitialized variables bugs.
    loader.memory_default(0x5a);

    soclib::common::MappingTable maptabp(32, IntTab(8), IntTab(8), 0xf0000000);
	
	maptabp.add(Segment("rom",   0x00802000, 0x00080000, IntTab(0), true));
	maptabp.add(Segment("text",  0x60000000, 0x00100000, IntTab(0), true));
	maptabp.add(Segment("rodata",0x80000000, 0x01000000, IntTab(0), true));

	maptabp.add(Segment("tty",   0xd0200000, 0x00000040, IntTab(1), false));
	maptabp.add(Segment("mem",   0x7f000000, 0x01000000, IntTab(2), false));
	maptabp.add(Segment("xicu",  0xd2200000, 0x00001000, IntTab(3), false));
	maptabp.add(Segment("bdev0", 0xd1200000, 0x00000020, IntTab(4), false));

#if defined(USE_GDB_SERVER)
# if defined(USE_MEMCHECKER)
    soclib::common::GdbServer<
        soclib::common::IssMemchecker<
            iss_t> >::set_loader(loader);
# else /* GDB only */
    soclib::common::GdbServer<iss_t>::set_loader(loader);
# endif
#else /* MEMCHECKER only */
    soclib::common::IssMemchecker<
        iss_t>::init(maptabp, loader, "tty,xicu,bdev0");
#endif

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
	soclib::caba::VciXicu<vci_param> vciicu("vciicu", maptabp,IntTab(3), 1, xicu_n_irq, ncpu, ncpu);
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
