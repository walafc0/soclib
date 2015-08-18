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

#include "vci_mwmr_controller.h"

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciMwmrController<vci_param>

tmpl (void)::update_time(sc_core::sc_time t)
{
  if(t > m_pdes_local_time->get()){
    m_pdes_local_time->set(t);
  }
}

tmpl (void)::update_time(uint64_t t)
{
  if(t > m_pdes_local_time->get().value()){
    m_pdes_local_time->set(t * UNIT_TIME);
  }
}

tmpl(void)::send_activity()
{
  // set the active or inactive command
  if(m_pdes_activity_status->get())
    m_activity_extension_ptr->set_active();
  else
    m_activity_extension_ptr->set_inactive();
  m_activity_extension_ptr->set_src_id(m_srcid);
  // set the extension to tlm payload
  m_activity_payload_ptr->set_extension(m_activity_extension_ptr);
  //set the tlm phase
  m_activity_phase = tlm::BEGIN_REQ;
  //set the local time to transaction time
  m_activity_time = m_pdes_local_time->get();

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] Send Activity %d with time = %d\n", m_srcid, m_pdes_activity_status->get(), (int)m_activity_time.value());
  std::cout << "[MWMR Initiator " << m_srcid << "] Send Activity " << m_pdes_activity_status->get() << " with time = " << m_activity_time.value() << std::endl;
#endif

  //send a message with command equals to PDES_ACTIVE or PDES_INACTIVE
  p_vci_initiator->nb_transport_fw(*m_activity_payload_ptr, m_activity_phase, m_activity_time);
  //deschedule
  wait(sc_core::SC_ZERO_TIME);
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if (INITIATOR VCI SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::nb_transport_bw      // receive the response packet from target socket
( tlm::tlm_generic_payload &payload,            // payload
  tlm::tlm_phase           &phase,              // phase
  sc_core::sc_time         &time)               // time
{

#ifdef SOCLIB_MODULE_DEBUG
  soclib_payload_extension *extension_pointer;
  payload.get_extension(extension_pointer);

  fprintf(pFile, "[MWMR Initiator %d] Receive from source %d a answer packet %d with time = %d\n", m_srcid, extension_pointer->get_src_id(), extension_pointer->get_pkt_id(), (int)time.value());
  std::cout << "[MWMR Initiator " << m_srcid << "] Receive from source " << extension_pointer->get_src_id() << " a answer packet " << extension_pointer->get_pkt_id() << " with time = " << time.value() << std::endl;
#endif

  //Update the time local
  update_time(time);
  m_vci_event.notify(sc_core::SC_ZERO_TIME);
  return tlm::TLM_COMPLETED;
}

// Not implemented for this example but required by interface
tmpl(void)::invalidate_direct_mem_ptr            // invalidate_direct_mem_ptr
( sc_dt::uint64 start_range,                     // start range
  sc_dt::uint64 end_range                        // end range
) 
{
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if (TARGET VCI SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::nb_transport_fw       // receive the command packet from initiator socket
( tlm::tlm_generic_payload &payload,             // payload
  tlm::tlm_phase           &phase,               // phase
  sc_core::sc_time         &time)                // time
{
  soclib_payload_extension *extension_pointer;
  payload.get_extension(extension_pointer);

  //this target does not treat the null message
  if(extension_pointer->is_null_message()){
    return tlm::TLM_COMPLETED;
  }

  std::list<soclib::common::Segment>::iterator seg;	
  size_t segIndex;

  for (segIndex=0,seg = m_target_segments.begin(); seg != m_target_segments.end(); ++segIndex, ++seg ) {
    soclib::common::Segment &s = *seg;
    if (!s.contains(payload.get_address()))
      continue;

    switch(extension_pointer->get_command()){
    case VCI_READ_COMMAND:
      return vci_read_nb_transport_fw(segIndex,s,payload,phase,time);
      break;
    case VCI_WRITE_COMMAND:
      return vci_write_nb_transport_fw(segIndex,s,payload,phase,time);
      break;
    default:
      break;
    }
  }
  //send error message
  payload.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
  phase = tlm::BEGIN_RESP;
  time = time + UNIT_TIME;
  
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[MWMR Target " << m_destid << "] Address " << payload.get_address() << " does not match any segment " << std::endl;
  std::cout << "[MWMR Target " << m_destid << "] Send to source "<< extension_pointer->get_src_id() << " a error packet with time = "  << time.value() << std::endl;
#endif
  
  p_vci_target->nb_transport_bw(payload, phase, time);
  return tlm::TLM_COMPLETED;
}

/// b_transport() - Blocking Transport
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

/////////////////////////////////////////////////////////////////////////////////////
// READ COMMAND (TARGET VCI SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::vci_read_nb_transport_fw // receive the READ command packet from initiator socket
( size_t                    segIndex,               // segment index
  soclib::common::Segment  &s,                      // list of segments
  tlm::tlm_generic_payload &payload,                // payload
  tlm::tlm_phase           &phase,                  // phase
  sc_core::sc_time         &time)                   // time
{
  soclib_payload_extension *extension_pointer;
  payload.get_extension(extension_pointer);

#ifdef SOCLIB_MODULE_DEBUG
  uint32_t srcid  = extension_pointer->get_src_id();
  uint32_t pktid  = extension_pointer->get_pkt_id();
#endif

  uint32_t nwords = (uint32_t)(payload.get_data_length() / vci_param::nbytes);
  int reg;

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Target %d] Receive from source %d a read packet %d with time = %d\n", m_destid, srcid, pktid, (int)time.value());
  std::cout << "[MWMR Target " << m_destid << "] Receive from source " << srcid << " a read packet " << pktid << " with time = " << time.value() << std::endl;
#endif
  
  //Update the time local
  update_time(time);
  
  for (unsigned int i=0, j=0; i<nwords; i++, j+=vci_param::nbytes){
    reg = (int)(((payload.get_address() + j) - s.baseAddress()) / vci_param::nbytes);
    if ( reg < MWMR_IOREG_MAX ) { // coprocessor IO register access
      //add the reading time (reading time equals to number of words, in this case 1)
      m_pdes_local_time->add(UNIT_TIME);
      (*p_status[reg])->nb_transport_fw(payload, phase, time);
      wait(m_copro_event);
    }
    else {                      // MWMR channel configuration access (or Reset)
      switch (reg) {
      case MWMR_CONFIG_FIFO_NO :
	utoa(m_channel_index, payload.get_data_ptr(), j);
	break;
      case MWMR_CONFIG_FIFO_WAY :
	utoa(m_channel_read, payload.get_data_ptr(), j);
	break;
      case MWMR_CONFIG_STATUS_ADDR :
	if(m_channel_read)
	  utoa(m_read_channel[m_channel_index].status_address, payload.get_data_ptr(), j);
	else
	  utoa(m_write_channel[m_channel_index].status_address, payload.get_data_ptr(), j);
	break;
      case MWMR_CONFIG_DEPTH :
	if(m_channel_read)
	  utoa(m_read_channel[m_channel_index].depth, payload.get_data_ptr(), j);
	else
	  utoa(m_write_channel[m_channel_index].depth, payload.get_data_ptr(), j);
	break;
      case MWMR_CONFIG_BUFFER_ADDR :
	if(m_channel_read)
	  utoa(m_read_channel[m_channel_index].base_address, payload.get_data_ptr(), j);
	else
	  utoa(m_write_channel[m_channel_index].base_address, payload.get_data_ptr(), j);
	break;
      case MWMR_CONFIG_RUNNING :
	if(m_channel_read)
	  utoa(m_read_channel[m_channel_index].running, payload.get_data_ptr(), j);
	else
	  utoa(m_write_channel[m_channel_index].running, payload.get_data_ptr(), j);
	break;
      }
    }
  }
    
  //send anwser
  payload.set_response_status(tlm::TLM_OK_RESPONSE);
  phase = tlm::BEGIN_RESP;
  time = time + (nwords * UNIT_TIME);

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Target %d ] Send answer packet %d with time = %d\n", m_destid , pktid, (int)time.value());
  std::cout << "[MWMR Target " << m_destid << " ] Send answer packet " << pktid << " with time = " << time.value() << std::endl;
#endif
	
  p_vci_target->nb_transport_bw(payload, phase, time);
  return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// WRITE COMMAND (TARGET VCI SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::vci_write_nb_transport_fw // receive the WRITE command packet from initiator socket
( size_t                    segIndex,                // segment index
  soclib::common::Segment  &s,                       // list of segments
  tlm::tlm_generic_payload &payload,                 // payload
  tlm::tlm_phase           &phase,                   // phase
  sc_core::sc_time         &time)                    // time
{

  soclib_payload_extension *extension_pointer;
  payload.get_extension(extension_pointer);

#ifdef SOCLIB_MODULE_DEBUG
  uint32_t srcid  = extension_pointer->get_src_id();
  uint32_t pktid  = extension_pointer->get_pkt_id();
#endif

  uint32_t nwords = (uint32_t)(payload.get_data_length() / vci_param::nbytes);
  int reg;

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Target %d ] Receive from source %d a write packet %d with time = %d\n",m_destid, srcid, pktid, (int)time.value());
  std::cout << "[MWMR Target " <<  m_destid << "] Receive from source " << srcid << " a write packet " << pktid << " with time = " << time.value() << std::endl;
#endif
    
  //Update the time local
  update_time(time);

  for (unsigned int i=0, j=0; i<nwords; i++, j+=vci_param::nbytes){
    reg = (int)(((payload.get_address() + j) - s.baseAddress()) / vci_param::nbytes);
    if ( reg < MWMR_IOREG_MAX ) { // coprocessor IO register access
      (*p_config[reg])->nb_transport_fw(payload, phase, time);
      wait(m_copro_event);
    }
    else {                      // MWMR channel configuration access (or Reset)
      switch (reg) {
      case MWMR_RESET :
	m_reset_request = true;
	break;
      case MWMR_CONFIG_FIFO_NO :
	m_channel_index = atou(payload.get_data_ptr(), j);
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Target %d] MWMR_CONFIG_FIFO_NO m_channel_index = %d\n", m_destid, m_channel_index);
	std::cout << "[MWMR Target " << m_destid << "] MWMR_CONFIG_FIFO_NO m_channel_index = " << m_channel_index << std::endl;
#endif
	break;
      case MWMR_CONFIG_FIFO_WAY :
	m_channel_read = !(atou(payload.get_data_ptr(), j));
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Target %d] MWMR_CONFIG_FIFO_WAY m_channel_read = %d\n", m_destid, m_channel_read);
	std::cout << "[MWMR Target " << m_destid << "] MWMR_CONFIG_FIFO_WAY m_channel_read = " << m_channel_read << std::endl;
#endif
	break;
      case MWMR_CONFIG_STATUS_ADDR :
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Target %d] MWMR_CONFIG_STATUS_ADDR status_address = %x\n", m_destid, atou(payload.get_data_ptr (), j));
	std::cout << "[MWMR Target " << m_destid << "] MWMR_CONFIG_STATUS_ADDR status_address = " << std::hex << atou(payload.get_data_ptr(), j) << std::dec << std::endl;
#endif
	if(m_channel_read)
	  m_read_channel[m_channel_index].status_address = atou(payload.get_data_ptr(), j);
	else
	  m_write_channel[m_channel_index].status_address = atou(payload.get_data_ptr(), j);
	break;
      case MWMR_CONFIG_DEPTH :
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Target %d] MWMR_CONFIG_DEPTH depth = %x\n", m_destid, atou(payload.get_data_ptr(), j));
	std::cout << "[MWMR Target " << m_destid << "] MWMR_CONFIG_DEPTH depth = " << std::hex << atou(payload.get_data_ptr(), j) << std::dec << std::endl;
#endif
	if(m_channel_read)
	  m_read_channel[m_channel_index].depth = atou(payload.get_data_ptr(), j);
	else
	  m_write_channel[m_channel_index].depth = atou(payload.get_data_ptr(), j);
	break;
      case MWMR_CONFIG_BUFFER_ADDR :
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Target %d] MWMR_CONFIG_BUFFER_ADDR base_address = %x\n", m_destid, atou(payload.get_data_ptr(), j));
	std::cout << "[MWMR Target " << m_destid << "] MWMR_CONFIG_BUFFER_ADDR base_address = " << std::hex << atou(payload.get_data_ptr(), j) << std::dec << std::endl;
