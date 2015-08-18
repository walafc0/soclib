///////////////////////////////////////////////////////////////////////////////////
// File     : top.cpp for the "caba_vgmn_vcache-nic-dma" architecture
// Date     : 01/04/2012
// Author   : alain greiner
// Copyright (c) UPMC-LIP6
///////////////////////////////////////////////////////////////////////////////////
// This architecture supports multi-processing, and virtual memory.
// It has been designed to run the GIET V2.0. 
// - The processors are MIPS32 supporting the SoCLib generic MMU.
// - The interconnect is the SoCLib VGMN : flat 32 bits address space. 
// - It supports NB_VMS virtual machines: NB_VMS cannot be larger than 4.
// - It contains a VCI_MULTI_NIC peripheral: number of channels is NB_VMS. 
// - It contains a VCI_CHBUF_DMA peripheral: number of channels is 2*NB_VMS.
// - It contains an IO controler and a Frame buffer.
// - Processor 0 is running the hypervisor, and there is one private processor
//   per virtual machine: number of processors is NB_VMS+1.
// - There is one privaye TTY per processor: number of TTY terminals is NB_VMS+1
// - There is one private VCI_TIMER per processor: number of timers is NB_VMS+1
// - It contains a VCI_MULTI_ICU supporting up to 32 IN_IRQs:
//   IRQ[Ø]             : IOC   
//   IRQ[4]  to IRQ[8]  : up to 5 TTY(s)    
//   IRQ[8]  to IRQ[13] : up to 5 TIMER(s) 
//   IRQ[16] to IRQ[19] : up to 4 DMA_RX(s) 
//   IRQ[20] to IRQ[23] : up to 4 DMA_TX(s)
///////////////////////////////////////////////////////////////////////////////////

#include <systemc>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "vci_signals.h"
#include "vci_param.h"
#include "mapping_table.h"
#include "gdbserver.h"

#include "mips32.h"
#include "vci_vgmn.h"
#include "vci_vcache_wrapper.h"
#include "vci_multi_tty.h"
#include "vci_timer.h"
#include "vci_multi_icu.h"
#include "vci_chbuf_dma.h"
#include "vci_multi_nic.h"
#include "vci_block_device.h"
#include "vci_framebuffer.h"
#include "vci_simple_ram.h"
#include "alloc_elems.h"
#include "loader.h"

#define PSEG_ROM_BASE    0xBFC00000
#define PSEG_ROM_SIZE    0x00010000    // ROM de boot: 64 Kbytes

#define PSEG_RAM_BASE    0x00000000
#define PSEG_RAM_SIZE    0x00100000    // RAM : 1 Mbytes

#define PSEG_TTY_BASE    0x90000000
#define PSEG_TTY_SIZE    0x00001000 

#define PSEG_TIM_BASE    0x91000000
#define PSEG_TIM_SIZE    0x00001000 

#define PSEG_IOC_BASE    0x92000000
#define PSEG_IOC_SIZE    0x00001000

#define PSEG_DMA_BASE    0x93000000
#define PSEG_DMA_SIZE    0x00008000

#define PSEG_NIC_BASE    0x97000000
#define PSEG_NIC_SIZE    0x00080000

#define PSEG_FBF_BASE    0x96000000
#define PSEG_FBF_SIZE    0x00200000  

#define PSEG_ICU_BASE    0x9F000000
#define PSEG_ICU_SIZE    0x00001000 

#define ROM_TGTID      0
#define RAM_TGTID      1
#define TIM_TGTID      2
#define FBF_TGTID      3
#define IOC_TGTID      4
#define DMA_TGTID      5
#define NIC_TGTID      6
#define TTY_TGTID      7
#define ICU_TGTID      8

#define NB_VMS         0

#define NB_PROCS       NB_VMS + 1

#define DMA_SRCID      NB_PROCS 
#define IOC_SRCID      NB_PROCS + 1

#define DEFAULT_MAC_4  0xBABEF00D
#define DEFAULT_MAC_2  0x0000BEEF

