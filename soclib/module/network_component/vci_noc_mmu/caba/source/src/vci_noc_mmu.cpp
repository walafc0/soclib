/* -*- c++ -*-C
 * File : vci_noc-mmu.cpp
 * Copyright (c) UPMC, Lip6, SoC
 * Author : Alain Greiner
 * Date : 11/11/2012
 *
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
 */

#include <cassert>
#include "arithmetics.h"
#include "alloc_elems.h"
#include "../include/vci_noc_mmu.h"
#include "../../../include/soclib/noc_mmu.h"

////////////////////////////////////////////////////////////////////////////////////
// This component implements a generic NOC MMU service, that can be used
// by any VCI initiator, to implement a secure partitioning of the physical address
// space between several virtual machines (VM). The VM address is translated to
// a physical address when entering the NoC, and access rights are checked.
//
// There is of course one page table per VM, that must be statically defined
// by the Hypervisor, before launching the VM, and there is one VCI_NOC_MMU
// component per VCI initiator.
//
// As a VCI initiator can be shared by several virtual machines, the VCI_NOC_NMU
// component contains one private TLB for each VM. The number of supported VM
// is a constructor parameter, that cannot be larger than 8.
// For each VCI command, the VM index (vm) is contained in the VCI PKTID field.
//
// Each VCI_NOC_MMU channel associated to a virtual machine is called NMU[vm],
// and is controled by private set of configuration registers, corresponding
// to a 32 bytes sub-segment:
// - NOC_MMU_PTPR[vm]  (R/W) : Page Table Pointer Register
// - NOC_MMU_MODE[vm]  (R/W) : NMU mode
// - NOC_MMU_BVAR[vm]  (R)   : Bad Virtual Address Register
// - NOC_MMU_ERROR[vm] (R)   : Error type
// - NOC_MMU_INVAL[vm] (W)   : Invalidate a PTE
// The max size of the configuration segment is therefore 8*32 = 256 bytes,
// for 8 virtual machines.
//
// In case of failure (access write violation, or unmapped VM address) for a VM,
// an error response is returned to the VCI initiator, the NMU[vm] enters the FAILURE
// mode, and an interrupt IRQ[vm] is rised to the hypervisor.
//
// Each NMU[vm] can be in four modes:
// - BLOCKED  : the VCI initiator command is not consumed => all NMUs are stalled
// - FAILURE  : failure detected => all VCI CMD from (vm) return an error RSP
// - IDENTITY : (paddr = vaddr) and no access rights checking for (vm)
// - ACTIVE   : TLB(vm) is actived and access rights are checked for (vm)
//
// This component implements an hardware "table walk" to handle the TLB Miss.
////////////////////////////////////////////////////////////////////////////////////
// Implementation note:
//
// This component uses the SoCLib generic TLB, and relies on the same two levels
// page table organisation as the vci_vcache_wrapper component, with restrictions:
// - Both the virtual address and the physical address must be 32 bits.
// - The page tables used by this component must only contain 4 KBytes pages
//   (2 Mbytes pages are not supported).
// - The page table L/R access bits, and the dirty bit are NOT updated by the
//   NMU component, as the page tables are statically defined in the boot phase,
//   and should not be modified during execution...
//
// This component implements a prefetch buffer containing several contiguous PTE2
// The number of PTE2 fetched in case of TLB miss is a constructor parameter
// (usualy defined by the cache line width). It is directly written by the RSP FSM
// when it must be reloaded.
//
// The NOC_MMU component send two types of VCI commands to the NoC:
// - commands from the VCI initiator (after address translation) : TRDID < 8
// - commands to access the page tables in case of miss TLB :      TRDID = 8
//
// The NOC_MMU component contains 3 FSMs:
// - The CMD FSM handles (sequencially) the commands from the VCI initiator,
//   including the TLB miss.
// - The CONFIG FSM handles the configuration commands from the Hypervisor,
//   for the various VMs.
// - The RSP FSM handles (sequencially) the responses from the NOC.
//
// The NOC_MMU component contains 2 FIFOs:
// - The r_cmd_fifo_*** contains the VCI output commands to the NoC (both the
//   translated commands from the VCI initiator, and the table walk commands)
// - The r_rsp_fifo_* contains the VCI responses (from the NOC, or directly
//   from the CMD FSM in case of failure.
//
// As there is only one VCI port from the VCI initiator, and we don't want to
// register the VCI command in a private VCI FIFO for each VM, the NOC_MMU component
// provide only a "best effort" service to the virtual machines :
// In case of TLB miss, the  incoming VCI command, and therefore, all VMs
// are stalled until the TLB miss is resolved.
////////////////////////////////////////////////////////////////////////////////////
// Debug services:
// All debug messages are conditionned by two variables:
// - compile time : DEBUG_*** : defined below
// - execution time : debug  : defined by constructor arguments
//    debug = (m_debug_ok) and (m_cpt_cycle > m_debug_start)
////////////////////////////////////////////////////////////////////////////////////

#define DEBUG_CMD_FSM	    	1
#define DEBUG_RSP_FSM	    	1
#define DEBUG_CONFIG_FSM		1

namespace soclib {
namespace caba {

// using namespace soclib::common;

#define tmpl(x)  template<typename vci_param> x VciNocMmu<vci_param>

//////////////////////
tmpl(/**/)::VciNocMmu(sc_module_name 	    name,
                      const MappingTable 	&mt,
                      const IntTab 	        tgtid,
                      const IntTab 	        srcid,
                      const uint32_t        vms,
                      const uint32_t        cache_words,
                      const uint32_t		tlb_ways,
                      const uint32_t		tlb_sets,
                      const uint32_t		debug_start,
                      const bool			debug_ok )
           :
           soclib::caba::BaseModule(name),

           p_clk("p_clk"),
           p_resetn("p_resetn"),
           p_irq(soclib::common::alloc_elems<sc_core::sc_out<bool> >("p_irq", vms)),
           p_vci_ini("p_vci_ini"),
           p_vci_tgt("p_vci_tgt"),
           p_vci_config("p_vci_config"),

           m_vms(vms),
           m_words(cache_words),
           m_segment(mt.getSegment(tgtid)),
           m_srcid(mt.indexForId(srcid)),
           m_ways(tlb_ways),
           m_sets(tlb_sets),
           m_debug_start(debug_start),
           m_debug_ok(debug_ok),

           r_ptpr(alloc_elems<sc_signal<uint32_t> >("r_ptpr",vms)),
           r_mode(alloc_elems<sc_signal<uint32_t> >("r_mode",vms)),
           r_bvar(alloc_elems<sc_signal<uint32_t> >("r_bvar",vms)),
           r_xcode(alloc_elems<sc_signal<uint32_t> >("r_xcode",vms)),

           r_cmd_fsm("r_cmd_fsm"),
           r_cmd_paddr("r_cmd_paddr"),
           r_cmd_srcid("r_cmd_srcid"),
           r_cmd_trdid("r_cmd_trdid"),
           r_cmd_pktid("r_cmd_pktid"),
           r_cmd_tlb_way("r_cmd_tlb_way"),
           r_cmd_tlb_set("r_cmd_tlb_set"),
           r_cmd_bypass("r_cmd_bypass"),
           r_cmd_ptba("r_cmd_ptba"),

           r_rsp_fsm("r_rsp_fsm"),
           r_rsp_buf_ppn(alloc_elems<sc_signal<uint32_t> >("r_rsp_buf_ppn",vms,cache_words/2)),
           r_rsp_buf_flags(alloc_elems<sc_signal<uint32_t> >("r_rsp_buf_flags",vms,cache_words/2)),
           r_rsp_buf_tag(alloc_elems<sc_signal<uint32_t> >("r_rsp_buf_tag",vms)),
           r_rsp_buf_val(alloc_elems<sc_signal<bool> >("r_rsp_buf_val",vms)),
           r_rsp_ptd("r_rsp_ptd"),
           r_rsp_ptd_val("r_rsp_ptd_val"),
           r_rsp_pte_count("r_rsp_pte_count"),
           r_rsp_pt_error("r_rsp_pt_error"),

