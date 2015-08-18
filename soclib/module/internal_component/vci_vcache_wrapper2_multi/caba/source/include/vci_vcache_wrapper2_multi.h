/* -*- c++ -*-
 * File : vci_vcache_wrapper2_multi.h
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
 
#ifndef SOCLIB_CABA_VCI_VCACHE_WRAPPER2_MULTI_H
#define SOCLIB_CABA_VCI_VCACHE_WRAPPER2_MULTI_H

#include <inttypes.h>
#include <systemc>
#include "caba_base_module.h"
#include "multi_write_buffer.h"
#include "generic_cache.h"
#include "vci_initiator.h"
#include "mapping_table.h"
#include "generic_tlb.h"
#include "static_assert.h"

namespace soclib {
namespace caba {

using namespace sc_core;

////////////////////////////////////////////
template<typename vci_param, typename iss_t>
class VciVCacheWrapper2Multi
////////////////////////////////////////////
    : public soclib::caba::BaseModule
{
    typedef uint32_t vaddr_t;
    typedef uint32_t data_t;
    typedef uint32_t tag_t;
    typedef uint32_t type_t;
    typedef typename iss_t::DataOperationType data_op_t;

    typedef typename vci_param::addr_t paddr_t;
    typedef typename vci_param::be_t   be_t;

    enum icache_fsm_state_e {  
        ICACHE_IDLE,                // 00
        ICACHE_BIS,                 // 01
        ICACHE_TLB1_READ,           // 02
        ICACHE_TLB1_WRITE,          // 03
        ICACHE_TLB1_UPDT,           // 04
        ICACHE_TLB2_READ,           // 05
        ICACHE_TLB2_WRITE,          // 06
        ICACHE_TLB2_UPDT,           // 07
        ICACHE_SW_FLUSH,            // 08
        ICACHE_TLB_FLUSH,           // 09
        ICACHE_CACHE_FLUSH,         // 0a
        ICACHE_TLB_INVAL,           // 0b
        ICACHE_CACHE_INVAL,         // 0c
        ICACHE_MISS_WAIT,           // 0d
        ICACHE_UNC_WAIT,            // 0e
        ICACHE_MISS_UPDT,           // 0f
        ICACHE_ERROR,               // 10
	    ICACHE_CACHE_INVAL_PA,	    // 11
    };

    enum dcache_fsm_state_e {  
        DCACHE_IDLE,                // 00
        DCACHE_BIS,                 // 01
        DCACHE_DTLB1_READ_CACHE,    // 02
        DCACHE_TLB1_LL_WAIT,        // 03
        DCACHE_TLB1_SC_WAIT,        // 04
        DCACHE_TLB1_READ,           // 05
        DCACHE_TLB1_READ_UPDT,      // 06
        DCACHE_TLB1_UPDT,           // 07
        DCACHE_DTLB2_READ_CACHE,    // 08
        DCACHE_TLB2_LL_WAIT,        // 09
        DCACHE_TLB2_SC_WAIT,        // 0a
        DCACHE_TLB2_READ,           // 0b
        DCACHE_TLB2_READ_UPDT,      // 0c
        DCACHE_TLB2_UPDT,           // 0d
        DCACHE_CTXT_SWITCH,         // 0e
        DCACHE_ICACHE_FLUSH,        // 0f
        DCACHE_DCACHE_FLUSH,        // 10
        DCACHE_ITLB_INVAL,          // 11
        DCACHE_DTLB_INVAL,          // 12
        DCACHE_ICACHE_INVAL,        // 13
        DCACHE_DCACHE_INVAL,        // 14
        DCACHE_DCACHE_SYNC,         // 15
        DCACHE_LL_DIRTY_WAIT,       // 16
        DCACHE_SC_DIRTY_WAIT,       // 17
        DCACHE_WRITE_UPDT,          // 18
        DCACHE_WRITE_DIRTY,         // 19
        DCACHE_WRITE_REQ,           // 1a
        DCACHE_MISS_WAIT,           // 1b
        DCACHE_MISS_UPDT,           // 1c
        DCACHE_UNC_WAIT,            // 1d
        DCACHE_ERROR,               // 1e
        DCACHE_ITLB_READ,           // 1f
        DCACHE_ITLB_UPDT,           // 20
        DCACHE_ITLB_LL_WAIT,        // 21
        DCACHE_ITLB_SC_WAIT,        // 22
        DCACHE_ICACHE_INVAL_PA,     // 23
        DCACHE_DCACHE_INVAL_PA,     // 24
    };

    enum cmd_fsm_state_e {      
        CMD_IDLE,                   // 00
        CMD_ITLB_READ,              // 01
        CMD_ITLB_ACC_LL,            // 02
        CMD_ITLB_ACC_SC,            // 03
        CMD_INS_MISS,               // 04
        CMD_INS_UNC,                // 05
        CMD_DTLB_READ,              // 06
        CMD_DTLB_ACC_LL,            // 07
        CMD_DTLB_ACC_SC,            // 08
        CMD_DTLB_DIRTY_LL,          // 09
        CMD_DTLB_DIRTY_SC,          // 0a
        CMD_DATA_UNC,               // 0b
        CMD_DATA_MISS,              // 0c
        CMD_DATA_WRITE,             // 0d
    };

    enum rsp_fsm_state_e {       
        RSP_IDLE,                   // 00
        RSP_ITLB_READ,              // 01
        RSP_ITLB_ACC_LL,            // 02
        RSP_ITLB_ACC_SC,            // 03
        RSP_INS_MISS,               // 04
        RSP_INS_UNC,                // 05
        RSP_DTLB_READ,              // 06
        RSP_DTLB_ACC_LL,            // 07
        RSP_DTLB_ACC_SC,            // 08
        RSP_DTLB_DIRTY_LL,          // 09
        RSP_DTLB_DIRTY_SC,          // 0a
        RSP_DATA_MISS,              // 0b
        RSP_DATA_UNC,               // 0c
        RSP_DATA_WRITE,             // 0d
    };

    // TLB Mode ITLB / DTLB / ICACHE / DCACHE
    enum {          
        ALL_DEACTIVE    = 0x0,   // TLBs disactive caches disactive
        INS_TLB_MASK    = 0x8,
        DATA_TLB_MASK   = 0x4,
        INS_CACHE_MASK  = 0x2,
        DATA_CACHE_MASK = 0x1,
    };

    // Error Type
    enum mmu_error_type_e {
        MMU_NONE                      = 0x0000, // None
        MMU_WRITE_PT1_UNMAPPED 	      = 0x0001, // Write access of Page fault on Page Table 1          (non fatal error)
        MMU_WRITE_PT2_UNMAPPED 	      = 0x0002, // Write access of Page fault on Page Table 2          (non fatal error)
        MMU_WRITE_PRIVILEGE_VIOLATION = 0x0004, // Write access of Protected access in user mode       (user error)
        MMU_WRITE_ACCES_VIOLATION     = 0x0008, // Write access of write access to a non writable page (user error)
        MMU_WRITE_UNDEFINED_XTN       = 0x0020, // Write access of undefined external access address   (user error)
        MMU_WRITE_PT1_ILLEGAL_ACCESS  = 0x0040, // Write access of Bus Error accessing Table 1         (kernel error)
        MMU_WRITE_PT2_ILLEGAL_ACCESS  = 0x0080, // Write access of Bus Error accessing Table 2         (kernel error)
        MMU_WRITE_DATA_ILLEGAL_ACCESS = 0x0100, // Write access of Bus Error in cache access           (kernel error)
        MMU_READ_PT1_UNMAPPED 	      = 0x1001, // Read access of Page fault on Page Table 1  	       (non fatal error)
        MMU_READ_PT2_UNMAPPED 	      = 0x1002, // Read access of Page fault on Page Table 2  	       (non fatal error)
        MMU_READ_PRIVILEGE_VIOLATION  = 0x1004, // Read access of Protected access in user mode        (user error)
        MMU_READ_EXEC_VIOLATION       = 0x1010, // Exec access to a non exec page                      (user error)
        MMU_READ_UNDEFINED_XTN 	      = 0x1020, // Read access of Undefined external access address    (user error)
        MMU_READ_PT1_ILLEGAL_ACCESS   = 0x1040, // Read access of Bus Error in Table1 access           (kernel error)
        MMU_READ_PT2_ILLEGAL_ACCESS   = 0x1080, // Read access of Bus Error in Table2 access 	       (kernel error)
        MMU_READ_DATA_ILLEGAL_ACCESS  = 0x1100, // Read access of Bus Error in cache access 	       (kernel error)
    };

    enum transaction_type_e {
        TYPE_DATA_MISS   = 0x0,
        TYPE_DATA_UNC    = 0x2,
        TYPE_INS_MISS    = 0x4,
        TYPE_INS_UNC     = 0x6,
        TYPE_INS_TLB     = 0x8,
        TYPE_INS_LL_ACC  = 0xa,
        TYPE_INS_SC_ACC  = 0xc,
        TYPE_DATA_TLB    = 0xe,
        TYPE_DATA_LL_ACC = 0x10,
        TYPE_DATA_SC_ACC = 0x12,
        TYPE_DATA_LL_D   = 0x14,
        TYPE_DATA_SC_D   = 0x16,
    };

public:
    sc_in<bool>                             p_clk;
    sc_in<bool>                             p_resetn;
    sc_in<bool>                             p_irq[iss_t::n_irq];
    soclib::caba::VciInitiator<vci_param>   p_vci;

private:
    // STRUCTURAL PARAMETERS
    soclib::common::AddressDecodingTable<uint32_t, bool>    m_cacheability_table;
    const uint32_t                                          m_srcid;
    iss_t                                                   m_iss;   

    const size_t  m_itlb_ways;
    const size_t  m_itlb_sets;

    const size_t  m_dtlb_ways;
    const size_t  m_dtlb_sets;

    const size_t  m_icache_ways;
    const size_t  m_icache_sets;
    const size_t  m_icache_yzmask;
    const size_t  m_icache_words;

    const size_t  m_dcache_ways;
    const size_t  m_dcache_sets;
    const size_t  m_dcache_yzmask;
    const size_t  m_dcache_words;

    const size_t  m_paddr_nbits;  

    // instruction and data vcache tlb instances 
    soclib::caba::GenericTlb<paddr_t>    icache_tlb;
    soclib::caba::GenericTlb<paddr_t>    dcache_tlb;

    sc_signal<vaddr_t>      r_mmu_ptpr;             // page table pointer register
    sc_signal<int>          r_mmu_mode;             // mmu mode register
    sc_signal<int>          r_mmu_params;           // mmu parameters register
    sc_signal<int>          r_mmu_release;          // mmu release register
    sc_signal<int>          r_mmu_word_lo;          // mmu misc data low
    sc_signal<int>          r_mmu_word_hi;          // mmu mmu misc data hight

    // DCACHE FSM REGISTERS
    sc_signal<int>          r_dcache_fsm;               // state register
    sc_signal<paddr_t>      r_dcache_paddr_save;        // physical address
    sc_signal<data_t>       r_dcache_wdata_save;        // write data
    sc_signal<data_t>       r_dcache_rdata_save;        // read data
    sc_signal<type_t>       r_dcache_type_save;         // access type
    sc_signal<be_t>         r_dcache_be_save;           // byte enable
    sc_signal<bool>         r_dcache_cached_save;       // used by the write buffer
    sc_signal<paddr_t>      r_dcache_tlb_paddr;         // physical address of tlb miss
    sc_signal<bool>         r_dcache_dirty_save;        // used for TLB dirty bit update
    sc_signal<size_t>       r_dcache_tlb_set_save;      // used for TLB dirty bit update
    sc_signal<size_t>       r_dcache_tlb_way_save;      // used for TLB dirty bit update
    sc_signal<vaddr_t>      r_dcache_id1_save;          // used by the PT1 bypass
    sc_signal<paddr_t>      r_dcache_ptba_save;         // used by the PT1 bypass
    sc_signal<bool>         r_dcache_ptba_ok;           // used by the PT1 bypass
    sc_signal<data_t>       r_dcache_pte_update;        // used for page table update
    sc_signal<data_t>       r_dcache_ppn_update;        // used for physical page number update
    sc_signal<tag_t>        r_dcache_ppn_save;          // used for speculative cache access
    sc_signal<tag_t>        r_dcache_vpn_save;          // used for speculative cache access
    sc_signal<bool>         r_dtlb_translation_valid;   // used for speculative address
    sc_signal<bool>         r_dcache_hit_p_save;        // used to save hit_p in case BIS

    sc_signal<data_t>       r_dcache_error_type;        // software visible register
    sc_signal<vaddr_t>      r_dcache_bad_vaddr;         // software visible register 

    sc_signal<bool>         r_dcache_miss_req;          // used for cached read miss
    sc_signal<bool>         r_dcache_unc_req;           // used for uncached read miss
    sc_signal<bool>         r_dcache_write_req;         // used for write 
    sc_signal<bool>         r_dcache_tlb_read_req;      // used for tlb ptba or pte read
 
    sc_signal<bool>         r_dcache_tlb_ll_acc_req;    // used for tlb access bit update
    sc_signal<bool>         r_dcache_tlb_sc_acc_req;    // used for tlb access bit update
    sc_signal<bool>         r_dcache_tlb_ll_dirty_req;  // used for tlb dirty bit update 
    sc_signal<bool>         r_dcache_tlb_sc_dirty_req;  // used for tlb dirty bit update 
    sc_signal<bool>         r_dcache_tlb_ptba_read;     // used for tlb ptba read when write dirty bit 
    sc_signal<bool>         r_dcache_xtn_req;           // used for xtn write for ICACHE

    // ICACHE FSM REGISTERS
    sc_signal<int>          r_icache_fsm;               // state register
    sc_signal<paddr_t>      r_icache_paddr_save;        // physical address
    sc_signal<vaddr_t>      r_icache_id1_save;          // used by the PT1 bypass
    sc_signal<paddr_t>      r_icache_ptba_save;         // used by the PT1 bypass
    sc_signal<bool>         r_icache_ptba_ok;           // used by the PT1 bypass
    sc_signal<data_t>       r_icache_pte_update;        // used for page table update
    sc_signal<tag_t>        r_icache_ppn_save;          // used for speculative cache access
    sc_signal<tag_t>        r_icache_vpn_save;          // used for speculative cache access
    sc_signal<bool>         r_itlb_translation_valid;   // used for speculative physical address

    sc_signal<data_t>       r_icache_error_type;        // software visible registers
    sc_signal<vaddr_t>      r_icache_bad_vaddr;         // software visible registers

    sc_signal<bool>         r_icache_miss_req;          // used for cached read miss
    sc_signal<bool>         r_icache_unc_req;           // used for uncached read miss
    sc_signal<bool>         r_dcache_itlb_read_req;     // used for tlb ptba or pte read 

    sc_signal<bool>         r_dcache_itlb_ll_acc_req;   // used for tlb access bit update
    sc_signal<bool>         r_dcache_itlb_sc_acc_req;   // used for tlb access bit update

    sc_signal<bool>	        r_itlb_read_dcache_req;     // used for instruction tlb miss, request in data cache
    sc_signal<bool>	        r_itlb_acc_dcache_req;      // used for itlb update entry type bits via dcache
    sc_signal<bool>	        r_dcache_rsp_itlb_error;    // used for data cache rsp error when itlb miss
    sc_signal<data_t>	    r_dcache_rsp_itlb_miss;	// used for dcache rsp data when itlb miss
    sc_signal<data_t>	    r_dcache_rsp_itlb_ppn;	// used for dcache rsp data when itlb miss
    sc_signal<vaddr_t>      r_icache_vaddr_req;		// virtual address requested by the CPU

    // VCI_CMD FSM REGISTERS
    sc_signal<int>          r_vci_cmd_fsm;
    sc_signal<size_t>       r_vci_cmd_min;       
    sc_signal<size_t>       r_vci_cmd_max;       
    sc_signal<size_t>       r_vci_cmd_cpt;     

    // VCI_RSP FSM REGISTERS
    sc_signal<int>          r_vci_rsp_fsm;
    sc_signal<size_t>       r_vci_rsp_cpt;
    sc_signal<bool>         r_vci_rsp_ins_error;
    sc_signal<bool>         r_vci_rsp_data_error;
    sc_signal<bool>         r_vci_rsp_ins_ok;
    sc_signal<bool>         r_vci_rsp_data_ok;

    data_t                  *r_icache_miss_buf;    
    data_t                  *r_dcache_miss_buf;  

    MultiWriteBuffer<paddr_t>   r_wbuf;
    GenericCache<paddr_t>       r_icache;
    GenericCache<paddr_t>       r_dcache;

    // Activity counters
    uint32_t m_cpt_dcache_data_read;        // DCACHE DATA READ
    uint32_t m_cpt_dcache_data_write;       // DCACHE DATA WRITE
    uint32_t m_cpt_dcache_dir_read;         // DCACHE DIR READ
    uint32_t m_cpt_dcache_dir_write;        // DCACHE DIR WRITE

    uint32_t m_cpt_icache_data_read;        // ICACHE DATA READ
    uint32_t m_cpt_icache_data_write;       // ICACHE DATA WRITE
    uint32_t m_cpt_icache_dir_read;         // ICACHE DIR READ
    uint32_t m_cpt_icache_dir_write;        // ICACHE DIR WRITE

    uint32_t m_cpt_frz_cycles;	            // number of cycles where the cpu is frozen
    uint32_t m_cpt_total_cycles;	        // total number of cycles 

    // Cache activity counters
    uint32_t m_cpt_read;                    // total number of read data
    uint32_t m_cpt_write;                   // total number of write data
    uint32_t m_cpt_data_miss;               // number of read miss
    uint32_t m_cpt_ins_miss;                // number of instruction miss
    uint32_t m_cpt_unc_read;                // number of read uncached
    uint32_t m_cpt_write_cached;            // number of cached write
    uint32_t m_cpt_ins_read;                // number of instruction read

    uint32_t m_cost_write_frz;              // number of frozen cycles related to write buffer         
    uint32_t m_cost_data_miss_frz;          // number of frozen cycles related to data miss
    uint32_t m_cost_unc_read_frz;           // number of frozen cycles related to uncached read
    uint32_t m_cost_ins_miss_frz;           // number of frozen cycles related to ins miss

    uint32_t m_cpt_imiss_transaction;       // number of VCI instruction miss transactions
    uint32_t m_cpt_dmiss_transaction;       // number of VCI data miss transactions
    uint32_t m_cpt_unc_transaction;         // number of VCI uncached read transactions
    uint32_t m_cpt_write_transaction;       // number of VCI write transactions

    uint32_t m_cost_imiss_transaction;      // cumulated duration for VCI IMISS transactions
    uint32_t m_cost_dmiss_transaction;      // cumulated duration for VCI DMISS transactions
    uint32_t m_cost_unc_transaction;        // cumulated duration for VCI UNC transactions
    uint32_t m_cost_write_transaction;      // cumulated duration for VCI WRITE transactions
    uint32_t m_length_write_transaction;    // cumulated length for VCI WRITE transactions

    // TLB activity counters
    uint32_t m_cpt_ins_tlb_read;            // number of instruction tlb read
    uint32_t m_cpt_ins_tlb_miss;            // number of instruction tlb miss
    uint32_t m_cpt_ins_tlb_write_et;        // number of instruction tlb write ET

    uint32_t m_cpt_data_tlb_read;           // number of data tlb read
    uint32_t m_cpt_data_tlb_miss;           // number of data tlb miss
    uint32_t m_cpt_data_tlb_write_et;       // number of data tlb write ET
    uint32_t m_cpt_data_tlb_write_dirty;    // number of data tlb write dirty
    
    uint32_t m_cost_ins_tlb_miss_frz;       // number of frozen cycles related to instruction tlb miss
    uint32_t m_cost_data_tlb_miss_frz;      // number of frozen cycles related to data tlb miss

    uint32_t m_cpt_itlbmiss_transaction;    // number of itlb miss transactions
    uint32_t m_cpt_itlb_write_transaction;  // number of itlb write ET transactions
    uint32_t m_cpt_dtlbmiss_transaction;    // number of dtlb miss transactions
    uint32_t m_cpt_dtlb_write_transaction;  // number of dtlb write ET and dirty transactions

    uint32_t m_cost_itlbmiss_transaction;   // cumulated duration for VCI instruction TLB miss transactions
    uint32_t m_cost_itlb_write_transaction; // cumulated duration for VCI instruction TLB write ET transactions
    uint32_t m_cost_dtlbmiss_transaction;   // cumulated duration for VCI data TLB miss transactions
    uint32_t m_cost_dtlb_write_transaction; // cumulated duration for VCI data TLB write transactions

protected:
    SC_HAS_PROCESS(VciVCacheWrapper2Multi);

public:
    VciVCacheWrapper2Multi(
        sc_module_name insname,
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
        size_t wbuf_nlines );

    ~VciVCacheWrapper2Multi();

    void print_cpi();
    void print_stats();

private:
    void transition();
    void genMoore();

    soclib_static_assert((int)iss_t::SC_ATOMIC == (int)vci_param::STORE_COND_ATOMIC);
    soclib_static_assert((int)iss_t::SC_NOT_ATOMIC == (int)vci_param::STORE_COND_NOT_ATOMIC);
};

}}

#endif /* SOCLIB_CABA_VCI_VCACHE_WRAPPER2_MULTI_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


