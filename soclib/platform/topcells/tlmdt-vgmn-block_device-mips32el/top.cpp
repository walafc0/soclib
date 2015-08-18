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
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: nipo
 */

#include <iostream>
#include <cstdlib>
#include <sys/timeb.h>
#include <limits>

#include "loader.h"           	
#include "mapping_table.h"
#include "mips32.h"
#include "vci_xcache_wrapper.h"
#include "vci_block_device.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_simhelper.h"
#include "vci_vgmn.h"
#include "vci_blackhole.h"

#include "segmentation.h"

std::vector<std::string> getTTYNames(int n)
{
    std::vector<std::string> ret;
    for(int i=0; i<n; i++){
      std::ostringstream tty_name;
      tty_name << "tty" << i;
      ret.push_back(tty_name.str());
    }
    return ret;
}

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

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(1), true));
	maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(1), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(1), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(2), true));
  
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(3), false));
	maptab.add(Segment("bd"   , BD_BASE   , BD_SIZE   , IntTab(0), false));
	maptab.add(Segment("simhelper", SIMHELPER_BASE, SIMHELPER_SIZE, IntTab(4), false));


	/////////////////////////////////////////////////////////////////////////////
	// LOADER
	/////////////////////////////////////////////////////////////////////////////
	soclib::common::Loader loader("soft/bin.soft");

	/////////////////////////////////////////////////////////////////////////////
	// VCI_VGMN
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciVgmn vgmn("vgmn", maptab, 2, 5, network_latence, 8);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_XCACHE_WRAPPER 
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Mips32ElIss> cache0("cache0", 0, maptab, IntTab(1), 1, 8, 4, 1, 8, 4);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_RAM
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(1), maptab, loader);
	soclib::tlmdt::VciRam<vci_param> vcimultiram1("vcimultiram1", IntTab(2), maptab, loader);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_TTY
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciMultiTty<vci_param> vcitty("vcitty", IntTab(3), maptab, "vcitty0", NULL);

	/////////////////////////////////////////////////////////////////////////////
	// BLOCK DEVIDE
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciBlockDevice<vci_param> vcibd("vcibd", maptab, IntTab(0), IntTab(0), "test.bin");

	/////////////////////////////////////////////////////////////////////////////
	// VCI_LOCKS
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciSimhelper<vci_param> vcisimhelper("vcisimhelper", IntTab(4), maptab);

	/////////////////////////////////////////////////////////////////////////////
	// CONNECTIONS
	/////////////////////////////////////////////////////////////////////////////
        (*vgmn.p_to_initiator[0])(vcibd.p_vci_initiator);
        (*vgmn.p_to_initiator[1])(cache0.p_vci);

        (*vgmn.p_to_target[0])(vcibd.p_vci_target);
        (*vgmn.p_to_target[1])(vcimultiram0.p_vci);
        (*vgmn.p_to_target[2])(vcimultiram1.p_vci);
	(*vgmn.p_to_target[3])(vcitty.p_vci);
	(*vgmn.p_to_target[4])(vcisimhelper.p_vci);

	vcibd.p_irq(*cache0.p_irq[0]);

	/////////////////////////////////////////////////////////////////////////////
	// VciBlackhole Initiator
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > fake_initiator0("fake_init", soclib::common::Mips32ElIss::n_irq-1);
	for(unsigned int irq=0; irq<soclib::common::Mips32ElIss::n_irq-1; irq++){
	  (*fake_initiator0.p_socket[irq])(*cache0.p_irq[irq+1]);
	}

	/////////////////////////////////////////////////////////////////////////////
	// VciBlackhole Target Tagged
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> > fake_target_tagged("fake_target_tagged", 1);

	(*vcitty.p_irq[0])(*fake_target_tagged.p_socket[0]);
 
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
