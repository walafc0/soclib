/* -*- c++ -*-C
 * File : vci_vcache_wrapper.cpp
 * Copyright (c) UPMC, Lip6, SoC
 * Author : Alain GREINER, Yang Gao
 * Date : 24/03/2012
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

#include "arithmetics.h"
#include "../include/vci_vcache_wrapper.h"

#define INSTRUMENTATION     1
#define DEBUG_DCACHE        1
#define DEBUG_ICACHE        1

namespace soclib { 
namespace caba {

namespace {
const char *icache_fsm_state_str[] = {
        "ICACHE_IDLE",
     
        "ICACHE_XTN_TLB_FLUSH", 
        "ICACHE_XTN_CACHE_FLUSH", 
        "ICACHE_XTN_TLB_INVAL",  
        "ICACHE_XTN_CACHE_INVAL_VA",
        "ICACHE_XTN_CACHE_INVAL_PA",
        "ICACHE_XTN_CACHE_INVAL_GO",

        "ICACHE_TLB_WAIT",

        "ICACHE_MISS_VICTIM",
        "ICACHE_MISS_INVAL",
        "ICACHE_MISS_WAIT",
        "ICACHE_MISS_UPDT",  

        "ICACHE_UNC_WAIT",  
    };
const char *dcache_fsm_state_str[] = {
        "DCACHE_IDLE",       

        "DCACHE_TLB_MISS",
        "DCACHE_TLB_PTE1_GET",
        "DCACHE_TLB_PTE1_SELECT",  
        "DCACHE_TLB_PTE1_UPDT", 
        "DCACHE_TLB_PTE2_GET", 
        "DCACHE_TLB_PTE2_SELECT",
        "DCACHE_TLB_PTE2_UPDT",   
        "DCACHE_TLB_LR_WAIT",
        "DCACHE_TLB_RETURN",

        "DCACHE_XTN_SWITCH", 
        "DCACHE_XTN_SYNC", 
        "DCACHE_XTN_IC_INVAL_VA",
        "DCACHE_XTN_IC_FLUSH", 
        "DCACHE_XTN_IC_INVAL_PA",
        "DCACHE_XTN_IT_INVAL",
        "DCACHE_XTN_DC_FLUSH", 
        "DCACHE_XTN_DC_INVAL_VA",
        "DCACHE_XTN_DC_INVAL_PA",
        "DCACHE_XTN_DC_INVAL_END",
        "DCACHE_XTN_DC_INVAL_GO",
        "DCACHE_XTN_DT_INVAL",

        "DCACHE_DIRTY_PTE_GET",
        "DCACHE_DIRTY_SC_WAIT",  

        "DCACHE_MISS_VICTIM",
        "DCACHE_MISS_INVAL",
        "DCACHE_MISS_WAIT",  
        "DCACHE_MISS_UPDT",  

        "DCACHE_UNC_WAIT",   
        "DCACHE_SC_WAIT",   
    };
const char *cmd_fsm_state_str[] = {
        "CMD_IDLE",           
        "CMD_INS_MISS",     
        "CMD_INS_UNC",     
        "CMD_DATA_MISS",    
        "CMD_DATA_UNC",     
        "CMD_DATA_WRITE", 
        "CMD_DATA_SC", 
    };
const char *rsp_fsm_state_str[] = {
        "RSP_IDLE",                  
        "RSP_INS_MISS",   
        "RSP_INS_UNC",           
        "RSP_DATA_MISS",             
        "RSP_DATA_UNC",              
        "RSP_DATA_WRITE",     
    };
}

#define tmpl(...)  template<typename vci_param, typename iss_t> __VA_ARGS__ VciVcacheWrapper<vci_param, iss_t>

using soclib::common::uint32_log2;

/////////////////////////////////
tmpl(/**/)::VciVcacheWrapper(
    sc_module_name             name,
    int                 procid,
    const soclib::common::MappingTable     &mtp,
    const soclib::common::IntTab     &srcid,
    size_t                 tlb_ways,
    size_t                 tlb_sets,
    size_t                 icache_ways,
    size_t                 icache_sets,
    size_t                 icache_words,
    size_t                 dcache_ways,
    size_t                 dcache_sets,
    size_t                 dcache_words,
    size_t                 wbuf_nlines, 
    size_t                 wbuf_nwords, 
    uint32_t                max_frozen_cycles,
    uint32_t                debug_start_cycle,
    bool                debug_ok)
    : soclib::caba::BaseModule(name),

      p_clk("p_clk"),
      p_resetn("p_resetn"),
      p_vci("p_vci"),

      m_cacheability_table(mtp.getCacheabilityTable()),
      m_srcid(mtp.indexForId(srcid)),

      m_tlb_ways(tlb_ways),
      m_tlb_sets(tlb_sets),

      m_icache_ways(icache_ways),
      m_icache_sets(icache_sets),
      m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2)),
      m_icache_words(icache_words),

      m_dcache_ways(dcache_ways),
      m_dcache_sets(dcache_sets),
      m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2)),
      m_dcache_words(dcache_words),

      m_procid(procid),
      m_paddr_nbits(vci_param::N),

      m_max_frozen_cycles(max_frozen_cycles),
      m_debug_start_cycle(debug_start_cycle),
      m_debug_ok(debug_ok),

      r_mmu_ptpr("r_mmu_ptpr"),
      r_mmu_mode("r_mmu_mode"),
      r_mmu_word_lo("r_mmu_word_lo"),
      r_mmu_word_hi("r_mmu_word_hi"),
      r_mmu_ibvar("r_mmu_ibvar"),
      r_mmu_dbvar("r_mmu_dbvar"),
      r_mmu_ietr("r_mmu_ietr"),
      r_mmu_detr("r_mmu_detr"),

      r_icache_fsm("r_icache_fsm"),
      r_icache_vci_paddr("r_icache_vci_paddr"),
      r_icache_vaddr_save("r_icache_vaddr_save"),

      r_icache_miss_way("r_icache_miss_way"),
      r_icache_miss_set("r_icache_miss_set"),
      r_icache_miss_word("r_icache_miss_word"),

      r_icache_flush_count("r_icache_flush_count"),

      r_icache_miss_req("r_icache_miss_req"),
      r_icache_unc_req("r_icache_unc_req"),

      r_icache_tlb_miss_req("r_icache_tlb_read_req"),
      r_icache_tlb_rsp_error("r_icache_tlb_rsp_error"),

      r_dcache_fsm("r_dcache_fsm"),

      r_dcache_p0_valid("r_dcache_p0_valid"),
      r_dcache_p0_vaddr("r_dcache_p0_vaddr"),
      r_dcache_p0_wdata("r_dcache_p0_wdata"),
      r_dcache_p0_be("r_dcache_p0_be"),
      r_dcache_p0_paddr("r_dcache_p0_paddr"),
      r_dcache_p0_cacheable("r_dcache_p0_cacheable"), 

      r_dcache_p1_valid("r_dcache_p1_valid"),
      r_dcache_p1_wdata("r_dcache_p1_wdata"),
      r_dcache_p1_be("r_dcache_p1_be"),
      r_dcache_p1_paddr("r_dcache_p1_paddr"),
      r_dcache_p1_cache_way("r_dcache_p1_cache_way"),
      r_dcache_p1_cache_set("r_dcache_p1_cache_set"),
      r_dcache_p1_cache_word("r_dcache_p1_word_save"),

      r_dcache_dirty_paddr("r_dcache_dirty_paddr"),
      r_dcache_dirty_way("r_dcache_dirty_way"),
      r_dcache_dirty_set("r_dcache_dirty_set"),

      r_dcache_vci_paddr("r_dcache_vci_paddr"),
      r_dcache_vci_miss_req("r_dcache_vci_miss_req"),
      r_dcache_vci_unc_req("r_dcache_vci_unc_req"),
      r_dcache_vci_unc_be("r_dcache_vci_unc_be"),
      r_dcache_vci_sc_req("r_dcache_vci_sc_req"),
      r_dcache_vci_sc_old("r_dcache_vci_sc_old"),
      r_dcache_vci_sc_new("r_dcache_vci_sc_new"),

      r_dcache_xtn_way("r_dcache_xtn_way"),
      r_dcache_xtn_set("r_dcache_xtn_set"),

      r_dcache_pending_unc_write("r_dcache_pending_unc_write"),

      r_dcache_miss_type("r_dcache_miss_type"),
      r_dcache_miss_word("r_dcache_miss_word"),
      r_dcache_miss_way("r_dcache_miss_way"),
      r_dcache_miss_set("r_dcache_miss_set"),

      r_dcache_sc_word("r_dcache_sc_word"),
      r_dcache_sc_way("r_dcache_sc_way"),
      r_dcache_sc_set("r_dcache_sc_set"),
      r_dcache_sc_hit("r_dcache_sc_hit"),

      r_dcache_flush_count("r_dcache_flush_count"),

      r_dcache_tlb_vaddr("r_dcache_tlb_vaddr"),
      r_dcache_tlb_ins("r_dcache_tlb_ins"),
      r_dcache_tlb_pte_flags("r_dcache_tlb_pte_flags"),
      r_dcache_tlb_pte_ppn("r_dcache_tlb_pte_ppn"),
      r_dcache_tlb_cache_way("r_dcache_tlb_cache_way"),
      r_dcache_tlb_cache_set("r_dcache_tlb_cache_set"),
      r_dcache_tlb_cache_word("r_dcache_tlb_cache_word"),
      r_dcache_tlb_way("r_dcache_tlb_way"),
      r_dcache_tlb_set("r_dcache_tlb_set"),

      r_dcache_ll_valid("r_dcache_ll_valid"),
      r_dcache_ll_data("r_dcache_ll_data"),
      r_dcache_ll_vaddr("r_dcache_ll_vaddr"),

      r_dcache_xtn_req("r_dcache_xtn_req"),
      r_dcache_xtn_opcode("r_dcache_xtn_opcode"),

      r_vci_cmd_fsm("r_vci_cmd_fsm"),
      r_vci_cmd_min("r_vci_cmd_min"),
      r_vci_cmd_max("r_vci_cmd_max"),
      r_vci_cmd_cpt("r_vci_cmd_cpt"),
      r_vci_cmd_imiss_prio("r_vci_cmd_imiss_prio"),

      r_vci_rsp_fsm("r_vci_rsp_fsm"),
      r_vci_rsp_cpt("r_vci_rsp_cpt"),
      r_vci_rsp_ins_error("r_vci_rsp_ins_error"),
      r_vci_rsp_data_error("r_vci_rsp_data_error"),
      r_vci_rsp_fifo_icache("r_vci_rsp_fifo_icache", 2),    // 2 words depth
      r_vci_rsp_fifo_dcache("r_vci_rsp_fifo_dcache", 2),    // 2 words depth

      r_iss(this->name(), procid),
      r_wbuf("wbuf", wbuf_nwords, wbuf_nlines, dcache_words ),
      r_icache("icache", icache_ways, icache_sets, icache_words),
      r_dcache("dcache", dcache_ways, dcache_sets, dcache_words),
      r_itlb("itlb", procid, tlb_ways,tlb_sets,vci_param::N),
      r_dtlb("dtlb", procid, tlb_ways,tlb_sets,vci_param::N)
{
    assert( ((icache_words*vci_param::B) < (1<<vci_param::K)) and
             "Need more PLEN bits.");

    assert( (vci_param::T > 2) and ((1<<(vci_param::T-1)) >= (wbuf_nlines)) and
             "Need more TRDID bits.");

    assert( (icache_words == dcache_words) and
             "icache_words and dcache_words parameters must be equal");

    r_mmu_params = (uint32_log2(m_tlb_ways)    << 29) | (uint32_log2(m_tlb_sets)    << 25) |
                   (uint32_log2(m_dcache_ways) << 22) | (uint32_log2(m_dcache_sets) << 18) |
                   (uint32_log2(m_tlb_ways)    << 15) | (uint32_log2(m_tlb_sets)    << 11) |
                   (uint32_log2(m_icache_ways) << 8)  | (uint32_log2(m_icache_sets) << 4)  |
                   (uint32_log2(m_icache_words << 2));

    r_mmu_release = (uint32_t)(1 << 16) | 0x1;

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    typename iss_t::CacheInfo cache_info;
    cache_info.has_mmu = true;
    cache_info.icache_line_size = icache_words*sizeof(uint32_t);
    cache_info.icache_assoc = icache_ways;
    cache_info.icache_n_lines = icache_sets;
    cache_info.dcache_line_size = dcache_words*sizeof(uint32_t);
    cache_info.dcache_assoc = dcache_ways;
    cache_info.dcache_n_lines = dcache_sets;
    r_iss.setCacheInfo(cache_info);
}

/////////////////////////////////////
tmpl(/**/)::~VciVcacheWrapper()
/////////////////////////////////////
{
}

//////////////////////////////////
tmpl(void)::file_stats(FILE* file)
//////////////////////////////////
{
    float imiss_rate = (float) r_cpt_icache_miss / (float) (r_cpt_exec_ins);
    float dmiss_rate = (float) r_cpt_dcache_miss / (float) (r_cpt_read - r_cpt_read_uncacheable);
    float cpi        = (float) r_cpt_total_cycles / (float) (r_cpt_exec_ins);

    fprintf(file, "%8d %8d %8d %8d %8d    %f    %f    %f \n",
            r_cpt_total_cycles,
            r_cpt_exec_ins,
            r_cpt_icache_miss,
            r_cpt_read - r_cpt_read_uncacheable,
            r_cpt_dcache_miss,
            imiss_rate,
            dmiss_rate,
            cpi);
}


////////////////////////
tmpl(void)::print_cpi()
////////////////////////
{
    std::cout << name() << " CPI = " 
        << (float)r_cpt_total_cycles/(r_cpt_total_cycles - r_cpt_frz_cycles) << std::endl ;
}

////////////////////////////////////
tmpl(void)::print_trace(size_t mode)
////////////////////////////////////
{
    // b0 : write buffer trace
    // b1 : write buffer verbose
    // b2 : dcache trace
    // b3 : icache trace
    // b4 : dtlb trace
    // b5 : itlb trace

    std::cout << name() << std::endl;

    std::cout << "  " << m_ireq << std::endl;
    std::cout << "  " << m_irsp << std::endl;
    std::cout << "  " << m_dreq << std::endl;
    std::cout << "  " << m_drsp << std::endl;

    std::cout << "  " << icache_fsm_state_str[r_icache_fsm.read()]
              << " | " << dcache_fsm_state_str[r_dcache_fsm.read()]
              << " | " << cmd_fsm_state_str[r_vci_cmd_fsm.read()]
              << " | " << rsp_fsm_state_str[r_vci_rsp_fsm.read()];
    if (r_dcache_p0_valid.read() ) std::cout << " | P1_WRITE";
    if (r_dcache_p1_valid.read() ) std::cout << " | P2_WRITE";
    std::cout << std::endl;

    if ( r_dcache_ll_valid.read() ) std::cout << "  Registered LL : address = " 
                                              << r_dcache_ll_vaddr.read()
                                              << " / data = "
                                              << r_dcache_ll_data.read() << std::endl;
    if(mode & 0x01)
    {
        r_wbuf.printTrace((mode>>1)&1);
    }
    if(mode & 0x04)
    {
        std::cout << "  Data Cache" << std::endl;
        r_dcache.printTrace();
    }
    if(mode & 0x08)
    {
        std::cout << "  Instruction Cache" << std::endl;
        r_icache.printTrace();
    }
    if(mode & 0x10)
    {
        std::cout << "  Data TLB" << std::endl;
        r_dtlb.printTrace();
    }
    if(mode & 0x20)
    {
        std::cout << "  Instruction TLB" << std::endl;
        r_itlb.printTrace();
    }
}

//////////////////////////////////////////
tmpl(void)::cache_monitor( paddr_t addr )
//////////////////////////////////////////
{ 
    size_t    cache_way;
    size_t    cache_set;
    size_t    cache_word;
    uint32_t    cache_rdata;

    bool    dcache_hit = r_dcache.read_neutral( addr,
                                           &cache_rdata,
                                           &cache_way,
                                           &cache_set,
                                           &cache_word );
    bool    icache_hit = r_icache.read_neutral( addr,
                                           &cache_rdata,
                                           &cache_way,
                                           &cache_set,
                                           &cache_word );
    if ( dcache_hit != r_debug_dcache_previous_hit )
    {
        std::cout << "PROC " << name() 
                  << " dcache change at cycle " << std::dec << r_cpt_total_cycles
                  << " for adresse " << std::hex << addr
                  << " / HIT = " << dcache_hit << std::endl;
    r_debug_dcache_previous_hit = dcache_hit;
    }
    if ( icache_hit != r_debug_dcache_previous_hit )
    {
        std::cout << "PROC " << name() 
                  << " icache change at cycle " << std::dec << r_cpt_total_cycles
                  << " for adresse " << std::hex << addr
                  << " / HIT = " << icache_hit << std::endl;
    r_debug_icache_previous_hit = icache_hit;
    }
}

