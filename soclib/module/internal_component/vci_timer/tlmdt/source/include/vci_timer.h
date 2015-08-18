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
 * Maintainers: alain
 *
 * Copyright (c) UPMC / Lip6, 2011
 *     alain.greiner@lip6.fr
 */

//////////////////////////////////////////////////////////////////////////////
// Implementation Note :
// This component is a VCI target, with a variable number of IRQ ports,
// associated to a variable number of timers.
// It is modeled as a purely reactive interface function : no thread.
// It has a local time that is updated when a command is received on the 
// VCI port. This behavior suppose that the interconnect send periodically
// synchronization NULL messages to all connected targets.
// The IRQ values are transmited on the IRQ ports each time a command is 
// received on the VCI port (NULL message or VCI command).
//////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_TLMDT_VCI_TIMER_H
#define SOCLIB_TLMDT_VCI_TIMER_H

#include <tlmdt>  
#include "mapping_table.h"
#include "timer.h"

namespace soclib { namespace tlmdt {

template <typename vci_param>
class VciTimer 
  : public sc_core::sc_module    
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types>
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
{
private:

    typedef typename vci_param::addr_t	addr_t;
    typedef typename vci_param::data_t	data_t;

    // Member Variables
    soclib::common::Segment  	m_segment;
    size_t			m_timers;
    sc_core::sc_time		m_local_time;

    // registers for each timer
    int*			m_timer_mode;
    int*			m_timer_period;
    int*			m_timer_value;
    int*			m_timer_counter;
    uint8_t*			m_irq_value;
  
    // IRQ TRANSACTIONS (one for each timer)
    tlm::tlm_generic_payload*   m_irq_payload;
    tlm::tlm_phase              m_irq_phase;
    sc_core::sc_time 		m_irq_time;

    // Functions 
    tlm::tlm_sync_enum nb_transport_fw ( tlm::tlm_generic_payload &payload,
                                         tlm::tlm_phase           &phase,
                                         sc_core::sc_time         &time);
    tlm::tlm_sync_enum irq_nb_transport_bw ( int                      id,
                                             tlm::tlm_generic_payload &payload,
                                             tlm::tlm_phase           &phase,
                                             sc_core::sc_time         &time);

    // Not implemented but mandatory
    void b_transport ( tlm::tlm_generic_payload &payload,
                       sc_core::sc_time         &time);

    bool get_direct_mem_ptr ( tlm::tlm_generic_payload &payload,
                              tlm::tlm_dmi             &dmi_data);

    unsigned int transport_dbg ( tlm::tlm_generic_payload &payload);

    void invalidate_direct_mem_ptr ( sc_dt::uint64 start_range,
                                     sc_dt::uint64 end_range);

    tlm::tlm_sync_enum nb_transport_bw ( tlm::tlm_generic_payload &payload,
                                         tlm::tlm_phase           &phase,
                                         sc_core::sc_time         &time);

protected:
    SC_HAS_PROCESS(VciTimer);

public:
    tlm::tlm_target_socket<32,tlm::tlm_base_protocol_types>                                            p_vci; 
    std::vector<tlm_utils::simple_initiator_socket_tagged<VciTimer,32,tlm::tlm_base_protocol_types> *> p_irq; 

    // constructor
    VciTimer( sc_core::sc_module_name name,
	      const soclib::common::IntTab &index,
	      const soclib::common::MappingTable &mt,
	      size_t ntimer);
  
    ~VciTimer();

};
}}

#endif /* SOCLIB_TLMDT_VCI_TIMER_H */