           r_config_fsm("r_config_fsm"),
           r_config_vm("r_config_vm"),
           r_config_wdata("r_config_wdata"),
           r_config_srcid("r_config_srcid"),
           r_config_trdid("r_config_trdid"),
           r_config_pktid("r_config_pktid"),

           r_cmd_fifo_address("r_cmd_fifo_address",2),
           r_cmd_fifo_srcid("r_cmd_fifo_srcid",2),
           r_cmd_fifo_trdid("r_cmd_fifo_trdid",2),
           r_cmd_fifo_pktid("r_cmd_fifo_pktid",2),
           r_cmd_fifo_be("r_cmd_fifo_be",2),
           r_cmd_fifo_cmd("r_cmd_fifo_cmd",2),
           r_cmd_fifo_wdata("r_cmd_fifo_wdata",2),
           r_cmd_fifo_plen("r_cmd_fifo_plen",2),
           r_cmd_fifo_contig("r_cmd_fifo_contig",2),
           r_cmd_fifo_cons("r_cmd_fifo_cons",2),
           r_cmd_fifo_wrap("r_cmd_fifo_wrap",2),
           r_cmd_fifo_eop("r_cmd_fifo_eop",2),

           r_rsp_fifo_rdata("r_rsp_fifo_rdata",2),
           r_rsp_fifo_rsrcid("r_rsp_fifo_rsrcid",2),
           r_rsp_fifo_rtrdid("r_rsp_fifo_rtrdid",2),
           r_rsp_fifo_rpktid("r_rsp_fifo_rpktid",2),
           r_rsp_fifo_rerror("r_rsp_fifo_rerror",2),
           r_rsp_fifo_reop("r_rsp_fifo_reop",2),

