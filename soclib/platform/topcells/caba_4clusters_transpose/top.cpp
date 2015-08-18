/**********************************************************************
 * File : tp5_cluster_top.cpp
 * Date : 15/12/2010
 * Author :  Alain Greiner
 * UPMC - LIP6
 * This program is released under the GNU public license
 *********************************************************************/

/*
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
 * Copyright (c) UPMC, Lip6, Asim
 *         alain.greiner@lip6.fr
 *
 * Maintainers: alain
 */

#include <systemc>
#include <limits>

#include "vci_signals.h"
#include "vci_param.h"
#include "mapping_table.h"
#include "vci_vgmn.h"
#include "vci_local_crossbar.h"
#include "vci_xcache_wrapper.h"
#include "mips32.h"
#include "vci_multi_tty.h"
#include "vci_timer.h"
#include "vci_icu.h"
#include "vci_dma.h"
#include "vci_block_device.h"
#include "vci_framebuffer.h"
#include "vci_simple_ram.h"
#include "alloc_elems.h"
#include "gdbserver.h"

#define SEG_KCODE_BASE   0x80000000
#define SEG_KCODE_SIZE   0x00004000

#define SEG_KDATA_BASE   0x91000000
#define SEG_KDATA_SIZE   0x00004000

#define SEG_KUNC_BASE    0x92000000
#define SEG_KUNC_SIZE    0x00001000

#define SEG_CODE_BASE    0x20000000
#define SEG_CODE_SIZE    0x00004000

#define SEG_DATA_BASE    0x31000000
#define SEG_DATA_SIZE    0x00010000

#define SEG_STACK0_BASE  0x04000000
#define SEG_STACK1_BASE  0x14000000
#define SEG_STACK2_BASE  0x24000000
#define SEG_STACK3_BASE  0x34000000
#define SEG_STACK_SIZE   0x00010000

#define SEG_ICU0_BASE    0x86000000
#define SEG_ICU1_BASE    0x96000000
#define SEG_ICU2_BASE    0xA6000000
#define SEG_ICU3_BASE    0xB6000000
#define SEG_ICU_SIZE     0x00000014

#define SEG_TTY0_BASE    0x87000000
#define SEG_TTY1_BASE    0x97000000
#define SEG_TTY2_BASE    0xA7000000
#define SEG_TTY3_BASE    0xB7000000
#define SEG_TTY_SIZE     0x00000040

#define SEG_TIM0_BASE    0x88000000
#define SEG_TIM1_BASE    0x98000000
#define SEG_TIM2_BASE    0xA8000000
#define SEG_TIM3_BASE    0xB8000000
#define SEG_TIM_SIZE     0x00000040

#define SEG_IOC_BASE     0x89000000
#define SEG_IOC_SIZE     0x00000020

#define SEG_DMA_BASE     0x9A000000
#define SEG_DMA_SIZE     0x00000014

#define SEG_FBF_BASE     0xAB000000
#define SEG_FBF_SIZE     0x00010000

#define SEG_RESET_BASE   0xBFC00000
#define SEG_RESET_SIZE   0x00001000

// SRCID definition
#define SRCID_PROC      0
#define SRCID_IOC       1
#define SRCID_DMA       1

// TGTID definition
#define TGTID_RAM       0
#define TGTID_TTY       1
#define TGTID_TIM       2
#define TGTID_ICU       3
#define TGTID_IOC       4
#define TGTID_DMA       4
#define TGTID_FBF       4
#define TGTID_ROM       4

// VCI fields width definition
#define cell_size       4
#define plen_size       8
#define addr_size       32
#define rerror_size     1
#define clen_size       1
#define rflag_size      1
#define srcid_size      3
#define trdid_size      1
#define pktid_size      1
#define wrplen_size     1

// Cache parameters definition
#define icache_ways     4
#define icache_sets     128
#define icache_words    8
#define dcache_ways     4
#define dcache_sets     128
#define dcache_words    8

