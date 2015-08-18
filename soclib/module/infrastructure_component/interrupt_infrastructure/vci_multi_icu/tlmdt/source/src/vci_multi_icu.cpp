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
 * Maintainers: alain.greiner@lip6.fr
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

#include <limits>
#include "vci_multi_icu.h"

#define SOCLIB_MODULE_DEBUG 1

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciMultiIcu<vci_param>

///////////////////////////////////////////////////////////////////
// constructor
///////////////////////////////////////////////////////////////////
tmpl(/**/)::VciMultiIcu ( sc_core::sc_module_name            name,
                          const soclib::common::IntTab       &index,
                          const soclib::common::MappingTable &mt,
                          size_t                             nirq_in,
                          size_t                             nirq_out )
	   : sc_module(name),             
	   m_tgtid(index),
       m_segment(mt.getSegment(index)),
	   m_nirq_in(nirq_in),
	   m_nirq_out(nirq_out),
	   p_vci("vci_target_socket")
{
    // initializes PDES local time
    m_pdes_local_time = new pdes_local_time(0*UNIT_TIME);

    assert( (nirq_in <= 32) && "The number of input IRQs cannot be larger than 32");
    assert( (nirq_out <= 8) && "The number of output IRQs cannot be larger than 8");
      
    // bind VCI port
    p_vci(*this);                     

    // Allocate and bind IRQ_IN[i] ports
    // Initialize variables and registers for each IRQ_IN[i] ports
    for(size_t i=0; i<m_nirq_in; i++)
    {
        m_irq_in_value[i] = 0;
        m_irq_in_time[i]  = sc_core::SC_ZERO_TIME;
    
        std::ostringstream name;
        name << "p_irq_in_" << i;
        p_irq_in.push_back(new tlm_utils::simple_target_socket_tagged
                               <VciMultiIcu,32,tlm::tlm_base_protocol_types>
                               (name.str().c_str()));
    
        p_irq_in[i]->register_nb_transport_fw( this,
                                               &VciMultiIcu::irq_nb_transport_fw, 
                                               i );
    }

    // Allocate and bind IRQ_OUT[channel] ports
    // Initialize variables and registers for each channel
    for(size_t channel=0; channel<m_nirq_out; channel++)
    {
        std::ostringstream name;
        name << "p_irq_out_" << channel;
        p_irq_out.push_back(new tlm_utils::simple_initiator_socket_tagged
                                <VciMultiIcu,32,tlm::tlm_base_protocol_types>
                                (name.str().c_str()));

        (*p_irq_out[channel]).register_nb_transport_bw( this,
                                                        &VciMultiIcu::irq_nb_transport_bw, 
                                                        channel );

        m_irq_mask[channel]      = 0;
        m_irq_out_value[channel] = 0;
        m_irq_out_payload[channel].set_data_ptr(&m_irq_out_value[channel]);
        m_irq_out_phase[channel] = tlm::BEGIN_REQ;
    }
    
    SC_THREAD(execLoop);
}
    
tmpl(/**/)::~VciMultiIcu(){}

