/* -*- c++ -*-
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
 * Maintainers: fpecheux, alain, aline
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

#ifndef __INTERCONNECT_H__ 
#define __INTERCONNECT_H__

#include <tlmdt>	            // TLM-DT headers
#include "mapping_table.h"                        
#include "centralized_buffer.h"          

namespace soclib { namespace tlmdt {

////////////////////
class Interconnect 
////////////////////
  : public sc_core::sc_module  
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> 
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> 
{
private:
 
  typedef soclib::common::AddressDecodingTable<uint64_t, size_t> cmd_routing_table_t;     
  typedef soclib::common::AddressDecodingTable<uint32_t, bool>   cmd_locality_table_t;   
  typedef soclib::common::AddressDecodingTable<uint32_t, size_t> rsp_routing_table_t; 
  typedef soclib::common::AddressDecodingTable<uint32_t, bool>   rsp_locality_table_t;

  /////////////////////////////////////////////////////////////////////////////////////
  // Member Variables
  /////////////////////////////////////////////////////////////////////////////////////
  size_t                        m_id;                 	// identifier
  size_t                        m_inits;              	// number of initiiators
  size_t                        m_targets;            	// number of targets
  size_t                        m_delay;              	// interconnect delay
  size_t                        m_local_delta_time;	    // minimal time between cmd/rsp
  size_t                        m_no_local_delta_time;  // minimal time between cmd/rsp
  bool                          m_is_local_crossbar;  	// true if local interconnect

  centralized_buffer            m_centralized_buffer; 	// centralized buffer
  const cmd_routing_table_t     m_cmd_routing_table;    // command routing table
  const cmd_locality_table_t    m_cmd_locality_table;   // command locality table
  const rsp_routing_table_t     m_rsp_routing_table; 	// response routing table
  const rsp_locality_table_t    m_rsp_locality_table;	// response locality table
  pdes_local_time*              m_pdes_local_time;    	// local time (pointer)

  // instrumentation counters
  size_t                        m_msg_count;
  size_t                        m_local_msg_count;
  size_t                        m_non_local_msg_count;
  size_t                        m_token_msg_count;
  size_t                        m_wait_count;
  size_t                        m_notify_count;
  size_t                        m_pop_count;

  // FIELDS OF TOKEN TRANSACTION
  tlm::tlm_generic_payload      m_payload_token;
  tlm::tlm_phase                m_phase_token;
  sc_core::sc_time              m_time_token;
  soclib_payload_extension      m_extension_token;

  // FIELDS OF NULL TRANSACTION
  tlm::tlm_generic_payload      m_null_payload;
  tlm::tlm_phase                m_null_phase;
  sc_core::sc_time              m_null_time;
  soclib_payload_extension      m_null_extension;

  /////////////////////////////////////////////////////////////////////////////////////
  // Functions
  /////////////////////////////////////////////////////////////////////////////////////
  void init();

  void execLoop(void);

  void route ( size_t                   from,       // port source
               tlm::tlm_generic_payload &payload,   // payload
               tlm::tlm_phase           &phase,     // phase
               sc_core::sc_time         &time);     // time

  void create_token();

  /////////////////////////////////////////////////////////////////////////////////////
  // Function executed when receiving command from VCI initiator
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_fw ( int                      id,         // socket id
                                       tlm::tlm_generic_payload &payload,   // payload
                                       tlm::tlm_phase           &phase,     // phase
                                       sc_core::sc_time         &time);     // time
 
  /////////////////////////////////////////////////////////////////////////////////////
  // Function executed when receiving response from VCI target
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw ( int                       id,        // socket id
                                       tlm::tlm_generic_payload &payload,   // payload
                                       tlm::tlm_phase           &phase,     // phase
                                       sc_core::sc_time         &time);     // time

 protected:

  SC_HAS_PROCESS(Interconnect);

public:  

  std::vector<tlm_utils::simple_target_socket_tagged
  <Interconnect,32,tlm::tlm_base_protocol_types> *>       p_to_initiator;

  std::vector<tlm_utils::simple_initiator_socket_tagged
  <Interconnect,32,tlm::tlm_base_protocol_types> *>       p_to_target;

  ////////////////////////////////
  // Constructors
  ////////////////////////////////

  // Global interconnect
  Interconnect( sc_core::sc_module_name     module_name,   // module name
	            const size_t                id,            // identifier
	            const cmd_routing_table_t   &cmd_rt,       // command routing table
	            const rsp_routing_table_t   &rsp_rt,       // response routing table
	            const size_t                n_inits,       // number of initiators
	            const size_t                n_targets,     // number of targets
	            const size_t                delay );       // interconnect latency

  // Global interconnect without identifier
  Interconnect( sc_core::sc_module_name     module_name,   // module name
	            const cmd_routing_table_t   &cmd_rt,       // command routing table
	            const rsp_routing_table_t   &rsp_rt,       // response routing table
	            const size_t                n_inits,       // number of initiators
	            const size_t                n_targets,     // number of targets
	            const size_t                delay );       // interconnect latency

  // Local interconnect
  Interconnect( sc_core::sc_module_name     module_name,   // module name
	            const size_t                id,            // identifier
	            const cmd_routing_table_t   &cmd_rt,       // command routing table
	            const cmd_locality_table_t  &cmd_lt,       // command locality table
	            const rsp_routing_table_t   &rsp_rt,       // response routing table
	            const rsp_locality_table_t  &rsp_lt,       // response locality table
	            const size_t                n_inits,       // number of initators
	            const size_t                n_targets,     // number of targets
	            const size_t                delay );       // interconnect latency

  // Local interconnect without identifier
  Interconnect( sc_core::sc_module_name     module_name,   // module name
	            const cmd_routing_table_t   &cmd_rt,       // command routing table
	            const cmd_locality_table_t  &cmd_lt,       // command locality table
	            const rsp_routing_table_t   &rsp_rt,       // response routing table
	            const rsp_locality_table_t  &rsp_lt,       // response locality table
	            size_t                      n_inits,       // number of initators
	            size_t                      n_targets,     // number of targets
	            size_t                      delay );       // interconnect latency

  ~Interconnect();

  uint32_t getLocalMsgCounter();

  uint32_t getNonLocalMsgCounter();

  uint32_t getTokenMsgCounter();

  void print();

};

}}

#endif /* __INTERCONNECT__ */
