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

#include "my_initiator.h"                       // Our header

#define RAM_BASE           0xBFC00000
#define MWMR0_BASE         0x10000000
#define MWMR1_BASE         0x20000000
#define MWMR2_BASE         0x30000000
#define MWMR3_BASE         0x40000000
#define CHANNEL_DEPTH      0x00000100  //256 bytes = 64 positions
#define STATUS_DEPTH       0x00000010  // 16 bytes =  4 positions
#define CHANNEL0_BASE      RAM_BASE
#define CHANNEL1_BASE      (CHANNEL0_BASE + STATUS_DEPTH + CHANNEL_DEPTH)
#define CHANNEL2_BASE      (CHANNEL1_BASE + STATUS_DEPTH + CHANNEL_DEPTH)
#define CHANNEL3_BASE      (CHANNEL2_BASE + STATUS_DEPTH + CHANNEL_DEPTH)

#define tmpl(x) x my_initiator

///Constructor
tmpl (/**/)::my_initiator
	    ( sc_core::sc_module_name name,           // module name
	      const soclib::common::IntTab &index,    // index of mapping table
	      const soclib::common::MappingTable &mt, // mapping table
	      sc_core::sc_time time_quantum,          // time quantum
	      sc_core::sc_time simulation_time)       // simulation time
	    : sc_module(name),                        // init module name
	    m_mt(mt),                                 // mapping table
	    p_vci_initiator("socket")                 // vci initiator socket name
{ 
  //register callback function VCI INITIATOR SOCKET
  p_vci_initiator.register_nb_transport_bw(this, &my_initiator::my_nb_transport_bw);

  // initiator identification
  m_srcid = mt.indexForId(index);
  m_pktid = 1;

  //PDES local time
  m_pdes_local_time = new pdes_local_time(time_quantum);

  //PDES activity status
  m_pdes_activity_status = new pdes_activity_status();

  //determine the simulation time
  m_simulation_time = simulation_time;

  // register thread process
  SC_THREAD(behavior);    
}

/////////////////////////////////////////////////////////////////////////////////////
// Fuctions
/////////////////////////////////////////////////////////////////////////////////////
void my_initiator::send_activity()
{
  tlm::tlm_generic_payload *payload_ptr = new tlm::tlm_generic_payload();
  tlm::tlm_phase            phase;
  sc_core::sc_time          time;
  soclib_payload_extension *extension_ptr = new soclib_payload_extension();

  // set the active or inactive command
  if(m_pdes_activity_status->get())
    extension_ptr->set_active();
  else
    extension_ptr->set_inactive();
  extension_ptr->set_src_id(m_srcid);
  // set the extension to tlm payload
  payload_ptr->set_extension (extension_ptr);
  //set the tlm phase
  phase = tlm::BEGIN_REQ;
  //set the local time to transaction time
  time = m_pdes_local_time->get();
   
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[INITIATOR " << m_srcid << "] send ACTIVITY " << m_pdes_activity_status->get() << " time = " << m_pdes_local_time->get().value() << std::endl;
#endif
  //send a message with command equals to PDES_ACTIVE or PDES_INACTIVE
  p_vci_initiator->nb_transport_fw(*payload_ptr, phase, time);
  //wait a response
  wait(m_rspEvent);
}

