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

#ifndef __VCI_RSP_ARB_CMD_ROUT_H__ 
#define __VCI_RSP_ARB_CMD_ROUT_H__

#include <tlmdt>	                                        // TLM-DT headers
#include "mapping_table.h"                                      // mapping table
#include "centralized_buffer.h"                                 // centralized buffer

namespace soclib { namespace tlmdt {

class VciCmdArbRspRout;

class VciRspArbCmdRout                                           // VciRspArbCmdRout
  : public sc_core::sc_module           	                 // inherit from SC module base clase
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "forward interface"
{
private:
 
  typedef soclib::common::AddressDecodingTable<uint32_t, int>  routing_table_t;      // routing table
  typedef soclib::common::AddressDecodingTable<uint32_t, bool> locality_table_t;     // locality table

  /////////////////////////////////////////////////////////////////////////////////////
  // Member Variables
  /////////////////////////////////////////////////////////////////////////////////////
  uint32_t                                                   m_index;              // local index
  sc_core::sc_time                                           m_delay;              // interconnect delay
  centralized_buffer                                        *m_centralized_buffer; // centralized buffer
  const routing_table_t                                      m_routing_table;      // routing table
  const locality_table_t                                     m_locality_table;     // locality table
  std::vector<VciCmdArbRspRout *>                            m_CmdArbRspRout;      // cmd_arb_rsp_rout blocks
  bool                                                       m_is_local_crossbar;

  FILE * myFile;

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuction  tlm::tlm_fw_transport_if (VCI TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  void create_token();
 
  tlm::tlm_sync_enum nb_transport_fw                    
  ( tlm::tlm_generic_payload &payload,   // payload
    tlm::tlm_phase           &phase,     // phase
    sc_core::sc_time         &time);     // time
 
  // Not implemented for this example but required by interface
  void b_transport                       // b_transport() - Blocking Transport
  ( tlm::tlm_generic_payload &payload,   // payload
    sc_core::sc_time         &time);     // time
  
  // Not implemented for this example but required by interface
  bool get_direct_mem_ptr
  ( tlm::tlm_generic_payload &payload,   // payload
    tlm::tlm_dmi             &dmi_data); // DMI data
  
  // Not implemented for this example but required by interface
  unsigned int transport_dbg                            
  ( tlm::tlm_generic_payload &payload);  // payload

public:  
  
  tlm::tlm_target_socket<32,tlm::tlm_base_protocol_types> p_vci_target; // VCI TARGET port
  
  VciRspArbCmdRout(                                                // constructor
		   sc_core::sc_module_name module_name             // SC module name
		   , const routing_table_t &rt                     // routing table
		   , const locality_table_t &lt                    // locality table
		   , uint32_t local_index                          // local initiator index
		   , sc_core::sc_time delay                        // interconnect delay
		   , centralized_buffer *cb);                      // centralized buffer
  
  VciRspArbCmdRout(                                                // constructor
		   sc_core::sc_module_name module_name             // SC module name
		   , const routing_table_t &rt                     // routing table
		   , uint32_t local_index                          // local initiator index
		   , sc_core::sc_time delay                        // interconnect delay
		   , centralized_buffer *cb);                      // centralized buffer

  ~VciRspArbCmdRout();

  void setCmdArbRspRout(std::vector<VciCmdArbRspRout *> &CmdArbRspRout);

  void set_external_access(unsigned int index, bool b);

};

}}

#endif /* __VCI_RSP_ARB_CMD_ROUT_H__ */