////////////////////////
tmpl(void)::print_stats()
////////////////////////
{
    std::cout << name() << std::endl
    << "- CPI               = " << (float)r_cpt_total_cycles/r_cpt_ins << std::endl 
    << "- READ        RATE  = " << (float)r_cpt_read/r_cpt_ins << std::endl 
    << "- WRITE       RATE  = " << (float)r_cpt_write/r_cpt_ins << std::endl
    << "- WRITE       COST  = " << (float)r_cost_wbuf_full_frz/r_cpt_write << std::endl        
    << "- SC          RATE  = " << (float)r_cpt_sc/r_cpt_ins << std::endl
    << "- SC          COST  = " << (float)r_cost_sc_frz/r_cpt_sc << std::endl        
    << "- LL          RATE  = " << (float)r_cpt_ll/r_cpt_ins << std::endl

    << "- ICACHE MISS RATE  = " << (float)r_cpt_icache_miss/(r_cpt_ins - r_cpt_ins_uncacheable) << std::endl
    << "- ICACHE MISS COST  = " << (float)r_cost_icache_miss_frz/r_cpt_icache_miss << std::endl     
    << "- DCACHE MISS RATE  = " << (float)r_cpt_dcache_miss/(r_cpt_read - r_cpt_read_uncacheable) << std::endl  
    << "- DCACHE MISS COST  = " << (float)r_cost_dcache_miss_frz/r_cpt_dcache_miss << std::endl 

    << "- CACHED WRITE RATE = " << (float)r_cpt_write_cached/r_cpt_write << std::endl 

    << "- ITLB MISS RATE    = " << (float)r_cpt_itlb_miss/r_cpt_itlb_read << std::endl
    << "- DTLB MISS RATE    = " << (float)r_cpt_dtlb_miss/r_cpt_dtlb_read << std::endl
    << "- ITLB MISS COST    = " << (float)r_cost_itlb_miss_frz/r_cpt_itlb_miss << std::endl
    << "- DTLB MISS COST    = " << (float)r_cost_dtlb_miss_frz/r_cpt_dtlb_miss << std::endl   
    << "- ITLB BYPASS RATE  = " << (float)r_cpt_itlb_miss_bypass/r_cpt_itlb_miss << std::endl
    << "- DTLB BYPASS RATE  = " << (float)r_cpt_dtlb_miss_bypass/r_cpt_dtlb_miss << std::endl

    << "- DIRTY UPDT   RATE = " << (float)r_cpt_dirty_bit_updt/r_cpt_ins << std::endl
    << "- DIRTY UPDT   COST = " << (float)r_cost_dirty_bit_updt_frz/r_cpt_dirty_bit_updt << std::endl

    << "- IMISS TRANSACTION = " << r_cpt_imiss_transaction << std::endl
    << "- DMISS TRANSACTION = " << r_cpt_dmiss_transaction << std::endl
    << "- IUNC  TRANSACTION = " << r_cpt_iunc_transaction << std::endl
    << "- DUNC  TRANSACTION = " << r_cpt_dunc_transaction << std::endl
    << "- SC    TRANSACTION = " << r_cpt_sc_transaction << std::endl
    << "- WRITE TRANSACTION = " << r_cpt_write_transaction << std::endl
    << "- WRITE LENGTH      = " << (float)r_length_write_transaction/r_cpt_write_transaction << std::endl;
}

