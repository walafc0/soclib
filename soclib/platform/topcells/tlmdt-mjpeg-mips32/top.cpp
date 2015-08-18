#include <iostream>
#include <systemc>
#include <sys/time.h>
#include <cstdlib>
#include <cstdarg>
#include <sys/timeb.h>
#include <limits>

#include "loader.h"           	
#include "mapping_table.h"
#include "vci_vgmn.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_xcache_wrapper.h"
#include "iss2.h"
#include "iss2_simhelper.h"
#include "mips32.h"
#include "vci_mwmr_controller.h"
#include "fifo_reader.h"
#include "fifo_writer.h"
#include "vci_blackhole.h"

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
  typedef soclib::tlmdt::VciParams<uint32_t,uint32_t> vci_param;
  typedef soclib::common::Mips32ElIss iss_t;
  typedef soclib::common::Iss2Simhelper<iss_t> iss_simhelper_t;

  struct timeb initial, final;

  size_t read_depth, write_depth, n_read_channel, n_write_channel, n_config, n_status;
  size_t dcache_size     = 32;
  size_t icache_size     = 32;
  size_t network_latence = 3;

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

  std::cout << "SIMULATION PARAMETERS: number of initiators = " << n_initiators << " icache size = " << icache_size << " dcache size = " << dcache_size << " network latence = " << network_latence << std::endl << std::endl;

  /////////////////////////////////////////////////////////////////////////////
  // MAPPING TABLE
  /////////////////////////////////////////////////////////////////////////////
  soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x00300000);
  maptab.add(soclib::common::Segment("boot",        0xbfc00000,       2048, soclib::common::IntTab(1), 1));
  maptab.add(soclib::common::Segment("cram0",       0x10000000, 0x00100000, soclib::common::IntTab(0), 1));
  maptab.add(soclib::common::Segment("cram1",       0x20000000, 0x00100000, soclib::common::IntTab(1), 1));
  maptab.add(soclib::common::Segment("excep",       0x80000080,       2048, soclib::common::IntTab(1), 1));
  maptab.add(soclib::common::Segment("ramdac_ctrl", 0x71200000,        256, soclib::common::IntTab(4), 0));
  maptab.add(soclib::common::Segment("tg_ctrl",     0x70200000,        256, soclib::common::IntTab(3), 0));
  maptab.add(soclib::common::Segment("tty0",        0x90200000,         32, soclib::common::IntTab(2), 0));
  maptab.add(soclib::common::Segment("uram0",       0x10200000, 0x00100000, soclib::common::IntTab(0), 0));
  maptab.add(soclib::common::Segment("uram1",       0x20200000, 0x00100000, soclib::common::IntTab(1), 0));

  /////////////////////////////////////////////////////////////////////////////
  // LOADER
  /////////////////////////////////////////////////////////////////////////////
  std::ostringstream soft;
  soft << "soft/bin" << n_initiators << "proc.soft";
  soclib::common::Loader loader(soft.str());

  /////////////////////////////////////////////////////////////////////////////
  // VCI_VGMN
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciVgmn vgmn_1("vgmn", maptab, n_initiators+2, 5, network_latence, 8);
 
  /////////////////////////////////////////////////////////////////////////////
  // VCI_XCACHE_WRAPPER
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciXcacheWrapper<vci_param, iss_simhelper_t > *xcache[n_initiators]; 
  for (int i=0 ; i < n_initiators ; i++) {
    std::ostringstream cpu_name;
    cpu_name << "xcache" << i;
    xcache[i] = new soclib::tlmdt::VciXcacheWrapper<vci_param, iss_simhelper_t>((cpu_name.str()).c_str(), i, maptab, IntTab(i), 1, icache_size, 8, 1, dcache_size, 8);
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // MWMR AND COPROCESSOR TG
  /////////////////////////////////////////////////////////////////////////////
  read_depth      = 0;
  write_depth     = 8;
  n_read_channel  = 0;
  n_write_channel = 1;
  n_config        = 0;
  n_status        = 0;

  soclib::tlmdt::VciMwmrController<vci_param> tg_ctrl("tg_ctrl", maptab, IntTab(n_initiators), IntTab(3), read_depth, write_depth, n_read_channel, n_write_channel, n_config, n_status, simulation_time * UNIT_TIME);

  soclib::tlmdt::FifoReader<vci_param> tg("tg", "bash", stringArray("bash", "-c", "while cat \"plan.jpg\" ; do true ; done", NULL),write_depth);

  /////////////////////////////////////////////////////////////////////////////
  // MWMR AND COPROCESSOR RAMDAC
  /////////////////////////////////////////////////////////////////////////////
  read_depth      = 320;
  write_depth     = 0;
  n_read_channel  = 1;
  n_write_channel = 0;
  n_config        = 0;
  n_status        = 0;

  soclib::tlmdt::VciMwmrController<vci_param> ramdac_ctrl("ramdac_ctrl", maptab, IntTab( n_initiators+1 ), IntTab(4), read_depth, write_depth, n_read_channel, n_write_channel, n_config, n_status, simulation_time * UNIT_TIME);

  soclib::tlmdt::FifoWriter<vci_param> ramdac("ramdac", "soclib-pipe2fb", stringArray("soclib-pipe2fb", "64", "64", NULL),read_depth);

  /////////////////////////////////////////////////////////////////////////////
  // TARGET - RAM
  /////////////////////////////////////////////////////////////////////////////
  int n_rams = 2;
  soclib::tlmdt::VciRam<vci_param> *ram[32];
  for (int i=0 ; i<n_rams; i++) {
    std::ostringstream name;
    name << "ram" << i;
    ram[i] = new soclib::tlmdt::VciRam<vci_param>((name.str()).c_str(), IntTab(i), maptab, loader);
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // TARGET - TTY
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciMultiTty<vci_param> vcitty("tty0", IntTab(2), maptab, "TTY0", NULL);

  /////////////////////////////////////////////////////////////////////////////
  // CONNECTIONS
  /////////////////////////////////////////////////////////////////////////////
  for (int i=0 ; i < n_initiators ; i++) {
    xcache[i]->p_vci(*vgmn_1.p_to_initiator[i]);
  }
  tg_ctrl.p_vci_initiator(*vgmn_1.p_to_initiator[n_initiators]);
  (*tg_ctrl.p_to_coproc[0])(tg.p_fifo);
  ramdac_ctrl.p_vci_initiator(*vgmn_1.p_to_initiator[n_initiators+1]);
  (*ramdac_ctrl.p_from_coproc[0])(ramdac.p_fifo);

  for (int i=0 ; i<n_rams; i++) {
    (*vgmn_1.p_to_target[i])(ram[i]->p_vci);
  }

  (*vgmn_1.p_to_target[2])(vcitty.p_vci);
  (*vgmn_1.p_to_target[3])(tg_ctrl.p_vci_target);
  (*vgmn_1.p_to_target[4])(ramdac_ctrl.p_vci_target);

  /////////////////////////////////////////////////////////////////////////////
  // VciBlackhole Target Tagged
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> > *fake_target_tagged;

  fake_target_tagged = new soclib::tlmdt::VciBlackhole<tlm_utils::simple_target_socket_tagged<soclib::tlmdt::VciBlackholeBase, 32, tlm::tlm_base_protocol_types> >("fake_target_tagged", 1);

  (*vcitty.p_irq[0])(*fake_target_tagged->p_socket[0]);

  /////////////////////////////////////////////////////////////////////////////
  // VciBlackhole Initiator
  /////////////////////////////////////////////////////////////////////////////
  soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> > *fake_initiator[n_initiators];

  for (int i=0 ; i < n_initiators ; i++) {
    std::ostringstream fake_name;
    fake_name << "fake" << i;
    fake_initiator[i] = new soclib::tlmdt::VciBlackhole<tlm::tlm_initiator_socket<> >((fake_name.str()).c_str(), iss_t::n_irq);
    
    for(unsigned int irq=0; irq<iss_t::n_irq; irq++){
      (*fake_initiator[i]->p_socket[irq])(*xcache[i]->p_irq[irq]);
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // SIMULATION
  /////////////////////////////////////////////////////////////////////////////
  ftime(&initial);
  sc_core::sc_start();  // start the simulation
  ftime(&final);

  std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl << std::endl;

  for (int i=0 ; i<n_rams; i++) {
    ram[i]->print_stats();
  }

  vcitty.print_stats();

  return 0;
}
