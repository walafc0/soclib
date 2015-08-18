
#include <iostream>
#include <cstdlib>

#define CONFIG_GDB_SERVER

#include "mapping_table.h"

#if defined(CONFIG_GDB_SERVER)
# include "gdbserver.h"
#endif

#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_heterogeneous_rom.h"
#include "vci_multi_tty.h"
#include "vci_locks.h"
#include "vci_xicu.h"
#include "vci_vgmn.h"

// You may set the SOCLIB_GDB environment variable to
// START_FROZEN before starting the simulator.

#include "mips32_vmem.h"

typedef soclib::common::GdbServer<soclib::common::Mips32El4kpIss> Processor;

int _main(int argc, char *argv[])
{
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;
	size_t cpu_count = 1;

	if (argc < 2)
	  return 1;
	
	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,9,32,1,1,1,8,1,1,1> vci_param;

	// Mapping table

	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00c00000);

	maptab.add(Segment("rom",   0x1fc00000, 0x00100000,  IntTab(0), false));
	maptab.add(Segment("ram" ,  0x00000000, 0x00400000 , IntTab(1), true));

	maptab.add(Segment("tty"  , 0xc0c00000,  0x10, IntTab(2), false));
	maptab.add(Segment("xicu",   0xd0c00200, 4096, IntTab(3), false));

	// Signals

	sc_core::sc_clock signal_clk("signal_clk");
	sc_core::sc_signal<bool> signal_resetn("signal_resetn");

	sc_core::sc_signal<bool> signal_cpu_it[cpu_count][Processor::n_irq]; 

	soclib::caba::VciSignals<vci_param> signal_vci_m[cpu_count];

	soclib::caba::VciSignals<vci_param> signal_vci_xicu("signal_vci_icu");
	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");

	sc_core::sc_signal<bool> signal_xicu_irq;

	// Components

	soclib::common::Loader loader(argv[1]);

	Processor::set_loader(loader);
	//	Processor::setResetAddress(0x80020000);
#if defined(CONFIG_SOCLIB_MEMCHECK)
	Processor::init(maptab, loader, "tty,xicu");
#endif

	soclib::caba::VciXcacheWrapper<vci_param, Processor> *cpu[cpu_count];

	for ( size_t i=0; i<cpu_count; ++i ) {
	  std::ostringstream o;
	  o << "cpu" << i;
	  cpu[i] = new soclib::caba::VciXcacheWrapper<vci_param, Processor>(
               o.str().c_str(), i, maptab,IntTab(i),1, 8, 4, 1, 8, 4);
	}

	soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty", NULL);
	soclib::caba::VciXicu<vci_param> vcixicu("vcixicu", maptab, IntTab(3), 2, 1, cpu_count, cpu_count);

	soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, cpu_count, 4, 2, 8);

	//	Net-List
	for ( size_t i=0; i<cpu_count; ++i ) {
	  cpu[i]->p_clk(signal_clk);
	  cpu[i]->p_resetn(signal_resetn);
	  for ( size_t irq=0; irq<Processor::n_irq; ++irq )
	    cpu[i]->p_irq[irq](signal_cpu_it[i][irq]);
	  vcixicu.p_irq[i](signal_cpu_it[i][0]);
	  cpu[i]->p_vci(signal_vci_m[i]);
	  vgmn.p_to_initiator[i](signal_vci_m[i]);
	}
  
	vcimultiram0.p_clk(signal_clk);
	vcimultiram0.p_resetn(signal_resetn);
	vcimultiram0.p_vci(signal_vci_vcimultiram0);

	vcimultiram1.p_clk(signal_clk);
	vcimultiram1.p_resetn(signal_resetn);
	vcimultiram1.p_vci(signal_vci_vcimultiram1);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_xicu_irq);

	vcixicu.p_clk(signal_clk);
	vcixicu.p_resetn(signal_resetn);
	vcixicu.p_vci(signal_vci_xicu);
	vcixicu.p_hwi[0](signal_xicu_irq);

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);
	vgmn.p_to_target[0](signal_vci_vcimultiram0);
	vgmn.p_to_target[1](signal_vci_vcimultiram1);
	vgmn.p_to_target[2](signal_vci_tty);
	vgmn.p_to_target[3](signal_vci_xicu);

	sc_core::sc_start(sc_core::sc_time(0, sc_core::SC_NS));
	signal_resetn = false;
	sc_core::sc_start(sc_core::sc_time(1, sc_core::SC_NS));
	signal_resetn = true;
	sc_core::sc_start();

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