///////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a command on the VCI port
// AS the dated transactions on the VCI port are the only way to update the ICU local
// time, this component requires periodical NULL messages from the interconnect.
///////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw ( tlm::tlm_generic_payload &payload,
                                            tlm::tlm_phase           &phase,  
                                            sc_core::sc_time         &time)   
{
    size_t  cell;
    size_t  reg;
    size_t  channel;

    soclib_payload_extension *extension_pointer;
    payload.get_extension(extension_pointer);

    // All received messages are used to update local time
    m_pdes_local_time->set( time );

    // No other action on NULL messages
    if(extension_pointer->is_null_message())
    {
    
#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << time.value() 
          << " Receive NULL message" << std::endl;
#endif
        return tlm::TLM_COMPLETED;
    }

    // Checking address and packet length for a VCI command
    bool	one_flit = (payload.get_data_length() == 4);
    addr_t	address  = payload.get_address();

    if ( m_segment.contains(address) && one_flit )
    {
        cell    = (size_t)((address - m_segment.baseAddress()) >> 2);
        reg     = cell % ICU_SPAN;
        channel = cell / ICU_SPAN;

        // checking channel index overflow
        if ( channel < m_nirq_out ) payload.set_response_status(tlm::TLM_OK_RESPONSE);
        else             payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);

        if ( extension_pointer->get_command() == VCI_READ_COMMAND )
        {

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << time.value() 
          << " Receive VCI read command : address = " << std::hex << address
          << " / channel = " << std::dec << channel
          << " / reg = "  << reg << std::endl;
#endif

            if (reg == ICU_INT)
            {
                utoa(getActiveIrqs( channel ), payload.get_data_ptr(), 0);
            }
            else if (reg == ICU_IT_VECTOR)
            {
                utoa(getIrqIndex( channel ), payload.get_data_ptr(), 0);
            }
            else if (reg == ICU_MASK)
            {
                utoa(m_irq_mask[channel], payload.get_data_ptr(), 0);
            }
            else    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        }

        else if ( extension_pointer->get_command() == VCI_WRITE_COMMAND )
        {
            uint32_t data = atou(payload.get_data_ptr(), 0);

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << time.value() 
          << " Receive VCI write command : address = " << std::hex << address
          << " / channel = " << std::dec << channel
          << " / reg = "  << reg 
          << " / data = " << data << std::endl;
#endif
            if     (reg == ICU_MASK_SET )    m_irq_mask[channel] |= data;
            else if(reg == ICU_MASK_CLEAR )  m_irq_mask[channel] &= (~data);
            else    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        }
        else // illegal command
        {
	        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        }
    } 
    else  // illegal address
    {
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    }

    // update transaction phase and time
    phase = tlm::BEGIN_RESP;
    time = time + UNIT_TIME;

    // send response
    p_vci->nb_transport_bw(payload, phase, time);

    return tlm::TLM_COMPLETED;
} // end nb_transport_fw()

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response on the IRQ_OUT port
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::irq_nb_transport_bw ( int                      id,
                                                tlm::tlm_generic_payload &payload,     
                                                tlm::tlm_phase           &phase,     
                                                sc_core::sc_time         &time)     
{
    // No action
    return tlm::TLM_COMPLETED;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Interface function excuted when receiving a value on an IRQ_IN port.
// The value and the date are registered, and the thread is notified.
//////////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::irq_nb_transport_fw ( int                      id,         
                                                tlm::tlm_generic_payload &payload,  
                                                tlm::tlm_phase           &phase,   
                                                sc_core::sc_time         &time)  
{
    // register interruption
    unsigned char value = payload.get_data_ptr()[0];
    m_irq_in_value[id]	= value;
    m_irq_in_time[id]	= time;

    // notify to ICU thread
    m_irq_in_received.notify();
    

#if SOCLIB_MODULE_DEBUG
if ( value != m_irq_in_value[id] )
std::cout << "[" << name() << "] time = " << time
          << " Received IRQ_IN " << std::dec << id 
          << " with value = " << (int)value 
          << " at time " << time << std::endl;
#endif
      
    return tlm::TLM_COMPLETED;
}

//////////////////////////////////////////////////////////////////////////////
// Thread implementing a relaxed time filtering for all IRQ_OUT channels.
// All channel are handled independantly.
// This thread is activated each time an IRQ_IN[i] value is updated.
// It tries successively to send a new value on each IRQ_OUT[channel] port. 
//////////////////////////////////////////////////////////////////////////////
tmpl (void)::execLoop()
{
    while(true) 
    {

#if SOCLIB_MODULE_DEBUG
std::cout << "######    [" << name() << "] wake up / time = "
          << std::dec << m_pdes_local_time->get().value() << std::endl;
        
for(size_t i = 0 ; i < m_nirq_in ; i++) 
std::cout << "    - IRQ_IN_" << i 
          << " / date = " << m_irq_in_time[i].value() 
          << " / value = " << (int)m_irq_in_value[i] << std::endl; 
#endif

        // Loop on the channels
        for ( size_t channel = 0 ; channel < m_nirq_out ; channel++ )
        {
            unsigned char       new_value;
            sc_core::sc_time    new_time;

            if ( irqTransmissible( channel, &new_value, &new_time ) )
            {

#if SOCLIB_MODULE_DEBUG
std::cout << "    [" << name() << "] send IRQ_OUT " << channel 
          << " / date = " << m_irq_out_time[channel].value()
          << " / value = " << (int)m_irq_out_value[channel] << std::endl; 
#endif
                m_irq_out_value[channel] = new_value;
              	m_irq_out_time[channel]  = new_time;
                (*p_irq_out[channel])->nb_transport_fw( m_irq_out_payload[channel], 
                                                        m_irq_out_phase[channel], 
                                                        m_irq_out_time[channel] );

            } // end if transmissible
        } // end for channel     

#if SOCLIB_MODULE_DEBUG
std::cout << "######    [" << name() << "] deschedule / time = "
          << std::dec << m_pdes_local_time->get().value() << std::endl;
#endif 
        wait( m_irq_in_received );

    } // end while thread
}