           r_config2cmd_req("r_config2cmd_req"),
           r_cmd2rsp_req("r_cmd2rsp_req"),
           r_rsp2cmd_error("r_rsp2cmd_error")
{
    assert( (vci_param::P >=  4) and
            "NMU ERROR: VCI PKTID field must have 4 bits");
    assert( (vci_param::T >=  4) and
            "NMU ERROR: VCI TRDID field must have 4 bits");
    assert( (vci_param::N ==  32) and
            "NMU ERROR: VCI ADDRESS field must have 32 bits");
    assert( (vci_param::B ==  4) and
            "NMU ERROR: VCI DATA field must have 32 bits");
    assert( ( (cache_words == 2) or (cache_words == 4) or
              (cache_words == 8) or (cache_words == 16) ) and
            "NMU ERROR: The cache_words argument must be 2,4,8, or 16");
    assert( (vms > 0) and (vms < 9) and
            "NMU ERROR: The vms argument cannot be larger than 8");

    for ( size_t vm = 0 ; vm < m_vms ; vm++ )
    {
        r_tlb[vm] = new GenericTlb<uint32_t>( "tlb", vm, tlb_ways, tlb_sets, 32 );
    }

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

} // end constructor

/////////////////////////////////////
tmpl(/**/)::~VciNocMmu()
/////////////////////////////////////
{
    for ( size_t vm = 0 ; vm < m_vms ; vm++ )
    {
        delete r_tlb[vm];
    }
}

////////////////////////////////////
tmpl(void)::print_trace(size_t mode)
////////////////////////////////////
{
    const char *cmd_fsm_str[] =
    {
            "CMD_IDLE",
            "CMD_SEND",
            "CMD_FAIL_WAIT_EOP",
            "CMD_FAIL_WAIT_RSP",
            "CMD_MISS_CHECK",
            "CMD_MISS_READ_PTD",
            "CMD_MISS_WAIT_PTD",
            "CMD_MISS_READ_PTE",
            "CMD_MISS_WAIT_PTE",
            "CMD_MISS_TLB_UPDT",
    };
    const char *rsp_fsm_str[] =
    {
            "RSP_IDLE",
            "RSP_DATA",
            "RSP_PTE_FLAGS",
            "RSP_PTE_PPN",
            "RSP_PTD",
            "RSP_FAILURE",
    };
    const char *config_fsm_str[] =
    {
            "CONFIG_IDLE",
            "CONFIG_PTPR_WRITE",
            "CONFIG_MODE_WRITE",
            "CONFIG_PTPR_READ",
            "CONFIG_MODE_READ",
            "CONFIG_BVAR_READ",
            "CONFIG_XCODE_READ",
            "CONFIG_INVAL_PTE",
            "CONFIG_ERROR_WAIT",
            "CONFIG_ERROR_RSP",
    };
    const char *mode_str[] =
    {
            "BLOCKED",
            "FAILURE",
            "IDENTITY",
            "ACTIVATE",
    };

    std::cout << std::dec << "NMU " << name()
              << "   " << cmd_fsm_str[r_cmd_fsm.read()]
              << " | " << rsp_fsm_str[r_rsp_fsm.read()]
              << " | " << config_fsm_str[r_config_fsm.read()] << std::endl;

    for ( size_t vm = 0 ; vm < m_vms ; vm++ )
    {
        std::cout << "  VM " << std::dec << vm
                  << " : MODE = " << mode_str[r_mode[vm].read()]
                  << " / PTPR = " << std::hex << r_ptpr[vm] << std::endl;
        if ( mode )
        {
            r_tlb[vm]->printTrace();
        }
    }
}

/////////////////////////////////////
tmpl(void)::print_stats(uint32_t vm )
/////////////////////////////////////
{
    std::cout << "NMU " << name() << "VM " << std::dec << vm << std::endl
              << "- TLB MISS RATE = " << (float)m_cpt_tlb_miss[vm]/m_cpt_tlb_read[vm] << std::endl
              << "- TLB MISS COST = " << (float)m_cost_tlb_miss[vm]/m_cpt_tlb_miss[vm] << std::endl;
}

//////////////////////////////////////
tmpl(void)::clear_stats( uint32_t vm )
//////////////////////////////////////
{
    m_cpt_tlb_read[vm]  = 0;
    m_cpt_tlb_miss[vm]  = 0;
    m_cost_tlb_miss[vm] = 0;
}

/////////////////////////
tmpl(void)::transition()
/////////////////////////
{
    if ( not p_resetn.read() )
    {
        // vms ressources
        for ( size_t vm = 0 ; vm < m_vms ; vm++ )
        {
            r_tlb[vm]->reset();
            r_mode[vm]        = NMU_MODE_BLOCKED;
            r_xcode[vm]       = NO_ERROR;
            r_rsp_buf_val[vm] = false;
        }

        // FSMs
        r_cmd_fsm	 = CMD_IDLE;
        r_rsp_fsm	 = RSP_IDLE;
        r_config_fsm = CONFIG_IDLE;

        // output CMD FIFOs
        r_cmd_fifo_address.init();
        r_cmd_fifo_srcid.init();
        r_cmd_fifo_trdid.init();
        r_cmd_fifo_pktid.init();
        r_cmd_fifo_be.init();
        r_cmd_fifo_cmd.init();
        r_cmd_fifo_contig.init();
        r_cmd_fifo_wdata.init();
        r_cmd_fifo_eop.init();
        r_cmd_fifo_cons.init();
        r_cmd_fifo_plen.init();
        r_cmd_fifo_wrap.init();

        // output RSP FIFOs
        r_rsp_fifo_rsrcid.init();
        r_rsp_fifo_rtrdid.init();
        r_rsp_fifo_rpktid.init();
        r_rsp_fifo_rdata.init();
        r_rsp_fifo_rerror.init();
        r_rsp_fifo_reop.init();

        // Communication flip-flops
        r_rsp_ptd_val         = false;
        r_rsp_pt_error        = false;
        r_cmd2rsp_req	      = false;
        r_config2cmd_req      = false;

        // activity counters
        m_cpt_total_cycles     = 0;
        for ( size_t vm = 0 ; vm < m_vms ; vm++ )
        {
            m_cpt_tlb_read[vm]  = 0;
            m_cpt_tlb_miss[vm]  = 0;
            m_cost_tlb_miss[vm] = 0;
        }

        return;
    }

    // default values for CMD FIFO
    bool          cmd_fifo_put = false;
    uint32_t      cmd_fifo_address;
    uint32_t      cmd_fifo_srcid;
    uint32_t      cmd_fifo_trdid;
    uint32_t      cmd_fifo_pktid;
    uint32_t      cmd_fifo_be;
    uint32_t      cmd_fifo_cmd;
    uint32_t      cmd_fifo_wdata;
    uint32_t      cmd_fifo_plen;
    bool          cmd_fifo_contig;
    bool          cmd_fifo_cons;
    bool          cmd_fifo_wrap;
    bool          cmd_fifo_eop;
    bool          cmd_fifo_get = p_vci_ini.cmdack.read();

    // default values for RSP FIFO
    bool          rsp_fifo_put = false;
    uint32_t      rsp_fifo_rdata;
    uint32_t      rsp_fifo_rerror;
    uint32_t      rsp_fifo_rsrcid;
    uint32_t      rsp_fifo_rtrdid;
    uint32_t      rsp_fifo_rpktid;
    bool          rsp_fifo_reop;
    bool          rsp_fifo_get = p_vci_tgt.rspack.read();

    m_cpt_total_cycles++;

    bool debug = (m_cpt_total_cycles > m_debug_start) and m_debug_ok;

    ////////////////////////////////////////////////////////////////////////////////////
    // The CMD FSM handles the commands from the VCI initiator.
    // It performs address translation and access rights checking.
    // It requests an error response to RSP FSM in case of access right violation,
    // or in case of illegal address (page unmaped or bus error).
    // It handles the TLB miss, looking in the prefetch buffer first.
    // It directly access the page table in case of miss on the prefetch buffer,
    // using the bypass mechanism supported by the generic TLB: It checks the bypass
    // condition in the CMD_MISS_CHECK state, and save one memory access when
    // the virtual address IX1 field has not been modified from the previous PTD read.
    // The prefetch buffer contains all PTE2 contained in a cache line (m_words/2).
    // The actual writing in the prefetch buffer is done by the RSP FSM.
    // The prefetch buffer tag is the common part (right shifted) of the physical
    // address for all PTEs contained in the prefetch buffer.
    // The VCI command is only consumed in CMD_SEND, CMD_FAIL_WAIT_EOP,
    // and CMD_FAIL_WAIT_RSP states.
    // Finally the CMD FSM handles PTE invalidation requests from CONFIG FSM,
    // invalidating both the TLB entry (if hit) and the prefetch buffer (if hit).
    // These invalidation requests have the highest priority.
    ////////////////////////////////////////////////////////////////////////////////////
    // TODO:
    // in case of miss on the prefetch buffer, the CMD FSM systematically read the PTD
    // before sending the command to get the cache line containing the missing PTE.
    ////////////////////////////////////////////////////////////////////////////////////

    switch( r_cmd_fsm.read() )
    {
        //////////////
        case CMD_IDLE:  // waiting a VCI command or an inval request from CONFIG FSM
        {
            if ( r_config2cmd_req.read() )   // invalidation request from CONFIG FSM
                                             // both TLB and prefetch buffer are cleared
            {
                uint32_t vm      = r_config_vm.read();
                uint32_t vpn     = r_config_wdata.read() >> 12;
                uint32_t tag     = vpn / (m_words >> 1);

                // inval pte in TLB
                bool     tlb_hit = r_tlb[vm]->inval( r_config_wdata.read() );

                // reset prefetch buffer
                bool     buf_hit = ( tag == r_rsp_buf_tag[vm].read() );
                if ( buf_hit ) r_rsp_buf_val[vm] = false;

                // reset request flip-flop
                r_config2cmd_req = false;

#if DEBUG_CMD_FSM
if( debug )
{
std::cout << "  <NMU[" << name() << "] CMD_IDLE> PTE invalidation request :";
if ( tlb_hit ) std::cout << " TLB cleared";
if ( buf_hit ) std::cout << " / prefetch buffer cleared";
std::cout << std::endl;
}
#endif
            }
            else if ( p_vci_tgt.cmdval.read() )  // request from VCI initiator
            {
                uint32_t vm = p_vci_tgt.pktid.read(); // VM index

                assert( (vm < 8 ) and
                "NMU ERROR : The VM index (PKTID) cannot be larger than 7");

                if ( r_mode[vm].read() == NMU_MODE_BLOCKED )  // stalled => no response
                {
#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name() << "] CMD_IDLE> VCI command from VM " 
          << std::dec << vm << " : BLOCKED" << std::endl;
#endif
                    break;
                }
                else if ( r_mode[vm].read() == NMU_MODE_FAILURE )  //  error response
                {
#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name() << "] CMD_IDLE> VCI command from VM " 
          << std::dec << vm << " : FAILURE" << std::endl;
#endif
                    r_cmd_trdid = p_vci_tgt.trdid.read();
                    r_cmd_pktid = p_vci_tgt.pktid.read();
                    r_cmd_srcid = p_vci_tgt.srcid.read();

                    if ( p_vci_tgt.eop.read() )
                    {
                        r_cmd2rsp_req = true;
                        r_cmd_fsm     = CMD_FAIL_WAIT_RSP;
                    }
                    else
                    {
                        r_cmd_fsm = CMD_FAIL_WAIT_EOP;
                    }
                }
                else if ( r_mode[vm].read() == NMU_MODE_IDENTITY )  // identity mapping
                {
#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name() << "] CMD_IDLE> VCI command from VM " 
          << std::dec << vm << " : IDENTITY MAPPING" << std::endl;
#endif
                    r_cmd_paddr = p_vci_tgt.address.read();
                    r_cmd_fsm   = CMD_SEND;
                }
                else                                           // TLB activated
                {
                    uint32_t    paddr;
                    pte_info_t  flags;
                    size_t      way;
                    size_t      set;
                    uint32_t    nline;
                    bool	    hit;

                    m_cpt_tlb_read[vm]++;

                    r_cmd_trdid = p_vci_tgt.trdid.read();
                    r_cmd_pktid = p_vci_tgt.pktid.read();
                    r_cmd_srcid = p_vci_tgt.srcid.read();

                    hit = r_tlb[vm]->translate( (uint32_t)p_vci_tgt.address.read(),
                                                &paddr,
                                                &flags,
                                                &nline,   // unused
                                                &way,	  // unused
                                                &set );   // unused

                    if ( hit )	// TLB hit : access rights checking
                    {
                        if ( not flags.w and (p_vci_tgt.cmd.read() == vci_param::CMD_WRITE) )
                        {
#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name() << "] CMD_IDLE> VCI command from VM " 
          << std::dec << vm << " : TLB HIT, but writable violation" << std::endl;
#endif
                            //  a VCI response error must be returned by the RSP FSM
                            r_xcode[vm]   = WRITE_ACCESS_VIOLATION;
                            r_bvar[vm]    = p_vci_tgt.address.read();
                            r_mode[vm]    = NMU_MODE_FAILURE;
                            if ( p_vci_tgt.eop.read() )
                            {
                                r_cmd2rsp_req = true;
                                r_cmd_fsm       = CMD_FAIL_WAIT_RSP;
                            }
                            else
                            {
                                r_cmd_fsm = CMD_FAIL_WAIT_EOP;
                            }
                        }
                        else  // TLB HIT and no access rights violation
                        {
#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name() << "] CMD_IDLE> VCI command from VM " 
          << std::dec << vm << " : TLB HIT, access rights OK" << std::endl;
#endif
                            r_cmd_paddr = paddr;
                            r_cmd_fsm   = CMD_SEND;
                        }
                    }
                    else      // TLB miss: we enter TLB_MISS treatment sub-fsm
                    {
#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name() << "] CMD_IDLE> VCI command from VM " 
          << std::dec << vm << " : TLB MISS" << std::endl;
#endif
                        m_cpt_tlb_miss[vm]++;
                        r_cmd_fsm = CMD_MISS_CHECK;
                    }
                } // end if tlb activated
            } // end if cmdval
            break;
        }
        //////////////
        case CMD_SEND:	// write the VCI command in r_cmd-fifos
        {
            if ( p_vci_tgt.cmdval && r_cmd_fifo_address.wok() )
            {
                // If contig, we have to increase pddr 
                r_cmd_paddr = r_cmd_paddr.read() + (p_vci_tgt.contig.read() ? 4 : 0);

                cmd_fifo_put     = true;
                cmd_fifo_address = r_cmd_paddr.read();
                cmd_fifo_srcid   = p_vci_tgt.srcid.read();
                cmd_fifo_trdid   = p_vci_tgt.trdid.read();
                cmd_fifo_pktid   = p_vci_tgt.pktid.read();
                cmd_fifo_be      = p_vci_tgt.be.read();
                cmd_fifo_cmd     = p_vci_tgt.cmd.read();
                cmd_fifo_wdata   = p_vci_tgt.wdata.read();
                cmd_fifo_plen    = p_vci_tgt.plen.read();
                cmd_fifo_contig  = p_vci_tgt.contig.read();
                cmd_fifo_cons    = p_vci_tgt.cons.read();
                cmd_fifo_wrap    = p_vci_tgt.wrap.read();
                cmd_fifo_eop     = p_vci_tgt.eop.read();

                if( p_vci_tgt.eop )    r_cmd_fsm = CMD_IDLE;

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_SEND> Push VCI command into cmd_fifo :"
          << " address = " << std::hex << cmd_fifo_address
          << " srcid = "   << std::hex << cmd_fifo_srcid
          << " trdid = "   << std::hex << cmd_fifo_trdid
          << " pktid = "   << std::hex << cmd_fifo_pktid
          << " wdata = "   << std::hex << cmd_fifo_wdata
          << " be = "      << std::hex << cmd_fifo_be
          << " cmd = "     << std::dec << cmd_fifo_cmd
          << " plen = "    << std::dec << cmd_fifo_plen
          << " eop = "     << std::dec << cmd_fifo_eop << std::endl;
#endif
            }
            break;
        }
        ///////////////////////
        case CMD_FAIL_WAIT_EOP:     // waiting  last flit of failing VCI command
        {
            if ( p_vci_tgt.cmdval.read() and p_vci_tgt.eop.read() )
            {
                r_cmd2rsp_req = true;
                r_cmd_fsm     = CMD_FAIL_WAIT_RSP;
            }
            break;
        }
        ///////////////////////
        case CMD_FAIL_WAIT_RSP:     // returns to IDLE state when the error
                                    // response has been sent by the RSP FSM
        {
            if( not r_cmd2rsp_req.read() )
            {
                r_cmd_fsm = CMD_IDLE;
            }
            break;
        }
        ////////////////////
        case CMD_MISS_CHECK:  // first step of TLB miss : we select a slot in TLB,
                              // and check if the missing PTE is in prefetch buffer
                              // using the tag and index values extracted from vaddr.
                              // we also check the PTD bypass condition on the TLB
        {
            uint32_t vm    = r_cmd_pktid.read();
            uint32_t vaddr = p_vci_tgt.address.read();
            uint32_t vpn   = vaddr >> 12;
            uint32_t tag   = vpn / (m_words >> 1);
            uint32_t index = vpn & ((m_words >> 1) - 1);
            size_t   way;
            size_t   set;
            uint32_t ptba;

            m_cost_tlb_miss[vm]++;

            // compute prefetch buffer miss
            bool buf_hit = (tag == r_rsp_buf_tag[vm].read()) and r_rsp_buf_val[vm].read();

            // compute PTD bypass condition
            bool     bypass = r_tlb[vm]->get_bypass(vaddr, &ptba);

            // select a slot in TLB
            r_tlb[vm]->select( vaddr,
                               false,  // it is a PTE2
                               &way,
                               &set );
#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_CHECK> vaddr = "
          << std::hex << vaddr
          << " set = " << set
          << std::endl;
#endif

            if ( buf_hit ) // hit in prefetch buffer
            {
                uint32_t pte_flags = r_rsp_buf_flags[vm][index].read();

                if ( pte_flags & PTE_V_MASK ) // hit in prefetch buffer, and PTE valid
                {

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_CHECK> PTE valid in prefetch buffer" << std::endl;
#endif
                    r_cmd_fsm = CMD_MISS_TLB_UPDT;
                }
                else                        // hit in prefetch buffer, but PTE unmapped
                {

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_CHECK> PTE unmapped in prefetch buffer" << std::endl;
#endif
                    //  a VCI response error must be returned by the RSP FSM
                    r_xcode[vm]   = PT2_ENTRY_UNMAPPED;
                    r_bvar[vm]    = p_vci_tgt.address.read();
                    r_mode[vm]    = NMU_MODE_FAILURE;

                    if ( p_vci_tgt.eop.read() )
                    {
                        r_cmd2rsp_req = true;
                        r_cmd_fsm     = CMD_FAIL_WAIT_RSP;
                    }
                    else
                    {
                        r_cmd_fsm = CMD_FAIL_WAIT_EOP;
                    }
                }
            }
            else           // miss in prefetch buffer
            {

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_CHECK> Miss in prefetch buffer : "
          << " TAG = " << std::hex << r_rsp_buf_tag[vm].read()
          << " / VADDR = " << vaddr << std::endl;
#endif
                // The prefetch buffer must be invalidated
                r_rsp_buf_val[vm] = false;

                // Page table access depends on bypass
                if (bypass)                               // PTD read bypass
                {
                    r_cmd_bypass = true;
                    r_cmd_ptba   = ptba;
                    r_cmd_fsm    = CMD_MISS_READ_PTE;
                }
                else                                      // must read PTD
                {
                    r_cmd_bypass = false;
                    r_cmd_fsm    = CMD_MISS_READ_PTD;
                }
            }
            r_cmd_tlb_way = way;
            r_cmd_tlb_set = set;
            break;
        }
        ///////////////////////
        case CMD_MISS_READ_PTD:  // post a PTD read request in r_cmd_fifo
        {
            uint32_t vm    = r_cmd_pktid.read();

            m_cost_tlb_miss[vm]++;

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_READ_PTD> set = " << r_cmd_tlb_set.read()
          << std::endl;
#endif

            if ( r_cmd_fifo_address.wok() )
            {
                uint32_t vaddr   = (uint32_t)p_vci_tgt.address.read();
                uint32_t ix1     = vaddr >> IX1_SHIFT;

                cmd_fifo_put     = true;
                cmd_fifo_address = (r_ptpr[vm].read() << PTPR_SHIFT) | (ix1 << 2);
                cmd_fifo_srcid   = m_srcid;
                cmd_fifo_trdid   = NMU_PT_ACCESS_PTD;
                cmd_fifo_pktid   = 0;
                cmd_fifo_be      = 0xF;
                cmd_fifo_cmd     = vci_param::CMD_READ;
                cmd_fifo_wdata   = 0;
                cmd_fifo_plen    = 4;
                cmd_fifo_contig  = true;
                cmd_fifo_cons    = false;
                cmd_fifo_wrap    = false;
                cmd_fifo_eop     = true;

                r_cmd_fsm = CMD_MISS_WAIT_PTD;

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_READ_PTD> Push PTD read into cmd_fifo :"
          << " address = " << std::hex << cmd_fifo_address
          << " srcid = "   << std::hex << cmd_fifo_srcid
          << " trdid = "   << std::hex << cmd_fifo_trdid
          << " pktid = "   << std::hex << cmd_fifo_pktid
          << " wdata = "   << std::hex << cmd_fifo_wdata
          << " be = "      << std::hex << cmd_fifo_be
          << " cmd = "     << std::dec << cmd_fifo_cmd
          << " plen = "    << std::dec << cmd_fifo_plen
          << " eop = "     << std::dec << cmd_fifo_eop << std::endl;
#endif
            }
            break;
        }
        ///////////////////////
        case CMD_MISS_WAIT_PTD:  // wait the PTD value from RSP FSM
                                 // and check possible errors
        {
            uint32_t xcode = NO_ERROR;
            uint32_t vm    = r_cmd_pktid.read();
            uint32_t ptd   = r_rsp_ptd.read();

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_WAIT_PTD> set = " << r_cmd_tlb_set.read() << std::endl;
#endif

            m_cost_tlb_miss[vm]++;

            if ( r_rsp_pt_error.read() )   // bus error
            {
                r_rsp_pt_error = false;
                xcode = READ_PT1_BUS_ERROR;
            }

            if ( r_rsp_ptd_val.read() )   // response available
            {
                if      ( not (ptd & PTE_V_MASK) ) xcode = PT1_ENTRY_UNMAPPED;
                else if ( not (ptd & PTE_T_MASK) ) xcode = UNSUPPORTED_BIG_PAGE;
            }

            if ( r_rsp_ptd_val.read() or r_rsp_pt_error.read() )
            {
                if ( (xcode == NO_ERROR) ) // PTD OK
                {

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_WAIT_PTD> Received PTD = "
          << std::hex << ptd << std::endl;
#endif

                    // register bypass in TLB
                    r_tlb[vm]->set_bypass( p_vci_tgt.address.read(),  // vaddr
                                           r_rsp_ptd.read(),          // ptba
                                           0 );                       // nline (unused)

                    r_cmd_fsm = CMD_MISS_READ_PTE;
                }
                else    // error detected
                {

#if DEBUG_CMD_FSM
if( debug )
{
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_WAIT_PTD> ERROR : ";
if (xcode == READ_PT1_BUS_ERROR)      std::cout << "READ_PT1_BUS_ERROR ";
if (xcode == PT1_ENTRY_UNMAPPED)      std::cout << "PT1_ENTRY_UNMAPPED ";
if (xcode == UNSUPPORTED_BIG_PAGE)    std::cout << "UNSUPPORTED_BIG_PAGE ";
std::cout << "for vaddr = " << std::hex << p_vci_tgt.address.read() << std::endl;
}
#endif
                    r_mode[vm]    = NMU_MODE_FAILURE;
                    r_xcode[vm]   = xcode;
                    r_bvar[vm]    = p_vci_tgt.address.read();
                    if ( p_vci_tgt.eop.read() )
                    {
                        r_cmd2rsp_req = true;
                        r_cmd_fsm     = CMD_FAIL_WAIT_RSP;
                    }
                    else
                    {
                        r_cmd_fsm     = CMD_FAIL_WAIT_EOP;
                    }
                }
            }
            break;
        }
        ///////////////////////
        case CMD_MISS_READ_PTE:  // post a PTE read burst request in r_cmd_fifo
        {
            uint32_t vm    = r_cmd_pktid.read();

            m_cost_tlb_miss[vm]++;

            if ( r_cmd_fifo_address.wok() )
            {
                uint32_t paddr;
                uint32_t vaddr    = p_vci_tgt.address.read();
                uint32_t ix2      = (vaddr >> IX2_SHIFT) & IX2_MASK;

                if ( r_cmd_bypass.read() )
                {
                    paddr         = (r_cmd_ptba.read() << PTBA_SHIFT) | (ix2<<3);
                }
                else
                {
                    paddr         = (r_rsp_ptd.read() << PTBA_SHIFT) | (ix2<<3);
                    r_rsp_ptd_val = false;
                }

                cmd_fifo_put      = true;
                cmd_fifo_address  = (paddr & 0xFFFFFFC0);
                cmd_fifo_srcid    = m_srcid;
                cmd_fifo_trdid    = NMU_PT_ACCESS_PTE;
                cmd_fifo_pktid    = 0;
                cmd_fifo_be       = 0xF;
                cmd_fifo_cmd      = vci_param::CMD_READ;
                cmd_fifo_wdata    = 0;
                cmd_fifo_plen     = (m_words << 2);
                cmd_fifo_contig   = true;
                cmd_fifo_cons     = false;
                cmd_fifo_wrap     = false;
                cmd_fifo_eop      = true;

                r_cmd_fsm         = CMD_MISS_WAIT_PTE;

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_READ_PTD> Push PTD read into cmd_fifo :"
          << " address = " << std::hex << cmd_fifo_address
          << " srcid = "   << std::hex << cmd_fifo_srcid
          << " trdid = "   << std::hex << cmd_fifo_trdid
          << " pktid = "   << std::hex << cmd_fifo_pktid
          << " wdata = "   << std::hex << cmd_fifo_wdata
          << " be = "      << std::hex << cmd_fifo_be
          << " cmd = "     << std::dec << cmd_fifo_cmd
          << " plen = "    << std::dec << cmd_fifo_plen
          << " eop = "     << std::dec << cmd_fifo_eop << std::endl;
#endif

            }
            break;
        }
        ///////////////////////
        case CMD_MISS_WAIT_PTE:    // wait availability of the prefetch buffer
                                   // that is written by the RSP FSM
        {
            uint32_t vm    = r_cmd_pktid.read();

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_READ_PTE> set = " << r_cmd_tlb_set.read()
          << std::endl;
#endif

            m_cost_tlb_miss[vm]++;

            if ( r_rsp_pt_error.read() )           // bus error
            {

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_WAIT_PTE> ERROR : READ_PT2_BUS_ERROR for vaddr = "
          << std::hex << p_vci_tgt.address.read() << std::endl;
#endif
                r_rsp_pt_error = false;
                r_mode[vm]    = NMU_MODE_FAILURE;
                r_xcode[vm]   = READ_PT2_BUS_ERROR;
                r_bvar[vm]    = p_vci_tgt.address.read();
                if ( p_vci_tgt.eop.read() )
                {
                    r_cmd2rsp_req = true;
                    r_cmd_fsm     = CMD_FAIL_WAIT_RSP;
                }
                else
                {
                    r_cmd_fsm = CMD_FAIL_WAIT_EOP;
                }
            }
            else if ( r_rsp_buf_val[vm].read() )   // buffer available
            {

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_WAIT_PTE_FLAGS> Prefetch buffer loaded for vaddr = "
          << std::hex << p_vci_tgt.address.read() << std::endl;
#endif
                r_cmd_fsm = CMD_MISS_TLB_UPDT;
            }
            break;
        }
        ///////////////////////
        case CMD_MISS_TLB_UPDT:  // write a new entry in TLB
        {
            uint32_t vm    = r_cmd_pktid.read();
            uint32_t vaddr = p_vci_tgt.address.read();
            uint32_t vpn   = vaddr >> 12;
            uint32_t index = vpn & ((m_words >> 1) - 1);
            uint32_t flags = r_rsp_buf_flags[vm][index].read();
            uint32_t ppn   = r_rsp_buf_ppn[vm][index].read();

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_TLB_UPDT> vaddr = "
          << std::hex << vaddr
          << " set = " << r_cmd_tlb_set.read()
          << std::endl;
#endif

            m_cost_tlb_miss[vm]++;

            if( not (flags & PTE_V_MASK) )  // PTE not valid
            {
                r_mode[vm]    = NMU_MODE_FAILURE;
                r_xcode[vm]   = PT2_ENTRY_UNMAPPED;
                r_bvar[vm]    = p_vci_tgt.address.read();
                if ( p_vci_tgt.eop.read() )
                {
                    r_cmd2rsp_req = true;
                    r_cmd_fsm     = CMD_FAIL_WAIT_RSP;
                }
                else
                {
                    r_cmd_fsm = CMD_FAIL_WAIT_EOP;
                }

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_TLB_UPDT> ERROR : PT2_ENTRY_UNMAPPED for vaddr = "
          << std::hex << p_vci_tgt.address.read() << std::endl;
#endif
            }
            else                            // PTE valid
            {
                r_tlb[vm]->write( false,     // it's not a big page
                                  flags,
                                  ppn,
                                  vaddr,
                                  r_cmd_tlb_way.read(),
                                  r_cmd_tlb_set.read(),
                                  0 );       // nline is not used in this component

                r_cmd_fsm = CMD_IDLE;

#if DEBUG_CMD_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CMD_MISS_TLB_UPDT> Write a new PTE in TLB:"
          << std::hex << " VPN = " << (vaddr>>12)
          << " PTE_FLAGS = " << r_rsp_buf_flags[vm][index].read()
          << " PTE_PPN = " << r_rsp_buf_ppn[vm][index].read() << std::endl;
#endif
            }
            break;
        }
    } // end switch CMD_FSM

    //////////////////////////////////////////////////////////////////////////
    // The RSP FSM handles VCI both responses from the NOC, and requests
    // from the CMD FSM in case of address translation failure,
    // to directly send a response error to the VCI initiator.
    // Regarding the NoC responses, the PKTID field is decoded:
    // a) PKTID < 8 : standard response to the VCI initiator
    // b) PKTID = NMU_PT_ACCESS_PTD : single flit response to a PTD read
    // c) PKTID = NMU_PT_ACCESS_PTE : multi flits response to a PTE read
    // In case of PTE, the RSP FSM directly writes the PTEs
    // in the prefetch buffer of the virtual machine selected
    // by the pending VCI command.
    // Requests from the CMD FSM in case of address translation failure,
    // have the highest priority in the IDLE state.
    //////////////////////////////////////////////////////////////////////////
    switch( r_rsp_fsm.read() )
    {
        //////////////
        case RSP_IDLE:  // waiting responses from NOC or requests from CMD FSM
        {
            if( r_cmd2rsp_req.read() )          // request from CMD FSM
            {
                r_cmd2rsp_req = false;
                r_rsp_fsm     = RSP_FAILURE;
            }
            else if( p_vci_ini.rspval.read() )  // NoC response
            {
                if( p_vci_ini.rtrdid.read() == NMU_PT_ACCESS_PTD )
                {
                    r_rsp_fsm = RSP_PTD;
                }
                else if( p_vci_ini.rtrdid.read() == NMU_PT_ACCESS_PTE )
                {
                    r_rsp_fsm       = RSP_PTE_FLAGS;
                    r_rsp_pte_count = 0;
                }
                else                                   // VCI initiator access
                {
                    r_rsp_fsm = RSP_DATA;
                }
            }
            break;
        }
        //////////////
        case RSP_DATA:  // push the NOC response into the RSP FIFO
        {
            if( p_vci_ini.rspval.read() && r_rsp_fifo_rdata.wok() )
            {
                rsp_fifo_put    = true;
                rsp_fifo_rdata  = p_vci_ini.rdata.read();
                rsp_fifo_rerror = p_vci_ini.rerror.read();
                rsp_fifo_rsrcid = p_vci_ini.rsrcid.read();
                rsp_fifo_rtrdid = p_vci_ini.rtrdid.read();
                rsp_fifo_rpktid = p_vci_ini.rpktid.read();
                rsp_fifo_reop   = p_vci_ini.reop.read();

                if( p_vci_ini.reop.read() ) r_rsp_fsm = RSP_IDLE;

#if DEBUG_RSP_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] RSP_STDANDARD> Push NoC response into rsp_fifo:"
          << " rsrcid = " << std::hex << p_vci_ini.rsrcid.read()
          << " rtrdid = " << std::hex << p_vci_ini.rtrdid.read()
          << " rpktid = " << std::hex << p_vci_ini.rpktid.read()
          << " rerror = " << std::hex << p_vci_ini.rerror.read()
          << " rdata = "  << std::hex << p_vci_ini.rdata.read() << std::endl;
#endif
            }
            break;
        }
        /////////////
        case RSP_PTD:   // write the NOC response in r_rsp_ptd register
        {
            if( p_vci_ini.rspval.read() )
            {
                assert( p_vci_ini.reop.read() and
                "NMU ERROR : The response to a Page Table access must be one flit");

                if( (p_vci_ini.rerror.read() & 0x1) == 0x1 )
                {
                    r_rsp_pt_error = true;
                }
                else
                {
                    r_rsp_ptd = p_vci_ini.rdata.read();
                    r_rsp_ptd_val = true;
                }
                r_rsp_fsm = RSP_IDLE;
            }
            break;
        }
        ///////////////////
        case RSP_PTE_FLAGS:  // write the NOC response (PTE_FLAGS) in the prefetch buffer
                             // of the virtual machine blocked by TLB miss.
        {
            if( p_vci_ini.rspval.read() )
            {
                unsigned int vm = r_cmd_pktid.read();
                unsigned int id = r_rsp_pte_count.read();

                if( (p_vci_ini.rerror.read() & 0x1) == 0x1 )
                {
                    assert( p_vci_ini.reop.read() and
                    "NMU ERROR : we expect an EOP in case of bus error response");

                    r_rsp_pt_error = true;
                    r_rsp_fsm      = RSP_IDLE;
                }
                else
                {
                    r_rsp_buf_flags[vm][id] = p_vci_ini.rdata.read();
                    r_rsp_fsm = RSP_PTE_PPN;
                }
            }
            break;
        }
        /////////////////
        case RSP_PTE_PPN:    // write the NOC response (PTE_PPN) in the prefetch buffer
                             // of the virtual machine blocked by TLB miss.
        {
            if( p_vci_ini.rspval.read() )
            {
                unsigned int vm = r_cmd_pktid.read();
                unsigned int id = r_rsp_pte_count.read();

                if( (p_vci_ini.rerror.read() & 0x1) == 0x1 )
                {
                    assert( p_vci_ini.reop.read() and
                    "NMU ERROR : we expect an EOP in case of bus error response");

                    r_rsp_pt_error = true;
                    r_rsp_fsm      = RSP_IDLE;
                }
                else
                {
                    r_rsp_buf_ppn[vm][id] = p_vci_ini.rdata.read();
                    r_rsp_pte_count = r_rsp_pte_count.read() + 1;
                    if( r_rsp_pte_count.read() == ((m_words>>1)-1) )  // last PTE
                    {
                        assert( p_vci_ini.reop.read() and
                        "NMU ERROR : we expect an EOP at the end of a PTE burst");

                        r_rsp_buf_val[vm] = true;
                        r_rsp_buf_tag[vm] = (p_vci_tgt.address.read() >> 12) / (m_words>>1);
                        r_rsp_fsm = RSP_IDLE;
                    }
                    else                                            // not the last PTE
                    {
                        assert( !p_vci_ini.reop.read() and
                        "NMU ERROR : EOP before the end of a PTE burst");

                        r_rsp_fsm = RSP_PTE_FLAGS;
                        r_rsp_pte_count = r_rsp_pte_count.read() + 1;
                    }
                }
            }
            break;
        }
        /////////////////
        case RSP_FAILURE:  // push a single flit error response into the RSP FIFO
                           // when the last flit of illegal command is arrived
        {
            if( r_rsp_fifo_rdata.wok() )
            {
                rsp_fifo_put    = true;
                rsp_fifo_rdata  = 0;
                rsp_fifo_rerror = vci_param::ERR_GENERAL_DATA_ERROR;
                rsp_fifo_rsrcid = r_cmd_srcid.read();
                rsp_fifo_rtrdid = r_cmd_trdid.read();
                rsp_fifo_rpktid = r_cmd_pktid.read();
                rsp_fifo_reop   = true;

                // reset request flip_flop
                r_cmd2rsp_req   = false;
                r_rsp_fsm       = RSP_IDLE;

#if DEBUG_RSP_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] RSP_FAILURE> Push error response into rsp_fifo:"
          << " rsrcid = " << std::hex << r_cmd_srcid.read()
          << " rtrdid = " << std::hex << r_cmd_trdid.read()
          << " rpktid = " << std::hex << r_cmd_pktid.read()
          << " rerror = 0x1"
          << " rdata = 0x0" << std::endl;
#endif
            }
            break;
        }
    } // end switch RSP_FSM

    /////////////////////////////////////////////////////////////////////
    // The CONFIG FSM handles the configuration requests. It checks
    // command type (READ/WRITE) and segmentation overflow.
    // All configuration access (read or write) must be one single flit.
    // A command flit is consumed in states IDLE and ERROR_WAIT.
    // All relevant command fields are registered in state IDLE.
    // The response flit is returnet in all other states.
    /////////////////////////////////////////////////////////////////////

    switch( r_config_fsm.read() )
    {
        /////////////////
        case CONFIG_IDLE:
        {
            if ( p_vci_config.cmdval.read() )
            {

#if DEBUG_CONFIG_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CONFIG_IDLE> Configuration command received :" <<std::endl
          << "  address = " << std::hex << p_vci_config.address.read()
          << " srcid = "   << std::hex << p_vci_config.srcid.read()
          << " trdid = "   << std::hex << p_vci_config.trdid.read()
          << " pktid = "   << std::hex << p_vci_config.pktid.read()
          << " wdata = "   << std::hex << p_vci_config.wdata.read()
          << " cmd = "     << std::hex << p_vci_config.cmd.read()
          << " plen = "    << std::dec << p_vci_config.plen.read() << std::endl;
#endif
                uint32_t   address = (uint32_t)p_vci_config.address.read();
                bool       read    = (p_vci_config.cmd.read() == vci_param::CMD_READ);
                uint32_t   cell    = (uint32_t)((address & 0x0000001C)>>2);
                uint32_t   vm      = (uint32_t)((address & 0x000000E0)>>5);

                r_config_wdata   = p_vci_config.wdata.read();
                r_config_srcid   = p_vci_config.srcid.read();
                r_config_trdid   = p_vci_config.trdid.read();
                r_config_pktid   = p_vci_config.pktid.read();
                r_config_vm      = vm;

                if ( not p_vci_config.eop.read() ) // more than one flit
                {
                    r_config_fsm = CONFIG_ERROR_WAIT;
                }
                else if ( vm >= m_vms )
                {
                    r_config_fsm = CONFIG_ERROR_RSP;
                }
                else if ( m_segment.contains(address) )
                {
                    if     (not read and (cell == NOC_MMU_PTPR))  r_config_fsm = CONFIG_PTPR_WRITE;
                    else if(not read and (cell == NOC_MMU_MODE))  r_config_fsm = CONFIG_MODE_WRITE;
                    else if(    read and (cell == NOC_MMU_PTPR))  r_config_fsm = CONFIG_PTPR_READ;
                    else if(    read and (cell == NOC_MMU_MODE))  r_config_fsm = CONFIG_MODE_READ;
                    else if(    read and (cell == NOC_MMU_BVAR))  r_config_fsm = CONFIG_BVAR_READ;
                    else if(    read and (cell == NOC_MMU_XCODE)) r_config_fsm = CONFIG_XCODE_READ;
                    else if(not read and (cell == NOC_MMU_INVAL))
                    {
                         r_config2cmd_req = true;
                         r_config_fsm     = CONFIG_INVAL_PTE;
                    }
                    else     // invalid operation.
                    {
                        r_config_fsm = CONFIG_ERROR_RSP;
                    }
                }
                else    // out of segment
                {
                    r_config_fsm = CONFIG_ERROR_RSP;
                }
            }
            break;
        }
        ///////////////////////
        case CONFIG_PTPR_WRITE:  // does nothing if the response cannot be returned
        {
            uint32_t vm = r_config_vm.read();

            if ( p_vci_config.rspack.read() )
            {
                r_ptpr[vm]    = (uint32_t)(r_config_wdata);
                r_config_fsm = CONFIG_IDLE;

#if DEBUG_CONFIG_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CONFIG_PTPR_WRITE> PTPR[" << std::dec << vm << "] = "
          <<  std::hex << (r_config_wdata) <<std::endl;
#endif
            }
            break;
        }
        ///////////////////////
        case CONFIG_MODE_WRITE: // does nothing if the response cannot be returned
        {
            uint32_t vm = r_config_vm.read();

            if ( p_vci_config.rspack.read() )
            {
                uint32_t mode = (uint32_t)(r_config_wdata & 0x3);
                r_mode[vm]    = mode;
                r_config_fsm  = CONFIG_IDLE;

#if DEBUG_CONFIG_FSM
if( debug )
{
std::cout << "  <NMU[" << name()
          << "] CONFIG_MODE_WRITE> MODE[" << std::dec << vm << "] = ";
if      ( mode == NMU_MODE_FAILURE  ) std::cout << "FAILURE"  << std::endl;
else if ( mode == NMU_MODE_IDENTITY ) std::cout << "IDENTITY" << std::endl;
else if ( mode == NMU_MODE_ACTIVATE ) std::cout << "ACTIVATE" << std::endl;
else                                  std::cout << "BLOCKED"  << std::endl;
}
#endif
            }
            break;
        }
        //////////////////////
        case CONFIG_INVAL_PTE:  // waiting invalidation acknowledge by CMD FSM
                                // does nothing if the response cannot be returned
        {
            if ( p_vci_config.rspack.read() and not r_config2cmd_req.read() )
            {
                r_config_fsm = CONFIG_IDLE;
            }
            break;
        }
        //////////////////////
        case CONFIG_PTPR_READ:  // does nothing if the response cannot be returned
        case CONFIG_MODE_READ:
        case CONFIG_BVAR_READ:
        case CONFIG_XCODE_READ:
        {
            if ( p_vci_config.rspack.read() )
            {
                r_config_fsm = CONFIG_IDLE;

#if DEBUG_CONFIG_FSM
if( debug )
{
uint32_t vm = r_config_vm.read();
std::cout << "  <NMU[" << name()
          << "] CONFIG_READ>";
if ( r_config_fsm.read() == CONFIG_PTPR_READ )
std::cout << " PTPR[" << vm << "] = " << std::hex << r_ptpr[vm].read() << std::endl;
if ( r_config_fsm.read() == CONFIG_MODE_READ )
std::cout << " MODE[" << vm << "] = " << std::hex << r_mode[vm].read() << std::endl;
if ( r_config_fsm.read() == CONFIG_BVAR_READ )
std::cout << " BVAR[" << vm << "] = " << std::hex << r_bvar[vm].read() << std::endl;
if ( r_config_fsm.read() == CONFIG_XCODE_READ )
std::cout << " XCODE[" << vm << "] = " << std::hex << r_xcode[vm].read() << std::endl;
}
#endif
            }
            break;
        }
        ///////////////////////
        case CONFIG_ERROR_WAIT:  // The error response should be send only when the
                                 // last command flit has been received (eop)
        {
            if ( p_vci_config.cmdval.read() and p_vci_config.eop.read() )
            {

#if DEBUG_CONFIG_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CONFIG_READ_ERROR_WAIT> Wait the config command last flit" << std::endl;
#endif
                r_config_fsm = CONFIG_ERROR_RSP;
            }
            break;
        }
        //////////////////////
        case CONFIG_ERROR_RSP:  // send a single flit response error
        {
            if ( p_vci_config.rspack.read() )
            {
                r_config_fsm = CONFIG_IDLE;

#if DEBUG_CONFIG_FSM
if( debug )
std::cout << "  <NMU[" << name()
          << "] CONFIG_READ_ERROR> Send a single flit error response" << std::endl;
#endif
            }
            break;
        }
    } // end switch CONFIG FSM

    // CMD FIFOs update
    r_cmd_fifo_address.update( cmd_fifo_get, cmd_fifo_put, cmd_fifo_address );
    r_cmd_fifo_cmd.update(     cmd_fifo_get, cmd_fifo_put, cmd_fifo_cmd );
    r_cmd_fifo_contig.update(  cmd_fifo_get, cmd_fifo_put, cmd_fifo_contig );
    r_cmd_fifo_cons.update(    cmd_fifo_get, cmd_fifo_put, cmd_fifo_cons );
    r_cmd_fifo_plen.update(    cmd_fifo_get, cmd_fifo_put, cmd_fifo_plen );
    r_cmd_fifo_wrap.update(    cmd_fifo_get, cmd_fifo_put, cmd_fifo_wrap );
    r_cmd_fifo_srcid.update(   cmd_fifo_get, cmd_fifo_put, cmd_fifo_srcid );
    r_cmd_fifo_trdid.update(   cmd_fifo_get, cmd_fifo_put, cmd_fifo_trdid );
    r_cmd_fifo_pktid.update(   cmd_fifo_get, cmd_fifo_put, cmd_fifo_pktid );
    r_cmd_fifo_wdata.update(   cmd_fifo_get, cmd_fifo_put, cmd_fifo_wdata );
    r_cmd_fifo_be.update(      cmd_fifo_get, cmd_fifo_put, cmd_fifo_be );
    r_cmd_fifo_eop.update(     cmd_fifo_get, cmd_fifo_put, cmd_fifo_eop );


    // RSP FIFOs update
    r_rsp_fifo_rdata.update(   rsp_fifo_get, rsp_fifo_put, rsp_fifo_rdata );
    r_rsp_fifo_rsrcid.update(  rsp_fifo_get, rsp_fifo_put, rsp_fifo_rsrcid );
    r_rsp_fifo_rtrdid.update(  rsp_fifo_get, rsp_fifo_put, rsp_fifo_rtrdid );
    r_rsp_fifo_rpktid.update(  rsp_fifo_get, rsp_fifo_put, rsp_fifo_rpktid );
    r_rsp_fifo_rerror.update(  rsp_fifo_get, rsp_fifo_put, rsp_fifo_rerror );
    r_rsp_fifo_reop.update(    rsp_fifo_get, rsp_fifo_put, rsp_fifo_reop );

} // end transition()

///////////////////////
tmpl(void)::genMoore()
///////////////////////
{
    // VCI initiator port to the NOC

    p_vci_ini.cmdval  = r_cmd_fifo_address.rok();

    p_vci_ini.address = r_cmd_fifo_address.read();
    p_vci_ini.be      = r_cmd_fifo_be.read();
    p_vci_ini.cmd     = r_cmd_fifo_cmd.read();
    p_vci_ini.wdata   = r_cmd_fifo_wdata.read();
    p_vci_ini.srcid   = r_cmd_fifo_srcid.read();
    p_vci_ini.trdid   = r_cmd_fifo_trdid.read();
    p_vci_ini.pktid   = r_cmd_fifo_pktid.read();
    p_vci_ini.plen    = r_cmd_fifo_plen.read();
    p_vci_ini.contig  = r_cmd_fifo_contig.read();
    p_vci_ini.cons    = r_cmd_fifo_cons.read();
    p_vci_ini.wrap    = r_cmd_fifo_wrap.read();
    p_vci_ini.cfixed  = false;
    p_vci_ini.clen    = 0;
    p_vci_ini.eop     = r_cmd_fifo_eop.read();

    p_vci_ini.rspack  = (((r_rsp_fsm.read() == RSP_DATA) and r_rsp_fifo_rdata.wok()) or
                         (r_rsp_fsm.read() == RSP_PTD) or
                         (r_rsp_fsm.read() == RSP_PTE_FLAGS) or
                         (r_rsp_fsm.read() == RSP_PTE_PPN) );


    // VCI target port to VCI initiator

    p_vci_tgt.rspval  = r_rsp_fifo_rdata.rok();
    p_vci_tgt.rdata   = r_rsp_fifo_rdata.read();
    p_vci_tgt.rerror  = r_rsp_fifo_rerror.read();
    p_vci_tgt.rsrcid  = r_rsp_fifo_rsrcid.read();
    p_vci_tgt.rtrdid  = r_rsp_fifo_rtrdid.read();
    p_vci_tgt.rpktid  = r_rsp_fifo_rpktid.read();
    p_vci_tgt.reop    = r_rsp_fifo_reop.read();

    p_vci_tgt.cmdack =  (r_cmd_fsm.read() == CMD_SEND) or
                        (r_cmd_fsm.read() == CMD_FAIL_WAIT_EOP) or
                        (r_cmd_fsm.read() == CMD_FAIL_WAIT_RSP);

    // VCI configuration port (to the NOC)

    p_vci_config.cmdack = (r_config_fsm.read() == CONFIG_IDLE) or
                          (r_config_fsm.read() == CONFIG_ERROR_WAIT);

    p_vci_config.rspval = (r_config_fsm.read() != CONFIG_IDLE) and
                          (r_config_fsm.read() != CONFIG_ERROR_WAIT);

    uint32_t vm = r_config_vm.read();

    if      (r_config_fsm.read()==CONFIG_PTPR_READ)  p_vci_config.rdata = r_ptpr[vm].read();
    else if (r_config_fsm.read()==CONFIG_MODE_READ)  p_vci_config.rdata = r_mode[vm].read();
    else if (r_config_fsm.read()==CONFIG_BVAR_READ)  p_vci_config.rdata = r_bvar[vm].read();
    else if (r_config_fsm.read()==CONFIG_XCODE_READ) p_vci_config.rdata = r_xcode[vm].read();
    else                                             p_vci_config.rdata = 0;

    p_vci_config.rsrcid = r_config_srcid.read();
    p_vci_config.rtrdid = r_config_trdid.read();
    p_vci_config.rpktid = r_config_pktid.read();

    p_vci_config.reop   = true;

    if (r_config_fsm.read() == CONFIG_ERROR_RSP )
        p_vci_config.rerror = vci_param::ERR_GENERAL_DATA_ERROR;
    else
        p_vci_config.rerror = vci_param::ERR_NORMAL;

    // interrupts
    for ( size_t vm = 0 ; vm < m_vms ; vm++ )
    {
        p_irq[vm] = (r_mode[vm] == NMU_MODE_FAILURE);
    }
} // end genMoore

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
