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


#include "vci_initiator_transactor.h"

template<typename T>
T mask2be(T mask)
{
  T be = 0;
  T m = 0xFF;
  for(size_t i=1; i<sizeof(T); i++)
    m <<= 8;

  for (size_t i=0; i<sizeof(T); i++) {
    be <<= 1;
    if((mask & m) == m )
      be|=1;
    mask <<= 8;
  }
  return be;
}

template<typename T>
T be2mask(T be)
{
	const T m = (1<<sizeof(T));
	T r = 0;

	for ( size_t i=0; i<sizeof(T); ++i ) {
		r <<= 8;
		be <<= 1;
		if ( be & m )
			r |= 0xff;
	}
	return r;
}

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param_caba,typename vci_param_tlmdt> x VciInitiatorTransactor<vci_param_caba,vci_param_tlmdt>

//////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//////////////////////////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciInitiatorTransactor( sc_core::sc_module_name name )
	   : sc_module(name)
	  , m_buffer()
	  , m_nirq(0)
	  , p_clk("p_clk")
	  , p_resetn("p_resetn")
	  , p_vci_initiator("vci_initiator")
	  , p_vci_target("vci_target")
{
  //PDES local time
  m_pdes_local_time = new pdes_local_time(sc_core::SC_ZERO_TIME);
  m_cmd_working     = false;
  m_clock_count     = 0;
  m_cmd_count       = 0;
  m_rsp_count       = 0;
  m_active_irq      = false;

  // bind target vci socket
  p_vci_target(*this);                     

  SC_METHOD(transition);
  dont_initialize();
  sensitive << p_clk.pos();
  
  SC_METHOD(genMoore);
  dont_initialize();
  sensitive << p_clk.neg();
}

tmpl(/**/)::VciInitiatorTransactor( sc_core::sc_module_name name, size_t nirq)
	   : sc_module(name)
	  , m_buffer()
	  , m_nirq(nirq)
	  , p_clk("p_clk")
	  , p_resetn("p_resetn")
	  , p_vci_initiator("vci_initiator")
	  , p_vci_target("vci_target")
{
  //PDES local time
  m_pdes_local_time = new pdes_local_time(sc_core::SC_ZERO_TIME);
  m_cmd_working     = false;
  m_clock_count     = 0;
  m_cmd_count       = 0;
  m_rsp_count       = 0;

  // bind target vci socket
  p_vci_target(*this); 

  //IRQ
  p_irq_target  = new sc_core::sc_in<bool>[m_nirq];
  m_irq         = new bool[m_nirq];
  m_active_irq  = false;
  
  //create irq sockets
  for(size_t i=0; i<m_nirq; i++){
    m_irq[i] = 0;

    std::ostringstream irq_name;
    irq_name << "irq" << i;
    p_irq_initiator.push_back(new tlm_utils::simple_initiator_socket_tagged<VciInitiatorTransactor,32,tlm::tlm_base_protocol_types>(irq_name.str().c_str()));
  }

  //create payload and extension of a irq transaction
  m_irq_payload_ptr = new tlm::tlm_generic_payload();
  m_irq_extension_ptr = new soclib_payload_extension();

  //has received a null message
  has_null_message = false;

  SC_METHOD(transition);
  dont_initialize();
  sensitive << p_clk.pos();
  
  SC_METHOD(genMoore);
  dont_initialize();
  sensitive << p_clk.neg();
}

tmpl(/**/)::~VciInitiatorTransactor(){}

