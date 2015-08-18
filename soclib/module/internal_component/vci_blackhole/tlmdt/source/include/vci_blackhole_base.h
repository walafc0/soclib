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

#ifndef SOCLIB_TLMDT_VCI_BLACKHOLE_BASE_H
#define SOCLIB_TLMDT_VCI_BLACKHOLE_BASE_H

#include <tlmdt>       			// TLM-DT headers

namespace soclib { namespace tlmdt {

class VciBlackholeBase
: public sc_core::sc_module                                             // inherit from SC module base clase
, virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "backward interface"
, virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "forward interface"
{
protected:

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_bw_transport_if (INITIATOR SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  // Not implemented for this example but required by interface
  tlm::tlm_sync_enum nb_transport_bw           // receive response from target
  ( tlm::tlm_generic_payload &payload,                // payload
    tlm::tlm_phase           &phase,                  // phase
    sc_core::sc_time         &time)                   // time
  {
    return tlm::TLM_COMPLETED;
  };

  // Not implemented for this example
  tlm::tlm_sync_enum nb_transport_bw           // receive response from target
  ( int                      id,                      // identificator socket
    tlm::tlm_generic_payload &payload,                // payload
    tlm::tlm_phase           &phase,                  // phase
    sc_core::sc_time         &time)                   // time
  {
    return tlm::TLM_COMPLETED;
  };

  // Not implemented for this example but required by interface
  void invalidate_direct_mem_ptr               // invalidate_direct_mem_ptr
  ( sc_dt::uint64 start_range,                        // start range
    sc_dt::uint64 end_range)                          // end range
  {
  };
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_fw_transport_if (TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  // Not implemented for this example but required by interface
  tlm::tlm_sync_enum nb_transport_fw          // receive command from initiator
  ( tlm::tlm_generic_payload &payload,               // payload
    tlm::tlm_phase           &phase,                 // phase
    sc_core::sc_time         &time)                  // time
  {
    return tlm::TLM_COMPLETED;
  };
  
  // Not implemented for this example
  tlm::tlm_sync_enum nb_transport_fw           // receive command from initiator
  ( int                      id,                      // identificator socket
    tlm::tlm_generic_payload &payload,                // payload
    tlm::tlm_phase           &phase,                  // phase
    sc_core::sc_time         &time)                   // time
  {
    return tlm::TLM_COMPLETED;
  };
  
  // Not implemented for this example but required by interface
  void b_transport                            // b_transport() - Blocking Transport
  ( tlm::tlm_generic_payload &payload,               // payload
    sc_core::sc_time         &time)                  // time
  {
  }

  // Not implemented for this example but required by interface
  bool get_direct_mem_ptr
  ( tlm::tlm_generic_payload &payload,               // payload
    tlm::tlm_dmi             &dmi_data)              // DMI data
  {
    return false;
  };
  
  // Not implemented for this example but required by interface
  unsigned int transport_dbg
  ( tlm::tlm_generic_payload &payload)               // payload
  {
    return 0;
  };
  
public:
  
  VciBlackholeBase
  ( sc_core::sc_module_name name)
    : sc_module(name)  // init module name
  {
  };
  
 
};

}}

#endif
