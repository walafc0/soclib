/**********************************************************************
 * File : top.cpp
 * Date : 01/01/2011
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

#include "gdbserver.h"
#include "vci_vgmn.h"
#include "vci_local_crossbar.h"
#include "vci_xcache_wrapper.h"
#include "mips32.h"
#include "vci_ram.h"
#include "vci_multi_tty.h"
#include "vci_timer.h"
#include "vci_icu.h"
#include "vci_dma.h"
#include "vci_block_device.h"
#include "vci_framebuffer.h"
#include "vci_param.h"
#include "vci_blackhole.h"
#include "mapping_table.h"

#define SEG_KERNEL_BASE  0x80000000
#define SEG_KERNEL_SIZE  0x00004000

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

// local SRCID definition
#define SRCID_PROC      0
#define SRCID_IOC       1
#define SRCID_DMA       1

// local TGTID definition
#define TGTID_RAM       0
#define TGTID_TTY       1
#define TGTID_TIM       2
#define TGTID_ICU       3
#define TGTID_IOC       4
#define TGTID_DMA       4
#define TGTID_FBF       4
#define TGTID_ROM       4

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
	using namespace soclib::tlmdt;
	using namespace soclib::common;

	typedef VciParams<uint32_t,uint32_t> vci_param;

    ///////////////////////////////////////////////////////////////
    // command line arguments
    ///////////////////////////////////////////////////////////////
    int     ncycles             = 1000000000;       // simulated cycles
    char    soft_filename[256]  = "soft/bin.soft";  // pathname for the loader
    char    ioc_filename[256]   = "to_be_defined";  // pathname for the ioc file
    size_t  fbf_size            = 128;              // number of lines = number of pixels
    bool    debug_ok            = false;            // debug activated
    int     from_cycle          = 0;                // debug start cycle

    std::cout << std::endl;
    std::cout << "********************************************************" << std::endl;
    std::cout << "******        transpose tlmdt                     ******" << std::endl;
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
    std::cout << "    ioc_filename  = " << soft_filename << std::endl;
    std::cout << "    fbf_size      = " << fbf_size << std::endl;
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

    maptab.add(Segment("seg_kernel", SEG_KERNEL_BASE, SEG_KERNEL_SIZE, IntTab(0,TGTID_RAM), true));
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

    VciXcacheWrapper<vci_param, GdbServer<Mips32ElIss> >*       proc[4];
    VciRam<vci_param>*                                          ram[4];
    VciMultiTty<vci_param>*                                     tty[4];
    VciTimer<vci_param>*                                        timer[4];
    VciIcu<vci_param>*                                          icu[4];
    VciBlackhole<tlm::tlm_initiator_socket<> >*                 fake[4];
    VciLocalCrossbar*                                           xbar[4];
    VciRam<vci_param>*  	                                rom;
    VciFrameBuffer<vci_param>*                                  fbf;
    VciBlockDevice<vci_param>*                                  ioc;
    VciDma<vci_param>*                                          dma;
    VciVgmn*                                                    noc;

    char* proc_name[4] = { "proc0", "proc1", "proc2", "proc3" };
    char* ram_name[4] = { "ram0", "ram1", "ram2", "ram3" };
    char* tty_name[4] = { "tty0", "tty1", "tty2", "tty3" };
    char* icu_name[4] = { "icuO", "icu1", "icu2", "icu3" };
    char* timer_name[4] = { "timO", "tim1", "tim2", "tim3" };

    for ( size_t i=0 ; i<4 ; i++ )
    {
        proc[i] = new VciXcacheWrapper<vci_param, GdbServer<Mips32ElIss> > (proc_name[i],
                                                              i,maptab,IntTab(i,SRCID_PROC),
                                                              icache_ways, icache_sets, icache_words,
                                                              dcache_ways, dcache_sets, dcache_words);
        ram[i]   = new VciRam<vci_param>(ram_name[i], IntTab(i, TGTID_RAM), maptab, loader);
        tty[i]   = new VciMultiTty<vci_param>(tty_name[i], IntTab(i, TGTID_TTY), maptab, tty_name[i], NULL);
        timer[i] = new VciTimer<vci_param>(timer_name[i], IntTab(i, TGTID_TIM), maptab, 1);
        icu[i]   = new VciIcu<vci_param>(icu_name[i], IntTab(i, TGTID_ICU), maptab, 4);
    }

std::cout << "procs, rams, ttys, timers, and icus constructed" << std::endl;

    xbar[0] = new VciLocalCrossbar("xbar0", maptab, IntTab(0), IntTab(0), 2, 5);
    xbar[1] = new VciLocalCrossbar("xbar1", maptab, IntTab(1), IntTab(1), 2, 5);
    xbar[2] = new VciLocalCrossbar("xbar2", maptab, IntTab(2), IntTab(2), 1, 5);
    xbar[3] = new VciLocalCrossbar("xbar3", maptab, IntTab(3), IntTab(3), 1, 5);

std::cout << "crossbars constructed" << std::endl;

    fake[0]  = new VciBlackhole<tlm::tlm_initiator_socket<> >("fake0", 6);
    fake[1]  = new VciBlackhole<tlm::tlm_initiator_socket<> >("fake1", 6);
    fake[2]  = new VciBlackhole<tlm::tlm_initiator_socket<> >("fake2", 7);
    fake[3]  = new VciBlackhole<tlm::tlm_initiator_socket<> >("fake3", 7);

std::cout << "fake constructed" << std::endl;

    ioc = new VciBlockDevice<vci_param>("ioc", maptab, IntTab(0,SRCID_IOC),
                                         IntTab(0,TGTID_IOC), ioc_filename, 512, 200000);
    dma = new VciDma<vci_param>("dma", maptab, IntTab(1,SRCID_DMA), IntTab(1,TGTID_DMA), 128);
    fbf = new VciFrameBuffer<vci_param>("fbf", IntTab(2,TGTID_FBF), maptab, fbf_size, fbf_size);
    rom = new VciRam<vci_param>("rom", IntTab(3,TGTID_ROM), maptab, loader);

std::cout << "rom, dma, fbf, ioc constructed" << std::endl;

    noc = new VciVgmn("noc", maptab, 4, 4, 4, 8);

std::cout << "noc constructed" << std::endl;


    //////////////////////////////////////////////////////////////////////////
    // Net-List
    //////////////////////////////////////////////////////////////////////////

    // global interconnect
    (*noc->p_to_initiator[0])			(xbar[0]->p_initiator_to_up);
    (*noc->p_to_target[0])			(xbar[0]->p_target_to_up);
    (*noc->p_to_initiator[1])			(xbar[1]->p_initiator_to_up);
    (*noc->p_to_target[1])			(xbar[1]->p_target_to_up);
    (*noc->p_to_initiator[2])			(xbar[2]->p_initiator_to_up);
    (*noc->p_to_target[2])			(xbar[2]->p_target_to_up);
    (*noc->p_to_initiator[3])			(xbar[3]->p_initiator_to_up);
    (*noc->p_to_target[3])			(xbar[3]->p_target_to_up);

std::cout << "noc connected" << std::endl;

    for( size_t i=0 ; i<4 ; i++)
    {
	(*xbar[i]->p_to_initiator[SRCID_PROC])		(proc[i]->p_vci);
	(*xbar[i]->p_to_target[TGTID_RAM])		(ram[i]->p_vci);
	(*xbar[i]->p_to_target[TGTID_ICU])		(icu[i]->p_vci);
	(*xbar[i]->p_to_target[TGTID_TTY])		(tty[i]->p_vci);
	(*xbar[i]->p_to_target[TGTID_TIM])		(timer[i]->p_vci);

std::cout << "crossbar " << i << " connected" << std::endl;

	(*proc[i]->p_irq[0])				(icu[i]->p_irq);
        (*tty[i]->p_irq[0])				(*icu[i]->p_irq_in[1]);
        (*timer[i]->p_irq[0])				(*icu[i]->p_irq_in[0]);

std::cout << "icu " << i << " connected" << std::endl;

	for( size_t n = 0 ; n <5 ; n++)
        {
            (*proc[i]->p_irq[n+1])			(*fake[i]->p_socket[n]);
        }

std::cout << "proc " << i << " unused irqs connected" << std::endl;

        if ( i==0 )
        {
            (*xbar[i]->p_to_target[TGTID_IOC])		(ioc->p_vci_target);
            (*xbar[i]->p_to_initiator[SRCID_IOC])	(ioc->p_vci_initiator);
            (*icu[i]->p_irq_in[2])			(ioc->p_irq);
            (*icu[i]->p_irq_in[3])			(*fake[i]->p_socket[5]);

std::cout << "cluster " << i << " extra components connected" << std::endl;

        }
        else if ( i==1 )
        {
            (*xbar[i]->p_to_target[TGTID_DMA])		(dma->p_vci_target);
            (*xbar[i]->p_to_initiator[SRCID_DMA])	(dma->p_vci_initiator);
            (*icu[i]->p_irq_in[2])			(*fake[i]->p_socket[5]);
            (*icu[i]->p_irq_in[3])			(dma->p_irq);

std::cout << "cluster " << i << " extra components connected" << std::endl;

        }
        else if ( i==2 )
        {
            (*xbar[i]->p_to_target[TGTID_FBF])		(fbf->p_vci);
            (*icu[i]->p_irq_in[2])			(*fake[i]->p_socket[5]);
            (*icu[i]->p_irq_in[3])			(*fake[i]->p_socket[6]);

std::cout << "cluster " << i << " extra components connected" << std::endl;

        }
        else 
        {
            (*xbar[i]->p_to_target[TGTID_ROM])		(rom->p_vci);
            (*icu[i]->p_irq_in[2])			(*fake[i]->p_socket[5]);
            (*icu[i]->p_irq_in[3])			(*fake[i]->p_socket[6]);

std::cout << "cluster " << i << " extra components connected" << std::endl;

        }
    }

    //////////////////////////////////////////////////////////////////////////
    // simulation
    //////////////////////////////////////////////////////////////////////////

    sc_start();

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
