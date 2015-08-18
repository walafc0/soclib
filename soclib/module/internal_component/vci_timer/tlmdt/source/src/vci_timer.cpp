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
 * Copyright (c) UPMC / Lip6, 2011
 *     Alain.Greiner@lip6.fr
 */

#include "vci_timer.h"

//#define SOCLIB_MODULE_DEBUG 1

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciTimer<vci_param>

// TIMER_VALUE    : R/W
// TIMER_MODE     : R/W
// TIMER_PERIOD   : R/W
// TIMER_RESETIRQ : R/W

//////////////////////////////////
// constructor
/////////////////////////////////
tmpl(/**/)::VciTimer ( sc_core::sc_module_name name,
                       const soclib::common::IntTab &index,
                       const soclib::common::MappingTable &mt,
                       size_t ntimer)
	   : sc_core::sc_module(name),
	   m_segment(mt.getSegment(index) ),
	   m_timers(ntimer),
           m_local_time(sc_core::SC_ZERO_TIME),
	   p_vci("p_vci")  
{
    // bind VCI port
    p_vci(*this);                     

    // create & initialize timers
    m_timer_mode    = new int[m_timers];
    m_timer_period  = new int[m_timers];
    m_timer_value   = new int[m_timers];
    m_timer_counter = new int[m_timers];
    m_irq_value     = new uint8_t[m_timers];
    for(size_t i=0 ; i<m_timers ; i++)
    {
        m_timer_mode[i]    = 0;
        m_timer_period[i]  = 0;
        m_timer_value[i]   = 0;
        m_timer_counter[i] = 0;
        m_irq_value[i]     = 0;
    } 

    // allocate IRQ ports
    for(size_t i=0 ; i<m_timers ; i++)
    {
        std::ostringstream irq_name;
        irq_name << "irq" << i;
        p_irq.push_back(new tlm_utils::simple_initiator_socket_tagged<VciTimer,32,
                        tlm::tlm_base_protocol_types>(irq_name.str().c_str()));
    }

    // create and initialize IRQ transaction
    m_irq_payload = new tlm::tlm_generic_payload[m_timers];
    for ( size_t i=0 ; i<m_timers ; i++) m_irq_payload[i].set_data_ptr(&m_irq_value[i]);
    m_irq_phase = tlm::BEGIN_REQ;

    // register interface function on irq ports
    for ( size_t i=0 ; i<m_timers ; i++ )
    {
        (*p_irq[i]).register_nb_transport_bw(this, &VciTimer::irq_nb_transport_bw, i);
    }
}

tmpl(/**/)::~VciTimer(){}

