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
 * Maintainers: alinevieiramello@hotmail.com, alain
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr> 
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr> 
 */

#ifndef __VCI_VGMN_H__ 
#define __VCI_VGMN_H__

#include <tlmdt>
#include "centralized_buffer.h"
#include "mapping_table.h"

namespace soclib { namespace tlmdt {

////////////////////////
class VciVgmn   
////////////////////////
  : public sc_core::sc_module  
{

private:
 
    typedef soclib::common::AddressDecodingTable<uint64_t, size_t> cmd_routing_table_t;     
    typedef soclib::common::AddressDecodingTable<uint32_t, bool>   cmd_locality_table_t;   
    typedef soclib::common::AddressDecodingTable<uint32_t, size_t> rsp_routing_table_t; 
    typedef soclib::common::AddressDecodingTable<uint32_t, bool>   rsp_locality_table_t;

    /////////////////////////////////////////////////////////////////////////////////////
    // Member Variables
    /////////////////////////////////////////////////////////////////////////////////////

    size_t                        m_inits;                // number of initiiators
    size_t                        m_targets;              // number of targets
    size_t                        m_latency;              // interconnect delay

    centralized_buffer            m_central_buffer; 	  // input fifos
 
    const cmd_routing_table_t     m_cmd_routing_table;    // command routing table
    const rsp_routing_table_t     m_rsp_routing_table; 	  // response routing table

    pdes_local_time*              m_pdes_local_time;      // local time (pointer)
 
    sc_core::sc_event             m_cmd_received;         // any command received

    // instrumentation counters
    size_t                        m_push_vci_count;
    size_t                        m_pop_vci_count;
    size_t                        m_push_null_count;
    size_t                        m_pop_null_count;
    size_t                        m_push_activity_count;
    size_t                        m_pop_activity_count;

    size_t                        m_null_sent_count;

    // FIELDS OF NULL TRANSACTION
    tlm::tlm_generic_payload      m_null_payload;
    tlm::tlm_phase                m_null_phase;
    sc_core::sc_time              m_null_time;
    soclib_payload_extension      m_null_extension;

    /////////////////////////////////////////////////////////////////////////////////////
    // Functions
    /////////////////////////////////////////////////////////////////////////////////////

    void execLoop(void);

    void route ( size_t                   from,       // port source
                 tlm::tlm_generic_payload &payload,   // payload
                 tlm::tlm_phase           &phase,     // phase
                 sc_core::sc_time         &time);     // time

    /////////////////////////////////////////////////////////////////////////////////////
    // Function executed when receiving command from VCI initiator[id]
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_fw ( int                      id,         
                                         tlm::tlm_generic_payload &payload,
                                         tlm::tlm_phase           &phase,
                                         sc_core::sc_time         &time); 
 
    /////////////////////////////////////////////////////////////////////////////////////
    // Function executed when receiving response from VCI target[id]
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_bw ( int                      id,       
                                         tlm::tlm_generic_payload &payload,
                                         tlm::tlm_phase           &phase, 
                                         sc_core::sc_time         &time );

protected:

    SC_HAS_PROCESS( VciVgmn );

public:  

    std::vector<tlm_utils::simple_target_socket_tagged
    <VciVgmn,32,tlm::tlm_base_protocol_types> *>             p_to_initiator;

    std::vector<tlm_utils::simple_initiator_socket_tagged
    <VciVgmn,32,tlm::tlm_base_protocol_types> *>             p_to_target;

    ////////////////////////////////
    // Constructor
    ////////////////////////////////

    VciVgmn( sc_core::sc_module_name             name,         // module name
	         const soclib::common::MappingTable  &mt,          // mapping table
	         const size_t                        n_inits,      // number of initiators
	         const size_t                        n_targets,    // number of targets
             const size_t                        min_latency,  // interconnect latency
             const size_t                        fifo_depth,   // unused in tlmdt
             const size_t                        default_tgtid = 0 );

    ~VciVgmn() {}

    ////////////////////////////////////////
    // Instrumentation functions
    ////////////////////////////////////////

    void print();

};

}}

#endif /* __VCI_VGMN__ */
