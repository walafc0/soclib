/* -*- mode: c++; coding: utf-8 -*-
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
 *
 * Maintainers:  alain
 *
 * Copyright (c) UPMC / Lip6, 2010
 *     Alain Greiner <alain.greiner@lip6.fr>
 */
#ifndef _TLMDT_XCACHE_WRAPPER_MULTI_H
#define _TLMDT_XCACHE_WRAPPER_MULTI_H
 
#include <tlmdt>
#include <inttypes.h>

#include "soclib_endian.h"
#include "multi_write_buffer.h"
#include "generic_cache.h"
#include "mapping_table.h"

namespace soclib { namespace tlmdt {

/////////////////////////////////////////////
template<typename vci_param, typename iss_t>
class VciXcacheWrapperMulti
/////////////////////////////////////////////
  : public sc_core::sc_module 
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> 
{
private:
  typedef typename vci_param::addr_t addr_t;
  typedef typename vci_param::data_t data_t;
  
    enum dcache_fsm_state_e {
        DCACHE_IDLE,
        DCACHE_WRITE_UPDT,
        DCACHE_WRITE_REQ,
        DCACHE_MISS_SELECT,
        DCACHE_MISS_INVAL,
        DCACHE_MISS_WAIT,
        DCACHE_UNC_WAIT,
        DCACHE_XTN_HIT,
        DCACHE_XTN_INVAL,
        DCACHE_XTN_SYNC,
    };

    enum icache_fsm_state_e {
        ICACHE_IDLE,
        ICACHE_MISS_SELECT,
        ICACHE_MISS_INVAL,
        ICACHE_MISS_WAIT,
        ICACHE_UNC_WAIT,
    };

    enum cmd_fsm_state_e {
        CMD_IDLE,
        CMD_INS_MISS,
        CMD_INS_UNC,
        CMD_DATA_MISS,
        CMD_DATA_UNC,
        CMD_DATA_WRITE,
    };

    enum rsp_fsm_state_e {
        RSP_IDLE,
        RSP_INS_MISS,
        RSP_INS_UNC,
        RSP_DATA_MISS,
        RSP_DATA_UNC,
        RSP_DATA_WRITE,
    };

    enum blocking_transaction_type_e {
        TYPE_DATA_UNC  = 0,
        TYPE_DATA_MISS = 1,
        TYPE_INS_UNC   = 2,
        TYPE_INS_MISS  = 3,
    };
  
  
    /////////////////////////////////////////////////////////////////////////////////////
    // Member Variables
    /////////////////////////////////////////////////////////////////////////////////////
    uint32_t                            m_srcid;       // VCI SRCID value
    iss_t                               m_iss;         // wrapped processor ISS

    typename iss_t::InstructionRequest  m_ireq;        // processor instruction request
    typename iss_t::InstructionResponse m_irsp;        // processor instruction response  

    typename iss_t::DataRequest         m_dreq;        // processor data request
    typename iss_t::DataResponse        m_drsp;        // processor data response

    bool*	                  m_irq_value;             // current value of IRQ[i]
    sc_core::sc_time*         m_irq_time;			   // date of last update IRQ[i]

    pdes_local_time*          m_pdes_local_time;       // thread local time
    pdes_activity_status*     m_pdes_activity_status;  // activity
    sc_core::sc_time          m_simulation_time;       // end of simulation date
  
    const size_t              m_icache_ways;
    const size_t              m_icache_sets;
    const size_t              m_icache_words;
    const addr_t              m_icache_yzmask;

    const size_t              m_dcache_ways;
    const size_t              m_dcache_sets;
    const size_t              m_dcache_words;
    const addr_t              m_dcache_yzmask;

    // REGISTERS  
    int                       m_dcache_fsm;
    addr_t                    m_dcache_addr_save;
    data_t                    m_dcache_wdata_save;
    data_t                    m_dcache_rdata_save;
    data_t                    m_dcache_be_save;
    int                       m_dcache_type_save;
    size_t                    m_dcache_way_save;
    size_t                    m_dcache_set_save;
    size_t                    m_dcache_word_save;
    bool                      m_dcache_cacheable_save;
    bool                      m_dcache_miss_req;
    bool                      m_dcache_unc_req;

    int                       m_icache_fsm;
    addr_t                    m_icache_addr_save;
    size_t                    m_icache_way_save;
    size_t                    m_icache_set_save;
    size_t                    m_icache_word_save;
    bool                      m_icache_miss_req;
    bool                      m_icache_unc_req;

    int                       m_vci_cmd_fsm;
    size_t                    m_vci_cmd_min;             // min word index in WBUF
    size_t                    m_vci_cmd_max;             // max word index in WBUF
    size_t                    m_vci_cmd_cpt;             // flit counter for a WRITE burst
    size_t                    m_vci_cmd_index;           // WBUF line index
  
    int                       m_vci_rsp_fsm;
    bool                      m_vci_rsp_ins_rok;         // From VCI_RSP to ICACHE
    bool                      m_vci_rsp_ins_error;       // From VCI_RSP to ICACHE
    bool                      m_vci_rsp_data_rok;        // From VCI_RSP to DCACHE
    bool                      m_vci_rsp_data_error;      // From VCI_RSP to DCACHE

    bool                      m_rsp_valid;               // From VCI Port to VCI_RSP_FSM
    uint32_t                  m_rsp_type;                // response type
    sc_core::sc_time          m_rsp_time;                // response date

    MultiWriteBuffer<addr_t>  m_wbuf;
    GenericCache<addr_t>      m_icache;
    GenericCache<addr_t>      m_dcache;

    soclib::common::AddressDecodingTable<uint64_t, bool> m_cacheability_table;
  
    // VCI COMMUNICATION
    unsigned int              m_nbytes;
    unsigned char             m_be_buf[64];    // 64 bytes
    unsigned char             m_data_buf[64];  // 64 bytes

    sc_core::sc_event         m_rsp_received;

    // Fields for the VCI IMISS or IUNC transaction (only one transaction)
    tlm::tlm_generic_payload  m_imiss_payload;
    soclib_payload_extension  m_imiss_extension;
    tlm::tlm_phase            m_imiss_phase;
    sc_core::sc_time          m_imiss_time;
    unsigned char*            m_imiss_data_buf;
    unsigned char*            m_imiss_be_buf;
  
    // Fields for the VCI DMISS or DUNC transaction (only one transaction)
    tlm::tlm_generic_payload  m_dmiss_payload;
    soclib_payload_extension  m_dmiss_extension;
    tlm::tlm_phase            m_dmiss_phase;
    sc_core::sc_time          m_dmiss_time;
    unsigned char*            m_dmiss_data_buf;
    unsigned char*            m_dmiss_be_buf;

    // Fields for the VCI WRITE transaction (up to 8 simultaneous transactions)
    tlm::tlm_generic_payload  m_write_payload[8];
    soclib_payload_extension  m_write_extension[8];
    tlm::tlm_phase            m_write_phase[8];
    sc_core::sc_time          m_write_time[8];
    unsigned char*            m_write_data_buf[8];
    unsigned char*            m_write_be_buf[8];

    // Fields for a NULL MESSAGE transaction
    tlm::tlm_generic_payload  m_null_payload;
    soclib_payload_extension  m_null_extension;
    tlm::tlm_phase            m_null_phase;
    sc_core::sc_time          m_null_time;

    // Fields for an ACTIVITY transaction
    tlm::tlm_generic_payload  m_activity_payload;
    soclib_payload_extension  m_activity_extension;
    tlm::tlm_phase            m_activity_phase;
    sc_core::sc_time          m_activity_time;

    // Cache Activity Counters
    uint32_t m_cpt_dcache_read;             // DCACHE DATA READ
    uint32_t m_cpt_dcache_write;            // DCACHE DATA WRITE
    uint32_t m_cpt_icache_read;             // ICACHE DATA READ
    uint32_t m_cpt_icache_write;            // ICACHE DATA WRITE
  
    // Instrumentation counters
    uint32_t m_cpt_exec_ins;  	            // number of executed instructions
    uint32_t m_pc_previous;   	            // previous value of PC
  
    uint32_t m_cpt_read;                    // total number of read instructions
    uint32_t m_cpt_write;                   // total number of write instructions
    uint32_t m_cpt_data_miss;               // number of read miss
    uint32_t m_cpt_ins_miss;                // number of instruction miss
    uint32_t m_cpt_data_unc;                // number of read uncached
    uint32_t m_cpt_ins_unc;                 // number of read uncached
    uint32_t m_cpt_write_cached;            // number of cached write
  

    /////////////////////////////////////////////////////////////////////////////////////
    // Fuctions
    /////////////////////////////////////////////////////////////////////////////////////
    void execLoop();
    void icache_fsm();
    void dcache_fsm();
    void vci_cmd_fsm();
    void vci_rsp_fsm();
  
    /////////////////////////////////////////////////////////////////////////////////////
    // Function executed when receiving a response on p_vci port
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload   &payload,  
                                        tlm::tlm_phase             &phase,   
                                        sc_core::sc_time           &time );  

