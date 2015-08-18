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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2011
 */

#include <stdint.h>
#include <iostream>
#include "register.h"
#include "../include/vci_dma.h"
#include <fcntl.h>
#include "dma.h"

namespace soclib { namespace tlmdt {

#define tmpl(t) template<typename vci_param> t VciDma<vci_param>

/////////////////////////
tmpl (void)::send_write()
{
    m_pdes_local_time->reset_sync();
    
    // send VCI command
    uint32_t length;
    if ( m_length < m_max_burst_length )	length = m_length;
    else					length = m_max_burst_length;
    m_vci_payload.set_address(m_destination & ~3);
    m_vci_payload.set_byte_enable_length(length);
    m_vci_payload.set_data_length(length);
    m_vci_extension.set_write();
    m_vci_time = m_pdes_local_time->get();
    p_vci_initiator->nb_transport_fw(m_vci_payload, m_vci_phase, m_vci_time);

    // send IRQ 
    m_irq_time = m_pdes_local_time->get();
    p_irq->nb_transport_fw(m_irq_payload, m_irq_phase, m_irq_time);

    // registers update
    m_length            = m_length - length;
    m_source            = m_source + length;
    m_destination       = m_destination + length;

#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_vci_time.value() 
          << std::hex << " WRITE COMMAND / address = " << m_destination 
          << " IRQ value = " << std::dec << (int)m_irq_value << std::endl;
#endif
}

////////////////////////
tmpl (void)::send_read()
{
    m_pdes_local_time->reset_sync();

    // send VCI read command
    uint32_t length;
    if ( m_length < m_max_burst_length )        length = m_length; 
    else                                        length = m_max_burst_length;
    m_vci_payload.set_address(m_source & ~3);
    m_vci_payload.set_data_length(length);
    m_vci_payload.set_byte_enable_length(length);
    m_vci_extension.set_read();
    m_vci_time = m_pdes_local_time->get();
    p_vci_initiator->nb_transport_fw(m_vci_payload, m_vci_phase, m_vci_time);

    // send IRQ 
    m_irq_time = m_pdes_local_time->get();
    p_irq->nb_transport_fw(m_irq_payload, m_irq_phase, m_irq_time);

#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_vci_time.value()
          << std::hex << " READ  COMMAND / address = " << m_source 
          << " / IRQ value = " << std::dec << (int)m_irq_value << std::endl;
#endif
}

////////////////////////
tmpl (void)::send_null()
{
    m_pdes_local_time->reset_sync();

    // send NULL message
    m_null_time = m_pdes_local_time->get();
    p_vci_initiator->nb_transport_fw(m_null_payload, m_null_phase, m_null_time);

    // send IRQ
    m_irq_time = m_pdes_local_time->get();
    p_irq->nb_transport_fw(m_irq_payload, m_irq_phase, m_irq_time);

#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_null_time.value()
          << " NULL MESSAGE / IRQ value = " << (int)m_irq_value << std::endl;
#endif
}

////////////////////////////////////////////
// thread associated to the initiator
////////////////////////////////////////////
tmpl (void)::execLoop ()
{
    while(true)
    {
        m_pdes_local_time->add(UNIT_TIME);

        switch ( m_state ) {
        case STATE_IDLE:
        {
            if ( !m_stop ) m_state = STATE_READ;
            else if ( m_pdes_local_time->need_sync() )
            {
                send_null();
                wait(m_rsp_received);
            }
        } 
        break;
        case STATE_READ:
        {
            // blocking command
            send_read();
            wait(m_rsp_received);

            // response analysis
            if ( !m_vci_payload.is_response_error() )
            {
                m_state = STATE_WRITE;
            }
            else  			
            {
                m_state = STATE_ERROR_READ; 
                if ( !m_irq_disabled )	m_irq_value = 0xFF;
            }
        }
        break;
        case STATE_WRITE:
        {
            // blocking command
            send_write();
            wait(m_rsp_received);

            // response analysis
            if ( m_stop ) 		
            {
                m_state = STATE_IDLE;
            }
            else if ( m_vci_payload.is_response_error() )
            {
                m_state = STATE_ERROR_WRITE;
                if ( !m_irq_disabled )	m_irq_value = 0xFF;
                m_irq_time = m_pdes_local_time->get();
                p_irq->nb_transport_fw(m_irq_payload, m_irq_phase, m_irq_time);
            }
            else if ( m_length == 0 )
            {
                m_state = STATE_SUCCESS;
                if ( !m_irq_disabled )	m_irq_value = 0xFF;
                m_irq_time = m_pdes_local_time->get();
                p_irq->nb_transport_fw(m_irq_payload, m_irq_phase, m_irq_time);
            }
            else
            {
                m_state = STATE_READ;
            }
        }
        break;
        case STATE_ERROR_READ:
        case STATE_ERROR_WRITE:
        case STATE_SUCCESS:
        {
            if ( m_stop ) 
            {
                m_state = STATE_IDLE;
                m_irq_value = 0x00;
                m_irq_time = m_pdes_local_time->get();
                p_irq->nb_transport_fw(m_irq_payload, m_irq_phase, m_irq_time);
            }
            else if ( m_pdes_local_time->need_sync() )
            {
                send_null();
                wait(m_rsp_received);
            }
        }
        } // end switch
    } // end while
} // end execLoop

////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response on the initiator port
////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::nb_transport_bw ( tlm::tlm_generic_payload &payload, 
                                             tlm::tlm_phase           &phase,       
                                             sc_core::sc_time         &time)  
{
    // update local time and notify
    if(time > m_pdes_local_time->get()) m_pdes_local_time->set(time);
    m_rsp_received.notify (sc_core::SC_ZERO_TIME);
    return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response to an IRQ transaction
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::irq_nb_transport_bw ( tlm::tlm_generic_payload    &payload,
                                                 tlm::tlm_phase              &phase,
                                                 sc_core::sc_time            &time) 
{
    // no action
    return tlm::TLM_COMPLETED;
}

///////////////////////////////////////////////////////////////////////////////
// Interface function used when receiving a VCI command on the target port
///////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw ( tlm::tlm_generic_payload &payload,
                                            tlm::tlm_phase           &phase, 
                                            sc_core::sc_time         &time)   
{
    int 	cell;
    bool	one_flit = (payload.get_data_length() == (unsigned int)vci_param::nbytes);
    addr_t	address = payload.get_address();

    soclib_payload_extension* extension_pointer;
    payload.get_extension(extension_pointer);
    
    // No response and no action for a received NULL messages
    if ( extension_pointer->is_null_message() ) return tlm::TLM_COMPLETED;

    // address and length checking for a VCI command
    if ( m_segment.contains(address) && one_flit )
    {
        cell = (int)((address - m_segment.baseAddress()) / vci_param::nbytes);
        payload.set_response_status(tlm::TLM_OK_RESPONSE);

        if (extension_pointer->get_command() == VCI_READ_COMMAND)
        {
                    
#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << time.value()
          << " STATUS REQUEST / " << cell << std::endl;
#endif

            if      ( cell == DMA_SRC )          utoa(m_source, payload.get_data_ptr(), 0);
            else if ( cell == DMA_DST )          utoa(m_destination, payload.get_data_ptr(), 0);
            else if ( cell == DMA_LEN )          utoa(m_state, payload.get_data_ptr(), 0);
            else if ( cell == DMA_IRQ_DISABLED ) utoa((int)(m_irq_disabled), payload.get_data_ptr(), 0);
            else                                 payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        } // end read

        else if (extension_pointer->get_command() == VCI_WRITE_COMMAND)
        {

#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << time.value() 
          << " CONFIGURATION REQUEST / " << cell << std::endl;
#endif

            // configuration command are ignored when the DMA is active
            if ( (m_state != STATE_READ) && (m_state != STATE_WRITE) )
            {
                data_t data = atou(payload.get_data_ptr(), 0);
                if      ( cell == DMA_SRC )  m_source = data;
                else if ( cell == DMA_DST )  m_destination = data;
                else if ( cell == DMA_LEN ) 
                {
                    m_length = data;
                    m_stop = false;
                }
                else if ( cell == DMA_IRQ_DISABLED )  m_irq_disabled = (data != 0);
                else if ( cell == DMA_RESET )  m_stop = true; 
                else    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            }
            else
            {
                std::cerr << name() 
                << " warning: receiving a new command while busy, ignored" << std::endl;
            }
        } // end if write
        else // illegal command
        {
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        }
    } // end if legal address 
    else // illegal address
    {
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    }

    // update transaction phase and time
    phase = tlm::BEGIN_RESP;
    if ( time.value() < m_pdes_local_time->get().value() ) time  = m_pdes_local_time->get();
    else                                                   time  = time + UNIT_TIME;

    // send response 
    p_vci_target->nb_transport_bw(payload, phase, time);
    return tlm::TLM_COMPLETED;
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

///////////////////
// constructor
///////////////////
tmpl(/**/)::VciDma( sc_module_name name,
                    const soclib::common::MappingTable &mt,
                    const soclib::common::IntTab &srcid,
                    const soclib::common::IntTab &tgtid,
                    const size_t max_burst_length)
           : sc_core::sc_module(name)
          , m_srcid(mt.indexForId(srcid))
          , m_max_burst_length(max_burst_length)
          , m_segment(mt.getSegment(tgtid))
          , p_vci_initiator("p_vci_init") 
          , p_vci_target("p_vci_tgt")    
          , p_irq("p_irq")     
{
    // bind vci initiator port
    p_vci_initiator(*this);                     
    
    // bind vci target port
    p_vci_target(*this);                     
    
    // register irq initiator port
    p_irq.register_nb_transport_bw(this, &VciDma::irq_nb_transport_bw);
    
    // initialize PDES local time
    m_pdes_local_time = new pdes_local_time(100*UNIT_TIME);
    
    // create and initialize the local buffers for VCI transactions (N bytes)
    m_vci_data_buf = new unsigned char[max_burst_length];
    m_vci_be_buf = new unsigned char[max_burst_length];
    for ( size_t i=0 ; i<max_burst_length ; i++) m_vci_be_buf[i] = 0xFF;

    // initialize payload, phase and extension for a VCI transaction
    m_vci_payload.set_command(tlm::TLM_IGNORE_COMMAND);
    m_vci_payload.set_data_ptr(m_vci_data_buf);
    m_vci_payload.set_byte_enable_ptr(m_vci_be_buf);
    m_vci_payload.set_extension(&m_vci_extension);
    m_vci_extension.set_src_id(m_srcid);
    m_vci_extension.set_trd_id(0);
    m_vci_extension.set_pkt_id(0);
    m_vci_phase = tlm::BEGIN_REQ;

    // initialize payload, phase and extension for a null message
    m_null_payload.set_extension(&m_null_extension);
    m_null_extension.set_null_message();
    m_null_phase = tlm::BEGIN_REQ;

    // initialize payload and phase for an irq message
    m_irq_payload.set_data_ptr(&m_irq_value);
    m_irq_phase = tlm::BEGIN_REQ;

    // register initialisation 
    m_state = STATE_IDLE;
    m_stop  = true;
    m_irq_disabled = false;
    m_irq_value = 0;
    
    SC_THREAD(execLoop);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// cle-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

