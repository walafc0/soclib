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
#include "mips32.h"
#include "vci_multi_tty.h"
#include "iss2_simhelper.h"
#include "vci_xcache_wrapper.h"
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

int sc_main (int   argc, char  *argv[])
{
  typedef VciParams<uint32_t,uint32_t> vci_param;
  typedef Mips32ElIss iss_t;
  typedef Iss2Simhelper<Mips32ElIss> simhelper;

  struct timeb initial, final;
 
  int n_initiators       = 1;
  size_t network_latence = 10;

  char * ncpu_env; //env variable that says the number of MIPS processors to be used
  ncpu_env = getenv("N_INITS");
  if (ncpu_env==NULL) {
    printf("WARNING : You have to specify the number of processors to be used in the simulation in the N_INITS variable\n");
  }else {
    n_initiators = atoi( ncpu_env );
  }

  std::cout << "SIMULATION PARAMETERS: number of initiators = " << n_initiators << std::endl << std::endl;
  
  /////////////////////////////////////////////////////////////////////////////
  // MAPPING TABLE
  /////////////////////////////////////////////////////////////////////////////
  MappingTable maptab(32, IntTab(8), IntTab(8), 0x00200000);
  maptab.add(Segment("boot",  0xbfc00000,       2048, IntTab(1), 1));
  maptab.add(Segment("excep", 0x80000080,       2048, IntTab(1), 1));
  maptab.add(Segment("text" , 0x00400000, 0x00050000, IntTab(1), 1));
  maptab.add(Segment("cram0", 0x10000000, 0x00100000, IntTab(0), 1));
  maptab.add(Segment("cram1", 0x20000000, 0x00100000, IntTab(1), 1));
  maptab.add(Segment("uram0", 0x10200000, 0x00100000, IntTab(0), 0));
  maptab.add(Segment("uram1", 0x20200000, 0x00100000, IntTab(1), 0));
  maptab.add(Segment("tty",   0x90200000,         96, IntTab(2), 0));
 
  /////////////////////////////////////////////////////////////////////////////
  // LOADER
  /////////////////////////////////////////////////////////////////////////////
  std::ostringstream soft;
  soft << "soft/bin" << n_initiators << "proc.soft";
  std::cout << "soft:" << soft.str() << std::endl;
  Loader loader(soft.str());

  /////////////////////////////////////////////////////////////////////////////
  // VGMN
  /////////////////////////////////////////////////////////////////////////////
  VciVgmn vgmn_1("vgmn", maptab, n_initiators, 3, network_latence, 8);

  /////////////////////////////////////////////////////////////////////////////
  // XCACHE
  /////////////////////////////////////////////////////////////////////////////
  VciXcacheWrapper<vci_param, simhelper > *xcache[n_initiators];
  VciBlackhole<tlm::tlm_initiator_socket<> > *fake_initiator[n_initiators];
  for (int i=0; i<n_initiators; i++) {
    printf("XCACHE %d\n",i);
    std::ostringstream name;
    name << "xcache" << i;
    xcache[i] = new VciXcacheWrapper<vci_param, simhelper >((name.str()).c_str(), i,  maptab, IntTab(i), 1, 8, 4, 1, 8, 4);
    xcache[i]->p_vci(*vgmn_1.p_to_initiator[i]);

    std::ostringstream fake_name;
    fake_name << "fake" << i;
    fake_initiator[i] = new VciBlackhole<tlm::tlm_initiator_socket<> >((fake_name.str()).c_str(), iss_t::n_irq);
    
    for(unsigned int irq=0; irq<iss_t::n_irq; irq++){
      (*fake_initiator[i]->p_socket[irq])(*xcache[i]->p_irq[irq]);
    }
    
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // TARGET - RAM
  /////////////////////////////////////////////////////////////////////////////
  int n_rams = 2;
  VciRam<vci_param> *ram[n_rams];
  for (int i=0 ; i<n_rams; i++) {
    std::ostringstream name;
    name << "ram" << i;
    ram[i] = new VciRam<vci_param>((name.str()).c_str(), IntTab(i), maptab, loader);
    (*vgmn_1.p_to_target[i])(ram[i]->p_vci);
  }
  

  /////////////////////////////////////////////////////////////////////////////
  // TARGET - TTY
  /////////////////////////////////////////////////////////////////////////////
  VciMultiTty<vci_param> vcitty("tty0", IntTab(n_rams), maptab, getTTYNames(n_initiators));
  (*vgmn_1.p_to_target[n_rams])(vcitty.p_vci);

  VciBlackhole<tlm_utils::simple_target_socket_tagged<VciBlackholeBase, 32, tlm::tlm_base_protocol_types> > *fake_target_tagged;
  
  fake_target_tagged = new VciBlackhole<tlm_utils::simple_target_socket_tagged<VciBlackholeBase, 32, tlm::tlm_base_protocol_types> >("fake_target_tagged", n_initiators);

  for (int i=0; i<n_initiators; i++) {
    (*vcitty.p_irq[i])(*fake_target_tagged->p_socket[i]);
  }

  /////////////////////////////////////////////////////////////////////////////
  // SIMULATION
  /////////////////////////////////////////////////////////////////////////////
  printf("run\n");
  ftime(&initial);

  sc_core::sc_start();  // start the simulation

  ftime(&final);

  std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl;

  for (int i=0 ; i<n_rams; i++) {
    ram[i]->print_stats();
  }

  vcitty.print_stats();

  return 0;
}
