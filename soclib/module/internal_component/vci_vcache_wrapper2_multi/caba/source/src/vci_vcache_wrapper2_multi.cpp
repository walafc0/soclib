/* -*- c++ -*-
 * File : vci_vcache_wrapper2_multi.cpp
 * Copyright (c) UPMC, Lip6, SoC
 * Authors : Alain GREINER, Yang GAO
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
#include "../include/vci_vcache_wrapper2_multi.h"

namespace soclib { 
namespace caba {

//#define SOCLIB_MODULE_DEBUG

#ifdef SOCLIB_MODULE_DEBUG
namespace {
const char *icache_fsm_state_str[] = {
        "ICACHE_IDLE",
        "ICACHE_BIS",       
        "ICACHE_TLB1_READ",  
        "ICACHE_TLB1_WRITE",  
        "ICACHE_TLB1_UPDT",  
        "ICACHE_TLB2_READ",  
        "ICACHE_TLB2_WRITE",  
        "ICACHE_TLB2_UPDT",  
        "ICACHE_SW_FLUSH", 
        "ICACHE_TLB_FLUSH", 
        "ICACHE_CACHE_FLUSH", 
        "ICACHE_TLB_INVAL",  
        "ICACHE_CACHE_INVAL",
        "ICACHE_MISS_WAIT",
        "ICACHE_UNC_WAIT",  
        "ICACHE_MISS_UPDT",  
        "ICACHE_ERROR", 	
    	"ICACHE_CACHE_INVAL_PA",
    };
const char *dcache_fsm_state_str[] = {
        "DCACHE_IDLE",       
        "DCACHE_BIS",   
        "DCACHE_DTLB1_READ_CACHE", 
	    "DCACHE_TLB1_LL_WAIT",
	    "DCACHE_TLB1_SC_WAIT",    
        "DCACHE_TLB1_READ",
        "DCACHE_TLB1_READ_UPDT",  
        "DCACHE_TLB1_UPDT", 
        "DCACHE_DTLB2_READ_CACHE",  
	    "DCACHE_TLB2_LL_WAIT",
	    "DCACHE_TLB2_SC_WAIT", 
        "DCACHE_TLB2_READ",
        "DCACHE_TLB2_READ_UPDT",  
        "DCACHE_TLB2_UPDT",   
        "DCACHE_CTXT_SWITCH",   
        "DCACHE_ICACHE_FLUSH", 
        "DCACHE_DCACHE_FLUSH", 
        "DCACHE_ITLB_INVAL",
        "DCACHE_DTLB_INVAL",
        "DCACHE_ICACHE_INVAL",
        "DCACHE_DCACHE_INVAL",
        "DCACHE_DCACHE_SYNC",
	    "DCACHE_LL_DIRTY_WAIT",
	    "DCACHE_SC_DIRTY_WAIT",
        "DCACHE_WRITE_UPDT", 
        "DCACHE_WRITE_DIRTY",
        "DCACHE_WRITE_REQ",  
        "DCACHE_MISS_WAIT",  
        "DCACHE_MISS_UPDT",  
        "DCACHE_UNC_WAIT",   
        "DCACHE_ERROR", 
        "DCACHE_ITLB_READ",
        "DCACHE_ITLB_UPDT",
        "DCACHE_ITLB_LL_WAIT",        
        "DCACHE_ITLB_SC_WAIT",
	    "DCACHE_ICACHE_INVAL_PA",
	    "DCACHE_DCACHE_INVAL_PA",
    };
const char *cmd_fsm_state_str[] = {
        "CMD_IDLE",           
        "CMD_ITLB_READ",
        "CMD_ITLB_ACC_LL",                
        "CMD_ITLB_ACC_SC",         
        "CMD_INS_MISS",     
        "CMD_INS_UNC",     
        "CMD_DTLB_READ",    
        "CMD_DTLB_ACC_LL",            
        "CMD_DTLB_ACC_SC",            
        "CMD_DTLB_DIRTY_LL",          
        "CMD_DTLB_DIRTY_SC",    
        "CMD_DATA_UNC",     
        "CMD_DATA_MISS",    
        "CMD_DATA_WRITE",    
    };
const char *rsp_fsm_state_str[] = {
        "RSP_IDLE",                  
        "RSP_ITLB_READ",             
        "RSP_ITLB_ACC_LL",                
        "RSP_ITLB_ACC_SC",                
        "RSP_INS_MISS",   
        "RSP_INS_UNC",           
        "RSP_DTLB_READ",            
        "RSP_DTLB_ACC_LL",            
        "RSP_DTLB_ACC_SC",            
        "RSP_DTLB_DIRTY_LL",          
        "RSP_DTLB_DIRTY_SC",                  
        "RSP_DATA_MISS",             
        "RSP_DATA_UNC",              
        "RSP_DATA_WRITE",            
    };	
}
#endif

#define tmpl(...)  template<typename vci_param, typename iss_t> __VA_ARGS__ VciVCacheWrapper2Multi<vci_param, iss_t>

using soclib::common::uint32_log2;

/***********************************************/
tmpl(/**/)::VciVCacheWrapper2Multi(
    sc_module_name name,
    int proc_id,
    const soclib::common::MappingTable &mt,
    const soclib::common::IntTab &initiator_index,
    size_t itlb_ways,
    size_t itlb_sets,
    size_t dtlb_ways,
    size_t dtlb_sets,
    size_t icache_ways,
    size_t icache_sets,
    size_t icache_words,
    size_t dcache_ways,
    size_t dcache_sets,
    size_t dcache_words,
    size_t wbuf_nwords,
    size_t wbuf_nlines )
/***********************************************/
    : soclib::caba::BaseModule(name),

      p_clk("clk"),
      p_resetn("resetn"),
      p_vci("vci"),

      m_cacheability_table(mt.getCacheabilityTable()),
      m_srcid(mt.indexForId(initiator_index)),
      m_iss(this->name(), proc_id),

      m_itlb_ways(itlb_ways),
      m_itlb_sets(itlb_sets),

      m_dtlb_ways(dtlb_ways),
      m_dtlb_sets(dtlb_sets),

      m_icache_ways(icache_ways),
      m_icache_sets(icache_sets),
      m_icache_yzmask((~0)<<(uint32_log2(icache_words) + 2)),
      m_icache_words(icache_words),

      m_dcache_ways(dcache_ways),
      m_dcache_sets(dcache_sets),
      m_dcache_yzmask((~0)<<(uint32_log2(dcache_words) + 2)),
      m_dcache_words(dcache_words),

      m_paddr_nbits(vci_param::N),

      icache_tlb(itlb_ways,itlb_sets,vci_param::N),
      dcache_tlb(dtlb_ways,dtlb_sets,vci_param::N),

      r_dcache_fsm("r_dcache_fsm"),
      r_dcache_paddr_save("r_dcache_paddr_save"),
      r_dcache_wdata_save("r_dcache_wdata_save"),
      r_dcache_rdata_save("r_dcache_rdata_save"),
      r_dcache_type_save("r_dcache_type_save"),
      r_dcache_be_save("r_dcache_be_save"),
      r_dcache_cached_save("r_dcache_cached_save"),
      r_dcache_tlb_paddr("r_dcache_tlb_paddr"),
      r_dcache_miss_req("r_dcache_miss_req"),
      r_dcache_unc_req("r_dcache_unc_req"),
      r_dcache_write_req("r_dcache_write_req"),
      r_dcache_tlb_read_req("r_dcache_tlb_read_req"),

      r_dcache_tlb_ll_acc_req("r_dcache_tlb_ll_acc_req"),       
      r_dcache_tlb_sc_acc_req("r_dcache_tlb_sc_acc_req"),       
      r_dcache_tlb_ll_dirty_req("r_dcache_tlb_ll_dirty_req"),    
      r_dcache_tlb_sc_dirty_req("r_dcache_tlb_sc_dirty_req"), 
      r_dcache_tlb_ptba_read("r_dcache_tlb_ptba_read"),
      r_dcache_xtn_req("r_dcache_xtn_req"),

      r_icache_fsm("r_icache_fsm"),
      r_icache_paddr_save("r_icache_paddr_save"),
      r_icache_miss_req("r_icache_miss_req"),
      r_icache_unc_req("r_icache_unc_req"),
      r_dcache_itlb_read_req("r_dcache_itlb_read_req"),
      r_dcache_itlb_ll_acc_req("r_dcache_itlb_ll_acc_req"),     
      r_dcache_itlb_sc_acc_req("r_dcache_itlb_sc_acc_req"),

      r_itlb_read_dcache_req("r_itlb_read_dcache_req"),
      r_itlb_acc_dcache_req("r_itlb_acc_dcache_req"),
      r_dcache_rsp_itlb_error("r_dcache_rsp_itlb_error"),

      r_vci_cmd_fsm("r_vci_cmd_fsm"),
      r_vci_cmd_min("r_vci_cmd_min"),
      r_vci_cmd_max("r_vci_cmd_max"),
      r_vci_cmd_cpt("r_vci_cmd_cpt"),

      r_vci_rsp_fsm("r_vci_rsp_fsm"),
      r_vci_rsp_cpt("r_vci_rsp_cpt"),
      r_vci_rsp_ins_error("r_vci_rsp_ins_error"),
      r_vci_rsp_data_error("r_vci_rsp_data_error"),
      r_vci_rsp_ins_ok("r_vci_rsp_ins_ok"),
      r_vci_rsp_data_ok("r_vci_rsp_data_ok"),

      r_wbuf("wbuf", wbuf_nwords, wbuf_nlines ),
      r_icache("icache", icache_ways, icache_sets, icache_words),
      r_dcache("dcache", dcache_ways, dcache_sets, dcache_words)
{
    r_icache_miss_buf = new data_t[icache_words];
    r_dcache_miss_buf = new data_t[dcache_words];

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();
  
    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();

    typename iss_t::CacheInfo cache_info;
    cache_info.has_mmu = true;
    cache_info.icache_line_size = icache_words*sizeof(data_t);
    cache_info.icache_assoc = icache_ways;
    cache_info.icache_n_lines = icache_sets;
    cache_info.dcache_line_size = dcache_words*sizeof(data_t);
    cache_info.dcache_assoc = dcache_ways;
    cache_info.dcache_n_lines = dcache_sets;
    m_iss.setCacheInfo(cache_info);
}

/////////////////////////////////////
tmpl(/**/)::~VciVCacheWrapper2Multi()
/////////////////////////////////////
{
    delete [] r_icache_miss_buf;
    delete [] r_dcache_miss_buf;
}

////////////////////////
tmpl(void)::print_cpi()
////////////////////////
{
    std::cout << name() << " CPI = " 
        << (float)m_cpt_total_cycles/(m_cpt_total_cycles - m_cpt_frz_cycles) << std::endl ;
}

////////////////////////
tmpl(void)::print_stats()
////////////////////////
{
    float run_cycles = (float)(m_cpt_total_cycles - m_cpt_frz_cycles);
    std::cout
        << name() << std::endl
        << "- CPI                    = " << (float)m_cpt_total_cycles/run_cycles << std::endl 
        << "- READ RATE              = " << (float)m_cpt_read/run_cycles << std::endl 
        << "- WRITE RATE             = " << (float)m_cpt_write/run_cycles << std::endl
        << "- UNCACHED READ RATE     = " << (float)m_cpt_unc_read/m_cpt_read << std::endl 
        << "- CACHED WRITE RATE      = " << (float)m_cpt_write_cached/m_cpt_write << std::endl 
        << "- IMISS_RATE             = " << (float)m_cpt_ins_miss/m_cpt_ins_read << std::endl
        << "- DMISS RATE             = " << (float)m_cpt_data_miss/(m_cpt_read-m_cpt_unc_read) << std::endl 
        << "- INS MISS COST          = " << (float)m_cost_ins_miss_frz/m_cpt_ins_miss << std::endl
        << "- IMISS TRANSACTION      = " << (float)m_cost_imiss_transaction/m_cpt_imiss_transaction << std::endl
        << "- DMISS COST             = " << (float)m_cost_data_miss_frz/m_cpt_data_miss << std::endl
        << "- DMISS TRANSACTION      = " << (float)m_cost_dmiss_transaction/m_cpt_dmiss_transaction << std::endl
        << "- UNC COST               = " << (float)m_cost_unc_read_frz/m_cpt_unc_read << std::endl
        << "- UNC TRANSACTION        = " << (float)m_cost_unc_transaction/m_cpt_unc_transaction << std::endl
        << "- WRITE COST             = " << (float)m_cost_write_frz/m_cpt_write << std::endl
        << "- WRITE TRANSACTION      = " << (float)m_cost_write_transaction/m_cpt_write_transaction << std::endl
        << "- WRITE LENGTH           = " << (float)m_length_write_transaction/m_cpt_write_transaction << std::endl
        << "- INS TLB MISS RATE      = " << (float)m_cpt_ins_tlb_miss/m_cpt_ins_tlb_read << std::endl
        << "- DATA TLB MISS RATE     = " << (float)m_cpt_data_tlb_miss/m_cpt_data_tlb_read << std::endl
        << "- ITLB MISS TRANSACTION  = " << (float)m_cost_itlbmiss_transaction/m_cpt_itlbmiss_transaction << std::endl
        << "- ITLB WRITE TRANSACTION = " << (float)m_cost_itlb_write_transaction/m_cpt_itlb_write_transaction << std::endl
        << "- ITLB MISS COST         = " << (float)m_cost_ins_tlb_miss_frz/(m_cpt_ins_tlb_miss+m_cpt_ins_tlb_write_et) << std::endl
        << "- DTLB MISS TRANSACTION  = " << (float)m_cost_dtlbmiss_transaction/m_cpt_dtlbmiss_transaction << std::endl
        << "- DTLB WRITE TRANSACTION = " << (float)m_cost_dtlb_write_transaction/m_cpt_dtlb_write_transaction << std::endl
        << "- DTLB MISS COST         = " << (float)m_cost_data_tlb_miss_frz/(m_cpt_data_tlb_miss+m_cpt_data_tlb_write_et+m_cpt_data_tlb_write_dirty) << std::endl;
}

