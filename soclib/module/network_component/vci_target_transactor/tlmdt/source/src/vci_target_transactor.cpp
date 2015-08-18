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

#include "vci_target_transactor.h"

#ifndef MY_DEBUG
#define MY_DEBUG 0
#endif

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

#define tmpl(x) template<typename vci_param_caba,typename vci_param_tlmdt> x VciTargetTransactor<vci_param_caba,vci_param_tlmdt>

//////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//////////////////////////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciTargetTransactor( sc_core::sc_module_name name)
	   : sc_module(name)
	  , m_nirq(0)
	  , m_buffer()
	  , p_clk("p_clk")
	  , p_resetn("p_resetn")
          , p_vci_target("vci_target")
	  , p_vci_initiator("vci_initiator")
{
  init();
}

tmpl(/**/)::VciTargetTransactor( sc_core::sc_module_name name, size_t n_irq)
	   : sc_module(name)
	  , m_nirq(n_irq)
	  , m_buffer()
	  , p_clk("p_clk")
	  , p_resetn("p_resetn")
          , p_vci_target("vci_target")
	  , p_vci_initiator("vci_initiator")
{
  init();
}

tmpl(/**/)::~VciTargetTransactor(){}

tmpl(void)::init(void)
{
  // bind vci initiator socket
  p_vci_initiator(*this);                     

  //PDES local time
  m_pdes_local_time = new pdes_local_time(100 * UNIT_TIME);

  //create payload and extension of a null message
  m_null_payload_ptr = new tlm::tlm_generic_payload();
  m_null_extension_ptr = new soclib_payload_extension();

  has_rsp_transaction = false;
  is_rsp_eop = false;
  m_cmd_index = 0;
  m_cmd_count = 0;
  m_rsp_count = 0;
  m_rsp_index = 0;
  m_clock_count = 0;
  
  //IRQ Port
  if(m_nirq>0){
    p_irq_initiator = new sc_core::sc_out<bool>[m_nirq];

    for(size_t i=0; i<m_nirq; i++){
      std::ostringstream irq_name;
      irq_name << "irq" << i;
      p_irq_target.push_back(new tlm_utils::simple_target_socket_tagged<VciTargetTransactor,32,tlm::tlm_base_protocol_types>(irq_name.str().c_str()));
      p_irq_target[i]->register_nb_transport_fw(this, &VciTargetTransactor::irq_nb_transport_fw, i);
    }
  }

  SC_METHOD(cmd);
  dont_initialize();
  sensitive << p_clk.pos();
   
  SC_METHOD(interruption);
  dont_initialize();
  sensitive << p_clk.pos();
  
  SC_METHOD(rsp);
  dont_initialize();
  sensitive << p_clk.neg();
}

