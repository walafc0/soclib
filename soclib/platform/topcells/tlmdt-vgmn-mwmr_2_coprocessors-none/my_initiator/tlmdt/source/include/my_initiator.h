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
 * Maintainers: fpecheux, alinev
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef __MY_INITIATOR_H__
#define __MY_INITIATOR_H__

#include <tlmdt>                                // TLM-DT headers
#include "mapping_table.h"                      // mapping table

class my_initiator                              // my_initiator 
  : public sc_core::sc_module                   // inherit from SC module base clase
{
private:
  //Variables
  typedef soclib::tlmdt::VciParams<uint32_t,uint32_t> vci_param;

  uint32_t                           m_srcid;
  uint32_t                           m_pktid;
  soclib::common::MappingTable       m_mt;
  pdes_local_time                   *m_pdes_local_time;
  pdes_activity_status              *m_pdes_activity_status;
  sc_core::sc_time                   m_simulation_time;
  sc_core::sc_event                  m_rspEvent;

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuctions
  /////////////////////////////////////////////////////////////////////////////////////
  void send_activity();
  void send_null_message();
  void configureChannelStatus(int n_channels);
  void configureMwmr(vci_param::addr_t mwmr_address, vci_param::addr_t channel_address, int n_channel, bool is_read);
  void writeConfigCopro(vci_param::addr_t address, int reg, int value);
  int readStatusCopro(vci_param::addr_t address, int reg);

  void behavior(void);                              // initiator thread

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (VCI INITIATOR SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum my_nb_transport_bw      // Receive rsp from target
  ( tlm::tlm_generic_payload   &payload,     // payload
    tlm::tlm_phase             &phase,       // phase
    sc_core::sc_time           &time);       // time

protected:
  SC_HAS_PROCESS(my_initiator);
  
public:
  tlm_utils::simple_initiator_socket<my_initiator, 32, tlm::tlm_base_protocol_types> p_vci_initiator; // VCI initiator port 

  my_initiator(                                              // constructor
	       sc_core::sc_module_name name,                 // module name
	       const soclib::common::IntTab &index,          // index of mapping table
	       const soclib::common::MappingTable &mt,       // mapping table
	       sc_core::sc_time time_quantum,                // time quantum
	       sc_core::sc_time simulation_time);            // simulation time
  
}; 
#endif /* __MY_INITIATOR_H__ */
