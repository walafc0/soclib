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
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

#include <limits>
#include "vci_icu.h"

//#define SOCLIB_MODULE_DEBUG 1

//ICU_INT = 0 read-only
//ICU_MASK = 1 read-only
//ICU_MASK_SET = 2 write-only
//ICU_MASK_CLEAR = 3 write-only
//ICU_IT_VECTOR = 4 read-only

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciIcu<vci_param>

//////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////
tmpl(/**/)::VciIcu ( sc_core::sc_module_name name,
                     const soclib::common::IntTab &index,
                     const soclib::common::MappingTable &mt,
                     size_t nirq )
	   : sc_module(name),             
	   m_tgtid(index),
           m_segment(mt.getSegment(index)),
	   m_nirq(nirq),
	   p_vci("vci_target_socket"),
	   p_irq("irq_init_socket")  
{
    // bind VCI port
    p_vci(*this);                     

    // bind IRQ port
    p_irq(*this);                     

    // initializes PDES local time
    m_pdes_local_time = new pdes_local_time(0*UNIT_TIME);

    assert( (m_nirq <= 32) && "The number of input IRQs cannot be larger than 32");
      
    for(size_t i=0; i<m_nirq; i++)
    {
        m_irq_in_enabled[i]  	= false;
        m_irq_in_value[i]  	= 0;
        m_irq_in_time[i] 	= sc_core::SC_ZERO_TIME;
    
        std::ostringstream irq_name;
        irq_name << "irqIn_" << i;
        p_irq_in.push_back(new tlm_utils::simple_target_socket_tagged<VciIcu,32,tlm::tlm_base_protocol_types>
             (irq_name.str().c_str()));
    
        p_irq_in[i]->register_nb_transport_fw(this, &VciIcu::irq_nb_transport_fw, i);
    }

    // set the constant values in IRQ payload
    m_irq_out_payload.set_data_ptr(&m_irq_out_value);
    m_irq_out_phase = tlm::BEGIN_REQ;

    SC_THREAD(execLoop);
}
    
