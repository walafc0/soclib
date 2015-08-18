#include <iostream>
#include <systemc>
#include <sys/time.h>
#include <cstdlib>
#include <cstdarg>
#include <sys/timeb.h>

#include "loader.h"           	
#include "mapping_table.h"
#include "vci_vgmn.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_xcache_wrapper.h"
#include "mips32.h"
#include "iss2.h"
#include "iss2_simhelper.h"
#include "vci_blackhole.h"

using namespace  soclib::tlmdt;
using namespace  soclib::common;

std::vector<std::string> stringArray(
    const char *first, ... )
{
    std::vector<std::string> ret;
	va_list arg;
	va_start(arg, first);
	const char *s = first;
	while(s) {
		ret.push_back(std::string(s));
		s = va_arg(arg, const char *);
	};
	va_end(arg);
    return ret;
}

int sc_main (int   argc, char  **argv)
{
  typedef VciParams<uint32_t,uint32_t> vci_param;
  typedef Mips32ElIss iss_t;
  typedef Iss2Simhelper<Mips32ElIss> simhelper;

  struct timeb initial, final;

  size_t dcache_size     = 32;
  size_t icache_size     = 32;
  size_t network_latence = 10;

  int n_initiators = 1;
  char * n_initiators_env; //env variable that says the number of initiators to be used
  n_initiators_env = getenv("N_INITS");
  if (n_initiators_env==NULL) {
    printf("WARNING : You should specify the number of initiators in variable N_INITS. For example, export N_INITS=2\n");
    printf("Using 1 initiator\n");
  }else {
    n_initiators = atoi( n_initiators_env );
  }

  uint32_t simulation_time = std::numeric_limits<uint32_t>::max();
  char * simulation_time_env; //env variable that says the simulation time
  simulation_time_env = getenv("SIMULATION_TIME");
  if (simulation_time_env==NULL) {
    printf("WARNING : You can specify the simulation time in variable SIMULATION_TIME. For example, export SIMULATION_TIME=100000\n");
  }else {
    simulation_time = atoi( simulation_time_env );
  }

  std::cout << "SIMULATION PARAMETERS: number of initiators = " << n_initiators << " simulation time = " << simulation_time << " icache size = " << icache_size << " dcache size = " << dcache_size << " network latence = " << network_latence << std::endl << std::endl;

  /////////////////////////////////////////////////////////////////////////////
  // MAPPING TABLE
  /////////////////////////////////////////////////////////////////////////////
  soclib::common::MappingTable maptab(32, soclib::common::IntTab(8), soclib::common::IntTab(8), 0x00200000);
  maptab.add(soclib::common::Segment("boot",  0xbfc00000,       2048, soclib::common::IntTab(1), 1));
  maptab.add(soclib::common::Segment("cram0", 0x10000000, 0x00100000, soclib::common::IntTab(0), 1));
  maptab.add(soclib::common::Segment("cram1", 0x20000000, 0x00100000, soclib::common::IntTab(1), 1));
  maptab.add(soclib::common::Segment("excep", 0x80000080,       2048, soclib::common::IntTab(1), 1));
  maptab.add(soclib::common::Segment("tty",   0x90200000,         32, soclib::common::IntTab(2), 0));
  maptab.add(soclib::common::Segment("uram0", 0x10200000, 0x00100000, soclib::common::IntTab(0), 0));
  maptab.add(soclib::common::Segment("uram1", 0x20200000, 0x00100000, soclib::common::IntTab(1), 0));

  /////////////////////////////////////////////////////////////////////////////
  // LOADER
  /////////////////////////////////////////////////////////////////////////////
  std::ostringstream soft;
  soft << "soft/bin" << n_initiators << "proc.soft";
  Loader loader(soft.str());

  /////////////////////////////////////////////////////////////////////////////
  // VCI_VGMN
  /////////////////////////////////////////////////////////////////////////////
  VciVgmn vgmn_1("vgmn", maptab, n_initiators, 3, network_latence, 8);
 
  /////////////////////////////////////////////////////////////////////////////
  // VCI_XCACHE 
  /////////////////////////////////////////////////////////////////////////////
  VciXcacheWrapper<vci_param, simhelper > *xcache[n_initiators]; 
  soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > *fake_initiator[n_initiators];

  for (int i=0 ; i < n_initiators ; i++) {
    std::ostringstream cpu_name;
    cpu_name << "xcache" << i;
    xcache[i] = new VciXcacheWrapper<vci_param, simhelper >((cpu_name.str()).c_str(), i, IntTab(i), maptab, 1, icache_size, 8, 1, dcache_size, 8, 1000 * UNIT_TIME);
    xcache[i]->p_vci(*vgmn_1.p_to_initiator[i]);

    std::ostringstream fake_name;
    fake_name << "fake" << i;
    fake_initiator[i] = new soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> >((fake_name.str()).c_str(), iss_t::n_irq);
    
    for(unsigned int irq=0; irq<iss_t::n_irq; irq++){
      (*fake_initiator[i]->p_socket[irq])(*xcache[i]->p_irq[irq]);
    }

  }

  /////////////////////////////////////////////////////////////////////////////
  // TARGET - RAM
  /////////////////////////////////////////////////////////////////////////////
  int n_rams = 2;
  VciRam<vci_param> *ram[32];
  for (int i=0 ; i<n_rams; i++) {
    std::ostringstream name;
    name << "ram" << i;
    ram[i] = new VciRam<vci_param>((name.str()).c_str(), IntTab(i), maptab, loader);
    (*vgmn_1.p_to_target[i])(ram[i]->p_vci);
  }
  

  /////////////////////////////////////////////////////////////////////////////
  // TARGET - TTY
  /////////////////////////////////////////////////////////////////////////////
  VciMultiTty<vci_param> vcitty("tty0", IntTab(2), maptab, "TTY0", NULL);
  (*vgmn_1.p_to_target[2])(vcitty.p_vci);
 
  soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> > *fake_target_tagged;
  
  fake_target_tagged = new soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> >("fake_target_tagged", 1);
  
  (*vcitty.p_irq[0])(*fake_target_tagged->p_socket[0]);

  /////////////////////////////////////////////////////////////////////////////
  // SIMULATION
  /////////////////////////////////////////////////////////////////////////////
  ftime(&initial);
  sc_core::sc_start();  // start the simulation
  ftime(&final);

  std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl << std::endl;

  return 0;
}
