///////////////////////////////////////////////////////////////////////////////////
// File     : top.cpp for the "caba_vgmn_noc_mmu" architecture
// Date     : 01/04/2012
// Author   : alain greiner
// Copyright (c) UPMC-LIP6
///////////////////////////////////////////////////////////////////////////////////
// This architecture implements the NOC MMU infrastructure.
//
// It contains 3 processors MIPS32 (without internal MMU):
// - MP0 is running the hypervisor (GIET-VM).
// - MP1 is running the virtual machine VMA
// - MP2 is running the virtual machine VMB
//
// The hardware architecture contains several paripherals:
// - IOC is a disk controller (not shared, 1 channel: Hypervisor) 
// - FBF is a Frame buffer (shared, 2 channels: VMA, VMB)
// - NIC is a network controller (shared, 2 channels: VMA, VMB)
// - CMA is a chained DMA controller (shared, 4 channels: 2 for VMA, 2 for VMB)
// - TTY is a TTY controller (shared, 3 channels: Hypervisor, VMA, VMB)
// - TIM is a timers controller (shared, 3 channels: Hypervisor, VMA, VMB)
//
// The interconnect is the SoCLib VGMN : flat 32 bits physical address space, and
// there is actualy 4 NOC_MMU components for the 4 initiators MP1, MP2, CMA and IOC
//
// It contains a VCI_ICU supporting 20 IRQ lines and 3 output lines (3 processors):
//   TIMER[0]   : IRQ_IN[00]    (Hypervisor => IRQ_OUT[0])
//   TTY[Ø]     : IRQ_IN[01]    (Hypervisor => IRQ_OUT[0])
//   P1X        : IRQ_IN[02]    (Hypervisor => IRQ_OUT[0])
//   P2X        : IRQ_IN[03]    (Hypervisor => IRQ_OUT[0])
//   CMX[0]     : IRQ_IN[04]    (Hypervisor => IRQ_OUT[0])
//   CMX[1]     : IRQ_IN[05]    (Hypervisor => IRQ_OUT[0])
//   IOX        : IRQ_IN[06]    (Hypervisor => IRQ_OUT[0])
//   IOC        : IRQ_IN[07]    (Hypervisor => IRQ_OUT[0])
//   TIMER[1]   : IRQ_IN[16]    (VM A       => IRQ_OUT[1])
//   TTY[1]     : IRQ_IN[17]    (VM A       => IRQ_OUT[1]) 
//   CMA[0]     : IRQ_IN[18]    (VM A       => IRQ_OUT[1])
//   CMA[1]     : IRQ_IN[19]    (VM A       => IRQ_OUT[1])
//   NIC_RX[0]  : IRQ_IN[20]    (VM A       => IRQ_OUT[1])
//   NIC_TX[0]  : IRQ_IN[21]    (VM A       => IRQ_OUT[1])
//   TIMER[2]   : IRQ_IN[24]    (VM B       => IRQ_OUT[2])
//   TTY[2]     : IRQ_IN[25]    (VM B       => IRQ_OUT[2])
//   CMA[2]     : IRQ_IN[26]    (VM B       => IRQ_OUT[2])
//   CMA[3]     : IRQ_IN[27]    (VM B       => IRQ_OUT[2])
//   NIC_RX[1]  : IRQ_IN[28]    (VM B       => IRQ_OUT[2])
//   NIC_TX[1]  : IRQ_IN[29]    (VM B       => IRQ_OUT[2])
// The routing (masking) from IRQ_IN[i] to IRQ_OUT[0] is not defined
// by the harware, but must be defined by software.
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
#include "vci_xcache_wrapper_multi.h"
#include "vci_multi_tty.h"
#include "vci_timer.h"
#include "vci_multi_icu.h"
#include "vci_chbuf_dma.h"
#include "vci_multi_nic.h"
#include "vci_block_device.h"
#include "vci_framebuffer.h"
#include "vci_simple_ram.h"
#include "vci_noc_mmu.h"
#include "alloc_elems.h"
#include "loader.h"

#define PSEG_ROM_BASE    0xBFC00000
#define PSEG_ROM_SIZE    0x00010000    // boot ROM : 64 Kbytes

#define PSEG_RAM_BASE    0x01000000
#define PSEG_RAM_SIZE    0x00100000    // RAM : 1 Mbytes

