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

#include "vci_locks.h"

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciLocks<vci_param>

tmpl(/**/)::VciLocks
( sc_core::sc_module_name name,
  const soclib::common::IntTab &index,
  const soclib::common::MappingTable &mt)
	   : sc_module(name),
	   m_index(index),
	   m_mt(mt),
	   p_vci("vcisocket")
{
  // bind target
  p_vci(*this);                     
  
  // segments
  segList=m_mt.getSegmentList(m_index);
  size_t nbSeg=segList.size();
  std::list<soclib::common::Segment>::iterator seg;
  
  m_contents = new ram_t*[nbSeg];
  size_t word_size = 4;
  size_t i=0;
  
  for (i=0, seg = segList.begin(); seg != segList.end(); ++i, ++seg ) {
    soclib::common::Segment &s = *seg;
    m_contents[i] = new ram_t[(s.size()+word_size-1)/word_size];
  }
  
  for (i=0, seg = segList.begin(); seg != segList.end(); ++i, ++seg ) {
    soclib::common::Segment &s = *seg;
    for ( size_t addr = 0; addr < s.size()/4; ++addr )
      m_contents[i][addr] = false;
  }
}
 
tmpl(/**/)::~VciLocks(){}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if VCI SOCKET
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,  
  sc_core::sc_time         &time)   
{
  soclib_payload_extension *extension_pointer;
  payload.get_extension(extension_pointer);

  //this target does not treat the null message
  if(extension_pointer->is_null_message()){
    return tlm::TLM_COMPLETED;
  }

  std::list<soclib::common::Segment>::iterator seg;
  size_t segIndex;

  uint32_t nwords = (uint32_t)(payload.get_data_length() / vci_param::nbytes);

  for (segIndex=0,seg = segList.begin();seg != segList.end(); ++segIndex, ++seg ){
    soclib::common::Segment &s = *seg;
    if (!s.contains(payload.get_address()))
      continue;
    switch(extension_pointer->get_command()){
    case VCI_READ_COMMAND:
      {
	typename vci_param::addr_t address;
	
	for (size_t i=0;i<nwords;i++){

	  //if (pkt->contig)
	  address = (payload.get_address()+(i*vci_param::nbytes)) - s.baseAddress(); //XXX contig = TRUE always
	  //else
	  //address = payload.get_address() - s.baseAddress(); //always the same address
	  
	  utoa(m_contents[segIndex][address / vci_param::nbytes], payload.get_data_ptr(),(i * vci_param::nbytes));
	  m_contents[segIndex][address / vci_param::nbytes] = true;
	}
	
	payload.set_response_status(tlm::TLM_OK_RESPONSE);
	phase = tlm::BEGIN_RESP;
	time = time + (nwords * UNIT_TIME);
	
	p_vci->nb_transport_bw(payload, phase, time);
	return tlm::TLM_COMPLETED;
      }
      break;
    case VCI_WRITE_COMMAND:
      {
	typename vci_param::addr_t address;
	for (size_t i=0; i<nwords; i++){
	  //if(payload.contig)
	  address = (payload.get_address()+(i*vci_param::nbytes)) - s.baseAddress();//XXX contig = TRUE always
	  //else
	  //address = payload.get_address() - s.baseAddress();
	  
 	  uint32_t index   = address / vci_param::nbytes;
	  ram_t *tab       = m_contents[segIndex];
	  tab[index]       = false;
	}

	payload.set_response_status(tlm::TLM_OK_RESPONSE);
	phase = tlm::BEGIN_RESP;
	time = time + (nwords * UNIT_TIME);
 	
	p_vci->nb_transport_bw(payload, phase, time);
	return tlm::TLM_COMPLETED;
      }
      break;
    default:
      std::cout << "Command does not implemmented" << std::endl;
      break;
    }
  }
  //send error message
  payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
  
  phase = tlm::BEGIN_RESP;
  time = time + (nwords * UNIT_TIME);
  
#if VCI_LOCKS_DEBUG
  std::cout << "[" << name() << "] Address " << std::hex << payload.get_address() << std::dec << " does not match any segment " << std::endl;
  std::cout << "[" << name() << "] Send a error packet with time = "  << time.value() << std::endl;
#endif

  p_vci->nb_transport_bw(payload, phase, time);
  return tlm::TLM_COMPLETED;
}

/// Not implemented for this example but required by interface
tmpl(void)::b_transport
( tlm::tlm_generic_payload &payload,                // payload
  sc_core::sc_time         &_time)                  //time
{
  return;
}

/// Not implemented for this example but required by interface
tmpl(bool)::get_direct_mem_ptr
( tlm::tlm_generic_payload &payload,                // address + extensions
  tlm::tlm_dmi             &dmi_data)               // DMI data
{ 
  return false;
}
    
/// Not implemented for this example but required by interface
tmpl(unsigned int):: transport_dbg                            
( tlm::tlm_generic_payload &payload)                // debug payload
{
  return false;
}
   
}}
