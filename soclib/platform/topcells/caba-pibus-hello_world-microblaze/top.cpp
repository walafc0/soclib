#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "microblaze.h"
#include "vci_xcache_wrapper.h"
#include "ississ2.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_pi_initiator_wrapper.h"
#include "vci_pi_target_wrapper.h"
#include "pibus_bcu.h"

int _main(int argc, char *argv[])
{
   using namespace sc_core;
   // Avoid repeating these everywhere
   using soclib::common::IntTab;
   using soclib::common::Segment;

   // Define our VCI parameters
   typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

   // Mapping table
   soclib::common::MappingTable maptab(32, IntTab(12), IntTab(2), 0x00F00000);

   maptab.add(Segment("tout", 0x00000000, 0x00008000, IntTab(0), true));
   maptab.add(Segment("tty" , 0x00400000, 0x00000258, IntTab(1), false));

   // Signals
   sc_clock    signal_clk("signal_clk");
   sc_signal<bool> signal_resetn("signal_resetn");
   
   sc_signal<bool> signal_mb_it("signal_mb_it"); 

   soclib::caba::Pibus pibus("pibus");
   sc_signal<bool> req("req");
   sc_signal<bool> gnt("gnt");
   sc_signal<bool> sel("sel");
   sc_signal<bool> sel1("sel1");
  
   soclib::caba::VciSignals<vci_param> signal_vci_m("signal_vci_m");

   soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
   soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram("signal_vci_vcimultiram");

   // Components

   typedef soclib::common::IssIss2<soclib::common::MicroBlazeIss> iss_t;
   soclib::caba::VciXcacheWrapper<vci_param, iss_t > mb("mb", 0,maptab,0,1,8,4,1,8,4);

   soclib::common::Loader loader("soft/a.out");
   soclib::caba::VciRam<vci_param> vcimultiram("vcimultiram", IntTab(0), maptab, loader);
   soclib::caba::VciMultiTty<vci_param> vcitty("vcitty", IntTab(1), maptab, "vcitty", NULL);
   
   soclib::caba::PibusBcu bcu("bcu", maptab, 1, 2, 100);
   soclib::caba::VciPiInitiatorWrapper<vci_param> cache_wrapper("cache_wrapper");
   soclib::caba::VciPiTargetWrapper<vci_param> multiram_wrapper("multiram_wrapper");
   soclib::caba::VciPiTargetWrapper<vci_param> tty_wrapper("tty_wrapper");

   // Net-List
 
   mb.p_clk(signal_clk);  
   vcimultiram.p_clk(signal_clk);
   vcitty.p_clk(signal_clk);
   cache_wrapper.p_clk(signal_clk);
   multiram_wrapper.p_clk(signal_clk);
   tty_wrapper.p_clk(signal_clk);
   bcu.p_clk(signal_clk);

   mb.p_resetn(signal_resetn);  
   vcimultiram.p_resetn(signal_resetn);
   vcitty.p_resetn(signal_resetn);
   cache_wrapper.p_resetn(signal_resetn);
   multiram_wrapper.p_resetn(signal_resetn);
   tty_wrapper.p_resetn(signal_resetn);
   bcu.p_resetn(signal_resetn);
  
   mb.p_irq[0](signal_mb_it); 
   mb.p_vci(signal_vci_m);

   vcimultiram.p_vci(signal_vci_vcimultiram);

   vcitty.p_vci(signal_vci_tty);
   vcitty.p_irq[0](signal_mb_it); 

   cache_wrapper.p_gnt(gnt);
   cache_wrapper.p_req(req);
   cache_wrapper.p_pi(pibus);
   cache_wrapper.p_vci(signal_vci_m);

   multiram_wrapper.p_sel(sel);
   multiram_wrapper.p_pi(pibus);
   multiram_wrapper.p_vci(signal_vci_vcimultiram);

   tty_wrapper.p_sel(sel1);
   tty_wrapper.p_pi(pibus);
   tty_wrapper.p_vci(signal_vci_tty);

   bcu.p_req[0](req);
   bcu.p_gnt[0](gnt);
   bcu.p_sel[0](sel);
   bcu.p_sel[1](sel1);

   bcu.p_a(pibus.a);
   bcu.p_lock(pibus.lock);
   bcu.p_ack(pibus.ack);
   bcu.p_tout(pibus.tout);

   int ncycles = 1000000;
#if 0
   if (argc == 2) {
      ncycles = std::atoi(argv[1]);
   } else {
      std::cerr
         << std::endl
         << "The number of simulation cycles must "
            "be defined in the command line"
         << std::endl;
      exit(1);
   }
#endif

   sc_start(0);
   signal_resetn = false;

   sc_start(1);
   signal_resetn = true;

   for (int i = 0; i < ncycles ; i++) {
      sc_start(1);
     
      if( (i % 100000) == 0) 
         std::cout
            << "Time elapsed: "<<i<<" cycles." << std::endl;
   }
   std::cout << "Stopping after "<<ncycles<<" cycles." << std::endl;
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
