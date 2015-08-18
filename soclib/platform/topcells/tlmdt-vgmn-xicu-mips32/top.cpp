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
 * Copyright (c) UPMC, Lip6, SoC
 *         Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>, 2010
 *
 * Maintainers: alinev
 */

#include <iostream>
#include <cstdlib>
#include <sys/timeb.h>

#include "mapping_table.h"
#include "iss2_simhelper.h"
#include "mips32.h"
#include "vci_xcache_wrapper.h"
#include "vci_xicu.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_locks.h"
#include "vci_vgmn.h"
#include "vci_blackhole.h"

#include "segmentation.h"

int _main(int argc, char *argv[])
{
	using namespace sc_core;
	// Avoid repeating these everywhere
	using soclib::common::IntTab;
	using soclib::common::Segment;

	// Define our VCI parameters
	typedef soclib::tlmdt::VciParams<uint32_t,uint32_t> vci_param;
	struct timeb initial, final;

	/////////////////////////////////////////////////////////////////////////////
	// VARIABLES
	/////////////////////////////////////////////////////////////////////////////
	size_t network_latence = 3;

	/////////////////////////////////////////////////////////////////////////////
	// MAPPING TABLE
	/////////////////////////////////////////////////////////////////////////////
	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
  
	maptab.add(Segment("loc0" , LOC0_BASE , LOC0_SIZE , IntTab(1), true));
	maptab.add(Segment("loc1" , LOC1_BASE , LOC1_SIZE , IntTab(1), true));
	maptab.add(Segment("loc2" , LOC2_BASE , LOC2_SIZE , IntTab(1), true));
	maptab.add(Segment("loc3" , LOC3_BASE , LOC3_SIZE , IntTab(1), true));
  
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));
	maptab.add(Segment("xicu", XICU_BASE, XICU_SIZE, IntTab(3), false));
	maptab.add(Segment("locks", LOCKS_BASE, LOCKS_SIZE, IntTab(4), false));

	/////////////////////////////////////////////////////////////////////////////
	// LOADER
	/////////////////////////////////////////////////////////////////////////////
	soclib::common::Loader loader("soft/bin.soft");


	/////////////////////////////////////////////////////////////////////////////
	// VCI_VGMN
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciVgmn vgmn("vgmn", maptab, 4, 5, network_latence, 8);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_XCACHE_WRAPPER 
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache0("cache0", 0, maptab,IntTab(0), 4, 1, 8, 4, 1, 8);
	soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache1("cache1", 1, maptab,IntTab(1), 4, 1, 8, 4, 1, 8);
	soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache2("cache2", 2, maptab,IntTab(2), 4, 1, 8, 4, 1, 8);
	soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > cache3("cache3", 3, maptab,IntTab(3), 4, 1, 8, 4, 1, 8);


	soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > fake_init0("fake_init0", soclib::common::Mips32ElIss::n_irq-1);
	soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > fake_init1("fake_init1", soclib::common::Mips32ElIss::n_irq-1);
	soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > fake_init2("fake_init2", soclib::common::Mips32ElIss::n_irq-1);
	soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > fake_init3("fake_init3", soclib::common::Mips32ElIss::n_irq-1);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_RAM
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
	soclib::tlmdt::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(1), maptab, loader);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_TTY
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "vcitty0", "vcitty1", "vcitty2", "vcitty3", NULL);
	soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> > fake_target("fake_target", 4);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_XICU
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciXicu<vci_param> vcixicu("vcitxicu", maptab, IntTab(3), 2, 0, 2, 4);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_LOCKS
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciLocks<vci_param> vcilocks("vcilocks", IntTab(4), maptab); 
	
	/////////////////////////////////////////////////////////////////////////////
	// CONNECTIONS
	/////////////////////////////////////////////////////////////////////////////
	cache0.p_vci(*vgmn.p_to_initiator[0]);
	cache1.p_vci(*vgmn.p_to_initiator[1]);
	cache2.p_vci(*vgmn.p_to_initiator[2]);
	cache3.p_vci(*vgmn.p_to_initiator[3]);

	vcimultiram0.p_vci(*vgmn.p_to_target[0]);
	vcimultiram1.p_vci(*vgmn.p_to_target[1]);
	vcitty.p_vci(*vgmn.p_to_target[2]);
	vcixicu.p_vci(*vgmn.p_to_target[3]);
	vcilocks.p_vci(*vgmn.p_to_target[4]);

	(*vcixicu.p_irq[0])(*cache0.p_irq[0]); 
	(*vcixicu.p_irq[1])(*cache1.p_irq[0]); 
	(*vcixicu.p_irq[2])(*cache2.p_irq[0]); 
	(*vcixicu.p_irq[3])(*cache3.p_irq[0]); 
  
	for(unsigned int irq=0; irq<soclib::common::Mips32ElIss::n_irq-1; irq++){
	  (*fake_init0.p_socket[irq])(*cache0.p_irq[irq+1]);
	  (*fake_init1.p_socket[irq])(*cache1.p_irq[irq+1]);
	  (*fake_init2.p_socket[irq])(*cache2.p_irq[irq+1]);
	  (*fake_init3.p_socket[irq])(*cache3.p_irq[irq+1]);
	}
 
	(*vcitty.p_irq[0])(*fake_target.p_socket[0]); 
	(*vcitty.p_irq[1])(*fake_target.p_socket[1]); 
	(*vcitty.p_irq[2])(*fake_target.p_socket[2]); 
	(*vcitty.p_irq[3])(*fake_target.p_socket[3]); 


	/////////////////////////////////////////////////////////////////////////////
	// START
	/////////////////////////////////////////////////////////////////////////////
	ftime(&initial);
	sc_core::sc_start();
	ftime(&final);
	
	std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl << std::endl;

	return 0;
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
