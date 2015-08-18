/* -*- c++ -*-
 * File : vci_noc_mmu.h
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

#ifndef SOCLIB_CABA_VCI_NOC_MMU_H
#define SOCLIB_CABA_VCI_NOC_MMU_H

#include <inttypes.h>
#include <systemc>
#include "caba_base_module.h"
#include "generic_fifo.h"
#include "generic_tlb.h"
#include "mapping_table.h"
#include "address_decoding_table.h"
#include "static_assert.h"
#include "vci_initiator.h"
#include "vci_target.h"

namespace soclib {
namespace caba {

using namespace soclib::common;

////////////////////////////////////////////////////////////////////////////////
template<typename vci_param>
class VciNocMmu
////////////////////////////////////////////////////////////////////////////////
    : public soclib::caba::BaseModule
{
    enum vci_address_shift_and_mask
    {
        PTE_V_MASK  = 0x80000000,
        PTE_T_MASK  = 0x40000000,
        PTE_W_MASK  = 0x04000000,
        IX1_SHIFT   = 21,
        IX1_MASK    = 0x000007FF,
        IX2_SHIFT   = 12,
        IX2_MASK    = 0x000001FF,
        OFFSET_MASK = 0x00000FFF,
        PTBA_SHIFT  = 12,
        PTPR_SHIFT  = 13,
    };

    enum cmd_fsm_state
    {
        CMD_IDLE,
        CMD_SEND,
        CMD_FAIL_WAIT_EOP,
        CMD_FAIL_WAIT_RSP,
        CMD_MISS_CHECK,
        CMD_MISS_READ_PTD,
        CMD_MISS_WAIT_PTD,
        CMD_MISS_READ_PTE,
        CMD_MISS_WAIT_PTE,
        CMD_MISS_TLB_UPDT,
    };

    enum rsp_fsm_state
    {
        RSP_IDLE,
        RSP_DATA,
        RSP_PTE_FLAGS,
        RSP_PTE_PPN,
        RSP_PTD,
        RSP_FAILURE,
    };

    enum config_fsm_state
    {
        CONFIG_IDLE,
        CONFIG_PTPR_WRITE,
        CONFIG_MODE_WRITE,
        CONFIG_PTPR_READ,
        CONFIG_MODE_READ,
        CONFIG_BVAR_READ,
        CONFIG_XCODE_READ,
        CONFIG_INVAL_PTE,
        CONFIG_ERROR_WAIT,
        CONFIG_ERROR_RSP,
	};

    enum nmu_mode
    {
        NMU_MODE_BLOCKED,
        NMU_MODE_FAILURE,
        NMU_MODE_IDENTITY,
        NMU_MODE_ACTIVATE,
	};

    enum nmu_pt_access_type
    {
        NMU_PT_ACCESS_PTD = 0x4,
        NMU_PT_ACCESS_PTE = 0x5,
    };

    enum nmu_error_type
    {
        NO_ERROR,
        WRITE_ACCESS_VIOLATION,
        PT1_ENTRY_UNMAPPED,
        PT2_ENTRY_UNMAPPED,
        READ_PT1_BUS_ERROR,
        READ_PT2_BUS_ERROR,
        UNSUPPORTED_BIG_PAGE,
    };

public:

    // PORTS
    sc_in<bool>                                 p_clk;
    sc_in<bool>                                 p_resetn;
    sc_out<bool>                               *p_irq;          // one per vm
    soclib::caba::VciInitiator<vci_param>       p_vci_ini;      // to NoC
    soclib::caba::VciTarget<vci_param>          p_vci_tgt;      // from initiator
    soclib::caba::VciTarget<vci_param>          p_vci_config;   // from NoC

private:

    // structural constants
    const uint32_t                m_vms;            // number of supported VMs
    const uint32_t                m_words;          // word number in prefetch buffer
    const soclib::common::Segment m_segment;        // configutration segment
    const uint32_t                m_srcid;          // same as the initiator itself
    const uint32_t				  m_ways;           // number of TLB sets
    const uint32_t				  m_sets;           // number of TLB ways
    const uint32_t                m_debug_start;    // detailed debug start cycle
    const bool                    m_debug_ok;       // detailed debug activated

    // addressable registers (one set per vm)
    sc_signal<uint32_t>          *r_ptpr;           // page table pointer ( >> 13 )
    sc_signal<uint32_t>          *r_mode;           // NOC-MMU mode
    sc_signal<uint32_t>          *r_bvar;           // bad virtual address register
    sc_signal<uint32_t>          *r_xcode;          // error type

    // TLBs (one TLB per vm)
    GenericTlb<uint32_t>         *r_tlb[8];

    // CMD FSM registers
    sc_signal<int>                r_cmd_fsm;	    // state register
    sc_signal<uint32_t>           r_cmd_paddr;      // physical address
    sc_signal<uint32_t>           r_cmd_srcid;      // save srcid for error rsp
    sc_signal<uint32_t>           r_cmd_trdid;      // save trdid for error rsp;
    sc_signal<uint32_t>           r_cmd_pktid;      // save pktid for error rsp;
    sc_signal<size_t>             r_cmd_tlb_way;    // selected way for TLB update
    sc_signal<size_t>             r_cmd_tlb_set;    // selected set for TLB update
    sc_signal<bool>               r_cmd_bypass;     // PTD read bypass when true
    sc_signal<uint32_t>           r_cmd_ptba;       // PTBA value (from PTD bypass)

    // RSP FSM registers
    sc_signal<int>                r_rsp_fsm;        // state register
    sc_signal<uint32_t>         **r_rsp_buf_ppn;    // prefetch : pte_ppn[m_words/2]
    sc_signal<uint32_t>         **r_rsp_buf_flags;  // prefetch : pte_flags[m_words/2]
    sc_signal<uint32_t>          *r_rsp_buf_tag;    // prefetch tag: VPN MSB bits
    sc_signal<bool>              *r_rsp_buf_val;    // prefetch buffer valid
    sc_signal<uint32_t>           r_rsp_ptd;        // page table descriptor (PT1 entry)
    sc_signal<bool>               r_rsp_ptd_val;    // page table descriptor valid
    sc_signal<size_t>             r_rsp_pte_count;  // word counter for PTE2 burst read
    sc_signal<bool>               r_rsp_pt_error;   // error returned by a page table access

    // CONFIG FSM
    sc_signal<int>                r_config_fsm;
    sc_signal<uint32_t>           r_config_vm;      // selected virtual machine (pktid)
    sc_signal<uint32_t>           r_config_wdata;   // virtual address to be invalidated
    sc_signal<uint32_t>           r_config_srcid;   // save for response
    sc_signal<uint32_t>           r_config_trdid;   // save for response
    sc_signal<uint32_t>           r_config_pktid;   // save for response

    // CMD FIFOs
    GenericFifo<uint32_t>         r_cmd_fifo_address;
    GenericFifo<uint32_t>         r_cmd_fifo_srcid;
    GenericFifo<uint32_t>         r_cmd_fifo_trdid;
    GenericFifo<uint32_t>         r_cmd_fifo_pktid;
    GenericFifo<uint32_t>         r_cmd_fifo_be;
    GenericFifo<uint32_t>         r_cmd_fifo_cmd;
    GenericFifo<uint32_t>         r_cmd_fifo_wdata;
    GenericFifo<uint32_t>         r_cmd_fifo_plen;
    GenericFifo<bool>             r_cmd_fifo_contig;
    GenericFifo<bool>             r_cmd_fifo_cons;
    GenericFifo<bool>             r_cmd_fifo_wrap;
    GenericFifo<bool>             r_cmd_fifo_eop;

    // RSP FIFOs
    GenericFifo<uint32_t>         r_rsp_fifo_rdata;
    GenericFifo<uint32_t>         r_rsp_fifo_rsrcid;
    GenericFifo<uint32_t>         r_rsp_fifo_rtrdid;
    GenericFifo<uint32_t>         r_rsp_fifo_rpktid;
    GenericFifo<uint32_t>         r_rsp_fifo_rerror;
    GenericFifo<bool>             r_rsp_fifo_reop;

    // Communication SET/RESET Flip-flops
    sc_signal<bool>               r_config2cmd_req;  // PTE inval requested
    sc_signal<bool>               r_cmd2rsp_req;     // error RSP requested
    sc_signal<bool>               r_rsp2cmd_error;   // VCI RSP error received


    // Activity counters
    uint32_t                      m_cpt_total_cycles; // total number of cycles
    uint32_t                      m_cpt_tlb_read[8];  // number of tlb read
    uint32_t                      m_cpt_tlb_miss[8];  // number of tlb miss
    uint32_t                      m_cost_tlb_miss[8]; // number of blocked cycles

protected:
    SC_HAS_PROCESS(VciNocMmu);

public:
    VciNocMmu(
        sc_module_name                      name,
        const soclib::common::MappingTable  &mt,
        const soclib::common::IntTab        tgtid,
        const soclib::common::IntTab        srcid,
        const uint32_t                      vms,
        const uint32_t                      dcache_words,
        const uint32_t                      tlb_ways,
        const uint32_t                      tlb_sets,
        const uint32_t                      debug_start,
        const bool                          debug_ok );

    ~VciNocMmu();

    void print_stats( uint32_t vm );
    void clear_stats( uint32_t vm );
    void print_trace( size_t mode = 0 );


private:
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_VCI_CC_VCACHE_WRAPPER_V4_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
