/* -*- c++ -*-
 * File : vci_vcache_wrapper.h
 * Copyright (c) UPMC, Lip6, SoC
 * Authors : Alain GREINER, Yang GAO
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
 
#ifndef SOCLIB_CABA_VCI_VCACHE_WRAPPER_H
#define SOCLIB_CABA_VCI_VCACHE_WRAPPER_H

#include <inttypes.h>
#include <systemc>
#include "caba_base_module.h"
#include "multi_write_buffer.h"
#include "generic_fifo.h"
#include "generic_tlb.h"
#include "generic_cache.h"
#include "vci_initiator.h"
#include "vci_target.h"
#include "mapping_table.h"
#include "static_assert.h"
#include "iss2.h"

namespace soclib {
namespace caba {

using namespace sc_core;

////////////////////////////////////////////
template<typename vci_param, typename iss_t>
class VciVcacheWrapper
////////////////////////////////////////////
    : public soclib::caba::BaseModule
{
    typedef uint32_t vaddr_t;
    typedef uint32_t tag_t;
    typedef uint32_t type_t;
    typedef typename iss_t::DataOperationType data_op_t;

    typedef typename vci_param::addr_t  paddr_t;
    typedef typename vci_param::srcid_t vci_srcid_t;
    typedef typename vci_param::trdid_t vci_trdid_t;
    typedef typename vci_param::pktid_t vci_pktid_t;
    typedef typename vci_param::plen_t  vci_plen_t;
    typedef uint32_t                    vci_be_t;       // don't use vci_param::be_t

    enum icache_fsm_state_e {  
        ICACHE_IDLE,             
        // handling XTN processor requests
        ICACHE_XTN_TLB_FLUSH,
        ICACHE_XTN_CACHE_FLUSH, 
        ICACHE_XTN_TLB_INVAL,  
        ICACHE_XTN_CACHE_INVAL_VA,
        ICACHE_XTN_CACHE_INVAL_PA,  
        ICACHE_XTN_CACHE_INVAL_GO,
        // handling tlb miss
        ICACHE_TLB_WAIT,
        // handling cache miss
        ICACHE_MISS_VICTIM,   
        ICACHE_MISS_INVAL,   
        ICACHE_MISS_WAIT,   
        ICACHE_MISS_UPDT, 
        // handling unc read
        ICACHE_UNC_WAIT,  
    };

    enum dcache_fsm_state_e {  
        DCACHE_IDLE,                
        // handling itlb & dtlb miss
        DCACHE_TLB_MISS,
        DCACHE_TLB_PTE1_GET,           
        DCACHE_TLB_PTE1_SELECT,      
        DCACHE_TLB_PTE1_UPDT,       
        DCACHE_TLB_PTE2_GET,    
        DCACHE_TLB_PTE2_SELECT,       
        DCACHE_TLB_PTE2_UPDT,           
        DCACHE_TLB_LR_WAIT,           
        DCACHE_TLB_RETURN,         
	    // handling processor XTN requests
        DCACHE_XTN_SWITCH,
        DCACHE_XTN_SYNC,
        DCACHE_XTN_IC_INVAL_VA,        
        DCACHE_XTN_IC_FLUSH,        
        DCACHE_XTN_IC_INVAL_PA,     
        DCACHE_XTN_IT_INVAL,          
        DCACHE_XTN_DC_FLUSH,        
        DCACHE_XTN_DC_INVAL_VA,        
        DCACHE_XTN_DC_INVAL_PA,     
        DCACHE_XTN_DC_INVAL_END,
        DCACHE_XTN_DC_INVAL_GO,          
        DCACHE_XTN_DT_INVAL,          
        //handling dirty bit update
        DCACHE_DIRTY_GET_PTE,
        DCACHE_DIRTY_SC_WAIT,           
	    // handling processor miss requests
        DCACHE_MISS_VICTIM,
        DCACHE_MISS_INVAL,   
        DCACHE_MISS_WAIT,           
        DCACHE_MISS_UPDT,           
        // handling processor unc and sc requests
        DCACHE_UNC_WAIT,            
        DCACHE_SC_WAIT,            
    };

    enum cmd_fsm_state_e {      
        CMD_IDLE,
        CMD_INS_MISS,
        CMD_INS_UNC,
        CMD_DATA_MISS,
        CMD_DATA_UNC,
        CMD_DATA_WRITE,
        CMD_DATA_SC, 
    };

    enum rsp_fsm_state_e {       
        RSP_IDLE,
        RSP_INS_MISS,
        RSP_INS_UNC,
        RSP_DATA_MISS,
        RSP_DATA_UNC,
        RSP_DATA_WRITE,
        RSP_DATA_SC,
    };

    // TLB Mode : ITLB / DTLB / ICACHE / DCACHE
    enum {          
        INS_TLB_MASK    = 0x8,
        DATA_TLB_MASK   = 0x4,
        INS_CACHE_MASK  = 0x2,
        DATA_CACHE_MASK = 0x1,
    };

    // Error Type
    enum mmu_error_type_e 
    {
        MMU_NONE                      = 0x0000, // None
        MMU_WRITE_PT1_UNMAPPED 	      = 0x0001, // Page fault on Page Table 1          
        MMU_WRITE_PT2_UNMAPPED 	      = 0x0002, // Page fault on Page Table 2          
        MMU_WRITE_PRIVILEGE_VIOLATION = 0x0004, // Protected access in user mode      
        MMU_WRITE_ACCES_VIOLATION     = 0x0008, // Write access to a non writable page
        MMU_WRITE_UNDEFINED_XTN       = 0x0020, // Undefined external access   
        MMU_WRITE_PT1_ILLEGAL_ACCESS  = 0x0040, // Bus Error accessing Table 1       
        MMU_WRITE_PT2_ILLEGAL_ACCESS  = 0x0080, // Bus Error accessing Table 2      
        MMU_WRITE_DATA_ILLEGAL_ACCESS = 0x0100, // Bus Error in cache access     
        MMU_READ_PT1_UNMAPPED 	      = 0x1001, // Page fault on Page Table 1  	
        MMU_READ_PT2_UNMAPPED 	      = 0x1002, // Page fault on Page Table 2  
        MMU_READ_PRIVILEGE_VIOLATION  = 0x1004, // Protected access in user mode 
        MMU_READ_EXEC_VIOLATION       = 0x1010, // Exec access to a non exec page 
        MMU_READ_UNDEFINED_XTN 	      = 0x1020, // Undefined external access address 
        MMU_READ_PT1_ILLEGAL_ACCESS   = 0x1040, // Bus Error in Table1 access      
        MMU_READ_PT2_ILLEGAL_ACCESS   = 0x1080, // Bus Error in Table2 access 	
        MMU_READ_DATA_ILLEGAL_ACCESS  = 0x1100, // Bus Error in cache access 
    };

    // miss types for data cache
    enum dcache_miss_type_e
    {
        PTE1_MISS, 
        PTE2_MISS,
        PROC_MISS,  
        DIRTY_MISS,
    };

    enum transaction_type_d_e
    {
        // b0 : 1 if cached
        // b1 : 1 if instruction
        TYPE_DATA_UNC     = 0x0,
        TYPE_DATA_MISS    = 0x1,
        TYPE_INS_UNC      = 0x2,
        TYPE_INS_MISS     = 0x3,
    };

public:
    sc_in<bool>                             p_clk;
    sc_in<bool>                             p_resetn;
    sc_in<bool>                             p_irq[iss_t::n_irq];
    soclib::caba::VciInitiator<vci_param>   p_vci;

private:

    // STRUCTURAL PARAMETERS
    soclib::common::AddressDecodingTable<uint64_t, bool>    	m_cacheability_table;
    const vci_srcid_t                                       	m_srcid;

    const size_t  						m_tlb_ways;
    const size_t  						m_tlb_sets;

    const size_t  						m_icache_ways;
    const size_t  						m_icache_sets;
    const paddr_t 						m_icache_yzmask;
    const size_t  						m_icache_words;

    const size_t  						m_dcache_ways;
    const size_t  						m_dcache_sets;
    const paddr_t 						m_dcache_yzmask;
    const size_t  						m_dcache_words;

    const size_t                        m_procid;
    const size_t  						m_paddr_nbits;  

    const uint32_t						m_max_frozen_cycles;   
    const uint32_t                      m_debug_start_cycle;    
    const bool                          m_debug_ok;

    ////////////////////////////////////////
    // Communication with processor ISS
    ////////////////////////////////////////
    typename iss_t::InstructionRequest  m_ireq;
    typename iss_t::InstructionResponse m_irsp;
    typename iss_t::DataRequest         m_dreq;
    typename iss_t::DataResponse        m_drsp;

    ///////////////////////////////
    // Software visible REGISTERS
    ///////////////////////////////
    sc_signal<uint32_t>     r_mmu_ptpr;             	// page table pointer register
    sc_signal<uint32_t>     r_mmu_mode;             	// mmu mode register
    sc_signal<uint32_t>     r_mmu_word_lo;          	// mmu misc data low
    sc_signal<uint32_t>     r_mmu_word_hi;          	// mmu misc data hight
    sc_signal<uint32_t>     r_mmu_ibvar;      	    	// mmu bad instruction address
    sc_signal<uint32_t>     r_mmu_dbvar;              	// mmu bad data address
    sc_signal<uint32_t>     r_mmu_ietr;                 // mmu instruction error type
    sc_signal<uint32_t>     r_mmu_detr;                 // mmu data error type
    uint32_t	            r_mmu_params;		        // read-only
    uint32_t	            r_mmu_release;		        // read_only

    //////////////////////////////
    // ICACHE FSM REGISTERS
    //////////////////////////////
    sc_signal<int>          r_icache_fsm;               // state register
    sc_signal<paddr_t>      r_icache_vci_paddr;      	// physical address 
    sc_signal<uint32_t>     r_icache_vaddr_save;        // virtual address 

    // icache miss handling
    sc_signal<size_t>       r_icache_miss_way;		    // selected way for cache update
    sc_signal<size_t>       r_icache_miss_set;		    // selected set for cache update 
    sc_signal<size_t>       r_icache_miss_word;		    // word index (cache update)
    sc_signal<bool>         r_icache_miss_inval;        // cc request match pending miss

    // icache flush handling
    sc_signal<size_t>       r_icache_flush_count;	    // slot counter (cache flush)

    // communication between ICACHE FSM and VCI_CMD FSM
    sc_signal<bool>         r_icache_miss_req;          // cached read miss
    sc_signal<bool>         r_icache_unc_req;           // uncached read miss

    // communication between ICACHE FSM and DCACHE FSM
    sc_signal<bool>	        r_icache_tlb_miss_req;      // set icache/reset dcache
    sc_signal<bool>         r_icache_tlb_rsp_error;     // itlb miss response error

    ///////////////////////////////
    // DCACHE FSM REGISTERS
    ///////////////////////////////
    sc_signal<int>          r_dcache_fsm;               // state register
    // registers written in P0 stage (used in P1 stage)
    sc_signal<bool>         r_dcache_p0_valid;		    // P1 pipe stage to be executed
    sc_signal<uint32_t>     r_dcache_p0_vaddr;          // virtual address (from proc)
    sc_signal<uint32_t>     r_dcache_p0_wdata;          // write data (from proc)
    sc_signal<vci_be_t>     r_dcache_p0_be;             // byte enable (from proc)
    sc_signal<paddr_t>      r_dcache_p0_paddr;          // physical address 
    sc_signal<bool>         r_dcache_p0_cacheable;	    // address cacheable 
    // registers written in P1 stage (used in P2 stage)
    sc_signal<bool>         r_dcache_p1_valid;		    // P2 pipe stage to be executed
    sc_signal<uint32_t>     r_dcache_p1_wdata;          // write data (from proc)
    sc_signal<vci_be_t>     r_dcache_p1_be;             // byte enable (from proc)
    sc_signal<paddr_t>      r_dcache_p1_paddr;          // physical address 
    sc_signal<size_t>       r_dcache_p1_cache_way;	    // selected way (from dcache) 
    sc_signal<size_t>       r_dcache_p1_cache_set;	    // selected set (from dcache)    
    sc_signal<size_t>       r_dcache_p1_cache_word;	    // selected word (from dcache)    
    // registers used by the Dirty bit sub-fsm
    sc_signal<paddr_t>      r_dcache_dirty_paddr;       // PTE physical address 
    sc_signal<size_t>       r_dcache_dirty_way;	        // way in dcache
    sc_signal<size_t>       r_dcache_dirty_set;	        // set in dcache
    sc_signal<size_t>       r_dcache_dirty_word;	    // word in dcache

    // communication between DCACHE FSM and VCI_CMD FSM
    sc_signal<paddr_t>      r_dcache_vci_paddr;		    // physical address for VCI 
    sc_signal<bool>         r_dcache_vci_miss_req;      // read miss request
    sc_signal<bool>         r_dcache_vci_unc_req;       // uncacheable read request
    sc_signal<vci_be_t>     r_dcache_vci_unc_be;        // uncacheable read byte enable
    sc_signal<bool>         r_dcache_vci_sc_req;        // atomic write request
    sc_signal<uint32_t>     r_dcache_vci_sc_old;        // previous data (atomic write)
    sc_signal<uint32_t>     r_dcache_vci_sc_new;        // new data (atomic write)

    // register used for XTN inval
    sc_signal<size_t>       r_dcache_xtn_way;		    // selected way (from dcache) 
    sc_signal<size_t>       r_dcache_xtn_set;		    // selected set (from dcache)    

    // write buffer state extension
    sc_signal<bool>         r_dcache_pending_unc_write; // pending uncacheable write

    // handling dcache miss
    sc_signal<int>	        r_dcache_miss_type;		    // depending on the requester
    sc_signal<size_t>       r_dcache_miss_word;		    // word index for cache update
    sc_signal<size_t>       r_dcache_miss_way;		    // selected way for cache update
    sc_signal<size_t>       r_dcache_miss_set;		    // selected set for cache update

    // dcache flush handling
    sc_signal<size_t>       r_dcache_flush_count;	    // slot counter for cache flush

    // used by the TLB miss sub-fsm
    sc_signal<uint32_t>     r_dcache_tlb_vaddr;		    // virtual address for a tlb miss
    sc_signal<bool>         r_dcache_tlb_ins;		    // target tlb (itlb if true)
    sc_signal<paddr_t>      r_dcache_tlb_paddr;		    // physical address of pte
    sc_signal<uint32_t>     r_dcache_tlb_pte_flags;	    // pte1 or first word of pte2
    sc_signal<uint32_t>     r_dcache_tlb_pte_ppn;	    // second word of pte2
    sc_signal<size_t>       r_dcache_tlb_cache_way;	    // selected way in dcache 
    sc_signal<size_t>       r_dcache_tlb_cache_set;	    // selected set in dcache 
    sc_signal<size_t>       r_dcache_tlb_cache_word;	// selected word in dcache
    sc_signal<size_t>       r_dcache_tlb_way;		    // selected way in tlb    
    sc_signal<size_t>       r_dcache_tlb_set;		    // selected set in tlb    

    // LL/SC handling
    sc_signal<bool>         r_dcache_ll_valid;          // valid LL reservation
    sc_signal<uint32_t>     r_dcache_ll_data;           // LL reserved data
    sc_signal<paddr_t>      r_dcache_ll_vaddr;          // LL reserved address 
    sc_signal<uint32_t>     r_dcache_sc_word;           // SC word index in cache line
    sc_signal<uint32_t>     r_dcache_sc_way;            // SC way in cache
    sc_signal<uint32_t>     r_dcache_sc_set;            // SC set in cache
    sc_signal<bool>         r_dcache_sc_hit;            // SC hit in cache
                            
    // communication between DCACHE FSM and ICACHE FSM
    sc_signal<bool>         r_dcache_xtn_req;           // xtn request 
    sc_signal<int>          r_dcache_xtn_opcode;        // xtn request type

    ///////////////////////////////////
    // VCI_CMD FSM REGISTERS
    ///////////////////////////////////
    sc_signal<int>          r_vci_cmd_fsm;
    sc_signal<size_t>       r_vci_cmd_min;      	    // used for write bursts 
    sc_signal<size_t>       r_vci_cmd_max;      	    // used for write bursts 
    sc_signal<size_t>       r_vci_cmd_cpt;    		    // used for write bursts 
    sc_signal<bool>         r_vci_cmd_imiss_prio;	    // round-robin imiss / dmiss

    ///////////////////////////////////
    // VCI_RSP FSM REGISTERS
    ///////////////////////////////////
    sc_signal<int>          r_vci_rsp_fsm;
    sc_signal<size_t>       r_vci_rsp_cpt;
    sc_signal<bool>         r_vci_rsp_ins_error;
    sc_signal<bool>         r_vci_rsp_data_error;
    GenericFifo<uint32_t>   r_vci_rsp_fifo_icache;	    // response FIFO to ICACHE FSM
    GenericFifo<uint32_t>   r_vci_rsp_fifo_dcache;	    // response FIFO to DCACHE FSM

    //////////////////////////////////////////////////////////////////////////////////
    // processor, write buffer, caches , TLBs 
    //////////////////////////////////////////////////////////////////////////////////
    iss_t                       r_iss;   
    MultiWriteBuffer<paddr_t>	r_wbuf;
    GenericCache<paddr_t>   	r_icache;
    GenericCache<paddr_t>    	r_dcache;
    GenericTlb<paddr_t>       	r_itlb;
    GenericTlb<paddr_t>     	r_dtlb;

    //////////////////////////////////////////////////////////////////////////////////
    // debug registers
    //////////////////////////////////////////////////////////////////////////////////
    uint32_t r_cpt_stop_simulation;         // frozen cycles counter
    bool     r_debug_icache_previous_hit;   // previous hit for monitored icache line
    bool     r_debug_dcache_previous_hit;   // previous hit for monitored dcache line
    bool     r_debug_active;                // detailed debug activated

    ///////////////////////////////////////////////////////////////////////////////////
    // Activity Counters
    ///////////////////////////////////////////////////////////////////////////////////

    // Caches access (for power consumption)
    uint32_t r_cpt_dcache_read;             // DCACHE read access
    uint32_t r_cpt_dcache_write;            // DCACHE write access
    uint32_t r_cpt_icache_read;             // ICACHE read access
    uint32_t r_cpt_icache_write;            // ICACHE write access

    // TLBs access (for power consumption)
    uint32_t r_cpt_dtlb_read;               // DTLB read access
    uint32_t r_cpt_dtlb_write;              // DTLB write access
    uint32_t r_cpt_itlb_read;               // ITLB read access
    uint32_t r_cpt_itlb_write;              // ITLB write access

    // WBUF access (for power consumtion)
    uint32_t r_cpt_wbuf_read;               // WBUF read access
    uint32_t r_cpt_wbuf_write;              // WBUF write access

    // general
    uint32_t r_pc_previous;                 // previous pc value
    uint32_t r_cpt_exec_ins;                // number of executed instructions 
    uint32_t r_cpt_frz_cycles;	            // number of cycles where the cpu is frozen
    uint32_t r_cpt_total_cycles;	        // total number of cycles

    // Other ICACHE FSM activity counters
    uint32_t r_cpt_ins;                     // number of requested instructions 
    uint32_t r_cpt_ins_uncacheable;         // number of uncachable instructions
    uint32_t r_cpt_icache_miss;             // number of ICACHE miss
    uint32_t r_cpt_icache_spec_miss;        // number of ICACHE speculative miss
    uint32_t r_cpt_itlb_miss;               // number of ITLB miss
    uint32_t r_cpt_itlb_miss_bypass;        // number of ITLB miss & bypass

    // Other DCACHE FSM activity counters
    uint32_t r_cpt_read;                    // number of READ instructions
    uint32_t r_cpt_read_uncacheable;        // number of READ uncacheable instructions
    uint32_t r_cpt_write;                   // number of WRITE instructions
    uint32_t r_cpt_write_uncacheable;       // number of WRITE uncacheable instructions
    uint32_t r_cpt_write_cached;            // number of WRITE & cache hit instructions
    uint32_t r_cpt_sc;                      // number of SC instructions
    uint32_t r_cpt_ll;                      // number of LL instructions
    uint32_t r_cpt_dcache_miss;             // number of DCACHE miss
    uint32_t r_cpt_dcache_spec_miss;        // number of DCACHE speculative miss
    uint32_t r_cpt_dirty_bit_updt;          // number of dirty bit updt
    uint32_t r_cpt_access_bit_updt;         // number of access bit updt
    uint32_t r_cpt_dtlb_miss;               // number of DTLB miss
    uint32_t r_cpt_dtlb_miss_bypass;        // number of DTLB miss & bypass
    
    // costs (processor frozen)
    uint32_t r_cost_icache_miss_frz;        // frozen cycles caused by ICACHE miss
    uint32_t r_cost_dcache_miss_frz;        // frozen cycles caused by DCACHE miss
    uint32_t r_cost_iunc_frz;               // frozen cycles caused by IUNC 
    uint32_t r_cost_dunc_frz;               // frozen cycles caused by DUNC
    uint32_t r_cost_itlb_miss_frz;          // frozen cycles caused by ITLB miss
    uint32_t r_cost_dtlb_miss_frz;          // frozen cycles caused by DTLB miss
    uint32_t r_cost_dirty_bit_updt_frz;     // frozen cycles caused by dirty bit updt
    uint32_t r_cost_access_bit_updt_frz;    // frozen cycles caused by access bit updt
    uint32_t r_cost_wbuf_full_frz;          // frozen cycles caused by wbuf full 
    uint32_t r_cost_sc_frz;                 // frozen cycles caused by data sc 
    uint32_t r_cost_unc_write_frz;          // frozen cycles caused by unc write

    // VCI transactions
    uint32_t r_cpt_imiss_transaction;       // number of VCI IMISS transactions
    uint32_t r_cpt_dmiss_transaction;       // number of VCI DMISS transactions
    uint32_t r_cpt_iunc_transaction;        // number of VCI IUNC transactions
    uint32_t r_cpt_dunc_transaction;        // number of VCI DUNC transactions
    uint32_t r_cpt_sc_transaction;          // number of VCI SC transactions
    uint32_t r_cpt_write_transaction;       // number of VCI WRITE transactions
    uint32_t r_length_write_transaction;    // cumulated length for VCI WRITE 

    // FSM activity counters
    uint32_t r_cpt_fsm_icache     [64];
    uint32_t r_cpt_fsm_dcache     [64];
    uint32_t r_cpt_fsm_cmd        [64];
    uint32_t r_cpt_fsm_rsp        [64];


protected:
    SC_HAS_PROCESS(VciVcacheWrapper);

public:
    VciVcacheWrapper(
        sc_module_name insname,
        int procid,
        const soclib::common::MappingTable &mtp,
        const soclib::common::IntTab &srcid,
        size_t   tlb_ways,
        size_t   tlb_sets,
        size_t   icache_ways,
        size_t   icache_sets,
        size_t   icache_words,
        size_t   dcache_ways,
        size_t   dcache_sets,
        size_t   dcache_words,
        size_t   wbuf_nlines, 
        size_t   wbuf_nwords, 
        uint32_t max_frozen_cycles,
        uint32_t debug_start_cycle,
        bool     debug_ok);

    ~VciVcacheWrapper();

    void print_cpi();
    void print_stats();
    void print_trace(size_t mode = 0);
    void file_stats(FILE * file);
    void cache_monitor(paddr_t addr);
    inline void iss_set_debug_mask(uint v) {
	r_iss.set_debug_mask(v);
    }

private:
    void transition();
    void genMoore();

    soclib_static_assert((int)iss_t::SC_ATOMIC == (int)vci_param::STORE_COND_ATOMIC);
    soclib_static_assert((int)iss_t::SC_NOT_ATOMIC == (int)vci_param::STORE_COND_NOT_ATOMIC);
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




