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
 * Maintainers: fpecheux, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef VCI_LOCAL_CROSSBAR_H
#define VCI_LOCAL_CROSSBAR_H

#include <tlmdt>                         // TLM-DT headers
#include "interconnect.h"

namespace soclib { namespace tlmdt {

class VciLocalCrossbar
  : public sc_core::sc_module // inherit from SC module base clase
{
private:
  Interconnect *m_interconnect;

  tlm_utils::simple_initiator_socket<VciLocalCrossbar,32,tlm::tlm_base_protocol_types> p_vci_initiator_to_down; // VCI initiator port connected to local crossbar

  tlm_utils::simple_target_socket<VciLocalCrossbar,32,tlm::tlm_base_protocol_types> p_vci_target_to_down; // VCI target port connected to local crossbar

  std::vector<tlm_utils::simple_target_socket_tagged<VciLocalCrossbar,32,tlm::tlm_base_protocol_types> *> p_vci_targets; // VCI target ports
  
  std::vector<tlm_utils::simple_initiator_socket_tagged<VciLocalCrossbar,32,tlm::tlm_base_protocol_types> *> p_vci_initiators; // VCI initiator ports


  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_fw_transport_if (VCI TARGET UP)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_fw_up   // receive command from initiator
  ( tlm::tlm_generic_payload &payload,    // payload
    tlm::tlm_phase           &phase,      // phase
    sc_core::sc_time         &time);      // time

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_fw_transport_if (VCI TARGET DOWN)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_fw_down // receive command from initiator
  ( tlm::tlm_generic_payload &payload,    // payload
    tlm::tlm_phase           &phase,      // phase
    sc_core::sc_time         &time);      // time
 
  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_bw_transport_if (VCI INITIATOR UP)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw_up   // receive command from initiator
  ( tlm::tlm_generic_payload &payload,    // payload
    tlm::tlm_phase           &phase,      // phase
    sc_core::sc_time         &time);      // time

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_bw_transport_if (VCI INITIATOR DOWN)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw_down // receive command from initiator
  ( tlm::tlm_generic_payload &payload,    // payload
    tlm::tlm_phase           &phase,      // phase
    sc_core::sc_time         &time);      // time

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_fw_transport_if (VCI TARGET PORTS UP)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_fw_up   // receive command from initiator
  ( int                       id,         // register id
    tlm::tlm_generic_payload &payload,    // payload
    tlm::tlm_phase           &phase,      // phase
    sc_core::sc_time         &time);      // time

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_fw_transport_if (VCI TARGET PORTS DOWN)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_fw_down // receive command from initiator
  ( int                       id,         // register id
    tlm::tlm_generic_payload &payload,    // payload
    tlm::tlm_phase           &phase,      // phase
    sc_core::sc_time         &time);      // time
 
  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_bw_transport_if (VCI INITIATOR PORTS UP)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw_up   // receive command from initiator
  ( int                       id,         // register id
    tlm::tlm_generic_payload &payload,    // payload
    tlm::tlm_phase           &phase,      // phase
    sc_core::sc_time         &time);      // time

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_bw_transport_if (VCI INITIATOR PORTS DOWN)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw_down // receive command from initiator
  ( int                       id,         // register id
    tlm::tlm_generic_payload &payload,    // payload
    tlm::tlm_phase           &phase,      // phase
    sc_core::sc_time         &time);      // time

  void init(size_t nb_init,
	    size_t nb_target );

public:

  tlm_utils::simple_initiator_socket<VciLocalCrossbar,32,tlm::tlm_base_protocol_types> p_initiator_to_up; // VCI initiator port connected to micro-network

  tlm_utils::simple_target_socket<VciLocalCrossbar,32,tlm::tlm_base_protocol_types> p_target_to_up; // VCI target port connected to micro-network

  std::vector<tlm_utils::simple_target_socket_tagged<VciLocalCrossbar,32,tlm::tlm_base_protocol_types> *> p_to_initiator; // VCI target ports
  
  std::vector<tlm_utils::simple_initiator_socket_tagged<VciLocalCrossbar,32,tlm::tlm_base_protocol_types> *> p_to_target; // VCI initiator ports

  VciLocalCrossbar(                                                     //constructor
		   sc_core::sc_module_name            name,             //module name
		   const soclib::common::MappingTable &mt,              //mapping table
		   const soclib::common::IntTab       &index,           //index of mapping table
		   int nb_init,                                         //number of initiators connect to interconnect
		   int nb_target,                                       //number of targets connect to interconnect
		   sc_core::sc_time delay);                             //interconnect delay


  VciLocalCrossbar(                                                     //constructor
		   sc_core::sc_module_name            name,             //module name
		   const soclib::common::MappingTable &mt,              //mapping table
		   const soclib::common::IntTab       &index,           //index of mapping table
		   size_t nb_init,                                      //number of initiators connect to interconnect
		   size_t nb_target);                                   //number of targets connect to interconnect


  VciLocalCrossbar(                                                     //constructor
		   sc_core::sc_module_name            name,             //module name
		   const soclib::common::MappingTable &mt,              //mapping table
		   const soclib::common::IntTab       &init_index,      //initiator index of mapping table
		   const soclib::common::IntTab       &target_index,    //target index of mapping table (do not used)
		   size_t nb_init,                                      //number of initiators connect to interconnect
		   size_t nb_target);                                   //number of targets connect to interconnect

  void print();
};

}}
#endif
