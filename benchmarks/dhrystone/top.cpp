/*
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
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2009
 *
 * Maintainers: nipo
 */

#include <iostream>
#include <cstdlib>

#define USE_CROSSBAR

#if defined(USE_CROSSBAR)
# include "vci_simple_crossbar.h"
#else
# include "vci_vgmn.h"
#endif

#if 0
# include "gdbserver.h"
# define gdb(x) soclib::common::GdbServer<x >
#else
# define gdb(x) x
#endif

#include "mapping_table.h"
#include "mips32.h"
#include "ppc405.h"
#include "arm.h"
#include "lm32.h"
#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_simhelper.h"

#include "segmentation.h"

typedef enum {
	MIPSEL,
	MIPSEB,
	POWERPC,
	ARM,
	LM32,
} arch_t;

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,9,32,1,1,1,8,1,1,1> vci_param;

	::setenv("SOCLIB_TTY", "TERM", 1);
	soclib::common::Loader loader("soft/bin.soft");

	// Mapping table

	const char *arch_string = "Unknown";
	arch_t arch;
	if ( loader.get_symbol_by_name("mips_interrupt_entry") ) {
		if ( loader.get_symbol_by_name("mips_is_little") ) {
			arch = MIPSEL;
			arch_string = "mipsel";
		} else {
			arch = MIPSEB;
			arch_string = "mipseb";
		}
	} else if ( loader.get_symbol_by_name("ppc_boot") ) {
		arch = POWERPC;
		arch_string = "powerpc";
	} else if ( loader.get_symbol_by_name("arm_irq") ) {
		arch = ARM;
		arch_string = "arm";
	} else if ( loader.get_symbol_by_name("_exit_lm32") ) {
		arch = LM32;
		arch_string = "lm32";
	} else
		throw soclib::exception::RunTimeError("Incorrect architecture");

	std::cout << "Binary file is " << arch_string << std::endl;

	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	switch ( arch ) {
	case MIPSEL:
	case MIPSEB:
		maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
		maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
		break;
	case POWERPC:
		maptab.add(Segment("ppc_boot", PPC_BOOT_BASE, PPC_BOOT_SIZE, IntTab(0), false));
		maptab.add(Segment("ppc_special" , PPC_SPECIAL_BASE , PPC_SPECIAL_SIZE , IntTab(0), true));
		break;
	case ARM:
		maptab.add(Segment("arm_boot", ARM_BOOT_BASE, ARM_BOOT_SIZE, IntTab(0), true));
		break;
	case LM32:
		maptab.add(Segment("lm32_boot", LM32_BOOT_BASE, LM32_BOOT_SIZE, IntTab(0), true));
		break;
	}
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
  
	maptab.add(Segment("loc0" , LOC0_BASE , LOC0_SIZE , IntTab(1), true));
  
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));

	maptab.add(Segment("simhelper", SIMHELPER_BASE  , SIMHELPER_SIZE  , IntTab(3), false));

	// Signals

	sc_clock		signal_clk("signal_clk");
	sc_signal<bool> signal_resetn("signal_resetn");
   
	sc_signal<bool> signal_cpu0_it0("signal_cpu0_it0"); 
	sc_signal<bool> signal_cpu0_it1("signal_cpu0_it1"); 
	sc_signal<bool> signal_cpu0_it2("signal_cpu0_it2"); 
	sc_signal<bool> signal_cpu0_it3("signal_cpu0_it3"); 
	sc_signal<bool> signal_cpu0_it4("signal_cpu0_it4"); 
	sc_signal<bool> signal_cpu0_it5("signal_cpu0_it5");
	sc_signal<bool> uncon_it       ("uncon_it");

	soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");

	soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
	soclib::caba::VciSignals<vci_param> signal_vci_simhelper("signal_vci_simhelper");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
	soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram1("signal_vci_vcimultiram1");

	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 

	// Components


	soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::Mips32ElIss)> *mipsel0;
	soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::Mips32EbIss)> *mipseb0;
	soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::Ppc405Iss)> *ppc0;
	soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::ArmIss)> *arm0;
	soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::LM32Iss<true>)> *lm32_0;

	soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::caba::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);
	soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", NULL);
	soclib::caba::VciSimhelper<vci_param> vcisimhelper("vcisimhelper",	IntTab(3), maptab);

#if defined(USE_CROSSBAR)
	soclib::caba::VciSimpleCrossbar<vci_param> interconnect("interconnect", maptab, 1, 4);
