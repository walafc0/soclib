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
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

#ifndef SOCLIB_TLMDT_VCI_ICU_H
#define SOCLIB_TLMDT_VCI_ICU_H

#include <tlmdt>      
#include "mapping_table.h"
#include "icu.h"

namespace soclib { namespace tlmdt {

template <typename vci_param>
class VciIcu
  : public sc_core::sc_module             
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> 
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> 
{
private:
  
    typedef typename vci_param::addr_t addr_t;

    /////////////////////////////////////////////////////////////////////////////////////
    // Member Variables
    /////////////////////////////////////////////////////////////////////////////////////

    bool				m_irq_in_enabled[32];
    unsigned char			m_irq_in_value[32];
    sc_core::sc_time			m_irq_in_time[32];
    soclib::common::IntTab             	m_tgtid;
    soclib::common::Segment  		m_segment;
    unsigned char			m_irq_out_value;

    sc_core::sc_event			m_irq_received;
    pdes_local_time*			m_pdes_local_time;

    size_t                             	m_nirq;

    // FIELDS OF AN IRQ TRANSACTION
    tlm::tlm_generic_payload        	m_irq_out_payload;
    tlm::tlm_phase                  	m_irq_out_phase;
    sc_core::sc_time                   	m_irq_out_time;

    /////////////////////////////////////////////////////////////////////////////////////
    //  Functions
    /////////////////////////////////////////////////////////////////////////////////////
    bool irqTransmissible(unsigned char* value, sc_core::sc_time* new_time);
    uint32_t getActiveIrqs();
    size_t getIrqIndex();
    uint32_t getMask();
    void setMask(uint32_t data);
    void clearMask(uint32_t data);
    void execLoop();

    /////////////////////////////////////////////////////////////////////////////////////
    // Interface function executed when receiving a VCI command       @
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_fw    
    ( tlm::tlm_generic_payload &payload, 
      tlm::tlm_phase           &phase, 
      sc_core::sc_time         &time);  

  // Not implemented for this example but required by interface
  void b_transport                          // b_transport() - Blocking Transport
  ( tlm::tlm_generic_payload &payload,      // payload
    sc_core::sc_time         &time);        // time
  
  // Not implemented for this example but required by interface
  bool get_direct_mem_ptr
  ( tlm::tlm_generic_payload &payload,      // payload
    tlm::tlm_dmi             &dmi_data);    // DMI data
  
  // Not implemented for this example but required by interface
  unsigned int transport_dbg                            
  ( tlm::tlm_generic_payload &payload);     // payload

    /////////////////////////////////////////////////////////////////////////////////////
    // Interface function executed when receiving a response on the IRQ port (unused)
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_bw        // receive resp messages from target
  ( tlm::tlm_generic_payload &payload,      // payload
    tlm::tlm_phase           &phase,        // phase
    sc_core::sc_time         &time);        // time
  
  // Not implemented for this example but required by interface
  void invalidate_direct_mem_ptr            // invalidate_direct_mem_ptr
  ( sc_dt::uint64 start_range,              // start range
    sc_dt::uint64 end_range);               // end range

    /////////////////////////////////////////////////////////////////////////////////////
    // Interface function executed when receiving and IRQ on port with index id
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum irq_nb_transport_fw  
    ( int                         id,       
    tlm::tlm_generic_payload   &payload,   
    tlm::tlm_phase             &phase,    
    sc_core::sc_time           &time);  

protected:
    SC_HAS_PROCESS(VciIcu);

public:
    tlm::tlm_target_socket<32, tlm::tlm_base_protocol_types> 					  p_vci;   
    tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types> 				  p_irq; 
    std::vector<tlm_utils::simple_target_socket_tagged<VciIcu,32,tlm::tlm_base_protocol_types> *> p_irq_in; 

    VciIcu(
	 sc_core::sc_module_name name,
	 const soclib::common::IntTab &index,
	 const soclib::common::MappingTable &mt,
	 size_t nirq);
  
    ~VciIcu();
};
}}

#endif /* SOCLIB_TLMDT_VCI_ICU_H */
