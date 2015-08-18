#include <iostream>
#include <cstdlib>

#include "mapping_table.h"
#include "microblaze.h"
#include "vci_xcache_wrapper.h"
#include "ississ2.h"
#include "vci_pci.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_vgmn.h"
#define MBTRACE

int _main(int argc, char *argv[])
{
   using namespace sc_core;
   // Avoid repeating these everywhere
   using soclib::common::IntTab;
   using soclib::common::Segment;

   // Define our VCI parameters
   typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

   // Mapping table
   soclib::common::MappingTable maptab(32, IntTab(12), IntTab(3), 0x00F00000);
   maptab.add(Segment("tout" , 0x00000000, 0x0000FFFF, IntTab(0), true));
   maptab.add(Segment("tty"  , 0x00400000, 0x00000258, IntTab(1), false));
   maptab.add(Segment("pci", 0x00500000, 0x000FFFF, IntTab(2), false));


   // Signals
   sc_clock    signal_clk("signal_clk");
   sc_signal<bool> signal_resetn("signal_resetn");
   
   sc_signal<bool> signal_mb0_it("signal_mb0_it"); 
   sc_signal<bool> signal_mb1_it("signal_mb1_it"); 
   sc_signal<bool> signal_mb2_it("signal_mb2_it"); 
   sc_signal<bool> signal_mb3_it("signal_mb3_it"); 
   
   sc_signal<bool> signal_pci_it("signal_pci_it"); 
   
   soclib::caba::VciSignals<vci_param> signal_vci_m0("signal_vci_m0");
   soclib::caba::VciSignals<vci_param> signal_vci_m1("signal_vci_m1");
   soclib::caba::VciSignals<vci_param> signal_vci_m2("signal_vci_m2");
   soclib::caba::VciSignals<vci_param> signal_vci_m3("signal_vci_m3");

   soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
   soclib::caba::VciSignals<vci_param> signal_vci_vcimultiram0("signal_vci_vcimultiram0");
   soclib::caba::VciSignals<vci_param> signal_vci_vcipci0("signal_vci_vcipci0");
   soclib::caba::VciSignals<vci_param> signal_vci_vcipci1("signal_vci_vcipci1");
   
   sc_signal<bool> signal_tty_irq0("signal_tty_irq0"); 
   sc_signal<bool> signal_tty_irq1("signal_tty_irq1"); 
   sc_signal<bool> signal_tty_irq2("signal_tty_irq2"); 
   sc_signal<bool> signal_tty_irq3("signal_tty_irq3"); 
   
   		sc_signal_rv<4>   Cbe("Cbe")      ;
   			sc_signal<bool> Idsel    ;
   			sc_signal<bool> Idsel2    ;
   			sc_signal_resolved Frame("Frame")   ;
   			sc_signal_resolved Devsel("Devsel")  ;
   			sc_signal_resolved Irdy("Irdy")    ;
   			sc_signal<bool> Req1("Req1")    ;
   			sc_signal_resolved Trdy("Trdy")   ;  
   			sc_signal_resolved Inta    ;
   			sc_signal_resolved Stop("Stop")    ;
   			sc_signal<bool> Req0("Req0")       ;
   			//sc_inout<bool> Req64	;
   			sc_signal_resolved Par("Par")      ;
   			sc_signal_rv<32>   AD32("AD32")       ;

   // Components
   typedef soclib::common::IssIss2<soclib::common::MicroBlazeIss> iss_t;

   soclib::caba::VciXcacheWrapper<vci_param, iss_t > mb0("mb0", 0,maptab,IntTab(0),1,8,4,1,8,4);
   soclib::caba::VciXcacheWrapper<vci_param, iss_t > mb1("mb1", 1,maptab,IntTab(1),1,8,4,1,8,4);
   soclib::caba::VciXcacheWrapper<vci_param, iss_t > mb2("mb2", 2,maptab,IntTab(2),1,8,4,1,8,4);
   soclib::caba::VciXcacheWrapper<vci_param, iss_t > mb3("mb3", 3,maptab,IntTab(3),1,8,4,1,8,4);

   soclib::common::Loader loader("soft/bin.soft");
   soclib::caba::VciRam<vci_param> vcimultiram0("vcimultiram0", IntTab(0), maptab, loader);
   soclib::caba::VciMultiTty<vci_param> vcitty("vcitty", IntTab(1), maptab, "vcitty0", "vcitty1", "vcitty2", "vcitty3", NULL);
   soclib::caba::VciPci<vci_param> vcipci0("vcipci0", IntTab(2), maptab, 23);
   soclib::caba::VciPci<vci_param> vcipci1("vcipci1", IntTab(4), maptab, 23);
   
   soclib::caba::VciVgmn<vci_param> vgmn("vgmn",maptab, 4, 3, 2, 8);

   // Net-List
 
   mb0.p_clk(signal_clk);  
   mb1.p_clk(signal_clk);  
   mb2.p_clk(signal_clk);  
   mb3.p_clk(signal_clk);  
   vcimultiram0.p_clk(signal_clk);
   vcipci0.p_clk(signal_clk);
   vcipci1.p_clk(signal_clk);
   
   mb0.p_resetn(signal_resetn);  
   mb1.p_resetn(signal_resetn);  
   mb2.p_resetn(signal_resetn);  
   mb3.p_resetn(signal_resetn);  
   vcimultiram0.p_resetn(signal_resetn);
   vcipci0.p_resetn(signal_resetn);
   vcipci1.p_resetn(signal_resetn);
   
   mb0.p_irq[0](signal_mb0_it); 
   mb1.p_irq[0](signal_mb1_it); 
   mb2.p_irq[0](signal_mb2_it); 
   mb3.p_irq[0](signal_mb3_it); 

   mb0.p_vci(signal_vci_m0);
   mb1.p_vci(signal_vci_m1);
   mb2.p_vci(signal_vci_m2);
   mb3.p_vci(signal_vci_m3);

   vcimultiram0.p_vci(signal_vci_vcimultiram0);

   vcipci0.p_vci(signal_vci_vcipci0);
   vcipci0.p_irq(signal_mb0_it); 
         vcipci0.p_Cbe(Cbe);
          vcipci0.p_clkpci(signal_clk);
       vcipci0.p_Sysrst(signal_resetn);
        vcipci0.p_Idsel(Idsel);
         vcipci0.p_Frame(Frame);
          vcipci0.p_Devsel(Devsel);
           vcipci0.p_Irdy(Irdy);
           vcipci0.p_Gnt(Req0);
            vcipci0.p_Trdy(Trdy);
             vcipci0.p_Inta(Inta);
              vcipci0.p_Stop(Stop);
               vcipci0.p_Req(Req0);
               vcipci0.p_Par(Par);
                vcipci0.p_AD32(AD32);
  
   vcipci1.p_vci(signal_vci_vcipci1);
   vcipci1.p_irq(signal_mb1_it); 
           vcipci1.p_Cbe(Cbe);
          vcipci1.p_clkpci(signal_clk);
       vcipci1.p_Sysrst(signal_resetn);
        vcipci1.p_Idsel(Idsel2);
         vcipci1.p_Frame(Frame);
          vcipci1.p_Devsel(Devsel);
           vcipci1.p_Irdy(Irdy);
           vcipci1.p_Gnt(Req1);
            vcipci1.p_Trdy(Trdy);
             vcipci1.p_Inta(Inta);
              vcipci1.p_Stop(Stop);
               vcipci1.p_Req(Req1);
               vcipci1.p_Par(Par);
                vcipci1.p_AD32(AD32); 
   vcitty.p_clk(signal_clk);
   vcitty.p_resetn(signal_resetn);
   vcitty.p_vci(signal_vci_tty);
   vcitty.p_irq[0](signal_tty_irq0); 
   vcitty.p_irq[1](signal_tty_irq1); 
   vcitty.p_irq[2](signal_tty_irq2); 
   vcitty.p_irq[3](signal_tty_irq3); 

   vgmn.p_clk(signal_clk);
   vgmn.p_resetn(signal_resetn);

   vgmn.p_to_initiator[0](signal_vci_m0);
   vgmn.p_to_initiator[1](signal_vci_m1);
   vgmn.p_to_initiator[2](signal_vci_m2);
   vgmn.p_to_initiator[3](signal_vci_m3);

   vgmn.p_to_target[0](signal_vci_vcimultiram0);
   vgmn.p_to_target[1](signal_vci_tty);
   vgmn.p_to_target[2](signal_vci_vcipci0);
   

#ifdef MBTRACE
   sc_trace_file *tf = sc_create_vcd_trace_file("sc_dump");
 
   sc_trace(tf, signal_clk,    "clk");
   sc_trace(tf, AD32, "AD32");   
   sc_trace(tf, Frame, "Frame");
      sc_trace(tf, Devsel, "Devsel");
        sc_trace(tf, Irdy, "Irdy"); 
        sc_trace(tf, Trdy, "Trdy"); 
        sc_trace(tf, Par, "Par"); 
        sc_trace(tf, Req0, "Req0");
	      sc_trace(tf, Req1, "Req1");
      sc_trace(tf, Stop, "Stop");
  sc_trace(tf, Cbe, "Cbe");
	 
 //  sc_trace(tf, signal_vci_m0.address, "vci_m0_addr");
#endif

   int ncycles = 1000000;
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
#ifdef MBTRACE
   sc_close_vcd_trace_file(tf);
#endif
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