#endif
	if(m_channel_read)
	  m_read_channel[m_channel_index].base_address = atou(payload.get_data_ptr(), j);
	else
	  m_write_channel[m_channel_index].base_address = atou(payload.get_data_ptr(), j);
	break;
      case MWMR_CONFIG_RUNNING :
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Target %d] MWMR_CONFIG_RUNNING channel = %d running = %d\n", m_destid, m_channel_index, atou(payload.get_data_ptr(), j));
	std::cout << "[MWMR Target " << m_destid << "] MWMR_CONFIG_RUNNING channel = " << m_channel_index <<" running = " << atou(payload.get_data_ptr(), j) << std::endl;
#endif
	if(m_channel_read)
	  m_read_channel[m_channel_index].running = atou(payload.get_data_ptr(), j);
	else
	  m_write_channel[m_channel_index].running = atou(payload.get_data_ptr(), j);
	
	m_active_event.notify(sc_core::SC_ZERO_TIME);
	break;
      } // end switch cell
    }
  }

  //send anwser
  payload.set_response_status(tlm::TLM_OK_RESPONSE);
  phase = tlm::BEGIN_RESP;
  time = time + (nwords * UNIT_TIME);

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Target %d ] Send answer packet %d with time = %d\n", m_destid , pktid, (int)time.value());
  std::cout << "[MWMR Target " <<  m_destid << "] Send answer packet " << pktid << " with time = " <<  time.value() << std::endl;
