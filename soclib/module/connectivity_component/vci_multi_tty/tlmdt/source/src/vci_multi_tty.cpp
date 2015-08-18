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
 * Maintainers: alain
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

//#define SOCLIB_MODULE_DEBUG 1

#include "vci_multi_tty.h"

namespace soclib { namespace tlmdt {

#define tmpl(x) template<typename vci_param> x VciMultiTty<vci_param>

//////////////////////////////////////////////////////
// constructors
///////////////////////////////////////////////////////
tmpl(/**/)::VciMultiTty ( sc_core::sc_module_name name,
                          const soclib::common::IntTab &index,
                          const soclib::common::MappingTable &mt,
                          const char *first_name, ...)
    : sc_core::sc_module(name)
    , m_segment(mt.getSegment(index))
    , p_vci("p_vci")
{
    va_list va_tty;
    va_start (va_tty, first_name);
    std::vector<std::string> names;
    const char *cur_tty = first_name;
    while (cur_tty) 
    {
        names.push_back(cur_tty);
        cur_tty = va_arg( va_tty, char * );
    }
    va_end( va_tty );
    init( names );
}

///////////////////////////////////////////////////////
tmpl(/**/)::VciMultiTty ( sc_core::sc_module_name name,
                          const soclib::common::IntTab &index,
                          const soclib::common::MappingTable &mt,
                          const std::vector<std::string> &names)
    : sc_core::sc_module(name)
    , m_segment(mt.getSegment(index))
    , p_vci("p_vci")
{
    init( names );
}

//////////////////////////////////////////////////////////
tmpl(void)::init( const std::vector<std::string>  &names )
//////////////////////////////////////////////////////////
{
    int	j = 0;

    // allocate terminals and IRQ ports
    for( std::vector<std::string>::const_iterator i = names.begin() ; 
         i != names.end() ; ++i )
    {
        m_term.push_back(soclib::common::allocateTty(*i));

        std::ostringstream irq_name;
        irq_name << "p_irq_" << j;
        p_irq.push_back(new tlm_utils::simple_initiator_socket_tagged<VciMultiTty,32,
                            tlm::tlm_base_protocol_types> (irq_name.str().c_str()));
        j++;
    }

    // bind VCI target port
    p_vci(*this);

    // allocate IRQ variables for all channels
    m_irq_value   = new uint8_t[m_term.size()];
    m_irq_payload = new tlm::tlm_generic_payload[m_term.size()];
    m_irq_phase   = new tlm::tlm_phase[m_term.size()];
    m_irq_time    = new sc_core::sc_time[m_term.size()];

    // initialise IRQ variables and bind IRQ ports
    for ( size_t channel=0 ; channel<m_term.size() ; channel++ ) 
    {  
        m_irq_value[channel]   = 0; 
        m_irq_phase[channel]   = tlm::BEGIN_REQ;
        m_irq_payload[channel].set_data_ptr(&m_irq_value[channel]); 

        (*p_irq[channel]).register_nb_transport_bw( this, 
                                                    &VciMultiTty::irq_nb_transport_bw, 
                                                    channel );
    }

    // initialize instrumentation counters
    m_cpt_read  = 0;
    m_cpt_write = 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// interface function executed when receiving a command on the VCI port
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw ( tlm::tlm_generic_payload &payload,
                                            tlm::tlm_phase           &phase,  
                                            sc_core::sc_time         &time )   
{
    soclib_payload_extension *extension_pointer;
    payload.get_extension(extension_pointer);
  
    // two actions in case of a NULL message:
    // - character acquisition on the terminals
    // - update IRQ values on the IRQ ports
    if( extension_pointer->is_null_message() )
    {

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = " << std::dec << time.value()
          << " RECEIVE NULL MESSAGE" << std::endl;
#endif

        for ( size_t channel=0 ; channel<m_term.size() ; ++channel ) 
        {
            if ( m_term[channel]->hasData() )  m_irq_value[channel] = 0xFF;
            else                               m_irq_value[channel] = 0x00;

            m_irq_time[channel] = time;

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = " << std::dec << time.value()
          << " SEND  IRQ / terminal = " << channel
          << " / value = " << (int)m_irq_payload[channel].get_data_ptr()[0] 
          << std::endl;
#endif

            (*p_irq[channel])->nb_transport_fw( m_irq_payload[channel], 
                                                m_irq_phase[channel], 
                                                m_irq_time[channel] );
        }    
        return tlm::TLM_COMPLETED;
    } // end NULL

    // two actions in case of a VCI command 
    // - execute the command  
    // - send a response 
    else 
    {
        size_t 	cell;
        size_t  reg;        // register index
        size_t  term;       // terminal index
        bool	one_flit = (payload.get_data_length() == (unsigned int)vci_param::nbytes);
        addr_t	address  = payload.get_address();
 
        // address and length checking for a VCI command
        if ( m_segment.contains(address) && one_flit )
        {
            cell = (size_t)((address - m_segment.baseAddress()) / vci_param::nbytes);
            reg  = cell % TTY_SPAN;
            term = cell / TTY_SPAN;
            payload.set_response_status(tlm::TLM_OK_RESPONSE);

	        assert ( (term < m_term.size() ) &&
            " Illegal TTY access : terminal index larger than number of terminals");
	    
            if ( extension_pointer->get_command() == VCI_READ_COMMAND )
            {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "[" << name() <<"] time = " << std::dec << time.value()
          << " RECEIVE READ COMMAND : term = "<< term << " / reg = " << reg 
          << std::endl;
#endif
                if ( reg == TTY_STATUS ) 
                {
                    utoa(m_term[term]->hasData(), payload.get_data_ptr(),0);
                }
                else if ( reg == TTY_READ )
                {
                    utoa(m_term[term]->getc(), payload.get_data_ptr(),0);
 	            }
	            else payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
	            m_cpt_read++;
            } // end read
            else if ( extension_pointer->get_command() == VCI_WRITE_COMMAND )
            {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "[" << name() <<"] time = " << std::dec << time.value()
          << " RECEIVE WRITE COMMAND : term = "<< term << " / reg = " << reg 
          << std::endl;
#endif
	            if ( reg == TTY_WRITE )
                {
	                if ( payload.get_data_ptr()[0] == '\a' ) 
                    {
		                char tmp[32];
		                size_t ret = snprintf( tmp, 
                                               sizeof(tmp), 
                                               "[%d] ", 
                                               (int)time.value());
		                for ( size_t i=0 ; i<ret ; ++i ) m_term[term]->putc( tmp[i] );
	                } 
	                else
                    {
		                m_term[term]->putc( payload.get_data_ptr()[0] );
	                }
 	            }
	            else
                {
                    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
	            } 
	            m_cpt_write++;
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
        time  = time + UNIT_TIME;
        phase = tlm::BEGIN_RESP;
        p_vci->nb_transport_bw( payload, 
                                phase, 
                                time );

        return tlm::TLM_COMPLETED;
    } // end VCI
} 

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response to an IRQ transaction
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::irq_nb_transport_bw ( int                        id,
                                                 tlm::tlm_generic_payload   &payload,
                                                 tlm::tlm_phase             &phase,
                                                 sc_core::sc_time           &time ) 
{
    // no action
    return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Instrumentation  Functions
/////////////////////////////////////////////////////////////////////////////////////
tmpl(size_t)::getNRead()
{
    return m_cpt_read;
}

tmpl(size_t)::getNWrite()
{
    return m_cpt_write;
}

tmpl(void)::print_stats()
{
    std::cout << name() << std::endl;
    std::cout << "- READ               = " << m_cpt_read << std::endl;
    std::cout << "- WRITE              = " << m_cpt_write << std::endl;
}

/////////////////////////////////////////////////////////////
// Not implemented but required by interface
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


