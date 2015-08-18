/*
 * Implementation note:
 * This generic architecture supports both multi-tasking, and multi-processing.
 * - The number of processors cannot be larger than 8.
 * - The number of tasks per processor cannot be larger than 4.
 * - An IO controler and a Frame buffer can be optionally activated.
 */

#include <systemc>
#include <limits>
#include <cstdlib>

#include "vci_signals.h"
#include "vci_param.h"
#include "mapping_table.h"
#include "gdbserver.h"

#include "mips32.h"
#include "vci_vgsb.h"
#include "vci_xcache_wrapper.h"
#include "vci_multi_tty.h"
#include "vci_timer.h"
#include "vci_multi_icu.h"
#include "vci_multi_dma.h"
#include "vci_block_device.h"
#include "vci_framebuffer.h"
#include "vci_simple_ram.h"
#include "alloc_elems.h"

#define SEG_RESET_BASE  0xBFC00000
#define SEG_RESET_SIZE  0x00001000

#define SEG_KERNEL_BASE 0x80000000
#define SEG_KERNEL_SIZE 0x00004000

#define SEG_KDATA_BASE  0x81000000
#define SEG_KDATA_SIZE  0x00004000

#define SEG_KUNC_BASE   0x82000000
#define SEG_KUNC_SIZE   0x00001000

#define SEG_DATA_BASE   0x10000000
#define SEG_DATA_SIZE   0x00080000

#define SEG_CODE_BASE   0x00400000
#define SEG_CODE_SIZE   0x00004000

#define SEG_STACK_BASE  0x20000000
#define SEG_STACK_SIZE  0x00800000

#define SEG_TTY_BASE    0x90000000
#define SEG_TTY_SIZE    0x00000200 /* size = max_nbprocs * max_nbtasks * 16 */

#define SEG_TIM_BASE    0x91000000
#define SEG_TIM_SIZE    0x00000080 /* size = max_nbprocs * 16 */

#define SEG_IOC_BASE    0x92000000
#define SEG_IOC_SIZE    0x00000020

#define SEG_DMA_BASE    0x93000000
#define SEG_DMA_SIZE    0x00000100 /* size = max_nbprocs) * 32 */

#define SEG_FBF_BASE    0x96000000
#define SEG_FBF_SIZE    0x00004000

#define SEG_ICU_BASE    0x9F000000
#define SEG_ICU_SIZE    0x00000100 /* size = max_nbprocs) * 32 */

#define ROM_TGTID   0
#define RAM_TGTID   1
#define TIM_TGTID   2
#define FBF_TGTID   3
#define IOC_TGTID   4
#define DMA_TGTID   5
#define TTY_TGTID   6
#define ICU_TGTID   7

