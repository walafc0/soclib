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
 * Copyright (c) UPMC, Lip6, Asim
 *         Alain Greiner  <alain.greiner@lip6.fr> 2011
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////////////
// Implementation Note
// This component is  a multi-channels DMA controller, acting both
// as a VCI initiator and as a VCI target.
// The number of channels is variable, but cannot be larger than 8.
//
// The target FSM is modeled by a purely reactive interface function.
//
// The initiator FSMs corresponding to the various parallel channels are 
// modeled by one single sc_thread, implementing all the initiator FSMs,
// but each channel has a private set of state registers.
//
// This component has a local time, that is updated depending on the
// DMA component global state:
// - when there is no channel activated, it requires periodical NULL 
//   messages from the interconnect, to be updated by  message received 
//   on the VCI target port (VCI commands or NULL messages).
// - when at least one channel is activated, the local time is updated 
//   by the local thread.
//
// Each initiator FSM can be in 6 states :
// - IDLE        : not running : waiting a VCI command on the target port
// - READ        : running     : performing a VCI read transaction
// - WRITE       : running     : performing a VCI write transaction
// - SUCCESS     : not running : waiting a VCI command on the target port
// - ERROR_READ  : not running : waiting a VCI command on the target port
// - ERROR_WRITE : not running : waiting a VCI command on the target port
//
// When the DMA is in a running state (READ or WRITE), the configuration
// commands are ignored, and the registers are not modified.
// In these running states, it makes continuously read/write transactions.
// there is no need to send null messages.,
// When the DMA is not in a running state, it send null messages.
// The IRQ flip-flop is implemented as an unsigned char.
// The IRQ value is periodically transmitted on the IRQ port (each time there is
// a VCI command or a NULL message) to allow the ICU to implement
// the PDES time filtering.
//////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_VCI_MULTI_DMA_H
#define SOCLIB_VCI_MULTI_DMA_H

#include <stdint.h>
#include <systemc>
#include <tlmdt>
#include "mapping_table.h"

namespace soclib {
namespace tlmdt {

using namespace sc_core;

template<typename vci_param>
class VciMultiDma
  : public sc_core::sc_module
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> 
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> 
{
    typedef typename vci_param::addr_t addr_t;
    typedef typename vci_param::data_t data_t;

private:

    // structural constants
    const uint32_t      m_srcid;                  // DMA SRCID
    const size_t        m_max_burst;	          // local buffer length (bytes)
    const size_t        m_channels;               // number of parallel channels

    soclib::common::Segment 	m_segment;        // segment associated to DMA

    // registers
    int                 m_state[8];               // channel state
    data_t              m_source[8];              // source buffer address
    data_t              m_destination[8];         // destination buffer address
    data_t              m_length[8];              // tranfer length (bytes)
    bool                m_irq_disabled[8];        // no IRQ when true
    bool                m_stop[8];                // channel desactivation request
    uint8_t             m_irq_value[8];	          // IRQ current value
    uint8_t*            m_vci_data_buf[8];        // local data buffer pointers  
    uint8_t*            m_vci_be_buf[8];          // local be buffer pointers  
    bool                m_rsp_received[8];        // VCI response available
    size_t              m_burst_length[8];        // actual burst length 

    bool                m_dma_activated;          // at least one channel active

    pdes_local_time*	m_pdes_local_time;        // local time pointer

    sc_core::sc_event   m_event;                  // event to wake-up the thread

    // VCI TRANSACTIONS (one per channel)
    tlm::tlm_generic_payload    m_vci_payload[8];
    tlm::tlm_phase              m_vci_phase[8];
    sc_core::sc_time            m_vci_time[8];
    soclib_payload_extension	m_vci_extension[8];
    
    // ACTIVITY MESSAGE
    tlm::tlm_generic_payload    m_activity_payload;
    tlm::tlm_phase              m_activity_phase;
    sc_core::sc_time            m_activity_time;
    soclib_payload_extension    m_activity_extension;
    
    // NULL MESSAGE
    tlm::tlm_generic_payload    m_null_payload;
    tlm::tlm_phase              m_null_phase;
    sc_core::sc_time            m_null_time;
    soclib_payload_extension    m_null_extension;
    
    // IRQ TRANSACTIONS (one per channel)
    tlm::tlm_generic_payload    m_irq_payload[8];
    tlm::tlm_phase              m_irq_phase[8];
    sc_core::sc_time            m_irq_time[8];

public:

    enum init_state_e 
    {
        STATE_IDLE,
        STATE_SUCCESS,
        STATE_ERROR_READ,
        STATE_ERROR_WRITE,
        STATE_READ_CMD,
        STATE_READ_RSP,
        STATE_WRITE_CMD,
        STATE_WRITE_RSP,
    };

    // Functions
    void execLoop();
    void send_null();
    void send_write( size_t channel );
    void send_read( size_t channel );
    void send_activity();
    void send_irq( size_t channel );
    bool all_channels_stopped();
    bool all_channels_idle();

    /////////////////////////////////////////////////////////////////////////////
    // Interface function to receive response on the VCI initiator port
    /////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum 	nb_transport_bw ( tlm::tlm_generic_payload   &payload,
                                          tlm::tlm_phase             &phase,  
                                          sc_core::sc_time           &time);   
    
    /////////////////////////////////////////////////////////////////////////////
    // Interface Function to receive response on the IRQ port
    /////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum 	irq_nb_transport_bw ( int                       id,
                                              tlm::tlm_generic_payload  &payload, 
                                              tlm::tlm_phase            &phase,  
                                              sc_core::sc_time          &time);  

    /////////////////////////////////////////////////////////////////////////////
    // Interface function to receive command on the VCI target port
    /////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum 	nb_transport_fw ( tlm::tlm_generic_payload &payload,  
                                          tlm::tlm_phase           &phase,      
                                          sc_core::sc_time         &time);        
    
    /////////////////////////////////////////////////////////////////////////////
    // Not implemented but required by interface
    /////////////////////////////////////////////////////////////////////////////
    void b_transport ( tlm::tlm_generic_payload &payload, 
                       sc_core::sc_time         &time); 
    
    bool get_direct_mem_ptr ( tlm::tlm_generic_payload &payload,  
                              tlm::tlm_dmi             &dmi_data);  
    
    unsigned int transport_dbg ( tlm::tlm_generic_payload &payload); 
    
    void invalidate_direct_mem_ptr ( sc_dt::uint64 start_range,  
                                     sc_dt::uint64 end_range); 


protected:

    SC_HAS_PROCESS(VciMultiDma);
    
public:

    // ports
    tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types>     p_vci_initiator; 

    tlm::tlm_target_socket<32,tlm::tlm_base_protocol_types>         p_vci_target;

    std::vector<tlm_utils::simple_initiator_socket_tagged
    <VciMultiDma,32,tlm::tlm_base_protocol_types> *>                p_irq; 


    VciMultiDma( sc_module_name                     name,
                 const soclib::common::MappingTable &mt,
                 const soclib::common::IntTab       &srcid,
                 const soclib::common::IntTab       &tgtid,
                 const size_t                       max_burst_length,
                 const size_t                       channels );
};

}}

#endif /* SOCLIB_VCI_DMA_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