///////////////////////////////////////////////////////////////////////////////////////
// This function checks if a new value should be transmitted on IRQ_OUT[channel]:
// - Only IRQ_IN[i] unmasked for this channel are taken into account.
// - Only significant events on IRQ_OUT[channel] (change of values) are transmitted.
// - Only events with a date smaller or equal to the ICU local time are handled 
//   (events in ICU component futur are registered, but not taken into account).
// - The date or the IRQ_OUT message is always the ICU local time.
// ////////////////////////////////////////////////////////////////////////////////////
tmpl(bool)::irqTransmissible( size_t             channel,
                              unsigned char*     new_value, 
                              sc_core::sc_time*  new_time )
{
    bool change_found;
    
    if ( m_irq_out_value[channel] )   // looking for 0->1 (at least one IRQ_IN active)
    {
        // initial value for loop on IRQ_IN[i]
        change_found = false;
        *new_value   = 1;

        for(size_t i = 0 ; i<m_nirq_in ; i++) 
        {
            if ( m_irq_mask[channel] & (1 << i) ) // only enabled IRQ_IN are involved 
            {
                if ( m_irq_in_value[i] and
                     (m_irq_in_time[i].value() <= m_pdes_local_time->get().value()) ) 
                         change_found = true;
            }
        }
    }
    else                              // looking for 1->0 (all IRQ_IN inactive)   
    {
        // initial value for loop on IRQ_IN[i]
        change_found = true;  
        *new_value   = 0;
        for(size_t i = 0 ; i<m_nirq_in ; i++)
        {
            if ( m_irq_mask[channel] & (1 << i) ) // only enabled IRQ_IN are involved 
            {
                if ( m_irq_in_value[i] and 
                     (m_irq_in_time[i].value() <= m_pdes_local_time->get().value()) ) 
                         change_found = false;
            }
        }
    }

    if ( change_found ) *new_time = m_pdes_local_time->get();
    return ( false );
}

///////////////////////////////////////////////////////////////////////////////
// This function returns a bit vector containing the unmasked and active IRQs
////////////////////////////////////////////////////////////////////////////////
tmpl(uint32_t)::getActiveIrqs( size_t channel )
{
    uint32_t irqs = 0;
    for ( size_t j=0 ; j<m_nirq_in ; j++) 
    {
        if ( (m_irq_mask[channel] & (1 << j)) and
             (m_irq_in_value[j] != 0) and 
             (m_irq_in_time[j].value() <= m_pdes_local_time->get().value()) )
                 irqs = irqs | (1 << j);
    }
    return irqs;
}

///////////////////////////////////////////////////////////////////////////////////
// This function returns index of the hignest priority unmasked and active IRQ 
///////////////////////////////////////////////////////////////////////////////////
tmpl(size_t)::getIrqIndex( size_t channel )
{
    for (size_t j=0 ; j<m_nirq_in ; j++) 
    {
        if ( (m_irq_mask[channel] & (1 << j)) and 
             (m_irq_in_value[j] != 0) and
             (m_irq_in_time[j].value() <= m_pdes_local_time->get().value()) )
                 return j;
    }
    return 32;
}

/////////////////////////////////////////////////////////////////////////////
// Functions not implemented but required by interface
/////////////////////////////////////////////////////////////////////////////
tmpl(void)::b_transport( tlm::tlm_generic_payload &payload,
                         sc_core::sc_time         &_time ) 
{ 
}

tmpl(bool)::get_direct_mem_ptr( tlm::tlm_generic_payload &payload, 
                                tlm::tlm_dmi             &dmi_data ) 
{ 
  return false;
}

tmpl(unsigned int):: transport_dbg( tlm::tlm_generic_payload &payload )  
{
  return false;
}

tmpl(void)::invalidate_direct_mem_ptr( sc_dt::uint64 start_range,   
                                       sc_dt::uint64 end_range ) 
{
}

///////////////////////////// No comprendo ////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_bw ( tlm::tlm_generic_payload  &payload,
                                            tlm::tlm_phase            &phase,
                                            sc_core::sc_time          &time )
{
    return tlm::TLM_COMPLETED;
}    

}}

