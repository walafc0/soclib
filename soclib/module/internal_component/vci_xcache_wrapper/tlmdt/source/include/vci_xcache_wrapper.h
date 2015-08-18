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
 * Maintainers: alinev, alain
 *
 * Copyright (c) UPMC / Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr>
 */
#ifndef _TLMDT_XCACHE_WRAPPER_H
#define _TLMDT_XCACHE_WRAPPER_H
 
#include <tlmdt>
#include <inttypes.h>

#include "soclib_endian.h"
#include "write_buffer.h"
#include "generic_cache.h"
#include "mapping_table.h"

namespace soclib { namespace tlmdt {

template<typename vci_param, typename iss_t>
class VciXcacheWrapper
  : public sc_core::sc_module                                             // inherit from SC module base clase
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "backward interface"
{
private:
  typedef typename vci_param::addr_t addr_t;
  typedef typename vci_param::data_t data_t;
  typedef typename vci_param::data_t tag_t;
  typedef typename vci_param::data_t be_t;
  
  enum dcache_fsm_state_e {
    DCACHE_IDLE,
    DCACHE_WRITE_UPDT,
    DCACHE_WRITE_REQ,
    DCACHE_MISS_WAIT,
    DCACHE_MISS_UPDT,
    DCACHE_UNC_WAIT,
    DCACHE_INVAL,
    DCACHE_ERROR,
  };

  enum icache_fsm_state_e {
    ICACHE_IDLE,
    ICACHE_MISS_WAIT,
    ICACHE_MISS_UPDT,
    ICACHE_UNC_WAIT,
    ICACHE_ERROR,
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
    RSP_DATA_WRITE_TIME_WAIT,
  };
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Member Variables
  /////////////////////////////////////////////////////////////////////////////////////
  uint32_t                m_id;
  iss_t                   m_iss;

  bool*	                  m_pending_irq;
  sc_core::sc_time*       m_pending_time;

  pdes_local_time        *m_pdes_local_time;
  pdes_activity_status   *m_pdes_activity_status;
  sc_core::sc_time        m_simulation_time;
  
  const size_t            m_icache_ways;
  const size_t            m_icache_sets;
  const size_t            m_icache_words;
  const addr_t            m_icache_yzmask;

  const size_t            m_dcache_ways;
  const size_t            m_dcache_sets;
  const size_t            m_dcache_words;
  const addr_t            m_dcache_yzmask;

  // REGISTERS
  int          m_dcache_fsm;
  addr_t       m_dcache_addr_save;
  data_t       m_dcache_wdata_save;
  data_t       m_dcache_rdata_save;
  int          m_dcache_type_save;
  be_t         m_dcache_be_save;
  bool         m_dcache_cached_save;
  bool         m_dcache_miss_req;
  bool         m_dcache_unc_req;
  bool         m_dcache_write_req;
  data_t       m_dcache_write_time_req;
  data_t       m_dcache_read_time_req;

  int          m_icache_fsm;
  addr_t       m_icache_addr_save;
  bool         m_icache_miss_req;
  bool         m_icache_unc_req;
  data_t       m_icache_time_req;

  int          m_vci_cmd_fsm;
  size_t       m_vci_cmd_min;
  size_t       m_vci_cmd_max;
  size_t       m_vci_cmd_cpt;
  
  int          m_vci_rsp_fsm;
  bool         m_vci_rsp_ins_error;
  bool         m_vci_rsp_data_error;
  size_t       m_vci_rsp_cpt;

  data_t      *m_icache_miss_buf;
  data_t      *m_dcache_miss_buf;
  bool         m_icache_buf_unc_valid;
  bool         m_dcache_buf_unc_valid;

 
  WriteBuffer<addr_t>     m_wbuf;
  GenericCache<addr_t>    m_icache;
  GenericCache<addr_t>    m_dcache;
  soclib::common::AddressDecodingTable<addr_t, bool> m_cacheability_table;
  
  //VCI COMMUNICATION
  unsigned int            m_nbytes;
  unsigned char           m_byte_enable_ptr[MAXIMUM_PACKET_SIZE * vci_param::nbytes];
  unsigned char           m_data_ptr[MAXIMUM_PACKET_SIZE * vci_param::nbytes];
  sc_core::sc_event       m_rsp_received;
  sc_core::sc_time        m_rsp_time;

  //FIELDS OF A NORMAL MESSAGE
  tlm::tlm_generic_payload *m_payload_ptr;
  soclib_payload_extension *m_extension_ptr;
  tlm::tlm_phase            m_phase;
  sc_core::sc_time          m_time;
  
  //FIELDS OF A NULL MESSAGE
  tlm::tlm_generic_payload *m_null_payload_ptr;
  soclib_payload_extension *m_null_extension_ptr;
  tlm::tlm_phase            m_null_phase;
  sc_core::sc_time          m_null_time;

  //FIELDS OF AN ACTIVITY STATUS MESSAGE
  tlm::tlm_generic_payload *m_activity_payload_ptr;
  soclib_payload_extension *m_activity_extension_ptr;
  tlm::tlm_phase            m_activity_phase;
  sc_core::sc_time          m_activity_time;

  // Activity counters
  uint32_t m_cpt_dcache_data_read;        // DCACHE DATA READ
  uint32_t m_cpt_dcache_data_write;       // DCACHE DATA WRITE
  uint32_t m_cpt_dcache_dir_read;         // DCACHE DIR READ
  uint32_t m_cpt_dcache_dir_write;        // DCACHE DIR WRITE
  
  uint32_t m_cpt_icache_data_read;        // ICACHE DATA READ
  uint32_t m_cpt_icache_data_write;       // ICACHE DATA WRITE
  uint32_t m_cpt_icache_dir_read;         // ICACHE DIR READ
  uint32_t m_cpt_icache_dir_write;        // ICACHE DIR WRITE
  
  uint32_t m_cpt_frz_cycles;	          // number of cycles where the cpu is frozen
  //uint32_t m_cpt_total_cycles;	          // total number of cycles
  
  uint32_t m_cpt_read;                    // total number of read instructions
  uint32_t m_cpt_write;                   // total number of write instructions
  uint32_t m_cpt_data_miss;               // number of read miss
  uint32_t m_cpt_ins_miss;                // number of instruction miss
  uint32_t m_cpt_unc_read;                // number of read uncached
  uint32_t m_cpt_write_cached;            // number of cached write
  
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
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Fuctions
  /////////////////////////////////////////////////////////////////////////////////////
  void execLoop();

  void init( size_t time_quantum );
  void iss();
  void frozen_iss(int time);
  void cmd_fsm();
  void rsp_fsm();
  void update_time(sc_core::sc_time t);
  void send_null_message();
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (VCI INITIATOR SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw         // receive rsp from target
  ( tlm::tlm_generic_payload   &payload,     // payload
    tlm::tlm_phase             &phase,       // phase
    sc_core::sc_time           &time);       // time

  void invalidate_direct_mem_ptr             // invalidate_direct_mem_ptr
  ( sc_dt::uint64 start_range,               // start range
    sc_dt::uint64 end_range);                // end range
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_fw_transport_if (IRQ TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum irq_nb_transport_fw     // receive interruption from initiator
  ( int                      id,             // interruption id
    tlm::tlm_generic_payload &payload,       // payload
    tlm::tlm_phase           &phase,         // phase
    sc_core::sc_time         &time);         // time

protected:
  SC_HAS_PROCESS(VciXcacheWrapper);
  
public:
  tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types> 	p_vci;  
  std::vector<tlm_utils::simple_target_socket_tagged<VciXcacheWrapper,32,tlm::tlm_base_protocol_types> *> p_irq; 
  
  VciXcacheWrapper(
		   sc_core::sc_module_name name,
		   int cpuid,
		   const soclib::common::IntTab &index,
		   const soclib::common::MappingTable &mt,
		   size_t icache_ways,
		   size_t icache_sets,
		   size_t icache_words,
		   size_t dcache_ways,
		   size_t dcache_sets,
		   size_t dcache_words,
		   sc_core::sc_time time_quantum);
  
  VciXcacheWrapper(
		   sc_core::sc_module_name name,
		   int cpuid,
		   const soclib::common::MappingTable &mt,
		   const soclib::common::IntTab &index,
		   size_t icache_ways,
		   size_t icache_sets,
		   size_t icache_words,
		   size_t dcache_ways,
		   size_t dcache_sets,
		   size_t dcache_words);

  VciXcacheWrapper(
		   sc_core::sc_module_name name,
		   int cpuid,
		   const soclib::common::MappingTable &mt,
		   const soclib::common::IntTab &index,
		   size_t icache_ways,
		   size_t icache_sets,
		   size_t icache_words,
		   size_t dcache_ways,
		   size_t dcache_sets,
		   size_t dcache_words,
		   size_t time_quantum);

  VciXcacheWrapper(
		   sc_core::sc_module_name name,
		   int cpuid,
		   const soclib::common::MappingTable &mt,
		   const soclib::common::IntTab &index,
		   size_t icache_ways,
		   size_t icache_sets,
		   size_t icache_words,
		   size_t dcache_ways,
		   size_t dcache_sets,
		   size_t dcache_words,
		   size_t time_quantum,
		   size_t simulation_time);

  void print_cpi();
  void print_stats();

  const sc_core::sc_time get_time() const
  {
    return m_pdes_local_time->get();
  }

};

}}

#endif
