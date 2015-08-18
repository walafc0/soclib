/*
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
 * Copyright (c) UPMC, Lip6, Asim
 *         alain.greiner@lip6.fr
 *
 * Maintainers: alain
 */

#include <systemc>
#include <limits>

#include "gdbserver.h"
#include "vci_vgsb.h"
#include "vci_xcache_wrapper_multi.h"
#include "mips32.h"
#include "vci_vgsb.h"
#include "vci_simple_ram.h"
#include "vci_multi_tty.h"
#include "vci_timer.h"
#include "vci_icu.h"
#include "vci_gcd_coprocessor.h"
#include "vci_signals.h"
#include "vci_param.h"
#include "mapping_table.h"

#define SEG_RESET_BASE	0xBFC00000
#define SEG_RESET_SIZE	0x00001000

#define SEG_KERNEL_BASE	0x80000000
#define SEG_KERNEL_SIZE	0x00004000

#define SEG_KDATA_BASE 	0x81000000
#define SEG_KDATA_SIZE 	0x00010000

#define SEG_KSAVE_BASE 	0x82000000
#define SEG_KSAVE_SIZE 	0x00010000

#define SEG_CODE_BASE 	0x00400000
#define SEG_CODE_SIZE 	0x00004000

#define SEG_DATA_BASE 	0x10000000
#define SEG_DATA_SIZE 	0x00020000

#define SEG_STACK_BASE	0x20000000
#define SEG_STACK_SIZE	0x00100000

#define SEG_ICU_BASE  	0xF0000000
#define SEG_ICU_SIZE  	32

#define SEG_TTY_BASE  	0x90000000
#define SEG_TTY_SIZE  	16

#define SEG_TIMER_BASE  0x91000000
#define SEG_TIMER_SIZE  16

#define SEG_GCD_BASE    0x95000000
#define SEG_GCD_SIZE    16