#endif
  
  p_vci_target->nb_transport_bw(payload, phase, time);
  return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if (READ FIFO TARGET SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::read_fifo_nb_transport_fw  // receive data from initiator read fifo
( int                      index,                    // fifo id
  tlm::tlm_generic_payload &payload,                 // payload
  tlm::tlm_phase           &phase,                   // phase
  sc_core::sc_time         &time)                    // time
{
  uint32_t nwords = payload.get_data_length() / vci_param::nbytes;

  //Update the time local
  if(m_read_fifo[index].time < time)
    m_read_fifo[index].time = time;

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d ] Receive read request fifo = %d nword = %d with time = %d has data = %d\n", m_srcid, index, nwords, (int)m_read_fifo[index].time.value(), (m_read_fifo[index].n_elements >= nwords));
  std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive read request fifo = " << index << " nword = " << nwords << " with time = " << (int)m_read_fifo[index].time.value() << " has data = " << (m_read_fifo[index].n_elements >= nwords) << std::endl;
#endif
  
  //add the reading time (reading time equals to number of words)
  m_read_fifo[index].time += (nwords * UNIT_TIME);
    
  if ( m_read_fifo[index].n_elements >= nwords){ //have element to read 
    for (uint32_t i = ((m_read_fifo_depth/vci_param::nbytes) - m_read_fifo[index].n_elements), j=0; i < nwords; i++, j+=4){
      utoa(m_read_fifo[index].data[i], payload.get_data_ptr(), j);
    }

    m_read_fifo[index].n_elements -= nwords;
    
    if(m_read_fifo[index].n_elements == 0){
      m_read_fifo[index].empty = true;
      m_fifo_event.notify(sc_core::SC_ZERO_TIME);
    }
    
    //send awnser
#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send read response to coprocessor\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read response to coprocessor" << std::endl;
#endif
    (*p_from_coproc[index])->nb_transport_bw(payload, phase, m_read_fifo[index].time);
  } 
  else{
    m_read_request[index].pending = true;
    m_read_request[index].data = payload.get_data_ptr();
    m_read_request[index].n_elements = nwords;
    m_read_request[index].time = m_read_fifo[index].time;
  } 
  return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_fw_transport_if (WRITE FIFO TARGET SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::write_fifo_nb_transport_fw // receive data from initiator write fifo
( int                      index,                    // fifo id
  tlm::tlm_generic_payload &payload,                 // payload
  tlm::tlm_phase           &phase,                   // phase
  sc_core::sc_time         &time)                    // time
{
  uint32_t nwords = payload.get_data_length() / vci_param::nbytes;

  //Update the time local
  if(m_write_fifo[index].time < time)
    m_write_fifo[index].time = time;

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] Receive write request fifo = %d nword = %d with time = %d has space = %d n_elements = %d full = %d\n", m_srcid, index, nwords, (int)m_write_fifo[index].time.value(), (((m_write_fifo_depth/vci_param::nbytes) - m_write_fifo[index].n_elements) >= nwords), m_write_fifo[index].n_elements,  m_write_fifo[index].full );
  std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive write request fifo = " <<  index << " nwords = " << nwords << " with time = " << (int)m_write_fifo[index].time.value() << " has space = " << (((m_write_fifo_depth/vci_param::nbytes) - m_write_fifo[index].n_elements) >= nwords) << " n_elements = " << m_write_fifo[index].n_elements << " full = " << m_write_fifo[index].full << std::endl;
#endif

  //add the writing time (writing time equals to number of words)
  m_write_fifo[index].time += (nwords * UNIT_TIME);

  if (((m_write_fifo_depth/vci_param::nbytes) - m_write_fifo[index].n_elements) >= nwords){ //have space to write
    
    for (uint32_t i = m_write_fifo[index].n_elements, j=0; i<nwords; i++, j+=vci_param::nbytes){
      m_write_fifo[index].data[i] = atou(payload.get_data_ptr(), j);
    }
    
    m_write_fifo[index].n_elements += nwords;
    
    if(m_write_fifo[index].n_elements == (m_write_fifo_depth/vci_param::nbytes)){
      m_write_fifo[index].full = true;
      m_fifo_event.notify(sc_core::SC_ZERO_TIME);
    }
    
    //send awnser
#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send write response to coprocessor\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write response to coprocessor" << std::endl;
#endif
    (*p_to_coproc[index])->nb_transport_bw(payload, phase, m_write_fifo[index].time);
  }
  else{// no have space
    m_write_request[index].pending = true;
    m_write_request[index].data = payload.get_data_ptr();
    m_write_request[index].n_elements = nwords;
    m_write_request[index].time = m_write_fifo[index].time;
  }
  return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if (INITIATOR COPROCESSOR (STATUS AND CONFIG) SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::copro_nb_transport_bw   // receive the response packet from coprocessor
( int                       id,                   // register id
  tlm::tlm_generic_payload &payload,              // payload
  tlm::tlm_phase           &phase,                // phase
  sc_core::sc_time         &time)                 // time
{
  //update_time(time);
  m_copro_event.notify(sc_core::SC_ZERO_TIME);
  return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////  
// RESET THE MWMR
/////////////////////////////////////////////////////////////////////////////  
tmpl(void)::reset() 
{
  for ( uint32_t  i = 0 ; i < m_read_channels ; i++ ) {
    m_read_fifo[i].empty       = true;
    m_read_fifo[i].full        = false;
    m_read_request[i].pending  = false;
    m_read_channel[i].running  = false;
  }
  for ( uint32_t  i = 0 ; i < m_write_channels ; i++ ) {
    m_write_fifo[i].empty      = true;
    m_write_fifo[i].full       = false;
    m_write_request[i].pending = false;
    m_write_channel[i].running = false;
  }
  
  m_fifo_read_phase  = tlm::BEGIN_REQ;
  m_fifo_write_phase = tlm::BEGIN_REQ;
  m_fifo_read_time   =  m_pdes_local_time->get();
  m_fifo_write_time  =  m_pdes_local_time->get();

  //send the anwser to all read fifo
  for ( uint32_t i = 0; i < m_read_channels; i++)
    (*p_from_coproc[i])->nb_transport_bw(*m_fifo_read_payload_ptr,m_fifo_read_phase,m_fifo_read_time);
  //send the anwser to all write fifo
  for ( uint32_t i = 0; i < m_write_channels; i++)
    (*p_to_coproc[i])->nb_transport_bw(*m_fifo_write_payload_ptr,m_fifo_write_phase,m_fifo_write_time);

  m_reset_request = false;
}

/////////////////////////////////////////////////////////////////////////////  
// GET LOCK  EXECUTE THE LOCKED_READ AND STORE CONDITIONAL OPERATIONS 
/////////////////////////////////////////////////////////////////////////////  
tmpl(void)::getLock(typename vci_param::addr_t status_address, uint32_t *status) 
{
  do{
#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] GET LOCK\n",m_srcid);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] GET LOCK" << std::endl;
#endif

    do{
      
#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Send locked read packet with time = %d\n",m_srcid,(int)m_pdes_local_time->get().value());
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Send locked read packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif

      send_read(VCI_LINKED_READ_COMMAND,
		status_address + 12,
		&status[3],
		1
		);

#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d lock = %d\n",m_srcid,(int)m_pdes_local_time->get().value(),status[3]);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)m_pdes_local_time->get().value() << " lock = " << status[3] << std::endl;
#endif

      m_pktid++;
	
      m_pdes_local_time->add(UNIT_TIME);

    }while(status[3]!=0);

    status[3]     = 1;

#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send store conditional packet with time = %d lock = %d\n",m_srcid, (int)m_pdes_local_time->get().value(),status[3]);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Send store conditional packet with time = " << (int)m_pdes_local_time->get().value() << " lock = " << status[3] << std::endl;
#endif

    send_write(VCI_STORE_COND_COMMAND,
	       status_address + 12,
	       &status[3],
	       1
	       );

#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Receive awnser store conditional packet with time = %d lock = %d\n",m_srcid, (int)m_pdes_local_time->get().value(),status[3]);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser store conditional packet with time = " << (int)m_pdes_local_time->get().value() << " lock = " << status[3] << std::endl;
#endif

    m_pktid++;
    
    m_pdes_local_time->add(UNIT_TIME);
    
  }while(status[3]!=0);

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] END GET LOCK\n",m_srcid);
  std::cout << "[MWMR Initiator " <<  m_srcid << "] END GET LOCK" << std::endl;
#endif
}

tmpl(void)::releaseLock(typename vci_param::addr_t status_address, uint32_t *status) 
{
#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] RELEASE THE LOCK\n",m_srcid);
  std::cout << "[MWMR Initiator " <<  m_srcid << "] RELEASE THE LOCK" << std::endl;
#endif
  status[3]     = 0; //release the lock

#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send write packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif

  send_write(VCI_WRITE_COMMAND,
	     status_address + 12,
	     &status[3],
	     1
	     );
  
  
  m_pktid++;

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] Receive awnser write packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
  std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser write packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif
  
  m_pdes_local_time->add(UNIT_TIME);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// READ STATUS OF A DETERMINED CHANNEL
////////////////////////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::readStatus(typename vci_param::addr_t status_address, uint32_t *status) 
{
  // STATUS[0] = index_read;
  // STATUS[1] = index_write;
  // STATUS[2] = content (capacity)
  // STATUS[3] = lock
  
#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] READ STATUS\n",m_srcid);
  std::cout << "[MWMR Initiator " <<  m_srcid << "] READ STATUS" << std::endl;
#endif
    
#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif

  send_read(VCI_READ_COMMAND,
	    status_address,
	    status,
	    3
	    );
  
  m_pktid++;
  
#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
  std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif
  
  m_pdes_local_time->add(UNIT_TIME);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE STATUS OF A DETERMINED CHANNEL
////////////////////////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::updateStatus(typename vci_param::addr_t status_address, uint32_t *status) 
{
#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] UPDATE STATUS\n",m_srcid);
  std::cout << "[MWMR Initiator " <<  m_srcid << "] UPDATE STATUS" << std::endl;
#endif
  status[3]     = 0; // release the lock 

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
  std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif

  send_write(VCI_WRITE_COMMAND,
	     status_address,
	     status,
	     4
	     );
  
  m_pktid++;

#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
  std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// READ DATA FROM A DETERMINED CHANNEL TO A FIFO
////////////////////////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::readFromChannel(uint32_t fifo_index, uint32_t *status) 
{
  
  // STATUS[0] = index_read;
  // STATUS[1] = index_write;
  // STATUS[2] = content (number of elements in the channel)
  // STATUS[3] = lock
  
  
#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] BUSY_POSITIONS = %d BASE ADDRESS = %.8x DEPTH = %.8x\n",m_srcid, status[2], m_read_channel[fifo_index].base_address, m_read_channel[fifo_index].depth);
  std::cout << "[MWMR Initiator " <<  m_srcid << "] BUSY_POSITIONS = " << status[2] << std::hex << " BASE ADDRESS = " << m_read_channel[fifo_index].base_address << " DEPTH = " << m_read_channel[fifo_index].depth << std::dec << std::endl;
#endif

  m_pdes_local_time->add(UNIT_TIME);

  ///////// read transfer OK //////////
  if(status[2] >= (m_read_fifo_depth/vci_param::nbytes)){  
    
    if((status[0]+m_read_fifo_depth)<=m_read_channel[fifo_index].depth){ // send only 1 message

#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] READ FIFO %d SEND MESSAGE 1 OF 1\n",m_srcid,fifo_index);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO " << fifo_index << " SEND MESSAGE 1 OF 1" << std::endl;
#endif
		  
#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d address = %d\n",m_srcid, (int)m_pdes_local_time->get().value(),(m_read_channel[fifo_index].base_address + status[0]));
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)m_pdes_local_time->get().value() << " address = " << (m_read_channel[fifo_index].base_address + status[0]) << std::endl;
#endif

      send_read(VCI_READ_COMMAND,
		(m_read_channel[fifo_index].base_address + status[0]),
		m_read_fifo[fifo_index].data,
		(m_read_fifo_depth/vci_param::nbytes)
		);

      m_pktid++;

#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif
      
      m_pdes_local_time->add(2 * UNIT_TIME);

    }
    else{ // send 2 message
      typename vci_param::data_t data_1[(m_read_fifo_depth/vci_param::nbytes)], data_2[(m_read_fifo_depth/vci_param::nbytes)];
      typename vci_param::addr_t address;
      uint32_t nwords_1, nwords_2;
      uint32_t count = 0;
      for(nwords_1 = 0, address = status[0]; address < m_read_channel[fifo_index].depth; nwords_1++, count++, address+=vci_param::nbytes );
      nwords_2 = (m_read_fifo_depth/vci_param::nbytes) - nwords_1;

#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] READ FIFO %d SEND MESSAGE 1 OF 2\n",m_srcid,fifo_index);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO " << fifo_index << " SEND MESSAGE 1 OF 2" << std::endl;
#endif

#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d address = %d\n",m_srcid, (int)m_pdes_local_time->get().value(),(m_read_channel[fifo_index].base_address + status[0]));
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)m_pdes_local_time->get().value() << " address = " << (m_read_channel[fifo_index].base_address + status[0]) << std::endl;
#endif

      send_read(VCI_READ_COMMAND,
		(m_read_channel[fifo_index].base_address + status[0]),
		data_1,
		nwords_1
		);
      
      m_pktid++;

#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif

      m_pdes_local_time->add(2 * UNIT_TIME);

#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] READ FIFO %d SEND MESSAGE 2 OF 2\n",m_srcid,fifo_index);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO " << fifo_index << " SEND MESSAGE 2 OF 2" << std::endl;
#endif


#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Send read packet with time = %d address = %d\n",m_srcid, (int)m_pdes_local_time->get().value(),m_read_channel[fifo_index].base_address);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Send read packet with time = " << (int)m_pdes_local_time->get().value() << " address =" << m_read_channel[fifo_index].base_address << std::endl;
#endif

      send_read(VCI_READ_COMMAND,
		m_read_channel[fifo_index].base_address,
		data_2,
		nwords_2
		);

      m_pktid++;
      
#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] Receive awnser read packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
      std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser read packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif

      m_pdes_local_time->add(2 * UNIT_TIME);
      
      count = 0;
      for(uint32_t j=0; count<nwords_1; count++, j++)
	m_read_fifo[fifo_index].data[count] = data_1[j];
      for(uint32_t j=0; count<(m_read_fifo_depth/vci_param::nbytes); count++, j++)
	m_read_fifo[fifo_index].data[count] = data_2[j];
      
    }

    // update the fifo state
    m_read_fifo[fifo_index].empty = false;
    m_read_fifo[fifo_index].full = true;
    m_read_fifo[fifo_index].n_elements = (m_read_fifo_depth/vci_param::nbytes);
    m_read_fifo[fifo_index].time = m_pdes_local_time->get();
    
    // update the read pointer
    status[0] = status[0] + m_read_fifo_depth;
    if(status[0] >= m_read_channel[fifo_index].depth){
      status[0] = status[0] - m_read_channel[fifo_index].depth;
    }
    
    // update the number of elements in the channel
    status[2] -= m_read_fifo_depth;
    
    // release pending fifo
    releasePendingReadFifo(fifo_index);
    
    // update the status descriptor
    updateStatus(m_read_channel[fifo_index].status_address, status);
    
    m_pdes_local_time->add(UNIT_TIME);
    
    //update the fifo time
    m_read_fifo[fifo_index].time = m_pdes_local_time->get() + (m_waiting_time * UNIT_TIME);
  }
  else{
#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] READ FIFO %d NOT OK\n",m_srcid,fifo_index);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO " << fifo_index << " NOT OK" << std::endl;
#endif
    
    ///////////// release lock ///////////////////////
    releaseLock(m_read_channel[fifo_index].status_address, status);
    
    ///////////// update the time ///////////////////
    m_read_fifo[fifo_index].time = m_pdes_local_time->get() + (m_waiting_time * UNIT_TIME);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// WRITE DATA FROM A FIFO TO A DETERMINED CHANNEL
////////////////////////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::writeToChannel(uint32_t fifo_index, uint32_t *status) 
{
  // STATUS[0] = index_read;
  // STATUS[1] = index_write;
  // STATUS[2] = content (number of elements in the channel)
  // STATUS[3] = lock
    
  m_pdes_local_time->add(UNIT_TIME);
    
  ///////// write transfer OK //////////
  if((m_write_channel[fifo_index].depth - status[2]) >= m_write_fifo_depth){
    do{
      
#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] BUSY_POSITIONS = %d FREE_POSITIONS = %d BASE ADDRESS = %.8x\n", m_srcid, status[2], (m_write_channel[fifo_index].depth - status[2]), m_write_channel[fifo_index].base_address);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] BUSY_POSITIONS = " << status[2] << " FREE_POSITIONS = " << (m_write_channel[fifo_index].depth - status[2]) << " BASE ADDRESS = " << std::hex << m_write_channel[fifo_index].base_address << std::dec << std::endl;
#endif
      
      if((status[1]+m_write_fifo_depth)<=m_write_channel[fifo_index].depth){ // send only 1 message

#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO %d SEND MESSAGE 1 OF 1\n",m_srcid,fifo_index);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO " << fifo_index << " SEND MESSAGE 1 OF 1" << std::endl;
#endif
	  
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Send write packet with time = %d address = %.8x nwords = %d\n",m_srcid, (int)m_pdes_local_time->get().value(),(m_write_channel[fifo_index].base_address + status[1]),(m_write_fifo_depth/vci_param::nbytes));
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write packet with time = " << (int)m_pdes_local_time->get().value() << std::hex << " address = " << (m_write_channel[fifo_index].base_address + status[1]) << std::dec << " nwords = " << (m_write_fifo_depth/vci_param::nbytes) << std::endl;
#endif

	send_write(VCI_WRITE_COMMAND,
		   (m_write_channel[fifo_index].base_address + status[1]),
		   m_write_fifo[fifo_index].data,
		   (m_write_fifo_depth/vci_param::nbytes)
		   );
	
	m_pktid++;
	
	m_pdes_local_time->add(2 * UNIT_TIME);
	  
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Receive awnser write packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser write packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif
      }
      else{ // send 2 message
	
	typename vci_param::addr_t address;
	typename vci_param::data_t data_1[(m_write_fifo_depth/vci_param::nbytes)], data_2[(m_write_fifo_depth/vci_param::nbytes)];
	uint32_t nwords_1, nwords_2;
	uint32_t count = 0;
	for(nwords_1 = 0, address = status[1]; address < m_write_channel[fifo_index].depth; nwords_1++, count++, address+=vci_param::nbytes)
	  data_1[nwords_1] = m_write_fifo[fifo_index].data[count];
	for(nwords_2=0; count <(m_write_fifo_depth/vci_param::nbytes); nwords_2++,count++)
	  data_2[nwords_2] = m_write_fifo[fifo_index].data[count];
	
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO %d SEND MESSAGE 1 OF 2\n",m_srcid,fifo_index);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO " << fifo_index << " SEND MESSAGE 1 OF 2" << std::endl;
#endif
	
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Send write packet with time = %d address = %d nwords = %d\n",m_srcid, (int)m_pdes_local_time->get().value(),(m_write_channel[fifo_index].base_address + status[1]),nwords_1);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write packet with time = " << (int)m_pdes_local_time->get().value() << std::hex << " address = " << (m_write_channel[fifo_index].base_address + status[1]) << std::dec << " nwords = " << nwords_1 << std::endl;
#endif

	send_write(VCI_WRITE_COMMAND,
		   (m_write_channel[fifo_index].base_address + status[1]),
		   data_1,
		   nwords_1
		   );
	
	m_pktid++;
	  
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Receive awnser write packet with time = %d\n",m_srcid, (int)m_pdes_local_time->get().value());
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser write packet with time = " << (int)m_pdes_local_time->get().value() << std::endl;
#endif
	
	m_pdes_local_time->add(2 * UNIT_TIME);

#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO %d SEND 2 MESSAGE OF 2\n",m_srcid,fifo_index);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO " << fifo_index << " SEND 2 MESSAGE OF 2" << std::endl;
#endif
	  
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Send write packet with time = %d nwords = %d\n",m_srcid, (int)m_pdes_local_time->get().value(), nwords_2);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Send write packet with time = " << (int)m_pdes_local_time->get().value() << " nwords = " << nwords_2 << std::endl;
#endif

	send_write(VCI_WRITE_COMMAND,
		   m_write_channel[fifo_index].base_address,
		   data_2,
		   nwords_2
		   );
	
	m_pktid++;
	  
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] Receive awnser write packet with time = %d address = %d\n",m_srcid, (int)m_pdes_local_time->get().value(), m_write_channel[fifo_index].base_address);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] Receive awnser write packet with time = " << (int)m_pdes_local_time->get().value() << std::hex << " address = " << m_write_channel[fifo_index].base_address << std::dec <<std::endl;
#endif
	
	m_pdes_local_time->add(2 * UNIT_TIME);

      }
		
      // update the fifo state
      m_write_fifo[fifo_index].empty = true;
      m_write_fifo[fifo_index].full  = false;
      m_write_fifo[fifo_index].n_elements = 0;
      m_write_fifo[fifo_index].time = m_pdes_local_time->get();
	
      // update the write pointer
      status[1] = status[1] + m_write_fifo_depth;
      if(status[1] >= m_write_channel[fifo_index].depth)
	status[1] = status[1] - m_write_channel[fifo_index].depth;
      
      // update the number of elements in the channel
      status[2] += m_write_fifo_depth;
      
      //// release pending fifo ////
      releasePendingWriteFifo(fifo_index);
    }while(m_write_fifo[fifo_index].full &&  (m_write_fifo[fifo_index].time<=m_pdes_local_time->get()) && ((m_write_channel[fifo_index].depth - status[2]) >= m_write_fifo_depth));
    
    //// update the status descriptor ////
    updateStatus(m_write_channel[fifo_index].status_address, status);
    
    m_pdes_local_time->add(UNIT_TIME);
    
    //update the fifo time
    m_write_fifo[fifo_index].time = m_pdes_local_time->get() + (m_waiting_time * UNIT_TIME);
  }
  else{
#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO %d NOT OK\n",m_srcid,fifo_index);
    std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO " << fifo_index << " NOT OK" << std::endl;
#endif
    
    ///////////// release lock ///////////////////////
    releaseLock(m_write_channel[fifo_index].status_address, status);
    
    ///////////// update the time ///////////////////
    m_write_fifo[fifo_index].time = m_pdes_local_time->get() + (m_waiting_time * UNIT_TIME);
  } 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// RELEASE THE PENDING READ FIFO
////////////////////////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::releasePendingReadFifo(uint32_t fifo_index)
{
#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] RELEASE PENDING FIFO\n",m_srcid);
  std::cout << "[MWMR Initiator " <<  m_srcid << "] RELEASE PENDING FIFO" << std::endl;
#endif
  
  if ( m_read_request[fifo_index].pending){ //there is request 
    //copy the data
    for ( uint32_t i=0, j=0; i<m_read_request[fifo_index].n_elements; i++, j+=vci_param::nbytes)
      utoa(m_read_fifo[fifo_index].data[i],m_read_request[fifo_index].data, j);
    
    // update the time
    m_read_fifo[fifo_index].n_elements -= m_read_request[fifo_index].n_elements;
    m_read_fifo[fifo_index].full = false;
    if(m_read_fifo[fifo_index].n_elements == 0){
      m_read_fifo[fifo_index].empty = true;
    }
    m_read_request[fifo_index].pending = false;
    
    // update the time
    if(m_read_fifo[fifo_index].time < m_pdes_local_time->get())
      m_read_fifo[fifo_index].time = m_pdes_local_time->get();
    
#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send answer to coprocessor time = %d\n",m_srcid,(int)m_read_fifo[fifo_index].time.value());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send answer to coprocessor time = " << (int)m_read_fifo[fifo_index].time.value() << std::endl;
#endif

    m_fifo_read_payload_ptr->set_data_ptr(m_read_request[fifo_index].data);
    m_fifo_read_phase = tlm::BEGIN_RESP;
    m_fifo_read_time = m_read_fifo[fifo_index].time;
    
    //send awnser to coprocessor
   (*p_from_coproc[fifo_index])->nb_transport_bw(*m_fifo_read_payload_ptr, m_fifo_read_phase, m_fifo_read_time);
  } 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// RELEASE THE PENDING FIFO
////////////////////////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::releasePendingWriteFifo(uint32_t fifo_index)
{
#ifdef SOCLIB_MODULE_DEBUG
  fprintf(pFile, "[MWMR Initiator %d] RELEASE PENDING FIFO\n",m_srcid);
  std::cout << "[MWMR Initiator " <<  m_srcid << "] RELEASE PENDING FIFO" << std::endl;
#endif
  
  if (m_write_request[fifo_index].pending){
    //copy the data
    for ( uint32_t n=0, j=0; n<m_write_request[fifo_index].n_elements; n++, j+=vci_param::nbytes)
      m_write_fifo[fifo_index].data[n] = atou(m_write_request[fifo_index].data, j);
      
    m_write_fifo[fifo_index].n_elements += m_write_request[fifo_index].n_elements;
    m_write_fifo[fifo_index].empty = false;
    if(m_write_fifo[fifo_index].n_elements == (m_write_fifo_depth/vci_param::nbytes)){
      m_write_fifo[fifo_index].full = true;
    }
    
    m_write_request[fifo_index].pending = false;
    
    // update the time
    if(m_write_fifo[fifo_index].time < m_pdes_local_time->get())
      m_write_fifo[fifo_index].time = m_pdes_local_time->get();
    
#ifdef SOCLIB_MODULE_DEBUG
    fprintf(pFile, "[MWMR Initiator %d] Send answer to coprocessor time = %d\n",m_srcid,(int)m_write_fifo[fifo_index].time.value());
    std::cout << "[MWMR Initiator " <<  m_srcid << "] Send answer to coprocessor time = " << (int)m_write_fifo[fifo_index].time.value() << std::endl;
#endif
      
    m_fifo_write_payload_ptr->set_data_ptr(m_write_request[fifo_index].data);
    m_fifo_write_phase = tlm::BEGIN_RESP;
    m_fifo_write_time = m_write_fifo[fifo_index].time;
    
    //send awnser to coprocessor
    (*p_to_coproc[fifo_index])->nb_transport_bw(*m_fifo_write_payload_ptr, m_fifo_write_phase, m_fifo_write_time);

  } 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXECUTE THE LOOP
////////////////////////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::execLoop() 
{
  bool                       fifo_serviceable = false;
  uint64_t                   fifo_time;
  uint32_t                   fifo_index = 0;
  bool                       fifo_read = false;
  uint32_t                   status[4];
  typename vci_param::addr_t status_address;
  
  // wait the running register to be set
  m_pdes_activity_status->set(false);
  send_activity();
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[MWMR Initiator " <<  m_srcid << "] BLOCKED time = " << m_pdes_local_time->get().value() << std::endl;
#endif
  wait(m_active_event);
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[MWMR Initiator " <<  m_srcid << "] DESBLOCKED time = " << m_pdes_local_time->get().value() << std::endl;
#endif

  m_pdes_activity_status->set(true);
  send_activity();
  
  while(m_pdes_local_time->get() < m_simulation_time){
    
    if(m_reset_request){
      reset();
      m_pdes_activity_status->set(false);
      send_activity();
      // wait the running register to be set
      wait(m_active_event);
      m_pdes_activity_status->set(true);
      send_activity();
    }
      
    //// select the first serviceable FIFO
    //// taking the request time into account 
    //// write FIFOs have the highest priority
    fifo_serviceable = false; 
    fifo_time = MAX_TIME;
    
    for (uint32_t  i = 0; i < m_read_channels; i++) {
#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] READ CHANNEL %d EMPTY = %d RUNNING = %d max fifo_time = %d fifo time = %d\n", m_srcid, i, m_read_fifo[i].empty,m_read_channel[i].running, (int)fifo_time.value(), (int)m_read_fifo[i].time.value());
      std::cout << "[MWMR Initiator " <<  m_srcid << "] READ CHANNEL " << i << " EMPTY = " << m_read_fifo[i].empty << " RUNNING = " << m_read_channel[i].running << " max fifo_time = " << fifo_time.value() << " fifo time = " << m_read_fifo[i].time.value() << " current time = " << m_pdes_local_time->get().value() << std::endl;
#endif
      if ( m_read_fifo[i].empty && m_read_channel[i].running){
	if (fifo_time >= m_read_fifo[i].time.value()) {
	  fifo_serviceable = true;
	  fifo_time = m_read_fifo[i].time.value();
	  fifo_index = i;
	  fifo_read  = true;
	} // end if date
      } // end if valid
    } // end for read fifo
    
    for (uint32_t  i = 0; i < m_write_channels; i++) {
#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] WRITE CHANNEL %d FULL = %d RUNNING = %d max fifo_time = %d fifo time = %d\n", m_srcid, i, m_write_fifo[i].empty, m_write_channel[i].running, (int)fifo_time.value(), (int)m_write_fifo[i].time.value());
      std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE CHANNEL " << i << " FULL = " << m_write_fifo[i].full << " RUNNING = " << m_write_channel[i].running << " max fifo_time = " << fifo_time.value() << " fifo time = " << m_write_fifo[i].time.value() << " current time = " << m_pdes_local_time->get().value() << std::endl;
#endif
      if ( m_write_fifo[i].full && m_write_channel[i].running) {
	if (fifo_time >= m_write_fifo[i].time.value()) {
	  fifo_serviceable = true;
	  fifo_time = m_write_fifo[i].time.value();
	  fifo_index = i;
	  fifo_read  = false;
	} // end if date
      } // end if valid
    } // end for write fifo
    
    if ( !fifo_serviceable ){
#ifdef SOCLIB_MODULE_DEBUG
      fprintf(pFile, "[MWMR Initiator %d] FIFO NO SERVICEABLE\n",m_srcid);
      std::cout << "[MWMR Initiator " <<  m_srcid << "] FIFO NO SERVICEABLE" << std::endl;
#endif
      m_pdes_activity_status->set(false);
      send_activity();
      // wait the coprocessor event
      wait(m_fifo_event);
      m_pdes_activity_status->set(true);
      send_activity();
    }
    else{
      //Update Time
      update_time(fifo_time);
      
      // get the status address
      if (fifo_read) {
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] READ FIFO SELECTED %d\n", m_srcid, fifo_index);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] READ FIFO SELECTED " << fifo_index << std::endl;
#endif
	status_address = m_read_channel[fifo_index].status_address;
      } 
      else {    /////////////////////////////////////////////////////
#ifdef SOCLIB_MODULE_DEBUG
	fprintf(pFile, "[MWMR Initiator %d] WRITE FIFO SELECTED %d\n", m_srcid, fifo_index);
	std::cout << "[MWMR Initiator " <<  m_srcid << "] WRITE FIFO SELECTED " << fifo_index << std::endl;
#endif
	status_address = m_write_channel[fifo_index].status_address;
      }
      
      m_pdes_local_time->add(2 * UNIT_TIME);
      
      //// get the lock ////
      getLock(status_address,status);
      
      //// read the status ////
      readStatus(status_address,status);
      
      if (fifo_read){
	////// read from channel //////
	readFromChannel(fifo_index,status);
      }
      else{
	////// write to channel //////
	writeToChannel(fifo_index,status);
      }
    }  
  }
} // end loopExec        
    
tmpl(/**/)::VciMwmrController
( sc_core::sc_module_name name,
  const soclib::common::MappingTable &mt,
  const soclib::common::IntTab &initiator_index,
  const soclib::common::IntTab &target_index,
  uint32_t read_fifo_depth,  //in bytes
  uint32_t write_fifo_depth, //in bytes
  uint32_t n_read_channels,
  uint32_t n_write_channels,
  uint32_t n_config,
  uint32_t n_status,
  sc_core::sc_time simulation_time)
	   : sc_module(name),
	   m_mt(mt),
	   m_read_fifo_depth(read_fifo_depth),
	   m_write_fifo_depth(write_fifo_depth),
	   m_read_channels(n_read_channels),
	   m_write_channels(n_write_channels),
	   m_config_registers(n_config),
	   m_status_registers(n_status),
	   p_vci_initiator("vci_initiator"),
	   p_vci_target("vci_target")
{
  // bind INITIATOR VCI SOCKET
  p_vci_initiator(*this);                     

  // bind TARGET VCI SOCKET
  p_vci_target(*this);                     

  //PDES local time
  //m_pdes_local_time = new pdes_local_time(time_quantum);
  m_pdes_local_time = new pdes_local_time(sc_core::SC_ZERO_TIME);
  
  //PDES activity status
  m_pdes_activity_status = new pdes_activity_status();

  //identification
  m_srcid = m_mt.indexForId(initiator_index);
  m_destid = m_mt.indexForId(target_index);
  m_initiator_segments = m_mt.getSegmentList(initiator_index);
  m_target_segments = m_mt.getSegmentList(target_index);

  //determine the simulation time
  m_simulation_time = simulation_time;
      
#ifdef SOCLIB_MODULE_DEBUG
  char fileName[50];
  sprintf (fileName, "mwmr%d.txt", m_srcid);
  pFile = fopen(fileName,"w");
#endif
  
  m_waiting_time = 64;
  m_channel_index = 0;
  m_channel_read = false;
  m_reset_request = false;
  m_pktid = 1;
      
  m_read_channel = new channel_struct<vci_param>[m_read_channels];
  m_read_request = new request_struct<vci_param>[m_read_channels];
  m_read_fifo    = new fifos_struct<vci_param>[m_read_channels];
  for(uint32_t i=0;i<m_read_channels;i++){
    //Channel
    m_read_channel[i].running   = false ;
    //Requests
    m_read_request[i].data      = new unsigned char[m_read_fifo_depth];
    m_read_request[i].pending   = false ;
    m_read_request[i].n_elements = 0;
    //Fifos
    m_read_fifo[i].data         = new typename vci_param::data_t[(m_read_fifo_depth/vci_param::nbytes)];
    m_read_fifo[i].empty        = true;
    m_read_fifo[i].full         = false ;
    m_read_fifo[i].n_elements   = 0;
    
    std::ostringstream read_fifo_name;
    read_fifo_name << "read_fifo" << i;
    p_from_coproc.push_back(new tlm_utils::simple_target_socket_tagged<VciMwmrController,32,tlm::tlm_base_protocol_types>(read_fifo_name.str().c_str()));
    p_from_coproc[i]->register_nb_transport_fw(this, &VciMwmrController::read_fifo_nb_transport_fw, i);
    
  }
  
  m_write_channel = new channel_struct<vci_param>[m_write_channels];
  m_write_request = new request_struct<vci_param>[m_write_channels];
  m_write_fifo    = new fifos_struct<vci_param>[m_write_channels];
  for(uint32_t i=0;i<m_write_channels;i++){
    //Channel
    m_write_channel[i].running  = false;
    //Request
    m_write_request[i].data     = new unsigned char[m_write_fifo_depth];
    m_write_request[i].pending  = false;
    m_write_request[i].n_elements = 0;
    //Fifos
    m_write_fifo[i].data        = new typename vci_param::data_t[(m_write_fifo_depth/vci_param::nbytes)];
    m_write_fifo[i].empty       = true;
    m_write_fifo[i].full        = false;
    m_write_fifo[i].n_elements  = 0;
    
    std::ostringstream write_fifo_name;
    write_fifo_name << "write_fifo" << i;
    p_to_coproc.push_back(new tlm_utils::simple_target_socket_tagged<VciMwmrController,32,tlm::tlm_base_protocol_types>(write_fifo_name.str().c_str()));
    p_to_coproc[i]->register_nb_transport_fw(this, &VciMwmrController::write_fifo_nb_transport_fw, i);
  }
  
  //CONFIG PORTS
  for(uint32_t i=0;i<m_config_registers;i++){
    std::ostringstream config_name;
    config_name << "config" << i;
    p_config.push_back(new tlm_utils::simple_initiator_socket_tagged<VciMwmrController,32,tlm::tlm_base_protocol_types>(config_name.str().c_str()));
    p_config[i]->register_nb_transport_bw(this, &VciMwmrController::copro_nb_transport_bw, i);
  }
  
  //STATUS PORTS
  for(uint32_t i=0;i<m_status_registers;i++){
    std::ostringstream status_name;
    status_name << "status" << i;
    p_status.push_back(new tlm_utils::simple_initiator_socket_tagged<VciMwmrController,32,tlm::tlm_base_protocol_types>(status_name.str().c_str()));
    p_status[i]->register_nb_transport_bw(this, &VciMwmrController::copro_nb_transport_bw, i);
  }

  // Create payload and extension to a normal message
  m_payload_ptr = new tlm::tlm_generic_payload();
  m_extension_ptr = new soclib_payload_extension();
 
  // Create payload and extension to a null message
  m_null_payload_ptr = new tlm::tlm_generic_payload();
  m_null_extension_ptr = new soclib_payload_extension();

  // Create payload and extension to an activity status message
  m_activity_payload_ptr = new tlm::tlm_generic_payload();
  m_activity_extension_ptr = new soclib_payload_extension();

  // Create payload to read_fifo message
  m_fifo_read_payload_ptr = new tlm::tlm_generic_payload();

  // Create payload to write fifo message
  m_fifo_write_payload_ptr = new tlm::tlm_generic_payload();

  SC_THREAD(execLoop);
  
}
    
tmpl(void)::send_write
( enum command command, 
  typename vci_param::addr_t address,
  typename vci_param::data_t *data, 
  //			   typename vci_param::data_t &rdata, 
  size_t size
  )
{
  uint32_t nbytes = size * vci_param::nbytes;
  typename vci_param::data_t byte_enable = 0xFFFFFFFF;
  unsigned char data_ptr[nbytes];
  unsigned char byte_enable_ptr[nbytes];

  for(unsigned int i=0, j=0; i<size;i++,j+=vci_param::nbytes){
    utoa(byte_enable, byte_enable_ptr, j);
    utoa(data[i], data_ptr, j);
  }

  // set the values in tlm payload
  m_payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
  m_payload_ptr->set_address(address);
  m_payload_ptr->set_byte_enable_ptr(byte_enable_ptr);
  m_payload_ptr->set_byte_enable_length(nbytes);
  m_payload_ptr->set_data_ptr(data_ptr);
  m_payload_ptr->set_data_length(nbytes);
  // set the values in payload extension
  m_extension_ptr->set_command(command);
  m_extension_ptr->set_src_id(m_srcid);
  m_extension_ptr->set_trd_id(0);
  m_extension_ptr->set_pkt_id(m_pktid);
  // set the extension to tlm payload
  m_payload_ptr->set_extension(m_extension_ptr);
  //set the tlm phase
  m_phase = tlm::BEGIN_REQ;
  //set the local time to transaction time
  m_time = m_pdes_local_time->get();

  //send a write message
  p_vci_initiator->nb_transport_fw(*m_payload_ptr, m_phase, m_time);
  wait(m_vci_event);

   //update response
  for(uint32_t i=0; i<(m_payload_ptr->get_data_length()/vci_param::nbytes); i++)
    data[i] = atou(m_payload_ptr->get_data_ptr(), (i * vci_param::nbytes));
 
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[" << name() << "] Send write packet with time = " << m_pdes_local_time->get().value() << " address =" << address << std::endl;
#endif
  
}

tmpl(void)::send_read
( enum command command,
  typename vci_param::addr_t address,
  typename vci_param::data_t *data, 
  size_t size
  )
{
  uint32_t nbytes = size * vci_param::nbytes;
  typename vci_param::data_t byte_enable = 0xFFFFFFFF;
  unsigned char data_ptr[nbytes];
  unsigned char byte_enable_ptr[nbytes];

  for(unsigned int i=0, j=0; i<size;i++,j+=vci_param::nbytes){
    utoa(byte_enable, byte_enable_ptr, j);
  }

  // set the values in tlm payload
  m_payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
  m_payload_ptr->set_address(address);
  m_payload_ptr->set_byte_enable_ptr(byte_enable_ptr);
  m_payload_ptr->set_byte_enable_length(nbytes);
  m_payload_ptr->set_data_ptr(data_ptr);
  m_payload_ptr->set_data_length(nbytes);
  // set the values in payload extension
  m_extension_ptr->set_command(command);
  m_extension_ptr->set_src_id(m_srcid);
  m_extension_ptr->set_trd_id(0);
  m_extension_ptr->set_pkt_id(m_pktid);
  // set the extension to tlm payload
  m_payload_ptr->set_extension(m_extension_ptr);
  //set the tlm phase
  m_phase = tlm::BEGIN_REQ;
  //set the local time to transaction time
  m_time = m_pdes_local_time->get();

  //send a write message
  p_vci_initiator->nb_transport_fw(*m_payload_ptr, m_phase, m_time);
  wait(m_vci_event);

  //update response
  for(uint32_t i=0; i<(m_payload_ptr->get_data_length()/vci_param::nbytes); i++)
    data[i] = atou(m_payload_ptr->get_data_ptr(), (i * vci_param::nbytes));

}

}}