tmpl(/**/)::~VciIcu(){}

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a command on the VCI port
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw
( tlm::tlm_generic_payload &payload,
  tlm::tlm_phase           &phase,  
  sc_core::sc_time         &time)   
{
    int 	cell;
    bool	one_flit = (payload.get_data_length() == (unsigned int)vci_param::nbytes);
    addr_t	address  = payload.get_address();

    soclib_payload_extension *extension_pointer;
    payload.get_extension(extension_pointer);

    // no response, and no action when receiving a NULL message
    if(extension_pointer->is_null_message()) return tlm::TLM_COMPLETED;
  
    // Checking address and packet length
    if ( m_segment.contains(address) && one_flit )
    {
        cell = (int)((address - m_segment.baseAddress()) / vci_param::nbytes);
	payload.set_response_status(tlm::TLM_OK_RESPONSE);

        if ( extension_pointer->get_command() == VCI_READ_COMMAND )
        {

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << time.value() 
          << " Receive a VCI read command = "  << cell << std::endl;
#endif

	    if      ( cell == ICU_INT)       utoa(getActiveIrqs(), payload.get_data_ptr(), 0);
	    else if ( cell == ICU_IT_VECTOR) utoa(getIrqIndex(), payload.get_data_ptr(), 0);
	    else if ( cell == ICU_MASK)      utoa(getMask(), payload.get_data_ptr(), 0);
	    else    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
	}
        else if ( extension_pointer->get_command() == VCI_WRITE_COMMAND )
        {
            uint32_t data = atou(payload.get_data_ptr(), 0);

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << time.value() 
          << " Receive a VCI write command = "  << cell << std::endl;
#endif
	    if      ( cell == ICU_MASK_SET ) setMask(data);
	    else if ( cell == ICU_MASK_CLEAR ) clearMask(data);
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
    p_vci->nb_transport_bw(payload, phase, time);
    return tlm::TLM_COMPLETED;
} // end nb_transport_fw()

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response on the IRQ port
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_bw  
( tlm::tlm_generic_payload &payload,     
  tlm::tlm_phase           &phase,     
  sc_core::sc_time         &time)     
{
    return tlm::TLM_COMPLETED;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Interface function excuted when receiving a value on an IRQ port identified by id
//////////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::irq_nb_transport_fw ( int                      id,         
                                                tlm::tlm_generic_payload &payload,  
                                                tlm::tlm_phase           &phase,   
                                                sc_core::sc_time         &time)  
{
    // register interruption if not masked
    if ( m_irq_in_enabled[id] )  
    {
        unsigned char	value = payload.get_data_ptr()[0];

#if SOCLIB_MODULE_DEBUG
if ( value != m_irq_in_value[id] )
std::cout << "[" << name() << "] time = " << time
          << " Received Interruption " << std::dec << id 
          << " with value = " << (int)value << std::endl;
#endif
      
        m_irq_in_value[id]	= value;
        m_irq_in_time[id]	= time;

        m_irq_received.notify();
    }
    return tlm::TLM_COMPLETED;
}

///////////////////////////////////////////////////////////////////////
//     PDES process
// Try to send an output IRQ value each time an input IRQ is updated.
// The thread local time is equal to the time of the registered IRQ
// that has the earliest time.
///////////////////////////////////////////////////////////////////////
tmpl (void)::execLoop()
{
    while(true) 
    {
        uint8_t			irq_value;
        sc_core::sc_time	new_time;

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = " << m_pdes_local_time->get().value()
          << " Pending IRQs :" << std::endl; 
for(size_t i = 0 ; i<m_nirq ; i++) 
std::cout << "      index = " << i << " / enabled = " << m_irq_in_enabled[i]
          << " / date = " << m_irq_in_time[i].value() 
          << " value = " << (int)m_irq_in_value[i] << std::endl; 
#endif

        if ( irqTransmissible( &irq_value, &new_time) )
        {
            m_pdes_local_time->set(new_time);
            if ( m_irq_out_value != irq_value )
            {
                m_irq_out_value = irq_value;
            	m_irq_out_time 	= m_pdes_local_time->get();
            	p_irq->nb_transport_fw(m_irq_out_payload, m_irq_out_phase, m_irq_out_time);

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = " << m_pdes_local_time->get().value()
          << " Send Interruption" << " with value = " << (int)irq_value << std::endl;
#endif
            } // end if new value
        } // end if transmissible
        wait( m_irq_received );
    } // end while thread
}

///////////////////////////////////////////////////////////////////////////////////////
// This function checks if an IRQ value should be transmitted :
// If there is at leat one enabled IRQ, and all enabled IRQs have a time larger than 
// the current ICU time, it returns OK : The value to be transmitted is the OR
// of all enabled IRQs, and the date is the smallest time of all enabled IRQs.
// ////////////////////////////////////////////////////////////////////////////////////
tmpl(bool)::irqTransmissible(unsigned char* irq_value, sc_core::sc_time* new_time)
{
    bool 	synchro_ok       = true;
    bool 	at_least_one     = false;

    *irq_value    	= 0;
    *new_time 		= 0xFFFFFFFF*UNIT_TIME; // assez moche... à améliorer

    for(size_t i = 0 ; i<m_nirq ; i++)
    {
        if ( m_irq_in_enabled[i] ) // only enabled IRQs are involved in time filtering 
        {
            at_least_one = true;
            if ( m_irq_in_time[i].value() > m_pdes_local_time->get().value() )
            {
                if ( m_irq_in_time[i].value() < new_time->value() ) *new_time = m_irq_in_time[i];
                *irq_value |= m_irq_in_value[i];
            }
            else
            {
                synchro_ok = false;
                break;
            }
        }
    }
    return (synchro_ok && at_least_one);
}

///////////////////////////////////////////////////////////////////////////////
// This function returns a bit vector containing the enabled active IRQs
////////////////////////////////////////////////////////////////////////////////
tmpl(uint32_t)::getActiveIrqs()
{
    uint32_t irqs = 0;
    for ( size_t j=0 ; j<m_nirq ; j++) 
    {
        if ( m_irq_in_enabled[j] && (m_irq_in_value[j] != 0) ) irqs = irqs | (1 << j);
    }
    return irqs;
}

///////////////////////////////////////////////////////////////////////////////////
// This function returns the index of the hignest priority enabled and active IRQ 
///////////////////////////////////////////////////////////////////////////////////
tmpl(size_t)::getIrqIndex()
{
    for (size_t j=0 ; j<m_nirq ; j++) 
    {
        if ( m_irq_in_enabled[j] && (m_irq_in_value[j] != 0) ) return j;
    }
    return 32;
}

////////////////////////////////////////////////////////////////////////////////////
//  returns the current mask value
////////////////////////////////////////////////////////////////////////////////////
tmpl(uint32_t)::getMask()
{
    uint32_t mask = 0;
    for (size_t j=0 ; j<m_nirq ; j++) 
    {
        if ( m_irq_in_enabled[j] ) mask |= (1 << j);
    }
    return mask;
}

////////////////////////////////////////////////////////////////////////////////////
// mask <= mask | data
////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::setMask( uint32_t data )
{
    for (size_t j=0 ; j<m_nirq ; j++) 
    {
        if ( data & (1 << j) ) m_irq_in_enabled[j] = true;
    }
}
////////////////////////////////////////////////////////////////////////////////////
// mask = mask & ~(data);
////////////////////////////////////////////////////////////////////////////////////
tmpl(void)::clearMask( uint32_t data )
{
    for (size_t j=0 ; j<m_nirq ; j++) 
    {
        if ( data & (1 << j) ) m_irq_in_enabled[j] = false;
    }
}
///////////////////////////////////////////////////////
// Functions not implemented but required by interface
tmpl(void)::b_transport
( tlm::tlm_generic_payload &payload,
  sc_core::sc_time         &_time) 
{
  return;
}

tmpl(bool)::get_direct_mem_ptr
( tlm::tlm_generic_payload &payload, 
  tlm::tlm_dmi             &dmi_data) 
{ 
  return false;
}

tmpl(unsigned int):: transport_dbg                            
( tlm::tlm_generic_payload &payload)  
{
  return false;
}

tmpl(void)::invalidate_direct_mem_ptr 
( sc_dt::uint64 start_range,   
  sc_dt::uint64 end_range  
) 
{ }

}}