#define PSEG_TTY_BASE    0x90000000
#define PSEG_TTY_SIZE    0x00001000 

#define PSEG_TIM_BASE    0x91000000
#define PSEG_TIM_SIZE    0x00001000 

#define PSEG_IOC_BASE    0x92000000
#define PSEG_IOC_SIZE    0x00001000

#define PSEG_CMA_BASE    0x93000000
#define PSEG_CMA_SIZE    0x00004000     // 16 Kbytes : 4 channels

#define PSEG_NIC_BASE    0x97000000
#define PSEG_NIC_SIZE    0x00080000     // 512 Kbytes

#define PSEG_FBF_BASE    0x96000000
#define PSEG_FBF_SIZE    0x00010000     // 64 Kbytes

#define PSEG_ICU_BASE    0x9F000000
#define PSEG_ICU_SIZE    0x00001000 

// segments for the NOC_MMU components

#define PSEG_P1X_BASE    0x9A000000
#define PSEG_P1X_SIZE    0x00001000 

#define PSEG_P2X_BASE    0x9B000000
#define PSEG_P2X_SIZE    0x00001000 

#define PSEG_IOX_BASE    0x9C000000
#define PSEG_IOX_SIZE    0x00001000

#define PSEG_CMX_BASE    0x9D000000
#define PSEG_CMX_SIZE    0x00001000

#define ROM_TGTID      0
#define RAM_TGTID      1
#define TIM_TGTID      2
#define FBF_TGTID      3
#define IOC_TGTID      4
#define CMA_TGTID      5
#define NIC_TGTID      6
#define TTY_TGTID      7
#define ICU_TGTID      8
#define IOX_TGTID      9
#define CMX_TGTID      10
#define P1X_TGTID      11
#define P2X_TGTID      12


