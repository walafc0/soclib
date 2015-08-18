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
 * Maintainers: fpecheux, nipo, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Nicolas Pouillon <nipo@ssji.net>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef SOCLIB_TLMT_VCI_MWMR_CONTROLLER_H
#define SOCLIB_TLMT_VCI_MWMR_CONTROLLER_H

#include <tlmdt>                                      // TLM-DT headers
#include "mapping_table.h"
//#include "soclib_endian.h"
#include "mwmr_controller.h"

template<typename vci_param>
struct channel_struct{
  typename vci_param::addr_t status_address;
  typename vci_param::addr_t base_address;
  uint32_t                   depth;
  bool                       running;
};

template<typename vci_param>
struct fifos_struct{
  typename vci_param::data_t *data;
  bool                       empty;
  bool                       full;
  sc_core::sc_time           time;
  uint32_t                   n_elements;
};

template<typename vci_param>
struct request_struct{
  unsigned char             *data;
  bool                       pending;
  uint32_t                   n_elements;
  sc_core::sc_time           time;
};

namespace soclib { namespace tlmdt {

template<typename vci_param>
class VciMwmrController 
  : public sc_core::sc_module             // inherit from SC module base clase
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "backward interface"
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "forward interface"
{
 private:

  /////////////////////////////////////////////////////////////////////////////////////
  // Membre Variables
  /////////////////////////////////////////////////////////////////////////////////////
  soclib::common::MappingTable       m_mt;
  std::list<soclib::common::Segment> m_initiator_segments;
  std::list<soclib::common::Segment> m_target_segments;
  sc_core::sc_time                   m_simulation_time;

  pdes_local_time                   *m_pdes_local_time;
  pdes_activity_status              *m_pdes_activity_status;

  ////////////// hardware FIFOs ////////////////////////
  fifos_struct<vci_param>   *m_read_fifo;
  fifos_struct<vci_param>   *m_write_fifo;

  /////// memory mapped channels registers ////////////
  uint32_t                  m_channel_index;
  bool                      m_channel_read;
  channel_struct<vci_param> *m_read_channel;
  channel_struct<vci_param> *m_write_channel;

  ///////// registers arrays for coprocessor requests //////////////////
  request_struct<vci_param> *m_read_request;
  request_struct<vci_param> *m_write_request;

  ////////////////////// signals /////////////////////////////////
  sc_core::sc_event m_vci_event;
  sc_core::sc_event m_fifo_event;
  sc_core::sc_event m_active_event;
  sc_core::sc_event m_copro_event;

  FILE     * pFile;
  uint32_t m_srcid;
  uint32_t m_destid;
  uint32_t m_pktid;
  uint32_t m_read_fifo_depth;
  uint32_t m_write_fifo_depth;
  uint32_t m_read_channels;
  uint32_t m_write_channels;
  uint32_t m_config_registers;
  uint32_t m_status_registers;
  uint32_t m_waiting_time;
  uint32_t m_end_simulation_time;
  bool     m_reset_request; 

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

  //FIELDS OF A READ FIFO MESSAGE
  tlm::tlm_generic_payload *m_fifo_read_payload_ptr;
  tlm::tlm_phase            m_fifo_read_phase;
  sc_core::sc_time          m_fifo_read_time;

  //FIELDS OF A WRITE FIFO MESSAGE
  tlm::tlm_generic_payload *m_fifo_write_payload_ptr;
  tlm::tlm_phase            m_fifo_write_phase;
  sc_core::sc_time          m_fifo_write_time;

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuctions
  /////////////////////////////////////////////////////////////////////////////////////
  void update_time(sc_core::sc_time t);
  void update_time(uint64_t t);
  void send_activity();
  void reset();
  void execLoop();
  void getLock(typename vci_param::addr_t status_address, uint32_t *status);
  void releaseLock(typename vci_param::addr_t status_address, uint32_t *status);
  void readStatus(typename vci_param::addr_t status_address, uint32_t *status);
  void updateStatus(typename vci_param::addr_t status_address, uint32_t *status);
  void readFromChannel(uint32_t fifo_index, uint32_t *status);
  void writeToChannel(uint32_t fifo_index, uint32_t *status);
  void releasePendingReadFifo(uint32_t fifo_index);
  void releasePendingWriteFifo(uint32_t fifo_index);

  void send_write(
		  enum command command,
		  typename vci_param::addr_t address,
		  typename vci_param::data_t *data, 
		  size_t size
		  );
 
  void send_read(
		 enum command command,
		 typename vci_param::addr_t address,
		 typename vci_param::data_t *data, 
		 size_t size
		 );

  tlm::tlm_sync_enum vci_read_nb_transport_fw     // receive READ command from initiator
  ( size_t                    segIndex,           // segment index
    soclib::common::Segment  &s,                  // list of segments
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time
 
  tlm::tlm_sync_enum vci_write_nb_transport_fw    // receive WRITE command from initiator
  ( size_t                    segIndex,           // segment index
    soclib::common::Segment  &s,                  // list of segments
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if  (INITIATOR VCI SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw              // receive answer from target
  ( tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time
  
  // Not implemented for this example but required by interface
  void invalidate_direct_mem_ptr                  // invalidate_direct_mem_ptr
  ( sc_dt::uint64 start_range,                    // start range
    sc_dt::uint64 end_range);                     // end range
 
  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_fw_transport_if  (TARGET VCI SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_fw              // receive command from initiator
  ( tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time

  // Not implemented for this example but required by interface
  void b_transport                                // b_transport() - Blocking Transport
  ( tlm::tlm_generic_payload &payload,            // payload
    sc_core::sc_time         &time);              // time
  
  // Not implemented for this example but required by interface
  bool get_direct_mem_ptr
  ( tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_dmi             &dmi_data);          // DMI data
  
  // Not implemented for this example but required by interface
  unsigned int transport_dbg                            
  ( tlm::tlm_generic_payload &payload);           // payload

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_fw_transport_if (READ FIFO TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum read_fifo_nb_transport_fw    // receive data from initiator read fifo
  ( int                       id,                 // fifo id
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time
  

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_fw_transport_if (WRITE FIFO TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum write_fifo_nb_transport_fw   // receive data from initiator fifo
  ( int                       id,                 // fifo id
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time


  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (COPRO (STATUS AND CONFIG) INITIATOR SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum copro_nb_transport_bw        // receive answer from target
  ( int                       id,                 // config register id
    tlm::tlm_generic_payload &payload,            // payload
    tlm::tlm_phase           &phase,              // phase
    sc_core::sc_time         &time);              // time

 protected:
  SC_HAS_PROCESS(VciMwmrController);
 public:
  tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types> p_vci_initiator;  // VCI initiator port 
  tlm::tlm_target_socket<32,tlm::tlm_base_protocol_types>     p_vci_target;     // VCI target socket

  std::vector<tlm_utils::simple_initiator_socket_tagged<VciMwmrController,32,tlm::tlm_base_protocol_types> *> p_config;
  std::vector<tlm_utils::simple_initiator_socket_tagged<VciMwmrController,32,tlm::tlm_base_protocol_types> *> p_status;

  std::vector<tlm_utils::simple_target_socket_tagged<VciMwmrController,32,tlm::tlm_base_protocol_types> *> p_from_coproc;
  std::vector<tlm_utils::simple_target_socket_tagged<VciMwmrController,32,tlm::tlm_base_protocol_types> *> p_to_coproc;

  VciMwmrController(sc_core::sc_module_name name,
		    const soclib::common::MappingTable &mt,
		    const soclib::common::IntTab &initiator_index,
		    const soclib::common::IntTab &target_index,
		    uint32_t read_fifo_depth,  //in words
		    uint32_t write_fifo_depth, //in words
		    uint32_t n_read_channels,
		    uint32_t n_write_channels,
		    uint32_t n_config,
		    uint32_t n_status,
		    sc_core::sc_time simulation_time);
  
};

}}

#endif
