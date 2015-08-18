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
 * Maintainers: alinev
 *
 * Copyright (c) UPMC / Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include "../include/tlmdt_target.h"

namespace soclib { namespace tlmdt {

//////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//////////////////////////////////////////////////////////////////////////////////////////
TlmdtTarget::TlmdtTarget(sc_core::sc_module_name module_name)
	   : sc_module(module_name)
	   , m_buffer()
	   , p_vci("socket")
{
  // bind target
  p_vci(*this);                     

  //PDES local time
  m_pdes_local_time = new pdes_local_time(sc_core::SC_ZERO_TIME);

  m_buffer.set_name(((std::string)name()).append("_buf"));

  // register thread process
  SC_THREAD(behavior);                  
}

TlmdtTarget::~TlmdtTarget(){}

/////////////////////////////////////////////////////////////////////////////////////
// THREAD
/////////////////////////////////////////////////////////////////////////////////////
void TlmdtTarget::behavior(void)   // initiator thread
{
  tlm::tlm_generic_payload *payload;
  tlm::tlm_phase           *phase;
  sc_core::sc_time         *time;

  while (true){
    while(m_buffer.pop(payload, phase, time)){
#if SOCLIB_MODULE_DEBUG
      printf("[%s] pop count = %d\n", name(), m_buffer.get_count());
#endif
      m_pop_event.notify(sc_core::SC_ZERO_TIME);
      target_processing(*payload, *phase, *time);
    }
    sc_core::wait(m_push_event);
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if VCI SOCKET
/////////////////////////////////////////////////////////////////////////////////////
tlm::tlm_sync_enum TlmdtTarget::nb_transport_fw
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,  
  sc_core::sc_time         &time)   
{
  bool push = false;
  int try_push = 0;
  do{
    push = m_buffer.push(payload, phase, time);

    //assert(push && "NOT PUSH");
    if(!push){
      try_push++;
#if SOCLIB_MODULE_DEBUG
      printf("[%s] <<<<<<<<< NOT PUSH >>>>>>>> try_push = %d \n", name(), try_push);
#endif
      sc_core::wait(m_pop_event);
      //sc_core::wait(sc_core::SC_ZERO_TIME);
    }
  }while(!push);
#if SOCLIB_MODULE_DEBUG
  printf("[%s] push count = %d\n", name(), m_buffer.get_count());
#endif
  m_push_event.notify(sc_core::SC_ZERO_TIME);
  return tlm::TLM_COMPLETED;
}


// Not implemented for this example but required by interface
void TlmdtTarget::b_transport
( tlm::tlm_generic_payload &payload,                // payload
  sc_core::sc_time         &_time)                  //time
{
  return;
}

// Not implemented for this example but required by interface
bool TlmdtTarget::get_direct_mem_ptr
( tlm::tlm_generic_payload &payload,                // address + extensions
  tlm::tlm_dmi             &dmi_data)               // DMI data
{ 
  return false;
}
    
// Not implemented for this example but required by interface
unsigned int TlmdtTarget:: transport_dbg                            
( tlm::tlm_generic_payload &payload)                // debug payload
{
  return false;
}

}}