#define MP0_SRCID      0
#define MP1_SRCID      1
#define MP2_SRCID      2
#define CMA_SRCID      3
#define IOC_SRCID      4

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
    //  pktid_size  = 4;
    //  trdid_size  = 4;
    //  wrplen_size = 1;

    typedef VciParams<4,8,32,1,1,1,12,4,4,1> vci_param;

    const char soft_name[256] = "/Users/alain/Documents/licence/almo_svn_2011/soft/giet_vm/soft.elf";

    size_t  n_cycles        = 1000000000;       // simulated cycles
    size_t  tlb_ways        = 8;                // NOC_MMU TLBs
    size_t  tlb_sets        = 8;                // NOC_MMU TLBs
    size_t  cache_words     = 16;               // L1 caches
    size_t  cache_ways      = 1;                // L1 caches
    size_t  cache_sets      = 4;                // L1 caches
    size_t  wbuf_nlines     = 4;                // procs Write Buffer
    size_t  wbuf_nwords     = 4;                // procs Write buffer
    size_t  ram_latency     = 0;                // Ram latency
    char    ioc_filename[256];                  // pathname for the ioc file
    size_t  fbf_size        = 128;              // number of lines = number of pixels
    bool    debug_ok        = false;            // debug activated
    size_t  from_cycle      = 0;                // debug start cycle

    strcpy(ioc_filename, "top.cpp");

    std::cout << std::endl << "********************************" << std::endl;
    std::cout << std::endl << "***    caba-vgmn-noc_mmu     ***" << std::endl;
    std::cout << std::endl << "********************************" << std::endl;

    if (argc > 1)
    {
        for( int n=1 ; n<argc ; n=n+2 )
        {
            if( (strcmp(argv[n],"-NCYCLES") == 0) && (n+1<argc) )
            {
                n_cycles = atoi(argv[n+1]);
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
            else if( (strcmp(argv[n],"-FBFSIZE") == 0) && (n+1<argc) )
            {
                fbf_size = atoi(argv[n+1]) ;
            }
            else
            {
                std::cout << "   Arguments on command line are (key,value)" << std::endl;
                std::cout << "   The order is not important." << std::endl;
                std::cout << "   Accepted arguments are :" << std::endl << std::endl;
                std::cout << "   -NCYCLES number_of_simulated_cycles" << std::endl;
                std::cout << "   -IOCFILE file_name" << std::endl;
                std::cout << "   -FBFSIZE number_of_pixels" << std::endl;
                std::cout << "   -DEBUG debug_start_cycle" << std::endl;
                exit(0);
            }
        }
    }
    std::cout << std::endl;
    std::cout << "    n_cycles     = " << n_cycles << std::endl;
    std::cout << "    cache_sets   = " << cache_sets << std::endl;
    std::cout << "    cache_words  = " << cache_words << std::endl;
    std::cout << "    cache_ways   = " << cache_ways << std::endl;
    std::cout << "    ram_latency  = " << ram_latency << std::endl;
    std::cout << "    ioc_file     = " << ioc_filename << std::endl;
    std::cout << "    frame buffer = " << fbf_size << " * " << fbf_size << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Mapping Table
    //////////////////////////////////////////////////////////////////////////
    MappingTable maptab(32, IntTab(8), IntTab(12), 0xFF000000);

    maptab.add(Segment("seg_rom", PSEG_ROM_BASE, PSEG_ROM_SIZE, IntTab(ROM_TGTID), true));
    maptab.add(Segment("seg_ram", PSEG_RAM_BASE, PSEG_RAM_SIZE, IntTab(RAM_TGTID), true));
    maptab.add(Segment("seg_tim", PSEG_TIM_BASE, PSEG_TIM_SIZE, IntTab(TIM_TGTID), false));
    maptab.add(Segment("seg_cma", PSEG_CMA_BASE, PSEG_CMA_SIZE, IntTab(CMA_TGTID), false));
    maptab.add(Segment("seg_nic", PSEG_NIC_BASE, PSEG_NIC_SIZE, IntTab(NIC_TGTID), false));
    maptab.add(Segment("seg_fbf", PSEG_FBF_BASE, PSEG_FBF_SIZE, IntTab(FBF_TGTID), false));
    maptab.add(Segment("seg_ioc", PSEG_IOC_BASE, PSEG_IOC_SIZE, IntTab(IOC_TGTID), false));
    maptab.add(Segment("seg_tty", PSEG_TTY_BASE, PSEG_TTY_SIZE, IntTab(TTY_TGTID), false));
    maptab.add(Segment("seg_icu", PSEG_ICU_BASE, PSEG_ICU_SIZE, IntTab(ICU_TGTID), false));
    maptab.add(Segment("seg_cmx", PSEG_CMX_BASE, PSEG_CMX_SIZE, IntTab(CMX_TGTID), false));
    maptab.add(Segment("seg_iox", PSEG_IOX_BASE, PSEG_IOX_SIZE, IntTab(IOX_TGTID), false));
    maptab.add(Segment("seg_p1x", PSEG_P1X_BASE, PSEG_P1X_SIZE, IntTab(P1X_TGTID), false));
    maptab.add(Segment("seg_p2x", PSEG_P2X_BASE, PSEG_P2X_SIZE, IntTab(P2X_TGTID), false));

    std::cout << std::endl << maptab << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Signals
    //////////////////////////////////////////////////////////////////////////
    sc_clock        signal_clk("signal_clk", sc_time( 1, SC_NS ), 0.5 );
    sc_signal<bool> signal_resetn("signal_resetn");

    VciSignals<vci_param> signal_vci_ini_mp0("signal_vci_ini_mp0");
    VciSignals<vci_param> signal_vci_ini_mp1("signal_vci_ini_mp1");
    VciSignals<vci_param> signal_vci_ini_mp2("signal_vci_ini_mp2");
    VciSignals<vci_param> signal_vci_ini_cma("signal_vci_ini_cma");
    VciSignals<vci_param> signal_vci_ini_ioc("signal_vci_ini_ioc");

    VciSignals<vci_param> signal_vci_ini_p1x("signal_vci_ini_p1x");
    VciSignals<vci_param> signal_vci_ini_p2x("signal_vci_ini_p2x");
    VciSignals<vci_param> signal_vci_ini_cmx("signal_vci_ini_cmx");
    VciSignals<vci_param> signal_vci_ini_iox("signal_vci_ini_iox");

    VciSignals<vci_param> signal_vci_tgt_rom("signal_vci_tgt_rom");
    VciSignals<vci_param> signal_vci_tgt_ram("signal_vci_tgt_ram");
    VciSignals<vci_param> signal_vci_tgt_tim("signal_vci_tgt_tim");
    VciSignals<vci_param> signal_vci_tgt_fbf("signal_vci_tgt_fbf");
    VciSignals<vci_param> signal_vci_tgt_ioc("signal_vci_tgt_ioc");
    VciSignals<vci_param> signal_vci_tgt_cma("signal_vci_tgt_cma");
    VciSignals<vci_param> signal_vci_tgt_nic("signal_vci_tgt_nic");
    VciSignals<vci_param> signal_vci_tgt_icu("signal_vci_tgt_icu");
    VciSignals<vci_param> signal_vci_tgt_tty("signal_vci_tgt_tty");

    VciSignals<vci_param> signal_vci_tgt_iox("signal_vci_tgt_iox");
    VciSignals<vci_param> signal_vci_tgt_cmx("signal_vci_tgt_cmx");
    VciSignals<vci_param> signal_vci_tgt_p1x("signal_vci_tgt_p1x");
    VciSignals<vci_param> signal_vci_tgt_p2x("signal_vci_tgt_p2x");

    sc_signal<bool> signal_false("signal_false");

    sc_signal<bool> signal_irq_mp0("signal_irq_mp0");
    sc_signal<bool> signal_irq_mp1("signal_irq_mp1");
    sc_signal<bool> signal_irq_mp2("signal_irq_mp2");

    sc_signal<bool> signal_irq_timer0("signal_irq_timer0");
    sc_signal<bool> signal_irq_timer1("signal_irq_timer1");
    sc_signal<bool> signal_irq_timer2("signal_irq_timer2");

    sc_signal<bool> signal_irq_tty0("signal_irq_tty0");
    sc_signal<bool> signal_irq_tty1("signal_irq_tty1");
    sc_signal<bool> signal_irq_tty2("signal_irq_tty2");

    sc_signal<bool> signal_irq_cma0("signal_irq_cma0");
    sc_signal<bool> signal_irq_cma1("signal_irq_cma1");
    sc_signal<bool> signal_irq_cma2("signal_irq_cma2");
    sc_signal<bool> signal_irq_cma3("signal_irq_cma3");

    sc_signal<bool> signal_irq_nic_rx0("signal_irq_nic_rx0");
    sc_signal<bool> signal_irq_nic_tx0("signal_irq_nic_tx0");
    sc_signal<bool> signal_irq_nic_rx1("signal_irq_nic_rx1");
    sc_signal<bool> signal_irq_nic_tx1("signal_irq_nic_tx1");

    sc_signal<bool> signal_irq_ioc("signal_irq_ioc");

    sc_signal<bool> signal_irq_p1x("signal_irq_p1x");
    sc_signal<bool> signal_irq_p2x("signal_irq_p2x");
    sc_signal<bool> signal_irq_iox("signal_irq_iox");
    sc_signal<bool> signal_irq_cmx0("signal_irq_cmx0");
    sc_signal<bool> signal_irq_cmx1("signal_irq_cmx1");

    std::cout << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // VCI Components : 5 initiators / 13 targets
    //////////////////////////////////////////////////////////////////////////

    //VLoader loader(map_name);
    Loader loader(soft_name);

    GdbServer<Mips32ElIss>::set_loader(&loader);

    ////////////////////////////////////////////////////////////////////////
    VciXcacheWrapperMulti<vci_param, GdbServer<Mips32ElIss> >* mp0;
    mp0 = new VciXcacheWrapperMulti< vci_param, GdbServer<Mips32ElIss> >
                          ( "mp0",
                            0,
                            maptab,
                            IntTab(MP0_SRCID),
                            cache_ways, cache_sets, cache_words,
                            cache_ways, cache_sets, cache_words,
                            wbuf_nlines, wbuf_nwords );
/*
                            1000,           // max frozen cycles
                            from_cycle,     // debug_start_cycle
                            debug_ok );     //  detailed debug activation
*/
    ////////////////////////////////////////////////////////////////////////
    VciXcacheWrapperMulti<vci_param, GdbServer<Mips32ElIss> >* mp1;
    mp1 = new VciXcacheWrapperMulti< vci_param, GdbServer<Mips32ElIss> >
                          ( "mp1",
                            1,
                            maptab,
                            IntTab(MP1_SRCID),
                            cache_ways, cache_sets, cache_words,
                            cache_ways, cache_sets, cache_words,
                            wbuf_nlines, wbuf_nwords );
/*
                            1000,           // max frozen cycles
                            from_cycle,     // debug_start_cycle
                            debug_ok );     //  detailed debug activation
*/
    ////////////////////////////////////////////////////////////////////////
    VciXcacheWrapperMulti<vci_param, GdbServer<Mips32ElIss> >* mp2;
    mp2 = new VciXcacheWrapperMulti< vci_param, GdbServer<Mips32ElIss> >
                          ( "mp2",
                            2,
                            maptab,
                            IntTab(MP2_SRCID),
                            cache_ways, cache_sets, cache_words,
                            cache_ways, cache_sets, cache_words,
                            wbuf_nlines, wbuf_nwords );
/*
                            1000,           // max frozen cycles
                            from_cycle,     // debug_start_cycle
                            debug_ok );     //  detailed debug activation
*/
    /////////////////////////////
    VciSimpleRam<vci_param>* ram;
    ram = new VciSimpleRam<vci_param>("ram",
                                      IntTab(RAM_TGTID),
                                      maptab,
                                      loader,
                                      ram_latency);
    /////////////////////////////
    VciSimpleRam<vci_param>* rom;
    rom = new VciSimpleRam<vci_param>("rom",
                                      IntTab(ROM_TGTID),
                                      maptab,
                                      loader);
    ////////////////////////////
    VciMultiTty<vci_param> *tty;
    tty = new VciMultiTty<vci_param>("tty",
                                     IntTab(TTY_TGTID),
                                     maptab,
                                     "HYPER", "VMA", "VMB", NULL);
    ////////////////////////////
    VciMultiIcu<vci_param> *icu;
    icu = new VciMultiIcu<vci_param>("icu",
                                     IntTab(ICU_TGTID),
                                     maptab,
                                     32,
                                     3);
    /////////////////////////
    VciTimer<vci_param>* tim;
    tim = new VciTimer<vci_param>("timer",
                                    IntTab(TIM_TGTID),
                                    maptab,
                                    3);
    ////////////////////////////
    VciChbufDma<vci_param>* cma;
    cma = new VciChbufDma<vci_param>("cma",
                                     maptab,
                                     IntTab(CMA_SRCID),
                                     IntTab(CMA_TGTID),
                                     (cache_words << 2),
                                     4);                // 4 channels
    ////////////////////////////
    VciMultiNic<vci_param>* nic;
    nic = new VciMultiNic<vci_param>("nic",
                                     IntTab(NIC_TGTID),
                                     maptab,
                                     2,                 // 2 channels
                                     DEFAULT_MAC_4,
                                     DEFAULT_MAC_2,
                                     1);                // NIC_MODE_SYNTHESIS
    ///////////////////////////////
    VciFrameBuffer<vci_param>* fbf;
    fbf = new VciFrameBuffer<vci_param>("fbf",
                                        IntTab(FBF_TGTID),
                                        maptab,
                                        fbf_size, fbf_size);
    ///////////////////////////////
    VciBlockDevice<vci_param>* ioc;
    ioc = new VciBlockDevice<vci_param>("ioc",
                                        maptab,
                                        IntTab(IOC_SRCID),
                                        IntTab(IOC_TGTID),
                                        ioc_filename,
                                        512,
                                        200000);
    //////////////////////////
    VciVgmn<vci_param>* noc;
    noc = new VciVgmn<vci_param>("noc",
                                 maptab,
                                 5,
                                 13,
                                 4,            // NoC latency
                                 8);           // fifo depth
    //////////////////////////
    VciNocMmu<vci_param>* p1x;
    p1x = new VciNocMmu<vci_param>("p1x",
                                   maptab,
                                   P1X_TGTID,
                                   MP1_SRCID,   // idem mp1
                                   1,           // single channel
                                   cache_words,
                                   tlb_ways, 
                                   tlb_sets,
                                   from_cycle,
                                   debug_ok );
    //////////////////////////
    VciNocMmu<vci_param>* p2x;
    p2x = new VciNocMmu<vci_param>("p2x",
                                   maptab,
                                   P2X_TGTID,
                                   MP2_SRCID,   // idem mp2
                                   1,           // single channel
                                   cache_words,
                                   tlb_ways, 
                                   tlb_sets,
                                   from_cycle,
                                   debug_ok );
    //////////////////////////
    VciNocMmu<vci_param>* cmx;
    cmx = new VciNocMmu<vci_param>("cmx",
                                   maptab,
                                   CMX_TGTID,
                                   CMA_SRCID,   // idem cma
                                   2,           // two channels
                                   cache_words,
                                   tlb_ways, 
                                   tlb_sets,
                                   from_cycle,
                                   debug_ok );
    //////////////////////////
    VciNocMmu<vci_param>* iox;
    iox = new VciNocMmu<vci_param>("iox",
                                   maptab,
                                   IOX_TGTID,
                                   IOC_SRCID,   // idem ioc
                                   1,           // single channel
                                   cache_words,
                                   tlb_ways, 
                                   tlb_sets,
                                   from_cycle,
                                   debug_ok );

    //////////////////////////////////////////////////////////////////////////
    // Net-List
    //////////////////////////////////////////////////////////////////////////

    /////////////////////////////////
    mp0->p_clk          (signal_clk);
    mp0->p_resetn       (signal_resetn);
    mp0->p_vci          (signal_vci_ini_mp0);
    mp0->p_irq[0]       (signal_irq_mp0);
    mp0->p_irq[1]       (signal_false);
    mp0->p_irq[2]       (signal_false);
    mp0->p_irq[3]       (signal_false);
    mp0->p_irq[4]       (signal_false);
    mp0->p_irq[5]       (signal_false);

    std::cout << "mp0 connected" << std::endl;

    /////////////////////////////////
    mp1->p_clk          (signal_clk);
    mp1->p_resetn       (signal_resetn);
    mp1->p_vci          (signal_vci_ini_mp1);
    mp1->p_irq[0]       (signal_irq_mp1);
    mp1->p_irq[1]       (signal_false);
    mp1->p_irq[2]       (signal_false);
    mp1->p_irq[3]       (signal_false);
    mp1->p_irq[4]       (signal_false);
    mp1->p_irq[5]       (signal_false);

    std::cout << "mp1 connected" << std::endl;

    /////////////////////////////////
    p1x->p_clk          (signal_clk);
    p1x->p_resetn       (signal_resetn);
    p1x->p_vci_tgt      (signal_vci_ini_mp1);
    p1x->p_vci_ini      (signal_vci_ini_p1x);
    p1x->p_vci_config   (signal_vci_tgt_p1x);
    p1x->p_irq[0]       (signal_irq_p1x);

    std::cout << "p1x connected" << std::endl;

    /////////////////////////////////
    mp2->p_clk          (signal_clk);
    mp2->p_resetn       (signal_resetn);
    mp2->p_vci          (signal_vci_ini_mp2);
    mp2->p_irq[0]       (signal_irq_mp2);
    mp2->p_irq[1]       (signal_false);
    mp2->p_irq[2]       (signal_false);
    mp2->p_irq[3]       (signal_false);
    mp2->p_irq[4]       (signal_false);
    mp2->p_irq[5]       (signal_false);

    std::cout << "mp2 connected" << std::endl;

    /////////////////////////////////
    p2x->p_clk          (signal_clk);
    p2x->p_resetn       (signal_resetn);
    p2x->p_vci_tgt      (signal_vci_ini_mp2);
    p2x->p_vci_ini      (signal_vci_ini_p2x);
    p2x->p_vci_config   (signal_vci_tgt_p2x);
    p2x->p_irq[0]       (signal_irq_p2x);

    std::cout << "p2x connected" << std::endl;

    ///////////////////////////////// 
    ram->p_clk          (signal_clk);
    ram->p_resetn       (signal_resetn);
    ram->p_vci          (signal_vci_tgt_ram);

    std::cout << "ram connected" << std::endl;

    ///////////////////////////////// 
    rom->p_clk          (signal_clk);
    rom->p_resetn       (signal_resetn);
    rom->p_vci          (signal_vci_tgt_rom);

    std::cout << "rom connected" << std::endl;

    ///////////////////////////////// 
    tty->p_clk          (signal_clk);
    tty->p_resetn       (signal_resetn);
    tty->p_vci          (signal_vci_tgt_tty);
    tty->p_irq[0]       (signal_irq_tty0);
    tty->p_irq[1]       (signal_irq_tty1);
    tty->p_irq[2]       (signal_irq_tty2);

    std::cout << "tty connected" << std::endl;

    //////////////////////////////////////////
    icu->p_clk          (signal_clk);
    icu->p_resetn       (signal_resetn);
    icu->p_vci          (signal_vci_tgt_icu);

    icu->p_irq_out[0]   (signal_irq_mp0);
    icu->p_irq_out[1]   (signal_irq_mp1);
    icu->p_irq_out[2]   (signal_irq_mp2);

    icu->p_irq_in[0]    (signal_irq_timer0);
    icu->p_irq_in[1]    (signal_irq_tty0);
    icu->p_irq_in[2]    (signal_irq_p1x);
    icu->p_irq_in[3]    (signal_irq_p2x);
    icu->p_irq_in[4]    (signal_irq_cmx0);
    icu->p_irq_in[5]    (signal_irq_cmx1);
    icu->p_irq_in[6]    (signal_irq_iox);
    icu->p_irq_in[7]    (signal_irq_ioc);
    icu->p_irq_in[8]    (signal_false);
    icu->p_irq_in[9]    (signal_false);
    icu->p_irq_in[10]   (signal_false);
    icu->p_irq_in[11]   (signal_false);
    icu->p_irq_in[12]   (signal_false);
    icu->p_irq_in[13]   (signal_false);
    icu->p_irq_in[14]   (signal_false);
    icu->p_irq_in[15]   (signal_false);

    icu->p_irq_in[16]   (signal_irq_timer1);
    icu->p_irq_in[17]   (signal_irq_tty1);
    icu->p_irq_in[18]   (signal_irq_cma0);
    icu->p_irq_in[19]   (signal_irq_cma1);
    icu->p_irq_in[20]   (signal_irq_nic_rx0);
    icu->p_irq_in[21]   (signal_irq_nic_tx0);
    icu->p_irq_in[22]   (signal_false);
    icu->p_irq_in[23]   (signal_false);

    icu->p_irq_in[24]   (signal_irq_timer2);
    icu->p_irq_in[25]   (signal_irq_tty2);
    icu->p_irq_in[26]   (signal_irq_cma2);
    icu->p_irq_in[27]   (signal_irq_cma3);
    icu->p_irq_in[28]   (signal_irq_nic_rx1);
    icu->p_irq_in[29]   (signal_irq_nic_tx1);
    icu->p_irq_in[30]   (signal_false);
    icu->p_irq_in[31]   (signal_false);

    std::cout << "icu connected" << std::endl;

    //////////////////////////////////////////
    tim->p_clk          (signal_clk);
    tim->p_resetn       (signal_resetn);
    tim->p_vci          (signal_vci_tgt_tim);
    tim->p_irq[0]       (signal_irq_timer0);
    tim->p_irq[1]       (signal_irq_timer1);
    tim->p_irq[2]       (signal_irq_timer2);

    std::cout << "timer connected" << std::endl;

    //////////////////////////////////////////
    cma->p_clk          (signal_clk);
    cma->p_resetn       (signal_resetn);
    cma->p_vci_initiator(signal_vci_ini_cma);
    cma->p_vci_target   (signal_vci_tgt_cma);
    cma->p_irq[0]       (signal_irq_cma0);
    cma->p_irq[1]       (signal_irq_cma1);
    cma->p_irq[2]       (signal_irq_cma2);
    cma->p_irq[3]       (signal_irq_cma3);

    std::cout << "cma connected" << std::endl;

    /////////////////////////////////
    cmx->p_clk          (signal_clk);
    cmx->p_resetn       (signal_resetn);
    cmx->p_vci_tgt      (signal_vci_ini_cma);
    cmx->p_vci_ini      (signal_vci_ini_cmx);
    cmx->p_vci_config   (signal_vci_tgt_cmx);
    cmx->p_irq[0]       (signal_irq_cmx0);
    cmx->p_irq[1]       (signal_irq_cmx1);

    std::cout << "cmx connected" << std::endl;

    //////////////////////////////////////////
    nic->p_clk          (signal_clk);
    nic->p_resetn       (signal_resetn);
    nic->p_vci          (signal_vci_tgt_nic);
    nic->p_rx_irq[0]    (signal_irq_nic_rx0);
    nic->p_tx_irq[0]    (signal_irq_nic_tx0);
    nic->p_rx_irq[1]    (signal_irq_nic_rx1);
    nic->p_tx_irq[1]    (signal_irq_nic_tx1);

    std::cout << "nic connected" << std::endl;

    //////////////////////////////////////////
    fbf->p_clk          (signal_clk);
    fbf->p_resetn       (signal_resetn);
    fbf->p_vci          (signal_vci_tgt_fbf);

    std::cout << "fbf connected" << std::endl;

    //////////////////////////////////////////
    ioc->p_clk          (signal_clk);
    ioc->p_resetn       (signal_resetn);
    ioc->p_vci_initiator(signal_vci_ini_ioc);
    ioc->p_vci_target   (signal_vci_tgt_ioc);
    ioc->p_irq          (signal_irq_ioc);

    std::cout << "ioc connected" << std::endl;

    /////////////////////////////////
    iox->p_clk          (signal_clk);
    iox->p_resetn       (signal_resetn);
    iox->p_vci_tgt      (signal_vci_ini_ioc);
    iox->p_vci_ini      (signal_vci_ini_iox);
    iox->p_vci_config   (signal_vci_tgt_iox);
    iox->p_irq[0]       (signal_irq_iox);

    std::cout << "iox connected" << std::endl;

    //////////////////////////////////////////
    noc->p_clk      (signal_clk);
    noc->p_resetn   (signal_resetn);

    noc->p_to_initiator[MP0_SRCID]  (signal_vci_ini_mp0);
    noc->p_to_initiator[MP1_SRCID]  (signal_vci_ini_p1x);  // NOC_MMU filter
    noc->p_to_initiator[MP2_SRCID]  (signal_vci_ini_p2x);  // NOC_MMU filter
    noc->p_to_initiator[CMA_SRCID]  (signal_vci_ini_cmx);  // NOC_MMU filter
    noc->p_to_initiator[IOC_SRCID]  (signal_vci_ini_iox);  // NOC_MMU filter

    noc->p_to_target[ROM_TGTID]     (signal_vci_tgt_rom);
    noc->p_to_target[RAM_TGTID]     (signal_vci_tgt_ram);
    noc->p_to_target[TIM_TGTID]     (signal_vci_tgt_tim);
    noc->p_to_target[CMA_TGTID]     (signal_vci_tgt_cma);
    noc->p_to_target[NIC_TGTID]     (signal_vci_tgt_nic);
    noc->p_to_target[FBF_TGTID]     (signal_vci_tgt_fbf);
    noc->p_to_target[IOC_TGTID]     (signal_vci_tgt_ioc);
    noc->p_to_target[TTY_TGTID]     (signal_vci_tgt_tty);
    noc->p_to_target[ICU_TGTID]     (signal_vci_tgt_icu);

    noc->p_to_target[P1X_TGTID]     (signal_vci_tgt_p1x);
    noc->p_to_target[P2X_TGTID]     (signal_vci_tgt_p2x);
    noc->p_to_target[CMX_TGTID]     (signal_vci_tgt_cmx);
    noc->p_to_target[IOX_TGTID]     (signal_vci_tgt_iox);

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

            mp0->print_trace();
            signal_vci_ini_mp0.print_trace("signal_mp0");

            mp1->print_trace();
            signal_vci_ini_mp1.print_trace("signal_mp1");

            mp2->print_trace();
            signal_vci_ini_mp2.print_trace("signal_mp2");

            noc->print_trace();

            rom->print_trace();
            signal_vci_tgt_rom.print_trace("signal_rom");

//            nic->print_trace();
//            signal_vci_tgt_nic.print_trace("signal_nic");

//            cma->print_trace();
//            signal_vci_tgt_cma.print_trace("signal_tgt_cma");
//            signal_vci_ini_cma.print_trace("signal_ini_cma");

//            p1x->print_trace();
//            signal_vci_tgt_p1x.print_trace("signal_tgt_p1x");
//            signal_vci_ini_p1x.print_trace("signal_ini_p1x");

//            p2x->print_trace();
//            signal_vci_tgt_p2x.print_trace("signal_tgt_p2x");
//            signal_vci_ini_p2x.print_trace("signal_ini_p2x");

//            iox->print_trace();
//            signal_vci_tgt_iox.print_trace("signal_tgt_iox");
//            signal_vci_ini_iox.print_trace("signal_ini_iox");

//            cmx->print_trace();
//            signal_vci_tgt_cmx.print_trace("signal_tgt_cmx");
//            signal_vci_ini_cmx.print_trace("signal_ini_cmx");

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

