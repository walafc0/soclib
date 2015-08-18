/**********************************************************************
 * File : multi_cluster_top.cpp
 * Date : 15/02/2011
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
#include <iostream>
#include <sstream>

#include "mapping_table.h"
#include "vci_vgmn.h"
#include "vci_local_crossbar.h"
#include "vci_xcache_wrapper.h"
#include "mips32.h"
#include "vci_multi_tty.h"
#include "vci_icu.h"
#include "vci_block_device.h"
#include "vci_dma.h"
#include "vci_timer.h"
#include "vci_framebuffer.h"
#include "vci_ram.h"
#include "alloc_elems.h"
#include "gdbserver.h"
#include "vci_blackhole.h"
#include "vci_param.h"

#define SEG_CODE_BASE    	0x00100000
#define SEG_CODE_SIZE    	0x00004000

#define SEG_DATA_BASE    	0x00200000
#define SEG_DATA_SIZE    	0x00010000

#define SEG_HEAP_BASE   	0x00300000
#define SEG_HEAP_SIZE   	0x00010000

#define SEG_STACK_BASE   	0x00400000
#define SEG_STACK_SIZE   	0x00010000

#define SEG_KCODE_BASE   	0x80000000
#define SEG_KCODE_SIZE   	0x00004000

#define SEG_KDATA_BASE   	0x80100000
#define SEG_KDATA_SIZE   	0x00004000

#define SEG_KUNC_BASE    	0x80200000
#define SEG_KUNC_SIZE    	0x00004000

#define SEG_ICU_BASE     	0x80800000
#define SEG_ICU_SIZE     	0x00000014

#define SEG_TTY_BASE     	0x80900000
#define SEG_TTY_SIZE     	0x00000010

#define SEG_IOC_BASE     	0xBFA00000
#define SEG_IOC_SIZE     	0x00000020

#define SEG_FBF_BASE     	0xBFB00000
#define SEG_FBF_SIZE     	0x00010000

#define SEG_RESET_BASE   	0xBFC00000
#define SEG_RESET_SIZE   	0x00001000

#define SEG_DMA_BASE   		0x80D00000
#define SEG_DMA_SIZE   		0x00000014

#define SEG_TIM_BASE   		0x80E00000
#define SEG_TIM_SIZE   		0x00000010

#define SEGMENT_INCREMENT 	0x01000000

// local TGTID definition
#define TGTID_RAM       0
#define TGTID_TTY       1
#define TGTID_ICU       2
#define TGTID_DMA       3
#define TGTID_TIM       4
#define TGTID_IOC       5
#define TGTID_FBF       6
#define TGTID_ROM       7

// local SRCID definition
#define SRCID_PROC	0
#define SRCID_DMA 	1
#define SRCID_IOC 	2

// IRQ index definition
#define IRQID_TTY	0
#define IRQID_DMA	1
#define IRQID_TIM	2
#define IRQID_IOC	3

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

    size_t  ncycles             = 1000000000;       	// simulated cycles
    size_t  nc                  = 1;       		// number of clusters
    char    soft_filename[256]  = "to be defined";  	// pathname for the loader
    char    ioc_filename[256]   = "to_be_defined";  	// pathname for the ioc file
    size_t  fbf_size            = 128;              	// number of lines = number of pixels
    bool    debug_ok            = false;            	// debug activated
    size_t  from_cycle          = 0;                	// debug start cycle
    size_t  cluster_io		= 0;			// cluster index for the ROM (0xBFC00000)

    ///////////////////////////////////////////////////////////////////////////////////
    // command line arguments
    ///////////////////////////////////////////////////////////////////////////////////

    std::cout << std::endl;
    std::cout << "********************************************************" << std::endl;
    std::cout << "******        multi_cluster_tlmdt                 ******" << std::endl;
    std::cout << "********************************************************" << std::endl;
    std::cout << std::endl;

    if (argc > 1)
    {
        for( int n=1 ; n<argc ; n=n+2 )
        {
            if( (strcmp(argv[n],"-CYCLES") == 0) && (n+1<argc) )
            {
                ncycles = atoi(argv[n+1]);
            }
            else if( (strcmp(argv[n],"-CLUSTERS") == 0) && (n+1<argc) )
            {
                nc = atoi(argv[n+1]);
                if ( nc > 128 )
                {
                    std::cout << "The CLUSTERS parameter cannot be larger than 128" << std::endl;
                    exit(0);
                }
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
            // These 3 parameters are interpreted by the parallel simulator
            else if( ((strcmp(argv[n],"-m") == 0) && (n+1<argc)) ||
                     ((strcmp(argv[n],"-ga") == 0) && (n+1<argc)) || 
                     (strcmp(argv[n],"-l") == 0) ) {}
            else
            {
                std::cout << "   Arguments on the command line are (key,value) couples." << std::endl;
                std::cout << "   The order is not important." << std::endl;
                std::cout << "   Accepted arguments are :" << std::endl << std::endl;
                std::cout << "   -CYCLES number_of_simulated_cycles" << std::endl;
                std::cout << "   -CLUSTERS number_of_clusters" << std::endl;
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
    std::cout << "    cycles        = " << ncycles << std::endl;
    std::cout << "    clusters      = " << nc << std::endl;
    std::cout << "    soft_filename = " << soft_filename << std::endl;
    std::cout << "    ioc_filename  = " << ioc_filename << std::endl;
    std::cout << "    icache_sets   = " << icache_sets << std::endl;
    std::cout << "    icache_words  = " << icache_words << std::endl;
    std::cout << "    icache_ways   = " << icache_ways << std::endl;
    std::cout << "    dcache_sets   = " << dcache_sets << std::endl;
    std::cout << "    dcache_words  = " << dcache_words << std::endl;
    std::cout << "    dcache_ways   = " << dcache_ways << std::endl;

    if      ( nc > 63 )	cluster_io = 63;
    else if ( nc > 31 ) cluster_io = 31;
    else if ( nc > 15 ) cluster_io = 15;
    else if ( nc >  7 ) cluster_io = 7;
    else if ( nc >  3 ) cluster_io = 3;
    else if ( nc >  1 ) cluster_io = 1;
    else                cluster_io = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 		Mapping Table
    // There is 5 replicated segments in each cluster, and 8 single (not replicated) segments.
    // - Peripheral single segments (rom, fbf, ioc) are mapped in the cluster_io.
    // - RAM single segments (code, data, kcode, kdata, kunc) are mapped in cluster 0. 
    // - Peripherals replicated segments (tty, icu, dma) are replicated in each cluster.
    // - RAM replicated segments (stack, heap) are replicated in each cluster.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    MappingTable maptab(32, IntTab(8,4), IntTab(8,4), 0xFFF00000);

    maptab.add(Segment("seg_kcode",SEG_KCODE_BASE, SEG_KCODE_SIZE, IntTab(0, TGTID_RAM),true));
    maptab.add(Segment("seg_kdata",SEG_KDATA_BASE, SEG_KDATA_SIZE, IntTab(0, TGTID_RAM),true));
    maptab.add(Segment("seg_kunc" ,SEG_KUNC_BASE , SEG_KUNC_SIZE , IntTab(0, TGTID_RAM),false));
    maptab.add(Segment("seg_code" ,SEG_CODE_BASE , SEG_CODE_SIZE , IntTab(0 , TGTID_RAM),true));
    maptab.add(Segment("seg_data" ,SEG_DATA_BASE , SEG_DATA_SIZE , IntTab(0 , TGTID_RAM),true));

    maptab.add(Segment("seg_ioc"   , SEG_IOC_BASE   , SEG_IOC_SIZE   , IntTab(cluster_io,TGTID_IOC), false));
    maptab.add(Segment("seg_fbf"   , SEG_FBF_BASE   , SEG_FBF_SIZE   , IntTab(cluster_io,TGTID_FBF), false));
    maptab.add(Segment("seg_reset" , SEG_RESET_BASE , SEG_RESET_SIZE , IntTab(cluster_io,TGTID_ROM), true));

    for ( size_t c=0 ; c < nc ; c++)
    {
        uint32_t heap_base = SEG_HEAP_BASE + c*SEGMENT_INCREMENT;
        uint32_t heap_size = SEG_HEAP_SIZE;
        std::ostringstream seg_heap_name;
        seg_heap_name << "seg_data_" << c;
        maptab.add(Segment(seg_heap_name.str(), heap_base, heap_size, IntTab(c, TGTID_RAM), false));

        uint32_t stack_base = SEG_STACK_BASE + c*SEGMENT_INCREMENT;
        uint32_t stack_size = SEG_STACK_SIZE;
        std::ostringstream seg_stack_name;
        seg_stack_name << "seg_stack_" << c;
        maptab.add(Segment(seg_stack_name.str(), stack_base, stack_size, IntTab(c, TGTID_RAM), true));

        uint32_t tty_base = SEG_TTY_BASE + c*SEGMENT_INCREMENT;
        uint32_t tty_size = SEG_TTY_SIZE;
        std::ostringstream seg_tty_name;
        seg_tty_name << "seg_tty_" << c;
        maptab.add(Segment(seg_tty_name.str(), tty_base, tty_size, IntTab(c, TGTID_TTY), false));

        uint32_t icu_base = SEG_ICU_BASE + c*SEGMENT_INCREMENT;
        uint32_t icu_size = SEG_ICU_SIZE;
        std::ostringstream seg_icu_name;
        seg_icu_name << "seg_icu_" << c;
        maptab.add(Segment(seg_icu_name.str(), icu_base, icu_size, IntTab(c, TGTID_ICU), false));

        uint32_t dma_base = SEG_DMA_BASE + c*SEGMENT_INCREMENT;
        uint32_t dma_size = SEG_DMA_SIZE;
        std::ostringstream seg_dma_name;
        seg_dma_name << "seg_dma_" << c;
        maptab.add(Segment(seg_dma_name.str(), dma_base, dma_size, IntTab(c, TGTID_DMA), false));

        uint32_t tim_base = SEG_TIM_BASE + c*SEGMENT_INCREMENT;
        uint32_t tim_size = SEG_TIM_SIZE;
        std::ostringstream seg_tim_name;
        seg_tim_name << "seg_tim_" << c;
        maptab.add(Segment(seg_tim_name.str(), tim_base, tim_size, IntTab(c, TGTID_TIM), false));
    }

    std::cout << std::endl << maptab << std::endl;

    ////////////////////////////////////////////////////////////////////
    // Each cluster contains 1 PROC, 1 RAM, 1 TTY, 1 ICU, 1 DMA
    // 1 TIMER, and a LOCAL_CROSSBAR.
    // The io_cluster contains 3 extra components : IOC, FBF & ROM.
    // The global interconnect is a VGMN.
    ////////////////////////////////////////////////////////////////////

    Loader loader(soft_filename);

    VciXcacheWrapper<vci_param, GdbServer<Mips32ElIss> >* 	proc[nc];
    VciRam<vci_param>* 				        	ram[nc];
    VciMultiTty<vci_param>* 				        tty[nc];
    VciIcu<vci_param>* 					        icu[nc];
    VciDma<vci_param>* 					        dma[nc];
    VciTimer<vci_param>* 					timer[nc];
    VciLocalCrossbar*			        		xbar[nc];
    VciBlackhole<tlm::tlm_initiator_socket<> >*			fake[nc];

    VciRam<vci_param>* 				        	rom;
    VciFrameBuffer<vci_param>* 				        fbf;
    VciBlockDevice<vci_param>* 				        ioc;
    VciVgmn* 				        		noc;

    for ( size_t c=0 ; c<nc ; c++ )
    {
        size_t nb_init;
        size_t nb_target;
        size_t nb_irq; 

        if ( c == cluster_io )
        {
            nb_init	= 3;
            nb_target	= 8;
            nb_irq	= 4;

            ioc = new VciBlockDevice<vci_param>("ioc", maptab, IntTab(cluster_io, SRCID_IOC), 
                                         IntTab(cluster_io,TGTID_IOC), ioc_filename, 128, 100);

            fbf = new VciFrameBuffer<vci_param>("fbf", IntTab(cluster_io,TGTID_FBF), 
                                                 maptab, fbf_size, fbf_size);

            rom = new VciRam<vci_param>("rom", IntTab(cluster_io,TGTID_ROM), maptab, loader);
        }
        else
        {
            nb_init	= 2;
            nb_target	= 5;
            nb_irq	= 3;
        }

        std::ostringstream fake_name;
        fake_name << "fake_" << c;
        fake[c] = new VciBlackhole<tlm::tlm_initiator_socket<> >(fake_name.str().c_str(), 5); 

        std::ostringstream xbar_name;
        xbar_name << "xbar_" << c;
        xbar[c] = new VciLocalCrossbar(xbar_name.str().c_str(), maptab, 
                                                  IntTab(c), IntTab(c), nb_init, nb_target);

        std::ostringstream ram_name;
        ram_name << "ram_" << c;
        ram[c] = new VciRam<vci_param>(ram_name.str().c_str(), IntTab(c, TGTID_RAM), 
                                             maptab, loader);

        std::ostringstream tty_name;
        tty_name << "tty_" << c;
        tty[c]   = new VciMultiTty<vci_param>(tty_name.str().c_str(), IntTab(c, TGTID_TTY), 
                                              maptab, tty_name.str().c_str(), NULL);

        std::ostringstream icu_name;
        icu_name << "icu_" << c;
        icu[c]   = new VciIcu<vci_param>(icu_name.str().c_str(), IntTab(c, TGTID_ICU), 
                                         maptab, nb_irq);

        std::ostringstream dma_name;
        dma_name << "dma_" << c;
        dma[c]   = new VciDma<vci_param>(dma_name.str().c_str(), maptab, IntTab(c, SRCID_DMA),
                                         IntTab(c, TGTID_DMA), 128); 

        std::ostringstream timer_name;
        timer_name << "timer_" << c;
        timer[c]   = new VciTimer<vci_param>(timer_name.str().c_str(), IntTab(c, TGTID_TIM), 
                                             maptab, 1); 
        std::ostringstream proc_name;
        proc_name << "proc_" << c;
        proc[c] = new VciXcacheWrapper<vci_param, GdbServer<Mips32ElIss> > 
                             (proc_name.str().c_str(), 
                              c, maptab, IntTab(c, SRCID_PROC),
                              icache_ways, icache_sets, icache_words,
                              dcache_ways, dcache_sets, dcache_words);
    }

    noc = new VciVgmn("noc", maptab, nc, nc, 4, 8);

    std::cout << "components definition OK" << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Net-List
    //////////////////////////////////////////////////////////////////////////
    
    
    for ( size_t c=0 ; c<nc ; c++)
    {
        (*noc->p_to_initiator[c])			(xbar[c]->p_initiator_to_up);
        (*noc->p_to_target[c])				(xbar[c]->p_target_to_up);	

        (*xbar[c]->p_to_target[TGTID_RAM])		(ram[c]->p_vci);
        (*xbar[c]->p_to_target[TGTID_TTY])		(tty[c]->p_vci);
        (*xbar[c]->p_to_target[TGTID_ICU])		(icu[c]->p_vci);
        (*xbar[c]->p_to_target[TGTID_DMA])		(dma[c]->p_vci_target);
        (*xbar[c]->p_to_target[TGTID_TIM])		(timer[c]->p_vci);
        (*xbar[c]->p_to_initiator[SRCID_DMA])		(dma[c]->p_vci_initiator);
        (*xbar[c]->p_to_initiator[SRCID_PROC])		(proc[c]->p_vci);

        (*proc[c]->p_irq[0])				(icu[c]->p_irq);
        (*tty[c]->p_irq[0])				(*icu[c]->p_irq_in[IRQID_TTY]);
        (dma[c]->p_irq)					(*icu[c]->p_irq_in[IRQID_DMA]);
        (*timer[c]->p_irq[0])				(*icu[c]->p_irq_in[IRQID_TIM]);
        
        for ( size_t n = 0 ; n < 5 ; n++ )
        {
            (*proc[c]->p_irq[n+1])			(*fake[c]->p_socket[n]);
        }
        
        if ( c == cluster_io )
        {
            (*xbar[c]->p_to_target[TGTID_ROM])		(rom->p_vci);
            (*xbar[c]->p_to_target[TGTID_FBF])		(fbf->p_vci);
            (*xbar[c]->p_to_target[TGTID_IOC])		(ioc->p_vci_target);
            (*xbar[c]->p_to_initiator[SRCID_IOC])	(ioc->p_vci_initiator);
            (ioc->p_irq)				(*icu[c]->p_irq_in[IRQID_IOC]);
        }
    } // en for clusters

    std::cout << "components connected OK" << std::endl << std::endl;

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

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// c-file-offsets:((innamespace . 0)(inline-open . 0));
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

