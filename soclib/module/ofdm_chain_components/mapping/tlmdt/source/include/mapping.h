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
 * Modified by : Caaliph Andriamisaina <andriami@univ-ubs.fr>
 */

#ifndef SOCLIB_TLMT_MAPPING_H
#define SOCLIB_TLMT_MAPPING_H

#include <tlmdt>                            // TLM-DT headers

class Mapping
: public sc_core::sc_module
{
 private:
  typedef soclib::tlmdt::VciParams<uint32_t,uint32_t> vci_param;

  uint32_t m_id;
  uint32_t m_read_fifo_depth;
  uint32_t m_write_fifo_depth;
  uint32_t m_read_channels;
  uint32_t m_write_channels;
  uint32_t m_config_registers;
  uint32_t m_status_registers;
  
  uint32_t *m_config_register;
  uint32_t *m_status_register;

  pdes_local_time        *m_pdes_local_time;
  pdes_activity_status   *m_pdes_activity_status;

  sc_core::sc_event m_rsp_write;
  sc_core::sc_event m_rsp_read;
  sc_core::sc_event m_active_event;

  // Functions
  void behavior();
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (READ FIFO INITIATOR SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum readResponseReceived         // Receive rsp from target
  ( int                       id,                 // fifo id
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time
 
  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (WRITE FIFO INITIATOR SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum writeResponseReceived        // Receive rsp from target
  ( int                       id,                 // fifo id
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time
 
  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (CONFIG FIFO TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum writeConfigReceived          // Receive command from initiator socket
  ( int                       id,                 // fifo id
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (STATUS FIFO TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum readStatusReceived           // Receive command from initiator socket
  ( int                       id,                 // fifo id
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time

 protected:
  SC_HAS_PROCESS(Mapping);
 public:
  std::vector<tlm_utils::simple_target_socket_tagged<Mapping,32,tlm::tlm_base_protocol_types> *> p_config;
  std::vector<tlm_utils::simple_target_socket_tagged<Mapping,32,tlm::tlm_base_protocol_types> *> p_status;

  std::vector<tlm_utils::simple_initiator_socket_tagged<Mapping,32,tlm::tlm_base_protocol_types> *> p_read_fifo;
  std::vector<tlm_utils::simple_initiator_socket_tagged<Mapping,32,tlm::tlm_base_protocol_types> *> p_write_fifo;

  Mapping(sc_core::sc_module_name name,
	      uint32_t id,
	      uint32_t read_fifo_depth,
	      uint32_t write_fifo_depth,
	      uint32_t n_read_channels,
	      uint32_t n_write_channels,
	      uint32_t n_config,
	      uint32_t n_status);


};

#endif