int _main(int argc, char *argv[])
{
    using namespace sc_core;
    using namespace soclib::caba;
    using namespace soclib::common;

    typedef VciParams<cell_size,
                      plen_size,
                      addr_size,
                      rerror_size,
                      clen_size,
                      rflag_size,
                      srcid_size,
                      trdid_size,
                      pktid_size,
                      wrplen_size> vci_param;

    ///////////////////////////////////////////////////////////////
    // command line arguments
    ///////////////////////////////////////////////////////////////
    int     ncycles             = 1000000000;       // simulated cycles
    char    soft_filename[256]  = "to_be_defined";  // pathname for the loader
    char    ioc_filename[256]   = "to_be_defined";  // pathname for the ioc file
    size_t  fbf_size            = 128;              // number of lines = number of pixels
    bool    debug_ok            = false;            // debug activated
    int     from_cycle          = 0;                // debug start cycle

    std::cout << std::endl;
    std::cout << "********************************************************" << std::endl;
    std::cout << "******        4 clusters                          ******" << std::endl;
    std::cout << "********************************************************" << std::endl;
    std::cout << std::endl;

    if (argc > 1)
    {
        for( int n=1 ; n<argc ; n=n+2 )
        {
            if( (strcmp(argv[n],"-NCYCLES") == 0) && (n+1<argc) )
            {
                ncycles = atoi(argv[n+1]);
            }
            else if( (strcmp(argv[n],"-DEBUG") == 0) && (n+1<argc) )
            {
                debug_ok = true;
                from_cycle = atoi(argv[n+1]);
            }
            else if( (strcmp(argv[n],"-SOFT") == 0) && (n+1<argc) )
            {
                strcpy(soft_filename, argv[n+1]) ;
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
                std::cout << "   Arguments on the command line are (key,value) couples." << std::endl;
                std::cout << "   The order is not important." << std::endl;
                std::cout << "   Accepted arguments are :" << std::endl << std::endl;
                std::cout << "   -NCYCLES number_of_simulated_cycles" << std::endl;
                std::cout << "   -IOCFILE file_name" << std::endl;
                std::cout << "   -FBFSIZE number_of_pixels" << std::endl;
                std::cout << "   -SOFT file_name" << std::endl;
                std::cout << "   -DEBUG debug_start_cycle" << std::endl;
                std::cout << "   -TOCYCLE debug_end_cycle" << std::endl;
                exit(0);
            }
        }
    }
    std::cout << std::endl;
    std::cout << "    ncycles       = " << ncycles << std::endl;
    std::cout << "    soft_filename = " << soft_filename << std::endl;
    std::cout << "    ioc_filename  = " << ioc_filename << std::endl;
    std::cout << "    icache_sets   = " << icache_sets << std::endl;
    std::cout << "    icache_words  = " << icache_words << std::endl;
    std::cout << "    icache_ways   = " << icache_ways << std::endl;
    std::cout << "    dcache_sets   = " << dcache_sets << std::endl;
    std::cout << "    dcache_words  = " << dcache_words << std::endl;
    std::cout << "    dcache_ways   = " << dcache_ways << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Mapping Table
    //////////////////////////////////////////////////////////////////////////
    MappingTable maptab(32, IntTab(4,4), IntTab(2,1), 0xFF000000);

    maptab.add(Segment("seg_kcode" , SEG_KCODE_BASE,  SEG_KCODE_SIZE , IntTab(0,TGTID_RAM), true));
    maptab.add(Segment("seg_kdata" , SEG_KDATA_BASE , SEG_KDATA_SIZE , IntTab(1,TGTID_RAM), true));
    maptab.add(Segment("seg_kunc"  , SEG_KUNC_BASE  , SEG_KUNC_SIZE  , IntTab(1,TGTID_RAM), false));
    maptab.add(Segment("seg_code"  , SEG_CODE_BASE  , SEG_CODE_SIZE  , IntTab(2,TGTID_RAM), true));
    maptab.add(Segment("seg_data"  , SEG_DATA_BASE  , SEG_DATA_SIZE  , IntTab(3,TGTID_RAM), false));

    maptab.add(Segment("seg_stack0", SEG_STACK0_BASE, SEG_STACK_SIZE , IntTab(0,TGTID_RAM), true));
    maptab.add(Segment("seg_stack1", SEG_STACK1_BASE, SEG_STACK_SIZE , IntTab(1,TGTID_RAM), true));
    maptab.add(Segment("seg_stack2", SEG_STACK2_BASE, SEG_STACK_SIZE , IntTab(2,TGTID_RAM), true));
    maptab.add(Segment("seg_stack3", SEG_STACK3_BASE, SEG_STACK_SIZE , IntTab(3,TGTID_RAM), true));

    maptab.add(Segment("seg_tty0"  , SEG_TTY0_BASE  , SEG_TTY_SIZE   , IntTab(0,TGTID_TTY), false));
    maptab.add(Segment("seg_tty1"  , SEG_TTY1_BASE  , SEG_TTY_SIZE   , IntTab(1,TGTID_TTY), false));
    maptab.add(Segment("seg_tty2"  , SEG_TTY2_BASE  , SEG_TTY_SIZE   , IntTab(2,TGTID_TTY), false));
    maptab.add(Segment("seg_tty3"  , SEG_TTY3_BASE  , SEG_TTY_SIZE   , IntTab(3,TGTID_TTY), false));

    maptab.add(Segment("seg_tim0"  , SEG_TIM0_BASE  , SEG_TIM_SIZE   , IntTab(0,TGTID_TIM), false));
    maptab.add(Segment("seg_tim1"  , SEG_TIM1_BASE  , SEG_TIM_SIZE   , IntTab(1,TGTID_TIM), false));
    maptab.add(Segment("seg_tim2"  , SEG_TIM2_BASE  , SEG_TIM_SIZE   , IntTab(2,TGTID_TIM), false));
    maptab.add(Segment("seg_tim3"  , SEG_TIM3_BASE  , SEG_TIM_SIZE   , IntTab(3,TGTID_TIM), false));

    maptab.add(Segment("seg_icu0"  , SEG_ICU0_BASE  , SEG_ICU_SIZE   , IntTab(0,TGTID_ICU), false));
    maptab.add(Segment("seg_icu1"  , SEG_ICU1_BASE  , SEG_ICU_SIZE   , IntTab(1,TGTID_ICU), false));
    maptab.add(Segment("seg_icu2"  , SEG_ICU2_BASE  , SEG_ICU_SIZE   , IntTab(2,TGTID_ICU), false));
    maptab.add(Segment("seg_icu3"  , SEG_ICU3_BASE  , SEG_ICU_SIZE   , IntTab(3,TGTID_ICU), false));

    maptab.add(Segment("seg_ioc"   , SEG_IOC_BASE   , SEG_IOC_SIZE   , IntTab(0,TGTID_IOC), false));
    maptab.add(Segment("seg_dma"   , SEG_DMA_BASE   , SEG_DMA_SIZE   , IntTab(1,TGTID_DMA), false));
    maptab.add(Segment("seg_fbf"   , SEG_FBF_BASE   , SEG_FBF_SIZE   , IntTab(2,TGTID_FBF), false));
    maptab.add(Segment("seg_reset" , SEG_RESET_BASE , SEG_RESET_SIZE , IntTab(3,TGTID_ROM), true));

    std::cout << std::endl << maptab << std::endl;

std::cout << "mapping table OK" << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Signals
    //////////////////////////////////////////////////////////////////////////
    sc_clock        signal_clk("signal_clk", sc_time( 1, SC_NS ), 0.5 );
    sc_signal<bool> signal_resetn("signal_resetn");

    VciSignals<vci_param>*  signal_vci_l2g = alloc_elems<VciSignals<vci_param> >("signal_vci_l2g", 4);
    VciSignals<vci_param>*  signal_vci_g2l = alloc_elems<VciSignals<vci_param> >("signal_vci_g2l", 4);

    VciSignals<vci_param>*  signal_vci_proc = alloc_elems<VciSignals<vci_param> >("signal_vci_proc", 4);
    VciSignals<vci_param>*  signal_vci_ram  = alloc_elems<VciSignals<vci_param> >("signal_vci_ram", 4);
    VciSignals<vci_param>*  signal_vci_icu  = alloc_elems<VciSignals<vci_param> >("signal_vci_icu", 4);
    VciSignals<vci_param>*  signal_vci_tty  = alloc_elems<VciSignals<vci_param> >("signal_vci_tty", 4);
    VciSignals<vci_param>*  signal_vci_tim  = alloc_elems<VciSignals<vci_param> >("signal_vci_tim", 4);

    VciSignals<vci_param>   signal_vci_init_dma("signal_vci_init_dma");
    VciSignals<vci_param>   signal_vci_tgt_dma("signal_vci_tgt_dma");

    VciSignals<vci_param>   signal_vci_init_ioc("signal_vci_init_ioc");
    VciSignals<vci_param>   signal_vci_tgt_ioc("signal_vci_tgt_ioc");

    VciSignals<vci_param>   signal_vci_tgt_rom("signal_vci_tgt_rom");

    VciSignals<vci_param>   signal_vci_tgt_fbf("signal_vci_tgt_fbf");

    sc_signal<bool> signal_false("signal_false");

    sc_signal<bool>* signal_irq_proc = alloc_elems<sc_signal<bool> >("signal_irq_proc", 4);
    sc_signal<bool>* signal_irq_tty  = alloc_elems<sc_signal<bool> >("signal_irq_tty", 4);
    sc_signal<bool>* signal_irq_tim  = alloc_elems<sc_signal<bool> >("signal_irq_tim", 4);
    sc_signal<bool> signal_irq_dma("signal_irq_dma");
    sc_signal<bool> signal_irq_ioc("signal_irq_ioc");

std::cout << "signals declaration OK" << std::endl;

    ////////////////////////////////////////////////////////////////////
    // VCI Components : 4 clusters
    // Each cluster contains 1 processor, 1 RAM, 1 TTY, 1 TIMER, 
    // 1 ICU and a LOCAL_CROSSBAR.
    // The cluster 0 contains the IOC.
    // The cluster 1 contains the DMA.
    // The cluster 2 contains the FBF.
    // The cluster 3 contains the ROM.
    // The global interconnect is a VGMN.
    ////////////////////////////////////////////////////////////////////

    Loader loader(soft_filename);

    VciXcacheWrapper<vci_param, GdbServer<Mips32ElIss> >* 		proc[4];
    VciSimpleRam<vci_param>* 				                    ram[4];
    VciMultiTty<vci_param>* 				                    tty[4];
    VciTimer<vci_param>* 				                        timer[4];
    VciIcu<vci_param>* 					                        icu[4];
    VciLocalCrossbar<vci_param>*			                    xbar[4];
    VciSimpleRam<vci_param>* 				                    rom;
    VciDma<vci_param>* 					                        dma;
    VciFrameBuffer<vci_param>* 				                    fbf;
    VciBlockDevice<vci_param>* 				                    ioc;
    VciVgmn<vci_param>* 				                        noc;

    for ( size_t i=0 ; i<4 ; i++ )
    {
        std::ostringstream proc_name;
        proc_name << "proc_" << i;
        proc[i] = new VciXcacheWrapper<vci_param, GdbServer<Mips32ElIss> > (
                      proc_name.str().c_str(),
                      i,
                      maptab,
                      IntTab(i,SRCID_PROC),
                      icache_ways, icache_sets, icache_words,
                      dcache_ways, dcache_sets, dcache_words);

        std::ostringstream ram_name;
        ram_name << "ram_" << i;
        ram[i]   = new VciSimpleRam<vci_param>(
                       ram_name.str().c_str(),
                       IntTab(i, TGTID_RAM), 
                       maptab, 
                       loader);

        std::ostringstream tty_name;
        tty_name << "tty_" << i;
        tty[i]   = new VciMultiTty<vci_param>(
                       tty_name.str().c_str(), 
                       IntTab(i, TGTID_TTY), 
                       maptab, 
                       tty_name.str().c_str(), NULL);

        std::ostringstream timer_name;
        timer_name << "timer_" << i;
        timer[i] = new VciTimer<vci_param>(
                       timer_name.str().c_str(),
                       IntTab(i, TGTID_TIM), 
                       maptab, 
                       1);

        std::ostringstream icu_name;
        icu_name << "icu_" << i;
        icu[i]   = new VciIcu<vci_param>(
                       icu_name.str().c_str(), 
                       IntTab(i, TGTID_ICU), 
                       maptab, 
                       4);
    }

    xbar[0] = new VciLocalCrossbar<vci_param>("xbar_0", maptab, 0, 2, 5, TGTID_RAM);
    xbar[1] = new VciLocalCrossbar<vci_param>("xbar_1", maptab, 1, 2, 5, TGTID_RAM);
    xbar[2] = new VciLocalCrossbar<vci_param>("xbar_2", maptab, 2, 1, 5, TGTID_RAM);
    xbar[3] = new VciLocalCrossbar<vci_param>("xbar_3", maptab, 3, 1, 5, TGTID_RAM);

    ioc = new VciBlockDevice<vci_param>(
              "ioc", 
              maptab, 
              IntTab(0,SRCID_IOC), 
              IntTab(0,TGTID_IOC), 
              ioc_filename, 
              512, 
              20);

    dma = new VciDma<vci_param>(
              "dma", 
              maptab, 
              IntTab(1,SRCID_DMA), 
              IntTab(1,TGTID_DMA), 
              128);

    fbf = new VciFrameBuffer<vci_param>(
              "fbf", 
              IntTab(2,TGTID_FBF), 
              maptab, 
              fbf_size, fbf_size);

    rom = new VciSimpleRam<vci_param>(
              "rom", 
              IntTab(3,TGTID_ROM), 
              maptab, 
              loader);

    noc = new VciVgmn<vci_param>("noc", maptab, 4, 4, 4, 8);

    //////////////////////////////////////////////////////////////////////////
    // Net-List
    //////////////////////////////////////////////////////////////////////////
    noc->p_clk					        (signal_clk);
    noc->p_resetn				        (signal_resetn);
    noc->p_to_initiator[0]			    (signal_vci_l2g[0]);
    noc->p_to_initiator[1]			    (signal_vci_l2g[1]);
    noc->p_to_initiator[2]			    (signal_vci_l2g[2]);
    noc->p_to_initiator[3]			    (signal_vci_l2g[3]);
    noc->p_to_target[0]				    (signal_vci_g2l[0]);
    noc->p_to_target[1]				    (signal_vci_g2l[1]);
    noc->p_to_target[2]				    (signal_vci_g2l[2]);
    noc->p_to_target[3]				    (signal_vci_g2l[3]);

    for ( size_t i=0 ; i<4 ; i++ )
    {
        xbar[i]->p_clk				        (signal_clk);
        xbar[i]->p_resetn			        (signal_resetn);
        xbar[i]->p_initiator_to_up		    (signal_vci_l2g[i]);
        xbar[i]->p_target_to_up			    (signal_vci_g2l[i]);
        xbar[i]->p_to_initiator[SRCID_PROC]	(signal_vci_proc[i]);
        xbar[i]->p_to_target[TGTID_RAM]		(signal_vci_ram[i]);
        xbar[i]->p_to_target[TGTID_TTY]		(signal_vci_tty[i]);
        xbar[i]->p_to_target[TGTID_TIM]		(signal_vci_tim[i]);
        xbar[i]->p_to_target[TGTID_ICU]		(signal_vci_icu[i]);

        proc[i]->p_clk                  	(signal_clk);
        proc[i]->p_resetn               	(signal_resetn);
        proc[i]->p_vci                  	(signal_vci_proc[i]);
        proc[i]->p_irq[0]               	(signal_irq_proc[i]);
        proc[i]->p_irq[1]               	(signal_false);
        proc[i]->p_irq[2]               	(signal_false);
        proc[i]->p_irq[3]               	(signal_false);
        proc[i]->p_irq[4]               	(signal_false);
        proc[i]->p_irq[5]               	(signal_false);

        ram[i]->p_clk                   	(signal_clk);
        ram[i]->p_resetn                	(signal_resetn);
        ram[i]->p_vci                   	(signal_vci_ram[i]);

        tty[i]->p_clk                   	(signal_clk);
        tty[i]->p_resetn                	(signal_resetn);
        tty[i]->p_vci                   	(signal_vci_tty[i]);
        tty[i]->p_irq[0]                	(signal_irq_tty[i]);

        timer[i]->p_clk                 	(signal_clk);
        timer[i]->p_resetn              	(signal_resetn);
        timer[i]->p_vci                 	(signal_vci_tim[i]);
        timer[i]->p_irq[0]              	(signal_irq_tim[i]);

        icu[i]->p_clk                   	(signal_clk);
        icu[i]->p_resetn                	(signal_resetn);
        icu[i]->p_vci                   	(signal_vci_icu[i]);
        icu[i]->p_irq                   	(signal_irq_proc[i]);
        icu[i]->p_irq_in[0]             	(signal_irq_tim[i]);
        icu[i]->p_irq_in[1]             	(signal_irq_tty[i]);
        if ( i==0 ) icu[i]->p_irq_in[2] 	(signal_irq_ioc);
        else	    icu[i]->p_irq_in[2] 	(signal_false);			
        if ( i==1 ) icu[i]->p_irq_in[3] 	(signal_irq_dma);
        else	    icu[i]->p_irq_in[3] 	(signal_false);			
    }
    
    xbar[0]->p_to_initiator[SRCID_IOC]		(signal_vci_init_ioc);
    xbar[0]->p_to_target[TGTID_IOC]		    (signal_vci_tgt_ioc);
    xbar[1]->p_to_initiator[SRCID_DMA]		(signal_vci_init_dma);
    xbar[1]->p_to_target[TGTID_DMA]		    (signal_vci_tgt_dma);
    xbar[2]->p_to_target[TGTID_FBF]		    (signal_vci_tgt_fbf);
    xbar[3]->p_to_target[TGTID_ROM]		    (signal_vci_tgt_rom);

    rom->p_clk                              (signal_clk);
    rom->p_resetn                           (signal_resetn);
    rom->p_vci                              (signal_vci_tgt_rom);

    fbf->p_clk                              (signal_clk);
    fbf->p_resetn                           (signal_resetn);
    fbf->p_vci                              (signal_vci_tgt_fbf);

    ioc->p_clk                              (signal_clk);
    ioc->p_resetn                           (signal_resetn);
    ioc->p_vci_initiator                    (signal_vci_init_ioc);
    ioc->p_vci_target                       (signal_vci_tgt_ioc);
    ioc->p_irq                              (signal_irq_ioc);

    dma->p_clk                              (signal_clk);
    dma->p_resetn                           (signal_resetn);
    dma->p_vci_initiator                    (signal_vci_init_dma);
    dma->p_vci_target                       (signal_vci_tgt_dma);
    dma->p_irq                              (signal_irq_dma);

    std::cout << "net-list completed" << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // simulation
    //////////////////////////////////////////////////////////////////////////

    sc_start( sc_time( 1, SC_NS ) ) ;
    signal_false = false;
    signal_resetn = false;

    signal_resetn = true;
    for ( int n=1 ; n<ncycles ; n++ )
    {
        if( debug_ok && (n > from_cycle) )
        {
            std::cout << "************************* cycle " << std::dec << n 
                      << " ************************" << std::endl;
            proc[0]->print_trace();
            signal_vci_proc[0].print_trace("[SIG] PROC_0 ");
            xbar[0]->print_trace();
            signal_vci_l2g[0].print_trace("[SIG] L2G_0 ");
            signal_vci_l2g[1].print_trace("[SIG] L2G_1 ");
            signal_vci_l2g[2].print_trace("[SIG] L2G_2 ");
            signal_vci_l2g[3].print_trace("[SIG] L2G_3 ");
            noc->print_trace();
            signal_vci_g2l[0].print_trace("[SIG] G2L_0 ");
            signal_vci_g2l[1].print_trace("[SIG] G2L_1 ");
            signal_vci_g2l[2].print_trace("[SIG] G2L_2 ");
            signal_vci_g2l[3].print_trace("[SIG] G2L_3 ");
            xbar[3]->print_trace();
            signal_vci_tgt_rom.print_trace("[SIG] ROM ");
            rom->print_trace();
        }
        sc_start( sc_time( 1 , SC_NS ) ) ;
    }

    sc_stop();

    return(0);

} // end _main

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

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// c-file-offsets:((innamespace . 0)(inline-open . 0));
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