tmpl(void)::cmd()
{
  //if( p_vci_target.cmdval){
  if ( p_vci_target.iAccepted() ) {

#if MY_DEBUG
      std::cout << " clock  :" << std::dec << m_clock_count << std::endl
		<< "VciCmdBuffer" << std::hex << std::endl
		<< " cmdval : " << p_vci_target.cmdval.read() << std::endl
		<< " address: " << p_vci_target.address.read() << std::endl
		<< " be     : " << p_vci_target.be.read() << std::endl
		<< " cmd    : " << p_vci_target.cmd.read() << std::endl
		<< " contig : " << p_vci_target.contig.read() << std::endl
		<< " wdata  : " << p_vci_target.wdata.read() << std::endl
		<< " eop    : " << p_vci_target.eop.read() << std::endl
		<< " cons   : " << p_vci_target.cons.read() << std::endl
		<< " plen   : " << p_vci_target.plen.read() << std::endl
		<< " wrap   : " << p_vci_target.wrap.read()  << std::endl
		<< " cfixed : " << p_vci_target.cfixed.read() << std::endl
		<< " clen   : " << p_vci_target.clen.read() << std::endl
		<< " srcid  : " << p_vci_target.srcid.read() << std::endl
		<< " trdid  : " << p_vci_target.trdid.read() << std::endl
		<< " pktid  : " << p_vci_target.pktid.read() << std::endl
		<< std::endl;
#endif

    if(m_cmd_count==0){
      m_cmd_index = m_buffer.get_empty_position();
#if SOCLIB_MODULE_DEBUG
      printf("[%s] m_clock_count = %d  m_cmd_index = %d\n", name(), m_clock_count, m_cmd_index);
#endif
      m_buffer.set_command( m_cmd_index, tlm::TLM_IGNORE_COMMAND );
      m_buffer.set_address( m_cmd_index, p_vci_target.address.read() );
      m_buffer.set_time( m_cmd_index, m_clock_count * UNIT_TIME);
      m_pdes_local_time->reset_sync();
      
      switch( p_vci_target.cmd.read() ){
      case vci_param_caba::CMD_READ:
	m_buffer.set_ext_command( m_cmd_index, VCI_READ_COMMAND);
	break;
      case vci_param_caba::CMD_WRITE:
	m_buffer.set_ext_command( m_cmd_index, VCI_WRITE_COMMAND);
	break;
      case vci_param_caba::CMD_STORE_COND:
	m_buffer.set_ext_command( m_cmd_index, VCI_STORE_COND_COMMAND);
	break;
      case vci_param_caba::CMD_LOCKED_READ:
	m_buffer.set_ext_command( m_cmd_index, VCI_LINKED_READ_COMMAND);
	break;
      default:
	m_buffer.set_ext_command( m_cmd_index, VCI_READ_COMMAND);
	break;
      }
      
      m_buffer.set_trd_id( m_cmd_index, p_vci_target.trdid.read() );
      m_buffer.set_pkt_id( m_cmd_index, p_vci_target.pktid.read() );
      m_buffer.set_src_id( m_cmd_index, p_vci_target.srcid.read() );
      
      m_cmd_nbytes = p_vci_target.plen.read();
      if(m_cmd_nbytes < vci_param_tlmdt::nbytes)
	m_cmd_nbytes= vci_param_tlmdt::nbytes;
    }// if count
    
    m_buffer.set_data( m_cmd_index, m_cmd_count, p_vci_target.wdata.read());
    m_buffer.set_byte_enable( m_cmd_index, m_cmd_count, be2mask<typename vci_param_caba::data_t>(p_vci_target.be.read()));
    
#if SOCLIB_MODULE_DEBUG
    printf("[%s] time = %d srcid = %d cmd_count = %d\n", name(), m_clock_count,  m_buffer.get_src_id( m_cmd_index), m_cmd_count);
#endif
    
    m_cmd_count++;
    
    if(p_vci_target.eop){
      m_buffer.set_data_length( m_cmd_index, m_cmd_nbytes);
      m_buffer.set_byte_enable_length( m_cmd_index, m_cmd_nbytes);
      m_buffer.set_phase( m_cmd_index, tlm::BEGIN_REQ);
#if SOCLIB_MODULE_DEBUG
      printf("[%s] SEND COMMAND time = %d\n", name(), m_buffer.get_time_value(m_cmd_index));
#endif
      p_vci_initiator->nb_transport_fw(*m_buffer.get_payload(m_cmd_index), *m_buffer.get_phase(m_cmd_index), *m_buffer.get_time(m_cmd_index));
      m_cmd_count=0;
    }
  }// if cmdval
  
  
  m_clock_count++;

  m_pdes_local_time->set(m_clock_count * UNIT_TIME);

  if (m_pdes_local_time->need_sync()) {
    send_null_message();
  }
}