tmpl(void)::transition()
{
  if (!p_resetn) {
    return;
  }

  //CLOCK
  m_clock_count++;
  m_pdes_local_time->add(UNIT_TIME);
  //std::cout << name() << " " << m_pdes_local_time->get().value() << std::endl;

  ///CMD
  if(!m_cmd_working){
    if(m_buffer.get_cmd_payload( m_clock_count, m_cmd_payload, m_cmd_phase, m_cmd_time)){
      m_cmd_payload->get_extension(m_cmd_extension);
      
      if(m_cmd_extension->is_write())
	m_cmd_nwords = m_cmd_payload->get_data_length() / vci_param_tlmdt::nbytes;
      else
	m_cmd_nwords = 1;
      
      m_cmd_count = 0;
      m_cmd_working = true;
    }
  }
  else{
    if(p_vci_initiator.cmdack.read()){
      m_cmd_count++;
      if(m_cmd_count == m_cmd_nwords){
	m_cmd_working = false;
      }
    }
  }

  ///RSP
  if(p_vci_initiator.rspval){
    if(m_rsp_count==0){
      m_rscrid = p_vci_initiator.rsrcid.read();
      m_rpktid = p_vci_initiator.rpktid.read();
      m_rtrdid = p_vci_initiator.rtrdid.read();
      
      m_rsp_index = m_buffer.get_rsp_payload( m_rscrid, m_rtrdid, m_rsp_payload, m_rsp_phase, m_rsp_time); 
      assert(m_rsp_index!=-1 && "response do not compatible with any command");
      
      if(p_vci_initiator.rerror.read())
	m_rsp_payload->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
      else
	m_rsp_payload->set_response_status(tlm::TLM_OK_RESPONSE);
    }
    
    utoa((int)p_vci_initiator.rdata.read(), m_rsp_payload->get_data_ptr(), m_rsp_count*vci_param_tlmdt::nbytes);
#if SOCLIB_MODULE_DEBUG
    printf("[%s] RECEIVE RESPONSE count = %d src_id = %d trd_id = %d rdata = %x local_time %d\n", name(), m_rsp_count, m_rscrid, m_rtrdid, (int)p_vci_initiator.rdata.read(), m_clock_count);
#endif
    
    
    if(p_vci_initiator.reop.read()){
      //modify the phase
      *m_rsp_phase = tlm::BEGIN_RESP;
      
      //update the message time
      *m_rsp_time = m_pdes_local_time->get();
      
#if SOCLIB_MODULE_DEBUG
      printf("[%s] SEND RESPONSE src_id = %d pkt_id = %d trdid = %d local_time = %d time = %d\n", name(), m_rscrid, m_rpktid, m_rtrdid, m_clock_count, (int)(*m_rsp_time).value());
#endif
      
      //send the response
      p_vci_target->nb_transport_bw(*m_rsp_payload, *m_rsp_phase, *m_rsp_time);
      //pop transaction
      m_buffer.pop( m_rsp_index );
      m_pop_event.notify(sc_core::SC_ZERO_TIME);
      m_rsp_count = 0;
    }
    else{
      m_rsp_count++;
    }
  }
  
  //if there is a null message response to be sent, then send it
  if(has_null_message && m_pdes_local_time->get().value()>=m_null_time->value()){
    has_null_message = false;
#if SOCLIB_MODULE_DEBUG
    printf("[%s] SEND RESPONSE NULL MESSAGE %d\n", name(), (int)m_pdes_local_time->get().value());
#endif
    p_vci_target->nb_transport_bw(*m_null_payload, *m_null_phase, *m_null_time);
  }

  irq();
}

tmpl(void)::genMoore(){
  p_vci_initiator.rspack = true;

  if(m_cmd_working){
    if(m_cmd_count < m_cmd_nwords){
      if(m_cmd_count==0){
	switch(m_cmd_extension->get_command()){
	case VCI_READ_COMMAND:
	  p_vci_initiator.cmd   = vci_param_caba::CMD_READ;
	  break;
	case VCI_WRITE_COMMAND:
	  p_vci_initiator.cmd   = vci_param_caba::CMD_WRITE;
	  break;
	case VCI_STORE_COND_COMMAND:
	  p_vci_initiator.cmd   = vci_param_caba::CMD_STORE_COND;
	  break;
	case VCI_LINKED_READ_COMMAND:
	  p_vci_initiator.cmd   = vci_param_caba::CMD_LOCKED_READ;
	  break;
	default:
	  p_vci_initiator.cmd   = vci_param_caba::CMD_NOP;
	  break;
	}       
	
	p_vci_initiator.trdid   = m_cmd_extension->get_trd_id();
	p_vci_initiator.pktid   = m_cmd_extension->get_pkt_id();
	p_vci_initiator.srcid   = m_cmd_extension->get_src_id();
	p_vci_initiator.cons    = false;
	p_vci_initiator.wrap    = false;
	p_vci_initiator.contig  = true;
	p_vci_initiator.clen    = 0;
	p_vci_initiator.cfixed  = false;
	p_vci_initiator.plen    = m_cmd_payload->get_data_length();
      }//if (m_cmd_count == 0)
      
      p_vci_initiator.cmdval  = true;
      p_vci_initiator.address = (m_cmd_payload->get_address() + (m_cmd_count*vci_param_tlmdt::nbytes));
      p_vci_initiator.wdata   = atou(m_cmd_payload->get_data_ptr(),(m_cmd_count*vci_param_tlmdt::nbytes));
      
      m_cmd_be = atou(m_cmd_payload->get_byte_enable_ptr(),(m_cmd_count*vci_param_tlmdt::nbytes));
      typename vci_param_caba::be_t be = mask2be<typename vci_param_tlmdt::data_t>(m_cmd_be);
      p_vci_initiator.be = be;
      
#if SOCLIB_MODULE_DEBUG
      printf("[%s] SEND COMMAND count = %d address = %x src_id = %d pkt_id = %d trdid = %d local_time = %d\n", name(), m_cmd_count, (int)(m_cmd_payload->get_address() + (m_cmd_count*vci_param_tlmdt::nbytes)), m_cmd_extension->get_src_id(), m_cmd_extension->get_pkt_id(), m_cmd_extension->get_trd_id(), m_clock_count);
#endif

      if(m_cmd_count==m_cmd_nwords-1){
	p_vci_initiator.eop   = true;
      }
      else{
	p_vci_initiator.eop   = false;
      }
    }//if(m_cmd_count < m_cmd_nwords)
    else{
      p_vci_initiator.cmdval = false;
    }
  }//if(m_cmd_working)
  else{
    p_vci_initiator.cmdval = false;
  }
}

