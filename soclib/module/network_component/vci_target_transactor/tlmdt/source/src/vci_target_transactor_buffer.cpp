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

#include <systemc>
#include "vci_target_transactor_buffer.h"

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param_caba,typename vci_param_tlmdt> x vci_target_transactor_buffer<vci_param_caba,vci_param_tlmdt>

tmpl(/**/)::vci_target_transactor_buffer()
	   : m_nentries(TAM_BUFFER)
	  , m_header_ptr(0)
	  , m_rsp_ptr(0)
{
  init();
}

tmpl(/**/)::vci_target_transactor_buffer(int n)
	   : m_nentries(n)
	  , m_header_ptr(0)
	  , m_rsp_ptr(0)
{
  init();
}

tmpl(/**/)::~vci_target_transactor_buffer()
{
}

tmpl(void)::init()
{
  m_buffer = new transaction_index_struct[m_nentries];
  for(int i=0; i<m_nentries; i++){
    m_buffer[i].status = EMPTY;
    //create payload and extension of a transaction
    m_buffer[i].payload = new tlm::tlm_generic_payload();
    unsigned char *data = new unsigned char[MAX_SIZE_PACKET];
    m_buffer[i].payload->set_data_ptr(data);
    unsigned char *byte_enable = new unsigned char[MAX_SIZE_PACKET];
    m_buffer[i].payload->set_byte_enable_ptr(byte_enable);
    soclib_payload_extension *extension = new soclib_payload_extension();
    m_buffer[i].payload->set_extension(extension);
    m_buffer[i].phase = tlm::UNINITIALIZED_PHASE;
    m_buffer[i].time = sc_core::SC_ZERO_TIME;
    m_buffer[i].initial_time = sc_core::SC_ZERO_TIME;
  }
}

tmpl(void)::set_status(int idx, int status){
  m_buffer[idx].status = status;
}

tmpl(void)::set_command(int idx, tlm::tlm_command cmd){
  m_buffer[idx].payload->set_command(cmd);
}

tmpl(void)::set_address(int idx, sc_dt::uint64 add){
  m_buffer[idx].payload->set_address(add);
}

tmpl(void)::set_ext_command(int idx, enum command cmd){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  extension->set_command(cmd);
}

tmpl(void)::set_src_id(int idx, unsigned int src){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  extension->set_src_id(src);
}

tmpl(void)::set_trd_id(int idx, unsigned int trd){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  extension->set_trd_id(trd);
}

tmpl(void)::set_pkt_id(int idx, unsigned int pkt){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  extension->set_pkt_id(pkt);
}

tmpl(void)::set_data(int idx, int idx_data, typename vci_param_caba::data_t int_data){
  unsigned char *data = m_buffer[idx].payload->get_data_ptr();
  utoa(int_data, data, idx_data * vci_param_tlmdt::nbytes);
}

tmpl(void)::set_data_length(int idx, unsigned int length){
  m_buffer[idx].payload->set_data_length(length);
}

tmpl(void)::set_byte_enable(int idx, int idx_be, typename vci_param_caba::data_t int_be){
  unsigned char *be = m_buffer[idx].payload->get_byte_enable_ptr();
  utoa(int_be, be, idx_be * vci_param_tlmdt::nbytes);
}

tmpl(void)::set_byte_enable_length(int idx, unsigned int length){
  m_buffer[idx].payload->set_byte_enable_length(length);
}

tmpl(void)::set_phase(int idx, tlm::tlm_phase phase){
  m_buffer[idx].phase = phase;
}

tmpl(void)::set_time(int idx, sc_core::sc_time time){
  m_buffer[idx].time = time;
}

