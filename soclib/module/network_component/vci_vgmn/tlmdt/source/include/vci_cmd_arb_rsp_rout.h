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
 *     Francois Pecheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef __VCI_CMD_ARB_RSP_ROUT_H__
#define __VCI_CMD_ARB_RSP_ROUT_H__

#include <tlmdt>			          // TLM-DT headers
#include "vci_rsp_arb_cmd_rout.h"                 // Our header

namespace soclib { namespace tlmdt {

class VciRspArbCmdRout;

class VciCmdArbRspRout                        // 
  : public sc_core::sc_module                 // inherit from SC module base clase
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "backward interface"
{
private:
  
  struct packet_struct{
    tlm::tlm_generic_payload *payload;
    tlm::tlm_phase           *phase;
    sc_core::sc_time         *time;
  } ;

  typedef soclib::common::AddressMaskingTable<uint32_t>        routing_table_t;  // routing table
  typedef soclib::common::AddressDecodingTable<uint32_t, bool> locality_table_t; // locality table

  /////////////////////////////////////////////////////////////////////////////////////
  // Member Variables
  /////////////////////////////////////////////////////////////////////////////////////
  std::vector <VciRspArbCmdRout *>                           m_RspArbCmdRout;     // vector of rsp_arb_cmd_rout
  const routing_table_t                                      m_routing_table;     // routing table
  const locality_table_t                                     m_locality_table;    // locality table
  std::list<packet_struct>                                   packet_fifo;         // fifo
  sc_core::sc_event                                          m_fifo_event;        // fifo event
 
  sc_core::sc_time                                           m_delay;             // interconnect delay
  pdes_local_time                                           *m_pdes_local_time;   // local time
  bool                                                       m_external_access;   // true if module has external access (crossbar parameter)
  bool                                                       m_is_local_crossbar; // true if module is vci_local_crossbar

  soclib_payload_extension                                  *m_extension_pointer; // payload extension

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuctions
  /////////////////////////////////////////////////////////////////////////////////////
  void behavior (void);                                              // initiator thread
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (VCI INITIATOR SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw
  ( tlm::tlm_generic_payload &payload,    // transaction payload
    tlm::tlm_phase           &phase,      // transaction phase
    sc_core::sc_time         &time);      // transaction time
  
  // Not implemented for this example but required by interface
  void invalidate_direct_mem_ptr          // invalidate_direct_mem_ptr
  ( sc_dt::uint64 start_range,            // start range
    sc_dt::uint64 end_range);             // end range

protected:
  SC_HAS_PROCESS(VciCmdArbRspRout);
  
public:
  tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types> p_vci_initiator;  // VCI initiator port 

  VciCmdArbRspRout(                                                  // constructor
		   sc_core::sc_module_name name                      // module name
		   , const routing_table_t &rt                       // routing table
		   , const locality_table_t &lt                      // locality table
		   , sc_core::sc_time delay                          // interconnect delay
		   , bool external_access);                          // true if module has external access (crossbar parameter)
  
  VciCmdArbRspRout(                                                  // constructor
		   sc_core::sc_module_name name                      // module name
		   , const routing_table_t &rt                       // routing table
		   , sc_core::sc_time delay);                        // interconnect delay

  void setRspArbCmdRout(std::vector<VciRspArbCmdRout *> &RspArbCmdRout);

  VciRspArbCmdRout* getRspArbCmdRout(unsigned int index);            

  void put
  ( tlm::tlm_generic_payload &payload,    // transaction payload
    tlm::tlm_phase           &phase,      // transaction phase
    sc_core::sc_time         &time);      // transaction time

}; 
}}
#endif /* __VCI_CMD_ARB_RSP_ROUT_H__ */