int _main(int argc, char *argv[])
{
    using namespace sc_core;
    using namespace soclib::caba;
    using namespace soclib::common;

    // VCI fields width definition
    //  cell_size   = 4;
    //  plen_size   = 8;
    //  addr_size   = 32;
    //  rerror_size = 1;
    //  clen_size   = 1;
    //  rflag_size  = 1;
    //  srcid_size  = 12;
    //  pktid_size  = 1;
    //  trdid_size  = 4;
    //  wrplen_size = 1;

    typedef VciParams<4,8,32,1,1,1,12,1,4,1> vci_param;

    char    soft_name[256]     = "soft/boot.elf";
    char    ioc_filename[256]  = "soft/boot.elf";

    size_t  n_cycles        = 1000000000;       // simulated cycles
    size_t  tlb_ways        = 8;                // Itlb & Dtlb parameters
    size_t  tlb_sets        = 8;
    size_t  icache_sets     = 256;              // Icache parameters
    size_t  icache_words    = 16;
    size_t  icache_ways     = 4;
    size_t  dcache_sets     = 256;              // Dcache parameters
    size_t  dcache_words    = 16;
    size_t  dcache_ways     = 4;
    size_t  wbuf_nlines     = 4;                // Write Buffer parameters
    size_t  wbuf_nwords     = 4;
    size_t  ram_latency     = 0;                // Ram latency (L2 MISS emulation)
    size_t  fbf_size        = 128;              // number of lines = number of pixels
    bool    debug_ok        = false;            // debug activated
    size_t  from_cycle      = 0;                // debug start cycle
    bool    trace_ok        = false;            // cache trace activated
    char    trace_filename[256];
    FILE*   trace_file      = NULL;
    bool    stats_ok        = false;            // statistics activated
    char    stats_filename[256];
    FILE*   stats_file      = NULL;

    std::cout << std::endl << "********************************" << std::endl;
    std::cout << std::endl << "*** caba-vgmn-vcache-nic-dma ***" << std::endl;
    std::cout << std::endl << "********************************" << std::endl;

    if (argc > 1)
    {
        for( int n=1 ; n<argc ; n=n+2 )
        {
            if( (strcmp(argv[n],"-NCYCLES") == 0) && (n+1<argc) )
            {
                n_cycles = atoi(argv[n+1]);
            }
            else if( (strcmp(argv[n],"-TRACE") == 0) && (n+1<argc) )
            {
                trace_ok = true;
                if(n_cycles > 10000) n_cycles = 10000;
                strcpy(trace_filename, argv[n+1]);
                trace_file = fopen(trace_filename,"w+");
            }
            else if( (strcmp(argv[n],"-STATS") == 0) && (n+1<argc) )
            {
                stats_ok = true;
                strcpy(stats_filename, argv[n+1]);
                stats_file = fopen(stats_filename,"w+");
            }
            else if( (strcmp(argv[n],"-DEBUG") == 0) && (n+1<argc) )
            {
                debug_ok = true;
                from_cycle = atoi(argv[n+1]);
            }
            else if( (strcmp(argv[n],"-IOCFILE") == 0) && (n+1<argc) )
            {
                strcpy(ioc_filename, argv[n+1]) ;
            }
            else
            {
                std::cout << "   Arguments on the command line are (key,value)" << std::endl;
                std::cout << "   The order is not important." << std::endl;
                std::cout << "   Accepted arguments are :" << std::endl << std::endl;
                std::cout << "   -NCYCLES number_of_simulated_cycles" << std::endl;
                std::cout << "   -IOCFILE file_name" << std::endl;
                std::cout << "   -TRACE file_name" << std::endl;
                std::cout << "   -STATS file_name" << std::endl;
                std::cout << "   -DEBUG debug_start_cycle" << std::endl;
                exit(0);
            }
        }
    }
    std::cout << std::endl;
    std::cout << "    n_cycles     = " << n_cycles << std::endl;
    std::cout << "    icache_sets  = " << icache_sets << std::endl;
    std::cout << "    icache_words = " << icache_words << std::endl;
    std::cout << "    icache_ways  = " << icache_ways << std::endl;
    std::cout << "    dcache_sets  = " << dcache_sets << std::endl;
    std::cout << "    dcache_words = " << dcache_words << std::endl;
    std::cout << "    dcache_ways  = " << dcache_ways << std::endl;
    std::cout << "    ram_latency  = " << ram_latency << std::endl;
    if(trace_ok) std::cout << "    trace_file   = " << trace_filename << std::endl;
    if(stats_ok) std::cout << "    stats_file   = " << stats_filename << std::endl;

    /*  limitation are related to ICU inputs */
    if ( NB_VMS > 4 )
    {
        std::cout << std::endl;
        std::cout << "The number of VM cannot be larger than 4" << std::endl;
        exit(0);
    }

    //////////////////////////////////////////////////////////////////////////
    // Mapping Table
    //////////////////////////////////////////////////////////////////////////
    MappingTable maptab(32, IntTab(16), IntTab(12), 0xFFF00000);

    maptab.add(Segment("seg_rom", PSEG_ROM_BASE, PSEG_ROM_SIZE, IntTab(ROM_TGTID) , true));
    maptab.add(Segment("seg_ram", PSEG_RAM_BASE, PSEG_RAM_SIZE, IntTab(RAM_TGTID) , true));
    maptab.add(Segment("seg_tim", PSEG_TIM_BASE, PSEG_TIM_SIZE, IntTab(TIM_TGTID) , false));
    maptab.add(Segment("seg_dma", PSEG_DMA_BASE, PSEG_DMA_SIZE, IntTab(DMA_TGTID) , false));
    maptab.add(Segment("seg_nic", PSEG_NIC_BASE, PSEG_NIC_SIZE, IntTab(NIC_TGTID) , false));
    maptab.add(Segment("seg_fbf", PSEG_FBF_BASE, PSEG_FBF_SIZE, IntTab(FBF_TGTID) , false));
    maptab.add(Segment("seg_ioc", PSEG_IOC_BASE, PSEG_IOC_SIZE, IntTab(IOC_TGTID) , false));
    maptab.add(Segment("seg_tty", PSEG_TTY_BASE, PSEG_TTY_SIZE, IntTab(TTY_TGTID) , false));
    maptab.add(Segment("seg_icu", PSEG_ICU_BASE, PSEG_ICU_SIZE, IntTab(ICU_TGTID) , false));

    std::cout << std::endl << maptab << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Signals
    //////////////////////////////////////////////////////////////////////////
    sc_clock        signal_clk("signal_clk", sc_time( 1, SC_NS ), 0.5 );
    sc_signal<bool> signal_resetn("signal_resetn");

    VciSignals<vci_param> *signal_vci_init_proc =
        alloc_elems<VciSignals<vci_param> >("signal_vci_init_proc", NB_PROCS);

    VciSignals<vci_param> signal_vci_init_dma("signal_vci_init_dma");
    VciSignals<vci_param> signal_vci_init_ioc("signal_vci_init_ioc");

    VciSignals<vci_param> signal_vci_tgt_rom("signal_vci_tgt_rom");
    VciSignals<vci_param> signal_vci_tgt_ram("signal_vci_tgt_ram");
    VciSignals<vci_param> signal_vci_tgt_tim("signal_vci_tgt_tim");
    VciSignals<vci_param> signal_vci_tgt_fbf("signal_vci_tgt_fbf");
    VciSignals<vci_param> signal_vci_tgt_ioc("signal_vci_tgt_ioc");
    VciSignals<vci_param> signal_vci_tgt_dma("signal_vci_tgt_dma");
    VciSignals<vci_param> signal_vci_tgt_nic("signal_vci_tgt_nic");
    VciSignals<vci_param> signal_vci_tgt_icu("signal_vci_tgt_icu");
    VciSignals<vci_param> signal_vci_tgt_tty("signal_vci_tgt_tty");

    sc_signal<bool> signal_false("signal_false");

    sc_signal<bool> *signal_irq_proc =
        alloc_elems<sc_signal<bool> >("signal_irq_proc", NB_PROCS);

    sc_signal<bool> *signal_irq_tim =
        alloc_elems<sc_signal<bool> >("signal_irq_tim", NB_PROCS);

    sc_signal<bool> *signal_irq_tty =
        alloc_elems<sc_signal<bool> >("signal_irq_tty", NB_PROCS);

    sc_signal<bool> *signal_irq_dma =
        alloc_elems<sc_signal<bool> >("signal_irq_dma", 8);

    sc_signal<bool> *signal_irq_nic_rx =
        alloc_elems<sc_signal<bool> >("signal_irq_proc", 4);

    sc_signal<bool> *signal_irq_nic_tx =
        alloc_elems<sc_signal<bool> >("signal_irq_proc", 4);

    sc_signal<bool> signal_irq_ioc("signal_irq_ioc");

    //////////////////////////////////////////////////////////////////////////
    // VCI Components : (NB_PROCS+2) initiators / (9) targets
    // The IOC & DMA components are both initiator & target.
    //////////////////////////////////////////////////////////////////////////

    Loader loader(soft_name);

    GdbServer<Mips32ElIss>::set_loader(&loader);

    std::cout << std::endl;

    ////////////////////////////////////////////////////////////////////////
    VciVcacheWrapper<vci_param, GdbServer<Mips32ElIss> >* proc[NB_PROCS];
    for( size_t p = 0 ; p < NB_PROCS ; p++ )
        {
            std::ostringstream proc_name;
            proc_name << "proc_" << p;
            proc[p] = new VciVcacheWrapper< vci_param, GdbServer<Mips32ElIss> >
                          ( proc_name.str().c_str(),
                            p,
                            maptab,
                            IntTab(p),
                            tlb_ways, tlb_sets,
                            icache_ways, icache_sets, icache_words,
                            dcache_ways, dcache_sets, dcache_words,
                            wbuf_nlines, wbuf_nwords,
                            1000,           // max frozen cycles
                            from_cycle,     // debug_start_cycle
                            debug_ok );     //  detailed debug activation

            std::cout << "proc " << std::dec << p << " constructed" << std::endl;
        }

    /////////////////////////////
    VciSimpleRam<vci_param>* ram;
    ram = new VciSimpleRam<vci_param>("ram",
                                      IntTab(RAM_TGTID),
                                      maptab,
                                      loader,
                                      ram_latency);

    std::cout << "ram constructed" << std::endl;
    
    /////////////////////////////
    VciSimpleRam<vci_param>* rom;
    rom = new VciSimpleRam<vci_param>("rom",
                                      IntTab(ROM_TGTID),
                                      maptab,
                                      loader);

    std::cout << "rom constructed" << std::endl;

    ////////////////////////////
    VciMultiTty<vci_param> *tty;
    std::vector<std::string> vect_names;
    for( size_t p = 0 ; p < (NB_PROCS) ; p++ )
    {
        std::ostringstream term_name;
        term_name <<  "term" << p;
        vect_names.push_back(term_name.str().c_str());
    }
    tty = new VciMultiTty<vci_param>("tty",
                                     IntTab(TTY_TGTID),
                                     maptab,
                                     vect_names);

    std::cout << "tty constructed" << std::endl;

    ////////////////////////////
    VciMultiIcu<vci_param> *icu;
    icu = new VciMultiIcu<vci_param>("icu",
                                     IntTab(ICU_TGTID),
                                     maptab,
                                     32,
                                     NB_PROCS);

    std::cout << "icu constructed" << std::endl;

    ///////////////////////////
    VciTimer<vci_param>* timer;
    timer = new VciTimer<vci_param>("timer",
                                    IntTab(TIM_TGTID),
                                    maptab,
                                    NB_PROCS);

    std::cout << "timer constructed" << std::endl;

    ////////////////////////////
    VciChbufDma<vci_param>* dma;
    dma = new VciChbufDma<vci_param>("dma",
                                     maptab,
                                     IntTab(DMA_SRCID),
                                     IntTab(DMA_TGTID),
                                     64,
                                     8);    // at most 8 DMA channels

    std::cout << "dma constructed" << std::endl;

    ////////////////////////////
    VciMultiNic<vci_param>* nic;
    nic = new VciMultiNic<vci_param>("nic",
                                     IntTab(NIC_TGTID),
                                     maptab,
                                     4,     // at most 4 NIC channels
                                     "./in_two_channels.txt",
                                     "./out_two_channels.txt",
                                     DEFAULT_MAC_4,
                                     DEFAULT_MAC_2);

    std::cout << "nic constructed" << std::endl;

    ///////////////////////////////
    VciFrameBuffer<vci_param>* fbf;
    fbf = new VciFrameBuffer<vci_param>("fbf",
                                        IntTab(FBF_TGTID),
                                        maptab,
                                        fbf_size, fbf_size);

    std::cout << "fbf constructed" << std::endl;

    ///////////////////////////////
    VciBlockDevice<vci_param>* ioc;
    ioc = new VciBlockDevice<vci_param>("ioc",
                                        maptab,
                                        IntTab(IOC_SRCID),
                                        IntTab(IOC_TGTID),
                                        ioc_filename,
                                        512,
                                        200000);

    std::cout << "fbf constructed" << std::endl;

    ////////////////////////
    VciVgmn<vci_param>* noc;
    noc = new VciVgmn<vci_param>("noc",
                                 maptab,
                                 NB_PROCS+2,
                                 9,
                                 2,
                                 8);

    std::cout << "noc constructed" << std::endl << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Net-List
    //////////////////////////////////////////////////////////////////////////
    for ( size_t p = 0 ; p < NB_PROCS ; p++)
        {
            proc[p]->p_clk      (signal_clk);
            proc[p]->p_resetn   (signal_resetn);
            proc[p]->p_vci      (signal_vci_init_proc[p]);
            proc[p]->p_irq[0]   (signal_irq_proc[p]);
            proc[p]->p_irq[1]   (signal_false);
            proc[p]->p_irq[2]   (signal_false);
            proc[p]->p_irq[3]   (signal_false);
            proc[p]->p_irq[4]   (signal_false);
            proc[p]->p_irq[5]   (signal_false);
        }

    std::cout << "processors connected" << std::endl;

    //////////////////////////////////////////
    ram->p_clk      (signal_clk);
    ram->p_resetn   (signal_resetn);
    ram->p_vci      (signal_vci_tgt_ram);

    std::cout << "ram connected" << std::endl;

    //////////////////////////////////////////
    rom->p_clk      (signal_clk);
    rom->p_resetn   (signal_resetn);
    rom->p_vci      (signal_vci_tgt_rom);

    std::cout << "rom connected" << std::endl;

    //////////////////////////////////////////
    tty->p_clk      (signal_clk);
    tty->p_resetn   (signal_resetn);
    tty->p_vci      (signal_vci_tgt_tty);
    for (size_t t = 0 ; t < NB_PROCS ; t++)
        tty->p_irq[t] (signal_irq_tty[t]);

    std::cout << "tty connected" << std::endl;

    //////////////////////////////////////////
    icu->p_clk      (signal_clk);
    icu->p_resetn   (signal_resetn);
    icu->p_vci      (signal_vci_tgt_icu);

    // irq_out
    for (size_t p = 0 ; p < NB_PROCS ; p++)
        icu->p_irq_out[p]       (signal_irq_proc[p]);

    // irq_in_[0] to irq_in[3] : IOC
    icu->p_irq_in[0] (signal_irq_ioc);
    icu->p_irq_in[1] (signal_false);
    icu->p_irq_in[2] (signal_false);
    icu->p_irq_in[3] (signal_false);

    // irq_in_[4] to irq_in[8] : TTY(s)
    for (size_t x = 0 ; x < 5 ; x++)
    {
        if ( x < NB_PROCS ) icu->p_irq_in[x+4] (signal_irq_tty[x]);
        else                icu->p_irq_in[x+4] (signal_false);
    }

    // irq_in_[9] to irq_in[13] : TIMER(s)
    for (size_t x = 0 ; x < 5 ; x++)
    {
        if ( x < NB_PROCS ) icu->p_irq_in[x+9] (signal_irq_tim[x]);
        else                icu->p_irq_in[x+9] (signal_false);
    }

    // irq_in[14] to irq_in[15] 
    icu->p_irq_in[14] (signal_false);
    icu->p_irq_in[15] (signal_false);


    // irq_in[16] to irq_in[23] : DMA(s)
    for (size_t x = 0 ; x < 8 ; x++)
    {
        icu->p_irq_in[16+x] (signal_irq_dma[x]);
    }

    // irq_in[24] to irq_in[31] 
    for (size_t x = 0 ; x < 8 ; x++)
    {
        icu->p_irq_in[24+x] (signal_false);
    };

    std::cout << "icu connected" << std::endl;

    //////////////////////////////////////////
    timer->p_clk    (signal_clk);
    timer->p_resetn (signal_resetn);
    timer->p_vci    (signal_vci_tgt_tim);
    for (size_t p = 0 ; p < NB_PROCS ; p++)
    {
        timer->p_irq[p] (signal_irq_tim[p]);
    }

    std::cout << "timer connected" << std::endl;

    //////////////////////////////////////////
    dma->p_clk          (signal_clk);
    dma->p_resetn       (signal_resetn);
    dma->p_vci_initiator(signal_vci_init_dma);
    dma->p_vci_target   (signal_vci_tgt_dma);
    for (size_t p = 0 ; p < 8 ; p++)
    {
        dma->p_irq[p] (signal_irq_dma[p]);
    }

    std::cout << "dma connected" << std::endl;

    //////////////////////////////////////////
    nic->p_clk          (signal_clk);
    nic->p_resetn       (signal_resetn);
    nic->p_vci          (signal_vci_tgt_nic);
    for (size_t p = 0 ; p < 4 ; p++)
    {
        nic->p_rx_irq[p]   (signal_irq_nic_rx[p]);
        nic->p_tx_irq[p]   (signal_irq_nic_tx[p]);
    }

    std::cout << "nic connected" << std::endl;

    //////////////////////////////////////////
    fbf->p_clk      (signal_clk);
    fbf->p_resetn   (signal_resetn);
    fbf->p_vci      (signal_vci_tgt_fbf);

    std::cout << "fbf connected" << std::endl;

    //////////////////////////////////////////
    ioc->p_clk          (signal_clk);
    ioc->p_resetn       (signal_resetn);
    ioc->p_vci_initiator(signal_vci_init_ioc);
    ioc->p_vci_target   (signal_vci_tgt_ioc);
    ioc->p_irq          (signal_irq_ioc);

    std::cout << "ioc connected" << std::endl;

    //////////////////////////////////////////
    noc->p_clk      (signal_clk);
    noc->p_resetn   (signal_resetn);
    for ( size_t p = 0 ; p < NB_PROCS ; p++)
    {
        noc->p_to_initiator[p]  (signal_vci_init_proc[p]);
    }
    noc->p_to_initiator[DMA_SRCID]  (signal_vci_init_dma);
    noc->p_to_initiator[IOC_SRCID]  (signal_vci_init_ioc);
    noc->p_to_target[ROM_TGTID]     (signal_vci_tgt_rom);
    noc->p_to_target[RAM_TGTID]     (signal_vci_tgt_ram);
    noc->p_to_target[TIM_TGTID]     (signal_vci_tgt_tim);
    noc->p_to_target[DMA_TGTID]     (signal_vci_tgt_dma);
    noc->p_to_target[NIC_TGTID]     (signal_vci_tgt_nic);
    noc->p_to_target[FBF_TGTID]     (signal_vci_tgt_fbf);
    noc->p_to_target[IOC_TGTID]     (signal_vci_tgt_ioc);
    noc->p_to_target[TTY_TGTID]     (signal_vci_tgt_tty);
    noc->p_to_target[ICU_TGTID]     (signal_vci_tgt_icu);

    std::cout << "noc connected" << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // simulation
    //////////////////////////////////////////////////////////////////////////

    signal_resetn = false;

    sc_start( sc_time( 1, SC_NS ) ) ;

    signal_resetn = true;
    
    for ( size_t n=1 ; n<n_cycles ; n++ )
    {
        sc_start( sc_time( 1 , SC_NS ) ) ;

        if( debug_ok && (n > from_cycle) )
        {
            std::cout << "***************** cycle " << std::dec << n
                      << " ***********************" << std::endl;

            proc[0]->print_trace();
            signal_vci_init_proc[0].print_trace("signal_proc_0");
#if 0
            proc[1]->print_trace();
            signal_vci_init_proc[1].print_trace("signal_proc_1");
            proc[2]->print_trace();
            signal_vci_init_proc[2].print_trace("signal_proc_2");
            proc[3]->print_trace();
            signal_vci_init_proc[3].print_trace("signal_proc_3");
            noc->print_trace(); // function not implemented
            rom->print_trace();
            signal_vci_tgt_rom.print_trace("signal_rom");
            ram->print_trace();
            signal_vci_tgt_ram.print_trace("signal_ram");
#endif
            nic->print_trace(0x3FF1);
            signal_vci_tgt_nic.print_trace("signal_nic");
            dma->print_trace();
            signal_vci_tgt_dma.print_trace("signal_tgt_dma");
            signal_vci_init_dma.print_trace("signal_init_dma");
#if 0
            if ( signal_irq_tim[0].read() ) 
            {
                std::cout << "!!! IRQ_TIMER [0] ACTIVATED !!!" << std::endl; 
                std::cout << "    PROC_IRQ [0] = " << signal_irq_proc[0] << std::endl;
            }
            if ( signal_irq_tim[1].read() ) 
            {
                std::cout << "!!! IRQ_TIMER [1] ACTIVATED !!!" << std::endl; 
                std::cout << "    PROC_IRQ [1] = " << signal_irq_proc[1] << std::endl;
            }
            if ( signal_irq_tim[2].read() ) 
            {
                std::cout << "!!! IRQ_TIMER [2] ACTIVATED !!!" << std::endl; 
                std::cout << "    PROC_IRQ [2] = " << signal_irq_proc[2] << std::endl;
            }
#endif
        }
    }

    kill(0, SIGINT);

    return 0;
} // end main

void quit(int)
{
    sc_core::sc_stop();
}

int sc_main (int argc, char *argv[])
{
    signal(SIGINT, quit);
    signal(SIGPIPE, quit);

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

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// c-file-offsets:((innamespace . 0)(inline-open . 0));
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