    /////////////////////////////////////////////////////////////////////////////////////
    // Function  executed when receiving a command on p_irq[id] ports
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum irq_nb_transport_fw( int                      id, 
                                            tlm::tlm_generic_payload &payload, 
                                            tlm::tlm_phase           &phase,  
                                            sc_core::sc_time         &time );

    ///////////////////////////////////////////////////////////////////////////////////
    // Not implemented but required by interface
    ///////////////////////////////////////////////////////////////////////////////////
    void invalidate_direct_mem_ptr ( unsigned long long start_range, 
                                     unsigned long long end_range );

protected:

    SC_HAS_PROCESS( VciXcacheWrapperMulti );
  
public:

    tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types> 	p_vci;  

    std::vector<tlm_utils::simple_target_socket_tagged
    <VciXcacheWrapperMulti,32,tlm::tlm_base_protocol_types> *>      p_irq; 
  
    ///////////////////////////////////////////////////////////////////
    //      constructor
    ///////////////////////////////////////////////////////////////////
    VciXcacheWrapperMulti( sc_core::sc_module_name             name,
		                   const int                           procid,
		                   const soclib::common::MappingTable  &mt,
		                   const soclib::common::IntTab        &srcid,
		                   const size_t                        icache_ways,
		                   const size_t                        icache_sets,
		                   const size_t                        icache_words,
		                   const size_t                        dcache_ways,
		                   const size_t                        dcache_sets,
		                   const size_t                        dcache_words,
                           const size_t                        wbuf_words,
                           const size_t                        wbuf_lines,
		                   const size_t                        time_quantum = 100, 
                           const size_t                        max_cycles   = 0xFFFFFFFF );
  
    void print_stats();

};

}}

#endif