tmpl(void)::irq()
{
  bool i;
  for (size_t n=0; n<m_nirq; n++){
    i = p_irq_target[n].read();
    if(i != m_irq[n]){
#ifdef SOCLIB_MODULE_DEBUG
      std::cout << "[" << name() << "] Receive Interrupt " << n << " val = " << i << std::endl;
#endif
      m_irq[n] = i;
      send_interrupt(n, i);
    }
  }
}

tmpl(void)::send_interrupt(int id, bool irq){
  size_t nbytes = vci_param_tlmdt::nbytes; //1 word
  unsigned char data_ptr[nbytes];
  unsigned char byte_enable_ptr[nbytes];
  unsigned int bytes_enabled = be2mask<unsigned int>(0xF);

  //if initiator is blocking waiting a response to a null message then send it
  if(has_null_message){
    has_null_message = false;
#if SOCLIB_MODULE_DEBUG
    printf("[%s] SEND RESPONSE NULL MESSAGE %d\n", name(), (int)m_pdes_local_time->get().value());
#endif
    p_vci_target->nb_transport_bw(*m_null_payload, *m_null_phase, *m_null_time);
  }

  m_irq[id] = irq;

  // set the all bytes to enabled
  utoa(bytes_enabled, byte_enable_ptr, 0);
  // set the val to data
  utoa(m_irq[id], data_ptr, 0);

  // set the values in tlm payload
  m_irq_payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
  m_irq_payload_ptr->set_byte_enable_ptr(byte_enable_ptr);
  m_irq_payload_ptr->set_byte_enable_length(nbytes);
  m_irq_payload_ptr->set_data_ptr(data_ptr);
  m_irq_payload_ptr->set_data_length(nbytes);
  // set the values in payload extension
  m_irq_extension_ptr->set_write();
  // set the extension to tlm payload
  m_irq_payload_ptr->set_extension(m_irq_extension_ptr);
  
  // set the tlm phase
  m_irq_phase = tlm::BEGIN_REQ;
  // set the local time to transaction time
  m_irq_time = m_pdes_local_time->get();
  //m_irq_time = sc_core::SC_ZERO_TIME;
  
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[" << name() << "] Send Interrupt " << id << " val = " << m_irq[id] <<  " time = " << m_irq_time.value() << std::endl;
#endif
  
  // send the transaction
  (*p_irq_initiator[id])->nb_transport_fw(*m_irq_payload_ptr, m_irq_phase, m_irq_time);
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if VCI SOCKET
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,  
  sc_core::sc_time         &time)   
{
  soclib_payload_extension *extension;
  payload.get_extension(extension);
  if(extension->is_null_message()){
 
    m_null_payload = &payload;
    m_null_phase   = &phase;
    m_null_time    = &time;
    *m_null_time   = *m_null_time - UNIT_TIME;
    has_null_message = true;
#if SOCLIB_MODULE_DEBUG
    printf("[%s] RECEIVE NULL MESSAGE %d null_time = %d\n", name(), (int)m_pdes_local_time->get().value(), (int)(*m_null_time).value());
#endif
 
    //send response to a null message
    if(m_pdes_local_time->get().value()>=m_null_time->value()){
      has_null_message = false;
#if SOCLIB_MODULE_DEBUG
      printf("[%s] SEND RESPONSE NULL MESSAGE %d\n", name(), (int)m_pdes_local_time->get().value());
#endif
      p_vci_target->nb_transport_bw(*m_null_payload, *m_null_phase, *m_null_time);
    }
      
    return tlm::TLM_COMPLETED;
  }

  bool push = false;
  int try_push = 0;
  do{

#if SOCLIB_MODULE_DEBUG
    printf("[%s] RECEIVE COMMAND time = %d local_time = %d\n", name(), (int)time.value(), m_clock_count);
#endif
    time = time - UNIT_TIME;
    push = m_buffer.push(payload, phase, time);

    if(!push){
      try_push++;
#if SOCLIB_MODULE_DEBUG
      printf("[%s] <<<<<<<<< NOT PUSH >>>>>>>> try_push = %d \n", name(), try_push);
#endif
      sc_core::wait(m_pop_event);
    }
  }while(!push);
  return tlm::TLM_COMPLETED;
}

// Not implemented for this example but required by interface
tmpl(void)::b_transport
( tlm::tlm_generic_payload &payload,                // payload
  sc_core::sc_time         &time)                   //time
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
