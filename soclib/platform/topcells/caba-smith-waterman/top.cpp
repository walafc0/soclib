
/**
 ** MARKER_BEGIN
 ** Auto-generated file, dont modify directly
 ** your changes will be lost !
 **
 ** Generated by DSX on 2009-02-04 14:54:02.955548
 ** by alinev@solo
 ** MARKER_END
 **/
#include <systemc>
#include <sys/time.h>
#include <cstdlib>
#include <cstdarg>
#include <sys/timeb.h>

// 
#include "base_module.h"
#include "loader.h"
#include "iss.h"
#include "iss_simhelper.h"
#include "iss_wrapper.h"
#include "mapping_table.h"
#include "mips.h"
#include "tty.h"
#include "vci_multi_tty.h"
#include "vci_param.h"
#include "vci_ram.h"
#include "vci_signals.h"
#include "vci_vgmn.h"
#include "vci_xcache.h"
#include "xcache_signals.h"
// Component getIncludes
// Configurator getIncludes
// Signal getIncludes

bool stop;
void run(sc_core::sc_signal<bool> &resetn)
{
#ifdef SYSTEMCASS
	sc_core::sc_start(0);
	resetn = false;
	sc_core::sc_start(1);
	resetn = true;
#else
	sc_core::sc_start(sc_core::sc_time(0, sc_core::SC_NS));
	resetn = false;
	sc_core::sc_start(sc_core::sc_time(1, sc_core::SC_NS));
	resetn = true;
#endif

#ifdef SOCVIEW
	debug();
#else
	const char *t = getenv("STATS_EVERY");
	if ( t ) {
		int delta = atoi(t);
		while (!stop) {
			struct timezone tz;
			struct timeval end, begin, tp;
			gettimeofday( &begin, &tz );
#ifdef SYSTEMCASS
			sc_core::sc_start(delta);
#else
			sc_core::sc_start(sc_core::sc_time(delta, sc_core::SC_NS));
#endif
			gettimeofday( &end, &tz );
			timersub( &end, &begin, &tp );
			long ms = (tp.tv_sec*1000+tp.tv_usec/1000);
			std::cout << delta << " cycles in " << ms << " ms: " << ((double)delta*1000/ms) << " c/s" << std::endl;
		}
	} else {
		sc_core::sc_start();
	}
#endif
}

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

std::vector<int> intArray(
    const int length, ... )
{
	int i;
    std::vector<int> ret;
	va_list arg;
	va_start(arg, length);

	for (i=0; i<length; ++i) {
		ret.push_back(va_arg(arg, int));
	};
	va_end(arg);
    return ret;
}

