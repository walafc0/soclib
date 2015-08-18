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

#include <systemc>
#include "vci_initiator_transactor_buffer.h"

namespace soclib { namespace tlmdt {

vci_initiator_transactor_buffer::vci_initiator_transactor_buffer()
  : m_nentries(TAM_BUFFER)
  , m_header_ptr(0)
  , m_cmd_ptr(0)
  , m_rsp_ptr(0)
{
  init();
}

vci_initiator_transactor_buffer::vci_initiator_transactor_buffer(int n)
  : m_nentries(n)
  , m_header_ptr(0)
  , m_cmd_ptr(0)
  , m_rsp_ptr(0)
{
  init();
}

vci_initiator_transactor_buffer::~vci_initiator_transactor_buffer()
{
}

void vci_initiator_transactor_buffer::init()
{
  m_table = new transaction_buffer[m_nentries];
  for(int i=0; i<m_nentries; i++){
    m_table[i].status = EMPTY;
  }
}

bool vci_initiator_transactor_buffer::push
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,
  sc_core::sc_time         &time)
{
  int i = m_header_ptr;
  if(m_table[i].status == EMPTY){
    m_table[i].status  = OPEN;
    m_table[i].payload = &payload;
    m_table[i].phase   = &phase;
    m_table[i].time    = &time;

    m_header_ptr++;
    if(m_header_ptr == m_nentries)
      m_header_ptr = 0;
#if SOCLIB_MODULE_DEBUG
    printf("PUSH [%d] STATUS = %d time = %d\n",i, m_table[i].status, (int)(*m_table[i].time).value());
#endif
    return true;
  }
  return false;
}

bool vci_initiator_transactor_buffer::get_cmd_payload
( uint32_t                   local_time,
  tlm::tlm_generic_payload *&payload,
  tlm::tlm_phase           *&phase,
  sc_core::sc_time         *&time)
{
  int i = m_cmd_ptr;
  if(m_table[i].status == OPEN){
    payload = m_table[i].payload;
    phase = m_table[i].phase;
    time = m_table[i].time;

    if(((*time).value())<=local_time){
      m_table[i].status = COMPLETED;
    
      m_cmd_ptr++;
      if(m_cmd_ptr == m_nentries)
	m_cmd_ptr = 0;
    
#if SOCLIB_MODULE_DEBUG
      printf("SELECT [%d] time = %d local_time = %d\n", i, (int)(*m_table[i].time).value(), (int)local_time);
#endif
      return true;
    }
  }
  return false;
}

int vci_initiator_transactor_buffer::get_rsp_payload
( unsigned int               src_id,
  unsigned int               trd_id,
  tlm::tlm_generic_payload *&payload,
  tlm::tlm_phase           *&phase,
  sc_core::sc_time         *&time)
{
#if SOCLIB_MODULE_DEBUG
  printf("GET_RSP_PAYLOAD src_id = %d trd_id = %d\n", src_id, trd_id);
#endif
  soclib_payload_extension *ext;
  if(m_rsp_ptr < m_cmd_ptr){
    for(int i=m_rsp_ptr; i < m_cmd_ptr; i++){
      if(m_table[i].status == COMPLETED){
	
	m_table[i].payload->get_extension(ext);
	
	if(ext->get_src_id() == src_id &&
	   ext->get_trd_id() == trd_id){
	  payload = m_table[i].payload;
	  phase   = m_table[i].phase;
	  time    = m_table[i].time;
	  
	  if(i == m_rsp_ptr){
	    m_rsp_ptr++;
	    if(m_rsp_ptr == m_nentries) m_rsp_ptr = 0;
	  }
	  
	  return i;
	}
      }
    }
  }
  else{
    for(int i=m_rsp_ptr; i < m_nentries; i++){
      if(m_table[i].status == COMPLETED){
	
	m_table[i].payload->get_extension(ext);
	
	if(ext->get_src_id() == src_id &&
	   ext->get_trd_id() == trd_id){
	  payload = m_table[i].payload;
	  phase   = m_table[i].phase;
	  time    = m_table[i].time;
	  
	  if(i == m_rsp_ptr){
	    m_rsp_ptr++;
	    if(m_rsp_ptr == m_nentries) m_rsp_ptr = 0;
	  }
	  
	  return i;
	}
      }
    }
    for(int i=0; i < m_cmd_ptr; i++){
      if(m_table[i].status == COMPLETED){
	
	m_table[i].payload->get_extension(ext);
	
	if(ext->get_src_id() == src_id &&
	   ext->get_trd_id() == trd_id){
	  payload = m_table[i].payload;
	  phase   = m_table[i].phase;
	  time    = m_table[i].time;
	  
	  if(i == m_rsp_ptr){
	    m_rsp_ptr++;
	  }
	  
	  return i;
	}
      }
    }
  }
  return -1;
}


bool vci_initiator_transactor_buffer::pop(int idx)
{
  if(m_table[idx].status == COMPLETED){
    m_table[idx].status = EMPTY;
#if SOCLIB_MODULE_DEBUG
    printf("POP [%d] =  %d\n",idx,m_table[idx].status);
#endif
    return true;
  }
  return false;
}

bool vci_initiator_transactor_buffer::waiting_response(){
  if(m_rsp_ptr == m_cmd_ptr){
#if SOCLIB_MODULE_DEBUG
    printf("DO NOT WAITING RESPONSE\n");
#endif
    return false;
  }
#if SOCLIB_MODULE_DEBUG
  printf("WAITING RESPONSE\n");
#endif
  return true;
}
}}
