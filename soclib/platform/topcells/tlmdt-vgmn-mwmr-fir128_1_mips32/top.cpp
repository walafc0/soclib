#include <iostream>
#include <systemc>
#include <sys/timeb.h>
#include <tlmdt>
#include "loader.h"           	
#include "loader.h"
#include "mapping_table.h"
#include "vci_vgmn.h"
///#include "vci_simple_initiator.h"
#include "vci_ram.h"
#include "vci_locks.h"
#include "vci_multi_tty.h"
#include "vci_mwmr_controller.h"
#include "vci_xcache_wrapper.h"
#include "mips32.h"
#include "iss2_simhelper.h"
#include "fir128.h"
#include "segmentation.h"
#include "vci_blackhole.h"

using namespace soclib::tlmdt;
using namespace soclib::common;

int sc_main (int argc, char *argv[])
{
  typedef soclib::tlmdt::VciParams<uint32_t,uint32_t> vci_param;

  struct timeb initial, final;
 
  size_t read_depth, write_depth, n_read_channel, n_write_channel, n_config, n_status;
  size_t network_latence = 10;
  size_t simulation_time = std::numeric_limits<uint32_t>::max();
  int n_initiators = 2;

    char * simulation_time_env; //env variable that says the simulation time
  simulation_time_env = getenv("SIMULATION_TIME");
  if (simulation_time_env==NULL) {
    printf("WARNING : You can specify the simulation time in variable SIMULATION_TIME. For example, export SIMULATION_TIME=100000\n");
  }else {
    simulation_time = atoi( simulation_time_env );
  }

  std::cout << "SIMULATION PARAMETERS: simulation time = " << simulation_time << " network latence = " << network_latence << std::endl << std::endl;

  /////////////////////////////////////////////////////////////////////////////
  // MAPPING TABLE
  /////////////////////////////////////////////////////////////////////////////
  MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);

  maptab.add(Segment("reset", RESET_BASE, RESET_SIZE, IntTab(0), true));
  maptab.add(Segment("excep", EXCEP_BASE, EXCEP_SIZE, IntTab(0), true));
  maptab.add(Segment("text" , TEXT_BASE , TEXT_SIZE , IntTab(0), true));

  maptab.add(Segment("data" , DATA_BASE , DATA_SIZE , IntTab(1), true));

  maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));

  maptab.add(Segment("mwmr0", MWMR_BASE, MWMR_SIZE, IntTab(3), false));
  maptab.add(Segment("mwmr_ram", MWMRd_BASE , MWMRd_SIZE , IntTab(4), false));

  maptab.add(Segment("locks" , LOCKS_BASE , LOCKS_SIZE , IntTab(5), false));

  //std::cout<<"Je suis ici"<<std::endl;
  /////////////////////////////////////////////////////////////////////////////
  // LOADER
  /////////////////////////////////////////////////////////////////////////////
  Loader loader("soft/bin.soft");

  /////////////////////////////////////////////////////////////////////////////
  // VGMN
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciVgmn vgmn_1("vgmn", maptab, IntTab(), n_initiators, 6, network_latence * UNIT_TIME);
  ///////////////////////////////////////////////////////////////////////
  //XCACHE
  ///////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> > *xcache;
  xcache= new soclib::tlmdt::VciXcacheWrapper<vci_param, soclib::common::Iss2Simhelper<soclib::common::Mips32ElIss> >("misp0", 0, IntTab(0), maptab, 1, 8, 4, 1, 8, 4, 500*UNIT_TIME);

  xcache->p_vci(*vgmn_1.p_to_initiator[0]);

  //std::cout<<"je suis ici2"<<std::endl;
   /////////////////////////////////////////////////////////////////////////////
  // VciBlackhole Initiator
  ///////////////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > *fake_initiator;
  fake_initiator = new soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> >("fake0", soclib::common::Mips32ElIss::n_irq/*-1*/);
  for(int irq=0; irq<soclib::common::Mips32ElIss::n_irq/*-1*/; irq++){
    (*fake_initiator->p_socket[irq])(*xcache->p_irq[irq/*+1*/]);
  }
  
  //std::cout<<"je suis ici4"<<std::endl;

  /////////////////////////////////////////////////////////////////////////////
  // RAMs
  /////////////////////////////////////////////////////////////////////////////
  //std::cout<<"Je suis ici1"<<std::endl;
  soclib::tlmdt::VciRam<vci_param> ram0("ram0", IntTab(0), maptab, loader);
  (*vgmn_1.p_to_target[0])(ram0.p_vci);
  
  soclib::tlmdt::VciRam<vci_param> ram1("ram1", IntTab(1), maptab, loader);
  (*vgmn_1.p_to_target[1])(ram1.p_vci);

  soclib::tlmdt::VciRam<vci_param> ram2("ram2", IntTab(4), maptab, loader);
  (*vgmn_1.p_to_target[4])(ram2.p_vci);

  ////////////////////////////////////////////////////////////////////////
  //TTY
  ///////////////////////////////////////////////////////////////////////
 
  soclib::tlmdt::VciMultiTty<vci_param> vcitty("vcitty", IntTab(2), maptab, "vcitty0", NULL);
  (*vgmn_1.p_to_target[2])(vcitty.p_vci);

 ///(*vcitty.p_irq_initiator[0])(*xcache->p_irq_target[0]);
  //std::cout<<"je suis ici3"<<std::endl;
  
   /////////////////////////////////////////////////////////////////////////////
  //	// VciBlackhole Target Tagged
  //		/////////////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> > *fake_target_tagged;

  fake_target_tagged = new soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> >("fake_target_tagged", 1);
  //for(int i=0; i<n_initiators; i++){
	(*vcitty.p_irq[0])(*fake_target_tagged->p_socket[0]);
  //} **/
  /****std::cout<<"je suis ici5"<<std::endl;***/
 //////////////////////////////////////////////////////////////////////////
 //LOCKs
 /////////////////////////////////////////////////////////////////////////
 soclib::tlmdt::VciLocks<vci_param> vcilocks("vcilocks", IntTab(5), maptab);
 (*vgmn_1.p_to_target[5])(vcilocks.p_vci);
  //std::cout<<"je suis ici6"<<std::endl;
  /////////////////////////////////////////////////////////////////////////////
  // MWMR AND COPROCESSOR
  /////////////////////////////////////////////////////////////////////////////
  read_depth      = 256;
  write_depth     = 256;
  n_read_channel  = 1;
  n_write_channel = 1;
  n_config        = 0;
  n_status        = 0;

 soclib::tlmdt::VciMwmrController<vci_param> mwmr0("mwmr0", maptab, IntTab(1), IntTab(3), read_depth, write_depth, n_read_channel, n_write_channel, n_config, n_status, simulation_time * UNIT_TIME);
  
  mwmr0.p_vci_initiator(*vgmn_1.p_to_initiator[1]);
  //std::cout<<"je suis ici7"<<std::endl;
  (*vgmn_1.p_to_target[3])(mwmr0.p_vci_target);
  //std::cout<<"je suis ici8"<<std::endl;

  Fir128 fir128("fir128", 0, read_depth/4, write_depth/4, n_read_channel, n_write_channel, n_config, n_status);

  for(uint32_t i=0; i<n_read_channel; i++)
    (*mwmr0.p_from_coproc[i])(*fir128.p_read_fifo[i]);

  for(uint32_t i=0; i<n_write_channel; i++)
    (*mwmr0.p_to_coproc[i])(*fir128.p_write_fifo[i]);

  for(uint32_t i=0; i<n_config; i++)
    (*mwmr0.p_config[i])(*fir128.p_config[i]);

  for(uint32_t i=0; i<n_status; i++)
    (*mwmr0.p_status[i])(*fir128.p_status[i]);
  /////////////////////////////////////////////////////////////////////////////
  // SIMULATION
  /////////////////////////////////////////////////////////////////////////////
  //std::cout<<"je suis ici9"<<std::endl;
  ftime(&initial);

  sc_core::sc_start();  // start the simulation

  ftime(&final);

  std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl;
  
  vcitty.print_stats();

  return 0;
}