tmpl(void)::rsp()
{
  if(!has_rsp_transaction){
    m_rsp_index = m_buffer.select_response(m_clock_count);
    if(m_rsp_index >= 0){
#if SOCLIB_MODULE_DEBUG
      printf("[%s] m_clock_count = %d m_rsp_index = %d\n", name(), m_clock_count, m_rsp_index);
#endif
      has_rsp_transaction = true;
      m_rsp_count = 0;
      if(m_buffer.is_write_command(m_rsp_index))
	m_rsp_nwords = 1;
      else
	m_rsp_nwords = m_buffer.get_data_length(m_rsp_index) / vci_param_tlmdt::nbytes;
    }
  }
  
  if(is_rsp_eop){
    is_rsp_eop = false;
    has_rsp_transaction = false;
  }
  
  if(has_rsp_transaction){
    p_vci_target.cmdack = false; 

    if((p_vci_target.rspack && m_rsp_count<m_rsp_nwords) || m_rsp_count==0){
      p_vci_target.rspval = true;
      
      if(m_rsp_count==0){
	p_vci_target.rsrcid = m_buffer.get_src_id( m_rsp_index );
	p_vci_target.rpktid = m_buffer.get_pkt_id( m_rsp_index );
	p_vci_target.rtrdid = m_buffer.get_trd_id( m_rsp_index );
	p_vci_target.rerror = m_buffer.get_response_status( m_rsp_index );
      }

      p_vci_target.rdata = m_buffer.get_data(m_rsp_index, m_rsp_count);

#if SOCLIB_MODULE_DEBUG
      printf("[%s] time = %d rscrid = %d rsp_count = %d rdata = 0x%08x\n", name(), m_clock_count, m_buffer.get_src_id( m_rsp_index ), m_rsp_count, (int)m_buffer.get_data(m_rsp_index, m_rsp_count));
#endif

      m_rsp_count++;
      if(m_rsp_count==m_rsp_nwords){
	//assert(m_clock_count == m_buffer.get_time_value(m_rsp_index) && "m_clock_count != target time");
	p_vci_target.reop = true;
	is_rsp_eop = true;
	m_buffer.set_empty_position(m_rsp_index);
      }
      else
	p_vci_target.reop = false;

#if MY_DEBUG
      printf(" clock:  %d\n",m_clock_count); 
      printf(" reop:   %d\n",is_rsp_eop);
      
      if(m_buffer.is_write_command(m_rsp_index) || m_buffer.get_response_status( m_rsp_index ))
	printf(" rdata:  0x000000000\n"); 
      else if(m_buffer.is_read_command(m_rsp_index))
	printf(" rdata:  0x%09x\n",(int)m_buffer.get_data(m_rsp_index, m_rsp_count-1));
      else
	printf(" rdata:  0x%09x\n",(int)m_buffer.get_data(m_rsp_index, 0));
      printf(" rerror: 0x%x\n",m_buffer.get_response_status( m_rsp_index )); 
      printf(" rsrcid: 0x%03x\n",m_buffer.get_src_id( m_rsp_index )); 
      printf(" rtrdid: 0x%03x\n",m_buffer.get_trd_id( m_rsp_index )); 
      printf(" rpktid: 0x%03x\n\n",m_buffer.get_pkt_id( m_rsp_index )); 
#endif
    }//if rspack
  }
  else{
    p_vci_target.cmdack = true; 
    p_vci_target.rspval = false;
  }
}

tmpl(void)::interruption(){
  if ( ! m_pending_irqs.empty() && m_pending_irqs.begin()->first <= m_pdes_local_time->get() ) {
    std::map<sc_core::sc_time, std::pair<int, bool> >::iterator i = m_pending_irqs.begin();

    if((p_irq_initiator[i->second.first].read() && !i->second.second) || 
       (!p_irq_initiator[i->second.first].read() && i->second.second)){

#ifdef SOCLIB_MODULE_DEBUG
      std::cout << "[" << name() << "] Time = " << m_pdes_local_time->get().value() << " execute interruption id  = " << i->second.first << " val = " << i->second.second << std::endl;
#endif

      p_irq_initiator[i->second.first].write(i->second.second);
      
      m_pending_irqs.erase(i);
    }
  }
}