void my_initiator::send_null_message()
{
  tlm::tlm_generic_payload *payload_ptr = new tlm::tlm_generic_payload();
  tlm::tlm_phase            phase;
  sc_core::sc_time          time;
  soclib_payload_extension *extension_ptr = new soclib_payload_extension();

  // set the null message command
  extension_ptr->set_null_message();
  extension_ptr->set_src_id(m_srcid);
  // set the extension to tlm payload
  payload_ptr->set_extension(extension_ptr);
  //set the tlm phase
  phase = tlm::BEGIN_REQ;
  //set the local time to transaction time
  time = m_pdes_local_time->get();
   
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[INITIATOR " << m_srcid << "] send NULL MESSAGE time = " << time.value() << std::endl;
#endif

  //send a null message
  p_vci_initiator->nb_transport_fw(*payload_ptr, phase, time);
  //deschedule the initiator thread
  wait(sc_core::SC_ZERO_TIME);
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONFIGURE THE COPROCESSOR
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(void)::writeConfigCopro(vci_param::addr_t address, int reg, int value)
{
  tlm::tlm_generic_payload *payload_ptr = new tlm::tlm_generic_payload();
  tlm::tlm_phase            phase;
  sc_core::sc_time          time;
  soclib_payload_extension *extension_ptr = new soclib_payload_extension();

  unsigned char data[vci_param::nbytes];
  unsigned char byte_enable[vci_param::nbytes];
  uint32_t nbytes = vci_param::nbytes;
  uint32_t byte_enabled = 0xffffffff;

  address += reg * vci_param::nbytes;

  utoa(byte_enabled, byte_enable, 0);
  utoa(value, data, 0);

  // set the values in tlm payload
  payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
  payload_ptr->set_address(address);
  payload_ptr->set_byte_enable_ptr(byte_enable);
  payload_ptr->set_byte_enable_length(nbytes);
  payload_ptr->set_data_ptr(data);
  payload_ptr->set_data_length(nbytes);
  // set the values in payload extension
  extension_ptr->set_write();
  extension_ptr->set_src_id(m_srcid);
  extension_ptr->set_trd_id(0);
  extension_ptr->set_pkt_id(m_pktid);
  // set the extension to tlm payload
  payload_ptr->set_extension(extension_ptr);
  // set the tlm phase
  phase = tlm::BEGIN_REQ;
  // set the local time to transaction time
  time = m_pdes_local_time->get();
  
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[Initiator " << m_srcid << "] Send WRITE CONFIG packet " << m_pktid << " with time = " << m_pdes_local_time->get().value() << std::endl;
#endif
    
  // send the transaction
  p_vci_initiator->nb_transport_fw(*payload_ptr, phase, time);
  wait(m_rspEvent);

    
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[Initiator "<< m_srcid << "] Receive Response packet " << m_pktid << " with time = " << m_pdes_local_time->get().value() << std::endl;
#endif
  m_pktid++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONFIGURE THE COPROCESSOR
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(int)::readStatusCopro(vci_param::addr_t address, int reg)
{
  tlm::tlm_generic_payload *payload_ptr = new tlm::tlm_generic_payload();
  tlm::tlm_phase            phase;
  sc_core::sc_time          time;
  soclib_payload_extension *extension_ptr = new soclib_payload_extension();

  unsigned char data[vci_param::nbytes];
  unsigned char byte_enable[vci_param::nbytes];
  uint32_t nbytes = vci_param::nbytes;
  uint32_t byte_enabled = 0xffffffff;

  address += reg * vci_param::nbytes;

  utoa(byte_enabled, byte_enable, 0);

  // set the values in tlm payload
  payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
  payload_ptr->set_address(address);
  payload_ptr->set_byte_enable_ptr(byte_enable);
  payload_ptr->set_byte_enable_length(nbytes);
  payload_ptr->set_data_ptr(data);
  payload_ptr->set_data_length(nbytes);
  // set the values in payload extension
  extension_ptr->set_read();
  extension_ptr->set_src_id(m_srcid);
  extension_ptr->set_trd_id(0);
  extension_ptr->set_pkt_id(m_pktid);
  // set the extension to tlm payload
  payload_ptr->set_extension(extension_ptr);
  // set the tlm phase
  phase = tlm::BEGIN_REQ;
  // set the local time to transaction time
  time = m_pdes_local_time->get();

#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[Initiator " << m_srcid << "] Send READ STATUS packet " << m_pktid << " with time = " << m_pdes_local_time->get().value() << std::endl;
#endif
    
  // send the transaction
  p_vci_initiator->nb_transport_fw(*payload_ptr, phase, time);
  wait(m_rspEvent);
    
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[Initiator "<< m_srcid << "] Receive Response packet " << m_pktid << " data = " << std::hex << atou(payload_ptr->get_data_ptr(),0) << std::dec << " with time = " << m_pdes_local_time->get().value() << std::endl;
#endif
  m_pktid++;

  return atou(payload_ptr->get_data_ptr(),0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONFIGURE CHANNEL STATUS 
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(void)::configureChannelStatus(int n_channels) 
{
  tlm::tlm_generic_payload *payload_ptr = new tlm::tlm_generic_payload();
  tlm::tlm_phase            phase;
  sc_core::sc_time          time;
  soclib_payload_extension *extension_ptr = new soclib_payload_extension();

  vci_param::addr_t current_address = RAM_BASE;
  uint32_t depth = CHANNEL_DEPTH;
  unsigned char data[CHANNEL_DEPTH];
  unsigned char byte_enable[CHANNEL_DEPTH];
  uint32_t nwords = 4;
  uint32_t nbytes = nwords * vci_param::nbytes;
  uint32_t byte_enabled = 0xffffffff;
  

  for (int i = 0; i<n_channels; i++){

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << "[ CONFIGURE STATUS FIFO " << i << "]" << std::endl;
#endif

    for(unsigned int i=0, j=0; i<nwords; i++, j+=vci_param::nbytes){
      utoa(byte_enabled, byte_enable, j);
      utoa(0, data, j);
    }

    // set the values in tlm payload
    payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
    payload_ptr->set_address(current_address);
    payload_ptr->set_byte_enable_ptr(byte_enable);
    payload_ptr->set_byte_enable_length(nbytes);
    payload_ptr->set_data_ptr(data);
    payload_ptr->set_data_length(nbytes);
    // set the values in payload extension
    extension_ptr->set_write();
    extension_ptr->set_src_id(m_srcid);
    extension_ptr->set_trd_id(0);
    extension_ptr->set_pkt_id(m_pktid);
    // set the extension to tlm payload
    payload_ptr->set_extension(extension_ptr);
    // set the tlm phase
    phase = tlm::BEGIN_REQ;
    // set the local time to transaction time
    time = m_pdes_local_time->get();

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << "[Initiator " << m_srcid << "] Send packet " << m_pktid << " address " << std::hex << payload_ptr->get_address() << std::dec << " with time = " << m_pdes_local_time->get().value() << std::endl;
#endif
    
    // send the transaction
    p_vci_initiator->nb_transport_fw(*payload_ptr, phase, time);
    wait(m_rspEvent);
    
#ifdef SOCLIB_MODULE_DEBUG
    std::cout << "[Initiator "<< m_srcid << "] Receive Response packet " << m_pktid << " with time = " << m_pdes_local_time->get().value() << std::endl;
#endif

    m_pktid++;

    current_address += (nwords*vci_param::nbytes); //positions correspond to descriptor status
    current_address += depth;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONFIGURE THE MWMR
//////////////////////////////////////////////////////////////////////////////////////////////// ///////
tmpl(void)::configureMwmr(vci_param::addr_t mwmr_address, vci_param::addr_t channel_address, int n_channel, bool is_read)
{
  tlm::tlm_generic_payload *payload_ptr = new tlm::tlm_generic_payload();
  tlm::tlm_phase            phase;
  sc_core::sc_time          time;
  soclib_payload_extension *extension_ptr = new soclib_payload_extension();

  unsigned int depth = CHANNEL_DEPTH;
  unsigned char data[CHANNEL_DEPTH];
  unsigned char byte_enable[CHANNEL_DEPTH];
  uint32_t nwords = 7;
  uint32_t nbytes = nwords * vci_param::nbytes;
  uint32_t byte_enabled = 0xffffffff;
  
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[ CONFIGURE MWMR ]" << std::endl;
#endif

  mwmr_address += 0x00000044; // WAY
  
  for(unsigned int j=0, k=0; j<nwords; j++, k+=4)
    utoa(byte_enabled, byte_enable, k);
  
  if(is_read) utoa(0, data, 0);                  // 0 = MWMR_FROM_COPROC = READ
  else        utoa(1, data, 0);                  // 1 = MWMR_FROM_COPROC = WRITE
  
  utoa(n_channel, data, 4);                      // NO
  utoa(channel_address, data, 8);                // STATUS_ADDRESS
  
  channel_address += STATUS_DEPTH;               // 4 words to status registers
  
  utoa(depth, data, 12);                         // DEPTH
  utoa(channel_address, data, 16);               // BASE_ADDRESS
  utoa(0, data, 20);                             // LOCK
  utoa(1, data, 24);                             // RUNNING
  
  // set the values in tlm payload
  payload_ptr->set_command(tlm::TLM_IGNORE_COMMAND);
  payload_ptr->set_address(mwmr_address);
  payload_ptr->set_byte_enable_ptr(byte_enable);
  payload_ptr->set_byte_enable_length(nbytes);
  payload_ptr->set_data_ptr(data);
  payload_ptr->set_data_length(nbytes);
  // set the values in payload extension
  extension_ptr->set_write();
  extension_ptr->set_src_id(m_srcid);
  extension_ptr->set_trd_id(0);
  extension_ptr->set_pkt_id(m_pktid);
  // set the extension to tlm payload
  payload_ptr->set_extension(extension_ptr);
  // set the tlm phase
  phase = tlm::BEGIN_REQ;
  // set the local time to transaction time
  time = m_pdes_local_time->get();
  
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[Initiator " << m_srcid << "] Send packet " << m_pktid << " address " << std::hex << payload_ptr->get_address() << std::dec << " with time = " << m_pdes_local_time->get().value() << std::endl;
#endif
  
  // send the transaction
  p_vci_initiator->nb_transport_fw(*payload_ptr, phase, time);
  wait(m_rspEvent);
  
#ifdef SOCLIB_MODULE_DEBUG
  std::cout << "[Initiator "<< m_srcid << "] Receive Response packet " << m_pktid << " with time = " << m_pdes_local_time->get().value() << std::endl;
#endif
  
  m_pktid++;
}

tmpl(void)::behavior()
{
  /*
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // CONFIGURE THE COPROCESSOR - TEST (WRITE AABBCCDD IN REG[0])
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  writeConfigCopro(MWMR0_BASE, 0, 0xAABBCCDD);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // READ STATUS THE COPROCESSOR - TEST (READ FROM REG[0])
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  readStatusCopro(MWMR0_BASE, 0);
  */
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // CONFIGURE THE STATUS DESCRIPTOR IN MEMORY OF ALL CHANNELS
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  configureChannelStatus(1);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // CONFIGURE THE MWMR
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  configureMwmr(MWMR0_BASE, CHANNEL0_BASE, 0, false);
  configureMwmr(MWMR1_BASE, CHANNEL0_BASE, 0, true);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // DISABLE THE INITIATOR THREAD
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  m_pdes_activity_status->set(false);
  send_activity();
}

/////////////////////////////////////////////////////////////////////////////////////
// Virtual Fuctions  tlm::tlm_bw_transport_if (VCI INITIATOR SOCKET)
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::my_nb_transport_bw  // receive the response packet from target socket
( tlm::tlm_generic_payload &payload,           // payload
  tlm::tlm_phase           &phase,             // phase
  sc_core::sc_time         &time)              // time
{
  if(time > m_pdes_local_time->get())
    m_pdes_local_time->set(time);
  m_rspEvent.notify(sc_core::SC_ZERO_TIME);
  return tlm::TLM_COMPLETED;
}

