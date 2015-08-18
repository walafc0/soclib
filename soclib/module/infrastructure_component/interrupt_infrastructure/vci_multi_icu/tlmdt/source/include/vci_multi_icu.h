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
 * Maintainers: alain
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

///////////////////////////////////////////////////////////////////////////////////////
// Implementation note:
// This component is a multi-channels interrupt controller acting as a VCI target.
// - The number of input interrupts is variable, but cannot be larger than 32.
// - The number of output interrupts is variable, but cannot be larger than 8.  
//
// The target FSM is modeled as a purely reactive interface function.
//
// This component has a local time and require periodical NULL messages from the 
// interconnect to be synchronized, as the local time is only updated by
// message received on the VCI port (VCI commands or NULL messages).
//
// It contains one single sc_thread implementing a relaxed time filtering for
// all IRQ_OUT channels:
// - This thread is activated each time a message is received on a IRQ_IN[i] port.
// - This thread handles all output port IRQ_OUT[channel] independantly.
// - Only IRQ_IN[i] unmasked for a channel are taken into account for this channel.
// - Only significant events on IRQ_OUT (change of values) are transmitted.
// - Only events with a date smaller or equal to the ICU local time are handled
//   (events in ICU component futur are registered but not taken into account).
// - The date or the IRQ_OUT message iq always the ICU local time.
//
// It means that any event on an IRQ_IN port at a date smaller than the ICU
// local time will be handled only after the next NULL message. This introduce
// a systematic delay that is bounded by the NULL message period.
//////////////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_TLMDT_VCI_MULTI_ICU_H
#define SOCLIB_TLMDT_VCI_MULTI_ICU_H

#include <tlmdt>      
#include "mapping_table.h"
#include "multi_icu.h"

namespace soclib { namespace tlmdt {

template <typename vci_param>
class VciMultiIcu
  : public sc_core::sc_module             
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> 
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> 
{
private:
  
    typedef typename vci_param::addr_t addr_t;

    //////////////////////////////////////////////////////////////////////////////
    // Member Variables
    //////////////////////////////////////////////////////////////////////////////

    pdes_local_time*			m_pdes_local_time;   // local time pointer

    uint32_t                    m_irq_mask[8];       // masks for each channel 
    unsigned char               m_irq_out_value[8];  // current IRQ_OUT values

    unsigned char               m_irq_in_value[32];  // current IRQ_IN values
    sc_core::sc_time            m_irq_in_time[32];   // last IRQ_IN change dates

    soclib::common::IntTab      m_tgtid;
    soclib::common::Segment     m_segment;

    sc_core::sc_event			m_irq_in_received;

    size_t                      m_nirq_in;           // actual number of IRQ_IN
    size_t                      m_nirq_out;			 // actual number of IRQ_OUT

    // Fields for the IRQ_OUT[channel] transactions
    tlm::tlm_generic_payload    m_irq_out_payload[8];
    tlm::tlm_phase              m_irq_out_phase[8];
    sc_core::sc_time            m_irq_out_time[8];

    //////////////////////////////////////////////////////////////////////////////
    //  Functions
    //////////////////////////////////////////////////////////////////////////////
    bool     irqTransmissible( size_t             channel,
                               unsigned char*     new_value, 
                               sc_core::sc_time*  new_time );
    uint32_t getActiveIrqs( size_t channel );
    size_t   getIrqIndex( size_t channel );
    void     execLoop();

    //////////////////////////////////////////////////////////////////////////////////
    // Interface function executed when receiving a VCI command on p_vci 
    //////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_fw ( tlm::tlm_generic_payload &payload, 
                                         tlm::tlm_phase           &phase, 
                                         sc_core::sc_time         &time);  

    //////////////////////////////////////////////////////////////////////////////////
    // Interface function executed when receiving a response on p_irq_out[id]
    //////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum irq_nb_transport_bw ( int                      id,
                                             tlm::tlm_generic_payload &payload, 
                                             tlm::tlm_phase           &phase,
                                             sc_core::sc_time         &time);
  
    /////////////////////////////////////////////////////////////////////////////////////
    // Interface function executed when receiving and IRQ on p_irq_in[id]
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum irq_nb_transport_fw ( int                       id,       
                                             tlm::tlm_generic_payload  &payload,   
                                             tlm::tlm_phase            &phase,    
                                             sc_core::sc_time          &time );  

    /////////////////////////////////////////////////////////////////////////////////////
    // Not used but required by interface
    /////////////////////////////////////////////////////////////////////////////////////
    void b_transport ( tlm::tlm_generic_payload &payload,
                       sc_core::sc_time         &time);
  
    bool get_direct_mem_ptr ( tlm::tlm_generic_payload &payload,
                              tlm::tlm_dmi             &dmi_data);
  
    unsigned int transport_dbg ( tlm::tlm_generic_payload &payload); 

    void invalidate_direct_mem_ptr ( sc_dt::uint64 start_range, 
                                     sc_dt::uint64 end_range); 

    tlm::tlm_sync_enum nb_transport_bw ( tlm::tlm_generic_payload  &payload,
                                         tlm::tlm_phase            &phase,
                                         sc_core::sc_time          &time );

protected:

    SC_HAS_PROCESS(VciMultiIcu);

public:

    tlm::tlm_target_socket<32, tlm::tlm_base_protocol_types>        p_vci;   

    std::vector<tlm_utils::simple_initiator_socket_tagged
    <VciMultiIcu,32,tlm::tlm_base_protocol_types> *>                p_irq_out; 

    std::vector<tlm_utils::simple_target_socket_tagged
    <VciMultiIcu,32,tlm::tlm_base_protocol_types> *>                p_irq_in; 

    VciMultiIcu( sc_core::sc_module_name            name,
	             const soclib::common::IntTab       &index,
	             const soclib::common::MappingTable &mt,
	             size_t                             nirq_in,
                 size_t                             nirq_out );

    ~VciMultiIcu();
};

}}

#endif