tmpl(void)::set_response(tlm::tlm_generic_payload &payload, sc_core::sc_time &time){
  int idx;
  soclib_payload_extension *extension;
  payload.get_extension(extension);

  idx = get_index(extension->get_src_id(), extension->get_trd_id());

  //set response to buffer
  m_buffer[idx].status = COMPLETED;
  m_buffer[idx].time = time;
  
  //set the initial time, i.e. time of the first word transmission
  if(is_write_command(idx)){
    m_buffer[idx].initial_time = m_buffer[idx].time - UNIT_TIME;
  }
  else{
    m_buffer[idx].initial_time = m_buffer[idx].time - (get_nwords(idx) * UNIT_TIME);
  }

#if SOCLIB_MODULE_DEBUG
  printf("SET RESPONSE idx = %d time = %d initial_time = %d \n", idx, (int)m_buffer[idx].time.value(), (int)m_buffer[idx].initial_time.value());
#endif

}

tmpl(unsigned int)::get_src_id(int idx){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  return extension->get_src_id();
}

tmpl(unsigned int)::get_pkt_id(int idx){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  return extension->get_pkt_id();
}

tmpl(unsigned int)::get_trd_id(int idx){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  return extension->get_trd_id();
}  

tmpl(enum command)::get_ext_command(int idx){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  return extension->get_command();
}

tmpl(bool)::is_write_command(int idx){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  return extension->is_write();
}

tmpl(bool)::is_read_command(int idx){
  soclib_payload_extension *extension;
  m_buffer[idx].payload->get_extension(extension);
  return extension->is_read();
}

tmpl(bool)::get_response_status(int idx){
  if(m_buffer[idx].payload->get_response_status()==tlm::TLM_OK_RESPONSE)
    return false;
  return true;
}

tmpl(sc_dt::uint64)::get_address(int idx){
  return m_buffer[idx].payload->get_address();
}

tmpl(typename vci_param_caba::data_t)::get_data(int idx, int idx_data){
  return atou(m_buffer[idx].payload->get_data_ptr(), idx_data*vci_param_tlmdt::nbytes);
}

tmpl(unsigned int)::get_data_length(int idx){
  return m_buffer[idx].payload->get_data_length();
}

tmpl(unsigned int)::get_nwords(int idx){
  return (m_buffer[idx].payload->get_data_length()/vci_param_tlmdt::nbytes);
}

tmpl(tlm::tlm_generic_payload*)::get_payload(int idx){
  return m_buffer[idx].payload;
}

tmpl(tlm::tlm_phase*)::get_phase(int idx){
  return &m_buffer[idx].phase;
}

tmpl(sc_core::sc_time*)::get_time(int idx){
  return &m_buffer[idx].time;
}

tmpl(unsigned int)::get_time_value(int idx){
  return m_buffer[idx].time.value();
}

tmpl(unsigned int)::get_initial_time_value(int idx){
  return m_buffer[idx].initial_time.value();
}

