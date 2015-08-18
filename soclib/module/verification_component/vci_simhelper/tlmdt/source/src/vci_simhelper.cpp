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
 * Maintainers: alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include "simhelper.h"
#include "../include/vci_simhelper.h"

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciSimhelper<vci_param>

//////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//////////////////////////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciSimhelper
( sc_core::sc_module_name name,
  const soclib::common::IntTab &index,
  const soclib::common::MappingTable &mt)
	   : sc_module(name)
	  , m_mt(mt)
	  , m_segments(m_mt.getSegmentList(index))
	  , p_vci("socket")
{
  // bind target
  p_vci(*this);                     

  // segments
  //  m_segments = m_mt.getSegmentList(index);

  //counters
  m_cpt_read = 0;
  m_cpt_write = 0;

}

tmpl(/**/)::~VciSimhelper(){}

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

  uint32_t nwords = (uint32_t)(payload.get_data_length() / vci_param::nbytes);
  uint32_t srcid  = extension_pointer->get_src_id();

  // First, find the right segment using the first address of the packet
  std::list<soclib::common::Segment>::iterator seg;
  size_t segIndex;
	
  for (segIndex=0, seg = m_segments.begin(); seg != m_segments.end(); ++segIndex, ++seg ) {
    soclib::common::Segment &s = *seg;
    if (!s.contains(payload.get_address()))
      continue;
    
    switch(extension_pointer->get_command()){
    case VCI_READ_COMMAND:
      payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
      phase = tlm::BEGIN_RESP;
      time = time + ((( 2 * nwords) - 1) * UNIT_TIME);
      
#ifdef SOCLIB_MODULE_DEBUG
      std::cout << "[" << name() << "] Read command is not disponible in Vci_Simhelper component" << std::endl;
#endif
      p_vci->nb_transport_bw(payload, phase, time);
      return tlm::TLM_COMPLETED;
      break;
    case VCI_WRITE_COMMAND:
      {
	m_cpt_write++;
	int cell = (int)(payload.get_address() - s.baseAddress()) / vci_param::nbytes;
	uint32_t data = atou( payload.get_data_ptr(), 0 );

	switch (cell) {
	case SIMHELPER_SC_STOP:
#ifdef SOCLIB_MODULE_DEBUG
	  std::cout << "[" << name() << "] Write in SIMHELPER_SC_STOP register Time = "  << time.value()  << std::endl;
#endif
	  sc_core::sc_stop();
	  break;
	case SIMHELPER_END_WITH_RETVAL:
#ifdef SOCLIB_MODULE_DEBUG
	    std::cout << "[" << name() << "] Write in SIMHELPER_END_WITH_RETVAL register Time = "  << time.value()  << std::endl;
#endif
	  std::cout << "Simulation exiting, retval=" << data << std::endl;
	  ::exit(data);
	case SIMHELPER_EXCEPT_WITH_VAL:
	  {
#ifdef SOCLIB_MODULE_DEBUG
	    std::cout << "[" << name() << "] Write in SIMHELPER_EXCEPT_WITH_VAL register Time = "  << time.value()  << std::endl;
#endif
	    std::ostringstream o;
	    o << "Simulation yielded error level " << data;
	    throw soclib::exception::RunTimeError(o.str());
	  }
	case SIMHELPER_PAUSE_SIM:
	  {
#ifdef SOCLIB_MODULE_DEBUG
	    std::cout << "[" << name() << "] Write in SIMHELPER_PAUSE_SIM register Time = "  << time.value()  << std::endl;
#endif
	    std::cout << "Simulation paused, press ENTER" << std::endl;
	    std::string a;
	    std::cin >> a;
	  }
	}
	
	payload.set_response_status(tlm::TLM_OK_RESPONSE);
	phase = tlm::BEGIN_RESP;
	time = time + UNIT_TIME;
	p_vci->nb_transport_bw(payload, phase, time);
	return tlm::TLM_COMPLETED;
      }
      break;
    default:
      break;
    }
  }
  
  //send error message
  payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
  
  phase = tlm::BEGIN_RESP;
  time = time + (nwords * UNIT_TIME);
  
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[" << name() << "] Address " << std::hex << payload.get_address() << std::dec << " does not match any segment " << std::endl;
#endif
  p_vci->nb_transport_bw(payload, phase, time);
  
  return tlm::TLM_COMPLETED;
}

// Not implemented for this example but required by interface
tmpl(void)::b_transport
( tlm::tlm_generic_payload &payload,                // payload
  sc_core::sc_time         &_time)                  //time
{
  return;
}

// Not implemented for this example but required by interface
tmpl(bool)::get_direct_mem_ptr
( tlm::tlm_generic_payload &payload,                // address + extensions
  tlm::tlm_dmi             &dmi_data)               // DMI data
{ 
  return false;
}
    
// Not implemented for this example but required by interface
tmpl(unsigned int):: transport_dbg                            
( tlm::tlm_generic_payload &payload)                // debug payload
{
  return false;
}

}}
