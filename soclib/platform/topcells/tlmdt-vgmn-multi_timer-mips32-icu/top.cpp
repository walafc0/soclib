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

#include "loader.h"           	
#include "mapping_table.h"
#include "mips32.h"
#include "iss2_simhelper.h"
#include "vci_xcache_wrapper.h"
#include "vci_timer.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_locks.h"
#include "vci_icu.h"
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
	size_t network_latence = 10;
	char * network_latence_env; //env variable that says the global interconnect delay
	network_latence_env = getenv("NETWORK_LATENCE");
	if (network_latence_env!=NULL) {
	  network_latence = atoi( network_latence_env );
	}

	int n_initiators = 1;
	char * n_initiators_env; //env variable that says the number of initiators to be used
	n_initiators_env = getenv("N_INITS");
	
	if (n_initiators_env==NULL) {
	  printf("WARNING : You should specify the number of initiators in variable N_INITS. For example, export N_INITS=2\n");
	  printf("Using 1 initiator\n");
	}else {
	  n_initiators = atoi( n_initiators_env );
	}
	assert(n_initiators<4 && "This platform can contain up to 3 initiators");

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
	maptab.add(Segment("timer", TIMER_BASE, TIMER_SIZE, IntTab(3), false));
	maptab.add(Segment("locks", LOCKS_BASE, LOCKS_SIZE, IntTab(4), false));
	maptab.add(Segment("icu0" , ICU0_BASE , ICU0_SIZE , IntTab(5), false));
	maptab.add(Segment("icu1" , ICU1_BASE , ICU1_SIZE , IntTab(6), false));
	maptab.add(Segment("icu2" , ICU2_BASE , ICU2_SIZE , IntTab(7), false));

	/////////////////////////////////////////////////////////////////////////////
	// LOADER
	/////////////////////////////////////////////////////////////////////////////
	soclib::common::Loader loader("soft/bin.soft");

	/////////////////////////////////////////////////////////////////////////////
	// VCI_VGMN
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciVgmn vgmn("vgmn", maptab, n_initiators, 5+n_initiators, network_latence, 8);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_XCACHE_WRAPPER 
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > *xcache[n_initiators]; 

	for (int i=0 ; i < n_initiators ; i++) {
	  std::ostringstream cpu_name;
	  cpu_name << "cache" << i;
	  xcache[i] = new soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> >((cpu_name.str()).c_str(), i, IntTab(i), maptab, 1, 8, 4, 1, 8, 4, 500 * UNIT_TIME);

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
	// VCI_TIMER
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciTimer<vci_param> vcitimer("vcitimer", IntTab(3), maptab, n_initiators);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_LOCKS
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciLocks<vci_param> vcilocks("vcilocks", IntTab(4), maptab);

	/////////////////////////////////////////////////////////////////////////////
	// VCI_ICU
	/////////////////////////////////////////////////////////////////////////////
	size_t nirq = 2;
	soclib::tlmdt::VciIcu<vci_param> *vciicu[n_initiators];
	for (int i=0 ; i < n_initiators ; i++) {
	  std::ostringstream icu_name;
	  icu_name << "vciicu" << i;
	  vciicu[i] = new soclib::tlmdt::VciIcu<vci_param>((icu_name.str()).c_str(), IntTab(5+i), maptab, nirq);

	}

	/////////////////////////////////////////////////////////////////////////////
	// CONNECTIONS
	/////////////////////////////////////////////////////////////////////////////
	for (int i=0 ; i < n_initiators ; i++) {
	  xcache[i]->p_vci(*vgmn.p_to_initiator[i]);
	}

        (*vgmn.p_to_target[0])(vciram0.p_vci);
        (*vgmn.p_to_target[1])(vciram1.p_vci);
	(*vgmn.p_to_target[2])(vcitty.p_vci);
	(*vgmn.p_to_target[3])(vcitimer.p_vci);
	(*vgmn.p_to_target[4])(vcilocks.p_vci);

	for (int i=0 ; i < n_initiators ; i++) {
	  (*vgmn.p_to_target[5+i])(vciicu[i]->p_vci);

	  vciicu[i]->p_irq(*xcache[i]->p_irq[0]);
	  (*vcitimer.p_irq[i])(*vciicu[i]->p_irq_in[0]);
	  (*vcitty.p_irq[i])(*vciicu[i]->p_irq_in[1]);
	}

	/////////////////////////////////////////////////////////////////////////////
	// VciBlackhole Initiator
	/////////////////////////////////////////////////////////////////////////////
	soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > *fake_initiator[n_initiators];

	for (int i=0 ; i < n_initiators ; i++) {
	  std::ostringstream fake_name;
	  fake_name << "fake" << i;
	  fake_initiator[i] = new soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> >((fake_name.str()).c_str(), soclib::common::Mips32ElIss::n_irq-1);

	  for(int irq=0; irq<soclib::common::Mips32ElIss::n_irq-1; irq++){
	    (*fake_initiator[i]->p_socket[irq])(*xcache[i]->p_irq[irq+1]);
	  }
	}

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