#else
	soclib::caba::VciVgmn<vci_param> interconnect("interconnect",maptab, 1, 4, 1, 8);
#endif

	//	Net-List
 
	vcimultiram0.p_clk(signal_clk);
	vcimultiram1.p_clk(signal_clk);

	vcimultiram0.p_resetn(signal_resetn);
	vcimultiram1.p_resetn(signal_resetn);

	switch ( arch ) {
	case MIPSEB:
		mipseb0 = new soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::Mips32EbIss)>("mipseb0", 0, maptab,IntTab(0),4, 512,16,4, 128,16);
		mipseb0->p_clk(signal_clk);  
		mipseb0->p_resetn(signal_resetn);  
		mipseb0->p_irq[0](signal_cpu0_it0); 
		mipseb0->p_irq[1](signal_cpu0_it1); 
		mipseb0->p_irq[2](signal_cpu0_it2); 
		mipseb0->p_irq[3](signal_cpu0_it3); 
		mipseb0->p_irq[4](signal_cpu0_it4); 
		mipseb0->p_irq[5](signal_cpu0_it5); 
		mipseb0->p_vci(signal_vci_m0);
		break;
	case MIPSEL:
		mipsel0 = new soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::Mips32ElIss)>("mipsel0", 0, maptab,IntTab(0),4, 512,16,4, 128,16);
		mipsel0->p_clk(signal_clk);  
		mipsel0->p_resetn(signal_resetn);  
		mipsel0->p_irq[0](signal_cpu0_it0); 
		mipsel0->p_irq[1](signal_cpu0_it1); 
		mipsel0->p_irq[2](signal_cpu0_it2); 
		mipsel0->p_irq[3](signal_cpu0_it3); 
		mipsel0->p_irq[4](signal_cpu0_it4); 
		mipsel0->p_irq[5](signal_cpu0_it5); 
		mipsel0->p_vci(signal_vci_m0);
		break;
	case POWERPC:
		ppc0 = new soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::Ppc405Iss)>("ppc0", 0, maptab,IntTab(0),4, 512,16,4, 128,16);
		ppc0->p_clk(signal_clk);  
		ppc0->p_resetn(signal_resetn);  
		ppc0->p_irq[0](signal_cpu0_it0); 
		ppc0->p_irq[1](signal_cpu0_it1); 
		ppc0->p_vci(signal_vci_m0);
		break;
	case ARM:
		arm0 = new soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::ArmIss)>("arm0", 0, maptab,IntTab(0),4, 512,16,4, 128,16);
		arm0->p_clk(signal_clk);  
		arm0->p_resetn(signal_resetn);  
		arm0->p_irq[0](signal_cpu0_it0); 
		arm0->p_vci(signal_vci_m0);
		break;
	case LM32:
		lm32_0 = new soclib::caba::VciXcacheWrapper<vci_param, gdb(soclib::common::LM32Iss<true>)>("lm32_0", 0, maptab,IntTab(0),4, 512,16,4, 128,16);
		lm32_0->p_clk(signal_clk);  
		lm32_0->p_resetn(signal_resetn);  
		lm32_0->p_irq[0](signal_cpu0_it0); 
		lm32_0->p_vci(signal_vci_m0);
        for (int i=1; i<32; i++)
            lm32_0->p_irq[i] (uncon_it);
		break;
	}

	vcimultiram0.p_vci(signal_vci_vcimultiram0);
	vcimultiram1.p_vci(signal_vci_vcimultiram1);

	vcitty.p_clk(signal_clk);
	vcitty.p_resetn(signal_resetn);
	vcitty.p_vci(signal_vci_tty);
	vcitty.p_irq[0](signal_tty_irq0); 

	vcisimhelper.p_clk(signal_clk);
	vcisimhelper.p_resetn(signal_resetn);
	vcisimhelper.p_vci(signal_vci_simhelper);

	interconnect.p_clk(signal_clk);
	interconnect.p_resetn(signal_resetn);

	interconnect.p_to_initiator[0](signal_vci_m0);
	interconnect.p_to_target[0](signal_vci_vcimultiram0);
	interconnect.p_to_target[1](signal_vci_vcimultiram1);
	interconnect.p_to_target[2](signal_vci_tty);
	interconnect.p_to_target[3](signal_vci_simhelper);


	sc_start(sc_time(0, SC_NS));
	signal_resetn = false;

	sc_start(sc_time(1, SC_NS));
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
