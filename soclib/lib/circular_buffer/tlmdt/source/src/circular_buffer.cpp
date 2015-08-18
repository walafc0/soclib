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
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include "circular_buffer.h"

namespace soclib { namespace tlmdt {

circular_buffer::circular_buffer()
  : front(0)
  , rear(0)
  , buffer_length(0)
  , name("buffer")
{
  buffer = new transaction[MAX_SIZE];
  max_size = MAX_SIZE;
}

circular_buffer::circular_buffer(int n)
  : front(0)
  , rear(0)
  , buffer_length(0)
  , name("buffer")
{
  buffer = new transaction[n];
  max_size = n;
}

circular_buffer::~circular_buffer()
{
   //delete buffer;
}

bool circular_buffer::push
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,
  sc_core::sc_time         &time)
{
  if(is_full()){
    return false;
  }
  
#if SOCLIB_MODULE_DEBUG
  soclib_payload_extension *ext;
  payload.get_extension(ext);
  std::cout << "[" << name << "] PUSH src_id = " << ext->get_src_id() << " pkt_id = " << ext->get_pkt_id() << std::endl;
#endif
  

  buffer[rear].payload = &payload;
  buffer[rear].phase   = &phase;
  buffer[rear].time    = &time;
  
  rear = rear < max_size-1 ? rear + 1 : 0;


  ATOMIC_ADD(&buffer_length, 1);
  
  assert( buffer_length <= max_size );

#if SOCLIB_MODULE_DEBUG
  std::cout << "[" << name << "] INCREMENT QUEUE buffer_length = " << buffer_length << " front = " << front << " rear = " << rear << std::endl;
#endif
    
  return true;
}

bool circular_buffer::get_front
( tlm::tlm_generic_payload *&payload,
  tlm::tlm_phase           *&phase,
  sc_core::sc_time         *&time)
{
  if(!is_empty()){
    payload = buffer[front].payload;
    phase = buffer[front].phase;
    time = buffer[front].time;

    return true;
  }
  return false;
}
    
bool circular_buffer::pop
( tlm::tlm_generic_payload *&payload,
  tlm::tlm_phase           *&phase,
  sc_core::sc_time         *&time)
{
  if(!is_empty()){

    payload = buffer[front].payload;
    phase = buffer[front].phase;
    time = buffer[front].time;
   
    front = (front < max_size-1) ? front + 1 : 0;

    ATOMIC_SUB(&buffer_length,1);
    
    assert(buffer_length >= 0);

#if SOCLIB_MODULE_DEBUG
    soclib_payload_extension *ext;
    payload->get_extension(ext);
    std::cout << "[" << name << "] POP  src_id = " << ext->get_src_id() << " pkt_id = " << ext->get_pkt_id() << std::endl;
#endif



#if SOCLIB_MODULE_DEBUG
    std::cout << "[" << name << "] INCREMENT HEADER buffer_length = " << buffer_length << " front = " << front << " rear = " << rear << std::endl;
#endif

    return true;
  }
  return false;
}
   



void circular_buffer::set_name(std::string n)
{
  name = n;
}

}}