tmpl (void)::print_transaction(bool fw, tlm::tlm_generic_payload &payload)
{
  soclib_payload_extension *extension_ptr;
  payload.get_extension(extension_ptr);
   
  if(fw)
    printf("[%s] SEND COMMAND ", name());
  else
    printf("[%s] RECEIVE RESPONSE ", name());

  switch(extension_ptr->get_command()){
  case VCI_READ_COMMAND:
    printf("VCI_READ_COMMAND\n");
    break;
  case VCI_WRITE_COMMAND:
    printf("VCI_WRITE_COMMAND\n");
    break;
  case VCI_STORE_COND_COMMAND:
    printf("VCI_STORE_COND_COMMAND\n");
    break;
  case VCI_LINKED_READ_COMMAND:
    printf("LINKED_READ_COMMAND\n");
    break;
  case PDES_NULL_MESSAGE:
    printf("PDES_NULL_MESSAGE\n");
    break;
  default:
    printf("DEFAULT\n");
    break;
  }       

  int nwords = payload.get_data_length()/vci_param_tlmdt::nbytes;
  printf("nword = %d\n", nwords);
  
  for(int i=0; i<nwords; i++){
    printf("address = 0x%08x\n",(int)(payload.get_address() + (i*vci_param_tlmdt::nbytes)));
    printf("be = 0x%08x\n",atou(payload.get_byte_enable_ptr(),(i*vci_param_tlmdt::nbytes)));
    if((fw && extension_ptr->is_write()) || (!fw && !extension_ptr->is_write()))
      printf("data = 0x%08x\n",atou(payload.get_data_ptr(),(i*vci_param_tlmdt::nbytes)));
  }
  
  printf("src_id = %d\n", extension_ptr->get_src_id());
  //printf("pkt_id = %d\n", extension_ptr->get_pkt_id());
  printf("trd_id = %d\n", extension_ptr->get_trd_id());
}

tmpl(void)::send_null_message()
{
  // set the null message command
  m_null_extension_ptr->set_null_message();
  // set the extension to tlm payload
  m_null_payload_ptr->set_extension(m_null_extension_ptr);
  //set the tlm phase
  m_null_phase = tlm::BEGIN_REQ;
  //set the local time to transaction time
  m_null_time = m_pdes_local_time->get();
   
#if SOCLIB_MODULE_DEBUG
  //printf("[%s] send NULL MESSAGE time = %d\n", name(), (int)m_null_time.value());
#endif

#if MY_DEBUG
  std::cout << "[" << name() << "] send NULL MESSAGE time = " << m_null_time.value() << std::endl;
#endif
  //send a null message
  m_pdes_local_time->reset_sync();
  p_vci_initiator->nb_transport_fw(*m_null_payload_ptr, m_null_phase, m_null_time);
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if VCI INITIATOR SOCKET
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_bw
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,  
  sc_core::sc_time         &time)   
{
  soclib_payload_extension *extension;
  payload.get_extension(extension);

  if(!extension->is_null_message()){
    m_buffer.set_response(payload, time);

#if SOCLIB_MODULE_DEBUG
    printf("[%s] RECEIVE RESPONSE clock_count = %d time = %d\n", name(), m_clock_count, (int)time.value());
#endif
  }

  return tlm::TLM_COMPLETED;
}

// Not implemented for this example but required by interface
tmpl(void)::invalidate_direct_mem_ptr               // invalidate_direct_mem_ptr
( sc_dt::uint64 start_range,                        // start range
  sc_dt::uint64 end_range                           // end range
  )
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if (IRQ TARGET SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::irq_nb_transport_fw
( int                      id,         // interruption id
  tlm::tlm_generic_payload &payload,   // payload
  tlm::tlm_phase           &phase,     // phase
  sc_core::sc_time         &time)      // time
{
  std::map<sc_core::sc_time, std::pair<int, bool> >::iterator i;
  bool v = (bool) atou(payload.get_data_ptr(), 0);
  bool find = false;
 
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[" << name() << "] receive Interrupt " << id << " value " << v << " time " << time.value() << std::endl;
#endif

  //if false interruption then it must be tested if there is a true interruption with the same id.
  //In afirmative case, the true interruption must be deleted
  if(!v){
    for( i = m_pending_irqs.begin(); i != m_pending_irqs.end(); ++i){
      if(i->second.first == id && i->first == time){
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << "[" << name() << "] delete interrupt " << i->second.first << " value " << i->second.second << " time " << i->first.value() << std::endl;
#endif
	find = true;
	m_pending_irqs.erase(i);
      }
    }
  }

  //if true interruption or (false interruption and no true interruption with the same id) the it adds
  if(!find){
#ifdef SOCLIB_MODULE_DEBUG
    std::cout << "[" << name() << "] insert interrupt " << id << " value " << v << " time " << time.value() << std::endl;
#endif
    m_pending_irqs[time] = std::pair<int, bool>(id, v);
  }

  return tlm::TLM_COMPLETED;

} // end backward nb transport 

}}