#define DMA_SRCID   n_procs
#define IOC_SRCID   n_procs+1

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

    size_t  n_cycles        = 1000000000;       // simulated cycles
    bool    icached         = true;             // Icache activated
    size_t  icache_sets     = 256;              // Icache parameters
    size_t  icache_words    = 4;
    size_t  icache_ways     = 1;
    bool    dcached         = true;             // Dcache activated
    size_t  dcache_sets     = 256;              // Dcache parameters
    size_t  dcache_words    = 4;
    size_t  dcache_ways     = 1;
    size_t  ram_latency     = 0;                // Ram latency (L2 MISS emulation)
    size_t  n_procs         = 1;                // Number of processors
    size_t  n_tasks         = 1;                // Number of terminals/task per processor
    char    sys_name[256]   = "soft/sys.bin";   // pathname to the system binary
    char    app_name[256]   = "soft/app.bin";   // pathname to the application binary
    bool    ioc_ok          = false;            // IOC activated
    char    ioc_filename[256];                  // pathname for the ioc file
    bool    fbf_ok          = false;            // FBF acctivated
    size_t  fbf_size        = 128;              // number of lines = number of pixels
    bool    debug_ok        = false;            // debug activated
    size_t  from_cycle      = 0;                // debug start cycle
    size_t  to_cycle        = 1000000000;       // debug end cycle
    bool    trace_ok        = false;            // cache trace activated
    char    trace_filename[256];
    FILE*   trace_file      = NULL;
    bool    stats_ok        = false;            // statistics activated
    char    stats_filename[256];
    FILE*   stats_file      = NULL;

    std::cout << std::endl << "********************************" << std::endl;
    std::cout << std::endl << "****** simul_almo_generic ******" << std::endl;
    std::cout << std::endl << "********************************" << std::endl;


    if (argc > 1)
    {
        for( int n=1 ; n<argc ; n=n+2 )
        {
            if( (strcmp(argv[n],"-NCYCLES") == 0) && (n+1<argc) )
            {
                n_cycles = atoi(argv[n+1]);
            }
            else if( (strcmp(argv[n],"-NPROCS") == 0) && (n+1<argc) )
            {
                n_procs = atoi(argv[n+1]);
                if( (n_procs > 8) || (n_procs < 1) )
                {
                    std::cout << "The NPROCS argument cannot be greater than 8" << std::endl;
                    exit(0);
                }
            }
            else if( (strcmp(argv[n],"-NTASKS") == 0) && (n+1<argc) )
            {
                n_tasks = atoi(argv[n+1]);
                if( (n_tasks > 4) || (n_tasks < 1) )
                {
                    std::cout << "The NTASKS argument cannot be greater than 4" << std::endl;
                    exit(0);
                }
            }
            else if( (strcmp(argv[n],"-NICACHE") == 0) && (n+1<argc) )
            {
                icache_sets = atoi(argv[n+1]);
                if(icache_sets == 0)
                {
                    icached = false;
                    icache_sets = 1;
                }
            }
            else if( (strcmp(argv[n],"-NDCACHE") == 0) && (n+1<argc) )
            {
                dcache_sets = atoi(argv[n+1]);
                if(dcache_sets == 0)
                {
                    dcached = false;
                    dcache_sets = 1;
                }
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
            else if( (strcmp(argv[n],"-TOCYCLE") == 0) && (n+1<argc) )
            {
                to_cycle = atoi(argv[n+1]);
            }
            else if( (strcmp(argv[n],"-SYS") == 0) && (n+1<argc) )
            {
                strcpy(sys_name, argv[n+1]) ;
            }
            else if( (strcmp(argv[n],"-APP") == 0) && (n+1<argc) )
            {
                strcpy(app_name, argv[n+1]) ;
            }
            else if( (strcmp(argv[n],"-IOCFILE") == 0) && (n+1<argc) )
            {
                ioc_ok = true;
                strcpy(ioc_filename, argv[n+1]) ;
            }
            else if( (strcmp(argv[n],"-FBFSIZE") == 0) && (n+1<argc) )
            {
                fbf_ok = true;
                fbf_size = atoi(argv[n+1]) ;
            }
            else if( (strcmp(argv[n],"-LATENCY") == 0) && (n+1<argc) )
            {
                ram_latency = atoi(argv[n+1]);
            }
            else
            {
                std::cout << "   Arguments on the command line are (key,value) couples." << std::endl;
                std::cout << "   The order is not important." << std::endl;
                std::cout << "   Accepted arguments are :" << std::endl << std::endl;
                std::cout << "   -SYS file_name (not optional)" << std::endl;
                std::cout << "   -APP file_name (not optional)" << std::endl << std::endl;
                std::cout << "   -NCYCLES number_of_simulated_cycles" << std::endl;
                std::cout << "   -NRPROCS number_of_processors" << std::endl;
                std::cout << "   -NTASKS max number_of_tasks_per_processor" << std::endl;
                std::cout << "   -NICACHE number_of_sets_for_instruction_cache" << std::endl;
                std::cout << "   -NDCACHE number_of_sets_for_data_cache" << std::endl;
                std::cout << "   -IOCFILE file_name" << std::endl;
                std::cout << "   -FBFSIZE number_of_pixels" << std::endl;
                std::cout << "   -TRACE file_name" << std::endl;
                std::cout << "   -STATS file_name" << std::endl;
                std::cout << "   -DEBUG debug_start_cycle" << std::endl;
                std::cout << "   -TOCYCLE debug_end_cycle" << std::endl;
                std::cout << "   -LATENCY number_of_cycles" << std::endl;
                exit(0);
            }
        }
    }
    std::cout << std::endl;
    std::cout << "    n_cycles     = " << n_cycles << std::endl;
    std::cout << "    n_procs      = " << n_procs << std::endl;
    std::cout << "    n_tasks      = " << n_tasks << std::endl;
    std::cout << "    sys_name     = " << sys_name << std::endl;
    std::cout << "    app_name     = " << app_name << std::endl;
    std::cout << "    icache_sets  = " << icache_sets << std::endl;
    std::cout << "    icache_words = " << icache_words << std::endl;
    std::cout << "    icache_ways  = " << icache_ways << std::endl;
    std::cout << "    dcache_sets  = " << dcache_sets << std::endl;
    std::cout << "    dcache_words = " << dcache_words << std::endl;
    std::cout << "    dcache_ways  = " << dcache_ways << std::endl;
    std::cout << "    ram_latency  = " << ram_latency << std::endl;
    if(trace_ok) std::cout << "    trace_file   = " << trace_filename << std::endl;
    if(stats_ok) std::cout << "    stats_file   = " << stats_filename << std::endl;
    if(ioc_ok)   std::cout << "    ioc_file     = " << ioc_filename << std::endl;
    if(fbf_ok)   std::cout << "    frame buffer = " << fbf_size << " * " << fbf_size << std::endl;

    /* parameters checking */
    if (!strcmp(sys_name, "") || !strcmp(app_name, ""))
    {
        std::cout << std::endl;
        std::cout << "-SYS and -APP parameters are not optional, you must specify them." << sys_name << std::endl;
        exit(0);
    }
    size_t nb_irq_in = (1 + n_procs * (2 + n_tasks));
    if (nb_irq_in > 32)
    {
        std::cout << "Error: the platform is not able to support these parameters"
            << "(e.g. up to 8 processors with 4 tasks on each)"
            << "\n\tThis makes too much IRQs inputs!"
            << std::endl;
        exit(0);
    }

    //////////////////////////////////////////////////////////////////////////
    // Mapping Table
    //////////////////////////////////////////////////////////////////////////
    MappingTable maptab(32, IntTab(12), IntTab(12), 0xFFF00000);

    maptab.add(Segment("seg_reset"  , SEG_RESET_BASE  , SEG_RESET_SIZE  , IntTab(ROM_TGTID) , icached));

    maptab.add(Segment("seg_kernel" , SEG_KERNEL_BASE , SEG_KERNEL_SIZE , IntTab(RAM_TGTID) , icached));
    maptab.add(Segment("seg_kdata"  , SEG_KDATA_BASE  , SEG_KDATA_SIZE  , IntTab(RAM_TGTID) , dcached));
    maptab.add(Segment("seg_kunc"   , SEG_KUNC_BASE   , SEG_KUNC_SIZE   , IntTab(RAM_TGTID) , false));
    maptab.add(Segment("seg_code"   , SEG_CODE_BASE   , SEG_CODE_SIZE   , IntTab(RAM_TGTID) , icached));
    maptab.add(Segment("seg_data"   , SEG_DATA_BASE   , SEG_DATA_SIZE   , IntTab(RAM_TGTID) , dcached));
    maptab.add(Segment("seg_stack"  , SEG_STACK_BASE  , SEG_STACK_SIZE  , IntTab(RAM_TGTID) , dcached));

    maptab.add(Segment("seg_timer"  , SEG_TIM_BASE    , SEG_TIM_SIZE    , IntTab(TIM_TGTID) , false));
    maptab.add(Segment("seg_dma"    , SEG_DMA_BASE    , SEG_DMA_SIZE    , IntTab(DMA_TGTID) , false));
    maptab.add(Segment("seg_fb"     , SEG_FBF_BASE    , SEG_FBF_SIZE    , IntTab(FBF_TGTID) , false));
    maptab.add(Segment("seg_io"     , SEG_IOC_BASE    , SEG_IOC_SIZE    , IntTab(IOC_TGTID) , false));
    maptab.add(Segment("seg_tty"    , SEG_TTY_BASE    , SEG_TTY_SIZE    , IntTab(TTY_TGTID) , false));
    maptab.add(Segment("seg_icu"    , SEG_ICU_BASE    , SEG_ICU_SIZE    , IntTab(ICU_TGTID) , false));

    std::cout << std::endl << maptab << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Signals
    //////////////////////////////////////////////////////////////////////////
    sc_clock        signal_clk("signal_clk", sc_time( 1, SC_NS ), 0.5 );
    sc_signal<bool> signal_resetn("signal_resetn");

    VciSignals<vci_param> *signal_vci_init_proc =
        alloc_elems<VciSignals<vci_param> >("signal_vci_init_proc", n_procs);

    VciSignals<vci_param> signal_vci_init_dma("signal_vci_init_dma");
    VciSignals<vci_param> signal_vci_init_ioc("signal_vci_init_ioc");

    VciSignals<vci_param> signal_vci_tgt_rom("signal_vci_tgt_rom");
    VciSignals<vci_param> signal_vci_tgt_ram("signal_vci_tgt_ram");
    VciSignals<vci_param> signal_vci_tgt_tim("signal_vci_tgt_tim");
    VciSignals<vci_param> signal_vci_tgt_fbf("signal_vci_tgt_fbf");
    VciSignals<vci_param> signal_vci_tgt_ioc("signal_vci_tgt_ioc");
    VciSignals<vci_param> signal_vci_tgt_dma("signal_vci_tgt_dma");
    VciSignals<vci_param> signal_vci_tgt_icu("signal_vci_tgt_icu");
    VciSignals<vci_param> signal_vci_tgt_tty("signal_vci_tgt_tty");

    sc_signal<bool> signal_false("signal_false");

    sc_signal<bool> *signal_irq_proc =
        alloc_elems<sc_signal<bool> >("signal_irq_proc", n_procs);

    sc_signal<bool> *signal_irq_tim =
        alloc_elems<sc_signal<bool> >("signal_irq_tim", n_procs);

    sc_signal<bool> *signal_irq_dma =
        alloc_elems<sc_signal<bool> >("signal_irq_dma", n_procs);

    sc_signal<bool> **signal_irq_tty =
        alloc_elems<sc_signal<bool> >("signal_irq_tty", n_procs, n_tasks);

    sc_signal<bool> signal_irq_ioc("signal_irq_ioc");

    ////////////////////////////////////////////////////////////////////
    // VCI Components : (n_procs+2) initiators / (8) targets
    // The IOC & DMA components are both initiator & target.
    // The IOC and FBF components are optionnal.
    ////////////////////////////////////////////////////////////////////
    // - srcid proc : pid
    // - srcid dma  : n_procs   : dma
    // - srcid ioc  : n_procs+1 : ioc
    /////////////////////////////////////////////////////////////////
    // - tgtid rom
    // - tgtid ram
    // - tgtid timer
    // - tgtid dma
    // - tgtid fbf
    // - tgtid ioc
    // - tgtid tty
    // - tgtid icu
    /////////////////////////////////////////////////////////////////
    // The ICU controls at most 32 input IRQs:
    // - IRQ[0] : ioc   (only for processor 0)
    //
    // then, for cpu_i, the base INT number is (1 + i * irq_span)
    // knowing that irq_span = 2 + n_tasks
    //
    // - IRQ[base]   : timer
    // - IRQ[base+1] : dma
    // - IRQ[base+2] : TTY0 (depending on n_tasks)
    // - IRQ[base+3] : TTY1 (depending on n_tasks)
    // - IRQ[base+4] : TTY2 (depending on n_tasks)
    // - IRQ[base+5] : TTY3 (depending on n_tasks)
    //
    // Eventually, the number of input IRQs is (1 + n_procs * (2 + n_tasks)).
    /////////////////////////////////////////////////////////////////

    Loader loader(sys_name, app_name);
    GdbServer<Mips32ElIss>::set_loader(&loader);

    VciXcacheWrapper<vci_param, GdbServer<Mips32ElIss> >* proc[n_procs];
    for( size_t p = 0 ; p < n_procs ; p++ )
    {
        std::ostringstream proc_name;
        proc_name << "proc_" << p;
        proc[p] = new VciXcacheWrapper< vci_param, GdbServer<Mips32ElIss> >(
                proc_name.str().c_str(),
                p,
                maptab,
                IntTab(p),
                icache_ways, icache_sets, icache_words,
                dcache_ways, dcache_sets, dcache_words);
    }

    VciSimpleRam<vci_param>* rom;
    rom = new VciSimpleRam<vci_param>("rom",
            IntTab(ROM_TGTID),
            maptab,
            loader);

    VciSimpleRam<vci_param>* ram;
    ram = new VciSimpleRam<vci_param>("ram",
            IntTab(RAM_TGTID),
            maptab,
            loader,
            ram_latency);

    std::vector<std::string> vect_names;
    for( size_t p = 0 ; p < n_procs ; p++ )
    {
        for (size_t t = 0; t < n_tasks; t++)
        {
            std::ostringstream term_name;
            term_name << "proc" << p << "_term" << t;
            vect_names.push_back(term_name.str().c_str());
        }
    }
    VciMultiTty<vci_param> *tty;
    tty = new VciMultiTty<vci_param>("tty",
            IntTab(TTY_TGTID),
            maptab,
            vect_names);

    VciMultiIcu<vci_param> *icu;
    icu = new VciMultiIcu<vci_param>("icu",
            IntTab(ICU_TGTID),
            maptab,
            nb_irq_in,
            n_procs);


    VciTimer<vci_param>* timer;
    timer = new VciTimer<vci_param>("timer",
            IntTab(TIM_TGTID),
            maptab,
            n_procs);

    VciMultiDma<vci_param>* dma;
    dma = new VciMultiDma<vci_param>("dma",
            maptab,
            IntTab(DMA_SRCID),
            IntTab(DMA_TGTID),
            64,
            n_procs);

    VciFrameBuffer<vci_param>* fbf;
    if( fbf_ok ) fbf = new VciFrameBuffer<vci_param>("fbf",
            IntTab(FBF_TGTID),
            maptab,
            fbf_size, fbf_size);
    else fbf = NULL;

    VciBlockDevice<vci_param>* ioc;
    if( ioc_ok ) ioc = new VciBlockDevice<vci_param>("ioc",
            maptab,
            IntTab(IOC_SRCID),
            IntTab(IOC_TGTID),
            ioc_filename,
            512,
            200000);
    else ioc = NULL;

    VciVgsb<vci_param>* bus;
    bus = new VciVgsb<vci_param>("bus",
            maptab,
            n_procs+2,
            8);

    std::cout << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // Net-List
    //////////////////////////////////////////////////////////////////////////
    for ( size_t p = 0 ; p < n_procs ; p++)
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

    std::cout << " - procs connected" << std::endl;

    ram->p_clk      (signal_clk);
    ram->p_resetn   (signal_resetn);
    ram->p_vci      (signal_vci_tgt_ram);

    std::cout << " - ram connected" << std::endl;

    rom->p_clk      (signal_clk);
    rom->p_resetn   (signal_resetn);
    rom->p_vci      (signal_vci_tgt_rom);

    std::cout << " - rom connected" << std::endl;

    tty->p_clk      (signal_clk);
    tty->p_resetn   (signal_resetn);
    tty->p_vci      (signal_vci_tgt_tty);
    for (size_t p = 0 ; p < n_procs ; p++)
        for (size_t t = 0 ; t < n_tasks ; t++)
            tty->p_irq[p * n_tasks + t] (signal_irq_tty[p][t]);

    std::cout << " - tty connected" << std::endl;

    icu->p_clk      (signal_clk);
    icu->p_resetn   (signal_resetn);
    icu->p_vci      (signal_vci_tgt_icu);
    for (size_t p = 0 ; p < n_procs ; p++)
    {
        icu->p_irq_out[p] (signal_irq_proc[p]);

        size_t base = 1 + p * (2 + n_tasks);

        icu->p_irq_in[base]     (signal_irq_tim[p]);
        icu->p_irq_in[base + 1] (signal_irq_dma[p]);

        for (size_t t = 0; t < n_tasks; t++)
            icu->p_irq_in[base + 2 + t] (signal_irq_tty[p][t]);
    }
    if( ioc_ok )
        icu->p_irq_in[0] (signal_irq_ioc);
    else
        icu->p_irq_in[0] (signal_false);

    std::cout << " - icu connected" << std::endl;

    timer->p_clk    (signal_clk);
    timer->p_resetn (signal_resetn);
    timer->p_vci    (signal_vci_tgt_tim);
    for (size_t p = 0 ; p < n_procs ; p++)
        timer->p_irq[p] (signal_irq_tim[p]);

    std::cout << " - timer connected" << std::endl;

    dma->p_clk          (signal_clk);
    dma->p_resetn       (signal_resetn);
    dma->p_vci_initiator(signal_vci_init_dma);
    dma->p_vci_target   (signal_vci_tgt_dma);
    for (size_t p = 0 ; p < n_procs ; p++)
        dma->p_irq[p] (signal_irq_dma[p]);

    std::cout << " - dma connected" << std::endl;

    if( fbf_ok )
    {
        fbf->p_clk      (signal_clk);
        fbf->p_resetn   (signal_resetn);
        fbf->p_vci      (signal_vci_tgt_fbf);
    }

    std::cout << " - fbf connected" << std::endl;

    if( ioc_ok )
    {
        ioc->p_clk          (signal_clk);
        ioc->p_resetn       (signal_resetn);
        ioc->p_vci_initiator(signal_vci_init_ioc);
        ioc->p_vci_target   (signal_vci_tgt_ioc);
        ioc->p_irq          (signal_irq_ioc);
    }

    std::cout << " - ioc connected" << std::endl;

    bus->p_clk      (signal_clk);
    bus->p_resetn   (signal_resetn);
    for ( size_t p = 0 ; p < n_procs ; p++)
    {
        bus->p_to_initiator[p]  (signal_vci_init_proc[p]);
    }
    bus->p_to_initiator[DMA_SRCID]  (signal_vci_init_dma);
    bus->p_to_initiator[IOC_SRCID]  (signal_vci_init_ioc);
    bus->p_to_target[ROM_TGTID]     (signal_vci_tgt_rom);
    bus->p_to_target[RAM_TGTID]     (signal_vci_tgt_ram);
    bus->p_to_target[TIM_TGTID]     (signal_vci_tgt_tim);
    bus->p_to_target[DMA_TGTID]     (signal_vci_tgt_dma);
    bus->p_to_target[FBF_TGTID]     (signal_vci_tgt_fbf);
    bus->p_to_target[IOC_TGTID]     (signal_vci_tgt_ioc);
    bus->p_to_target[TTY_TGTID]     (signal_vci_tgt_tty);
    bus->p_to_target[ICU_TGTID]     (signal_vci_tgt_icu);

    std::cout << " - bus connected" << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // simulation
    //////////////////////////////////////////////////////////////////////////

    sc_start( sc_time( 1, SC_NS ) ) ;

    if( !ioc_ok ) signal_vci_init_ioc.cmdval = false;
    if( !ioc_ok ) signal_vci_tgt_ioc.rspval = false;
    if( !fbf_ok ) signal_vci_tgt_fbf.rspval = false;

    signal_resetn = false;
    sc_start( sc_time( 1, SC_NS ) ) ;

    signal_resetn = true;
    for ( size_t n=1 ; n<n_cycles ; n++ )
    {
        sc_start( sc_time( 1 , SC_NS ) ) ;

        if( stats_ok && (n%10 == 0) )
            proc[0]->file_stats(stats_file);
        if( trace_ok )
            proc[0]->file_trace(trace_file);

        if( debug_ok && (n > from_cycle) && (n < to_cycle) )
        {
            std::cout << "***************** cycle " << std::dec << n
                << " ***********************" << std::endl;
            for( size_t p = 0 ; p < n_procs ; p++)
                proc[p]->print_trace();
            bus->print_trace();
            timer->print_trace();
            rom->print_trace();
            ram->print_trace();

            signal_vci_init_proc[0].print_trace("signal_proc_0");
            signal_vci_tgt_rom.print_trace("signal_rom");
            signal_vci_tgt_ram.print_trace("signal_ram");
        }
    }

    /* forcing xterm to quit too */
    kill(0, SIGINT);

    return 0;
}

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