/*************************************************/
tmpl(void)::transition()
/*************************************************/
{
    if ( ! p_resetn.read() ) 
    {
        m_iss.reset();

        r_dcache_fsm = DCACHE_IDLE;
        r_icache_fsm = ICACHE_IDLE;
        r_vci_cmd_fsm = CMD_IDLE;
        r_vci_rsp_fsm = RSP_IDLE;

        // write buffer & caches
        r_wbuf.reset();
        r_icache.reset();
        r_dcache.reset();

        icache_tlb.reset();    
        dcache_tlb.reset();   
 
        r_mmu_mode = ALL_DEACTIVE;
        r_mmu_params = (uint32_log2(m_dtlb_ways) << 29)   | (uint32_log2(m_dtlb_sets) << 25)   |
                       (uint32_log2(m_dcache_ways) << 22) | (uint32_log2(m_dcache_sets) << 18) |
                       (uint32_log2(m_itlb_ways) << 15)   | (uint32_log2(m_itlb_sets) << 11)   |
                       (uint32_log2(m_icache_ways) << 8)  | (uint32_log2(m_icache_sets) << 4)  |
                       (uint32_log2(m_icache_words * 4));
        r_mmu_release = (uint32_t)(2 << 16) | 0x0;

        r_icache_miss_req        = false;
        r_icache_unc_req         = false;
        r_dcache_itlb_read_req   = false;

        r_itlb_read_dcache_req   = false;      
        r_itlb_acc_dcache_req    = false;   
        r_dcache_rsp_itlb_error  = false;
 
        r_dcache_miss_req        = false;
        r_dcache_unc_req         = false;
        r_dcache_write_req       = false;
        r_dcache_tlb_read_req    = false;
        r_dcache_tlb_ptba_read   = false;
        r_dcache_xtn_req         = false;

        r_dcache_tlb_ll_acc_req   = false;    
        r_dcache_tlb_sc_acc_req   = false;    
        r_dcache_tlb_ll_dirty_req = false;   
        r_dcache_tlb_sc_dirty_req = false;   
        r_dcache_itlb_ll_acc_req  = false;   
        r_dcache_itlb_sc_acc_req  = false;  

        r_dcache_dirty_save      = false;
        r_dcache_hit_p_save      = false;

        r_vci_rsp_ins_error      = false;
        r_vci_rsp_data_error     = false;

        r_vci_rsp_ins_ok         = false;
        r_vci_rsp_data_ok        = false;

        r_icache_id1_save        = 0;
        r_icache_ppn_save        = 0;
        r_icache_vpn_save        = 0;
        r_itlb_translation_valid = false;

        r_dcache_id1_save        = 0;
        r_dcache_ppn_save        = 0;
        r_dcache_vpn_save        = 0;
        r_dtlb_translation_valid = false;

        r_icache_ptba_ok         = false;
        r_dcache_ptba_ok         = false;

        r_icache_error_type      = MMU_NONE;
        r_dcache_error_type      = MMU_NONE;

        // activity counters
        m_cpt_dcache_data_read  = 0;
        m_cpt_dcache_data_write = 0;
        m_cpt_dcache_dir_read   = 0;
        m_cpt_dcache_dir_write  = 0;
        m_cpt_icache_data_read  = 0;
        m_cpt_icache_data_write = 0;
        m_cpt_icache_dir_read   = 0;
        m_cpt_icache_dir_write  = 0;

	    m_cpt_frz_cycles   = 0;
        m_cpt_total_cycles = 0;

        m_cpt_read         = 0;
        m_cpt_write        = 0;
        m_cpt_data_miss    = 0;
        m_cpt_ins_miss     = 0;
        m_cpt_unc_read     = 0;
        m_cpt_write_cached = 0;
        m_cpt_ins_read     = 0;

        m_cost_write_frz     = 0;
        m_cost_data_miss_frz = 0;
        m_cost_unc_read_frz  = 0;
        m_cost_ins_miss_frz  = 0;

        m_cpt_imiss_transaction = 0;
        m_cpt_dmiss_transaction = 0;
        m_cpt_unc_transaction   = 0;
        m_cpt_write_transaction = 0;

        m_cost_imiss_transaction   = 0;
        m_cost_dmiss_transaction   = 0;
        m_cost_unc_transaction     = 0;
        m_cost_write_transaction   = 0;
        m_length_write_transaction = 0;

        m_cpt_ins_tlb_read         = 0;             
        m_cpt_ins_tlb_miss         = 0;             
        m_cpt_ins_tlb_write_et     = 0;         

        m_cpt_data_tlb_read        = 0;           
        m_cpt_data_tlb_miss        = 0;           
        m_cpt_data_tlb_write_et    = 0;       
        m_cpt_data_tlb_write_dirty = 0;    

        m_cost_ins_tlb_miss_frz    = 0;      
        m_cost_data_tlb_miss_frz   = 0;      

        m_cpt_itlbmiss_transaction   = 0;    
        m_cpt_itlb_write_transaction = 0;  
        m_cpt_dtlbmiss_transaction   = 0;  
        m_cpt_dtlb_write_transaction = 0;  
 
        m_cost_itlbmiss_transaction   = 0;   
        m_cost_itlb_write_transaction = 0;  
        m_cost_dtlbmiss_transaction   = 0;   
        m_cost_dtlb_write_transaction = 0;   
        return;
    }

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name() << " cycle = " << m_cpt_total_cycles
              << " dcache fsm: " << dcache_fsm_state_str[r_dcache_fsm]
              << " icache fsm: " << icache_fsm_state_str[r_icache_fsm]
              << " cmd fsm: " << cmd_fsm_state_str[r_vci_cmd_fsm]
              << " rsp fsm: " << rsp_fsm_state_str[r_vci_rsp_fsm] << std::endl;
#endif

    m_cpt_total_cycles++;

    typename iss_t::InstructionRequest ireq = ISS_IREQ_INITIALIZER;
    typename iss_t::InstructionResponse irsp = ISS_IRSP_INITIALIZER;

    typename iss_t::DataRequest dreq = ISS_DREQ_INITIALIZER;
    typename iss_t::DataResponse drsp = ISS_DRSP_INITIALIZER;

    m_iss.getRequests( ireq, dreq );

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name() << " Instruction Request: " << ireq << std::endl;
    std::cout << name() << " Data Request: " << dreq << std::endl;
#endif

    ////////////////////////////////////////////////////////////////////////////////////////
    //      ICACHE_FSM
    //
    // There is 9 mutually exclusive conditions to exit the IDLE state.
    // Four configurations corresponding to an XTN request from processor,
    // - Flush TLB (in case of Context switch) => TLB_FLUSH state 
    // - Flush cache => CACHE_FLUSH state 
    // - Invalidate a TLB entry => TLB_INVAL state
    // - Invalidate a cache line => CACHE_INVAL state
    // Five configurations corresponding to various TLB or cache MISS :
    // - TLB miss(in case hit_p miss) => TLB1_READ state
    // - TLB miss(in case hit_p hit) => TLB2_READ state
    // - Hit in TLB but VPN changed => BIS state
    // - Cached read miss => MISS_REQ state
    // - Uncache read miss => UNC_REQ state
    // 
    // In case of MISS, the controller writes a request in the r_icache_paddr_save register 
    // and sets the corresponding request flip-flop : r_dcache_itlb_read_req, r_icache_miss_req 
    // or r_icache_unc_req. These request flip-flops are reset by the VCI_RSP controller 
    // when the response is ready in the ICACHE buffer.
    //
    // The DCACHE FSM signals XTN processor requests using the r_dcache_xtn_req flip-flop. 
    // The request opcod and the address to be invalidated are transmitted
    // in the r_dcache_paddr_save & r_dcache_wdata_save registers respectively.
    // The request flip-flop is reset by the ICACHE_FSM when the operation is completed.
    //
    // The r_vci_rsp_ins_error flip-flop is set by the VCI_RSP FSM and reset
    // by the ICACHE-FSM in the ICACHE_ERROR state.
    //
    //----------------------------------------------------------------------------------- 
    // Instruction TLB: 
    //  
    // - int        ET          (00: unmapped; 01: unused or PTD)
    //                          (10: PTE new;  11: PTE old      )
    // - bool       cachable    (cached bit)
    // - bool       writable    (** not used alwayse false) 
    // - bool       executable  (executable bit)
    // - bool       user        (access in user mode allowed)
    // - bool       global      (PTE not invalidated by a TLB flush)
    // - bool       dirty       (** not used alwayse false) 
    // - uint32_t   vpn         (virtual page number)
    // - uint32_t   ppn         (physical page number)
    ////////////////////////////////////////////////////////////////////////////////////////

    switch(r_icache_fsm) {

    ////////////////
    case ICACHE_IDLE:
    {
        pte_info_t  icache_pte_info;
        paddr_t     tlb_ipaddr     = 0;        // physical address obtained from TLB                                
        paddr_t     spc_ipaddr     = 0;        // physical adress obtained from PPN_save (speculative)                         
        data_t      icache_ins     = 0;        // read instruction
        bool        icache_hit_c   = false;    // Cache hit
        bool        icache_cached  = false;    // cacheable access (read)
        bool        icache_hit_t   = false;    // hit on TLB
        bool        icache_hit_x   = false;    // VPN unmodified (can use spc_dpaddr)
        bool        icache_hit_p   = false;    // PTP unmodified (can skip first level page table walk)
        size_t      icache_tlb_way = 0;        // selected way (in case of cache hit)
        size_t      icache_tlb_set = 0;        // selected set (Y field in address)

        // Decoding processor XTN requests
        // They are sent by DCACHE FSM  

        if (r_dcache_xtn_req)
        {
            if ((int)r_dcache_type_save == (int)iss_t::XTN_PTPR)  
            {
                r_icache_fsm = ICACHE_SW_FLUSH;   
                break;
            }
            if ((int)r_dcache_type_save == (int)iss_t::XTN_ICACHE_FLUSH)
            {
                r_icache_fsm = ICACHE_CACHE_FLUSH;   
                break;
            }
            if ((int)r_dcache_type_save == (int)iss_t::XTN_ITLB_INVAL) 
            {
                r_icache_fsm = ICACHE_TLB_INVAL;   
                break;
            }
            if ((int)r_dcache_type_save == (int)iss_t::XTN_ICACHE_INVAL) 
            {
                r_icache_fsm = ICACHE_CACHE_INVAL;   
                break;
            }
            if ((int)r_dcache_type_save == (int)iss_t::XTN_MMU_ICACHE_PA_INV) 
            {
                r_icache_fsm = ICACHE_CACHE_INVAL_PA;   
                break;
            }
            if ((int)r_dcache_type_save == (int)iss_t::XTN_DCACHE_FLUSH )  
            {
                // special for ins tlb miss via data cache
                r_icache_fsm = ICACHE_TLB_FLUSH;   
                break;
            }
        } // end if xtn_req

        // icache_hit_t_m, icache_hit_t_k, icache_hit_x, icache_hit_p 
        // icache_pte_info, icache_tlb_way, icache_tlb_set & ipaddr & cacheability 
        // - If MMU activated : cacheability is defined by the cachable bit in the TLB
        // - If MMU not activated : cacheability is defined by the segment table.

        if ( !(r_mmu_mode.read() & INS_TLB_MASK) )   // MMU not activated 
        {
            icache_hit_t  = true;         
            icache_hit_x  = true;         
            icache_hit_p  = true;         
            tlb_ipaddr    = ireq.addr;
            spc_ipaddr    = ireq.addr;
            icache_cached = m_cacheability_table[ireq.addr];
        } 
        else                                                                // MMU activated
        { 
            m_cpt_ins_tlb_read++;
            icache_hit_t  = icache_tlb.translate(ireq.addr, &tlb_ipaddr, &icache_pte_info, 
                                                 &icache_tlb_way, &icache_tlb_set); 
            icache_hit_x  = (((vaddr_t)r_icache_vpn_save << PAGE_K_NBITS) == (ireq.addr & ~PAGE_K_MASK)) && r_itlb_translation_valid;
            icache_hit_p  = (((ireq.addr >> PAGE_M_NBITS) == r_icache_id1_save) && r_icache_ptba_ok); 
            spc_ipaddr    = ((paddr_t)r_icache_ppn_save << PAGE_K_NBITS) | (paddr_t)(ireq.addr & PAGE_K_MASK);
            icache_cached = icache_pte_info.c; 
        }

        if ( !(r_mmu_mode.read() & INS_CACHE_MASK) )   // cache not actived
        {
            icache_cached = false;
        }

        if ( ireq.valid ) 
        {
            m_cpt_icache_dir_read += m_icache_ways;
            m_cpt_icache_data_read += m_icache_ways;

            // icache_hit_c & icache_ins
            if ( icache_cached )    // using speculative physical address for cached access
            {
                icache_hit_c = r_icache.read(spc_ipaddr, &icache_ins);
            }
            else                    // using actual physical address for uncached access
            {
                icache_hit_c = ( r_vci_rsp_ins_ok && (tlb_ipaddr == (paddr_t)r_icache_paddr_save) );
                icache_ins = r_icache_miss_buf[0];
            }

            if ( r_mmu_mode.read() & INS_TLB_MASK ) 
            {
                if ( icache_hit_t ) 
                {
                    // check access rights
                    if ( !icache_pte_info.u && (ireq.mode == iss_t::MODE_USER)) 
                    {
                        r_icache_error_type = MMU_READ_PRIVILEGE_VIOLATION;  
                        r_icache_bad_vaddr = ireq.addr;
                        irsp.valid = true;
                        irsp.error = true;
                        irsp.instruction = 0;
                        break;
                    }
                    if ( !icache_pte_info.x ) 
                    {
                        r_icache_error_type = MMU_READ_EXEC_VIOLATION;  
                        r_icache_bad_vaddr = ireq.addr;
                        irsp.valid = true;
                        irsp.error = true;
                        irsp.instruction = 0;
                        break;
                    }
                }

                // update LRU, save ppn, vpn and page type
                if ( icache_hit_t )
                {  
                    icache_tlb.setlru(icache_tlb_way,icache_tlb_set);     
                    r_icache_ppn_save = tlb_ipaddr >> PAGE_K_NBITS;
                    r_icache_vpn_save = ireq.addr >> PAGE_K_NBITS;
                    r_itlb_translation_valid = true;
                }
                else
                {
                    r_itlb_translation_valid = false;
                }

            } // end if MMU activated

            // compute next state 

            if ( !icache_hit_t && !icache_hit_p )      // TLB miss
            {
                // walk page table  level 1
                r_icache_paddr_save = (paddr_t)r_mmu_ptpr << (INDEX1_NBITS+2) | (paddr_t)((ireq.addr>>PAGE_M_NBITS)<<2);
                r_itlb_read_dcache_req = true;
                r_icache_fsm = ICACHE_TLB1_READ;
		        r_icache_vaddr_req = ireq.addr;
                m_cpt_ins_tlb_miss++;
                m_cost_ins_tlb_miss_frz++;
            }
            else if ( !icache_hit_t && icache_hit_p )  // TLB Miss with possibility of bypass first level page
            {
                // walk page table level 2
                r_icache_paddr_save = (paddr_t)r_icache_ptba_save | 
                                      (paddr_t)(((ireq.addr&PTD_ID2_MASK)>>PAGE_K_NBITS) << 3);
                r_itlb_read_dcache_req = true;
                r_icache_fsm = ICACHE_TLB2_READ;
		        r_icache_vaddr_req = ireq.addr;
                m_cpt_ins_tlb_miss++;
                m_cost_ins_tlb_miss_frz++;
            }
            else if ( icache_hit_t && !icache_hit_x && icache_cached ) // cached access with an ucorrect speculative physical address
            {
                r_icache_paddr_save = tlb_ipaddr;   // save actual physical address for BIS
                r_icache_fsm = ICACHE_BIS;
		        r_icache_vaddr_req = ireq.addr;
                m_cost_ins_miss_frz++;
            }
            else    // cached or uncached access with a correct speculative physical address 
            {        
                m_cpt_ins_read++; 
                if ( !icache_hit_c ) 
                {
                    m_cpt_ins_miss++;
                    m_cost_ins_miss_frz++;
                    if ( icache_cached ) 
                    {
                        r_icache_miss_req = true;
                        r_vci_rsp_ins_ok = false;
                        r_icache_paddr_save = spc_ipaddr; 
                        r_icache_fsm = ICACHE_MISS_WAIT;
			            r_icache_vaddr_req = ireq.addr;
                    } 
                    else 
                    {
                        r_icache_unc_req = true;
                        r_vci_rsp_ins_ok = false;
                        r_icache_paddr_save = tlb_ipaddr; 
                        r_icache_fsm = ICACHE_UNC_WAIT;
			            r_icache_vaddr_req = ireq.addr;
                    } 
                } 
                else 
                {
                    r_vci_rsp_ins_ok = false;
                    r_icache_fsm = ICACHE_IDLE;
                }
                irsp.valid = icache_hit_c;
                irsp.instruction = icache_ins;
            } // end if next states
            
        } // end if ireq.valid
        break;
    }
    ////////////////
    case ICACHE_BIS: 
    {
        data_t      icache_ins = 0;        	    // read instruction
        bool        icache_hit_c = false;    	// Cache hit
        bool        icache_hit_t = false;    	// TLB hit
        paddr_t     tlb_ipaddr     = 0;         // physical address obtained from TLB      
 
        // acces always cached and MMU activated in this state
        m_cpt_ins_tlb_read++;
        m_cpt_icache_dir_read += m_icache_ways;
        m_cpt_icache_data_read += m_icache_ways;

        // processor address translation
        icache_hit_t = icache_tlb.translate(ireq.addr, &tlb_ipaddr);

        // test if processor request modified
        if ( (tlb_ipaddr == r_icache_paddr_save.read()) && ireq.valid && icache_hit_t ) // unmodified & valid
        {
            m_cpt_ins_read++;
            icache_hit_c = r_icache.read(r_icache_paddr_save, &icache_ins);

            if ( !icache_hit_c ) 
            {
                r_icache_miss_req = true;
                r_vci_rsp_ins_ok = false;
                r_icache_fsm = ICACHE_MISS_WAIT;
                m_cpt_ins_miss++;
                m_cost_ins_miss_frz++;
            } 
            else 
            {
                r_icache_fsm = ICACHE_IDLE;
            }
            irsp.valid = icache_hit_c;
	    if (irsp.valid)
	            assert((r_icache_vaddr_req.read() == ireq.addr) &&
		        "vaddress should not be modified while ICACHE_BIS");
            irsp.error = false;
            irsp.instruction = icache_ins;
        } 
        else                                                                        // modified or invalid
        {                                                   
            irsp.valid = false;
            irsp.error = false;
            irsp.instruction = 0;
            r_icache_fsm = ICACHE_IDLE;
        }      
        break;
    }
    //////////////////////
    case ICACHE_TLB1_READ:
    {
        if ( ireq.valid ) m_cost_ins_tlb_miss_frz++;

        if ( !r_itlb_read_dcache_req ) // TLB miss read response
        {
	        if (r_icache_vaddr_req.read() != ireq.addr || !ireq.valid) {
	        /* request modified, drop response and restart */
	        r_icache_ptba_ok = false;
	        r_icache_fsm = ICACHE_IDLE;
	        break;
	    }
		
            if (!r_dcache_rsp_itlb_error ) // vci response ok
            { 
                if ( !(r_dcache_rsp_itlb_miss >> PTE_V_SHIFT) ) // unmapped
                {
            	    r_icache_ptba_ok    = false;	
                    r_icache_error_type = MMU_READ_PT1_UNMAPPED;  
                    r_icache_bad_vaddr  = ireq.addr;
                    r_icache_fsm        = ICACHE_ERROR;
                }
	            else if ( (r_dcache_rsp_itlb_miss & PTE_T_MASK ) >> PTE_T_SHIFT ) // PTD
	            {
                    r_icache_ptba_ok       = true;	
                    r_icache_ptba_save     = (paddr_t)(r_dcache_rsp_itlb_miss & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1)) << PAGE_K_NBITS; 
                    r_icache_id1_save      = ireq.addr >> PAGE_M_NBITS;
                    r_icache_paddr_save    = (paddr_t)(r_dcache_rsp_itlb_miss & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1)) << PAGE_K_NBITS |
                                             (paddr_t)(((ireq.addr & PTD_ID2_MASK) >> PAGE_K_NBITS) << 3); 
                    r_itlb_read_dcache_req = true;
                    r_icache_fsm           = ICACHE_TLB2_READ;
	            }	
	            else
	            {
                    r_icache_ptba_ok = false;
	            
	                if ( (m_srcid >> 4) == ((r_dcache_rsp_itlb_miss & ((1<<(m_paddr_nbits - PAGE_M_NBITS))-1)) >> (m_paddr_nbits - PAGE_M_NBITS -10)) ) // local
	                {
	                    if ( (r_dcache_rsp_itlb_miss & PTE_L_MASK ) >> PTE_L_SHIFT ) // L bit is set
	                    {
                            r_icache_pte_update = r_dcache_rsp_itlb_miss;
                            r_icache_fsm        = ICACHE_TLB1_UPDT;
	                    }
	                    else
	                    {
                            r_icache_pte_update   = r_dcache_rsp_itlb_miss | PTE_L_MASK;
                            r_itlb_acc_dcache_req = true;
                            r_icache_fsm          = ICACHE_TLB1_WRITE;
                            m_cpt_ins_tlb_write_et++;
	                    }
                    }
	                else // remotely
	                {
	                    if ( (r_dcache_rsp_itlb_miss & PTE_R_MASK ) >> PTE_R_SHIFT ) // R bit is set
	                    {
                            r_icache_pte_update = r_dcache_rsp_itlb_miss;
                            r_icache_fsm        = ICACHE_TLB1_UPDT;
	                    }
	                    else
	                    {
                            r_icache_pte_update   = r_dcache_rsp_itlb_miss | PTE_R_MASK;
                            r_itlb_acc_dcache_req = true;
                            r_icache_fsm          = ICACHE_TLB1_WRITE;
                            m_cpt_ins_tlb_write_et++;
	                    }
	                }
	            }
            }
            else        // vci response error
            {
                r_icache_fsm = ICACHE_ERROR;
                r_icache_error_type = MMU_READ_PT1_ILLEGAL_ACCESS;    
                r_icache_bad_vaddr = ireq.addr;
            }
        }
        break;
    }
    ///////////////////////
    case ICACHE_TLB1_WRITE:  
    {
        if ( ireq.valid ) m_cost_ins_tlb_miss_frz++;

        if (!r_itlb_acc_dcache_req )        
        { 
            if ( r_dcache_rsp_itlb_error ) 
            {
                r_icache_error_type = MMU_READ_PT1_ILLEGAL_ACCESS;  
                r_icache_bad_vaddr = r_icache_vaddr_req.read();
                r_icache_fsm = ICACHE_ERROR;
            } 
            else  
            {
                r_icache_fsm = ICACHE_TLB1_UPDT;  
            }
        } 
        break;
    }
    //////////////////////
    case ICACHE_TLB1_UPDT:
    {
        if ( ireq.valid ) m_cost_ins_tlb_miss_frz++;

        icache_tlb.update(r_icache_pte_update,r_icache_vaddr_req.read());
        r_icache_fsm = ICACHE_IDLE;
        break;
    }
    /////////////////////
    case ICACHE_TLB2_READ:
    {
        if ( ireq.valid ) m_cost_ins_tlb_miss_frz++;

        if ( !r_itlb_read_dcache_req )
        { 
	        if (r_icache_vaddr_req.read() != ireq.addr || !ireq.valid) {
	        /* request modified, drop response and restart */
	        r_icache_ptba_ok = false;
	        r_icache_fsm = ICACHE_IDLE;
	        break;
	    }
            if ( !r_dcache_rsp_itlb_error ) // VCI response ok        
            {
	            if ( !(r_dcache_rsp_itlb_miss >> PTE_V_SHIFT) ) // unmapped
	            {
                    r_icache_error_type = MMU_READ_PT2_UNMAPPED;  
                    r_icache_bad_vaddr  = r_icache_vaddr_req.read();
                    r_icache_fsm = ICACHE_ERROR;
	            }
	            else
	            {
	                if ( (m_srcid >> 4) == ((r_dcache_rsp_itlb_miss & ((1<<(m_paddr_nbits - PAGE_M_NBITS))-1)) >> (m_paddr_nbits - PAGE_M_NBITS -10)) ) // local
	                {
	                    if ( (r_dcache_rsp_itlb_miss & PTE_L_MASK ) >> PTE_L_SHIFT ) // L bit is set
	                    {
                            r_icache_fsm        = ICACHE_TLB2_UPDT;
                            r_icache_pte_update = r_dcache_rsp_itlb_miss;
	                    }
	                    else
	                    {
                            r_icache_pte_update   = r_dcache_rsp_itlb_miss | PTE_L_MASK;
                            r_itlb_acc_dcache_req = true;
                            r_icache_fsm          = ICACHE_TLB2_WRITE;
                            m_cpt_ins_tlb_write_et++;
	                    }
                    }
	                else // remotely
	                {
	                    if ( (r_dcache_rsp_itlb_miss & PTE_R_MASK ) >> PTE_R_SHIFT ) // R bit is set
	                    {
                            r_icache_fsm        = ICACHE_TLB2_UPDT;
                            r_icache_pte_update = r_dcache_rsp_itlb_miss;
	                    }
	                    else
	                    {
                            r_icache_pte_update   = r_dcache_rsp_itlb_miss | PTE_R_MASK;
                            r_itlb_acc_dcache_req = true;
                            r_icache_fsm          = ICACHE_TLB2_WRITE;
                            m_cpt_ins_tlb_write_et++;
	                    }
	                }
	            }
            }
            else    // VCI response error
            {
                r_icache_error_type = MMU_READ_PT2_ILLEGAL_ACCESS;
                r_icache_bad_vaddr = ireq.addr;
                r_icache_fsm = ICACHE_ERROR;
            }
        }
        break;
    }
    /////////////////////////
    case ICACHE_TLB2_WRITE:
    {  
        if ( ireq.valid ) m_cost_ins_tlb_miss_frz++;

        if (!r_itlb_acc_dcache_req)         
        {
            if ( r_dcache_rsp_itlb_error )             
            {
                r_icache_error_type = MMU_READ_PT2_ILLEGAL_ACCESS;  
                r_icache_bad_vaddr = r_icache_vaddr_req.read();
                r_icache_fsm = ICACHE_ERROR;
            } 
            else  
            {
                r_icache_fsm = ICACHE_TLB2_UPDT;  
            }
        } 
        break;
    }
    /////////////////////
    case ICACHE_TLB2_UPDT: 
    {
        if ( ireq.valid ) m_cost_ins_tlb_miss_frz++;

        icache_tlb.update(r_icache_pte_update,r_dcache_rsp_itlb_ppn,r_icache_vaddr_req.read()); 
        r_icache_fsm = ICACHE_IDLE;  
        break;
    }
    /////////////////////
    case ICACHE_SW_FLUSH:
    {
        icache_tlb.flush(false);    // global entries are not invalidated
        r_dcache_xtn_req = false;
        r_itlb_translation_valid = false;
        r_icache_ptba_ok = false;
        r_icache_fsm = ICACHE_IDLE;
        break;
    }
    /////////////////////
    case ICACHE_TLB_FLUSH:
    {
        icache_tlb.flush(true);    // global entries are invalidated
        r_dcache_xtn_req = false;
        r_icache_fsm = ICACHE_IDLE;
        break;
    }
    ////////////////////////
    case ICACHE_CACHE_FLUSH:
    {
        r_icache.reset();
        r_dcache_xtn_req = false;
        r_icache_fsm = ICACHE_IDLE;
        break;
    }
    /////////////////////
    case ICACHE_TLB_INVAL:  
    {
        icache_tlb.inval(r_dcache_wdata_save);
        r_dcache_xtn_req = false;
        r_itlb_translation_valid = false;
        r_icache_ptba_ok = false;
        r_icache_fsm = ICACHE_IDLE;
        break;
	}
    ////////////////////////
    case ICACHE_CACHE_INVAL:
    {	
        paddr_t ipaddr;                     
        bool    icache_hit_t;

        if ( r_mmu_mode.read() & INS_TLB_MASK ) 
        {
            icache_hit_t = icache_tlb.translate(r_dcache_wdata_save, &ipaddr); 
        } 
        else 
        {
            ipaddr = (paddr_t)r_dcache_wdata_save;
            icache_hit_t = true;
        }
        if ( icache_hit_t )
        {
            r_icache.inval(ipaddr);  
        }
        r_dcache_xtn_req = false; 
        r_icache_fsm = ICACHE_IDLE;
        break;
    }
    ////////////////////////
    case ICACHE_CACHE_INVAL_PA:
    {	
        paddr_t ipaddr = (paddr_t)r_mmu_word_hi.read() << 32 | r_mmu_word_lo.read();

        r_icache.inval(ipaddr);  
        r_dcache_xtn_req = false; 
        r_icache_fsm = ICACHE_IDLE;
        break;
    }
    ///////////////////////
    case ICACHE_MISS_WAIT:
    {
        m_cost_ins_miss_frz++;
        if ( r_vci_rsp_ins_ok )
        {
            if ( r_vci_rsp_ins_error ) 
            {
                r_icache_error_type = MMU_READ_DATA_ILLEGAL_ACCESS; 
                r_icache_bad_vaddr = r_icache_vaddr_req.read();
                r_icache_fsm = ICACHE_ERROR;
            } 
            else 
            {
                r_icache_fsm = ICACHE_MISS_UPDT;  
            } 
        }
        break;
    }
    ////////////////////
    case ICACHE_UNC_WAIT:
    {
        m_cost_ins_miss_frz++;
        if ( r_vci_rsp_ins_ok ) 
        {
            if ( r_vci_rsp_ins_error ) 
            {
                r_icache_error_type = MMU_READ_DATA_ILLEGAL_ACCESS;    
                r_icache_bad_vaddr = r_icache_vaddr_req.read();
                r_icache_fsm = ICACHE_ERROR;
            } 
            else 
            {
                r_icache_fsm = ICACHE_IDLE;
            }
        }
        break;
    }
    //////////////////////
    case ICACHE_MISS_UPDT:
    {
        paddr_t  victim_index = 0;
        m_cpt_icache_dir_write++;
        m_cpt_icache_data_write++;
        m_cost_ins_miss_frz++;
        r_icache.update(r_icache_paddr_save.read(), r_icache_miss_buf, &victim_index);
        r_icache_fsm = ICACHE_IDLE;
        break;
    }
    ///////////////////
    case ICACHE_ERROR:
    {
        r_vci_rsp_ins_error = false;
        r_dcache_rsp_itlb_error = false;
        irsp.valid = true;
        irsp.error = true;
        irsp.instruction = 0; 
        r_icache_fsm = ICACHE_IDLE;
        break;
    }
    } // end switch r_icache_fsm

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name() << " Instruction Response: " << irsp << std::endl;
#endif

    ////////////////////////////////////////////////////////////////////////////////////
    //      DCACHE FSM 
    //
    // Both the Cacheability Table, and the MMU cached bit are used to define
    // the cacheability.
    // 
    // There is 14 mutually exclusive conditions to exit the IDLE state.
    // Seven configurations corresponding to an XTN request from processor:
    // - Context switch => CTXT_SWITCH state
    // - Flush dcache => DCACHE_FLUSH state 
    // - Flush icache => ICACHE_FLUSH state 
    // - Invalidate a dtlb entry => DTLB_INVAL state
    // - Invalidate a itlb entry => ITLB_INVAL state
    // - Invalidate a dcache line => DCACHE_INVAL state
    // - Invalidate a icache line => ICACHE_INVAL state
    // Seven configurations corresponding to various read miss or write requests: 
    // - TLB miss(in case hit_p miss) => TLB1_READ state
    // - TLB miss(in case hit_p hit) => TLB2_READ state
    // - Hit in TLB but VPN changed => BIS state
    // - Cached read miss => MISS_REQ state
    // - Uncache read miss => UNC_REQ state
    // - Write hit => WRITE_UPDT state
    // - Write miss => WRITE_REQ
    //
    // The r_vci_rsp_data_error flip-flop is set by the VCI_RSP controller and reset 
    // by DCACHE-FSM when its state is in DCACHE_ERROR. 
    //--------------------------------------------------------------------- 
    // Data TLB: 
    //  
    // - int        ET          (00: unmapped; 01: unused or PTD)
    //                          (10: PTE new;  11: PTE old      )
    // - bool       cachable    (cached bit)
    // - bool       writable    (writable bit) 
    // - bool       executable  (** not used alwayse false)
    // - bool       user        (access in user mode allowed)
    // - bool       global      (PTE not invalidated by a TLB flush)
    // - bool       dirty       (page has been modified) 
    // - uint32_t   vpn         (virtual page number)
    // - uint32_t   ppn         (physical page number)
    ////////////////////////////////////////////////////////////////////////////////////////

    switch (r_dcache_fsm) {

    //////////////////////
    case DCACHE_WRITE_REQ:
    {
         if ( !r_wbuf.wok(r_dcache_paddr_save) )
         {
            m_cost_write_frz++;
            drsp.valid = false;
            drsp.rdata = 0;
            break;
         }
        // The next state and the processor request parameters are computed 
        // as in the DCACHE_IDLE state (see below ...)
    }    
    /////////////////
    case DCACHE_IDLE:
    {
        // instruction tlb miss
    	if ( r_itlb_read_dcache_req )
    	{
            data_t rsp_itlb_miss;
            data_t rsp_itlb_ppn;

    	    bool itlb_hit_dcache = r_dcache.read(r_icache_paddr_save, &rsp_itlb_miss);	

	        if ( (r_icache_fsm == ICACHE_TLB2_READ) && itlb_hit_dcache )
	        {	
	            bool itlb_hit_ppn = r_dcache.read(r_icache_paddr_save.read()+4, &rsp_itlb_ppn);
	            assert(itlb_hit_ppn && "Address of pte[64-32] and pte[31-0] should be successive");
	        }

    	    if ( itlb_hit_dcache ) // ins TLB request hits in data cache
    	    {
                r_dcache_rsp_itlb_miss = rsp_itlb_miss; 
                r_dcache_rsp_itlb_ppn = rsp_itlb_ppn;
    	    	r_itlb_read_dcache_req = false;
                r_dcache_fsm = DCACHE_IDLE;
    	    }
    	    else                    // ins TLB request miss in data cache
    	    {
                r_dcache_itlb_read_req = true;
                r_vci_rsp_data_ok = false; 
                r_dcache_fsm = DCACHE_ITLB_READ;
            }
    	}
    	else if ( r_itlb_acc_dcache_req )  // instruction tlb ET write
    	{
            r_dcache.write(r_icache_paddr_save, r_icache_pte_update);
            //assert(write_hit && "Write on miss ignores data");
            r_dcache_itlb_ll_acc_req = true;
            r_vci_rsp_data_ok = false; 
	        r_dcache_fsm = DCACHE_ITLB_LL_WAIT;	    		
    	}
        else if (dreq.valid) 
        {
            pte_info_t  dcache_pte_info;
            int         xtn_opcod      = (int)dreq.addr/4;
            paddr_t     tlb_dpaddr     = 0;        // physical address obtained from TLB
            paddr_t     spc_dpaddr     = 0;        // physical adress obtained from PPN_save (speculative)
            bool        dcache_hit_t   = false;    // hit on TLB
            bool        dcache_hit_x   = false;    // VPN unmodified (can use spc_dpaddr)
            bool        dcache_hit_p   = false;    // PTP unmodified (can skip first level page table walk)
            bool        dcache_hit_c   = false;    // Cache hit
            size_t      dcache_tlb_way = 0;        // selected way (in case of cache hit)
            size_t      dcache_tlb_set = 0;        // selected set (Y field in address)
            data_t      dcache_rdata   = 0;        // read data
            bool        dcache_cached  = false;    // cacheable access (read or write)
            m_cpt_dcache_data_read += m_dcache_ways;
            m_cpt_dcache_dir_read += m_dcache_ways;

            // Decoding READ XTN requests from processor
            // They are executed in this DCACHE_IDLE state

            if (dreq.type == iss_t::XTN_READ) 
            {
                switch(xtn_opcod) {
                case iss_t::XTN_INS_ERROR_TYPE:
                    drsp.rdata = (uint32_t)r_icache_error_type;
                    r_icache_error_type = MMU_NONE;
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_DATA_ERROR_TYPE:
                    drsp.rdata = (uint32_t)r_dcache_error_type;
                    r_dcache_error_type = MMU_NONE;
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_INS_BAD_VADDR:
                    drsp.rdata = (uint32_t)r_icache_bad_vaddr;       
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_DATA_BAD_VADDR:
                    drsp.rdata = (uint32_t)r_dcache_bad_vaddr;        
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_PTPR:
                    drsp.rdata = (uint32_t)r_mmu_ptpr;
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_TLB_MODE:
                    drsp.rdata = (uint32_t)r_mmu_mode;
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_MMU_PARAMS:
                    drsp.rdata = (uint32_t)r_mmu_params;
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_MMU_RELEASE:
                    drsp.rdata = (uint32_t)r_mmu_release;
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_MMU_WORD_LO:
                    drsp.rdata = (uint32_t)r_mmu_word_lo;
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                case iss_t::XTN_MMU_WORD_HI:
                    drsp.rdata = (uint32_t)r_mmu_word_hi;
                    drsp.valid = true;
                    drsp.error = false;
                    break;
                default:
                    r_dcache_error_type = MMU_READ_UNDEFINED_XTN; 
                    r_dcache_bad_vaddr  = dreq.addr;
                    drsp.valid = true;
                    drsp.error = true;
                    break;
                }
                r_dcache_fsm = DCACHE_IDLE;
                break;
            }

            // Decoding WRITE XTN requests from processor
            // If there is no privilege violation, they are not executed in this DCACHE_IDLE state,
            // but in the next state, because they generally require access to the caches or the TLBs 

            if (dreq.type == iss_t::XTN_WRITE) 
            {
                drsp.valid = false;
                drsp.error = false;
                drsp.rdata = 0;
                r_dcache_wdata_save = dreq.wdata;   
                switch(xtn_opcod) {     

                case iss_t::XTN_PTPR:       // context switch : checking the kernel mode
                                            // both instruction & data TLBs must be flushed
                    if ((dreq.mode == iss_t::MODE_HYPER) || (dreq.mode == iss_t::MODE_KERNEL)) 
                    {
                        r_mmu_ptpr = dreq.wdata;
                        r_icache_error_type = MMU_NONE;
                        r_dcache_error_type = MMU_NONE;
                        r_dcache_type_save = dreq.addr/4; 
                        r_dcache_xtn_req = true;
                        r_dcache_fsm = DCACHE_CTXT_SWITCH;
                    } 
                    else 
                    { 
                        r_dcache_error_type = MMU_WRITE_PRIVILEGE_VIOLATION; 
                        r_dcache_bad_vaddr  = dreq.addr;
                        drsp.valid = true;
                        drsp.error = true;
                        r_dcache_fsm = DCACHE_IDLE;
                    }
                    break;

                case iss_t::XTN_TLB_MODE:     // modifying TLBs mode : checking the kernel mode
                    if ((dreq.mode == iss_t::MODE_HYPER) || (dreq.mode == iss_t::MODE_KERNEL)) 
                    {
                        r_mmu_mode = (int)dreq.wdata;
                        drsp.valid = true;
                    } 
                    else 
                    {
                        r_dcache_error_type = MMU_WRITE_PRIVILEGE_VIOLATION; 
                        r_dcache_bad_vaddr  = dreq.addr;
                        drsp.valid = true;
                        drsp.error = true;
                    }
                    r_dcache_fsm = DCACHE_IDLE;
                    break;

                case iss_t::XTN_DTLB_INVAL:     //  checking the kernel mode
                    if ((dreq.mode == iss_t::MODE_HYPER) || (dreq.mode == iss_t::MODE_KERNEL)) 
                    {
                        r_dcache_fsm = DCACHE_DTLB_INVAL;  
                    } 
                    else 
                    {
                        r_dcache_error_type = MMU_WRITE_PRIVILEGE_VIOLATION; 
                        r_dcache_bad_vaddr  = dreq.addr;
                        drsp.valid = true;
                        drsp.error = true;
                        r_dcache_fsm = DCACHE_IDLE;
                    }
                    break;

                case iss_t::XTN_ITLB_INVAL:     //  checking the kernel mode
                    if ((dreq.mode == iss_t::MODE_HYPER) || (dreq.mode == iss_t::MODE_KERNEL)) 
                    {
                        r_dcache_xtn_req = true;
                        r_dcache_type_save = dreq.addr/4;
                        r_dcache_fsm = DCACHE_ITLB_INVAL;  
                    } 
                    else 
                    {
                        r_dcache_error_type = MMU_WRITE_PRIVILEGE_VIOLATION; 
                        r_dcache_bad_vaddr  = dreq.addr;
                        drsp.valid = true;
                        drsp.error = true;
                        r_dcache_fsm = DCACHE_IDLE;
                    }
                    break;

                case iss_t::XTN_DCACHE_INVAL:   // cache inval can be executed in user mode.
                    r_dcache_fsm = DCACHE_DCACHE_INVAL;
                    break;

                case iss_t::XTN_MMU_DCACHE_PA_INV:   // cache inval can be executed in user mode.
                    r_dcache_fsm = DCACHE_DCACHE_INVAL_PA;
                    break;

                case iss_t::XTN_DCACHE_FLUSH:   // cache flush can be executed in user mode.
                    r_dcache_type_save = dreq.addr/4; 
                    r_dcache_xtn_req = true;
                    r_dcache_fsm = DCACHE_DCACHE_FLUSH; 
                    break;

                case iss_t::XTN_ICACHE_INVAL:   // cache inval can be executed in user mode.
                    r_dcache_type_save = dreq.addr/4; 
                    r_dcache_xtn_req = true;
                    r_dcache_fsm = DCACHE_ICACHE_INVAL; 
                    break;

                case iss_t::XTN_MMU_ICACHE_PA_INV:   // cache inval can be executed in user mode.
                    r_dcache_type_save = dreq.addr/4; 
                    r_dcache_xtn_req = true;
                    r_dcache_fsm = DCACHE_ICACHE_INVAL_PA; 
                    break;

                case iss_t::XTN_ICACHE_FLUSH:   // cache flush can be executed in user mode.
                    r_dcache_type_save = dreq.addr/4; 
                    r_dcache_xtn_req = true; 
                    r_dcache_fsm = DCACHE_ICACHE_FLUSH;
                    break;

                case iss_t::XTN_SYNC:           // cache synchronization can be executed in user mode.
                    if (r_wbuf.rok())
                    {
                        r_dcache_fsm = DCACHE_DCACHE_SYNC; 
                    }
                    else
                    {
                        drsp.valid = true;
                        r_dcache_fsm = DCACHE_IDLE;
                    }
                    break;

                case iss_t::XTN_MMU_WORD_LO: // modifying MMU misc registers
                    if ((dreq.mode == iss_t::MODE_HYPER) || (dreq.mode == iss_t::MODE_KERNEL)) 
                    {
                        r_mmu_word_lo = (int)dreq.wdata;
                        drsp.valid = true;
                    } 
                    else 
                    {
                        r_dcache_error_type = MMU_WRITE_PRIVILEGE_VIOLATION; 
                        r_dcache_bad_vaddr  = dreq.addr;
                        drsp.valid = true;
                        drsp.error = true;
                    }
                    r_dcache_fsm = DCACHE_IDLE;
                    break;

                case iss_t::XTN_MMU_WORD_HI: // modifying MMU misc registers
                    if ((dreq.mode == iss_t::MODE_HYPER) || (dreq.mode == iss_t::MODE_KERNEL)) 
                    {
                        r_mmu_word_hi = (int)dreq.wdata;
                        drsp.valid = true;
                    } 
                    else 
                    {
                        r_dcache_error_type = MMU_WRITE_PRIVILEGE_VIOLATION; 
                        r_dcache_bad_vaddr  = dreq.addr;
                        drsp.valid = true;
                        drsp.error = true;
                    }
                    r_dcache_fsm = DCACHE_IDLE;
                    break;

                case iss_t::XTN_ICACHE_PREFETCH:
                case iss_t::XTN_DCACHE_PREFETCH:
                    drsp.valid = true;
                    r_dcache_fsm = DCACHE_IDLE;
                    break;

                default:
                    r_dcache_error_type = MMU_WRITE_UNDEFINED_XTN; 
                    r_dcache_bad_vaddr  = dreq.addr;
                    drsp.valid = true;
                    drsp.error = true;
                    r_dcache_fsm = DCACHE_IDLE;
                    break;
                } // end switch xtn_opcod

                break;
            } // end if XTN_WRITE

            // Evaluating dcache_hit_t, dcache_hit_x, dcache_hit_p, dcache_hit_c,
            // dcache_pte_info, dcache_tlb_way, dcache_tlb_set & dpaddr & cacheability 
            // - If MMU activated : cacheability is defined by the cachable bit in the TLB
            // - If MMU not activated : cacheability is defined by the segment table.

            if ( !(r_mmu_mode.read() & DATA_TLB_MASK) ) // MMU not activated
            {
                dcache_hit_t  = true;         
                dcache_hit_x  = true;   
                dcache_hit_p  = true;  
                tlb_dpaddr    = dreq.addr; 
                spc_dpaddr    = dreq.addr;    
                dcache_cached = m_cacheability_table[dreq.addr] && 
                                ((dreq.type != iss_t::DATA_LL)  && (dreq.type != iss_t::DATA_SC) &&
                                 (dreq.type != iss_t::XTN_READ) && (dreq.type != iss_t::XTN_WRITE));     
            } 
            else                                                            // MMU activated
            {
                m_cpt_data_tlb_read++;
                dcache_hit_t = dcache_tlb.translate(dreq.addr, &tlb_dpaddr, &dcache_pte_info, 
                                                    &dcache_tlb_way, &dcache_tlb_set); 
                dcache_hit_x   = (((vaddr_t)r_dcache_vpn_save << PAGE_K_NBITS) == (dreq.addr & ~PAGE_K_MASK)) && r_dtlb_translation_valid; 
                dcache_hit_p   = (((dreq.addr >> PAGE_M_NBITS) == r_dcache_id1_save) && r_dcache_ptba_ok );
                spc_dpaddr     = ((paddr_t)r_dcache_ppn_save << PAGE_K_NBITS) | (paddr_t)((dreq.addr & PAGE_K_MASK));
                dcache_cached  = dcache_pte_info.c && 
                                 ((dreq.type != iss_t::DATA_LL)  && (dreq.type != iss_t::DATA_SC) &&
                                  (dreq.type != iss_t::XTN_READ) && (dreq.type != iss_t::XTN_WRITE));    
            }

            if ( !(r_mmu_mode.read() & DATA_CACHE_MASK) )   // cache not actived
            {
                dcache_cached = false;
            }
 
            // dcache_hit_c & dcache_rdata
            if ( dcache_cached )    // using speculative physical address for cached access
            {
                dcache_hit_c = r_dcache.read(spc_dpaddr, &dcache_rdata);
            } 
            else                    // using actual physical address for uncached access
            {
                dcache_hit_c = ((tlb_dpaddr == (paddr_t)r_dcache_paddr_save) && r_vci_rsp_data_ok ); 
                dcache_rdata = r_dcache_miss_buf[0];
            }

            if ( r_mmu_mode.read() & DATA_TLB_MASK ) 
            {
                // Checking access rights
                if ( dcache_hit_t ) 
                {
                    if (!dcache_pte_info.u && (dreq.mode == iss_t::MODE_USER)) 
                    {
                        if ((dreq.type == iss_t::DATA_READ)||(dreq.type == iss_t::DATA_LL))
                        {
                            r_dcache_error_type = MMU_READ_PRIVILEGE_VIOLATION;
                        }
                        else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                        {
                            r_dcache_error_type = MMU_WRITE_PRIVILEGE_VIOLATION;
                        }  
                        r_dcache_bad_vaddr = dreq.addr;
                        drsp.valid = true;
                        drsp.error = true;
                        drsp.rdata = 0;
                        r_dcache_fsm = DCACHE_IDLE;
                        break;
                    }
                    if (!dcache_pte_info.w && ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))) 
                    {
                        r_dcache_error_type = MMU_WRITE_ACCES_VIOLATION;  
                        r_dcache_bad_vaddr = dreq.addr;
                        drsp.valid = true;
                        drsp.error = true;
                        drsp.rdata = 0;
                        r_dcache_fsm = DCACHE_IDLE;
                        break;
                    }
                }

                // update LRU, save ppn, vpn and page type
                if ( dcache_hit_t ) {
                    dcache_tlb.setlru(dcache_tlb_way,dcache_tlb_set); 
                    r_dcache_ppn_save = tlb_dpaddr >> PAGE_K_NBITS;
                    r_dcache_vpn_save = dreq.addr >> PAGE_K_NBITS;
                    r_dtlb_translation_valid = true;
                }
                else
                {
                    r_dtlb_translation_valid = false;
                }

            } // end if MMU activated

            // compute next state 
            if ( !dcache_hit_p && !dcache_hit_t )  // TLB miss
            {
                r_dcache_tlb_paddr = (paddr_t)r_mmu_ptpr << (INDEX1_NBITS+2) | (paddr_t)((dreq.addr>>PAGE_M_NBITS)<<2);
                r_dcache_fsm = DCACHE_DTLB1_READ_CACHE;
                m_cpt_data_tlb_miss++;
                m_cost_data_tlb_miss_frz++;
            }
            else if ( dcache_hit_p && !dcache_hit_t )  // TLB Miss with possibility of bypass first level page
            {
                // walk page table level 2
                r_dcache_tlb_paddr = (paddr_t)r_dcache_ptba_save | 
                                     (paddr_t)(((dreq.addr&PTD_ID2_MASK)>>PAGE_K_NBITS) << 3); 
                r_dcache_fsm = DCACHE_DTLB2_READ_CACHE;
                m_cpt_data_tlb_miss++;
                m_cost_data_tlb_miss_frz++;
            }
            else if ( dcache_hit_t && !dcache_hit_x && dcache_cached )// cached access with an ucorrect speculative physical address
            {
                r_dcache_hit_p_save = dcache_hit_p;
                r_dcache_fsm = DCACHE_BIS;
            }
            else  // cached or uncached access with a correct speculative physical address
            {
                switch( dreq.type ) {
                    case iss_t::DATA_READ:
                    case iss_t::DATA_LL:
                    case iss_t::DATA_SC:
                        m_cpt_read++;
                        if ( dcache_hit_c ) 
                        {
                            r_vci_rsp_data_ok = false;
                            r_dcache_fsm = DCACHE_IDLE;
                            drsp.valid = true;
                            drsp.rdata = dcache_rdata;
                        } 
                        else 
                        {
                            if ( dcache_cached ) 
                            {
                                r_dcache_miss_req = true;
                                r_vci_rsp_data_ok = false;
                                r_dcache_fsm = DCACHE_MISS_WAIT;
                                m_cpt_data_miss++;
                                m_cost_data_miss_frz++;
                            } 
                            else 
                            {
                                r_dcache_unc_req = true;
                                r_vci_rsp_data_ok = false;
                                r_dcache_fsm = DCACHE_UNC_WAIT;
                                m_cpt_unc_read++;
                                m_cost_unc_read_frz++;
                            }
                        }
                        break;
                    case iss_t::DATA_WRITE:
                        m_cpt_write++;
                        if ( dcache_cached ) m_cpt_write_cached++;

                        if ( dcache_hit_c && dcache_cached )    // cache update required
                        {
                            r_dcache_fsm = DCACHE_WRITE_UPDT;
                        } 
                        else if ((r_mmu_mode.read() & DATA_TLB_MASK) && !dcache_pte_info.d)   // dirty bit update required
                        {
                            if (dcache_tlb.getpagesize(dcache_tlb_way, dcache_tlb_set)) 
                            {
                                r_dcache_pte_update = dcache_tlb.getpte(dcache_tlb_way, dcache_tlb_set) | PTE_D_MASK;
                                r_dcache_tlb_paddr = (paddr_t)r_mmu_ptpr << (INDEX1_NBITS+2) | (paddr_t)((dreq.addr>>PAGE_M_NBITS)<<2);
                                r_dcache_tlb_ll_dirty_req = true;
                                r_vci_rsp_data_ok = false;
                                r_dcache_fsm = DCACHE_LL_DIRTY_WAIT;
                                m_cpt_data_tlb_write_dirty++;
                            }
                            else
                            {   
                                if (dcache_hit_p) 
                                {
                                    r_dcache_pte_update = dcache_tlb.getpte(dcache_tlb_way, dcache_tlb_set) | PTE_D_MASK;
                                    r_dcache_tlb_paddr = (paddr_t)r_dcache_ptba_save | (paddr_t)(((dreq.addr&PTD_ID2_MASK)>>PAGE_K_NBITS) << 3);
                                    r_dcache_tlb_ll_dirty_req = true;
                                    r_vci_rsp_data_ok = false;
                                    r_dcache_fsm = DCACHE_LL_DIRTY_WAIT;
                                    m_cpt_data_tlb_write_dirty++;
                                }
                                else    // get PTBA to calculate the physical address of PTE
                                {
                                    r_dcache_pte_update = dcache_tlb.getpte(dcache_tlb_way, dcache_tlb_set) | PTE_D_MASK;
                                    r_dcache_tlb_paddr = (paddr_t)r_mmu_ptpr << (INDEX1_NBITS+2) | (paddr_t)((dreq.addr>>PAGE_M_NBITS)<<2);
                                    r_dcache_tlb_ptba_read = true;
                                    r_dcache_fsm = DCACHE_DTLB1_READ_CACHE;
                                }
                            }
                        }
                        else                                    // no cache update, not dirty bit update
                        {
                            r_dcache_fsm = DCACHE_WRITE_REQ;
                            drsp.valid = true;
                            drsp.rdata = 0;
                        }
                        break;
                    default:
                        break;
                } // end switch dreq.type
            } // end if next states

            // save values for the next states
            r_dcache_paddr_save   = tlb_dpaddr;
            r_dcache_type_save    = dreq.type;
            r_dcache_wdata_save   = dreq.wdata;
            r_dcache_be_save      = dreq.be;
            r_dcache_rdata_save   = dcache_rdata;
            r_dcache_cached_save  = dcache_cached;
            r_dcache_dirty_save   = dcache_pte_info.d;
            r_dcache_tlb_set_save = dcache_tlb_set;
            r_dcache_tlb_way_save = dcache_tlb_way;

        } // end if dreq.valid
        else 
        {   
            r_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    /////////////////
    case DCACHE_BIS:
    {
        data_t      dcache_rdata = 0;       // read data
        bool        dcache_hit_c = false;   // cache hit
        bool        dcache_hit_t = false;   // TLB hit        
        paddr_t     tlb_dpaddr   = 0;       // physical address obtained from TLB    
 
        // processor address translation
        dcache_hit_t = dcache_tlb.translate(dreq.addr, &tlb_dpaddr);

        if ( (tlb_dpaddr == r_dcache_paddr_save.read()) && dreq.valid && dcache_hit_t )     // unmodified & valid
        {
            // acces always cached in this state
            dcache_hit_c = r_dcache.read(r_dcache_paddr_save, &dcache_rdata);
            
            if ( dreq.type == iss_t::DATA_READ )  // cached read
            {
                m_cpt_read++;
                if ( !dcache_hit_c ) 
                {
                    r_dcache_miss_req = true;
                    r_vci_rsp_data_ok = false;
                    r_dcache_fsm = DCACHE_MISS_WAIT;
                    m_cpt_data_miss++;
                    m_cost_data_miss_frz++;
                }
                else
                {
                    r_dcache_fsm = DCACHE_IDLE;
                }
                drsp.valid = dcache_hit_c;
                drsp.error = false;
                drsp.rdata = dcache_rdata;
            }
            else    // cached write
            {
                m_cpt_write++;
                m_cpt_write_cached++;
                if ( dcache_hit_c )    // cache update required
                {
                	r_dcache_rdata_save = dcache_rdata;
                    r_dcache_fsm = DCACHE_WRITE_UPDT;
                } 
                else if (!r_dcache_dirty_save && (r_mmu_mode.read() & DATA_TLB_MASK))   // dirty bit update required
                {
                    if (dcache_tlb.getpagesize(r_dcache_tlb_way_save, r_dcache_tlb_set_save)) 
                    {
                        r_dcache_pte_update = dcache_tlb.getpte(r_dcache_tlb_way_save, r_dcache_tlb_set_save) | PTE_D_MASK;
                        r_dcache_tlb_paddr = (paddr_t)r_mmu_ptpr << (INDEX1_NBITS+2) | (paddr_t)((dreq.addr>>PAGE_M_NBITS)<<2);
                        r_dcache_tlb_ll_dirty_req = true;
                        r_vci_rsp_data_ok = false;
                        r_dcache_fsm = DCACHE_LL_DIRTY_WAIT;
                        m_cpt_data_tlb_write_dirty++;
                    }
                    else
                    {   
                        if (r_dcache_hit_p_save) 
                        {
                            r_dcache_pte_update = dcache_tlb.getpte(r_dcache_tlb_way_save, r_dcache_tlb_set_save) | PTE_D_MASK;
                            r_dcache_tlb_paddr = (paddr_t)r_dcache_ptba_save|(paddr_t)(((dreq.addr&PTD_ID2_MASK)>>PAGE_K_NBITS) << 3);
                            r_dcache_tlb_ll_dirty_req = true;
                            r_vci_rsp_data_ok = false;
                            r_dcache_fsm = DCACHE_LL_DIRTY_WAIT;
                            m_cpt_data_tlb_write_dirty++;
                        }
                        else
                        {
                            r_dcache_pte_update = dcache_tlb.getpte(r_dcache_tlb_way_save, r_dcache_tlb_set_save) | PTE_D_MASK;
                            r_dcache_tlb_paddr = (paddr_t)r_mmu_ptpr << (INDEX1_NBITS+2) | (paddr_t)((dreq.addr>>PAGE_M_NBITS)<<2);
                            r_dcache_tlb_ptba_read = true;
                            r_dcache_fsm = DCACHE_DTLB1_READ_CACHE;
                        }
                    }
                }
                else                                    // no cache update, not dirty bit update
                {
                    r_dcache_fsm = DCACHE_WRITE_REQ;
                    drsp.valid = true;
                    drsp.rdata = 0;
                }
            }
        }
        else                // modified or invalid
        {
            drsp.valid = false;
            drsp.error = false;
            drsp.rdata = 0;
            r_dcache_fsm = DCACHE_IDLE;
        }
        break;
    }
    //////////////////////////
    case DCACHE_LL_DIRTY_WAIT:
    {
        if (r_vci_rsp_data_ok)
        {
            if ( r_vci_rsp_data_error ) // VCI response ko
            {
                if (dcache_tlb.getpagesize(r_dcache_tlb_way_save, r_dcache_tlb_set_save)) 
                {
                    r_dcache_error_type = MMU_WRITE_PT1_ILLEGAL_ACCESS;     
                }
                else
                {
                    r_dcache_error_type = MMU_WRITE_PT2_ILLEGAL_ACCESS;     
                }
                r_dcache_bad_vaddr = dreq.addr;
                r_dcache_fsm = DCACHE_ERROR; 
            }
            else
            {
                if ( !(r_dcache_miss_buf[0] >> PTE_V_SHIFT) )	// unmapped
                {
        	        if (dcache_tlb.getpagesize(r_dcache_tlb_way_save, r_dcache_tlb_set_save))
        	        { 
        	            r_dcache_error_type = MMU_WRITE_PT1_UNMAPPED;       
        	        }
        	        else
        	        {
        	            r_dcache_error_type = MMU_WRITE_PT2_UNMAPPED;       
          	        }
                    r_dcache_bad_vaddr = dreq.addr;
                    r_dcache_fsm = DCACHE_ERROR;
                }
        	    else
        	    {
        	        r_dcache_tlb_sc_dirty_req = true;
                    r_vci_rsp_data_ok = false;
                    r_dcache_pte_update = r_dcache_miss_buf[0] | r_dcache_pte_update.read();
                    r_dcache_fsm = DCACHE_SC_DIRTY_WAIT; 
        	    }
            }
        }
        break;
    }
    //////////////////////////
    case DCACHE_SC_DIRTY_WAIT:
    {
        if ( r_vci_rsp_data_ok && r_vci_rsp_data_error ) // VCI response ko
	    {
	        if (dcache_tlb.getpagesize(r_dcache_tlb_way_save, r_dcache_tlb_set_save))
	        {
	            r_dcache_error_type = MMU_WRITE_PT1_ILLEGAL_ACCESS;    
	        }
	        else
	        {
	            r_dcache_error_type = MMU_WRITE_PT2_ILLEGAL_ACCESS;    
 	        }
	        r_dcache_bad_vaddr = dreq.addr;
	        r_dcache_fsm = DCACHE_ERROR; 
        }
        else if ( r_vci_rsp_data_ok && r_dcache_tlb_ll_dirty_req )
        {
            r_vci_rsp_data_ok = false;
            r_dcache_fsm = DCACHE_LL_DIRTY_WAIT; 
        }
        else if ( r_vci_rsp_data_ok )
        {
            r_vci_rsp_data_ok = false;
            r_dcache_fsm = DCACHE_WRITE_DIRTY; 
        }
        break;
    }
    ////////////////////////////
    case DCACHE_DTLB1_READ_CACHE:
    {
        data_t tlb_data;
        bool tlb_hit_cache = r_dcache.read(r_dcache_tlb_paddr, &tlb_data);
        bool write_hit = false;

        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;

        // DTLB request hit in cache
        if ( tlb_hit_cache )
        {
	        if ( !(tlb_data >> PTE_V_SHIFT) )	// unmapped
	        {
                r_dcache_ptba_ok    = false;
                if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
                {
                    r_dcache_error_type = MMU_READ_PT1_UNMAPPED;
                }
                else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                {
                    r_dcache_error_type = MMU_WRITE_PT1_UNMAPPED;
                }  
                r_dcache_bad_vaddr  = dreq.addr;
                r_dcache_fsm        = DCACHE_ERROR;
	        }
	        else if ( (tlb_data & PTE_T_MASK) >> PTE_T_SHIFT )	// PTD
	        {
                r_dcache_ptba_ok   = true;
                r_dcache_ptba_save = (paddr_t)(tlb_data & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1)) << PAGE_K_NBITS;  
                r_dcache_id1_save  = dreq.addr >> PAGE_M_NBITS;
                r_dcache_tlb_paddr = (paddr_t)(tlb_data & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1)) << PAGE_K_NBITS | 
                                     (paddr_t)(((dreq.addr & PTD_ID2_MASK) >> PAGE_K_NBITS) << 3);
                if ( r_dcache_tlb_ptba_read )
                {
                    r_dcache_tlb_ptba_read = false;
                    write_hit = r_dcache.write(((paddr_t)(tlb_data & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1)) << PAGE_K_NBITS | 
                                           (paddr_t)(((dreq.addr & PTD_ID2_MASK) >> PAGE_K_NBITS) << 3)), r_dcache_pte_update);
                    //assert(write_hit && "Write on miss ignores data");
                    r_dcache_tlb_ll_dirty_req = true;
                    r_vci_rsp_data_ok = false;
                    r_dcache_fsm = DCACHE_LL_DIRTY_WAIT;
                    m_cpt_data_tlb_write_dirty++;
                }
                else
                {
                    r_dcache_fsm = DCACHE_DTLB2_READ_CACHE;
                }
	        }
	        else	// PTE
	        {
                r_dcache_ptba_ok = false;
	            if ( (m_srcid >> 4) == ((r_dcache_tlb_paddr.read() & ((1<<(m_paddr_nbits - PAGE_M_NBITS))-1)) >> (m_paddr_nbits - PAGE_M_NBITS -10)) ) // local
	            {
	                if ( (tlb_data & PTE_L_MASK ) >> PTE_L_SHIFT ) // L bit is set
	                {
                        r_dcache_pte_update = tlb_data;
                        r_dcache_fsm = DCACHE_TLB1_UPDT;
	                }
	                else
	                {
                        r_dcache_pte_update = tlb_data | PTE_L_MASK;
                        r_dcache_tlb_ll_acc_req = true;
                        r_vci_rsp_data_ok = false;
                        write_hit = r_dcache.write(r_dcache_tlb_paddr,(tlb_data | PTE_L_MASK));  
                        assert(write_hit && "Write on miss ignores data");  
                        r_dcache_fsm = DCACHE_TLB1_LL_WAIT;
                        m_cpt_ins_tlb_write_et++;
	                }
                }
	            else // remotely
	            {
	                if ( (tlb_data & PTE_R_MASK ) >> PTE_R_SHIFT ) // R bit is set
	                {
                        r_dcache_pte_update = tlb_data;
                        r_dcache_fsm = DCACHE_TLB1_UPDT;
	                }
	                else
	                {
                        r_dcache_pte_update = tlb_data | PTE_R_MASK;
                        r_dcache_tlb_ll_acc_req = true;
                        r_vci_rsp_data_ok = false;
                        write_hit = r_dcache.write(r_dcache_tlb_paddr,(tlb_data | PTE_R_MASK));  
                        assert(write_hit && "Write on miss ignores data");  
                        r_dcache_fsm = DCACHE_TLB1_LL_WAIT;
                        m_cpt_ins_tlb_write_et++;
	                }
	            }
	        }
        }
        else
        {
            // DTLB request miss in cache and walk page table level 1
            r_dcache_tlb_read_req = true;
            r_vci_rsp_data_ok = false;
            r_dcache_fsm = DCACHE_TLB1_READ;
        }
        break;
    }
    ///////////////////////
    case DCACHE_TLB1_LL_WAIT:
    {
	    if (r_vci_rsp_data_ok)
	    {
            if ( r_vci_rsp_data_error ) // VCI response ko
            {
                if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
                {
                    r_dcache_error_type = MMU_READ_PT1_ILLEGAL_ACCESS;
                }
                else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                {
                    r_dcache_error_type = MMU_WRITE_PT1_ILLEGAL_ACCESS;
                } 
                r_dcache_bad_vaddr = dreq.addr;
                r_dcache_fsm = DCACHE_ERROR; 
            }
	        else
	        {
	            if ( !(r_dcache_miss_buf[0] >> PTE_V_SHIFT) )	// unmapped
	            {
                    if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
                    {
                        r_dcache_error_type = MMU_READ_PT1_UNMAPPED;
                    }
                    else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                    {
                        r_dcache_error_type = MMU_WRITE_PT1_UNMAPPED;
                    } 
                    r_dcache_bad_vaddr  = dreq.addr;
                    r_dcache_fsm        = DCACHE_ERROR;
	            }
		        else
		        {
		            r_dcache_tlb_sc_acc_req = true;
                    r_vci_rsp_data_ok = false;
                    r_dcache_pte_update = r_dcache_miss_buf[0] | r_dcache_pte_update.read();
                    r_dcache_fsm = DCACHE_TLB1_SC_WAIT; 
		        }
	        }
	    }
	    break;
    }
    ///////////////////////
    case DCACHE_TLB1_SC_WAIT:
    {
        if ( r_vci_rsp_data_ok && r_vci_rsp_data_error ) // VCI response ko
	    {
            if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
            {
                r_dcache_error_type = MMU_READ_PT1_ILLEGAL_ACCESS;
            }
            else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
            {
                r_dcache_error_type = MMU_WRITE_PT1_ILLEGAL_ACCESS;
            } 
	        r_dcache_bad_vaddr = dreq.addr;
	        r_dcache_fsm = DCACHE_ERROR; 
	    }
	    else if ( r_vci_rsp_data_ok && r_dcache_tlb_ll_acc_req )
	    {
            r_vci_rsp_data_ok = false;
	        r_dcache_fsm = DCACHE_TLB1_LL_WAIT; 
	    }
	    else if ( r_vci_rsp_data_ok )
	    {   
            r_vci_rsp_data_ok = false;
	        r_dcache_fsm = DCACHE_TLB1_UPDT; 
	    }
	    break;
    }
    //////////////////////
    case DCACHE_TLB1_READ:
    {
        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;

        if ( r_vci_rsp_data_ok )     
        {
            if ( r_vci_rsp_data_error ) // VCI response error
            {
                if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
                {
                    r_dcache_error_type = MMU_READ_PT1_ILLEGAL_ACCESS;
                }
                else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                {
                    r_dcache_error_type = MMU_WRITE_PT1_ILLEGAL_ACCESS;
                } 
                r_dcache_bad_vaddr = dreq.addr;
                r_dcache_fsm = DCACHE_ERROR; 
            }
            else                        // VCI response ok
            {
                r_vci_rsp_data_ok = false;
                r_dcache_fsm = DCACHE_TLB1_READ_UPDT; 
            }
        }
        break;
    }
    //////////////////////////
    case DCACHE_TLB1_READ_UPDT:
    {
        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;

        // update dcache
        data_t   rsp_dtlb_miss;
        paddr_t  victim_index = 0;
        bool write_hit = false;
        r_dcache.update(r_dcache_tlb_paddr, r_dcache_miss_buf, &victim_index);
        r_dcache.read(r_dcache_tlb_paddr, &rsp_dtlb_miss);
	
	    if ( !(rsp_dtlb_miss >> PTE_V_SHIFT) )	// unmapped
	    {
            r_dcache_ptba_ok    = false;
            if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
            {
                r_dcache_error_type = MMU_READ_PT1_UNMAPPED;
            }
            else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
            {
                r_dcache_error_type = MMU_WRITE_PT1_UNMAPPED;
            } 
            r_dcache_bad_vaddr  = dreq.addr;
            r_dcache_fsm        = DCACHE_ERROR;
	    }
	    else if ( (rsp_dtlb_miss & PTE_T_MASK) >> PTE_T_SHIFT ) // PTD
	    {
            r_dcache_ptba_ok   = true;
            r_dcache_ptba_save = (paddr_t)(rsp_dtlb_miss & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1)) << PAGE_K_NBITS;  
            r_dcache_id1_save  = dreq.addr >> PAGE_M_NBITS;
            r_dcache_tlb_paddr = (paddr_t)(rsp_dtlb_miss & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1)) << PAGE_K_NBITS | 
                                 (paddr_t)(((dreq.addr & PTD_ID2_MASK) >> PAGE_K_NBITS) << 3);
            if ( r_dcache_tlb_ptba_read )
            {
                r_dcache_tlb_ptba_read = false;
                write_hit = r_dcache.write(((paddr_t)(rsp_dtlb_miss & ((1<<(m_paddr_nbits - PAGE_K_NBITS))-1)) << PAGE_K_NBITS | 
                                       (paddr_t)(((dreq.addr & PTD_ID2_MASK) >> PAGE_K_NBITS) << 3)), r_dcache_pte_update);
                //assert(write_hit && "Write on miss ignores data");
                r_dcache_tlb_ll_dirty_req = true;
                r_vci_rsp_data_ok = false;
                r_dcache_fsm = DCACHE_LL_DIRTY_WAIT;
                m_cpt_data_tlb_write_dirty++;
            }
            else
            {
                r_dcache_fsm = DCACHE_DTLB2_READ_CACHE;
            }
	    }
	    else	// PTE
	    {
            r_dcache_ptba_ok = false;
	        if ( (m_srcid >> 4) == ((r_dcache_tlb_paddr.read() & ((1<<(m_paddr_nbits - PAGE_M_NBITS))-1)) >> (m_paddr_nbits - PAGE_M_NBITS -10)) ) // local
		    {
		        if ( (rsp_dtlb_miss & PTE_L_MASK ) >> PTE_L_SHIFT ) // L bit is set
		        {
                    r_dcache_pte_update = rsp_dtlb_miss;
                    r_dcache_fsm        = DCACHE_TLB1_UPDT;
		        }
		        else
		        {
                    r_dcache_pte_update = rsp_dtlb_miss | PTE_L_MASK;
                    r_dcache_tlb_ll_acc_req = true;
                    r_vci_rsp_data_ok = false;
                	write_hit = r_dcache.write(r_dcache_tlb_paddr,(rsp_dtlb_miss | PTE_L_MASK));  
                	assert(write_hit && "Write on miss ignores data");  
                    r_dcache_fsm        = DCACHE_TLB1_LL_WAIT;
                    m_cpt_ins_tlb_write_et++;
		        }
    	    }
		    else // remotely
		    {
		        if ( (rsp_dtlb_miss & PTE_R_MASK ) >> PTE_R_SHIFT ) // R bit is set
		        {
                    r_dcache_pte_update = rsp_dtlb_miss;
                    r_dcache_fsm        = DCACHE_TLB1_UPDT;
		        }
		        else
		        {
                    r_dcache_pte_update = rsp_dtlb_miss | PTE_R_MASK;
                    r_dcache_tlb_ll_acc_req = true;
                    r_vci_rsp_data_ok = false;
                	write_hit = r_dcache.write(r_dcache_tlb_paddr,(rsp_dtlb_miss | PTE_R_MASK));  
                	assert(write_hit && "Write on miss ignores data");  
                    r_dcache_fsm        = DCACHE_TLB1_LL_WAIT;
                    m_cpt_ins_tlb_write_et++;
		        }
		    }
	    }
        break;
    }
    //////////////////////
    case DCACHE_TLB1_UPDT: 
    {
        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;

        dcache_tlb.update(r_dcache_pte_update,dreq.addr);
        r_dcache_fsm = DCACHE_IDLE;
        break;
    }
    /////////////////////////////
    case DCACHE_DTLB2_READ_CACHE:
    {
        data_t tlb_data = 0;
        data_t tlb_data_ppn = 0;
        bool write_hit = false;
        bool tlb_hit_cache = r_dcache.read(r_dcache_tlb_paddr, &tlb_data);

        if ( tlb_hit_cache )
        {
            bool tlb_hit_ppn = r_dcache.read(r_dcache_tlb_paddr.read()+4, &tlb_data_ppn);
            assert(tlb_hit_ppn && "Address of pte[64-32] and pte[31-0] should be successive");
        }

        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;

        // DTLB request hit in cache
        if ( tlb_hit_cache )
        {
	        if ( !(tlb_data >> PTE_V_SHIFT) )	// unmapped
	        {
                if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
                {
                    r_dcache_error_type = MMU_READ_PT2_UNMAPPED;
                }
                else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                {
                    r_dcache_error_type = MMU_WRITE_PT2_UNMAPPED;
                } 
                r_dcache_bad_vaddr  = dreq.addr;
                r_dcache_fsm        = DCACHE_ERROR;
	        }
	        else if ( (tlb_data & PTE_T_MASK) >> PTE_T_SHIFT ) //PTD
	        {
                r_dcache_pte_update = tlb_data;
	            r_dcache_ppn_update = tlb_data_ppn;
		        r_dcache_fsm = DCACHE_TLB2_UPDT;
	        }
            else
            {
	            if ( (m_srcid >> 4) == ((r_dcache_tlb_paddr.read() & ((1<<(m_paddr_nbits - PAGE_M_NBITS))-1)) >> (m_paddr_nbits - PAGE_M_NBITS -10)) ) // local
		        {
		            if ( (tlb_data & PTE_L_MASK ) >> PTE_L_SHIFT ) // L bit is set
		            {
                        r_dcache_pte_update = tlb_data;
			            r_dcache_ppn_update = tlb_data_ppn;
                        r_dcache_fsm        = DCACHE_TLB2_UPDT;
		            }
		            else
		            {
                        r_dcache_pte_update = tlb_data | PTE_L_MASK;
	        	        r_dcache_ppn_update = tlb_data_ppn;
                        r_dcache_tlb_ll_acc_req = true;
                        r_vci_rsp_data_ok = false;
                	    write_hit = r_dcache.write(r_dcache_tlb_paddr,(tlb_data | PTE_L_MASK));  
                	    assert(write_hit && "Write on miss ignores data");  
                        r_dcache_fsm = DCACHE_TLB2_LL_WAIT;
                        m_cpt_ins_tlb_write_et++;
		            }
    	        }
		        else // remotely
		        {
		            if ( (tlb_data & PTE_R_MASK ) >> PTE_R_SHIFT ) // R bit is set
		            {
                        r_dcache_pte_update = tlb_data;
			            r_dcache_ppn_update = tlb_data_ppn;
                        r_dcache_fsm        = DCACHE_TLB2_UPDT;
		            }
		            else
		            {
                        r_dcache_pte_update = tlb_data | PTE_R_MASK;
	        	        r_dcache_ppn_update = tlb_data_ppn;
                        r_dcache_tlb_ll_acc_req = true;
                        r_vci_rsp_data_ok = false;
                	    write_hit = r_dcache.write(r_dcache_tlb_paddr,(tlb_data | PTE_R_MASK));  
                	    assert(write_hit && "Write on miss ignores data");  
                        r_dcache_fsm = DCACHE_TLB2_LL_WAIT;
                        m_cpt_ins_tlb_write_et++;
		            }
		        }
            }
        }
        else
        {
            // DTLB request miss in cache and walk page table level 2
            r_dcache_tlb_read_req = true;
            r_vci_rsp_data_ok = false;
            r_dcache_fsm = DCACHE_TLB2_READ;
        }
        break;
    }
    ///////////////////////
    case DCACHE_TLB2_LL_WAIT:
    {
	    if (r_vci_rsp_data_ok)
	    {
            if ( r_vci_rsp_data_error ) // VCI response ko
            {
                if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
                {
                    r_dcache_error_type = MMU_READ_PT2_ILLEGAL_ACCESS;
                }
                else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                {
                    r_dcache_error_type = MMU_WRITE_PT2_ILLEGAL_ACCESS;
                } 
                r_dcache_bad_vaddr = dreq.addr;
                r_dcache_fsm = DCACHE_ERROR; 
            }
	        else
	        {
	            if ( !(r_dcache_miss_buf[0] >> PTE_V_SHIFT) )	// unmapped
	            {
                    if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
                    {
                        r_dcache_error_type = MMU_READ_PT2_UNMAPPED;
                    }
                    else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                    {
                        r_dcache_error_type = MMU_WRITE_PT2_UNMAPPED;
                    }       
                    r_dcache_bad_vaddr = dreq.addr;
                    r_dcache_fsm = DCACHE_ERROR;
	            }
		        else
		        {
		            r_dcache_tlb_sc_acc_req = true;
                    r_vci_rsp_data_ok = false;
                    r_dcache_pte_update = r_dcache_miss_buf[0] | r_dcache_pte_update.read();
                    r_dcache_fsm = DCACHE_TLB2_SC_WAIT; 
		        }
	        }
	    }
	    break;
    }
    ///////////////////////
    case DCACHE_TLB2_SC_WAIT:
    {
        if ( r_vci_rsp_data_ok && r_vci_rsp_data_error ) // VCI response ko
	    {
            if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
            {
                r_dcache_error_type = MMU_READ_PT2_ILLEGAL_ACCESS;
            }
            else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
            {
                r_dcache_error_type = MMU_WRITE_PT2_ILLEGAL_ACCESS;
            } 
	        r_dcache_bad_vaddr = dreq.addr;
	        r_dcache_fsm = DCACHE_ERROR; 
	    }
	    else if ( r_vci_rsp_data_ok && r_dcache_tlb_ll_acc_req )
	    {
            r_vci_rsp_data_ok = false;
	        r_dcache_fsm = DCACHE_TLB2_LL_WAIT; 
	    }
	    else if ( r_vci_rsp_data_ok )
	    {
            r_vci_rsp_data_ok = false;
	        r_dcache_fsm = DCACHE_TLB2_UPDT; 
	    }
	    break;
    }
    /////////////////////
    case DCACHE_TLB2_READ:
    {
        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;

        if ( r_vci_rsp_data_ok )  
        {
            if ( r_vci_rsp_data_error ) // VCI response error
            {
                if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
                {
                    r_dcache_error_type = MMU_READ_PT2_ILLEGAL_ACCESS;
                }
                else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
                {
                    r_dcache_error_type = MMU_WRITE_PT2_ILLEGAL_ACCESS;
                } 
                r_dcache_bad_vaddr = dreq.addr;
                r_dcache_fsm = DCACHE_ERROR;
            }
            else                        // VCI response ok
            {
                r_vci_rsp_data_ok = false;
                r_dcache_fsm = DCACHE_TLB2_READ_UPDT;
            }
        }
        break;
    }
    //////////////////////////
    case DCACHE_TLB2_READ_UPDT:
    {
        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;

        // update cache
        data_t rsp_dtlb_miss = 0;
        data_t tlb_data_ppn = 0;
        paddr_t  victim_index = 0;
        bool write_hit = false;

        r_dcache.update(r_dcache_tlb_paddr, r_dcache_miss_buf, &victim_index);
        r_dcache.read(r_dcache_tlb_paddr, &rsp_dtlb_miss);	

        bool tlb_hit_ppn = r_dcache.read(r_dcache_tlb_paddr.read()+4, &tlb_data_ppn);	
	    assert(tlb_hit_ppn && "Address of pte[64-32] and pte[31-0] should be successive");

	    if ( !(rsp_dtlb_miss >> PTE_V_SHIFT) )	// unmapped
	    {
            if ((r_dcache_type_save == iss_t::DATA_READ)||(r_dcache_type_save == iss_t::DATA_LL))
            {
                r_dcache_error_type = MMU_READ_PT2_UNMAPPED;
            }
            else /*if ((dreq.type == iss_t::DATA_WRITE)||(dreq.type == iss_t::DATA_SC))*/
            {
                r_dcache_error_type = MMU_WRITE_PT2_UNMAPPED;
            } 
            r_dcache_bad_vaddr  = dreq.addr;
            r_dcache_fsm        = DCACHE_ERROR;
	    }
	    else if ( (rsp_dtlb_miss & PTE_T_MASK) >> PTE_T_SHIFT ) // PTD
	    {
            r_dcache_pte_update = rsp_dtlb_miss;
	        r_dcache_ppn_update = tlb_data_ppn;
		    r_dcache_fsm = DCACHE_TLB2_UPDT;
	    }
        else
        {
	        if ( (m_srcid >> 4) == ((r_dcache_tlb_paddr.read() & ((1<<(m_paddr_nbits - PAGE_M_NBITS))-1)) >> (m_paddr_nbits - PAGE_M_NBITS -10)) ) // local
		    {
		        if ( (rsp_dtlb_miss & PTE_L_MASK ) >> PTE_L_SHIFT ) // L bit is set
		        {
                    r_dcache_pte_update = rsp_dtlb_miss;
			        r_dcache_ppn_update = tlb_data_ppn;
                    r_dcache_fsm        = DCACHE_TLB2_UPDT;
		        }
		        else
		        {
                    r_dcache_pte_update = rsp_dtlb_miss | PTE_L_MASK;
	        	    r_dcache_ppn_update = tlb_data_ppn;
                    r_dcache_tlb_ll_acc_req = true;
                    r_vci_rsp_data_ok = false;
                	write_hit = r_dcache.write(r_dcache_tlb_paddr,(rsp_dtlb_miss | PTE_L_MASK));  
                	assert(write_hit && "Write on miss ignores data");  
                    r_dcache_fsm = DCACHE_TLB2_LL_WAIT;
                    m_cpt_ins_tlb_write_et++;
		        }
    	    }
		    else // remotely
		    {
		        if ( (rsp_dtlb_miss & PTE_R_MASK ) >> PTE_R_SHIFT ) // R bit is set
		        {
                    r_dcache_pte_update = rsp_dtlb_miss;
			        r_dcache_ppn_update = tlb_data_ppn;
                    r_dcache_fsm        = DCACHE_TLB2_UPDT;
		        }
		        else
		        {
                    r_dcache_pte_update = rsp_dtlb_miss | PTE_R_MASK;
	        	    r_dcache_ppn_update = tlb_data_ppn;
                    r_dcache_tlb_ll_acc_req = true;
                    r_vci_rsp_data_ok = false;
                	write_hit = r_dcache.write(r_dcache_tlb_paddr,(rsp_dtlb_miss | PTE_R_MASK));  
                	assert(write_hit && "Write on miss ignores data");  
                    r_dcache_fsm = DCACHE_TLB2_LL_WAIT;
                    m_cpt_ins_tlb_write_et++;
		        }
		    }
        }
        break;
    }
    //////////////////////
    case DCACHE_TLB2_UPDT:  
    {
        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;
        dcache_tlb.update(r_dcache_pte_update,r_dcache_ppn_update,dreq.addr);
        r_dcache_fsm = DCACHE_IDLE;
        break;
    }
    ///////////////////////
    case DCACHE_CTXT_SWITCH:
    {
        dcache_tlb.flush(false);      // global entries are not invalidated   
        if ( !r_dcache_xtn_req ) 
        {
            r_dcache_fsm = DCACHE_IDLE;
            r_dtlb_translation_valid = false;
            r_dcache_ptba_ok = false;
            drsp.valid = true; 
        }
        break;
    }
    ////////////////////////
    case DCACHE_ICACHE_FLUSH:
    case DCACHE_ICACHE_INVAL:
    case DCACHE_ICACHE_INVAL_PA:
    case DCACHE_ITLB_INVAL:
    {
        if ( !r_dcache_xtn_req ) 
        {
            r_dcache_fsm = DCACHE_IDLE;
            drsp.valid = true;
        }
        break;
    }
    ////////////////////////
    case DCACHE_DCACHE_FLUSH:
    {
        r_dcache.reset();
        dcache_tlb.flush(true);

        if ( !r_dcache_xtn_req )
        {
            r_dcache_fsm = DCACHE_IDLE;
            drsp.valid = true;
        }
        break;
    }
    //////////////////////
    case DCACHE_DTLB_INVAL: 
    {
        dcache_tlb.inval(r_dcache_wdata_save);
        r_dtlb_translation_valid = false;
        r_dcache_ptba_ok = false;
        r_dcache_fsm = DCACHE_IDLE;
        drsp.valid = true;
        break;
    }
    ////////////////////////
    case DCACHE_DCACHE_INVAL:
    {
        m_cpt_dcache_dir_read += m_dcache_ways;
        vaddr_t invadr = dreq.wdata;
        paddr_t dpaddr;
        bool dcache_hit_t = false; 

        if ( r_mmu_mode.read() & DATA_TLB_MASK ) 
        {
            dcache_hit_t = dcache_tlb.translate(invadr, &dpaddr); 
        } 
        else 
        {
            dpaddr = invadr;  
            dcache_hit_t = true;
        }

        if ( dcache_hit_t )
        {
            r_dcache.inval(dpaddr);
        }
        r_dcache_fsm = DCACHE_IDLE;
        drsp.valid = true;
        break;
    }
    ////////////////////////
    case DCACHE_DCACHE_INVAL_PA:
    {
        m_cpt_dcache_dir_read += m_dcache_ways;
        paddr_t dpaddr = (paddr_t)r_mmu_word_hi.read() << 32 | r_mmu_word_lo.read();

        r_dcache.inval(dpaddr);
        r_dcache_fsm = DCACHE_IDLE;
        drsp.valid = true;
        break;
    }
    /////////////////////////
    case DCACHE_DCACHE_SYNC:
    {
        if ( r_wbuf.empty() ) 
        {    
            drsp.valid = true;
            r_dcache_fsm = DCACHE_IDLE;  
        }      
        break;
    }
    /////////////////////
    case DCACHE_MISS_WAIT:
    {
        if ( dreq.valid ) m_cost_data_miss_frz++; 

        if ( r_vci_rsp_data_ok ) 
        {
            if ( r_vci_rsp_data_error ) 
            {
                r_dcache_error_type = MMU_READ_DATA_ILLEGAL_ACCESS; 
                r_dcache_bad_vaddr = dreq.addr;
                r_dcache_fsm = DCACHE_ERROR;
            } 
            else 
            {
                r_dcache_fsm = DCACHE_MISS_UPDT; 
            } 
        } 
        break;
    }
    /////////////////////
    case DCACHE_MISS_UPDT:
    {
        paddr_t  victim_index = 0;
        if ( dreq.valid ) 
            m_cost_data_miss_frz++;
        m_cpt_dcache_data_write++;
        m_cpt_dcache_dir_write++;
        r_dcache.update(r_dcache_paddr_save, r_dcache_miss_buf, &victim_index);
        r_dcache_fsm = DCACHE_IDLE;
        break;
    }
    //////////////////////
    case DCACHE_UNC_WAIT:
    {
        if ( dreq.valid ) m_cost_unc_read_frz++;

        if ( r_vci_rsp_data_ok ) 
        {
            if ( r_vci_rsp_data_error ) 
            {
                r_dcache_error_type = MMU_READ_DATA_ILLEGAL_ACCESS; 
                r_dcache_bad_vaddr = dreq.addr;
                r_dcache_fsm = DCACHE_ERROR;
            } 
            else 
            {
                r_dcache_fsm = DCACHE_IDLE;
                // Special case : if request was a DATA_SC, we need to invalidate 
                // the corresponding cache line, so that subsequent access to this line
                // are correctly directed to RAM
                if(dreq.type == iss_t::DATA_SC) 
                {
                    // Simulate an invalidate request
                    r_dcache.inval(r_dcache_paddr_save);
                }
            } 
        }
        break;
    }
    ///////////////////////
    case DCACHE_WRITE_UPDT:
    {
        m_cpt_dcache_data_write++;
        data_t mask = vci_param::be2mask(r_dcache_be_save.read());
        data_t wdata = (mask & r_dcache_wdata_save) | (~mask & r_dcache_rdata_save);
        bool write_hit = r_dcache.write(r_dcache_paddr_save, wdata);
        assert(write_hit && "Write on miss ignores data");

        if ( !r_dcache_dirty_save && (r_mmu_mode.read() & DATA_TLB_MASK))   
        {
            if ( dcache_tlb.getpagesize(r_dcache_tlb_way_save, r_dcache_tlb_set_save) )	// 2M page size, one level page table 
            {
                r_dcache_pte_update = dcache_tlb.getpte(r_dcache_tlb_way_save, r_dcache_tlb_set_save) | PTE_D_MASK;
                r_dcache_tlb_paddr = (paddr_t)r_mmu_ptpr << (INDEX1_NBITS+2) | (paddr_t)((dreq.addr>>PAGE_M_NBITS)<<2);
                r_dcache_tlb_ll_dirty_req = true;
                r_vci_rsp_data_ok = false;
                r_dcache_fsm = DCACHE_LL_DIRTY_WAIT;
                m_cpt_data_tlb_write_dirty++;
            }
            else
            {   
                if (r_dcache_hit_p_save) 
                {
                    r_dcache_pte_update = dcache_tlb.getpte(r_dcache_tlb_way_save, r_dcache_tlb_set_save) | PTE_D_MASK;
                    r_dcache_tlb_paddr = (paddr_t)r_dcache_ptba_save|(paddr_t)(((dreq.addr&PTD_ID2_MASK)>>PAGE_K_NBITS) << 3);
                    r_dcache_tlb_ll_dirty_req = true;
                    r_vci_rsp_data_ok = false;
                    r_dcache_fsm = DCACHE_LL_DIRTY_WAIT;
                    m_cpt_data_tlb_write_dirty++;
                }
                else
                {
                    r_dcache_pte_update = dcache_tlb.getpte(r_dcache_tlb_way_save, r_dcache_tlb_set_save) | PTE_D_MASK;
                    r_dcache_tlb_paddr = (paddr_t)r_mmu_ptpr << (INDEX1_NBITS+2) | (paddr_t)((dreq.addr>>PAGE_M_NBITS)<<2);
                    r_dcache_tlb_ptba_read = true;
                    r_dcache_fsm = DCACHE_DTLB1_READ_CACHE;
                }
            }        
        }
        else
        {
            r_dcache_fsm = DCACHE_WRITE_REQ;
            drsp.valid = true;
            drsp.rdata = 0;
        }
        break;
    }
    ////////////////////////
    case DCACHE_WRITE_DIRTY:
    {
        if ( dreq.valid ) m_cost_data_tlb_miss_frz++;

        r_dcache.write(r_dcache_tlb_paddr, r_dcache_pte_update);
        dcache_tlb.setdirty(r_dcache_tlb_way_save, r_dcache_tlb_set_save);
        r_dcache_fsm = DCACHE_WRITE_REQ;
        drsp.valid = true;
        drsp.rdata = 0;
        break;
    }
    /////////////////
    case DCACHE_ERROR:
    {
        r_vci_rsp_data_error = false;
        drsp.valid = true;
        drsp.error = true;
        drsp.rdata = 0;
        r_dcache_fsm = DCACHE_IDLE;
        break;
    }   
    //////////////////////
    case DCACHE_ITLB_READ:
    {
    	if ( r_vci_rsp_data_ok ) // vci response ok
        {  
            if ( r_vci_rsp_data_error )
            {
                r_dcache_rsp_itlb_error = true;	
                r_itlb_read_dcache_req = false;
                r_vci_rsp_data_error = false;
                r_dcache_fsm = DCACHE_IDLE;
            }
            else 
            {
                r_dcache_fsm = DCACHE_ITLB_UPDT;
            }
        }
	    break;    	
    }
    //////////////////////
    case DCACHE_ITLB_UPDT:
    {
        data_t rsp_itlb_miss = 0;
        data_t rsp_itlb_ppn = 0;
        paddr_t  victim_index = 0;
        r_dcache.update(r_icache_paddr_save, r_dcache_miss_buf, &victim_index);
        bool itlb_hit_dcache = r_dcache.read(r_icache_paddr_save, &rsp_itlb_miss);	
       
	    if ( (r_icache_fsm == ICACHE_TLB2_READ) && itlb_hit_dcache )
	    {	
            bool itlb_hit_ppn = r_dcache.read(r_icache_paddr_save.read()+4, &rsp_itlb_ppn);	
		    assert(itlb_hit_ppn && "Address of pte[64-32] and pte[31-0] should be successive");
	    }
        r_dcache_rsp_itlb_miss = rsp_itlb_miss;
        r_dcache_rsp_itlb_ppn = rsp_itlb_ppn;
        r_dcache_rsp_itlb_error = false;	
        r_itlb_read_dcache_req = false;
        r_dcache_fsm = DCACHE_IDLE;
        break;
    }
    //////////////////////////
    case DCACHE_ITLB_LL_WAIT:
    {
	    if ( r_vci_rsp_data_ok )
	    {
            if ( r_vci_rsp_data_error ) // VCI response ko
            {
                r_dcache_rsp_itlb_error = true;  
                r_vci_rsp_data_error = false;
                r_itlb_acc_dcache_req = false;
		        r_dcache_fsm = DCACHE_IDLE;	
            }
	        else
	        {
	            if ( !(r_dcache_miss_buf[0] >> PTE_V_SHIFT) )	// unmapped
	            {
                    r_dcache_rsp_itlb_error = true;  
                    r_itlb_acc_dcache_req = false;
		            r_dcache_fsm = DCACHE_IDLE;	
	            }
		        else
		        {
		            r_dcache_itlb_sc_acc_req = true;
                    r_vci_rsp_data_ok = false;
                    r_icache_pte_update = r_dcache_miss_buf[0] | r_icache_pte_update.read();
                    r_dcache_fsm = DCACHE_ITLB_SC_WAIT; 
		        }
	        }
	    }
	    break;
    }
    //////////////////////////
    case DCACHE_ITLB_SC_WAIT:
    {
        if ( r_vci_rsp_data_ok && r_vci_rsp_data_error ) // VCI response ko
	    {
            r_dcache_rsp_itlb_error = true;  
            r_vci_rsp_data_error = false;
            r_itlb_acc_dcache_req = false;
	        r_dcache_fsm = DCACHE_IDLE;	
	    }
	    else if ( r_vci_rsp_data_ok && r_dcache_itlb_ll_acc_req )
	    {
	        r_dcache_fsm = DCACHE_ITLB_LL_WAIT;
            r_vci_rsp_data_ok = false; 
	    }
	    else if ( r_vci_rsp_data_ok )
	    {
	        r_itlb_acc_dcache_req = false;
	        r_dcache_fsm = DCACHE_IDLE; 
	    }
	    break;
    }  
    } // end switch r_dcache_fsm

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name() << " Data Response: " << drsp << std::endl;
#endif
    if ( r_dcache_fsm == DCACHE_WRITE_REQ )
    {
        r_wbuf.write(true, r_dcache_paddr_save.read(), r_dcache_be_save.read(), r_dcache_wdata_save); 
    }
    else
    {
        r_wbuf.write(false, 0, 0, 0);
    }
    /////////// execute one iss cycle /////////////////////////////////
    {
    uint32_t it = 0;
    for (size_t i=0; i<(size_t)iss_t::n_irq; i++) if(p_irq[i].read()) it |= (1<<i);
    m_iss.executeNCycles(1, irsp, drsp, it);
    }

    ////////////// number of frozen cycles //////////////////////////
    if ( (ireq.valid && !irsp.valid) || (dreq.valid && !drsp.valid) )
    {
        m_cpt_frz_cycles++;
    }

    ////////////////////////////////////////////////////////////////////////////
    //     VCI_CMD FSM 
    //
    // This FSM handles requests from both the DCACHE controler
    // (request registers) and the ICACHE controler (request registers).
    // There is 10 VCI transaction types :
    // - INS_TLB_READ
    // - INS_TLB_WRITE
    // - INS_MISS
    // - INS_UNC_MISS
    // - DATA_TLB_READ
    // - DATA_TLB_WRITE
    // - DATA_TLB_DIRTY
    // - DATA_MISS
    // - DATA_UNC 
    // - DATA_WRITE
    // The ICACHE requests have the highest priority.
    // There is at most one (CMD/RSP) VCI transaction, as both CMD_FSM and RSP_FSM
    // exit simultaneously the IDLE state.
    //////////////////////////////////////////////////////////////////////////////

    switch (r_vci_cmd_fsm) {
    
    case CMD_IDLE:
        if (r_dcache_itlb_read_req && r_wbuf.miss( r_icache_paddr_save ))           
        {            
            r_vci_cmd_fsm = CMD_ITLB_READ;
            r_dcache_itlb_read_req = false;
            m_cpt_itlbmiss_transaction++; 
        } 
	    else if (r_dcache_itlb_ll_acc_req && r_wbuf.miss( r_icache_paddr_save ))
	    {
	        r_vci_cmd_fsm = CMD_ITLB_ACC_LL;
            r_dcache_itlb_ll_acc_req = false;
            m_cpt_itlb_write_transaction++; 
	    }
	    else if (r_dcache_itlb_sc_acc_req && r_wbuf.miss( r_icache_paddr_save ))
	    {
	        r_vci_cmd_fsm = CMD_ITLB_ACC_SC;
            r_dcache_itlb_sc_acc_req = false;
            m_cpt_itlb_write_transaction++; 
	    }
        else if (r_icache_miss_req && r_wbuf.miss( r_icache_paddr_save )) 
        {    
            r_vci_cmd_fsm = CMD_INS_MISS;
            r_icache_miss_req = false;
            m_cpt_imiss_transaction++; 
        }
        else if (r_icache_unc_req && r_wbuf.miss( r_icache_paddr_save )) 
        {    
            r_vci_cmd_fsm = CMD_INS_UNC;
            r_icache_unc_req = false;
            m_cpt_imiss_transaction++; 
        }  
        else if (r_dcache_tlb_read_req && r_wbuf.miss( r_dcache_tlb_paddr )) 
        {            
            r_vci_cmd_fsm = CMD_DTLB_READ;
            r_dcache_tlb_read_req = false;
            m_cpt_dtlbmiss_transaction++; 
        } 
        else if (r_dcache_tlb_ll_acc_req && r_wbuf.miss( r_dcache_tlb_paddr )) 
        {  
            r_vci_cmd_fsm = CMD_DTLB_ACC_LL;
            r_dcache_tlb_ll_acc_req = false;
            m_cpt_dtlb_write_transaction++; 
        } 
        else if (r_dcache_tlb_sc_acc_req && r_wbuf.miss( r_dcache_tlb_paddr )) 
        {  
            r_vci_cmd_fsm = CMD_DTLB_ACC_SC;
            r_dcache_tlb_sc_acc_req = false;
            m_cpt_dtlb_write_transaction++; 
        } 
        else if (r_dcache_tlb_ll_dirty_req && r_wbuf.miss( r_dcache_tlb_paddr )) 
        {  
            r_vci_cmd_fsm = CMD_DTLB_DIRTY_LL;
            r_dcache_tlb_ll_dirty_req = false;
            m_cpt_dtlb_write_transaction++; 
        } 
        else if (r_dcache_tlb_sc_dirty_req && r_wbuf.miss( r_dcache_tlb_paddr )) 
        {  
            r_vci_cmd_fsm = CMD_DTLB_DIRTY_SC;
            r_dcache_tlb_sc_dirty_req = false;
            m_cpt_dtlb_write_transaction++; 
        } 
        else if (r_dcache_miss_req && r_wbuf.miss( r_dcache_paddr_save ))  
        {
            r_vci_cmd_fsm = CMD_DATA_MISS;
            r_dcache_miss_req = false;
            m_cpt_dmiss_transaction++; 
        }
        else if (r_dcache_unc_req && r_wbuf.miss( r_dcache_paddr_save ))  
        {
            r_vci_cmd_fsm = CMD_DATA_UNC;
            r_dcache_unc_req = false;
            m_cpt_unc_transaction++; 
        }
        else if ( r_wbuf.rok() )
        {
            r_vci_cmd_fsm = CMD_DATA_WRITE;
            r_vci_cmd_cpt = r_wbuf.getMin();
            r_vci_cmd_min = r_wbuf.getMin();
            r_vci_cmd_max = r_wbuf.getMax(); 
            m_cpt_write_transaction++; 
            m_length_write_transaction += (r_wbuf.getMax() - r_wbuf.getMin() + 1);
        }
        break;

    case CMD_DATA_WRITE:
        if ( p_vci.cmdack.read() ) 
        {
            r_vci_cmd_cpt = r_vci_cmd_cpt + 1;
            if (r_vci_cmd_cpt == r_vci_cmd_max) 
            {
                r_vci_cmd_fsm = CMD_IDLE;
                r_wbuf.sent();
            }
        }
        break;

    default:
        if ( p_vci.cmdack.read() )
        {  
            r_vci_cmd_fsm = CMD_IDLE;
        }
        break;

    } // end  switch r_vci_cmd_fsm

    //////////////////////////////////////////////////////////////////////////
    //      VCI_RSP FSM 
    //
    // This FSM is synchronized with the VCI_CMD FSM, as both FSMs exit the
    // IDLE state simultaneously.
    //////////////////////////////////////////////////////////////////////////

    switch (r_vci_rsp_fsm) {

    case RSP_IDLE:
        if ( p_vci.rspval.read() )
        {
            r_vci_rsp_cpt = 0;
            if ( p_vci.rpktid.read()%2 == 1 )
            {
 			    r_vci_rsp_fsm = RSP_DATA_WRITE;
            }
            else if ( p_vci.rpktid.read() == TYPE_INS_TLB ) 	
            {
                r_vci_rsp_fsm = RSP_ITLB_READ;
            } 
            else if ( p_vci.rpktid.read() == TYPE_INS_LL_ACC ) 	
            {
                r_vci_rsp_fsm = RSP_ITLB_ACC_LL;
            }
            else if ( p_vci.rpktid.read() == TYPE_INS_SC_ACC ) 	
            {
                r_vci_rsp_fsm = RSP_ITLB_ACC_SC;
            }
            else if ( p_vci.rpktid.read() == TYPE_INS_MISS ) 	
            {
                r_vci_rsp_fsm = RSP_INS_MISS;
            }
            else if ( p_vci.rpktid.read() == TYPE_INS_UNC ) 	
            {
                r_vci_rsp_fsm = RSP_INS_UNC;
            }
            else if ( p_vci.rpktid.read() == TYPE_DATA_TLB ) 	
            {
                r_vci_rsp_fsm = RSP_DTLB_READ;
            }
            else if ( p_vci.rpktid.read() == TYPE_DATA_LL_ACC ) 	
            {
                r_vci_rsp_fsm = RSP_DTLB_ACC_LL;
            }
            else if ( p_vci.rpktid.read() == TYPE_DATA_SC_ACC ) 	
            {
                r_vci_rsp_fsm = RSP_DTLB_ACC_SC;
            }
            else if ( p_vci.rpktid.read() == TYPE_DATA_LL_D ) 	
            {
                r_vci_rsp_fsm = RSP_DTLB_DIRTY_LL;
            }
            else if ( p_vci.rpktid.read() == TYPE_DATA_SC_D ) 	
            {
                r_vci_rsp_fsm = RSP_DTLB_DIRTY_SC;
            }
            else if ( p_vci.rpktid.read() == TYPE_DATA_MISS ) 	
            {
                r_vci_rsp_fsm = RSP_DATA_MISS;
            }
            else if ( p_vci.rpktid.read() == TYPE_DATA_UNC ) 	
            {
                r_vci_rsp_fsm = RSP_DATA_UNC;
            }
        }
        break;

    case RSP_ITLB_READ:
        m_cost_itlbmiss_transaction++;
        if ( ! p_vci.rspval.read() )
            break;

        assert(r_vci_rsp_cpt != m_dcache_words &&
               "illegal VCI response packet for data read miss");

        r_vci_rsp_cpt = r_vci_rsp_cpt + 1;
        r_dcache_miss_buf[r_vci_rsp_cpt] = (data_t)p_vci.rdata.read();
        if ( p_vci.reop.read() ) 
        {
            assert(r_vci_rsp_cpt == m_dcache_words - 1 &&
                    "illegal VCI response packet for data read miss");
            r_vci_rsp_data_ok = true;
            r_vci_rsp_fsm = RSP_IDLE;
        } 
        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL )
        {
            r_vci_rsp_data_error = true;
        }
        break;

    case RSP_ITLB_ACC_LL:
        if ( ! p_vci.rspval.read() )
            break;

        assert(p_vci.reop.read() &&
               "illegal VCI response packet for ll tlb");

        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL ) 
        {
            r_vci_rsp_data_error = true;
        }
	    else
	    {
	        r_dcache_miss_buf[0] = (data_t)p_vci.rdata.read();
	    }
        r_vci_rsp_data_ok = true;
        r_vci_rsp_fsm = RSP_IDLE;
	    break;

    case RSP_ITLB_ACC_SC:
        if ( ! p_vci.rspval.read() )
            break;

        assert(p_vci.reop.read() &&
               "illegal VCI response packet for sc tlb");

        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL ) 
        {
            r_vci_rsp_data_error = true;
        }
	    else if ( p_vci.rdata.read() == 1 ) // store conditional is not successful
	    {
	        r_dcache_itlb_ll_acc_req = true;
	    }
        r_vci_rsp_data_ok = true;
        r_vci_rsp_fsm = RSP_IDLE;
	    break;

    case RSP_INS_MISS:
        m_cost_imiss_transaction++;
        if ( ! p_vci.rspval.read() )
            break;

        assert( (r_vci_rsp_cpt < m_icache_words) && 
               "The VCI response packet for instruction miss is too long");
        r_vci_rsp_cpt = r_vci_rsp_cpt + 1;
        r_icache_miss_buf[r_vci_rsp_cpt] = (data_t)p_vci.rdata.read();

        if ( p_vci.reop.read() ) 
        {
            assert( (r_vci_rsp_cpt == m_icache_words - 1) &&
                       "The VCI response packet for instruction miss is too short");
            r_vci_rsp_ins_ok = true;
            r_vci_rsp_fsm = RSP_IDLE;
                
        } 
        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL )
        {
            r_vci_rsp_ins_error = true;
        }
        break;

    case RSP_INS_UNC:
        m_cost_imiss_transaction++;
        if ( ! p_vci.rspval.read() )
            break;

        assert(p_vci.reop.read() &&
               "illegal VCI response packet for uncached instruction");

        r_icache_miss_buf[0] = (data_t)p_vci.rdata.read();
        r_vci_rsp_ins_ok = true;
        r_vci_rsp_fsm = RSP_IDLE;

        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL )
        {
            r_vci_rsp_ins_error = true;
        }
        break;

    case RSP_DTLB_READ:
        m_cost_dtlbmiss_transaction++;
        if ( ! p_vci.rspval.read() )
            break;

        assert(r_vci_rsp_cpt != m_dcache_words &&
               "illegal VCI response packet for data read miss");

        r_vci_rsp_cpt = r_vci_rsp_cpt + 1;
        r_dcache_miss_buf[r_vci_rsp_cpt] = (data_t)p_vci.rdata.read();
        if ( p_vci.reop.read() ) 
        {
            assert(r_vci_rsp_cpt == m_dcache_words - 1 &&
                    "illegal VCI response packet for data read miss");
            r_vci_rsp_data_ok = true;
            r_vci_rsp_fsm = RSP_IDLE;
        } 
        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL )
        {
            r_vci_rsp_data_error = true;
        }
        break;

    case RSP_DTLB_ACC_LL:
        if ( ! p_vci.rspval.read() )
            break;

        assert(p_vci.reop.read() &&
               "illegal VCI response packet for ll tlb");

        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL ) 
        {
            r_vci_rsp_data_error = true;
        }
	    else
	    {
	        r_dcache_miss_buf[0] = (data_t)p_vci.rdata.read();
	    }
        r_vci_rsp_data_ok = true;
        r_vci_rsp_fsm = RSP_IDLE;
	    break;

    case RSP_DTLB_ACC_SC:
        if ( ! p_vci.rspval.read() )
            break;

        assert(p_vci.reop.read() &&
               "illegal VCI response packet for sc tlb");

        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL ) 
        {
            r_vci_rsp_data_error = true;
        }
	    else if ( p_vci.rdata.read() == 1 ) // store conditional is not successful
	    {
	        r_dcache_tlb_ll_acc_req = true;
	    }
        r_vci_rsp_data_ok = true;
        r_vci_rsp_fsm = RSP_IDLE;
	    break;

    case RSP_DTLB_DIRTY_LL:
        if ( ! p_vci.rspval.read() )
            break;

        assert(p_vci.reop.read() &&
               "illegal VCI response packet for ll tlb");

        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL ) 
        {
            r_vci_rsp_data_error = true;
        }
	    else
	    {
	        r_dcache_miss_buf[0] = (data_t)p_vci.rdata.read();
	    }
        r_vci_rsp_data_ok = true;
        r_vci_rsp_fsm = RSP_IDLE;
	    break;

    case RSP_DTLB_DIRTY_SC:
        if ( ! p_vci.rspval.read() )
            break;

        assert(p_vci.reop.read() &&
               "illegal VCI response packet for sc tlb");

        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL ) 
        {
            r_vci_rsp_data_error = true;
        }
	    else if ( p_vci.rdata.read() == 1 ) // store conditional is not successful
	    {
	        r_dcache_tlb_ll_dirty_req = true;
	    }
        r_vci_rsp_data_ok = true;
        r_vci_rsp_fsm = RSP_IDLE;
	    break;

    case RSP_DATA_UNC:
        m_cost_unc_transaction++;
        if ( ! p_vci.rspval.read() ) 
            break;

        assert(p_vci.reop.read() &&
               "illegal VCI response packet for data read uncached");

        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL ) 
        {
            r_vci_rsp_data_error = true;
        }
        else
        {
            r_dcache_miss_buf[0] = (data_t)p_vci.rdata.read();
            r_vci_rsp_data_ok = true;
        }
        r_vci_rsp_fsm = RSP_IDLE;
        break;

    case RSP_DATA_MISS:
        m_cost_dmiss_transaction++;
        if ( ! p_vci.rspval.read() )
            break;

        assert(r_vci_rsp_cpt != m_dcache_words &&
               "illegal VCI response packet for data read miss");

        r_vci_rsp_cpt = r_vci_rsp_cpt + 1;
        r_dcache_miss_buf[r_vci_rsp_cpt] = (data_t)p_vci.rdata.read();
        if ( p_vci.reop.read() ) 
        {
            assert(r_vci_rsp_cpt == m_dcache_words - 1 &&
                    "illegal VCI response packet for data read miss");
            r_vci_rsp_data_ok = true;
            r_vci_rsp_fsm = RSP_IDLE;
        } 
        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL )
        {
            r_vci_rsp_data_error = true;
        }
        break;

    case RSP_DATA_WRITE:
        m_cost_write_transaction++;
        if ( ! p_vci.rspval.read() )
            break;

        if ( p_vci.reop.read() ) 
        {
            r_vci_rsp_fsm = RSP_IDLE;
            r_wbuf.completed( p_vci.rpktid.read() >> 1 );
        }
        if ( p_vci.rerror.read() != vci_param::ERR_NORMAL ) 
        {
            m_iss.setWriteBerr();
        }
        break;

    } // end switch r_vci_rsp_fsm
} // end transition()

