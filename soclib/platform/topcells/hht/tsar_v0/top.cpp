/* -*- c++ -*-
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
 *         Etienne Le Grand <etilegr@hotmail.com>, 2009
 */

#include <systemc>
#include <sys/time.h>
#include <iostream>
#include <cstdlib>
#include <cstdarg>

#include "vci_cori_config_initiator.h"
#include "vci_ciro_config_initiator.h"
#include "vci_hht_cori_bridge.h"
#include "vci_hht_ciro_bridge.h"

#include "mapping_table.h"
#include "mips.h"
#include "ississ2.h"
#include "iss_simhelper.h"
#include "vci_simple_ram.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"
#include "vci_mem_cache.h"
#include "vci_local_crossbar.h"
#include "vci_dspinplus_network.h"
#include "vci_cc_xcache_wrapper.h"


#ifdef USE_GDB_SERVER
#include "iss/gdbserver.h"
#endif

#include "segmentation.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define VCI & HHT parameters
	typedef soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1> vci_param;
	typedef soclib::caba::HhtParam<64,32> hht_param;
	typedef soclib::common::IssIss2<soclib::common::IssSimhelper<soclib::common::MipsElIss> > proc_iss;

	// Mapping table
	soclib::common::MappingTable maptabp(32, IntTab(2,10), IntTab(2,3), 0x00C00000);

	maptabp.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(2,1), true));
	maptabp.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(2,1), true));
	maptabp.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(3,1), false));

	maptabp.add(Segment("mc_r0" , MC0_R_BASE , MC0_R_SIZE , IntTab(0,0), false, true, IntTab(0,0)));
	maptabp.add(Segment("mc_m0" , MC0_M_BASE , MC0_M_SIZE , IntTab(0,0), true ));
	maptabp.add(Segment("mc_r1" , MC1_R_BASE , MC1_R_SIZE , IntTab(1,0), false, true, IntTab(1,0)));
	maptabp.add(Segment("mc_m1" , MC1_M_BASE , MC1_M_SIZE , IntTab(1,0), true ));
	maptabp.add(Segment("mc_r2" , MC2_R_BASE , MC2_R_SIZE , IntTab(2,0), false, true, IntTab(2,0)));
	maptabp.add(Segment("mc_m2" , MC2_M_BASE , MC2_M_SIZE , IntTab(2,0), true ));
	maptabp.add(Segment("mc_r3" , MC3_R_BASE , MC3_R_SIZE , IntTab(3,0), false, true, IntTab(3,0)));
	maptabp.add(Segment("mc_m3" , MC3_M_BASE , MC3_M_SIZE , IntTab(3,0), true ));

	std::cout << maptabp << std::endl;

	soclib::common::MappingTable maptabc(32, IntTab(2,10), IntTab(2,3), 0x00C00000);

	maptabc.add(Segment("proc0" , PROC0_BASE , PROC0_SIZE , IntTab(0,0), false, true, IntTab(0,0)));
	maptabc.add(Segment("proc1" , PROC1_BASE , PROC1_SIZE , IntTab(0,1), false, true, IntTab(0,1)));
	maptabc.add(Segment("proc2" , PROC2_BASE , PROC2_SIZE , IntTab(0,2), false, true, IntTab(0,2)));
	maptabc.add(Segment("proc3" , PROC3_BASE , PROC3_SIZE , IntTab(0,3), false, true, IntTab(0,3)));
	maptabc.add(Segment("proc4" , PROC4_BASE , PROC4_SIZE , IntTab(1,0), false, true, IntTab(1,0)));
	maptabc.add(Segment("proc5" , PROC5_BASE , PROC5_SIZE , IntTab(1,1), false, true, IntTab(1,1)));
	maptabc.add(Segment("proc6" , PROC6_BASE , PROC6_SIZE , IntTab(1,2), false, true, IntTab(1,2)));
	maptabc.add(Segment("proc7" , PROC7_BASE , PROC7_SIZE , IntTab(1,3), false, true, IntTab(1,3)));
	maptabc.add(Segment("proc8" , PROC8_BASE , PROC8_SIZE , IntTab(2,0), false, true, IntTab(2,0)));
	maptabc.add(Segment("proc9" , PROC9_BASE , PROC9_SIZE , IntTab(2,1), false, true, IntTab(2,1)));
	maptabc.add(Segment("proc10" , PROC10_BASE , PROC10_SIZE , IntTab(2,2), false, true, IntTab(2,2)));
	maptabc.add(Segment("proc11" , PROC11_BASE , PROC11_SIZE , IntTab(2,3), false, true, IntTab(2,3)));
	maptabc.add(Segment("proc12" , PROC12_BASE , PROC12_SIZE , IntTab(3,0), false, true, IntTab(3,0)));
	maptabc.add(Segment("proc13" , PROC13_BASE , PROC13_SIZE , IntTab(3,1), false, true, IntTab(3,1)));
	maptabc.add(Segment("proc14" , PROC14_BASE , PROC14_SIZE , IntTab(3,2), false, true, IntTab(3,2)));
	maptabc.add(Segment("proc15" , PROC15_BASE , PROC15_SIZE , IntTab(3,3), false, true, IntTab(3,3)));

	std::cout << maptabc << std::endl;

	soclib::common::MappingTable maptabx(32, IntTab(8,4), IntTab(4,4), 0x00C00000);
	maptabx.add(Segment("xram0" , MC0_M_BASE , MC0_M_SIZE , IntTab(0,0), false));
	maptabx.add(Segment("xram1" , MC1_M_BASE , MC1_M_SIZE , IntTab(0,0), false));
	maptabx.add(Segment("xram2" , MC2_M_BASE , MC2_M_SIZE , IntTab(0,0), false));
	maptabx.add(Segment("xram3" , MC3_M_BASE , MC3_M_SIZE , IntTab(0,0), false));
	std::cout << maptabx << std::endl;


	// Signals

	sc_clock	signal_clk("clk");
	sc_signal<bool> signal_resetn("resetn");
   
	sc_signal<bool> signal_proc0_it0("proc0_it0"); 
	sc_signal<bool> signal_proc0_it1("proc0_it1"); 
	sc_signal<bool> signal_proc0_it2("proc0_it2"); 
	sc_signal<bool> signal_proc0_it3("proc0_it3"); 
	sc_signal<bool> signal_proc0_it4("proc0_it4"); 
	sc_signal<bool> signal_proc0_it5("proc0_it5");

	sc_signal<bool> signal_proc1_it0("proc1_it0"); 
	sc_signal<bool> signal_proc1_it1("proc1_it1"); 
	sc_signal<bool> signal_proc1_it2("proc1_it2"); 
	sc_signal<bool> signal_proc1_it3("proc1_it3"); 
	sc_signal<bool> signal_proc1_it4("proc1_it4"); 
	sc_signal<bool> signal_proc1_it5("proc1_it5");

	sc_signal<bool> signal_proc2_it0("proc2_it0"); 
	sc_signal<bool> signal_proc2_it1("proc2_it1"); 
	sc_signal<bool> signal_proc2_it2("proc2_it2"); 
	sc_signal<bool> signal_proc2_it3("proc2_it3"); 
	sc_signal<bool> signal_proc2_it4("proc2_it4"); 
	sc_signal<bool> signal_proc2_it5("proc2_it5");

	sc_signal<bool> signal_proc3_it0("proc3_it0"); 
	sc_signal<bool> signal_proc3_it1("proc3_it1"); 
	sc_signal<bool> signal_proc3_it2("proc3_it2"); 
	sc_signal<bool> signal_proc3_it3("proc3_it3"); 
	sc_signal<bool> signal_proc3_it4("proc3_it4"); 
	sc_signal<bool> signal_proc3_it5("proc3_it5");

	sc_signal<bool> signal_proc4_it0("proc4_it0"); 
	sc_signal<bool> signal_proc4_it1("proc4_it1"); 
	sc_signal<bool> signal_proc4_it2("proc4_it2"); 
	sc_signal<bool> signal_proc4_it3("proc4_it3"); 
	sc_signal<bool> signal_proc4_it4("proc4_it4"); 
	sc_signal<bool> signal_proc4_it5("proc4_it5");

	sc_signal<bool> signal_proc5_it0("proc5_it0"); 
	sc_signal<bool> signal_proc5_it1("proc5_it1"); 
	sc_signal<bool> signal_proc5_it2("proc5_it2"); 
	sc_signal<bool> signal_proc5_it3("proc5_it3"); 
	sc_signal<bool> signal_proc5_it4("proc5_it4"); 
	sc_signal<bool> signal_proc5_it5("proc5_it5");

	sc_signal<bool> signal_proc6_it0("proc6_it0"); 
	sc_signal<bool> signal_proc6_it1("proc6_it1"); 
	sc_signal<bool> signal_proc6_it2("proc6_it2"); 
	sc_signal<bool> signal_proc6_it3("proc6_it3"); 
	sc_signal<bool> signal_proc6_it4("proc6_it4"); 
	sc_signal<bool> signal_proc6_it5("proc6_it5");

	sc_signal<bool> signal_proc7_it0("proc7_it0"); 
	sc_signal<bool> signal_proc7_it1("proc7_it1"); 
	sc_signal<bool> signal_proc7_it2("proc7_it2"); 
	sc_signal<bool> signal_proc7_it3("proc7_it3"); 
	sc_signal<bool> signal_proc7_it4("proc7_it4"); 
	sc_signal<bool> signal_proc7_it5("proc7_it5");

	sc_signal<bool> signal_proc8_it0("proc8_it0"); 
	sc_signal<bool> signal_proc8_it1("proc8_it1"); 
	sc_signal<bool> signal_proc8_it2("proc8_it2"); 
	sc_signal<bool> signal_proc8_it3("proc8_it3"); 
	sc_signal<bool> signal_proc8_it4("proc8_it4"); 
	sc_signal<bool> signal_proc8_it5("proc8_it5");

	sc_signal<bool> signal_proc9_it0("proc9_it0"); 
	sc_signal<bool> signal_proc9_it1("proc9_it1"); 
	sc_signal<bool> signal_proc9_it2("proc9_it2"); 
	sc_signal<bool> signal_proc9_it3("proc9_it3"); 
	sc_signal<bool> signal_proc9_it4("proc9_it4"); 
	sc_signal<bool> signal_proc9_it5("proc9_it5");

	sc_signal<bool> signal_proc10_it0("proc10_it0"); 
	sc_signal<bool> signal_proc10_it1("proc10_it1"); 
	sc_signal<bool> signal_proc10_it2("proc10_it2"); 
	sc_signal<bool> signal_proc10_it3("proc10_it3"); 
	sc_signal<bool> signal_proc10_it4("proc10_it4"); 
	sc_signal<bool> signal_proc10_it5("proc10_it5");

	sc_signal<bool> signal_proc11_it0("proc11_it0"); 
	sc_signal<bool> signal_proc11_it1("proc11_it1"); 
	sc_signal<bool> signal_proc11_it2("proc11_it2"); 
	sc_signal<bool> signal_proc11_it3("proc11_it3"); 
	sc_signal<bool> signal_proc11_it4("proc11_it4"); 
	sc_signal<bool> signal_proc11_it5("proc11_it5");

	sc_signal<bool> signal_proc12_it0("proc12_it0"); 
	sc_signal<bool> signal_proc12_it1("proc12_it1"); 
	sc_signal<bool> signal_proc12_it2("proc12_it2"); 
	sc_signal<bool> signal_proc12_it3("proc12_it3"); 
	sc_signal<bool> signal_proc12_it4("proc12_it4"); 
	sc_signal<bool> signal_proc12_it5("proc12_it5");

	sc_signal<bool> signal_proc13_it0("proc13_it0"); 
	sc_signal<bool> signal_proc13_it1("proc13_it1"); 
	sc_signal<bool> signal_proc13_it2("proc13_it2"); 
	sc_signal<bool> signal_proc13_it3("proc13_it3"); 
	sc_signal<bool> signal_proc13_it4("proc13_it4"); 
	sc_signal<bool> signal_proc13_it5("proc13_it5");

	sc_signal<bool> signal_proc14_it0("proc14_it0"); 
	sc_signal<bool> signal_proc14_it1("proc14_it1"); 
	sc_signal<bool> signal_proc14_it2("proc14_it2"); 
	sc_signal<bool> signal_proc14_it3("proc14_it3"); 
	sc_signal<bool> signal_proc14_it4("proc14_it4"); 
	sc_signal<bool> signal_proc14_it5("proc14_it5");

	sc_signal<bool> signal_proc15_it0("proc15_it0"); 
	sc_signal<bool> signal_proc15_it1("proc15_it1"); 
	sc_signal<bool> signal_proc15_it2("proc15_it2"); 
	sc_signal<bool> signal_proc15_it3("proc15_it3"); 
	sc_signal<bool> signal_proc15_it4("proc15_it4"); 
	sc_signal<bool> signal_proc15_it5("proc15_it5");


	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc0("vci_ini_proc0");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc0("vci_tgt_proc0");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc1("vci_ini_proc1");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc1("vci_tgt_proc1");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc2("vci_ini_proc2");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc2("vci_tgt_proc2");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc3("vci_ini_proc3");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc3("vci_tgt_proc3");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc4("vci_ini_proc4");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc4("vci_tgt_proc4");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc5("vci_ini_proc5");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc5("vci_tgt_proc5");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc6("vci_ini_proc6");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc6("vci_tgt_proc6");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc7("vci_ini_proc7");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc7("vci_tgt_proc7");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc8("vci_ini_proc8");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc8("vci_tgt_proc8");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc9("vci_ini_proc9");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc9("vci_tgt_proc9");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc10("vci_ini_proc10");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc10("vci_tgt_proc10");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc11("vci_ini_proc11");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc11("vci_tgt_proc11");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc12("vci_ini_proc12");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc12("vci_tgt_proc12");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc13("vci_ini_proc13");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc13("vci_tgt_proc13");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc14("vci_ini_proc14");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc14("vci_tgt_proc14");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_proc15("vci_ini_proc15");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_proc15("vci_tgt_proc15");

	soclib::caba::VciSignals<vci_param> signal_vci_tgt_tty("vci_tgt_tty");

	soclib::caba::VciSignals<vci_param> signal_vci_tgt_rom("vci_tgt_rom");

	soclib::caba::VciSignals<vci_param> signal_vci_tgt_xram("vci_tgt_xram");

	soclib::caba::VciSignals<vci_param> signal_vci_hht("signal_vci_hht");
	soclib::caba::VciSignals<vci_param> s_vci_cori_config("s_vci_cori_config");
	soclib::caba::VciSignals<vci_param> s_vci_ciro_config("s_vci_ciro_config");
	soclib::caba::HhtSignals<hht_param> s_hht("s_hht");
	
	soclib::caba::VciSignals<vci_param> signal_vci_ixr_memc0("vci_ixr_memc0");
	soclib::caba::VciSignals<vci_param> signal_vci_ini_memc0("vci_ini_memc0");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_memc0("vci_tgt_memc0");

	soclib::caba::VciSignals<vci_param> signal_vci_ixr_memc1("vci_ixr_memc1");
	soclib::caba::VciSignals<vci_param> signal_vci_ini_memc1("vci_ini_memc1");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_memc1("vci_tgt_memc1");

	soclib::caba::VciSignals<vci_param> signal_vci_ixr_memc2("vci_ixr_memc2");
	soclib::caba::VciSignals<vci_param> signal_vci_ini_memc2("vci_ini_memc2");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_memc2("vci_tgt_memc2");

	soclib::caba::VciSignals<vci_param> signal_vci_ixr_memc3("vci_ixr_memc3");
	soclib::caba::VciSignals<vci_param> signal_vci_ini_memc3("vci_ini_memc3");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_memc3("vci_tgt_memc3");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_pdspin0("vci_ini_pdspin0");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_pdspin0("vci_tgt_pdspin0");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_pdspin1("vci_ini_pdspin1");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_pdspin1("vci_tgt_pdspin1");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_pdspin2("vci_ini_pdspin2");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_pdspin2("vci_tgt_pdspin2");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_pdspin3("vci_ini_pdspin3");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_pdspin3("vci_tgt_pdspin3");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_cdspin0("vci_ini_cdspin0");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_cdspin0("vci_tgt_cdspin0");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_cdspin1("vci_ini_cdspin1");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_cdspin1("vci_tgt_cdspin1");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_cdspin2("vci_ini_cdspin2");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_cdspin2("vci_tgt_cdspin2");

	soclib::caba::VciSignals<vci_param> signal_vci_ini_cdspin3("vci_ini_cdspin3");
	soclib::caba::VciSignals<vci_param> signal_vci_tgt_cdspin3("vci_tgt_cdspin3");



	sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 
	sc_signal<bool> signal_tty_irq1("signal_tty_irq1"); 
	sc_signal<bool> signal_tty_irq2("signal_tty_irq2"); 
	sc_signal<bool> signal_tty_irq3("signal_tty_irq3"); 
	sc_signal<bool> signal_tty_irq4("signal_tty_irq4"); 
	sc_signal<bool> signal_tty_irq5("signal_tty_irq5"); 
	sc_signal<bool> signal_tty_irq6("signal_tty_irq6"); 
	sc_signal<bool> signal_tty_irq7("signal_tty_irq7"); 
	sc_signal<bool> signal_tty_irq8("signal_tty_irq8"); 
	sc_signal<bool> signal_tty_irq9("signal_tty_irq9"); 
	sc_signal<bool> signal_tty_irq10("signal_tty_irq10"); 
	sc_signal<bool> signal_tty_irq11("signal_tty_irq11"); 
	sc_signal<bool> signal_tty_irq12("signal_tty_irq12"); 
	sc_signal<bool> signal_tty_irq13("signal_tty_irq13"); 
	sc_signal<bool> signal_tty_irq14("signal_tty_irq14"); 
	sc_signal<bool> signal_tty_irq15("signal_tty_irq15"); 

	// Components
	// VCI ports indexation : 3 initiateurs et 5 cibles

        // INIT0 : proc0_ini
        // INIT1 : memc_ini
        // INIT2 : memc_ixr

	// TGT 0 : rom_tgt
	// TGT 1 : tty_tgt
        // TGT 2 : xram_tgt
	// TGT 3 : memc_tgt
	// TGT 4 : proc0_tgt

	soclib::common::Loader loader("soft/bin.soft");

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc0("proc0", 0, maptabp,maptabc,IntTab(0,0),IntTab(0,0),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc1("proc1", 1, maptabp,maptabc,IntTab(0,1),IntTab(0,1),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc2("proc2", 2, maptabp,maptabc,IntTab(0,2),IntTab(0,2),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc3("proc3", 3, maptabp,maptabc,IntTab(0,3),IntTab(0,3),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc4("proc4", 4, maptabp,maptabc,IntTab(1,0),IntTab(1,0),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc5("proc5", 5, maptabp,maptabc,IntTab(1,1),IntTab(1,1),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc6("proc6", 6, maptabp,maptabc,IntTab(1,2),IntTab(1,2),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc7("proc7", 7, maptabp,maptabc,IntTab(1,3),IntTab(1,3),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc8("proc8", 8, maptabp,maptabc,IntTab(2,0),IntTab(2,0),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc9("proc9", 9, maptabp,maptabc,IntTab(2,1),IntTab(2,1),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc10("proc10", 10, maptabp,maptabc,IntTab(2,2),IntTab(2,2),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc11("proc11", 11, maptabp,maptabc,IntTab(2,3),IntTab(2,3),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc12("proc12", 12, maptabp,maptabc,IntTab(3,0),IntTab(3,0),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc13("proc13", 13, maptabp,maptabc,IntTab(3,1),IntTab(3,1),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc14("proc14", 14, maptabp,maptabc,IntTab(3,2),IntTab(3,2),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciCcXcacheWrapper<vci_param, proc_iss > 
	proc15("proc15", 15, maptabp,maptabc,IntTab(3,3),IntTab(3,3),4,64,16,4,64,16,CLEANUP_OFFSET);

	soclib::caba::VciSimpleRam<vci_param> 
	rom("rom", IntTab(2,1), maptabp, loader);

	soclib::caba::VciSimpleRam<vci_param> 
	xram("xram",IntTab(0,0),maptabx, loader);

	soclib::caba::VciMemCache<vci_param> 
	memc0("memc0",maptabp,maptabc,maptabx,IntTab(0,0),IntTab(0,0),IntTab(0,0),16,256,16);

	soclib::caba::VciMemCache<vci_param> 
	memc1("memc1",maptabp,maptabc,maptabx,IntTab(1,0),IntTab(1,0),IntTab(1,0),16,256,16);

	soclib::caba::VciMemCache<vci_param> 
	memc2("memc2",maptabp,maptabc,maptabx,IntTab(2,0),IntTab(2,0),IntTab(2,0),16,256,16);

	soclib::caba::VciMemCache<vci_param> 
	memc3("memc3",maptabp,maptabc,maptabx,IntTab(3,0),IntTab(3,0),IntTab(3,0),16,256,16);
	
	soclib::caba::VciMultiTty<vci_param> 
	tty("tty",IntTab(3,1),maptabp,"tty0","tty1","tty2","tty3","tty4","tty5","tty6","tty7","tty8","tty9","tty10","tty11","tty12","tty13","tty14","tty15",NULL);

	soclib::caba::VciLocalCrossbar<vci_param> 
	clusterPN0("clusterPN0",maptabp, IntTab(0), IntTab(0), 4, 1 );

	soclib::caba::VciLocalCrossbar<vci_param> 
	clusterPN1("clusterPN1",maptabp, IntTab(1), IntTab(1), 4, 1 );

	soclib::caba::VciLocalCrossbar<vci_param> 
	clusterPN2("clusterPN2",maptabp, IntTab(2), IntTab(2), 4, 2 );

	soclib::caba::VciLocalCrossbar<vci_param> 
	clusterPN3("clusterPN3",maptabp, IntTab(3), IntTab(3), 4, 2 );

	soclib::caba::VciLocalCrossbar<vci_param> 
	clusterCN0("clusterCN0",maptabc, IntTab(0), IntTab(0), 1, 4 );

	soclib::caba::VciLocalCrossbar<vci_param> 
	clusterCN1("clusterCN1",maptabc, IntTab(1), IntTab(1), 1, 4 );

	soclib::caba::VciLocalCrossbar<vci_param> 
	clusterCN2("clusterCN2",maptabc, IntTab(2), IntTab(2), 1, 4 );

	soclib::caba::VciLocalCrossbar<vci_param> 
	clusterCN3("clusterCN3",maptabc, IntTab(3), IntTab(3), 1, 4 );

	soclib::caba::VciDspinPlusNetwork<vci_param, 4, 1> vciDspinCNetwork("vciDspinCNetwork", maptabc, 2, 2);

	soclib::caba::VciDspinPlusNetwork<vci_param, 4, 1> vciDspinPNetwork("vciDspinPNetwork", maptabp, 2, 2);

	soclib::caba::VciVgmn<vci_param> 
	vgmn("vgmn",maptabx, 4, 1, 1, 4);

	soclib::caba::VciCoriConfigInitiator<vci_param> vci_cori_config("vci_cori_config");
	soclib::caba::VciCiroConfigInitiator<vci_param> vci_ciro_config("vci_ciro_config");
	soclib::caba::VciHhtCoriBridge<vci_param, hht_param> cori_bridge("cori_bridge");
	soclib::caba::VciHhtCiroBridge<vci_param, hht_param> ciro_bridge("ciro_bridge");
	
	// Net-List
 
	proc0.p_clk(signal_clk);  
	proc0.p_resetn(signal_resetn);  
	proc0.p_irq[0](signal_proc0_it0); 
	proc0.p_irq[1](signal_proc0_it1); 
	proc0.p_irq[2](signal_proc0_it2); 
	proc0.p_irq[3](signal_proc0_it3); 
	proc0.p_irq[4](signal_proc0_it4); 
	proc0.p_irq[5](signal_proc0_it5); 
	proc0.p_vci_ini(signal_vci_ini_proc0);
	proc0.p_vci_tgt(signal_vci_tgt_proc0);

	proc1.p_clk(signal_clk);  
	proc1.p_resetn(signal_resetn);  
	proc1.p_irq[0](signal_proc1_it0); 
	proc1.p_irq[1](signal_proc1_it1); 
	proc1.p_irq[2](signal_proc1_it2); 
	proc1.p_irq[3](signal_proc1_it3); 
	proc1.p_irq[4](signal_proc1_it4); 
	proc1.p_irq[5](signal_proc1_it5); 
	proc1.p_vci_ini(signal_vci_ini_proc1);
	proc1.p_vci_tgt(signal_vci_tgt_proc1);

	proc2.p_clk(signal_clk);  
	proc2.p_resetn(signal_resetn);  
	proc2.p_irq[0](signal_proc2_it0); 
	proc2.p_irq[1](signal_proc2_it1); 
	proc2.p_irq[2](signal_proc2_it2); 
	proc2.p_irq[3](signal_proc2_it3); 
	proc2.p_irq[4](signal_proc2_it4); 
	proc2.p_irq[5](signal_proc2_it5); 
	proc2.p_vci_ini(signal_vci_ini_proc2);
	proc2.p_vci_tgt(signal_vci_tgt_proc2);

	proc3.p_clk(signal_clk);  
	proc3.p_resetn(signal_resetn);  
	proc3.p_irq[0](signal_proc3_it0); 
	proc3.p_irq[1](signal_proc3_it1); 
	proc3.p_irq[2](signal_proc3_it2); 
	proc3.p_irq[3](signal_proc3_it3); 
	proc3.p_irq[4](signal_proc3_it4); 
	proc3.p_irq[5](signal_proc3_it5); 
	proc3.p_vci_ini(signal_vci_ini_proc3);
	proc3.p_vci_tgt(signal_vci_tgt_proc3);

	proc4.p_clk(signal_clk);  
	proc4.p_resetn(signal_resetn);  
	proc4.p_irq[0](signal_proc4_it0); 
	proc4.p_irq[1](signal_proc4_it1); 
	proc4.p_irq[2](signal_proc4_it2); 
	proc4.p_irq[3](signal_proc4_it3); 
	proc4.p_irq[4](signal_proc4_it4); 
	proc4.p_irq[5](signal_proc4_it5); 
	proc4.p_vci_ini(signal_vci_ini_proc4);
	proc4.p_vci_tgt(signal_vci_tgt_proc4);

	proc5.p_clk(signal_clk);  
	proc5.p_resetn(signal_resetn);  
	proc5.p_irq[0](signal_proc5_it0); 
	proc5.p_irq[1](signal_proc5_it1); 
	proc5.p_irq[2](signal_proc5_it2); 
	proc5.p_irq[3](signal_proc5_it3); 
	proc5.p_irq[4](signal_proc5_it4); 
	proc5.p_irq[5](signal_proc5_it5); 
	proc5.p_vci_ini(signal_vci_ini_proc5);
	proc5.p_vci_tgt(signal_vci_tgt_proc5);

	proc6.p_clk(signal_clk);  
	proc6.p_resetn(signal_resetn);  
	proc6.p_irq[0](signal_proc6_it0); 
	proc6.p_irq[1](signal_proc6_it1); 
	proc6.p_irq[2](signal_proc6_it2); 
	proc6.p_irq[3](signal_proc6_it3); 
	proc6.p_irq[4](signal_proc6_it4); 
	proc6.p_irq[5](signal_proc6_it5); 
	proc6.p_vci_ini(signal_vci_ini_proc6);
	proc6.p_vci_tgt(signal_vci_tgt_proc6);

	proc7.p_clk(signal_clk);  
	proc7.p_resetn(signal_resetn);  
	proc7.p_irq[0](signal_proc7_it0); 
	proc7.p_irq[1](signal_proc7_it1); 
	proc7.p_irq[2](signal_proc7_it2); 
	proc7.p_irq[3](signal_proc7_it3); 
	proc7.p_irq[4](signal_proc7_it4); 
	proc7.p_irq[5](signal_proc7_it5); 
	proc7.p_vci_ini(signal_vci_ini_proc7);
	proc7.p_vci_tgt(signal_vci_tgt_proc7);

	proc8.p_clk(signal_clk);  
	proc8.p_resetn(signal_resetn);  
	proc8.p_irq[0](signal_proc8_it0); 
	proc8.p_irq[1](signal_proc8_it1); 
	proc8.p_irq[2](signal_proc8_it2); 
	proc8.p_irq[3](signal_proc8_it3); 
	proc8.p_irq[4](signal_proc8_it4); 
	proc8.p_irq[5](signal_proc8_it5); 
	proc8.p_vci_ini(signal_vci_ini_proc8);
	proc8.p_vci_tgt(signal_vci_tgt_proc8);

	proc9.p_clk(signal_clk);  
	proc9.p_resetn(signal_resetn);  
	proc9.p_irq[0](signal_proc9_it0); 
	proc9.p_irq[1](signal_proc9_it1); 
	proc9.p_irq[2](signal_proc9_it2); 
	proc9.p_irq[3](signal_proc9_it3); 
	proc9.p_irq[4](signal_proc9_it4); 
	proc9.p_irq[5](signal_proc9_it5); 
	proc9.p_vci_ini(signal_vci_ini_proc9);
	proc9.p_vci_tgt(signal_vci_tgt_proc9);

	proc10.p_clk(signal_clk);  
	proc10.p_resetn(signal_resetn);  
	proc10.p_irq[0](signal_proc10_it0); 
	proc10.p_irq[1](signal_proc10_it1); 
	proc10.p_irq[2](signal_proc10_it2); 
	proc10.p_irq[3](signal_proc10_it3); 
	proc10.p_irq[4](signal_proc10_it4); 
	proc10.p_irq[5](signal_proc10_it5); 
	proc10.p_vci_ini(signal_vci_ini_proc10);
	proc10.p_vci_tgt(signal_vci_tgt_proc10);

	proc11.p_clk(signal_clk);  
	proc11.p_resetn(signal_resetn);  
	proc11.p_irq[0](signal_proc11_it0); 
	proc11.p_irq[1](signal_proc11_it1); 
	proc11.p_irq[2](signal_proc11_it2); 
	proc11.p_irq[3](signal_proc11_it3); 
	proc11.p_irq[4](signal_proc11_it4); 
	proc11.p_irq[5](signal_proc11_it5); 
	proc11.p_vci_ini(signal_vci_ini_proc11);
	proc11.p_vci_tgt(signal_vci_tgt_proc11);

	proc12.p_clk(signal_clk);  
	proc12.p_resetn(signal_resetn);  
	proc12.p_irq[0](signal_proc12_it0); 
	proc12.p_irq[1](signal_proc12_it1); 
	proc12.p_irq[2](signal_proc12_it2); 
	proc12.p_irq[3](signal_proc12_it3); 
	proc12.p_irq[4](signal_proc12_it4); 
	proc12.p_irq[5](signal_proc12_it5); 
	proc12.p_vci_ini(signal_vci_ini_proc12);
	proc12.p_vci_tgt(signal_vci_tgt_proc12);

	proc13.p_clk(signal_clk);  
	proc13.p_resetn(signal_resetn);  
	proc13.p_irq[0](signal_proc13_it0); 
	proc13.p_irq[1](signal_proc13_it1); 
	proc13.p_irq[2](signal_proc13_it2); 
	proc13.p_irq[3](signal_proc13_it3); 
	proc13.p_irq[4](signal_proc13_it4); 
	proc13.p_irq[5](signal_proc13_it5); 
	proc13.p_vci_ini(signal_vci_ini_proc13);
	proc13.p_vci_tgt(signal_vci_tgt_proc13);

	proc14.p_clk(signal_clk);  
	proc14.p_resetn(signal_resetn);  
	proc14.p_irq[0](signal_proc14_it0); 
	proc14.p_irq[1](signal_proc14_it1); 
	proc14.p_irq[2](signal_proc14_it2); 
	proc14.p_irq[3](signal_proc14_it3); 
	proc14.p_irq[4](signal_proc14_it4); 
	proc14.p_irq[5](signal_proc14_it5); 
	proc14.p_vci_ini(signal_vci_ini_proc14);
	proc14.p_vci_tgt(signal_vci_tgt_proc14);

	proc15.p_clk(signal_clk);  
	proc15.p_resetn(signal_resetn);  
	proc15.p_irq[0](signal_proc15_it0); 
	proc15.p_irq[1](signal_proc15_it1); 
	proc15.p_irq[2](signal_proc15_it2); 
	proc15.p_irq[3](signal_proc15_it3); 
	proc15.p_irq[4](signal_proc15_it4); 
	proc15.p_irq[5](signal_proc15_it5); 
	proc15.p_vci_ini(signal_vci_ini_proc15);
	proc15.p_vci_tgt(signal_vci_tgt_proc15);

	vci_cori_config.p_clk(signal_clk);
	vci_cori_config.p_resetn(signal_resetn);
	vci_ciro_config.p_clk(signal_clk);
	vci_ciro_config.p_resetn(signal_resetn);
	cori_bridge.p_clk(signal_clk);
	cori_bridge.p_resetn(signal_resetn);
	ciro_bridge.p_clk(signal_clk);
	ciro_bridge.p_resetn(signal_resetn);

	vci_cori_config.p_vci(s_vci_cori_config);
	vci_ciro_config.p_vci(s_vci_ciro_config);
	cori_bridge.p_vci_config(s_vci_cori_config);
	ciro_bridge.p_vci_config(s_vci_ciro_config);
	cori_bridge.p_vci_io(signal_vci_tgt_rom); // To initiator
	ciro_bridge.p_vci_io(signal_vci_hht); //To target
	
	cori_bridge.p_hht(s_hht);
	ciro_bridge.p_hht(s_hht);

	

	rom.p_clk(signal_clk);
	rom.p_resetn(signal_resetn);
	rom.p_vci(signal_vci_hht);

	tty.p_clk(signal_clk);
	tty.p_resetn(signal_resetn);
	tty.p_vci(signal_vci_tgt_tty);
	tty.p_irq[0](signal_tty_irq0);
	tty.p_irq[1](signal_tty_irq1);
	tty.p_irq[2](signal_tty_irq2);
	tty.p_irq[3](signal_tty_irq3);
	tty.p_irq[4](signal_tty_irq4);
	tty.p_irq[5](signal_tty_irq5);
	tty.p_irq[6](signal_tty_irq6);
	tty.p_irq[7](signal_tty_irq7); 
	tty.p_irq[8](signal_tty_irq8);
	tty.p_irq[9](signal_tty_irq9);
	tty.p_irq[10](signal_tty_irq10);
	tty.p_irq[11](signal_tty_irq11); 
	tty.p_irq[12](signal_tty_irq12);
	tty.p_irq[13](signal_tty_irq13);
	tty.p_irq[14](signal_tty_irq14);
	tty.p_irq[15](signal_tty_irq15);  

	memc0.p_clk(signal_clk);
	memc0.p_resetn(signal_resetn);
	memc0.p_vci_tgt(signal_vci_tgt_memc0);	
	memc0.p_vci_ini(signal_vci_ini_memc0);
	memc0.p_vci_ixr(signal_vci_ixr_memc0);

	memc1.p_clk(signal_clk);
	memc1.p_resetn(signal_resetn);
	memc1.p_vci_tgt(signal_vci_tgt_memc1);	
	memc1.p_vci_ini(signal_vci_ini_memc1);
	memc1.p_vci_ixr(signal_vci_ixr_memc1);

	memc2.p_clk(signal_clk);
	memc2.p_resetn(signal_resetn);
	memc2.p_vci_tgt(signal_vci_tgt_memc2);	
	memc2.p_vci_ini(signal_vci_ini_memc2);
	memc2.p_vci_ixr(signal_vci_ixr_memc2);

	memc3.p_clk(signal_clk);
	memc3.p_resetn(signal_resetn);
	memc3.p_vci_tgt(signal_vci_tgt_memc3);	
	memc3.p_vci_ini(signal_vci_ini_memc3);
	memc3.p_vci_ixr(signal_vci_ixr_memc3);

	xram.p_clk(signal_clk);
        xram.p_resetn(signal_resetn);
	xram.p_vci(signal_vci_tgt_xram);	

	///////////////////////////////////////////////////////
	// R�seau vers la XRAM
	///////////////////////////////////////////////////////

	vgmn.p_clk(signal_clk);
	vgmn.p_resetn(signal_resetn);

	vgmn.p_to_initiator[0](signal_vci_ixr_memc0);
	vgmn.p_to_initiator[1](signal_vci_ixr_memc1);
	vgmn.p_to_initiator[2](signal_vci_ixr_memc2);
	vgmn.p_to_initiator[3](signal_vci_ixr_memc3);

	vgmn.p_to_target[0](signal_vci_tgt_xram);

	///////////////////////////////////////////////////////
	// R�seau des commandes primaires
	///////////////////////////////////////////////////////

	clusterPN0.p_clk(signal_clk);
	clusterPN0.p_resetn(signal_resetn);

	clusterPN0.p_to_initiator[0](signal_vci_ini_proc0);
	clusterPN0.p_to_initiator[1](signal_vci_ini_proc1);
	clusterPN0.p_to_initiator[2](signal_vci_ini_proc2);
	clusterPN0.p_to_initiator[3](signal_vci_ini_proc3);

	clusterPN0.p_to_target[0](signal_vci_tgt_memc0);

	clusterPN0.p_initiator_to_up(signal_vci_ini_pdspin0);
	clusterPN0.p_target_to_up(signal_vci_tgt_pdspin0);

	clusterPN1.p_clk(signal_clk);
	clusterPN1.p_resetn(signal_resetn);

	clusterPN1.p_to_initiator[0](signal_vci_ini_proc4);
	clusterPN1.p_to_initiator[1](signal_vci_ini_proc5);
	clusterPN1.p_to_initiator[2](signal_vci_ini_proc6);
	clusterPN1.p_to_initiator[3](signal_vci_ini_proc7);

	clusterPN1.p_to_target[0](signal_vci_tgt_memc1);

	clusterPN1.p_initiator_to_up(signal_vci_ini_pdspin1);
	clusterPN1.p_target_to_up(signal_vci_tgt_pdspin1);

	clusterPN2.p_clk(signal_clk);
	clusterPN2.p_resetn(signal_resetn);

	clusterPN2.p_to_initiator[0](signal_vci_ini_proc8);
	clusterPN2.p_to_initiator[1](signal_vci_ini_proc9);
	clusterPN2.p_to_initiator[2](signal_vci_ini_proc10);
	clusterPN2.p_to_initiator[3](signal_vci_ini_proc11);
	

	clusterPN2.p_to_target[0](signal_vci_tgt_memc2);
	clusterPN2.p_to_target[1](signal_vci_tgt_rom);

	clusterPN2.p_initiator_to_up(signal_vci_ini_pdspin2);
	clusterPN2.p_target_to_up(signal_vci_tgt_pdspin2);

	clusterPN3.p_clk(signal_clk);
	clusterPN3.p_resetn(signal_resetn);

	clusterPN3.p_to_initiator[0](signal_vci_ini_proc12);
	clusterPN3.p_to_initiator[1](signal_vci_ini_proc13);
	clusterPN3.p_to_initiator[2](signal_vci_ini_proc14);
	clusterPN3.p_to_initiator[3](signal_vci_ini_proc15);

	clusterPN3.p_to_target[0](signal_vci_tgt_memc3);
	clusterPN3.p_to_target[1](signal_vci_tgt_tty);

	clusterPN3.p_initiator_to_up(signal_vci_ini_pdspin3);
	clusterPN3.p_target_to_up(signal_vci_tgt_pdspin3);


	vciDspinPNetwork.p_clk(signal_clk);
	vciDspinPNetwork.p_resetn(signal_resetn);

	vciDspinPNetwork.p_to_initiator[0][0](signal_vci_ini_pdspin0);
	vciDspinPNetwork.p_to_initiator[0][1](signal_vci_ini_pdspin1);
	vciDspinPNetwork.p_to_initiator[1][0](signal_vci_ini_pdspin2);
	vciDspinPNetwork.p_to_initiator[1][1](signal_vci_ini_pdspin3);

	vciDspinPNetwork.p_to_target[0][0](signal_vci_tgt_pdspin0);
	vciDspinPNetwork.p_to_target[0][1](signal_vci_tgt_pdspin1);
	vciDspinPNetwork.p_to_target[1][0](signal_vci_tgt_pdspin2);
	vciDspinPNetwork.p_to_target[1][1](signal_vci_tgt_pdspin3);

	///////////////////////////////////////////////////////
	// R�seau des commandes de coh�rence
	///////////////////////////////////////////////////////
	clusterCN0.p_clk(signal_clk);
	clusterCN0.p_resetn(signal_resetn);

	clusterCN0.p_to_initiator[0](signal_vci_ini_memc0);

	clusterCN0.p_to_target[0](signal_vci_tgt_proc0);
	clusterCN0.p_to_target[1](signal_vci_tgt_proc1);
	clusterCN0.p_to_target[2](signal_vci_tgt_proc2);
	clusterCN0.p_to_target[3](signal_vci_tgt_proc3);

	clusterCN0.p_initiator_to_up(signal_vci_ini_cdspin0);
	clusterCN0.p_target_to_up(signal_vci_tgt_cdspin0);

	clusterCN1.p_clk(signal_clk);
	clusterCN1.p_resetn(signal_resetn);

	clusterCN1.p_to_initiator[0](signal_vci_ini_memc1);

	clusterCN1.p_to_target[0](signal_vci_tgt_proc4);
	clusterCN1.p_to_target[1](signal_vci_tgt_proc5);
	clusterCN1.p_to_target[2](signal_vci_tgt_proc6);
	clusterCN1.p_to_target[3](signal_vci_tgt_proc7);


	clusterCN1.p_initiator_to_up(signal_vci_ini_cdspin1);
	clusterCN1.p_target_to_up(signal_vci_tgt_cdspin1);

	clusterCN2.p_clk(signal_clk);
	clusterCN2.p_resetn(signal_resetn);

	clusterCN2.p_to_initiator[0](signal_vci_ini_memc2);

	clusterCN2.p_to_target[0](signal_vci_tgt_proc8);
	clusterCN2.p_to_target[1](signal_vci_tgt_proc9);
	clusterCN2.p_to_target[2](signal_vci_tgt_proc10);
	clusterCN2.p_to_target[3](signal_vci_tgt_proc11);

	clusterCN2.p_initiator_to_up(signal_vci_ini_cdspin2);
	clusterCN2.p_target_to_up(signal_vci_tgt_cdspin2);

	clusterCN3.p_clk(signal_clk);
	clusterCN3.p_resetn(signal_resetn);

	clusterCN3.p_to_initiator[0](signal_vci_ini_memc3);

	clusterCN3.p_to_target[0](signal_vci_tgt_proc12);
	clusterCN3.p_to_target[1](signal_vci_tgt_proc13);
	clusterCN3.p_to_target[2](signal_vci_tgt_proc14);
	clusterCN3.p_to_target[3](signal_vci_tgt_proc15);

	clusterCN3.p_initiator_to_up(signal_vci_ini_cdspin3);
	clusterCN3.p_target_to_up(signal_vci_tgt_cdspin3);

	vciDspinCNetwork.p_clk(signal_clk);
	vciDspinCNetwork.p_resetn(signal_resetn);

	vciDspinCNetwork.p_to_initiator[0][0](signal_vci_ini_cdspin0);
	vciDspinCNetwork.p_to_initiator[0][1](signal_vci_ini_cdspin1);
	vciDspinCNetwork.p_to_initiator[1][0](signal_vci_ini_cdspin2);
	vciDspinCNetwork.p_to_initiator[1][1](signal_vci_ini_cdspin3);

	vciDspinCNetwork.p_to_target[0][0](signal_vci_tgt_cdspin0);
	vciDspinCNetwork.p_to_target[0][1](signal_vci_tgt_cdspin1);
	vciDspinCNetwork.p_to_target[1][0](signal_vci_tgt_cdspin2);
	vciDspinCNetwork.p_to_target[1][1](signal_vci_tgt_cdspin3);


	////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////
   	FILE *file_vciCO=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/tsar_v0/log/vciCO.txt","w");
	FILE *file_vciCI=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/tsar_v0/log/vciCI.txt","w");
	if (file_vciCO==0)
		printf("Error creating vciCO log file\n");
	if (file_vciCI==0)
		printf("Error creating vciCI log file\n");
	FILE *file_vciRO=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/tsar_v0/log/vciRO.txt","w");
	FILE *file_vciRI=fopen("/root/Desktop/Recherche/SystemC/hht/platforms/tsar_v0/log/vciRI.txt","w");
	if (file_vciRO==0)
		printf("Error creating vciRO log file\n");
	if (file_vciRI==0)
		printf("Error creating vciRI log file\n");
		
	int ncycles;
	if (argc == 2) {
		ncycles = std::atoi(argv[1]);
	} else {
		std::cerr
			<< std::endl << "The number of simulation cycles must be defined in the command line" << std::endl;
		exit(1);
	}

	printf("Starting simulation\n");
	sc_start(sc_core::sc_time(0, SC_NS));
	signal_resetn = false;
	sc_start(sc_core::sc_time(1, SC_NS));
	signal_resetn = true;
	printf("Cycle\tvciCO           \tctrlNPCO        \tctrlPCO         \tvciCI           \tvciRO           \tctrlRI          \tvciRI           \t\n");
	for (int i = 0; i < ncycles ; i+=1)
	{
		sc_start(sc_core::sc_time(1, SC_NS));
		// Displays important fifos
		if (cori_bridge.f_vciCO.has_put && cori_bridge.f_vciCO.has_written.eop==1||
			cori_bridge.f_ctrlNPCO.has_put ||
			cori_bridge.f_ctrlPCO.has_put ||
			ciro_bridge.f_vciCI.has_put && ciro_bridge.f_vciCI.has_written.eop==1 ||
			ciro_bridge.f_vciRO.has_put && ciro_bridge.f_vciRO.has_written.reop==1 ||
			cori_bridge.f_ctrlRI.has_put ||
			cori_bridge.f_vciRI.has_put && cori_bridge.f_vciRI.has_written.reop==1 || 
			false)
		{
			printf("%d\t",i);
			cori_bridge.f_vciCO.has_written.print	(cori_bridge.f_vciCO.has_put && cori_bridge.f_vciCO.has_written.eop==1);
			cori_bridge.f_ctrlNPCO.has_written.print(cori_bridge.f_ctrlNPCO.has_put);
			cori_bridge.f_ctrlPCO.has_written.print	(cori_bridge.f_ctrlPCO.has_put);
			ciro_bridge.f_vciCI.has_written.print	(ciro_bridge.f_vciCI.has_put && ciro_bridge.f_vciCI.has_written.eop==1);
			ciro_bridge.f_vciRO.has_written.print	(ciro_bridge.f_vciRO.has_put && ciro_bridge.f_vciRO.has_written.reop==1);
			cori_bridge.f_ctrlRI.has_written.print	(cori_bridge.f_ctrlRI.has_put);
			cori_bridge.f_vciRI.has_written.print	(cori_bridge.f_vciRI.has_put && cori_bridge.f_vciRI.has_written.reop==1);
			printf("\n");
		}	
		if (/*cori_bridge.f_dataNPCO.has_put || 
			cori_bridge.f_dataRI.has_put || */
			false)
		{
			printf("%d\t",i);
			cori_bridge.f_vciCO.has_written.print(false);
			if (cori_bridge.f_dataNPCO.has_put)	printf("data(%.8X)\t",(int)cori_bridge.f_dataNPCO.has_written);	else printf("                \t");
			ciro_bridge.f_vciCI.has_written.print(false);
			ciro_bridge.f_vciRO.has_written.print(false);
			if (cori_bridge.f_dataRI.has_put)	printf("data(%.8X)\t",(int)cori_bridge.f_dataRI.has_written);		else printf("                \t");
			cori_bridge.f_vciRI.has_written.print(false);
			printf("\n");
		}	
		if (cori_bridge.f_vciCO.has_put) cori_bridge.f_vciCO.has_written.fprint (file_vciCO);
		if (ciro_bridge.f_vciCI.has_put) ciro_bridge.f_vciCI.has_written.fprint (file_vciCI);
		if (ciro_bridge.f_vciRO.has_put) ciro_bridge.f_vciRO.has_written.fprint (file_vciRO);
		if (cori_bridge.f_vciRI.has_put) cori_bridge.f_vciRI.has_written.fprint (file_vciRI);
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