int _main(int argc, char **argv)
{
  struct timeb initial, final;

  size_t simulation_time = std::numeric_limits<uint32_t>::max();
  size_t icache_set      = 1;
  size_t icache_size     = 1024;
  size_t icache_word     = 8;
  size_t dcache_set      = 1;
  size_t dcache_size     = 1024;
  size_t dcache_word     = 8;
  size_t network_latence = 10;
  
  unsigned int ncpu;
  std::string  soft;
  unsigned int n_initiators;

  char * ncpu_env; //env variable that says the number of MIPS processors to be used
  ncpu_env = getenv("N_INITS");
  if (ncpu_env==NULL) {
    printf("WARNING : You have to specify the number of processors to be used in the simulation in the N_INITS variable\n");
    ncpu = 1;
  }else {
    ncpu = atoi( ncpu_env );
  }

  char * simulation_time_env; //env variable that says the number of cycles to be simulated
  simulation_time_env = getenv("SIMULATION_TIME");
  if (simulation_time_env==NULL) {
    printf("WARNING : You have to specify the number of cycles to be simulated in the SIMULATION_TIME variable\n");
  }else {
    simulation_time = atoi( simulation_time_env );
  }

  std::ostringstream s_ncpu; 
  s_ncpu << ncpu;

  soft = "soft/bin"+s_ncpu.str()+"proc.soft";
  n_initiators = ncpu;

  std::cout << "SIMULATION PARAMETERS: nprocs = " << ncpu << " simulation time = " << simulation_time << " network latence = " << network_latence << std::endl;
  std::cout << "CACHE: icache_set = " << icache_set << " icache_word = " << icache_word << " icache size = " << icache_size << " dcache_set = " << dcache_set << " dcache_word = " << dcache_word << " dcache size = " << dcache_size << std::endl << std::endl;

	// Define our VCI parameters
	typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

	// Configurator instanciateOnStack
	soclib::common::Loader loader(soft);
	soclib::common::MappingTable mapping_table(32, soclib::common::IntTab(8), soclib::common::IntTab(8), 0x00200000);
	mapping_table.add(soclib::common::Segment("boot",  0xbfc00000,       2048, soclib::common::IntTab(1), 1));
	mapping_table.add(soclib::common::Segment("cram0", 0x10000000, 0x00100000, soclib::common::IntTab(0), 1));
	mapping_table.add(soclib::common::Segment("cram1", 0x40000000, 0x00100000, soclib::common::IntTab(1), 1));
	mapping_table.add(soclib::common::Segment("excep", 0x80000080,       2048, soclib::common::IntTab(1), 1));
	mapping_table.add(soclib::common::Segment("tty",   0x90200000,         32, soclib::common::IntTab(2), 0));
	mapping_table.add(soclib::common::Segment("uram0", 0x30200000, 0x00100000, soclib::common::IntTab(0), 0));
	mapping_table.add(soclib::common::Segment("uram1", 0x60200000, 0x00100000, soclib::common::IntTab(1), 0));

	// Component instanciateOnStack
	////////////////////////////////////////////////
	// VGMN
	////////////////////////////////////////////////
	soclib::caba::VciVgmn<vci_param >  vgmn0("vgmn0", mapping_table, n_initiators, 3, network_latence, 8);

	////////////////////////////////////////////////
	// XCACHE + MIPS
	////////////////////////////////////////////////
	soclib::caba::IssWrapper<soclib::common::IssSimhelper<soclib::common::MipsElIss> > *mips[ncpu];
	soclib::caba::VciXCache<vci_param>  *xcache[ncpu];

	for (unsigned int i=0 ; i < ncpu ; i++) {
	  std::ostringstream mips_name;
	  mips_name << "mips" << i;
	  mips[i] = new soclib::caba::IssWrapper<soclib::common::IssSimhelper<soclib::common::MipsElIss> >((mips_name.str()).c_str(), i);
	  
	  std::ostringstream xcache_name;
	  xcache_name << "xcache" << i;
	  xcache[i] = new soclib::caba::VciXCache<vci_param>((xcache_name.str()).c_str(), mapping_table, soclib::common::IntTab(i), icache_size, icache_word, dcache_size, dcache_word);
	}
	
	////////////////////////////////////////////////
	// RAM
	////////////////////////////////////////////////
	soclib::caba::VciRam<vci_param>  ram0("ram0", soclib::common::IntTab(0), mapping_table, loader);
	soclib::caba::VciRam<vci_param>  ram1("ram1", soclib::common::IntTab(1), mapping_table, loader);

	////////////////////////////////////////////////
	// TTY
	////////////////////////////////////////////////
	soclib::caba::VciMultiTty<vci_param>  tty0("tty0", soclib::common::IntTab(2), mapping_table, stringArray("tty0", NULL));

	////////////////////////////////////////////////
	// SIGNALS
	////////////////////////////////////////////////
	// Signal instanciateOnStack
	sc_core::sc_clock clock("clock");
	sc_core::sc_signal<bool> resetn("resetn");

	soclib::caba::DCacheSignals *mips_p_dcache_to_xcache_p_dcache[ncpu];
	soclib::caba::ICacheSignals *mips_p_icache_to_xcache_p_icache[ncpu];
	soclib::caba::VciSignals<vci_param> *vgmn0_p_to_initiator_to_xcache_p_vci[ncpu];
	sc_core::sc_signal<bool> *irq[ncpu][6];
	for (unsigned int i=0 ; i < ncpu ; i++) {

	  std::ostringstream vgmn;
	  vgmn << "vgmn0_p_to_initiator_" << i << "_to_xcache" << i << "_p_vci";
	  vgmn0_p_to_initiator_to_xcache_p_vci[i] = new soclib::caba::VciSignals<vci_param>((vgmn.str()).c_str());


	  std::ostringstream mips_dcache;
	  mips_dcache << "mips" << i << "_p_dcache_to_xcache" << i << "_p_dcache";
	  mips_p_dcache_to_xcache_p_dcache[i] = new soclib::caba::DCacheSignals((mips_dcache.str()).c_str());
	  
	  std::ostringstream mips_icache;
	  mips_icache << "mips" << i << "_p_icache_to_xcache" << i << "_p_icache";
	  mips_p_icache_to_xcache_p_icache[i] = new soclib::caba::ICacheSignals((mips_icache.str()).c_str());
	  

	  for (int j = 0; j < soclib::common::MipsElIss::n_irq; j++){
	    std::ostringstream irq_name;
	    irq_name << "irq" << i << "_" << j;
	    irq[i][j] = new sc_core::sc_signal<bool>((irq_name.str()).c_str());
	  }
	}

	soclib::caba::VciSignals<vci_param>  ram0_p_vci_to_vgmn0_p_to_target_0_("ram0_p_vci_to_vgmn0_p_to_target_0_");
	soclib::caba::VciSignals<vci_param>  ram1_p_vci_to_vgmn0_p_to_target_1_("ram1_p_vci_to_vgmn0_p_to_target_1_");
	soclib::caba::VciSignals<vci_param>  tty0_p_vci_to_vgmn0_p_to_target_2_("tty0_p_vci_to_vgmn0_p_to_target_2_");


	////////////////////////////////////////////////
	// CONNECTIONS
	////////////////////////////////////////////////
	vgmn0.p_clk(clock);
	vgmn0.p_resetn(resetn);
	for (unsigned int i=0 ; i < ncpu ; i++)
	  vgmn0.p_to_initiator[i](*vgmn0_p_to_initiator_to_xcache_p_vci[i]);
	vgmn0.p_to_target[0](ram0_p_vci_to_vgmn0_p_to_target_0_);
	vgmn0.p_to_target[1](ram1_p_vci_to_vgmn0_p_to_target_1_);
	vgmn0.p_to_target[2](tty0_p_vci_to_vgmn0_p_to_target_2_);


	for (unsigned int i=0 ; i < ncpu ; i++) {

	  mips[i]->setCacheInfo(xcache[i]->getCacheInfo());
	  mips[i]->p_clk(clock);
	  mips[i]->p_resetn(resetn);
	  mips[i]->p_dcache(*mips_p_dcache_to_xcache_p_dcache[i]);
	  mips[i]->p_icache(*mips_p_icache_to_xcache_p_icache[i]);
	  for (int j = 0; j < soclib::common::MipsElIss::n_irq; j++)
	    mips[i]->p_irq[j](*irq[i][j]);


	  xcache[i]->p_clk(clock);
	  xcache[i]->p_resetn(resetn);
	  xcache[i]->p_dcache(*mips_p_dcache_to_xcache_p_dcache[i]);
	  xcache[i]->p_icache(*mips_p_icache_to_xcache_p_icache[i]);
	  xcache[i]->p_vci(*vgmn0_p_to_initiator_to_xcache_p_vci[i]);
	}

	ram0.p_clk(clock);
	ram0.p_resetn(resetn);
	ram0.p_vci(ram0_p_vci_to_vgmn0_p_to_target_0_);
	ram1.p_clk(clock);
	ram1.p_resetn(resetn);
	ram1.p_vci(ram1_p_vci_to_vgmn0_p_to_target_1_);
	tty0.p_clk(clock);
	tty0.p_irq[0](*new sc_core::sc_signal<bool>("tty0_p_irq_0_"));
	tty0.p_resetn(resetn);
	tty0.p_vci(tty0_p_vci_to_vgmn0_p_to_target_2_);


	////////////////////////////////////////////////
	// SIMULATION
	////////////////////////////////////////////////
	ftime(&initial);
	run(resetn);
	ftime(&final);

	std::cout << "Execution Time = " << (int)((1000.0 * (final.time - initial.time))+ (final.millitm - initial.millitm)) << std::endl << std::endl;

	ram0.print_stats();
	ram1.print_stats();
	tty0.print_stats();

	return 0;
}

void quit(int)
{
	sc_core::sc_stop();
    stop = true;
}

int sc_main (int argc, char *argv[])
{
	signal(SIGINT, quit);
	atexit(sc_core::sc_stop);
    stop = false;

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