/////////////////////////
tmpl(void)::transition()
/////////////////////////
{
    if ( not p_resetn.read() ) 
    {
        r_iss.reset();
        r_wbuf.reset();
        r_icache.reset();
        r_dcache.reset();
        r_itlb.reset();    
        r_dtlb.reset();    

        r_dcache_fsm      = DCACHE_IDLE;
        r_icache_fsm      = ICACHE_IDLE;
        r_vci_cmd_fsm     = CMD_IDLE;
        r_vci_rsp_fsm     = RSP_IDLE;

        // Response FIFOs 
        r_vci_rsp_fifo_icache.init();
        r_vci_rsp_fifo_dcache.init();

        // ICACHE & DCACHE activated
        r_mmu_mode = 0x3;

        // No request from ICACHE FSM to CMD FSM
        r_icache_miss_req          = false;
        r_icache_unc_req           = false;

        // No request from ICACHE_FSM to DCACHE FSM
        r_icache_tlb_miss_req      = false;      
 
        // No pending write in pipeline
        r_dcache_p0_valid          = false;
        r_dcache_p1_valid          = false;

        // No request from DCACHE_FSM to CMD_FSM
        r_dcache_vci_miss_req      = false;
        r_dcache_vci_unc_req       = false;
        r_dcache_vci_sc_req        = false;

        // No uncacheable write pending
        r_dcache_pending_unc_write = false;

        // No LL reservation
        r_dcache_ll_valid          = false;

        // No processor XTN request pending
        r_dcache_xtn_req           = false;

        // No signalisation  of errors
        r_vci_rsp_ins_error        = false;
        r_vci_rsp_data_error       = false;

        // Debug registers
        r_debug_icache_previous_hit = false;
        r_debug_dcache_previous_hit = false;
        r_debug_active              = false;
        r_cpt_stop_simulation       = 0;

        // Instrumentation counters 
        r_cpt_dcache_read        = 0;
        r_cpt_dcache_write       = 0;
        r_cpt_icache_read        = 0;
        r_cpt_icache_write       = 0;

        r_cpt_dtlb_read        = 0;
        r_cpt_dtlb_write       = 0;
        r_cpt_itlb_read        = 0;
        r_cpt_itlb_write       = 0;

        r_cpt_wbuf_read        = 0;
        r_cpt_wbuf_write       = 0;

        r_pc_previous             = 1;
        r_cpt_exec_ins            = 0;
        r_cpt_frz_cycles          = 0;
        r_cpt_total_cycles        = 0;

        r_cpt_ins                 = 0;
        r_cpt_ins_uncacheable     = 0;
        r_cpt_icache_miss         = 0;
        r_cpt_icache_spec_miss    = 0;
        r_cpt_itlb_miss           = 0;
        r_cpt_itlb_miss_bypass    = 0;

        r_cpt_read          = 0;
        r_cpt_read_uncacheable      = 0;
        r_cpt_write               = 0;
        r_cpt_write_uncacheable   = 0;
        r_cpt_write_cached        = 0;
        r_cpt_sc                  = 0;
        r_cpt_ll                  = 0;
        r_cpt_dcache_miss         = 0;
        r_cpt_dcache_spec_miss    = 0;
        r_cpt_dirty_bit_updt      = 0;
        r_cpt_access_bit_updt     = 0;
        r_cpt_dtlb_miss           = 0;
        r_cpt_dtlb_miss_bypass    = 0;

        r_cost_icache_miss_frz    = 0;
        r_cost_dcache_miss_frz    = 0;
        r_cost_iunc_frz           = 0;
        r_cost_dunc_frz           = 0;
        r_cost_itlb_miss_frz      = 0;
        r_cost_dtlb_miss_frz      = 0;
        r_cost_dirty_bit_updt_frz = 0;
        r_cost_access_bit_updt_frz= 0;
        r_cost_wbuf_full_frz      = 0;
        r_cost_sc_frz             = 0;
        r_cost_unc_write_frz      = 0;

        r_cpt_imiss_transaction   = 0;
        r_cpt_dmiss_transaction   = 0;
        r_cpt_iunc_transaction    = 0;
        r_cpt_dunc_transaction    = 0;
        r_cpt_sc_transaction      = 0;
        r_cpt_write_transaction   = 0;
        r_length_write_transaction = 0;    

        for (uint32_t i = 0; i < 32; ++i) r_cpt_fsm_icache[i] = 0;
        for (uint32_t i = 0; i < 32; ++i) r_cpt_fsm_dcache[i] = 0;
        for (uint32_t i = 0; i < 32; ++i) r_cpt_fsm_cmd[i] = 0;
        for (uint32_t i = 0; i < 32; ++i) r_cpt_fsm_rsp[i] = 0;

        return;
    }

    // Response FIFOs default values
    bool       vci_rsp_fifo_icache_get       = false;
    bool       vci_rsp_fifo_icache_put       = false;
    uint32_t   vci_rsp_fifo_icache_data      = 0;

    bool       vci_rsp_fifo_dcache_get       = false;
    bool       vci_rsp_fifo_dcache_put       = false;
    uint32_t   vci_rsp_fifo_dcache_data      = 0;

#if INSTRUMENTATION
    r_cpt_fsm_dcache  [r_dcache_fsm.read() ] ++;
    r_cpt_fsm_icache  [r_icache_fsm.read() ] ++;
    r_cpt_fsm_cmd     [r_vci_cmd_fsm.read()] ++;
    r_cpt_fsm_rsp     [r_vci_rsp_fsm.read()] ++;
#endif

    r_cpt_total_cycles++;

    r_debug_active = (r_cpt_total_cycles > m_debug_start_cycle) and m_debug_ok;

    /////////////////////////////////////////////////////////////////////
    // Get data and instruction requests from processor
    ///////////////////////////////////////////////////////////////////////

    r_iss.getRequests(m_ireq, m_dreq);

    ////////////////////////////////////////////////////////////////////////////////////
    //      ICACHE_FSM
    //
    // Both the Cacheability Table, and the MMU cacheable bit are used to define
    // the cacheability, depending on the MMU mode.
    // 
    // There is 8 causes to exit the IDLE state:
    // Five configurations corresponding to XTN processor requests (from DCACHE FSM) :
    // - Flush ITLB                     => ICACHE_XTN_TLB_FLUSH 
    // - Flush ICACHE                     => ICACHE_XTN_CACHE_FLUSH 
    // - Invalidate ITLB entry                 => ICACHE_XTN_TLB_INVAL
    // - Invalidate ICACHE line using virtual address    => ICACHE_XTN_CACHE_INVAL_VA
    // - Invalidate ICACHE line using physical address    => ICACHE_XTN_CACHE_INVAL_PA
    // Three configurations corresponding to instruction processor requests :
    // - tlb miss                     => ICACHE_TLB_WAIT 
    // - cacheable read miss                 => ICACHE_MISS_VICTIM
    // - uncacheable read miss                 => ICACHE_UNC_REQ 
    // 
    // In case of cache miss, the ICACHE FSM request a VCI transaction to CMD FSM
    // using the r_icache_tlb_miss_req flip-flop, that reset this flip-flop when the
    // transaction starts. Then it goes to ICACHE_MISS VICTIM state to select a slot. 
    // It goes next to the ICACHE_MISS_WAIT state waiting a response from RSP FSM. 
    // The availability of the missing cache line is signaled by the response fifo,
    // and the cache update is done (one word per cycle) in the ICACHE_MISS_UPDT state.
    // 
    // In case of uncacheable address, the ICACHE FSM request an uncached VCI transaction
    // to CMD FSM using the r_icache_unc_req flip-flop, that reset this flip-flop
    // when the transaction starts. The ICACHE FSM goes to ICACHE_UNC_WAIT to wait
    // the response from the RSP FSM, through the response fifo. The missing instruction
    // is directly returned to processor in this state.
    // 
    // In case of tlb miss, the ICACHE FSM request to the DCACHE FSM to update the tlb
    // using the r_icache_tlb_miss_req flip-flop and the r_icache_tlb_miss_vaddr register,
    // and goes to the ICACHE_TLB_WAIT state.
    // The tlb update is entirely done by the DCACHE FSM (who becomes the owner of dtlb until
    // the update is completed, and reset r_icache_tlb_miss_req to signal the completion.
    //
    // The DCACHE FSM signals XTN processor requests to ICACHE_FSM
    // using the r_dcache_xtn_req flip-flop. 
    // The request opcode and the address to be invalidated are transmitted
    // in the r_dcache_xtn_opcode and r_dcache_p0_wdata registers respectively.
    // The r_dcache_xtn_req flip-flop is reset by the ICACHE_FSM when the operation 
    // is completed.
    //
    // The r_vci_rsp_ins_error flip-flop is set by the RSP FSM in case of bus error
    // in a cache miss or uncacheable read VCI transaction. Nothing is written 
    // in the response fifo. This flip-flop is reset by the ICACHE-FSM.
    ////////////////////////////////////////////////////////////////////////////////////////

    // default value for m_irsp
    m_irsp.valid       = false;
    m_irsp.error       = false;
    m_irsp.instruction = 0;

    switch( r_icache_fsm.read() ) 
    {
    /////////////////
    case ICACHE_IDLE:    // In this state, we handle processor requests, and XTN requests 
                        // sent by DCACHE FSM. XTN requests are handled first.
                        // We access the itlb and dcache in parallel:
            // - with the virtual address for ITLB 
                        // - with a speculative physical address for ICACHE
                        //   (address computed during the previous cycle)
    {
        // Decoding processor XTN requests sent by DCACHE FSM  
        // These request are not executed in this IDLE state, because
        // they require access to icache or itlb (already accessed in this state)
        if ( r_dcache_xtn_req.read() )
        {
            if ( (int)r_dcache_xtn_opcode.read() == (int)iss_t::XTN_PTPR ) 
            {
                r_icache_fsm         = ICACHE_XTN_TLB_FLUSH;   
                break;
            }
            if ( (int)r_dcache_xtn_opcode.read() == (int)iss_t::XTN_ICACHE_FLUSH)
            {
                r_icache_flush_count = 0;
                r_icache_fsm         = ICACHE_XTN_CACHE_FLUSH;   
                break;
            }
            if ( (int)r_dcache_xtn_opcode.read() == (int)iss_t::XTN_ITLB_INVAL) 
            {
                r_icache_fsm         = ICACHE_XTN_TLB_INVAL;   
                break;
            }
            if ( (int)r_dcache_xtn_opcode.read() == (int)iss_t::XTN_ICACHE_INVAL) 
            {
                r_icache_fsm         = ICACHE_XTN_CACHE_INVAL_VA;   
                break;
            }
            if ( (int)r_dcache_xtn_opcode.read() == (int)iss_t::XTN_MMU_ICACHE_PA_INV) 
            {
        if (sizeof(paddr_t) <= 32) {
            assert(r_mmu_word_hi.read() == 0 &&
                "high bits should be 0 for 32bit paddr");
            r_icache_vci_paddr = (paddr_t)r_mmu_word_lo.read();
        } else {
            r_icache_vci_paddr =
                (paddr_t)r_mmu_word_hi.read() << 32 | 
                (paddr_t)r_mmu_word_lo.read();
        }
                r_icache_fsm         = ICACHE_XTN_CACHE_INVAL_PA;   
                break;
            }
        } // end if xtn_req

        // processor request
        if ( m_ireq.valid )
        {
            bool    cacheable;
            paddr_t    paddr;

            // We register processor request
            r_icache_vaddr_save = m_ireq.addr;

            // speculative icache access (if cache activated)
            // we use the speculative PPN computed during the previous cycle
            
            uint32_t     cache_inst = 0;
            bool    cache_hit  = false;

            if ( r_mmu_mode.read() & INS_CACHE_MASK )
            {
                paddr_t   spc_paddr = (r_icache_vci_paddr.read() & ~PAGE_K_MASK) |
                                      ((paddr_t)m_ireq.addr & PAGE_K_MASK);

#if INSTRUMENTATION
r_cpt_icache_read++;
#endif
                cache_hit = r_icache.read( spc_paddr,
                                           &cache_inst );
            }

            // systematic itlb access (if tlb activated)
            // we use the virtual address

            paddr_t    tlb_paddr;
            pte_info_t  tlb_flags; 
            size_t      tlb_way;  
            size_t      tlb_set;
            paddr_t     tlb_nline;
            bool    tlb_hit   = false;;  

            if ( r_mmu_mode.read() & INS_TLB_MASK )
            {

#if INSTRUMENTATION
r_cpt_itlb_read++;
#endif
                tlb_hit = r_itlb.translate( m_ireq.addr,
                                            &tlb_paddr,
                                            &tlb_flags,
                                            &tlb_nline,    // unused
                                            &tlb_way,    // unused
                                            &tlb_set );    // unused
            }

            // We compute cacheability, physical address and check access rights:
            // - If MMU activated : cacheability is defined by the C bit in the PTE,
            //   the physical address is obtained from the TLB, and the access rights are
            //   defined by the U and X bits in the PTE.
            // - If MMU not activated : cacheability is defined by the segment table,
            //   the physical address is equal to the virtual address (identity mapping)
            //   and there is no access rights checking

            if ( not (r_mmu_mode.read() & INS_TLB_MASK) )     // tlb not activated: 
            {
                // cacheability
                if ( not (r_mmu_mode.read() & INS_CACHE_MASK) ) cacheable = false;
                else     cacheable = m_cacheability_table[(uint64_t)m_ireq.addr];

                // physical address
                paddr = (paddr_t)m_ireq.addr;
            }
            else                        // itlb activated
            {
                if ( tlb_hit )    // tlb hit
                {  
                    // cacheability
                    if ( not (r_mmu_mode.read() & INS_CACHE_MASK) ) cacheable = false;
                    else  cacheable = tlb_flags.c;

                    // physical address 
                    paddr       = tlb_paddr;

                    // access rights checking 
                    if ( not tlb_flags.u && (m_ireq.mode == iss_t::MODE_USER) )
                    {
                        r_mmu_ietr        = MMU_READ_PRIVILEGE_VIOLATION;
                        r_mmu_ibvar       = m_ireq.addr;
                        m_irsp.valid        = true;
                        m_irsp.error        = true;
                        m_irsp.instruction  = 0;
                        break;
                    }
                    else if ( not tlb_flags.x )
                    {
                        r_mmu_ietr        = MMU_READ_EXEC_VIOLATION;
                        r_mmu_ibvar       = m_ireq.addr;
                        m_irsp.valid        = true;
                        m_irsp.error        = true;
                        m_irsp.instruction  = 0;
                        break;
                    }
                }
                // in case of TLB miss we send an itlb miss request to DCACHE FSM and break
                else
                {

#if INSTRUMENTATION
r_cpt_itlb_miss++;
#endif
                    r_icache_fsm          = ICACHE_TLB_WAIT;
                    r_icache_tlb_miss_req = true;
                    break;
                } 
            } // end if itlb activated

            // physical address registration (for next cycle)
            r_icache_vci_paddr   = paddr;

            // We enter this section only in case of TLB hit:
            // Finally, we get the instruction depending on cacheability,
            // we send the response to processor, and compute next state
            if ( cacheable )      // cacheable read
            {
                if ( (r_icache_vci_paddr.read() & ~PAGE_K_MASK) 
                      != (paddr & ~PAGE_K_MASK) )     // speculative access KO 
                {

#if INSTRUMENTATION
r_cpt_icache_spec_miss++;
#endif
                    // we return an invalid response and stay in IDLE state
                    // the cache access will cost one extra cycle.
                    break;
                }
                
                if ( not cache_hit )    // cache miss
                {

#if INSTRUMENTATION
r_cpt_icache_miss++;
#endif
                    r_icache_fsm      = ICACHE_MISS_VICTIM;
                    r_icache_miss_req = true;
                }
                else            // cache hit
                {
      
#if INSTRUMENTATION
r_cpt_ins++; 
#endif
                    m_irsp.valid       = true;
                    m_irsp.instruction = cache_inst;
                }
            }
            else                 // non cacheable read
            {
                r_icache_unc_req  = true;
                r_icache_fsm      = ICACHE_UNC_WAIT;
            }
        }    // end if m_ireq.valid
        break;
    }
    /////////////////////
    case ICACHE_TLB_WAIT:    // Waiting the itlb update by the DCACHE FSM after a tlb miss
                                // the itlb is udated by the DCACHE FSM, as well as the 
                                // r_mmu_ietr and r_mmu_ibvar registers in case of error.
                                // the itlb is not accessed by ICACHE FSM until DCACHE FSM
                                // reset the r_icache_tlb_miss_req flip-flop
    {
#if INSTRUMENTATION
r_cost_itlb_miss_frz++;
#endif
        // DCACHE FSM signals response by reseting the request flip-flop
        if ( not r_icache_tlb_miss_req.read() )
        {
            if ( r_icache_tlb_rsp_error.read() ) // error reported : tlb not updated
            {
                r_icache_tlb_rsp_error = false;
                m_irsp.error             = true;
                m_irsp.valid             = true;
                r_icache_fsm           = ICACHE_IDLE;
            }
            else                // tlb updated : return to IDLE state
            {
                r_icache_fsm  = ICACHE_IDLE;
            }
        }
        break;
    }
    //////////////////////////
    case ICACHE_XTN_TLB_FLUSH:  // invalidate in one cycle all non global TLB entries
    {   
        r_itlb.flush();   
        r_dcache_xtn_req     = false;
        r_icache_fsm         = ICACHE_IDLE;
        break;
    }
    ////////////////////////////
    case ICACHE_XTN_CACHE_FLUSH:  // Invalidate sequencially all cache lines using
                                  // the r_icache_flush_count register as a slot counter.
                                  // We loop in this state until all slots have been visited.
    {
        size_t    way = r_icache_flush_count.read()/m_icache_sets;
        size_t    set = r_icache_flush_count.read()%m_icache_sets;
        paddr_t     nline;    // unused

        r_icache.inval( way, 
                        set, 
                        &nline );

        r_icache_flush_count = r_icache_flush_count.read() + 1;
       
        if ( r_icache_flush_count.read() == (m_icache_sets*m_icache_ways - 1) )
        {
            r_dcache_xtn_req     = false;
            r_icache_fsm     = ICACHE_IDLE;
        }
        break;
    }
    //////////////////////////
    case ICACHE_XTN_TLB_INVAL:       // invalidate one TLB entry selected by the virtual address 
                        // stored in the r_dcache_p0_wdata register
    {
        r_itlb.inval(r_dcache_p0_wdata.read());
        r_dcache_xtn_req     = false;
        r_icache_fsm         = ICACHE_IDLE;
        break;
    }
    ///////////////////////////////
    case ICACHE_XTN_CACHE_INVAL_VA:  // Selective cache line invalidate with virtual address
                                     // requires 3 cycles (in case of hit on itlb and icache).
                     // In this state, we access TLB to translate virtual 
                                     // address stored in the r_dcache_p0_wdata register.
    {
        paddr_t     paddr;                     
        bool        hit;

        // read physical address in TLB when MMU activated
        if ( r_mmu_mode.read() & INS_TLB_MASK )     // itlb activated
        {

#if INSTRUMENTATION
r_cpt_itlb_read++;
#endif
            hit = r_itlb.translate(r_dcache_p0_wdata.read(), 
                                   &paddr); 
        } 
        else                         // itlb not activated
        {
            paddr     = (paddr_t)r_dcache_p0_wdata.read();
            hit     = true;
        }

        if ( hit )        // continue the selective inval process
        {
            r_icache_vci_paddr    = paddr;                
            r_icache_fsm          = ICACHE_XTN_CACHE_INVAL_PA;
        }
        else            // miss : send a request to DCACHE FSM
        {

#if INSTRUMENTATION
r_cpt_itlb_miss++;
#endif
            r_icache_tlb_miss_req = true;
            r_icache_fsm          = ICACHE_TLB_WAIT;
        }
        break;
    }
    ///////////////////////////////
    case ICACHE_XTN_CACHE_INVAL_PA:  // selective invalidate cache line with physical address 
                                     // require 2 cycles. In this state, we read dcache,
                                     // with address stored in r_icache_vci_paddr register.
    {
        uint32_t    data;
        size_t        way;
        size_t        set;
        size_t        word;

#if INSTRUMENTATION
r_cpt_icache_read++;
#endif
        bool         hit = r_icache.read(r_icache_vci_paddr.read(),
                                            &data,
                                            &way,
                                            &set,
                                            &word);

        if ( hit )    // inval to be done
        {
                r_icache_miss_way = way;
                r_icache_miss_set = set;
                r_icache_fsm      = ICACHE_XTN_CACHE_INVAL_GO;
        }
        else        // miss : acknowlege the XTN request and return
        {
            r_dcache_xtn_req = false; 
            r_icache_fsm     = ICACHE_IDLE;
        }
        break;
    }
    ///////////////////////////////
    case ICACHE_XTN_CACHE_INVAL_GO:  // In this state, we invalidate the cache line 
    {
        paddr_t nline;    // unused

        bool hit = r_icache.inval( r_icache_miss_way.read(),
                                   r_icache_miss_set.read(),
                                   &nline );

        assert(hit && "XTN_ICACHE_INVAL way/set should still be in icache");
  
        r_dcache_xtn_req      = false; 
        r_icache_fsm          = ICACHE_IDLE;
        break;
    }

    ////////////////////////
    case ICACHE_MISS_VICTIM:      // Selects a victim line
    {
#if INSTRUMENTATION
r_cost_icache_miss_frz++;
#endif
        bool    valid;
        size_t  way;
        size_t  set;
        paddr_t victim;    

        valid = r_icache.victim_select(r_icache_vci_paddr.read(),
                                       &victim, 
                                       &way, 
                                       &set);
        r_icache_miss_way = way;
        r_icache_miss_set = set;

        if ( valid )  r_icache_fsm = ICACHE_MISS_INVAL;
        else          r_icache_fsm = ICACHE_MISS_WAIT;
        break;
    }
    ///////////////////////
    case ICACHE_MISS_INVAL:    // invalidate the victim line
    {
        paddr_t    nline;    // unused
    bool hit;

        hit = r_icache.inval( r_icache_miss_way.read(),
                        r_icache_miss_set.read(),
                        &nline );
    assert(hit && "selected way/set line should be in icache");

        r_icache_fsm = ICACHE_MISS_WAIT;
        break;
    }
    //////////////////////
    case ICACHE_MISS_WAIT:    // waiting a response to a miss request from VCI_RSP FSM
    {
#if INSTRUMENTATION
r_cost_icache_miss_frz++;
#endif
    if ( r_vci_rsp_ins_error.read() ) // bus error 
    {
            r_mmu_ietr = MMU_READ_DATA_ILLEGAL_ACCESS; 
            r_mmu_ibvar  = r_icache_vaddr_save.read();
            m_irsp.valid           = true;
            m_irsp.error           = true;
            r_vci_rsp_ins_error  = false;
            r_icache_fsm = ICACHE_IDLE;
        }
        else if ( r_vci_rsp_fifo_icache.rok() ) // response available
        {
            r_icache_miss_word = 0;
            r_icache_fsm       = ICACHE_MISS_UPDT;  
    }    
        break;
    }
    //////////////////////
    case ICACHE_MISS_UPDT:    // update the cache (one word per cycle)
    {
#if INSTRUMENTATION
r_cost_icache_miss_frz++;
#endif

        if ( r_vci_rsp_fifo_icache.rok() )    // response available
        {

#if INSTRUMENTATION
r_cpt_icache_write++;
#endif
            r_icache.write( r_icache_miss_way.read(),
                            r_icache_miss_set.read(),
                            r_icache_miss_word.read(),
                            r_vci_rsp_fifo_icache.read() );

            vci_rsp_fifo_icache_get = true;
            r_icache_miss_word = r_icache_miss_word.read() + 1;
            if ( r_icache_miss_word.read() == m_icache_words-1 )  // last word
            {
                r_icache.victim_update_tag( r_icache_vci_paddr.read(),
                                            r_icache_miss_way.read(),
                                            r_icache_miss_set.read() );
                r_icache_fsm = ICACHE_IDLE;
            }
        }
        break;
    }
    ////////////////////
    case ICACHE_UNC_WAIT:   // waiting a response to an uncacheable read from VCI_RSP FSM
    {
#if INSTRUMENTATION
r_cost_iunc_frz++;
#endif

    if ( r_vci_rsp_ins_error.read() ) // bus error
        {
            r_mmu_ietr          = MMU_READ_DATA_ILLEGAL_ACCESS;    
            r_mmu_ibvar         = m_ireq.addr;
            r_vci_rsp_ins_error = false;
            m_irsp.valid        = true;
            m_irsp.error        = true;
            r_icache_fsm        = ICACHE_IDLE;
        }
        else if (r_vci_rsp_fifo_icache.rok() ) // instruction available
        {
            vci_rsp_fifo_icache_get = true;
            r_icache_fsm            = ICACHE_IDLE;

            // response to processor only if request not modified
            if ( m_ireq.valid and (m_ireq.addr == r_icache_vaddr_save.read()) ) 
            {
#if INSTRUMENTION
r_cpt_ins++;
r_cpt_ins_uncacheable++;
#endif
                m_irsp.valid       = true;
                m_irsp.instruction = r_vci_rsp_fifo_icache.read();
            }
    }    
        break;
    }
    } // end switch r_icache_fsm

    ////////////////////////////////////////////////////////////////////////////////////
    //      DCACHE FSM 
    //
    // Both the Cacheability Table, and the MMU cacheable bit are used to define
    // the cacheability, depending on the MMU mode.
    // 
    // 1/ TLB miss
    //    As the page tables are cacheable, all TLB miss are handled by the DCACHE FSM.
    //    In case of miss in ITLB or DTLB, the miss is handled by a dedicated
    //    sub-fsm (DCACHE_TLB_MISS state), that handle possible miss in DCACHE,
    //    implementing the hardware table-walk.
    //    There is no hardware support for TLB coherence: Any modification in
    //    in the page table by the OS must be handled by software invalidation
    //    or flush of TLBs... 
    //
    // 2/ processor requests 
    //    Processor READ, WRITE, LL or SC requests are taken in IDLE state only.
    //    The IDLE state implements a three stages pipe-line to handle write bursts:
    //    - The physical address is computed by dtlb in stage P0.
    //    - The registration in wbuf and the dcache hit are done in stage P1. 
    //    - The dcache update is done in stage P2.  
    //    WRITE or SC requests can require a PTE Dirty bit update (in memory), 
    //    that is done (before handling the processor request) by a dedicated sub-fsm 
    //    (DCACHE_DIRTY_TLB_SET state).
    //    If there is no write in the pipe, dcache and dtlb are accessed in parallel,
    //    (virtual address for itlb, and speculative physical address computed during 
    //    previous cycle for dcache) in order to return the data in one cycle for a READ
    //    request. We just pay an extra cycle when the speculative access is failing.
    //
    // 3/ Atomic instructions LL/SC
    //    Both LL and SC instructions are considered as uncacheable: there is
    //    a VCI transaction for each ll or sc insyruction.
    //    The reservation registers (r_dcache_ll_valid, r_dcache_ll_vaddr and
    //    r_dcache_ll_data are stored in the L1 cache controller, and not in the 
    //    memory controller.
    //    - LL requests from the processor are transmitted as standard VCI
    //     uncached  READ transactions (one word).
    //    - SC requests from the processor are systematically transmitted to the 
    //      memory cache as Compare&swap requests (both the data value stored in the
    //      r_dcache_ll_data register and the new value). 
    //
    // 4/ Non cacheable access
    //    This component implement a strong order between non cacheable access
    //    (read or write) : A new non cacheable VCI transaction starts only when
    //    the previous non cacheable transaction is completed. Both cacheable and
    //    non cacheable transactions use the write buffer, but the DCACHE FSM registers
    //    a non cacheable write transaction posted in the write buffer by setting the
    //    r_dcache_pending_unc_write flip_flop. All other non cacheable requests
    //    are stalled until this flip-flop is reset by the VCI_RSP_FSM (when the 
    //    pending non cacheable write transaction completes).
    //
    // 5/ Error handling  
    //    When the MMU is not activated, Read Bus Errors are synchronous events, 
    //    but Write Bus Errors are asynchronous events (processor is not frozen).
    //    - If a Read Bus Error is detected, the VCI_RSP FSM sets the
    //      r_vci_rsp_data_error flip-flop, without writing any data in the
    //      r_vci_rsp_fifo_dcache FIFO, and the synchronous error is signaled
    //      by the DCACHE FSM.
    //    - If a Write Bus Error is detected, the VCI_RSP FSM  signals
    //      the asynchronous error using the setWriteBerr() method.
    //    When the MMU is activated bus error are rare events, as the MMU
    //    checks the physical address before the VCI transaction starts.
    ////////////////////////////////////////////////////////////////////////////////////////

    // default value for m_drsp 
    m_drsp.valid = false;
    m_drsp.error = false;
    m_drsp.rdata = 0;

    switch ( r_dcache_fsm.read() ) 
    {
    case DCACHE_IDLE:    // There is several causes to exit the IDLE state :
                        // - Dirty bit update         => DCACHE_DIRTY_GET_PTE
                        // - ITLB miss          => DCACHE_TLB_MISS
                        // - DTLB miss          => DCACHE_TLB_MISS
                        // - Cacheable read miss      => DCACHE_MISS_VICTIM
                        // - Uncacheable read          => DCACHE_UNC_WAIT 
                        // - SC access             => DCACHE_SC_WAIT
                        // - XTN request         => DCACHE_XTN_*
                        //
                        // In this state, the dtlb is unconditionally accessed to translate 
                        // the virtual adress from processor, but there is 4 configurations 
                        // to access the cache, depending on the pipe-line state, defined 
                        // by the r_dcache_p0_valid (V0) flip-flop : P1 stage activated
                        // and    r_dcache_p1_valid (V1) flip-flop : P2 stage activated
                        //  V0 / V1 / Data      / Directory / comment                    
                        //  0  / 0  / read(A0)  / read(A0)  / read speculative access  
                        //  0  / 1  / write(A2) / nop       / read request delayed
                        //  1  / 0  / nop       / read(A1)  / read request delayed
                        //  1  / 1  / write(A2) / read(A1)  / read request delayed
    { 
        bool write_pipe_frozen  = false;
        ////////////////////////////////////////////////////////////////////////////////
        // Handling P2 pipe-line stage 
        // Inputs are r_dcache_p1_* registers.
        // If r_dcache_p1_valid is true, we update the local copy in dcache.
        // If the modified cache line has copies in TLBs, we launch a TLB invalidate
        // operation, going to DCACHE_INVAL_TLB_SCAN state.

        if ( r_dcache_p1_valid.read() )        // P2 stage activated
        {
            size_t   way        = r_dcache_p1_cache_way.read();
            size_t   set        = r_dcache_p1_cache_set.read();
            size_t   word       = r_dcache_p1_cache_word.read();
            uint32_t wdata      = r_dcache_p1_wdata.read();
            vci_be_t be         = r_dcache_p1_be.read();

#if INSTRUMENTATION
r_cpt_write_cached++;
r_cpt_dcache_write++; 
#endif
            r_dcache.write( way,
                            set,
                            word,
                            wdata,
                            be );
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_IDLE> Cache update in P2 stage" << std::dec
              << " / WAY = " << way 
              << " / SET = " << set 
              << " / WORD = " << word << std::hex
              << " / DATA = " << wdata
              << " / BE = " << be << std::endl;
}
#endif
        } // end P2 stage

        ///////////////////////////////////////////////////////////////////////////
        // Handling P1 pipe-line stage 
        // Inputs are r_dcache_p0_* registers.
        // We must write into wbuf and test the hit in dcache.
        // If the write request is non cacheable, and there is a pending
        // non cacheable write, or if the write buffer is full, we break,
        // because the P0 and P1 pipe-line stages are frozen until the write
        // request registration is possible, but he P2 stage is not frozen.
        // The r_dcache_p1_valid bit must be computed at all cycles, and 
        // the P2 stage must be activated if there is a local copy in dcache. 

        if ( r_dcache_p0_valid.read() )  // P1 stage activated
        {
            // uncacheable write, and previous uncacheable write registered : frozen
            if ( not r_dcache_p0_cacheable.read() and r_dcache_pending_unc_write.read() ) 
            {
#if INSTRUMENTATION
r_cost_unc_write_frz++;
#endif
                r_dcache_p1_valid = false;
				write_pipe_frozen = true;
            }
			else
			{
				// try a registration into write buffer
				bool wok = r_wbuf.write( r_dcache_p0_paddr.read(),
										 r_dcache_p0_be.read(),
										 r_dcache_p0_wdata.read(),
										 r_dcache_p0_cacheable.read() );
#if INSTRUMENTATION
r_cpt_wbuf_write++;
#endif

				// write buffer full : frozen
				if ( not wok ) 
				{
#if INSTRUMENTATION
r_cost_wbuf_full_frz++;
#endif
					r_dcache_p1_valid = false;
					write_pipe_frozen = true;
				}
				else
				{
					// update the write_buffer state extension
					if ( not r_dcache_p0_cacheable.read() )
					{
#if INSTRUMENTATION
r_cpt_write_uncacheable++;
#endif
						r_dcache_pending_unc_write = true;
					}

					// read directory to check local copy
					size_t  cache_way;
					size_t  cache_set;
					size_t  cache_word;
					bool    local_copy;

					if ( r_mmu_mode.read() & DATA_CACHE_MASK)     // cache activated
					{
						local_copy = r_dcache.hit( r_dcache_p0_paddr.read(),
								&cache_way,
								&cache_set,
								&cache_word );
					}
					else
					{
						local_copy = false;
					}

					// store values for P2 pipe stage
					if ( local_copy )
					{
						r_dcache_p1_valid       = true;
						r_dcache_p1_wdata       = r_dcache_p0_wdata.read();
						r_dcache_p1_be          = r_dcache_p0_be.read();
						r_dcache_p1_paddr       = r_dcache_p0_paddr.read();
						r_dcache_p1_cache_way   = cache_way;
						r_dcache_p1_cache_set   = cache_set;
						r_dcache_p1_cache_word  = cache_word;
					}
					else
					{
						r_dcache_p1_valid       = false;
					}
				}
			}
		}
		else // P1 stage not activated 
		{
			r_dcache_p1_valid = false; 
		} // end P1 stage

        /////////////////////////////////////////////////////////////////////////////////
        // handling P0 pipe-line stage
        // This stage is controlling r_dcache_fsm and r_dcache_p0_* registers. 
        // The r_dcache_p0_valid flip-flop is only set in case of a WRITE request.
        // The itlb miss requests, are handled before the processor requests. 
        // If dtlb is activated, there is an unconditionnal access to dtlb, 
        // for address translation.
        // 1) A processor WRITE request is blocked if the Dirty bit mus be set, or if 
        //    dtlb miss. If dtlb OK, it enters the three stage pipe-line (fully 
        //    handled in the IDLE state), and the processor request is acknowledged.
        // 2) A processor READ or LL request generate a simultaneouss access to
        //    both dcache data and dcache directoty, using speculative PPN, but
        //    is delayed if the write pipe-line is not empty.
        //    In case of miss, we wait the VCI response in DCACHE_UNC_WAIT or 
        //    DCACHE_MISS_WAIT states.
        // 3) A processor SC request is delayed until the write pipe-line is empty.
        //    A VCI SC transaction is launched, and we wait the VCI response in
        //    DCACHE_SC_WAIT state. It can be completed by a "long write" if the 
        //    PTE dirty bit must be updated in dtlb, dcache, and RAM.
        //    The data is not modified in dcache, as it will be done by the
        //    coherence transaction.   

		// processor request
		// A new processor request is only treated if the pipe-line is not frozen
		// due to a non finished write operation in the P1 stage
        if ( m_dreq.valid and not write_pipe_frozen)
        {
            // dcache access using speculative PPN only if pipe-line empty
            paddr_t     cache_paddr;
            size_t      cache_way;
            size_t      cache_set;
            size_t      cache_word;
            uint32_t    cache_rdata = 0;
            bool        cache_hit;

            if ( (r_mmu_mode.read() & DATA_CACHE_MASK) and     // cache activated
                 not r_dcache_p0_valid.read() and 
                 not r_dcache_p1_valid.read() )            // pipe-line empty
            {
                cache_paddr = (r_dcache_p0_paddr.read() & ~PAGE_K_MASK) | 
                              ((paddr_t)m_dreq.addr & PAGE_K_MASK);
#if INSTRUMENTATION
r_cpt_dcache_read++;
#endif
                cache_hit = r_dcache.read( cache_paddr,
                                           &cache_rdata,
                                           &cache_way,
                                           &cache_set,
                                           &cache_word );
            }
            else
            {
                cache_hit = false;
            } // end dcache access    

            // systematic dtlb access using virtual address
            paddr_t     tlb_paddr;
            pte_info_t  tlb_flags; 
            size_t      tlb_way; 
            size_t      tlb_set; 
            paddr_t     tlb_nline; 
            bool        tlb_hit;        

            if ( r_mmu_mode.read() & DATA_TLB_MASK )    // DTLB activated
            {
#if INSTRUMENTATION
r_cpt_dtlb_read++;
#endif
                tlb_hit = r_dtlb.translate( m_dreq.addr,
                                            &tlb_paddr,
                                            &tlb_flags,
                                            &tlb_nline,
                                            &tlb_way,    
                                            &tlb_set );    
            }
            else
            {
                tlb_hit = false;
            } // end dtlb access

            // register the processor request
            r_dcache_p0_vaddr = m_dreq.addr;
            r_dcache_p0_be    = m_dreq.be;
            r_dcache_p0_wdata = m_dreq.wdata;

            // Handling READ XTN requests from processor
            // They are executed in this DCACHE_IDLE state.
            // The processor must not be in user mode
            if (m_dreq.type == iss_t::XTN_READ) 
            {
                int xtn_opcode = (int)m_dreq.addr/4;

                // checking processor mode:
                if (m_dreq.mode  == iss_t::MODE_USER)
                {
                    r_mmu_detr   = MMU_READ_PRIVILEGE_VIOLATION; 
                    r_mmu_dbvar  = m_dreq.addr;
                    m_drsp.valid = true;
                    m_drsp.error = true;
                    r_dcache_fsm = DCACHE_IDLE;
                }
                else 
                {
                    switch( xtn_opcode ) 
                    {
                    case iss_t::XTN_INS_ERROR_TYPE:
                        m_drsp.rdata = r_mmu_ietr.read();
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_DATA_ERROR_TYPE:
                        m_drsp.rdata = r_mmu_detr.read();
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_INS_BAD_VADDR:
                        m_drsp.rdata = r_mmu_ibvar.read();       
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_DATA_BAD_VADDR:
                        m_drsp.rdata = r_mmu_dbvar.read();        
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_PTPR:
                        m_drsp.rdata = r_mmu_ptpr.read();
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_TLB_MODE:
                        m_drsp.rdata = r_mmu_mode.read();
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_MMU_PARAMS:
                        m_drsp.rdata = r_mmu_params;
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_MMU_RELEASE:
                        m_drsp.rdata = r_mmu_release;
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_MMU_WORD_LO:
                        m_drsp.rdata = r_mmu_word_lo.read();
                        m_drsp.valid = true;
                        break;

                    case iss_t::XTN_MMU_WORD_HI:
                        m_drsp.rdata = r_mmu_word_hi.read();
                        m_drsp.valid = true;
                        break;

                    default:
                        r_mmu_detr = MMU_READ_UNDEFINED_XTN; 
                        r_mmu_dbvar  = m_dreq.addr;
                        m_drsp.valid = true;
                        m_drsp.error = true;
                        break;
                    } // end switch xtn_opcode
                } // end else
                r_dcache_p0_valid = false;
            } // end if XTN_READ

            // Handling WRITE XTN requests from processor.
            // They are not executed in this DCACHE_IDLE state,
            // if they require access to the caches or the TLBs
            // that are already accessed for speculative read.
            // Caches can be invalidated or flushed in user mode,
            // and the sync instruction can be executed in user mode
            else if (m_dreq.type == iss_t::XTN_WRITE and
					not r_dcache_p0_valid) 
            {
                int xtn_opcode      = (int)m_dreq.addr/4;
                r_dcache_xtn_opcode = xtn_opcode;

                // checking processor mode:
                if ( (m_dreq.mode  == iss_t::MODE_USER) &&
                     (xtn_opcode != iss_t:: XTN_SYNC) &&
                     (xtn_opcode != iss_t::XTN_DCACHE_INVAL) &&
                     (xtn_opcode != iss_t::XTN_DCACHE_FLUSH) &&
                     (xtn_opcode != iss_t::XTN_ICACHE_INVAL) &&
                     (xtn_opcode != iss_t::XTN_ICACHE_FLUSH) )
                {
                    r_mmu_detr = MMU_WRITE_PRIVILEGE_VIOLATION; 
                    r_mmu_dbvar  = m_dreq.addr;
                    m_drsp.valid          = true;
                    m_drsp.error          = true;
                    r_dcache_fsm        = DCACHE_IDLE;
                }
                else
                {
                    switch( xtn_opcode ) 
                    {     
                    case iss_t::XTN_PTPR:               // itlb & dtlb must be flushed
                        r_mmu_ptpr       = m_dreq.wdata;
                        r_dcache_xtn_req = true;
                        r_dcache_fsm     = DCACHE_XTN_SWITCH;
                        break;

                    case iss_t::XTN_TLB_MODE:            // no cache or tlb access 
                        r_mmu_mode = m_dreq.wdata;
                        m_drsp.valid = true;
                        r_dcache_fsm = DCACHE_IDLE;
                        break;

                    case iss_t::XTN_DTLB_INVAL:             // dtlb access
                        r_dcache_fsm = DCACHE_XTN_DT_INVAL;  
                        break;

                    case iss_t::XTN_ITLB_INVAL:             // itlb access
                        r_dcache_xtn_req = true;
                        r_dcache_fsm = DCACHE_XTN_IT_INVAL;  
                        break;

                    case iss_t::XTN_DCACHE_INVAL:           // dcache, dtlb & itlb access
                        r_dcache_fsm = DCACHE_XTN_DC_INVAL_VA;
                        break;

                    case iss_t::XTN_MMU_DCACHE_PA_INV:      // dcache, dtlb & itlb access
                        r_dcache_fsm   = DCACHE_XTN_DC_INVAL_PA;
                        if (sizeof(paddr_t) <= 32) 
                        {
                            assert(r_mmu_word_hi.read() == 0 &&
                            "high bits should be 0 for 32bit paddr");
                            r_dcache_p0_paddr = (paddr_t)r_mmu_word_lo.read();
                        } 
                        else 
                        {
                            r_dcache_p0_paddr = (paddr_t)r_mmu_word_hi.read() << 32 | 
                                                (paddr_t)r_mmu_word_lo.read();
                        }
                        break;

                    case iss_t::XTN_DCACHE_FLUSH:              // itlb and dtlb must be reset  
                        r_dcache_flush_count = 0;
                        r_dcache_fsm         = DCACHE_XTN_DC_FLUSH; 
                        break;

                    case iss_t::XTN_ICACHE_INVAL:           // icache and itlb access
                        r_dcache_xtn_req = true;
                        r_dcache_fsm     = DCACHE_XTN_IC_INVAL_VA; 
                        break;

                    case iss_t::XTN_MMU_ICACHE_PA_INV:        // icache access 
                        r_dcache_xtn_req = true;
                        r_dcache_fsm     = DCACHE_XTN_IC_INVAL_PA; 
                        break;

                    case iss_t::XTN_ICACHE_FLUSH:           // icache access
                        r_dcache_xtn_req = true; 
                        r_dcache_fsm     = DCACHE_XTN_IC_FLUSH;
                        break;

                    case iss_t::XTN_SYNC:                   // wait until write buffer empty
                        r_dcache_fsm     = DCACHE_XTN_SYNC;
                        break;

                    case iss_t::XTN_MMU_WORD_LO:         // no cache or tlb access
                        r_mmu_word_lo = m_dreq.wdata;
                        m_drsp.valid    = true;
                        r_dcache_fsm  = DCACHE_IDLE;
                        break;

                    case iss_t::XTN_MMU_WORD_HI:         // no cache or tlb access
                        r_mmu_word_hi = m_dreq.wdata;
                        m_drsp.valid    = true;
                        r_dcache_fsm  = DCACHE_IDLE;
                        break;

                    case iss_t::XTN_ICACHE_PREFETCH:        // not implemented : no action
                    case iss_t::XTN_DCACHE_PREFETCH:        // not implemented : no action
                        m_drsp.valid   = true;
                        r_dcache_fsm = DCACHE_IDLE;
                    break;
    
                    default:
                        r_mmu_detr = MMU_WRITE_UNDEFINED_XTN; 
                        r_mmu_dbvar  = m_dreq.addr;
                        m_drsp.valid = true;
                        m_drsp.error = true;
                        r_dcache_fsm = DCACHE_IDLE;
                        break;
                    } // end switch xtn_opcode
                } // end else 
                r_dcache_p0_valid = false;
            } // end if XTN_WRITE

            // Handling read/write/ll/sc processor requests.
            // The dtlb and dcache can be activated or not.
            // We compute the physical address, the cacheability, and check processor request.
            // - If DTLB not activated : cacheability is defined by the segment table,
            //   the physical address is equal to the virtual address (identity mapping)
            // - If DTLB activated : cacheability is defined by the C bit in the PTE,
            //   the physical address is obtained from the TLB, and the U & W bits
            //   of the PTE are checked. 
            // The processor request is decoded only if the TLB is not activated or if
            // the virtual address hits in tLB and access rights are OK.
            // We call the TLB_MISS sub-fsm in case of dtlb miss.
            // The processor LL or SC accesses are handled as uncacheable.
            else
            {
                bool    valid_req = false;
                bool    cacheable = false;
                paddr_t    paddr     = 0;

                if ( not (r_mmu_mode.read() & DATA_TLB_MASK) )        // dtlb not activated 
                {
                    valid_req     = true;

                    // cacheability
                    if ( (m_dreq.type == iss_t::DATA_LL) or 
                         not (r_mmu_mode.read() & DATA_CACHE_MASK) ) 
                    {
                        cacheable = false;
                    }
                    else 
                    {
                        cacheable = m_cacheability_table[(uint64_t)m_dreq.addr];
                    }

                    // physical address
                    paddr       = (paddr_t)m_dreq.addr;
                }
                else                             // dtlb activated
                {
                    if ( tlb_hit )                    // tlb hit
                    {
                        // cacheability
                        if ( (m_dreq.type == iss_t::DATA_LL) or 
                             not (r_mmu_mode.read() & DATA_CACHE_MASK) ) 
                        {
                            cacheable = false;
                        }
                        else 
                        {
                            cacheable = tlb_flags.c;
                        }

                        // access rights checking 
                        if ( not tlb_flags.u and (m_dreq.mode == iss_t::MODE_USER)) 
                        {
                            if ( (m_dreq.type == iss_t::DATA_READ) or 
                                 (m_dreq.type == iss_t::DATA_LL) )
                                r_mmu_detr = MMU_READ_PRIVILEGE_VIOLATION;
                            else 
                                r_mmu_detr = MMU_WRITE_PRIVILEGE_VIOLATION;

                            r_mmu_dbvar  = m_dreq.addr;
                            m_drsp.valid   = true;
                            m_drsp.error   = true;
                            m_drsp.rdata   = 0;
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_IDLE> HIT in dtlb, but privilege violation" << std::endl;
}
#endif
                        }
                        else if ((m_dreq.mode == iss_t::MODE_USER ) and not tlb_flags.w and 
                                ((m_dreq.type == iss_t::DATA_WRITE) or 
                                 (m_dreq.type == iss_t::DATA_SC   ))) 
                        {
                            r_mmu_detr   = MMU_WRITE_ACCES_VIOLATION;  
                            r_mmu_dbvar  = m_dreq.addr;
                            m_drsp.valid   = true;
                            m_drsp.error   = true;
                            m_drsp.rdata   = 0;
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_IDLE> HIT in dtlb, but writable violation" << std::endl;
}
#endif
                        }
                        else
                        {
                            valid_req    = true;
                        }

                        // physical address
                        paddr = tlb_paddr;
                    }
                    else                        // tlb miss
                    {
#if INSTRUMENTATION
r_cpt_dtlb_miss++;
#endif
						// The pipeline must be empty before treatment of DTLB MISS
						// We do not care about the P2 state valid register because if it is
						// valid, the operation is already made
						if ( r_dcache_p0_valid.read() )
						{
							r_dcache_p0_valid = false;
							break;
						}

                        r_dcache_tlb_vaddr   = m_dreq.addr;
                        r_dcache_tlb_ins     = false; 
                        r_dcache_fsm         = DCACHE_TLB_MISS;
                    }
                }    // end DTLB activated

                if ( valid_req )     // processor request is valid after TLB check
                {
                    // physical address and cacheability registration 
                    r_dcache_p0_paddr          = paddr;
                    r_dcache_p0_cacheable      = cacheable;

                    // READ or LL request
                    // The read requests are taken only if the write pipe-line is empty.
                    // If dcache hit, dtlb hit, and speculative PPN OK, data in one cycle.
                    // If speculative access is KO we just pay one extra cycle.
                    // If dcache miss, we go to DCACHE_MISS_VICTIM state.
                    // If uncacheable, we go to DCACHE_UNC_WAIT state.
                    if ( ((m_dreq.type == iss_t::DATA_READ) or (m_dreq.type == iss_t::DATA_LL)) 
                        and not r_dcache_p0_valid.read() and not r_dcache_p1_valid.read() )
                    { 
                        if ( cacheable )                    // cacheable read
                        {
                            // if the speculative access is illegal, we pay an extra cycle
                            if ( (r_dcache_p0_paddr.read() & ~PAGE_K_MASK) 
                                 != (paddr & ~PAGE_K_MASK))
                            {
#if INSTRUMENTATION
r_cpt_dcache_spec_miss++;
#endif

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_IDLE> Speculative access miss" << std::endl;
}
#endif
                            }
                            // if cache miss, try to get the missing line
                            else if ( not cache_hit )
                            {
#if INSTRUMENTATION
r_cpt_dcache_miss++;
#endif
                                r_dcache_vci_paddr    = paddr;
                                r_dcache_vci_miss_req = true;
                                r_dcache_miss_type    = PROC_MISS;
                                r_dcache_fsm          = DCACHE_MISS_VICTIM;
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_IDLE> Dcache miss" << std::endl;
}
#endif
                            }
                            // if cache hit return the data
                            else                    
                            {
#if INSTRUMENTATION
r_cpt_read++;
#endif
                                m_drsp.valid   = true;
                                m_drsp.rdata   = cache_rdata;
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_IDLE> Dcache hit" << std::endl;
}
#endif
                            }
                        }
                        else                    // uncacheable read
                        {
                            r_dcache_vci_paddr    = paddr;
                            r_dcache_vci_unc_be   = m_dreq.be;
                            r_dcache_vci_unc_req  = true;
                            r_dcache_fsm          = DCACHE_UNC_WAIT;
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_IDLE> Uncachable Read :" << std::hex 
                        << " VADDR = " << m_dreq.addr
                        << " | PADDR = " << paddr
                        << " | BE = " << (uint32_t)m_dreq.be << std::endl;
}
#endif
                        }

                        r_dcache_p0_valid = false;
                    } // end READ or LL

                    // WRITE request:
                    // If the TLB is activated and the PTE Dirty bit is not set, we stall 
                    // the processor and set the Dirty bit before handling the write request. 
                    // If we don't need to set the Dirty bit, we can acknowledge
                    // the processor request, as the write arguments (including the
                    // physical address) are registered in r_dcache_p0 registers:
                    // We simply activate the P1 pipeline stage.
                    else if ( m_dreq.type == iss_t::DATA_WRITE )
                    {
                        if ( (r_mmu_mode.read() & DATA_TLB_MASK ) 
                              and not tlb_flags.d )        // Dirty bit must be set
                        {
#if INSTRUMENTATION
r_cpt_dirty_bit_updt++;
#endif
							// The pipeline must be empty before treatment of WRITE request
							// with PTE Dirty bit not set
							// We do not care about the P2 state valid register because if it is
							// valid, the operation is already made
							if ( r_dcache_p0_valid.read() )
							{
								r_dcache_p0_valid = false;
								break;
							}

                            // The PTE physical address is obtained from the nline value (dtlb),
                            // and the word index (proper bits of the virtual address)
                            if ( tlb_flags.b )    // PTE1
                            {
                                r_dcache_dirty_paddr = (paddr_t)(tlb_nline*(m_dcache_words<<2)) |
                                                       (paddr_t)((m_dreq.addr>>19) & 0x3c);
                            }
                            else        // PTE2
                            {
                                r_dcache_dirty_paddr = (paddr_t)(tlb_nline*(m_dcache_words<<2)) |
                                                       (paddr_t)((m_dreq.addr>>9) & 0x38);
                            }
                            r_dcache_tlb_way  = tlb_way;
                            r_dcache_tlb_set  = tlb_set;
                            r_dcache_fsm      = DCACHE_DIRTY_GET_PTE;
                        }
                        else                    // Write request accepted
                        {
#if INSTRUMENTATION
r_cpt_write++;
#endif
                            m_drsp.valid      = true;
                            m_drsp.rdata      = 0;
                            r_dcache_p0_valid = true;
                        }
                    } // end WRITE
 
                    // SC request:
                    // - if there is no valid registered LL, we just return rdata = 1 
                    //   (atomic access failed) and the SC transaction is completed.
                    // - if a valid LL reservation (with the same address) is registered, 
                    //   we test if a DIRTY bit update is required. If it's required, we stall
                    //   the processor and set the Dirty bit before handling the SC request. 
                    //   If we don't need to set the Dirty bit, we request a SC transaction 
                    //   to CMD FSM and go to DCACHE_SC_WAIT state, that will return 
                    //   the response to the processor. 
                    else if ( m_dreq.type == iss_t::DATA_SC )
                    {
                        if ( (r_dcache_ll_vaddr.read() != m_dreq.addr)
                             or not r_dcache_ll_valid.read() )     // no valid registered LL
                        { 
#if INSTRUMENTATION
r_cpt_sc++;
#endif
                            m_drsp.valid        = true;
                            m_drsp.rdata        = 1;
                            r_dcache_ll_valid   = false;
                        }
                        else                    // valid registered LL
                        {
							// The pipeline must be empty before treatment of SC valid request
							if ( r_dcache_p0_valid.read() or r_dcache_p1_valid.read() )
							{
								r_dcache_p0_valid = false;
								break;
							}

                            if ( (r_mmu_mode.read() & DATA_TLB_MASK ) 
                                  and not tlb_flags.d )            // Dirty bit must be set
                            {
#if INSTRUMENTATION
r_cpt_dirty_bit_updt++;
#endif
                                // The PTE physical address is obtained from the nline value (dtlb),
                                // and the word index (proper bits of the virtual address)
                                if ( tlb_flags.b )    // PTE1
                                {
                                    r_dcache_dirty_paddr = (paddr_t)(tlb_nline*(m_dcache_words<<2)) |
                                                           (paddr_t)((m_dreq.addr>>19) & 0x3c);
                                }
                                else            // PTE2
                                {
                                    r_dcache_dirty_paddr = (paddr_t)(tlb_nline*(m_dcache_words<<2)) |
                                                           (paddr_t)((m_dreq.addr>>9) & 0x38);
                                }
                                r_dcache_tlb_way    = tlb_way;
                                r_dcache_tlb_set    = tlb_set;
                                r_dcache_fsm        = DCACHE_DIRTY_GET_PTE;
                            }
                            else                    // SC request accepted
                            {
                                // (speculative access success) or not cacheable
                                if ( not cacheable or ((r_dcache_p0_paddr.read() & ~PAGE_K_MASK) == (paddr & ~PAGE_K_MASK)))
                                {
                                    r_dcache_sc_hit     = cache_hit;
                                    r_dcache_sc_way     = cache_way;
                                    r_dcache_sc_set     = cache_set;
                                    r_dcache_sc_word    = cache_word;
                                    r_dcache_vci_paddr  = paddr;
                                    r_dcache_vci_sc_req = true;
                                    r_dcache_vci_sc_old = r_dcache_ll_data.read();
                                    r_dcache_vci_sc_new = m_dreq.wdata;
                                    r_dcache_ll_valid   = false;
                                    r_dcache_fsm        = DCACHE_SC_WAIT;
                                }
                            }
                        }
                        r_dcache_p0_valid = false;
                    } // end SC
                    else
                    {
                        r_dcache_p0_valid = false;
                    }
                } // end valid_req
                else
                {
                    r_dcache_p0_valid = false;
                }
            }  // end if read/write/ll/sc request    
        } // end dreq.valid
        
        // itlb miss request 
        else if ( r_icache_tlb_miss_req.read() )
        {
			// The pipeline must be empty before treatment of ITLB MISS request
			// We do not care about the P2 state valid register because if it is
			// valid, the operation is already made
			if ( r_dcache_p0_valid.read() )
			{
				r_dcache_p0_valid = write_pipe_frozen;
				break;
			}

            r_dcache_tlb_ins    = true;
            r_dcache_tlb_vaddr  = r_icache_vaddr_save.read();
            r_dcache_fsm        = DCACHE_TLB_MISS;
        }
        else
        {
            r_dcache_p0_valid = r_dcache_p0_valid.read() and write_pipe_frozen;
        } // end P0 pipe stage
        break;
    } 
    /////////////////////
    case DCACHE_TLB_MISS: // This is the entry point for the sub-fsm handling all tlb miss.
                          // Input arguments are:
                          // - r_dcache_tlb_vaddr
                          // - r_dcache_tlb_ins (true when itlb miss) 
                          // The sub-fsm access the dcache to find the missing TLB entry,
                          // and activates the cache miss procedure in case of miss.
                          // It bypass the first level page table access if possible.
                          // It uses atomic access to update the R/L access bits
                          // in the page table if required.
                          // It directly updates the itlb or dtlb, and writes into the 
                          // r_mmu_ins_* or r_mmu_data* error reporting registers.
    {
        uint32_t    ptba = 0;
        bool        bypass;
        paddr_t        pte_paddr;

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
#endif

        // evaluate bypass in order to skip first level page table access
        if ( r_dcache_tlb_ins.read() )                // itlb miss
        {
            bypass = r_itlb.get_bypass(r_dcache_tlb_vaddr.read(), &ptba);

#if INSTRUMENTATION
if ( bypass ) r_cpt_itlb_miss_bypass++;
#endif
        }
        else                            // dtlb miss
        {
            bypass = r_dtlb.get_bypass(r_dcache_tlb_vaddr.read(), &ptba);

            #if INSTRUMENTATION
                if ( bypass ) r_cpt_dtlb_miss_bypass++;
            #endif
        }

        if ( not bypass )     // Try to read PTE1/PTD1 in dcache
        {
            pte_paddr = (paddr_t)r_mmu_ptpr.read() << (INDEX1_NBITS+2) |
                        (paddr_t)((r_dcache_tlb_vaddr.read() >> PAGE_M_NBITS) << 2);
            r_dcache_tlb_paddr = pte_paddr;
            r_dcache_fsm       = DCACHE_TLB_PTE1_GET;
        }
        else                  // Try to read PTE2 in dcache
        {
            pte_paddr = (paddr_t)ptba << PAGE_K_NBITS |
                        (paddr_t)(r_dcache_tlb_vaddr.read()&PTD_ID2_MASK)>>(PAGE_K_NBITS-3);
            r_dcache_tlb_paddr = pte_paddr;
            r_dcache_fsm       = DCACHE_TLB_PTE2_GET;
        }

#if DEBUG_DCACHE
if ( r_debug_active )
{
    if ( r_dcache_tlb_ins.read() ) 
    {
        std::cout << name() << " <DCACHE_TLB_MISS> ITLB miss";
    }
    else
    {                           
        std::cout << name() << " <DCACHE_TLB_MISS> DTLB miss";
    }
    std::cout << " / VADDR = " << std::hex << r_dcache_tlb_vaddr.read()
              << " / BYPASS = " << bypass 
              << " / PTE_ADR = " << pte_paddr << std::endl;
}
#endif
  
        break;
    }
    /////////////////////////  
    case DCACHE_TLB_PTE1_GET:    // try to read a PT1 entry in dcache
    {

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
#endif

        uint32_t     entry;
        size_t        way;
        size_t        set;
        size_t        word;

#if INSTRUMENTATION
r_cpt_dcache_read++;
#endif
        bool     hit = r_dcache.read( r_dcache_tlb_paddr.read(),
                                      &entry,
                                      &way,
                                      &set,
                                      &word );
        if ( hit )    //  hit in dcache 
        {
            if ( not (entry & PTE_V_MASK) )    // unmapped
            {
                if ( r_dcache_tlb_ins.read() ) 
                {
                    r_mmu_ietr             = MMU_READ_PT1_UNMAPPED;
                    r_mmu_ibvar            = r_dcache_tlb_vaddr.read();
                    r_icache_tlb_miss_req  = false;
                    r_icache_tlb_rsp_error = true;
                }
                else
                {
                    r_mmu_detr             = MMU_READ_PT1_UNMAPPED;
                    r_mmu_dbvar            = r_dcache_tlb_vaddr.read();
                    m_drsp.valid             = true;
                    m_drsp.error             = true;
                }
                r_dcache_fsm          = DCACHE_IDLE;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_PTE1_GET> HIT in dcache, but unmapped"
              << std::hex << " / PADDR = " << r_dcache_tlb_paddr.read()
              << std::dec << " / WAY = " << way
              << std::dec << " / SET = " << set
              << std::dec << " / WORD = " << word
              << std::hex << " / PTE1 = " << entry << std::endl;
}
#endif
  
            }
            else if( entry & PTE_T_MASK )     //  PTD : me must access PT2
            {
                // register bypass
                if ( r_dcache_tlb_ins.read() )        // itlb
                {
                    r_itlb.set_bypass(r_dcache_tlb_vaddr.read(),
                                      entry & ((1 << (m_paddr_nbits-PAGE_K_NBITS)) - 1), 
                                      r_dcache_tlb_paddr.read() >> (uint32_log2(m_icache_words<<2))); 
                }
                else                    // dtlb
                {
                    r_dtlb.set_bypass(r_dcache_tlb_vaddr.read(),
                                      entry & ((1 << (m_paddr_nbits-PAGE_K_NBITS)) - 1),
                                      r_dcache_tlb_paddr.read() >> (uint32_log2(m_dcache_words)+2));
                }
                r_dcache_tlb_paddr = (paddr_t)(entry & ((1<<(m_paddr_nbits-PAGE_K_NBITS))-1)) << PAGE_K_NBITS |
                                     (paddr_t)(((r_dcache_tlb_vaddr.read() & PTD_ID2_MASK) >> PAGE_K_NBITS) << 3);
                r_dcache_fsm       = DCACHE_TLB_PTE2_GET;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_PTE1_GET> HIT in dcache"
              << std::hex << " / PADDR = " << r_dcache_tlb_paddr.read()
              << std::dec << " / WAY = " << way
              << std::dec << " / SET = " << set
              << std::dec << " / WORD = " << word
              << std::hex << " / PTD = " << entry << std::endl;
}
#endif
            }
            else            //  PTE1 :  we must update the TLB
            {
                r_dcache_tlb_pte_flags  = entry;
                r_dcache_tlb_cache_way  = way;
                r_dcache_tlb_cache_set  = set;
                r_dcache_tlb_cache_word = word;
                r_dcache_fsm            = DCACHE_TLB_PTE1_SELECT;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_PTE1_GET> HIT in dcache"
              << std::hex << " / PADDR = " << r_dcache_tlb_paddr.read()
              << std::dec << " / WAY = " << way
              << std::dec << " / SET = " << set
              << std::dec << " / WORD = " << word
              << std::hex << " / PTE1 = " << entry << std::endl;
}
#endif
            }
        }
        else        // we must load the missing cache line in dcache
        {
            r_dcache_vci_miss_req  = true;        
            r_dcache_vci_paddr     = r_dcache_tlb_paddr.read(); 
            r_dcache_miss_type     = PTE1_MISS;
            r_dcache_fsm           = DCACHE_MISS_VICTIM;     

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_PTE1_GET> MISS in dcache:"
              << " PADDR = " << std::hex << r_dcache_tlb_paddr.read() << std::endl;
}
#endif
        }
        break;
    }
    ////////////////////////////
    case DCACHE_TLB_PTE1_SELECT:    // select a slot for PTE1 
    {

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
#endif

        size_t     way;
        size_t     set;

        if ( r_dcache_tlb_ins.read() )
        {
            r_itlb.select( r_dcache_tlb_vaddr.read(),
                           true,  // PTE1 
                           &way,
                           &set );
        }
        else
        {
            r_dtlb.select( r_dcache_tlb_vaddr.read(),
                           true,  // PTE1 
                           &way,
                           &set );
        }
        r_dcache_tlb_way = way;
        r_dcache_tlb_set = set;
        r_dcache_fsm     = DCACHE_TLB_PTE1_UPDT;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    if ( r_dcache_tlb_ins.read() ) 
        std::cout << name() << " <DCACHE_TLB_PTE1_SELECT> Select a slot in ITLB:";
    else                           
        std::cout << name() << " <DCACHE_TLB_PTE1_SELECT> Select a slot in DTLB:";
        std::cout << " WAY = " << std::dec << way
                  << " / SET = " << set << std::endl;
}
#endif
        break;
    }
    //////////////////////////
    case DCACHE_TLB_PTE1_UPDT:    // write a new PTE1 in tlb after testing the L/R bit
                // if L/R bit already set, exit the sub-fsm
                                // if not, dcache is updated, and the page table
                                // must be updated by a SC transaction.
    {

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
#endif

        paddr_t      nline = r_dcache_tlb_paddr.read() >> (uint32_log2(m_dcache_words)+2);   
        uint32_t  pte   = r_dcache_tlb_pte_flags.read();
        bool      updt  = false;
        bool      local = true;

        // We should compute the access locality: 
        // The PPN MSB bits define the destination cluster index.
        // The m_srcid_d MSB bits define the source cluster index.
        // The number of bits to compare depends on the number of clusters, 
        // and can be obtained in the mapping table.
        // As this computation is not done yet, all access are marked as local.

        if ( local )                      // local access
        {
            if ( not ((pte & PTE_L_MASK) == PTE_L_MASK) ) // we must set the L bit
            {
                updt = true;

                // request a SC transaction for page table update
                r_dcache_vci_sc_old    = pte;
                r_dcache_vci_sc_new    = pte | PTE_L_MASK;
                r_dcache_vci_paddr     = r_dcache_tlb_paddr.read();
                r_dcache_vci_sc_req    = true;

                // prepare both TLB and dcache update
                pte = pte | PTE_L_MASK; 

                // update dcache
                r_dcache.write( r_dcache_tlb_cache_way.read(),
                                r_dcache_tlb_cache_set.read(),
                                r_dcache_tlb_cache_word.read(),
                                pte );
#if INSTRUMENTATION
r_cpt_dcache_write++;
#endif
            }
        }
        else                                                    // remote access
        {
            if ( not ((pte & PTE_R_MASK) == PTE_R_MASK) ) // we must set the R bit
            {
                updt                   = true;

                // request a SC transaction for page table update
                r_dcache_vci_sc_old    = pte;
                r_dcache_vci_sc_new    = pte | PTE_R_MASK;
                r_dcache_vci_paddr     = r_dcache_tlb_paddr.read();
                r_dcache_vci_sc_req    = true;

                // prepare both TLB and dcache update
                pte = pte | PTE_R_MASK;

                // update dcache
                r_dcache.write( r_dcache_tlb_cache_way.read(),
                                r_dcache_tlb_cache_set.read(),
                                r_dcache_tlb_cache_word.read(),
                                pte );
#if INSTRUMENTATION
r_cpt_dcache_write++;
#endif
            }
        }

        // update TLB for a PTE1
        if ( r_dcache_tlb_ins.read() )  
        {
            r_itlb.write( true,        // 2M page
                          pte,
                          0,        // argument unused for a PTE1
                          r_dcache_tlb_vaddr.read(),    
                          r_dcache_tlb_way.read(), 
                          r_dcache_tlb_set.read(),
                          nline );
#if INSTRUMENTATION
r_cpt_itlb_write++;
#endif
        }
        else
        {
            r_dtlb.write( true,        // 2M page
                          pte,
                          0,        // argument unused for a PTE1
                          r_dcache_tlb_vaddr.read(),    
                          r_dcache_tlb_way.read(), 
                          r_dcache_tlb_set.read(),
                          nline );
#if INSTRUMENTATION
r_cpt_dtlb_write++;
#endif
        }

        // next state
        if ( updt ) r_dcache_fsm = DCACHE_TLB_LR_WAIT;     // waiting SC response
        else        r_dcache_fsm = DCACHE_TLB_RETURN;    // exit sub-fsm

#if DEBUG_DCACHE
if ( r_debug_active )
{
    if ( r_dcache_tlb_ins.read() ) 
    {
        std::cout << name() << " <DCACHE_TLB_PTE1_UPDT> write PTE1 in ITLB";
        std::cout << " / SET = " << std::dec << r_dcache_tlb_set.read()
                  << " / WAY = " << r_dcache_tlb_way.read() << std::endl;
        r_itlb.printTrace();
    }
    else                           
    {
        std::cout << name() << " <DCACHE_TLB_PTE1_UPDT> write PTE1 in DTLB";
        std::cout << " / SET = " << std::dec << r_dcache_tlb_set.read()
                  << " / WAY = " << r_dcache_tlb_way.read() << std::endl;
        r_dtlb.printTrace();
    }
    if ( updt )
    {
        std::cout << "                              Update (L/R) bit in dcache" << std::endl;
    }
    
}
#endif
        break;
    }
    /////////////////////////
    case DCACHE_TLB_PTE2_GET:    // Try to get a PTE2 (64 bits) in the dcache
    {

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
#endif

        uint32_t     pte_flags;
        uint32_t     pte_ppn;
        size_t       way;
        size_t       set;
        size_t        word; 
 
#if INSTRUMENTATION
r_cpt_dcache_read++;
#endif
        bool     hit = r_dcache.read( r_dcache_tlb_paddr.read(),
                                      &pte_flags,
                                      &pte_ppn,
                                      &way,
                                      &set,
                                      &word );
        if ( hit )      // request hits in dcache 
        {
            if ( not (pte_flags & PTE_V_MASK) )    // unmapped
            {
                if ( r_dcache_tlb_ins.read() ) 
                {
                    r_mmu_ietr             = MMU_READ_PT2_UNMAPPED;
                    r_mmu_ibvar            = r_dcache_tlb_vaddr.read();
                    r_icache_tlb_miss_req  = false;
                    r_icache_tlb_rsp_error = true;
                }
                else
                {
                    r_mmu_detr             = MMU_READ_PT2_UNMAPPED;
                    r_mmu_dbvar            = r_dcache_tlb_vaddr.read();
                    m_drsp.valid             = true;
                    m_drsp.error             = true;
                }
                r_dcache_fsm          = DCACHE_IDLE;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_PTE2_GET> HIT in dcache, but PTE unmapped"
              << " PADDR = " << std::hex << r_dcache_tlb_paddr.read()
              << " / PTE_FLAGS = " << pte_flags 
              << " / PTE_PPN = " << pte_ppn << std::endl;
}
#endif
            }
            else                // mapped : we must update the TLB
            {
                r_dcache_tlb_pte_flags  = pte_flags;
                r_dcache_tlb_pte_ppn    = pte_ppn;
                r_dcache_tlb_cache_way  = way;
                r_dcache_tlb_cache_set  = set;
                r_dcache_tlb_cache_word = word;
                r_dcache_fsm            = DCACHE_TLB_PTE2_SELECT;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_PTE2_GET> HIT in dcache:"
              << " PADDR = " << std::hex << r_dcache_tlb_paddr.read()
              << " / PTE_FLAGS = " << pte_flags 
              << " / PTE_PPN = " << pte_ppn << std::endl;
}
#endif
             }
        }
        else            // we must load the missing cache line in dcache
        {
            r_dcache_fsm          = DCACHE_MISS_VICTIM; 
            r_dcache_vci_miss_req = true;
            r_dcache_vci_paddr    = r_dcache_tlb_paddr.read();
            r_dcache_miss_type    = PTE2_MISS;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_PTE2_GET> MISS in dcache:"
              << " PADDR = " << std::hex << r_dcache_tlb_paddr.read() << std::endl;
}
#endif
        }
        break;
    }
    ////////////////////////////
    case DCACHE_TLB_PTE2_SELECT:    // select a slot for PTE2
    {

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
#endif

        size_t way;
        size_t set;

        if ( r_dcache_tlb_ins.read() )
        {
            r_itlb.select( r_dcache_tlb_vaddr.read(),
                           false,    // PTE2 
                           &way,
                           &set );
        }
        else
        {
            r_dtlb.select( r_dcache_tlb_vaddr.read(),
                           false,    // PTE2 
                           &way,
                           &set );
        }

#if DEBUG_DCACHE
if ( r_debug_active )
{
    if ( r_dcache_tlb_ins.read() ) 
        std::cout << name() << " <DCACHE_TLB_PTE2_SELECT> Select a slot in ITLB:";
    else                           
        std::cout << name() << " <DCACHE_TLB_PTE2_SELECT> Select a slot in DTLB:";
        std::cout << " way = " << std::dec << way
                  << " / set = " << set << std::endl;
}
#endif
        r_dcache_tlb_way = way;
        r_dcache_tlb_set = set;
        r_dcache_fsm     = DCACHE_TLB_PTE2_UPDT;
        break;
    }
    //////////////////////////
    case DCACHE_TLB_PTE2_UPDT:          // write a new PTE2 in tlb after testing the L/R bit
                        // if L/R bit already set, exit the sub-fsm
                                        // if not, the dcache entry is updated, and the
                                        // page table must be updated by an atomic access
    {

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
#endif

        paddr_t            nline     = r_dcache_tlb_paddr.read() >> (uint32_log2(m_dcache_words)+2);   
        uint32_t        pte_flags = r_dcache_tlb_pte_flags.read();
        uint32_t        pte_ppn   = r_dcache_tlb_pte_ppn.read();
        bool            updt      = false;
        bool            local     = true;

        // We should compute the access locality: 
        // The PPN MSB bits define the destination cluster index.
        // The m_srcid_d MSB bits define the source cluster index.
        // The number of bits to compare depends on the number of clusters, 
        // and can be obtained in the mapping table.
        // As this computation is not done yet, all access are marked as local.

        if ( local )                        // local access
        {
            if ( not ((pte_flags & PTE_L_MASK) == PTE_L_MASK) ) // we must set the L bit
            {
                updt = true;

                // request a SC transaction for page table update
                r_dcache_vci_sc_old    = pte_flags;
                r_dcache_vci_sc_new    = pte_flags | PTE_L_MASK;
                r_dcache_vci_paddr     = r_dcache_tlb_paddr.read();
                r_dcache_vci_sc_req    = true;

                // prepare both TLB and dcache update
                pte_flags = pte_flags | PTE_L_MASK; 

                // update dcache
                r_dcache.write( r_dcache_tlb_cache_way.read(),
                                r_dcache_tlb_cache_set.read(),
                                r_dcache_tlb_cache_word.read(),
                                pte_flags );
#if INSTRUMENTATION
r_cpt_dcache_write++;
#endif
            }
        }
        else                                                    // remote access
        {
            if ( not ((pte_flags & PTE_R_MASK) == PTE_R_MASK) ) // we must set the R bit
            {
                updt                   = true;

                // request a SC transaction for page table update
                r_dcache_vci_sc_old    = pte_flags;
                r_dcache_vci_sc_new    = pte_flags | PTE_R_MASK;
                r_dcache_vci_paddr     = r_dcache_tlb_paddr.read();
                r_dcache_vci_sc_req    = true;

                // prepare both TLB and dcache update
                pte_flags = pte_flags | PTE_R_MASK;

                // update dcache
                r_dcache.write( r_dcache_tlb_cache_way.read(),
                                r_dcache_tlb_cache_set.read(),
                                r_dcache_tlb_cache_word.read(),
                                pte_flags );
#if INSTRUMENTATION
r_cpt_dcache_write++;
#endif
            }
        }
        
        // update TLB for a PTE2
        if ( r_dcache_tlb_ins.read() )  
        {
            r_itlb.write( false,    // 4K page
                          pte_flags,
                          pte_ppn,
                          r_dcache_tlb_vaddr.read(),    
                          r_dcache_tlb_way.read(), 
                          r_dcache_tlb_set.read(),
                          nline );
#if INSTRUMENTATION
r_cpt_itlb_write++;
#endif
        }
        else
        {
            r_dtlb.write( false,    // 4K page
                          pte_flags,
                          pte_ppn,
                          r_dcache_tlb_vaddr.read(),    
                          r_dcache_tlb_way.read(), 
                          r_dcache_tlb_set.read(),
                          nline );
#if INSTRUMENTATION
r_cpt_dtlb_write++;
#endif
        }

#if DEBUG_DCACHE
if ( r_debug_active )
{
    if ( r_dcache_tlb_ins.read() ) 
    {
        std::cout << name() << " <DCACHE_TLB_PTE2_UPDT> write PTE2 in ITLB";
        std::cout << " / set = " << std::dec << r_dcache_tlb_set.read()
                  << " / way = " << r_dcache_tlb_way.read() << std::endl;
        r_itlb.printTrace();
    }
    else                           
    {
        std::cout << name() << " <DCACHE_TLB_PTE2_UPDT> write PTE2 in DTLB";
        std::cout << " / set = " << std::dec << r_dcache_tlb_set.read()
                  << " / way = " << r_dcache_tlb_way.read() << std::endl;
        r_dtlb.printTrace();
    }
    if ( updt )
    {
        std::cout << "                              Update (L/R) bit in dcache" << std::endl;
    }
}
#endif
        // next state
        if ( updt ) r_dcache_fsm = DCACHE_TLB_LR_WAIT;     // waiting response to SC
        else        r_dcache_fsm = DCACHE_TLB_RETURN;    // exit sub-fsm
        break;
    }
    ////////////////////////
    case DCACHE_TLB_LR_WAIT:        // Waiting a response to SC transaction for L/R update.
                                        // We consume the response in rsp FIFO, and we update
                    // the cache in case of success. 
                                        // We just return in case of failure, because we don't
                                        // care if the L/R bit update is not done.
                                        // We must take the coherence requests because
                                        // there is a risk of dead-lock
    {

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
r_cost_access_bit_updt_frz++;
#endif

        if ( r_vci_rsp_data_error.read() )     // bus error
        {
            std::cout << "BUS ERROR in DCACHE_TLB_LR_WAIT state" << std::endl;
            std::cout << "This should not happen in this state" << std::endl;
            exit(0);
        }
    else if ( r_vci_rsp_fifo_dcache.rok() ) // response available
    {
            if ( r_vci_rsp_fifo_dcache.read() == 0 )    // update dcache and dtlb if atomic
            {
                // update dcache
                r_dcache.write( r_dcache_tlb_cache_way.read(),
                                r_dcache_tlb_cache_set.read(),
                                r_dcache_tlb_cache_word.read(),
                                r_dcache_vci_sc_new.read() );
#if INSTRUMENTATION
r_cpt_dcache_write++;
#endif
 
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_LR_WAIT> L/R bit successfully set / dcache updated" 
              << std::endl;
}
#endif
            }
            else
            {
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_LR_WAIT> PTE modified / dcache not updated" 
              << std::endl;
}
#endif
            }
            vci_rsp_fifo_dcache_get = true;     
            r_dcache_fsm            = DCACHE_TLB_RETURN;
        }
        break;
    }
    ///////////////////////
    case DCACHE_TLB_RETURN:        // return to caller depending on tlb miss type
    {

#if INSTRUMENTATION
if ( not r_dcache_tlb_ins ) r_cost_dtlb_miss_frz++;
#endif

        if ( r_dcache_tlb_ins.read() ) r_icache_tlb_miss_req = false;
        r_dcache_fsm = DCACHE_IDLE;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_TLB_RETURN> TLB MISS completed" << std::endl;
}
#endif
        break;
    }
    ///////////////////////
    case DCACHE_XTN_SWITCH:        // Both itlb and dtlb must be flushed
                                        // LL reservation must be invalidated...
    {
        if ( not r_dcache_xtn_req.read() )
        {
            r_dcache_ll_valid = false;
            r_dtlb.flush();
            r_dcache_fsm = DCACHE_IDLE;
            m_drsp.valid = true;
        }
        break;
    }
    /////////////////////
    case DCACHE_XTN_SYNC:        // waiting until write buffer empty
    {
        if ( r_wbuf.empty() )
        {
            m_drsp.valid   = true;
            r_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    ////////////////////////
    case DCACHE_XTN_IC_FLUSH:        // Waiting completion of an XTN request to the ICACHE FSM
    case DCACHE_XTN_IC_INVAL_VA:    // Caution : the itlb miss requests must be taken 
    case DCACHE_XTN_IC_INVAL_PA:    // because the XTN_ICACHE_INVAL request to icache
    case DCACHE_XTN_IT_INVAL:        // can generate an itlb miss...
    {
        // itlb miss request
        if ( r_icache_tlb_miss_req.read() )
        {
            r_dcache_tlb_ins    = true;
            r_dcache_tlb_vaddr  = r_icache_vaddr_save.read();
            r_dcache_fsm        = DCACHE_TLB_MISS;
            break;
        }

        // test if XTN request to icache completed
        if ( not r_dcache_xtn_req.read() ) 
        {
            r_dcache_fsm = DCACHE_IDLE;
            m_drsp.valid = true;
        }
        break;
    }
    /////////////////////////
    case DCACHE_XTN_DC_FLUSH:    // Invalidate sequencially all cache lines, using
                                // the r_dcache_flush counter as a slot counter.
                                // We loop in this state until all slots have been visited.
                            // Finally, both the itlb and dtlb are flushed
                                // (including global entries)

    {
        paddr_t nline;    // unused
        size_t    way = r_dcache_flush_count.read()/m_icache_sets;
        size_t    set = r_dcache_flush_count.read()%m_icache_sets;

        r_dcache.inval( way,
                        set,
                        &nline );

        r_dcache_flush_count = r_dcache_flush_count.read() + 1;

        if ( r_dcache_flush_count.read() == (m_dcache_sets*m_dcache_ways - 1) )    // last 
        {
            r_dtlb.reset();
            r_itlb.reset();
            r_dcache_fsm = DCACHE_IDLE;
            m_drsp.valid = true;
        }
    break;
    }
    /////////////////////////
    case DCACHE_XTN_DT_INVAL:     // handling processor XTN_DTLB_INVAL request
    {
        r_dtlb.inval(r_dcache_p0_wdata.read());
        r_dcache_fsm        = DCACHE_IDLE;
        m_drsp.valid          = true;
        break;
    }
    ////////////////////////////
    case DCACHE_XTN_DC_INVAL_VA:  // selective cache line invalidate with virtual address
                                  // requires 3 cycles: access tlb, read cache, inval cache
                                     // we compute the physical address in this state 
    {
        paddr_t paddr;
        bool    hit;

        if ( r_mmu_mode.read() & DATA_TLB_MASK )     // dtlb activated
        {
#if INSTRUMENTATION
r_cpt_dtlb_read++;
#endif
            hit = r_dtlb.translate( r_dcache_p0_wdata.read(),
                                    &paddr ); 
        }
        else                         // dtlb not activated
        {
            paddr = (paddr_t)r_dcache_p0_wdata.read();
            hit   = true;
        }

        if ( hit )        // tlb hit
        {
            r_dcache_p0_paddr = paddr;
            r_dcache_fsm      = DCACHE_XTN_DC_INVAL_PA;
        }
        else            // tlb miss
           {
#if INSTRUMENTATION
r_cpt_dtlb_miss++;
#endif
            r_dcache_tlb_ins    = false;        // dtlb
            r_dcache_tlb_vaddr  = r_dcache_p0_wdata.read();
            r_dcache_fsm        = DCACHE_TLB_MISS; 
        } 
  
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_XTN_DC_INVAL_VA> Compute physical address" << std::hex
              << " / VADDR = " << r_dcache_p0_wdata.read()
              << " / PADDR = " << paddr << std::endl;
}
#endif

        break;
    }
    ////////////////////////////
    case DCACHE_XTN_DC_INVAL_PA:  // selective cache line invalidate with physical address
                                  // requires 2 cycles: read cache / inval cache
                                  // In this state we read dcache.
    {
        uint32_t    data;
        size_t        way;
        size_t        set;
        size_t        word;

#if INSTRUMENTATION
r_cpt_dcache_read++;
#endif
        bool        hit = r_dcache.read( r_dcache_p0_paddr.read(),
                                             &data,
                                             &way,
                                             &set,
                                             &word );
        if ( hit )    // inval to be done
        {
            r_dcache_xtn_way = way;
            r_dcache_xtn_set = set;
            r_dcache_fsm      = DCACHE_XTN_DC_INVAL_GO;
        }
        else        // miss : nothing to do
        {
            r_dcache_fsm      = DCACHE_IDLE;
            m_drsp.valid        = true;
        }

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_XTN_DC_INVAL_PA> Test hit in dcache" << std::hex
              << " / PADDR = " << r_dcache_p0_paddr.read() << std::dec
              << " / HIT = " << hit
              << " / SET = " << set
              << " / WAY = " << way << std::endl;
}
#endif
        break;
    }
    ////////////////////////////
    case DCACHE_XTN_DC_INVAL_GO:  // In this state, we invalidate the cache line 
    {
        paddr_t    nline;
        size_t    way        = r_dcache_xtn_way.read();
        size_t    set        = r_dcache_xtn_set.read();
        bool hit;
   
        hit = r_dcache.inval( way,
                              set,
                              &nline );

        assert(hit && "XTN_DC_INVAL way/set should still be in cache");

        r_dcache_fsm = DCACHE_IDLE;
        m_drsp.valid = true;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_XTN_DC_INVAL_GO> Actual dcache inval" << std::hex
              << " / NLINE = " << nline << std::endl;
}
#endif
        break;
    }
    //////////////////////////////
    case DCACHE_XTN_DC_INVAL_END:      // send response to processor XTN request
    {
        r_dcache_fsm = DCACHE_IDLE;
        m_drsp.valid = true;
        break;
    }
    ////////////////////////
    case DCACHE_MISS_VICTIM:        // Selects a victim line
    {
#if INSTRUMENTATION
r_cost_dcache_miss_frz++;
#endif
        bool      valid;
        size_t    way;
        size_t    set;
        paddr_t   victim;    // unused

        valid = r_dcache.victim_select( r_dcache_vci_paddr.read(),
                                        &victim,
                                        &way,
                                        &set );
        r_dcache_miss_way = way;
        r_dcache_miss_set = set;

        if ( valid )  r_dcache_fsm = DCACHE_MISS_INVAL; 
        else          r_dcache_fsm = DCACHE_MISS_WAIT; 

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_MISS_VICTIM> Select a slot:" << std::dec
              << " / WAY = "   << way 
              << " / SET = "   << set << std::endl; 
}
#endif
        break;
    }
    ///////////////////////
    case DCACHE_MISS_INVAL:        // invalidate the victim line
    {
        paddr_t    nline;
        size_t    way        = r_dcache_miss_way.read();
        size_t    set        = r_dcache_miss_set.read();
    bool hit;

        hit = r_dcache.inval( way, 
                        set,
                        &nline );

    assert(hit && "selected way/set line should be in dcache");

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_MISS_INVAL> inval line:"
              << " / WAY = "   << way 
              << " / SET = "   << set
              << " / NLINE = "  << std::hex << nline << std::endl; 
}
#endif
        r_dcache_fsm = DCACHE_MISS_WAIT;
        break;
    }
    //////////////////////
    case DCACHE_MISS_WAIT:    // waiting the response to a miss request from VCI_RSP FSM
                                // This state is in charge of error signaling
                                // There is 5 types of error depending on the requester
    {
#if INSTRUMENTATION
r_cost_dcache_miss_frz++;
#endif
        if ( r_vci_rsp_data_error.read() )             // bus error
        {
            switch ( r_dcache_miss_type.read() )
            {
                case PROC_MISS:  
                case DIRTY_MISS:  
                {
                    r_mmu_detr            = MMU_READ_DATA_ILLEGAL_ACCESS; 
                    r_mmu_dbvar           = r_dcache_p0_vaddr.read();
                    m_drsp.valid            = true;
                    m_drsp.error            = true;
                    r_dcache_fsm          = DCACHE_IDLE;
                    break;
                }
                case PTE1_MISS:
                {
                    if ( r_dcache_tlb_ins.read() )
                    {
                        r_mmu_ietr              = MMU_READ_PT1_ILLEGAL_ACCESS;
                        r_mmu_ibvar             = r_dcache_tlb_vaddr.read();
                        r_icache_tlb_miss_req   = false;
                        r_icache_tlb_rsp_error  = true;
                    }
                    else
                    {
                        r_mmu_detr              = MMU_READ_PT1_ILLEGAL_ACCESS;
                        r_mmu_dbvar             = r_dcache_tlb_vaddr.read();
                        m_drsp.valid              = true;
                        m_drsp.error              = true;
                    }
                    r_dcache_fsm                = DCACHE_IDLE;
                    break;
                }
                case PTE2_MISS: 
                {
                    if ( r_dcache_tlb_ins.read() )
                    {
                        r_mmu_ietr              = MMU_READ_PT2_ILLEGAL_ACCESS;
                        r_mmu_ibvar             = r_dcache_tlb_vaddr.read();
                        r_icache_tlb_miss_req   = false;
                        r_icache_tlb_rsp_error  = true;
                    }
                    else
                    {
                        r_mmu_detr              = MMU_READ_PT2_ILLEGAL_ACCESS;
                        r_mmu_dbvar             = r_dcache_tlb_vaddr.read();
                        m_drsp.valid              = true;
                        m_drsp.error              = true;
                    }
                    r_dcache_fsm                = DCACHE_IDLE;
                    break;
                }
            } // end switch type
            r_vci_rsp_data_error = false;
        }
        else if ( r_vci_rsp_fifo_dcache.rok() )        // valid response available
        {
            r_dcache_miss_word = 0;
        r_dcache_fsm       = DCACHE_MISS_UPDT;
        }    
        break;
    }
    //////////////////////
    case DCACHE_MISS_UPDT:    // update the dcache (one word per cycle)
                                // returns the response depending on the miss type
    {

#if INSTRUMENTATION
r_cost_dcache_miss_frz++;
#endif

        if ( r_vci_rsp_fifo_dcache.rok() )    // one word available
        {
            size_t way  = r_dcache_miss_way.read();
            size_t set  = r_dcache_miss_set.read();
            size_t word = r_dcache_miss_word.read();

#if INSTRUMENTATION
r_cpt_dcache_write++;
#endif
            r_dcache.write( way,
                            set,
                            word,
                            r_vci_rsp_fifo_dcache.read());

            vci_rsp_fifo_dcache_get = true;
            r_dcache_miss_word = r_dcache_miss_word.read() + 1;
               
            // if last word, update directory
            if ( r_dcache_miss_word.read() == (m_dcache_words - 1) ) 
            {
                r_dcache.victim_update_tag( r_dcache_vci_paddr.read(),
                                            r_dcache_miss_way.read(),
                                            r_dcache_miss_set.read() );

                if      (r_dcache_miss_type.read()==PTE1_MISS) r_dcache_fsm = DCACHE_TLB_PTE1_GET; 
                else if (r_dcache_miss_type.read()==PTE2_MISS) r_dcache_fsm = DCACHE_TLB_PTE2_GET;
                else if (r_dcache_miss_type.read()==PROC_MISS) r_dcache_fsm = DCACHE_IDLE;
                else                                           r_dcache_fsm = DCACHE_DIRTY_GET_PTE;
            }

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_MISS_UPDT> Write one word" << std::hex
              << " / PADDR = " << r_dcache_vci_paddr.read() 
              << " / DATA = "  << r_vci_rsp_fifo_dcache.read() << std::dec
              << " / WAY = "   << r_dcache_miss_way.read() 
              << " / SET = "   << r_dcache_miss_set.read()
              << " / WORD = "  << r_dcache_miss_word.read() << std::endl; 
}
#endif
  
        } // end if rok
        break;
    }
    /////////////////////
    case DCACHE_UNC_WAIT:
    {

#if INSTRUMENTATION
r_cost_dunc_frz++;
#endif

        if ( r_vci_rsp_data_error.read() )     // bus error
        {
            r_mmu_detr           = MMU_READ_DATA_ILLEGAL_ACCESS; 
            r_mmu_dbvar          = m_dreq.addr;
            r_vci_rsp_data_error = false;
            m_drsp.error           = true;
            m_drsp.valid           = true;
            r_dcache_fsm         = DCACHE_IDLE;
            break;
        }
        else if ( r_vci_rsp_fifo_dcache.rok() )     // data available
        {
            vci_rsp_fifo_dcache_get = true;     
            r_dcache_fsm            = DCACHE_IDLE;

            // we acknowledge the processor request if it has not been modified
            if ( m_dreq.valid and (m_dreq.addr == r_dcache_p0_vaddr.read()) )
            {

#if INSTRUMENTATION
r_cpt_read++;
r_cpt_read_uncacheable++;
#endif
            m_drsp.valid          = true;
            m_drsp.rdata          = r_vci_rsp_fifo_dcache.read();
                // makes reservation in case of LL
                if ( m_dreq.type == iss_t::DATA_LL )
                {
                    r_dcache_ll_valid = true;
                    r_dcache_ll_data  = r_vci_rsp_fifo_dcache.read();
                    r_dcache_ll_vaddr = m_dreq.addr;
                }
            }

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_UNC_WAIT> Receive a response" << std::hex
              << " / PADDR = " << r_dcache_vci_paddr.read() 
              << " / DATA = "  << r_vci_rsp_fifo_dcache.read() << std::endl; 
}
#endif
  
        }    
        break;
    }
    ////////////////////
    case DCACHE_SC_WAIT:    // waiting VCI response after a processor SC request
    {

#if INSTRUMENTATION
r_cost_sc_frz++;
#endif

        if ( r_vci_rsp_data_error.read() )         // bus error
        {
            r_mmu_detr           = MMU_READ_DATA_ILLEGAL_ACCESS; 
            r_mmu_dbvar          = m_dreq.addr;
            r_vci_rsp_data_error = false;
            m_drsp.error         = true;
            m_drsp.valid         = true;
            r_dcache_fsm         = DCACHE_IDLE;
            break;
        }
    else if ( r_vci_rsp_fifo_dcache.rok() )         // response available
    {

            if(r_dcache_sc_hit && (r_vci_rsp_fifo_dcache.read() == 0))
	        {
                	r_dcache.write( r_dcache_sc_way,
                                    r_dcache_sc_set,
                                    r_dcache_sc_word,
                                    r_dcache_vci_sc_new.read() );
	        }

#if INSTRUMENTATION
r_cpt_sc++;
#endif
            vci_rsp_fifo_dcache_get = true;     
        m_drsp.valid            = true;
        m_drsp.rdata            = r_vci_rsp_fifo_dcache.read();
            r_dcache_fsm            = DCACHE_IDLE;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_SC_WAIT> Receive a response" << std::hex
              << " / PADDR = " << r_dcache_vci_paddr.read() 
              << " / DATA = "  << r_vci_rsp_fifo_dcache.read() << std::endl; 
}
#endif
    }    
        break;
    }
    //////////////////////////
    case DCACHE_DIRTY_GET_PTE:        // This sub_fsm set the PTE Dirty bit in memory 
                                        // before handling a processor WRITE or SC request  
                    // Input argument is r_dcache_dirty_paddr
                                        // In this first state, we get PTE value in dcache
                                        // and post a SC request to CMD FSM
    {

#if INSTRUMENTATION
r_cost_dirty_bit_updt_frz++;
#endif

        // get PTE in dcache
        uint32_t pte;
        size_t   way;
        size_t   set;
        size_t   word;    // unused

#if INSTRUMENTATION
r_cpt_dcache_read++;
#endif
        bool     hit = r_dcache.read( r_dcache_dirty_paddr.read(),
                                      &pte,
                                      &way,
                                      &set,
                                      &word );

    if ( hit )    // request sc transaction to CMD_FSM
        {
            r_dcache_dirty_way  = way; 
            r_dcache_dirty_set  = set; 
            r_dcache_dirty_word = word; 
            r_dcache_vci_sc_req = true;
            r_dcache_vci_paddr  = r_dcache_dirty_paddr.read();
            r_dcache_vci_sc_old = pte;
            r_dcache_vci_sc_new = pte | PTE_D_MASK;
            r_dcache_fsm        = DCACHE_DIRTY_SC_WAIT;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_DIRTY_GET_PTE> Get PTE in dcache" << std::hex
              << " / PTE_PADDR = " << r_dcache_dirty_paddr.read() 
              << " / PTE_VALUE = " << pte << std::dec 
              << " / CACHE_SET = " << set
              << " / CACHE_WAY = " << way << std::endl;
}
#endif
        }
        else        // request the missing line
        {
            r_dcache_vci_paddr  = r_dcache_dirty_paddr.read();
            r_dcache_vci_miss_req = true;
            r_dcache_miss_type    = DIRTY_MISS;
            r_dcache_fsm          = DCACHE_MISS_VICTIM;

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_DIRTY_GET_PTE> Miss PTE in dcache" << std::hex
              << " / PTE_PADDR = " << r_dcache_dirty_paddr.read() << std::endl;
}
#endif
        }
        break;
    }
    //////////////////////////
    case DCACHE_DIRTY_SC_WAIT:        // wait completion of SC for Dirty bit update
                                        // If PTE update is a success, we update both
                                        // DCACHE and DTLB, and return to IDLE state.
                                        // If PTE update is a failure, we invalidate the
                                        // cache line in DCACHE and flush ITLB and DTLB.
    {
#if INSTRUMENTATION
r_cost_dirty_bit_updt_frz++;
#endif
        if ( r_vci_rsp_data_error.read() )    // bus error
        {
            std::cout << "BUS ERROR in DCACHE_DIRTY_SC_WAIT state" << std::endl;
            std::cout << "This should not happen in this state" << std::endl;
            exit(0);
        }
        else if ( r_vci_rsp_fifo_dcache.rok() )    // response available
        {
            vci_rsp_fifo_dcache_get = true;
            if ( r_vci_rsp_fifo_dcache.read() == 0 )    // update dcache and dtlb if atomic
            {
                // update dcache
                r_dcache.write( r_dcache_dirty_way.read(),
                                r_dcache_dirty_set.read(),
                                r_dcache_dirty_word.read(),
                                r_dcache_vci_sc_new.read() );
#if INSTRUMENTATION
r_cpt_dcache_write++;
#endif
                // update dtlb 
                r_dtlb.set_dirty( r_dcache_tlb_way.read(),
                                  r_dcache_tlb_set.read() );
#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_DIRTY_SC_WAIT> Dirty bit successfully set"
              << " / dcache and dtlb updated" << std::endl
              << "      TLB_SET = " << std::dec << r_dcache_tlb_set.read()
              << " / TLB_WAY = " << r_dcache_tlb_way.read()
              << " / CACHE_WAY = " << r_dcache_dirty_way.read()
              << " / CACHE_SET = " << r_dcache_dirty_set.read()
              << " / CACHE_WORD = " << r_dcache_dirty_word.read()
              << " / PTE_FLAGS = " << std::hex << r_dcache_vci_sc_new.read() << std::endl;
    r_dtlb.printTrace();
}
#endif
            }
            else                    // invalidate cache line and flush TLBs
            {
                paddr_t    nline;    
                r_dcache.inval( r_dcache_dirty_way.read(),
                                r_dcache_dirty_set.read(),
                                &nline );

                r_itlb.reset();
                r_dtlb.reset();

#if DEBUG_DCACHE
if ( r_debug_active )
{
    std::cout << name() << " <DCACHE_DIRTY_SC_WAIT> PTE modified : Inval cache line & flush TLBs"
              << " / PADDR = " << std::hex << (nline*m_dcache_words*4) << std::endl;
}
#endif
            }
            r_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    } // end switch r_dcache_fsm

    ///////////////// wbuf update //////////////////////////////////////////////////////
    r_wbuf.update();

    //////////////// test processor frozen /////////////////////////////////////////////
    // The simulation exit if the number of consecutive frozen cycles
    // is larger than the m_max_frozen_cycles (constructor parameter)
    if ( (m_ireq.valid and not m_irsp.valid) or (m_dreq.valid and not m_drsp.valid) )       
    {
        r_cpt_frz_cycles++;         // used for instrumentation
        r_cpt_stop_simulation++;    // used for debug
        if ( r_cpt_stop_simulation > m_max_frozen_cycles )
        {
            std::cout << std::dec << "\nERROR in VCI_VCACHE_WRAPPER " << name() << std::endl
                      << " stop at cycle " << r_cpt_total_cycles
                      << " frozen since cycle " << r_cpt_total_cycles - m_max_frozen_cycles 
                      << std::endl;
            exit(1);
        }
    }
    else
    {
        r_cpt_stop_simulation = 0;
    }

    /////////// execute one iss cycle /////////////////////////////////
    uint32_t it = 0;
    for (size_t i=0; i<(size_t)iss_t::n_irq; i++) 
    {
        if ( p_irq[i].read() ) it |= (1<<i);
    }
    r_iss.executeNCycles(1, m_irsp, m_drsp, it);

    if ( (m_ireq.valid and m_irsp.valid) and 
         (!m_dreq.valid or m_drsp.valid) and 
         (m_ireq.addr != r_pc_previous) )
    {
        r_cpt_exec_ins++;
        r_pc_previous = m_ireq.addr;
    }

    ////////////////////////////////////////////////////////////////////////////
    // This FSM handles requests from both the DCACHE FSM & the ICACHE FSM.
    // There is 6 request types, with the following priorities : 
    // 1 - Data Read Miss         : r_dcache_vci_miss_req and miss in the write buffer
    // 2 - Data Read Uncachable   : r_dcache_vci_unc_req and miss in the write buffer 
    // 3 - Instruction Miss       : r_icache_miss_req and miss in the write buffer
    // 4 - Instruction Uncachable : r_icache_unc_req 
    // 5 - Data Write             : r_wbuf.rok()      
    // 6 - Data Store Conditionnal: r_dcache_vci_sc_req
    //
    // As we want to support several simultaneous VCI transactions, the VCI_CMD_FSM 
    // and the VCI_RSP_FSM are fully desynchronized.
    //
    // VCI formats:
    // According to the VCI advanced specification, all read requests packets 
    // (data Uncached, Miss data, instruction Uncached, Miss instruction) 
    // are one word packets.
    // For write burst packets, all words are in the same cache line,
    // and addresses must be contiguous (the BE field is 0 in case of "holes").
    // The sc command packet implements actually a compare-and-swap mechanism
    // and the packet contains two flits.
    ////////////////////////////////////////////////////////////////////////////////////

    switch ( r_vci_cmd_fsm.read() ) 
    {
        //////////////
        case CMD_IDLE:
        {
            // r_dcache_vci_miss_req and r_icache_miss_req require both a write_buffer access 
            // to check a possible pending write on the same cache line. 
            // As there is only one possible access per cycle to write buffer, we implement 
            // a round-robin priority for this access, using the r_vci_cmd_imiss_prio flip-flop.

            size_t      wbuf_min;
            size_t      wbuf_max;

            bool dcache_miss_req = r_dcache_vci_miss_req.read()
                 and ( not r_icache_miss_req.read() or not r_vci_cmd_imiss_prio.read() );

            bool icache_miss_req = r_icache_miss_req.read()
                 and ( not r_dcache_vci_miss_req.read() or r_vci_cmd_imiss_prio.read() );

            // 1 - Data Read Miss
            if ( dcache_miss_req and r_wbuf.miss(r_dcache_vci_paddr.read()) )
            {
                r_vci_cmd_fsm         = CMD_DATA_MISS;
                r_dcache_vci_miss_req = false;
                r_vci_cmd_imiss_prio  = true;

#if INSTRUMENTATION
r_cpt_dmiss_transaction++;
#endif
            }
            // 2 - Data Read Uncachable
            else if ( r_dcache_vci_unc_req.read() and r_wbuf.miss(r_dcache_vci_paddr.read()))
            {
                r_vci_cmd_fsm        = CMD_DATA_UNC;
                r_dcache_vci_unc_req = false;

#if INSTRUMENTATION
r_cpt_dunc_transaction++;
#endif
            }
            // 3 - Instruction Miss
            else if ( icache_miss_req and r_wbuf.miss(r_icache_vci_paddr.read()) )
            {
                r_vci_cmd_fsm        = CMD_INS_MISS;
                r_icache_miss_req    = false;
                r_vci_cmd_imiss_prio = false;

#if INSTUMENTATION
r_cpt_imiss_transaction++;
#endif
            }
            // 4 - Instruction Uncachable
            else if ( r_icache_unc_req.read() )
            {
                r_vci_cmd_fsm    = CMD_INS_UNC;
                r_icache_unc_req = false;

#if INSTRUMENTATION
r_cpt_iunc_transaction++;
#endif
            }
            // 5 - Data Write
            else if ( r_wbuf.rok(&wbuf_min, &wbuf_max) )
            {
                r_vci_cmd_fsm       = CMD_DATA_WRITE;
                r_vci_cmd_cpt       = wbuf_min;
                r_vci_cmd_min       = wbuf_min;
                r_vci_cmd_max       = wbuf_max;

#if INSTRUMENTATION
r_cpt_write_transaction++;
r_length_write_transaction += (wbuf_max-wbuf_min+1);
#endif
            }
            // 6 - Data Store Conditionnal
            else if ( r_dcache_vci_sc_req.read() )
            {
                r_vci_cmd_fsm       = CMD_DATA_SC;
                r_dcache_vci_sc_req = false;
                r_vci_cmd_cpt       = 0;

#if INSTRUMENTATION
r_cpt_sc_transaction++;
#endif
            }
            break;
        }
        ////////////////////
        case CMD_DATA_WRITE:
        {
            if ( p_vci.cmdack.read() )
            {

#if INSTRUMENTATION
r_cpt_wbuf_read++;
#endif
                r_vci_cmd_cpt = r_vci_cmd_cpt + 1;
                if (r_vci_cmd_cpt == r_vci_cmd_max) // last flit sent
                {
                    r_vci_cmd_fsm = CMD_IDLE ;
                    r_wbuf.sent() ;
                }
            }
            break;
        }
        /////////////////
        case CMD_DATA_SC:
        {
            // The SC VCI command contains two flits
            if ( p_vci.cmdack.read() )
            {
               r_vci_cmd_cpt = r_vci_cmd_cpt + 1;
               if (r_vci_cmd_cpt == 1) r_vci_cmd_fsm = CMD_IDLE ;
            }
            break;
        }
        //////////////////
        case CMD_INS_MISS:
        case CMD_INS_UNC:
        case CMD_DATA_MISS:
        case CMD_DATA_UNC:
        {
            // all read VCI commands contain one single flit
            if ( p_vci.cmdack.read() )  r_vci_cmd_fsm = CMD_IDLE;
            break;
        }

    } // end  switch r_vci_cmd_fsm

    //////////////////////////////////////////////////////////////////////////
    // The VCI_RSP FSM controls the following ressources:
    // - r_vci_rsp_fsm:
    // - r_vci_rsp_fifo_icache (push)
    // - r_vci_rsp_fifo_dcache (push)
    // - r_vci_rsp_data_error (set)
    // - r_vci_rsp_ins_error (set)
    // - r_vci_rsp_cpt
    //
    // As the VCI_RSP and VCI_CMD are fully desynchronized to support several
    // simultaneous VCI transactions, this FSM uses the VCI TRDID field 
    // to identify the transactions.
    //
    // VCI vormat:
    // This component checks the response packet length and accepts only
    // single word packets for write response packets. 
    //
    // Error handling:
    // This FSM analyzes the VCI error code and signals directly the Write Bus Error. 
    // In case of Read Data Error, the VCI_RSP FSM sets the r_vci_rsp_data_error 
    // flip_flop and the error is signaled by the DCACHE FSM.  
    // In case of Instruction Error, the VCI_RSP FSM sets the r_vci_rsp_ins_error 
    // flip_flop and the error is signaled by the ICACHE FSM.  
    // In case of Cleanup Error, the simulation stops with an error message...
    //////////////////////////////////////////////////////////////////////////

    switch ( r_vci_rsp_fsm.read() ) 
    {
    //////////////
    case RSP_IDLE:
    {
        if ( p_vci.rspval.read() )
        {
            r_vci_rsp_cpt = 0;

            if ( (p_vci.rtrdid.read() >> (vci_param::T-1)) != 0 ) // Write transaction
            {
                r_vci_rsp_fsm = RSP_DATA_WRITE;
            }
            else if ( p_vci.rtrdid.read() == TYPE_INS_MISS )
            {
                r_vci_rsp_fsm = RSP_INS_MISS;
            }
            else if ( p_vci.rtrdid.read() == TYPE_INS_UNC )
            {
                r_vci_rsp_fsm = RSP_INS_UNC;
            }
            else if ( p_vci.rtrdid.read() == TYPE_DATA_MISS )
            {
                r_vci_rsp_fsm = RSP_DATA_MISS;
            }
            else if ( p_vci.rtrdid.read() == TYPE_DATA_UNC )
            {
                r_vci_rsp_fsm = RSP_DATA_UNC;
            }
            else
            {
                assert(false and "Unexpected VCI response");
            }
        }
        break;
    }
        //////////////////
        case RSP_INS_MISS:
        {
            if ( p_vci.rspval.read() )
            {
                if ( (p_vci.rerror.read()&0x1) != 0 )  // error reported
                {
                    r_vci_rsp_ins_error = true;
                    if ( p_vci.reop.read() ) r_vci_rsp_fsm = RSP_IDLE;
                }
                else                                        // no error reported
                {
                    if ( r_vci_rsp_fifo_icache.wok() )
                    {
                        assert( (r_vci_rsp_cpt.read() < m_icache_words) and
                        "The VCI response packet for instruction miss is too long" );

                        r_vci_rsp_cpt                 = r_vci_rsp_cpt.read() + 1;
                        vci_rsp_fifo_icache_put       = true,
                        vci_rsp_fifo_icache_data      = p_vci.rdata.read();
                        if ( p_vci.reop.read() )
                        {
                            assert( (r_vci_rsp_cpt.read() == m_icache_words - 1) and
                            "The VCI response packet for instruction miss is too short");

                            r_vci_rsp_fsm    = RSP_IDLE;
                        }
                    }
                }
            }
            break;
        }
        /////////////////
        case RSP_INS_UNC:
        {
            if (p_vci.rspval.read() )
            {
                assert( p_vci.reop.read() and
                "illegal VCI response packet for uncachable instruction");

                if ( (p_vci.rerror.read()&0x1) != 0 )  // error reported
                {
                    r_vci_rsp_ins_error = true;
                    r_vci_rsp_fsm = RSP_IDLE;
                }
                else                                         // no error reported
                {
                    if ( r_vci_rsp_fifo_icache.wok())
                    {
                        vci_rsp_fifo_icache_put       = true;
                        vci_rsp_fifo_icache_data      = p_vci.rdata.read();
                        r_vci_rsp_fsm = RSP_IDLE;
                    }
                }
            }
            break;
        }
        ///////////////////
        case RSP_DATA_MISS:
        {
            if ( p_vci.rspval.read() )
            {
                if ( (p_vci.rerror.read()&0x1) != 0 )  // error reported
                {
                    r_vci_rsp_data_error = true;
                    if ( p_vci.reop.read() ) r_vci_rsp_fsm = RSP_IDLE;
                }
                else                                        // no error reported
                {
                    if ( r_vci_rsp_fifo_dcache.wok() )
                    {
                        assert( (r_vci_rsp_cpt.read() < m_dcache_words) and
                        "The VCI response packet for data miss is too long");

                        r_vci_rsp_cpt                 = r_vci_rsp_cpt.read() + 1;
                        vci_rsp_fifo_dcache_put       = true,
                        vci_rsp_fifo_dcache_data      = p_vci.rdata.read();
                        if ( p_vci.reop.read() )
                        {
                            assert( (r_vci_rsp_cpt.read() == m_dcache_words - 1) and
                            "The VCI response packet for data miss is too short");

                            r_vci_rsp_fsm     = RSP_IDLE;
                        }
                    }
                }
            }
            break;
        }
        //////////////////
        case RSP_DATA_UNC:
        {
            if (p_vci.rspval.read() )
            {
                assert( p_vci.reop.read() and
                "illegal VCI response packet for uncachable read data");

                if ( (p_vci.rerror.read()&0x1) != 0 )  // error reported
                {
                    r_vci_rsp_data_error = true;
                    r_vci_rsp_fsm = RSP_IDLE;
                }
                else                                         // no error reported
                {
                    if ( r_vci_rsp_fifo_dcache.wok())
                    {
                        vci_rsp_fifo_dcache_put       = true;
                        vci_rsp_fifo_dcache_data      = p_vci.rdata.read();
                        r_vci_rsp_fsm = RSP_IDLE;
                    }
                }
            }
            break;
        }
        ////////////////////
        case RSP_DATA_WRITE:
        {
            if (p_vci.rspval.read())
            {
                assert( p_vci.reop.read() and
                "a VCI response packet must contain one flit for a write transaction");

                r_vci_rsp_fsm = RSP_IDLE;
                uint32_t   wbuf_index = p_vci.rtrdid.read() - (1<<(vci_param::T-1));
                bool       cacheable  = r_wbuf.completed(wbuf_index);
                if ( not cacheable ) r_dcache_pending_unc_write = false;
                if ( (p_vci.rerror.read()&0x1) != 0 ) r_iss.setWriteBerr();
            }
            break;
        }
    } // end switch r_vci_rsp_fsm

    ///////////////// Response FIFOs update  //////////////////////
    r_vci_rsp_fifo_icache.update(vci_rsp_fifo_icache_get,
                                 vci_rsp_fifo_icache_put,
                                 vci_rsp_fifo_icache_data);

    r_vci_rsp_fifo_dcache.update(vci_rsp_fifo_dcache_get,
                                 vci_rsp_fifo_dcache_put,
                                 vci_rsp_fifo_dcache_data);
} // end transition()