int _main(int argc, char *argv[])
{
        using namespace sc_core;
	using namespace soclib::caba;
	using namespace soclib::common;

	// VCI fields width definition
	//	cell_size	= 4;
	// 	plen_size	= 8;
	// 	addr_size	= 32;
	// 	rerror_size	= 1;
	// 	clen_size	= 1;
	// 	rflag_size	= 1;
	// 	srcid_size	= 12;
	// 	pktid_size	= 4;
	// 	trdid_size	= 4;
	// 	wrplen_size	= 1;

	typedef VciParams<4,8,32,1,1,1,12,4,4,1> vci_param;

	///////////////////////////////////////////////////////////////////////////
	// simulation arguments : number of cycles & seed for the random generation
	///////////////////////////////////////////////////////////////////////////
        int ncycles = std::numeric_limits<int>::max();
        char soft_name[256] = "soft/bin.soft";
        if (argc > 1) ncycles = atoi(argv[1]) ;
        if (argc > 2) strcpy(soft_name, argv[2]) ;

	//////////////////////////////////////////////////////////////////////////
	// Mapping Table
	//////////////////////////////////////////////////////////////////////////
	MappingTable maptab(32, IntTab(8), IntTab(8), 0xFF000000);

	maptab.add(Segment("seg_reset" , SEG_RESET_BASE , SEG_RESET_SIZE , IntTab(0), true));

	maptab.add(Segment("seg_kernel", SEG_KERNEL_BASE, SEG_KERNEL_SIZE, IntTab(1), true));
	maptab.add(Segment("seg_kdata" , SEG_KDATA_BASE , SEG_KDATA_SIZE , IntTab(1), false));
	maptab.add(Segment("seg_ksave" , SEG_KSAVE_BASE , SEG_KSAVE_SIZE , IntTab(1), true));
	maptab.add(Segment("seg_code"  , SEG_CODE_BASE  , SEG_CODE_SIZE  , IntTab(1), true));
	maptab.add(Segment("seg_data"  , SEG_DATA_BASE  , SEG_DATA_SIZE  , IntTab(1), true));
	maptab.add(Segment("seg_stack" , SEG_STACK_BASE , SEG_STACK_SIZE , IntTab(1), true));

	maptab.add(Segment("seg_tty"   , SEG_TTY_BASE   , SEG_TTY_SIZE   , IntTab(2), false));
	maptab.add(Segment("seg_gcd"   , SEG_GCD_BASE   , SEG_GCD_SIZE   , IntTab(3), false));
	maptab.add(Segment("seg_icu"   , SEG_ICU_BASE   , SEG_ICU_SIZE   , IntTab(4), false));
	maptab.add(Segment("seg_timer" , SEG_TIMER_BASE , SEG_TIMER_SIZE , IntTab(5), false));

	std::cout << std::endl << maptab << std::endl;

	//////////////////////////////////////////////////////////////////////////
        // Signals
	//////////////////////////////////////////////////////////////////////////
        sc_clock               		signal_clk("signal_clk", sc_time( 1, SC_NS ), 0.5 );
        sc_signal<bool> 		signal_resetn("signal_resetn");
        VciSignals<vci_param> 		signal_vci_m0("signal_vci_m0");
        VciSignals<vci_param> 		signal_vci_t0("signal_vci_t0");
        VciSignals<vci_param> 		signal_vci_t1("signal_vci_t1");
        VciSignals<vci_param> 		signal_vci_t2("signal_vci_t2");
        VciSignals<vci_param> 		signal_vci_t3("signal_vci_t3");
        VciSignals<vci_param> 		signal_vci_t4("signal_vci_t4");
        VciSignals<vci_param> 		signal_vci_t5("signal_vci_t5");
        sc_signal<bool> 		signal_false("signal_false");
        sc_signal<bool> 		signal_dummy("signal_dummy");
        sc_signal<bool> 		signal_irq_tty("signal_irq_tty");
        sc_signal<bool> 		signal_irq_timer("signal_irq_timer");
        sc_signal<bool> 		signal_irq_proc("signal_irq_proc");

	//////////////////////////////////////////////////////////////////////////
	// Components
	//////////////////////////////////////////////////////////////////////////

	Loader	loader(soft_name);

	VciXcacheWrapperMulti<vci_param, GdbServer<Mips32ElIss> >
		proc("proc", 0,maptab,IntTab(0),4,128,8,4,128,8,4,8);
	VciSimpleRam<vci_param>
		rom("rom", IntTab(0), maptab, loader);
	VciSimpleRam<vci_param>
		ram("ram", IntTab(1), maptab, loader);
	VciMultiTty<vci_param>
		tty("tty", IntTab(2), maptab, "term", NULL);
	VciGcdCoprocessor<vci_param>
		gcd("gcd", IntTab(3), maptab);
	VciIcu<vci_param>
		icu("icu", IntTab(4), maptab, 2);
	VciTimer<vci_param>
		timer("timer", IntTab(5), maptab, 1);
	VciVgsb<vci_param>		
		bus("bus", maptab, 1, 6);

	//////////////////////////////////////////////////////////////////////////
	// Net-List
	//////////////////////////////////////////////////////////////////////////
	proc.p_clk(signal_clk);
	proc.p_resetn(signal_resetn);
	proc.p_vci(signal_vci_m0);
	proc.p_irq[0](signal_irq_proc);
	proc.p_irq[1](signal_false);
	proc.p_irq[2](signal_false);
	proc.p_irq[3](signal_false);
	proc.p_irq[4](signal_false);
	proc.p_irq[5](signal_false);

	rom.p_clk(signal_clk);
	rom.p_resetn(signal_resetn);
	rom.p_vci(signal_vci_t0);

	ram.p_clk(signal_clk);
	ram.p_resetn(signal_resetn);
	ram.p_vci(signal_vci_t1);

	tty.p_clk(signal_clk);
	tty.p_resetn(signal_resetn);
	tty.p_vci(signal_vci_t2);
	tty.p_irq[0](signal_irq_tty);

	gcd.p_clk(signal_clk);
	gcd.p_resetn(signal_resetn);
	gcd.p_vci(signal_vci_t3);

	icu.p_clk(signal_clk);
	icu.p_resetn(signal_resetn);
	icu.p_vci(signal_vci_t4);
	icu.p_irq(signal_irq_proc);
	icu.p_irq_in[0](signal_irq_timer);
	icu.p_irq_in[1](signal_irq_tty);

	timer.p_clk(signal_clk);
	timer.p_resetn(signal_resetn);
	timer.p_vci(signal_vci_t5);
	timer.p_irq[0](signal_irq_timer);

	bus.p_clk(signal_clk);
	bus.p_resetn(signal_resetn);
	bus.p_to_initiator[0](signal_vci_m0);
	bus.p_to_target[0](signal_vci_t0);
	bus.p_to_target[1](signal_vci_t1);
	bus.p_to_target[2](signal_vci_t2);
	bus.p_to_target[3](signal_vci_t3);
	bus.p_to_target[4](signal_vci_t4);
	bus.p_to_target[5](signal_vci_t5);

	//////////////////////////////////////////////////////////////////////////
	// simulation
	//////////////////////////////////////////////////////////////////////////

	sc_start(0);

        signal_false = false;

        signal_resetn = false;
        sc_start( sc_time( 1, SC_NS ) ) ;

        signal_resetn = true;
	for ( int n=1 ; n<ncycles ; n++ ) 
        { 
/*
            std::cout << "******************************* cycle = " << std::dec << n 
                      << " ******************************************" << std::endl;
            proc.printTrace(0x1);
            bus.printTrace();
            rom.printTrace();
            ram.printTrace();
            std::cout << " proc.cmdval  = " << signal_vci_m0.cmdval << std::endl;
            std::cout << " proc.cmdack  = " << signal_vci_m0.cmdack << std::endl;
            std::cout << " proc.address = " << signal_vci_m0.address << std::endl;
            std::cout << " proc.trdid   = " << signal_vci_m0.trdid << std::endl;
            std::cout << " proc.wdata   = " << signal_vci_m0.wdata << std::endl;
            std::cout << " proc.eop     = " << signal_vci_m0.eop << std::endl;
            std::cout << " proc.len     = " << signal_vci_m0.plen << std::endl;
            std::cout << " proc.rspval  = " << signal_vci_m0.rspval << std::endl;
            std::cout << " proc.rspack  = " << signal_vci_m0.rspack << std::endl;
            std::cout << " proc.reop    = " << signal_vci_m0.reop << std::endl;
            std::cout << " proc.rdata   = " << signal_vci_m0.rdata << std::endl;
            std::cout << " proc.rerror  = " << signal_vci_m0.rerror << std::endl;
            std::cout << " rom.cmdval   = " << signal_vci_t0.cmdval << std::endl;
            std::cout << " rom.cmdack   = " << signal_vci_t0.cmdack << std::endl;
            std::cout << " rom.rspval   = " << signal_vci_t0.rspval << std::endl;
            std::cout << " rom.rspack   = " << signal_vci_t0.rspack << std::endl;
            std::cout << " rom.rdata    = " << signal_vci_t0.rdata << std::endl;
            std::cout << " rom.rtrdid   = " << signal_vci_t0.rtrdid << std::endl;
            std::cout << " rom.rerror   = " << signal_vci_t0.rerror << std::endl;
            std::cout << " rom.reop     = " << signal_vci_t0.reop << std::endl;
*/
            sc_start( sc_time( 1 , SC_NS ) ) ;
	}

        return(0);

} // end _main

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
