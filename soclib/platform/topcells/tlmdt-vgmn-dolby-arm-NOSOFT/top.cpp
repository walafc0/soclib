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
#include "arm.h"
#include "iss2_simhelper.h"
#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_audio_fifo.h"
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
	typedef soclib::common::ArmIss iss_t;
	typedef soclib::common::Iss2Simhelper<soclib::common::ArmIss> iss2_t;

	struct timeb initial, final;

	/////////////////////////////////////////////////////////////////////////////
	// CONSTANTS
	/////////////////////////////////////////////////////////////////////////////
	size_t network_latence = 3;
	int n_initiators = 1;

	/////////////////////////////////////////////////////////////////////////////
	// MAPPING TABLE
	/////////////////////////////////////////////////////////////////////////////

	soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0xF0000000);

  maptab.add(Segment("boot", BOOT_BASE, BOOT_SIZE, IntTab(0), true));

	maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
	//maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
	maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));
  
	maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));
  
	maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));

	maptab.add(Segment("audio", AUDIO_FIFO_BASE, AUDIO_FIFO_SIZE   , IntTab(3), false));


	/////////////////////////////////////////////////////////////////////////////
	// LOADER
	/////////////////////////////////////////////////////////////////////////////
	soclib::common::Loader loader("soft/bin.soft");

	/////////////////////////////////////////////////////////////////////////////
	// VCI_VGMN
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciVgmn vgmn("vgmn", maptab, 1, 4, network_latence, 8);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_XCACHE_WRAPPER 
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciXcacheWrapper<vci_param, iss2_t > *xcache[n_initiators]; 

	for (int i=0 ; i < n_initiators ; i++) {
	  std::ostringstream cpu_name;
	  cpu_name << "cache" << i;
	  xcache[i] = new soclib::tlmdt::VciXcacheWrapper<vci_param, iss2_t >((cpu_name.str()).c_str(), i, maptab, IntTab(i), 1, 8, 4, 1, 8, 4);

	}

	/////////////////////////////////////////////////////////////////////////////
	// VCI_RAM
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciRam<vci_param> vciram0("vciram0", IntTab(0), maptab, loader);
	soclib::tlmdt::VciRam<vci_param> vciram1("vciram1", IntTab(1), maptab, loader);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_TTY
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciMultiTty<vci_param> vcitty("vcitty", IntTab(2), maptab, getTTYNames(n_initiators));

	/////////////////////////////////////////////////////////////////////////////
	// VCI_FRAMEBUFFER
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciAudioFifo<vci_param> vciaf("vciaf", IntTab(3), maptab); 

	/////////////////////////////////////////////////////////////////////////////
	// CONNECTIONS
	/////////////////////////////////////////////////////////////////////////////
	for (int i=0 ; i < n_initiators ; i++) {
	  xcache[i]->p_vci(*vgmn.p_to_initiator[i]);
	}

        (*vgmn.p_to_target[0])(vciram0.p_vci);
        (*vgmn.p_to_target[1])(vciram1.p_vci);
	(*vgmn.p_to_target[2])(vcitty.p_vci);
	(*vgmn.p_to_target[3])(vciaf.p_vci);

	/////////////////////////////////////////////////////////////////////////////
	// VciBlackhole Initiator
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > *fake_initiator[n_initiators];

	for (int i=0 ; i < n_initiators ; i++) {
	  std::ostringstream fake_name;
	  fake_name << "fake" << i;
	  fake_initiator[i] = new soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> >((fake_name.str()).c_str(), iss_t::n_irq);

	  for(int irq=0; irq<iss_t::n_irq; irq++){
	    (*fake_initiator[i]->p_socket[irq])(*xcache[i]->p_irq[irq]);
	  }
	}

	/////////////////////////////////////////////////////////////////////////////
	// VciBlackhole Target Tagged
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> > *fake_target_tagged;

	fake_target_tagged = new soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> >("fake_target_tagged", n_initiators);


	for(int i=0; i<n_initiators; i++){
	  (*vcitty.p_irq[i])(*fake_target_tagged->p_socket[i]);
	}
 
	/////////////////////////////////////////////////////////////////////////////
	// START
	/////////////////////////////////////////////////////////////////////////////
	printf("Start\n");


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