/////////////////////////////////////////////////////////////////////////////////////
// Interface Function executed when receiving a command on the VCI port
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw ( tlm::tlm_generic_payload &payload, 
                                            tlm::tlm_phase           &phase, 
                                            sc_core::sc_time         &time)  
{
    assert ( ((int)time.value() < 0x7FFFFFFF) &&
             "Error in vci_timer : receiving a command with a time larger than 0x7FFFFFFF");

    soclib_payload_extension *extension_pointer;
    payload.get_extension(extension_pointer);

    // Four actions are executed for a NULL message
    // - increment/decrement all activated timer counters
    // - set interrupts when enabled & required by the timer counter
    // - update IRQ values on the IRQ ports
    // - update local time 
    if ( extension_pointer->is_null_message() )
    {

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << std::dec << time.value() 
          << " RECEIVE NULL MESSAGE" 
          << " mode = " << m_timer_mode[0] 
          << " / period = " << m_timer_period[0]
          << " / value = " << m_timer_value[0] 
          << " / counter = " << m_timer_counter[0] << std::endl;
#endif

        for ( size_t i=0 ; i<m_timers ; i++)
        {
            if ( m_timer_mode[i] & TIMER_RUNNING )
            {
                int	increment = (int)time.value() - (int)m_local_time.value();
                m_timer_value[i] += increment;
                m_timer_counter[i] -= increment;
        
                if ( (m_timer_counter[i] < 0) && (m_timer_mode[i] & TIMER_IRQ_ENABLED) ) 
                {
                    m_timer_counter[i] += m_timer_period[i];
                    m_irq_value[i] = 0xFF;
                }
            }

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = " << std::dec << time.value()
          << " SEND  IRQ for timer = " << i
          << " / value = " << (int)m_irq_payload[i].get_data_ptr()[0] << std::endl;
#endif

            (*p_irq[i])->nb_transport_fw(m_irq_payload[i], m_irq_phase, m_local_time);
        }

        if ( m_local_time < time ) m_local_time = time;

        return tlm::TLM_COMPLETED;
    } // end if null message

    // two actions in case of a VCI command
    // - execute the command
    // - send a response
    else
    {
        int	cell;
        int	reg;		// register index
        int	index;		// timer index
        bool	one_flit = (payload.get_data_length() == (unsigned int)vci_param::nbytes);
        addr_t	address  = payload.get_address();

        // address and length checking
        if ( m_segment.contains(address) && one_flit )
        {
            cell  = (int)((address - m_segment.baseAddress()) / vci_param::nbytes);
	    reg   = cell % TIMER_SPAN;
	    index = cell / TIMER_SPAN;
            payload.set_response_status(tlm::TLM_OK_RESPONSE);

            assert ( (index < (int)m_timers) &&
                   " Illegal access to TIMER : timer index larger than the number of timers");

            if ( extension_pointer->get_command() == VCI_READ_COMMAND )
            {

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << std::dec << time.value() 
          << " RECEIVE READ COMMAND : timer = " << index << " / reg = " << reg << std::endl;
#endif
                switch (reg) {
	        case TIMER_VALUE:
	            utoa(m_timer_value[index], payload.get_data_ptr(), 0);
	            break;
	        case TIMER_PERIOD:
	            utoa(m_timer_period[index], payload.get_data_ptr(), 0);
	            break;
	        case TIMER_MODE:
	            utoa(m_timer_mode[index], payload.get_data_ptr(), 0);
	            break;
	        case TIMER_RESETIRQ:
	            utoa((int)m_irq_value[index], payload.get_data_ptr(), 0); 
                    break;
                default:
                    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
                    break;
	        }
            } // end read
            else if ( extension_pointer->get_command() == VCI_WRITE_COMMAND )
            {

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << std::dec << time.value() 
          << " RECEIVE WRITE COMMAND : timer = " << index << " / reg = " << reg << std::endl;
#endif
	        switch (reg) {
	        case TIMER_VALUE:
	            m_timer_value[index] = (int)atou(payload.get_data_ptr(), 0);
	            break;
	        case TIMER_MODE:
	            m_timer_mode[index] = (int)atou(payload.get_data_ptr(), 0);
	            break;
	        case TIMER_PERIOD:
	            m_timer_period[index] = (int)atou(payload.get_data_ptr(), 0);
	            m_timer_counter[index] = (int)atou(payload.get_data_ptr(), 0);
	            break;
	        case TIMER_RESETIRQ:
                    m_irq_value[index] = 0x00;
                    break;
                default:
                    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
                    break;
	        }
            } // end write
            else // illegal command
            {
                payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            }
        }
        else // illegal address
        {
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        }

        //update the transaction time & send the response
        time = time + UNIT_TIME;
        phase = tlm::BEGIN_RESP;
        p_vci->nb_transport_bw(payload, phase, time);

        return tlm::TLM_COMPLETED;
    } // end VCI
} // end nb_transport_fw

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response to an IRQ transaction
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::irq_nb_transport_bw ( int                        id,
                                                 tlm::tlm_generic_payload    &payload,
                                                 tlm::tlm_phase              &phase,
                                                 sc_core::sc_time            &time)
{
    // no action
    return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////
// Not implemented but mandatory
/////////////////////////////////////////////////////////////
tmpl(void)::b_transport ( tlm::tlm_generic_payload &payload,
                          sc_core::sc_time         &_time)
{
    return;
}
tmpl(bool)::get_direct_mem_ptr ( tlm::tlm_generic_payload &payload,
                                 tlm::tlm_dmi             &dmi_data)
{
    return false;
}
tmpl(unsigned int):: transport_dbg ( tlm::tlm_generic_payload &payload)
{
    return false;
}
tmpl(void)::invalidate_direct_mem_ptr ( sc_dt::uint64 start_range,
                                        sc_dt::uint64 end_range)
{
    return;
}
tmpl (tlm::tlm_sync_enum)::nb_transport_bw ( tlm::tlm_generic_payload &payload,
                                             tlm::tlm_phase           &phase,
                                             sc_core::sc_time         &time)
{
    return tlm::TLM_COMPLETED;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// cle-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

