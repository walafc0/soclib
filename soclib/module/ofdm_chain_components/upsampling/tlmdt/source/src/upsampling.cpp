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
 * Maintainers: fpecheux, alinev
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#include "upsampling.h"

#define tmpl(x) x Upsampling

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(/**/)::Upsampling(sc_core::sc_module_name name,
			uint32_t id,
			uint32_t read_fifo_depth,
			uint32_t write_fifo_depth,
			uint32_t n_read_channels,
			uint32_t n_write_channels,
			uint32_t n_config,
			uint32_t n_status)
	   : sc_module(name),
	   m_id(id),
	   m_read_fifo_depth(read_fifo_depth),
	   m_write_fifo_depth(write_fifo_depth),
	   m_read_channels(n_read_channels),
	   m_write_channels(n_write_channels),
	   m_config_registers(n_config),
	   m_status_registers(n_status)
{
  //READ FIFO PORTS
  for(uint32_t i=0;i<m_read_channels;i++){
    std::ostringstream tmpName;
    tmpName << "read_fifo" << i;
    p_read_fifo.push_back(new tlm_utils::simple_initiator_socket_tagged<Upsampling,32,tlm::tlm_base_protocol_types>(tmpName.str().c_str()));
    p_read_fifo[i]->register_nb_transport_bw(this, &Upsampling::readResponseReceived, i);
  }

  //WRITE FIFO PORTS
  for(uint32_t i=0;i<m_write_channels;i++){
    std::ostringstream tmpName;
    tmpName << "write_fifo" << i;
    p_write_fifo.push_back(new tlm_utils::simple_initiator_socket_tagged<Upsampling,32,tlm::tlm_base_protocol_types>(tmpName.str().c_str()));
    p_write_fifo[i]->register_nb_transport_bw(this, &Upsampling::writeResponseReceived, i);
  }

  //CONFIG PORTS
  m_config_register = new uint32_t[m_config_registers];
  for(uint32_t i=0;i<m_config_registers;i++){
    m_config_register[i] = 0;
    std::ostringstream tmpName;
    tmpName << "config" << i;
    p_config.push_back(new tlm_utils::simple_target_socket_tagged<Upsampling,32,tlm::tlm_base_protocol_types>(tmpName.str().c_str()));
    p_config[i]->register_nb_transport_fw(this, &Upsampling::writeConfigReceived, i);
  }

  //STATUS PORTS
  m_status_register = new uint32_t[m_status_registers];
  for(uint32_t i=0;i<m_status_registers;i++){
    m_status_register[i] = 0;
    std::ostringstream tmpName;
    tmpName << "status" << i;
    p_status.push_back(new tlm_utils::simple_target_socket_tagged<Upsampling,32,tlm::tlm_base_protocol_types>(tmpName.str().c_str()));
    p_status[i]->register_nb_transport_fw(this, &Upsampling::readStatusReceived, i);

  }

  //PDES local time
  //m_pdes_local_time = new pdes_local_time(time_quantum);
  m_pdes_local_time = new pdes_local_time(sc_core::SC_ZERO_TIME);
  
  //PDES activity status
  m_pdes_activity_status = new pdes_activity_status();

  SC_THREAD(behavior);
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
// RECEIVE REPONSE OF A READ REQUEST 
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(tlm::tlm_sync_enum)::readResponseReceived
( int                       id,                      // fifo id
  tlm::tlm_generic_payload &payload,                 // payload
  tlm::tlm_phase           &phase,                   // phase
  sc_core::sc_time         &time)                    // time
{
  //update time
  if(time > m_pdes_local_time->get())
    m_pdes_local_time->set(time);
   m_rsp_read.notify(sc_core::SC_ZERO_TIME);
  return tlm::TLM_COMPLETED;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// RECEIVE REPONSE OF A WRITE REQUEST 
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(tlm::tlm_sync_enum)::writeResponseReceived
( int                       id,                      // fifo id
  tlm::tlm_generic_payload &payload,                 // payload
  tlm::tlm_phase           &phase,                   // phase
  sc_core::sc_time         &time)                    // time
{
  //update time
  if(time > m_pdes_local_time->get())
    m_pdes_local_time->set(time);
  m_rsp_write.notify(sc_core::SC_ZERO_TIME);
  return tlm::TLM_COMPLETED;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// RECEIVE A REQUEST TO WRITE TO A CONFIGURATION REGISTER 
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(tlm::tlm_sync_enum)::writeConfigReceived
( int                       id,                      // fifo id
  tlm::tlm_generic_payload &payload,                 // payload
  tlm::tlm_phase           &phase,                   // phase
  sc_core::sc_time         &time)                    // time
{
  m_config_register[id] = atou(payload.get_data_ptr(),0);
  //m_status_register[id] = m_config_register[id];

#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[" << name() << "] CONFIG " << id << " Write data = " << std::hex << m_config_register[id] << std::dec << " time = " << time.value() << std::endl;
#endif
 
  phase= tlm::BEGIN_RESP;
  time = time + UNIT_TIME;
    
  (*p_config[id])->nb_transport_bw(payload, phase, time);

  return tlm::TLM_COMPLETED;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// RECEIVE A REQUEST TO READ FROM A STATUS REGISTER 
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(tlm::tlm_sync_enum)::readStatusReceived
( int                       id,                      // fifo id
  tlm::tlm_generic_payload &payload,                 // payload
  tlm::tlm_phase           &phase,                   // phase
  sc_core::sc_time         &time)                    // time
{
  utoa(m_status_register[id], payload.get_data_ptr(),0);

#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[" << name() << "] STATUS " << id << " Read data = " << std::hex << atou(payload.get_data_ptr(),0) << std::dec << " time = " << time.value() << std::endl;
#endif
 
  phase= tlm::BEGIN_RESP;
  time = time + UNIT_TIME;
    
  (*p_status[id])->nb_transport_bw(payload, phase, time);

  return tlm::TLM_COMPLETED;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOOP: WRITE TO A FIFO AND AFTER  RECEIVE A REQUEST TO WRITE TO A CONFIGURATION REGISTER 
////////////////////////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::behavior(){
  tlm::tlm_generic_payload  *payload_ptr = new tlm::tlm_generic_payload();
  tlm::tlm_phase            phase;
  sc_core::sc_time          time;

  vci_param::data_t write_buffer[m_write_fifo_depth];
  vci_param::data_t read_buffer[m_read_fifo_depth];

  unsigned char data[MAXIMUM_PACKET_SIZE];

  
  int count = 0;
  while(true){
      count++;
      // read
      for(uint32_t j = 0; j < m_read_channels; j++){
         // set the values in tlm payload
         payload_ptr->set_data_ptr(data);
         payload_ptr->set_data_length(m_read_fifo_depth * vci_param::nbytes);
         //set the tlm phase
         phase = tlm::BEGIN_REQ;
         //set the local time to transaction time
         time = m_pdes_local_time->get();

         #ifdef SOCLIB_MODULE_DEBUG
         std::cout << "[" << name() << "] Read FIFO " << j << " with time " << time.value() << " number of executions "<< count << std::endl;
         #endif
         //send message and wait response
         (*p_read_fifo[j])->nb_transport_fw(*payload_ptr, phase, time);    
         wait(m_rsp_read);
	 m_pdes_local_time->add(m_read_fifo_depth * UNIT_TIME);
      
         #ifdef SOCLIB_MODULE_DEBUG
         std::cout << "[" << name() << "] Awnser Read FIFO " << j << " with time " << m_pdes_local_time->get().value() << " number of executions "<< count << std::endl;
         #endif
 
         for(uint32_t i=0, k=0; i<m_read_fifo_depth; i++,k+=vci_param::nbytes){
	    read_buffer[i] = atou(payload_ptr->get_data_ptr(), k);
            #ifdef SOCLIB_MODULE_DEBUG
	    std::cout << std::dec/*hex*/ << "[ " << i <<" ] =  " << read_buffer[i] << std::dec << std::endl;
            #endif
        }
      }
      // Upsampling computation
      int k = 0;	
      for(uint32_t i=0; i < m_read_fifo_depth; i++){
          write_buffer[k] = read_buffer[i];
	  for (k = i*4+1; k<(i+1)*4; k++)
	     write_buffer[k] = 0;        
     }                        
     for(uint32_t i=0, j=0; i < m_write_fifo_depth; i++, j+=vci_param::nbytes){
        // Writing the result on data
        utoa(write_buffer[i], data, j);
    }
    //calcule time
    m_pdes_local_time->add(500 * UNIT_TIME);   
    
    //write
    for(uint32_t j = 0; j < m_write_channels; j++){
      // set the values in tlm payload
      payload_ptr->set_data_ptr(data);
      payload_ptr->set_data_length(m_write_fifo_depth * vci_param::nbytes);
      //set the tlm phase
      phase = tlm::BEGIN_REQ;
      //set the local time to transaction time
      time = m_pdes_local_time->get();
      
#ifdef SOCLIB_MODULE_DEBUG
      std::cout << "[" << name() << "] Write FIFO " << j << " with time " << time.value() << " number of executions "<< count << std::endl;
      for(uint32_t i=0; i<m_write_fifo_depth;i++)
	std::cout << std::dec/*hex*/ << "[ " << i <<" ] =  " << write_buffer[i] << std::dec << std::endl;
#endif
      
      (*p_write_fifo[j])->nb_transport_fw(*payload_ptr, phase, time);
      wait(m_rsp_write);
      m_pdes_local_time->add(m_write_fifo_depth * UNIT_TIME);

#ifdef SOCLIB_MODULE_DEBUG
      std::cout << "[" << name() << "] Awnser Write FIFO " << j << " with time " << m_pdes_local_time->get().value() << " number of executions "<< count << std::endl;
#endif
    }
    
    //calcule time
    //m_pdes_local_time->add(500 * UNIT_TIME);
    
    
  }// end while
}