///////////////////////
tmpl(void)::genMoore()
///////////////////////
{
    /////////////////////////////////////////////////////////////////
    // VCI initiator command on the direct network
    // it depends on the CMD FSM state

    p_vci.pktid  = 0;
    p_vci.srcid  = m_srcid;
    p_vci.cons   = (r_vci_cmd_fsm.read() == CMD_DATA_SC);
    p_vci.contig = not (r_vci_cmd_fsm.read() == CMD_DATA_SC);
    p_vci.wrap   = false;
    p_vci.clen   = 0;
    p_vci.cfixed = false;

    switch ( r_vci_cmd_fsm.read() ) {

    case CMD_IDLE:
        p_vci.cmdval  = false;
        p_vci.address = 0;
        p_vci.wdata   = 0;
        p_vci.be      = 0;
        p_vci.trdid   = 0;
        p_vci.plen    = 0;
        p_vci.cmd     = vci_param::CMD_NOP;
        p_vci.eop     = false;
        break;

    case CMD_INS_MISS:
        p_vci.cmdval  = true;
        p_vci.address = r_icache_vci_paddr.read() & m_icache_yzmask;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = TYPE_INS_MISS;
        p_vci.plen    = m_icache_words<<2;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    case CMD_INS_UNC:
        p_vci.cmdval  = true;
        p_vci.address = r_icache_vci_paddr.read() & ~0x3;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = TYPE_INS_UNC;
        p_vci.plen    = 4;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    case CMD_DATA_MISS:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_vci_paddr.read() & m_dcache_yzmask;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = TYPE_DATA_MISS;
        p_vci.plen    = m_dcache_words << 2;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    case CMD_DATA_UNC:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_vci_paddr.read() & ~0x3;
        p_vci.wdata   = 0;
        p_vci.be      = r_dcache_vci_unc_be.read();
        p_vci.trdid   = TYPE_DATA_UNC;
        p_vci.plen    = soclib::common::fls(r_dcache_vci_unc_be.read())
                        - ffs(r_dcache_vci_unc_be.read()) + 1;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    case CMD_DATA_WRITE:
        p_vci.cmdval  = true;
        p_vci.address = r_wbuf.getAddress(r_vci_cmd_cpt.read()) & ~0x3;
        p_vci.wdata   = r_wbuf.getData(r_vci_cmd_cpt.read());
        p_vci.be      = r_wbuf.getBe(r_vci_cmd_cpt.read());
        p_vci.trdid   = r_wbuf.getIndex() + (1<<(vci_param::T-1));
        p_vci.plen    = soclib::common::fls(r_wbuf.getBe(r_vci_cmd_max))
                        - ffs(r_wbuf.getBe(r_vci_cmd_min)) + 1
                        + (r_vci_cmd_max.read() - r_vci_cmd_min.read()) * vci_param::B;
        p_vci.cmd     = vci_param::CMD_WRITE;
        p_vci.eop     = (r_vci_cmd_cpt.read() == r_vci_cmd_max.read());
        break;

    case CMD_DATA_SC:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_vci_paddr.read() & ~0x3;
        if ( r_vci_cmd_cpt.read() == 0 ) p_vci.wdata = r_dcache_vci_sc_old.read();
        else                             p_vci.wdata = r_dcache_vci_sc_new.read();
        p_vci.be      = 0xF;
        p_vci.trdid   = TYPE_DATA_UNC;  // SC transactions are handled as UNC by the RSP_FSM
        p_vci.plen    = 8;
        p_vci.cmd     = vci_param::CMD_STORE_COND;
        p_vci.eop     = (r_vci_cmd_cpt.read() == 1);
        break;      
    } // end switch r_vci_cmd_fsm

    //////////////////////////////////////////////////////////
    // VCI initiator response on the direct network
    // it depends on the VCI RSP state

    switch (r_vci_rsp_fsm.read() )
    {
        case RSP_DATA_WRITE : p_vci.rspack = true; break;
        case RSP_INS_MISS   : p_vci.rspack = r_vci_rsp_fifo_icache.wok(); break;
        case RSP_INS_UNC    : p_vci.rspack = r_vci_rsp_fifo_icache.wok(); break;
        case RSP_DATA_MISS  : p_vci.rspack = r_vci_rsp_fifo_dcache.wok(); break;
        case RSP_DATA_UNC   : p_vci.rspack = r_vci_rsp_fifo_dcache.wok(); break;
        case RSP_IDLE       : p_vci.rspack = false; break;
    } // end switch r_vci_rsp_fsm

} // end genMoore

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4