///////////////////////
tmpl(void)::genMoore()
///////////////////////
{
    // VCI initiator response
    p_vci.rspack = ( r_vci_rsp_fsm != RSP_IDLE );

    // VCI initiator command
    p_vci.srcid  = m_srcid;
    p_vci.cons   = false;
    p_vci.wrap   = false;
    p_vci.contig = true;
    p_vci.clen   = 0;
    p_vci.cfixed = false;

    switch (r_vci_cmd_fsm) {

    case CMD_IDLE:
        p_vci.cmdval  = false;
        p_vci.address = 0;
        p_vci.wdata   = 0;
        p_vci.be      = 0;
        p_vci.trdid   = 0;
        p_vci.pktid   = 0;
        p_vci.plen    = 0;
        p_vci.cmd     = vci_param::CMD_NOP;
        p_vci.eop     = false;
        break;

    case CMD_ITLB_READ:     
        p_vci.cmdval  = true;
        p_vci.address = r_icache_paddr_save.read() & m_dcache_yzmask;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = 1;  // via data cache cached read
        p_vci.pktid   = TYPE_INS_TLB;
        p_vci.plen    = m_dcache_words << 2;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    case CMD_ITLB_ACC_LL:
        p_vci.cmdval  = true;
        p_vci.address = r_icache_paddr_save.read() & ~0x3;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = 0; // data cache uncached read
        p_vci.pktid   = TYPE_INS_LL_ACC;
        p_vci.plen    = 4;
        p_vci.cmd     = vci_param::CMD_LOCKED_READ;
        p_vci.eop     = true;
	break;

    case CMD_ITLB_ACC_SC:
        p_vci.cmdval  = true;
        p_vci.address = r_icache_paddr_save.read() & ~0x3;
        p_vci.wdata   = r_icache_pte_update.read();
        p_vci.be      = 0xF;
        p_vci.trdid   = 0; // data cache uncached read
        p_vci.pktid   = TYPE_INS_SC_ACC;
        p_vci.plen    = 4;
        p_vci.cmd     = vci_param::CMD_STORE_COND;
        p_vci.eop     = true;
	break;	

    case CMD_INS_MISS:
        p_vci.cmdval  = true;
        p_vci.address = r_icache_paddr_save.read() & m_icache_yzmask;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = 3; // ins cache cached read
        p_vci.pktid   = TYPE_INS_MISS;
        p_vci.plen    = m_icache_words << 2;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    case CMD_INS_UNC:
        p_vci.cmdval  = true;
        p_vci.address = r_icache_paddr_save.read() & ~0x3;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = 2; // ins cache uncached read
        p_vci.pktid   = TYPE_INS_UNC;
        p_vci.plen    = 4;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    case CMD_DTLB_READ:     
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_tlb_paddr.read() & m_dcache_yzmask;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = 1; // via cache cached read
        p_vci.pktid   = TYPE_DATA_TLB;
        p_vci.plen    = m_dcache_words << 2;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    case CMD_DTLB_ACC_LL:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_tlb_paddr.read() & ~0x3;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = 0; // data cache uncached read
        p_vci.pktid   = TYPE_DATA_LL_ACC;
        p_vci.plen    = 4;
        p_vci.cmd     = vci_param::CMD_LOCKED_READ;
        p_vci.eop     = true;
	break;

    case CMD_DTLB_ACC_SC:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_tlb_paddr.read() & ~0x3;
        p_vci.wdata   = r_dcache_pte_update.read();
        p_vci.be      = 0xF;
        p_vci.trdid   = 0; // data cache uncached read
        p_vci.pktid   = TYPE_DATA_SC_ACC;
        p_vci.plen    = 4;
        p_vci.cmd     = vci_param::CMD_STORE_COND;
        p_vci.eop     = true;
	break;	

    case CMD_DTLB_DIRTY_LL:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_tlb_paddr.read() & ~0x3;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = 0; // data cache uncached read
        p_vci.pktid   = TYPE_DATA_LL_D;
        p_vci.plen    = 4;
        p_vci.cmd     = vci_param::CMD_LOCKED_READ;
        p_vci.eop     = true;
	break;

    case CMD_DTLB_DIRTY_SC:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_tlb_paddr.read() & ~0x3;
        p_vci.wdata   = r_dcache_pte_update.read();
        p_vci.be      = 0xF;
        p_vci.trdid   = 0; // data cache uncached read
        p_vci.pktid   = TYPE_DATA_SC_D;
        p_vci.plen    = 4;
        p_vci.cmd     = vci_param::CMD_STORE_COND;
        p_vci.eop     = true;
	break;	

    case CMD_DATA_UNC:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_paddr_save.read() & ~0x3;
        p_vci.trdid   = 0; // data cache uncached read
        p_vci.pktid   = TYPE_DATA_UNC;
        p_vci.plen    = 4;
        p_vci.eop     = true;
        switch(r_dcache_type_save) {
        case iss_t::DATA_READ:
            p_vci.wdata = 0;
            p_vci.be    = r_dcache_be_save.read();
            p_vci.cmd   = vci_param::CMD_READ;
            break;
        case iss_t::DATA_LL:
            p_vci.wdata = 0;
            p_vci.be    = 0xF;
            p_vci.cmd   = vci_param::CMD_LOCKED_READ;
            break;
        case iss_t::DATA_SC:
            p_vci.wdata = r_dcache_wdata_save.read();
            p_vci.be    = 0xF;
            p_vci.cmd   = vci_param::CMD_STORE_COND;
            break;
        default:
            assert("this should not happen");
        }
        break;

    case CMD_DATA_WRITE:
        p_vci.cmdval  = true;
        p_vci.address = r_wbuf.getAddress(r_vci_cmd_cpt);
        p_vci.wdata   = r_wbuf.getData(r_vci_cmd_cpt);
        p_vci.be      = r_wbuf.getBe(r_vci_cmd_cpt);
        p_vci.trdid   = 0; // data cache write
        p_vci.pktid   = (r_wbuf.getIndex() << 1) + 1;
        p_vci.plen    = (r_vci_cmd_max - r_vci_cmd_min + 1)<<2;
        p_vci.cmd     = vci_param::CMD_WRITE;
        p_vci.eop     = (r_vci_cmd_cpt == r_vci_cmd_max);
        break;

    case CMD_DATA_MISS:
        p_vci.cmdval  = true;
        p_vci.address = r_dcache_paddr_save.read() & m_dcache_yzmask;
        p_vci.wdata   = 0;
        p_vci.be      = 0xF;
        p_vci.trdid   = 1; // data cache cached read
        p_vci.pktid   = TYPE_DATA_MISS;
        p_vci.plen    = m_dcache_words << 2;
        p_vci.cmd     = vci_param::CMD_READ;
        p_vci.eop     = true;
        break;

    } // end switch r_vci_cmd_fsm

#ifdef SOCLIB_MODULE_DEBUG 
    std::cout << name()
              << "Moore:" << std::hex
              << "p_vci.cmdval:" << p_vci.cmdval
              << "p_vci.address:" << p_vci.address
              << "p_vci.wdata:" << p_vci.wdata
              << "p_vci.cmd:" << p_vci.cmd
              << "p_vci.eop:" << p_vci.eop
              << std::endl;
#endif
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