tmpl(unsigned int)::select_response(int clock_count){
  int i = 0;
  int idx = -1;
  uint64_t min_time = MAX_TIME;
  //clock_count--;

  if(m_rsp_ptr == m_header_ptr)
    return idx;

#if SOCLIB_MODULE_DEBUG
  printf("SELECT RESPONSE m_rsp_ptr = %d m_header_ptr = %d clock_count = %d\n", m_rsp_ptr, m_header_ptr, clock_count);
#endif

  if(m_rsp_ptr < m_header_ptr){
    for(i=m_rsp_ptr; i<m_header_ptr; i++){
      if(m_buffer[i].status == COMPLETED){
	if((int) m_buffer[i].initial_time.value() <= clock_count && m_buffer[i].initial_time.value() < min_time){
#if SOCLIB_MODULE_DEBUG
	  printf("SELECT RESPONSE i = %d COMPLETED clock_count = %d initial_time = %d time = %d\n", i, clock_count, (int)m_buffer[i].initial_time.value(), (int)m_buffer[i].time.value());
#endif
	  idx = i;
	  min_time = m_buffer[i].initial_time.value();
	}
      }
    }
  }
  else{
    for(i=m_rsp_ptr; i<m_nentries; i++){
      if(m_buffer[i].status == COMPLETED){
	if((int)m_buffer[i].initial_time.value() <= clock_count && m_buffer[i].initial_time.value() < min_time){
#if SOCLIB_MODULE_DEBUG
	  printf("SELECT RESPONSE i = %d COMPLETED clock_count = %d initial_time = %d time = %d \n", i, clock_count, (int)m_buffer[i].initial_time.value(),(int)m_buffer[i].time.value());
#endif
	  idx = i;
	  min_time = m_buffer[i].initial_time.value();
	}
      }
    }
    for(i=0; i<m_header_ptr; i++){
      if(m_buffer[i].status == COMPLETED){
	if((int)m_buffer[i].initial_time.value() <= clock_count && m_buffer[i].initial_time.value() < min_time){
#if SOCLIB_MODULE_DEBUG
	  printf("SELECT RESPONSE i = %d COMPLETED clock_count = %d initial_time = %d time = %d \n", i, clock_count, (int)m_buffer[i].initial_time.value(),(int)m_buffer[i].time.value());
#endif
	  idx = i;
	  min_time = m_buffer[i].initial_time.value();
	}
      }
    }
  }
  
  return idx;
}

tmpl(int)::get_empty_position()
{
  int i = m_header_ptr;
  if(m_buffer[i].status == EMPTY){
    m_buffer[i].status  = OPEN;

    m_header_ptr++;
    if(m_header_ptr == m_nentries)
      m_header_ptr = 0;

#if SOCLIB_MODULE_DEBUG
    printf("GET EMPTY POSITION = %d\n",i);
#endif
    return i;
  }
  printf("PROBLEM WITH BUFFER SIZE\n");
  return -1;
}

tmpl(int)::get_index( unsigned int src_id, unsigned int trd_id)
{
#if SOCLIB_MODULE_DEBUG
  printf("GET_INDEX src_id = %d trd_id = %d\n", src_id, trd_id);
#endif
  soclib_payload_extension *ext;
  int i;
  if(m_rsp_ptr < m_header_ptr){
    for(i=m_rsp_ptr; i < m_header_ptr; i++){
      if(m_buffer[i].status == OPEN){
	
	m_buffer[i].payload->get_extension(ext);
	
	if(ext->get_src_id() == src_id &&
	   ext->get_trd_id() == trd_id){
	  return i;
	}
      }
    }
  }
  else{
    for(i=m_rsp_ptr; i < m_nentries; i++){
      if(m_buffer[i].status == OPEN){
	
	m_buffer[i].payload->get_extension(ext);
	
	if(ext->get_src_id() == src_id &&
	   ext->get_trd_id() == trd_id){
	  return i;
	}
      }
    }
    for(i=0; i < m_header_ptr; i++){
      if(m_buffer[i].status == OPEN){
	
	m_buffer[i].payload->get_extension(ext);
	
	if(ext->get_src_id() == src_id &&
	   ext->get_trd_id() == trd_id){
	  return i;
	}
      }
    }
  }
  return -1;
}


tmpl(bool)::set_empty_position(int idx)
{
  if(m_buffer[idx].status == COMPLETED){
    m_buffer[idx].status = EMPTY;

    if(idx == m_rsp_ptr){
	while(m_rsp_ptr > m_header_ptr && m_rsp_ptr < m_nentries && m_buffer[m_rsp_ptr].status == EMPTY){
	  m_rsp_ptr++;
	}

	if(m_rsp_ptr == m_nentries)
	  m_rsp_ptr=0;

 	while(m_rsp_ptr < m_header_ptr && m_buffer[m_rsp_ptr].status == EMPTY){
	  m_rsp_ptr++;
	}
      }


#if SOCLIB_MODULE_DEBUG
    printf("POP [%d] =  %d\n",idx, m_buffer[idx].status);
#endif
    return true;
  }
  return false;
}
}}
