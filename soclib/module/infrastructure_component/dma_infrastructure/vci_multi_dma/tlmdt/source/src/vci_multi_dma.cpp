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
#include "../include/vci_multi_dma.h"
#include <fcntl.h>
#include "multi_dma.h"

namespace soclib { namespace tlmdt {

#define tmpl(t) template<typename vci_param> t VciMultiDma<vci_param>

///////////////////////////////////////////////////////////////////
//  constructor
///////////////////////////////////////////////////////////////////
tmpl(/**/)::VciMultiDma( sc_module_name                     name,
                         const soclib::common::MappingTable &mt,
                         const soclib::common::IntTab       &srcid,
                         const soclib::common::IntTab       &tgtid,
                         const size_t                       max_burst_length,
                         const size_t                       channels )
          : sc_core::sc_module(name)
          , m_srcid(mt.indexForId(srcid))
          , m_max_burst(max_burst_length)
          , m_channels(channels)
          , m_segment(mt.getSegment(tgtid))
          , p_vci_initiator("p_vci_init") 
          , p_vci_target("p_vci_tgt")    
{
    // initialize PDES local time
    m_pdes_local_time = new pdes_local_time(100*UNIT_TIME);
    
    assert( (channels <= 8) && "The number of channels cannot be larger than 8");
    assert( (max_burst_length <= 256) && "The burst length cannot be larger than 64");

    // bind vci initiator port
    p_vci_initiator(*this);                     
    
    // bind vci target port
    p_vci_target(*this);                     
    
    // initialize channels
    for( size_t channel = 0 ; channel < channels ; channel++ )
    {
        // bind IRQ[channel] ports
        std::ostringstream name;
        name << "p_irq_" << channel;
        p_irq.push_back(new tlm_utils::simple_initiator_socket_tagged
                                <VciMultiDma,32,tlm::tlm_base_protocol_types>
                                (name.str().c_str()));

        (*p_irq[channel]).register_nb_transport_bw( this,
                                                    &VciMultiDma::irq_nb_transport_bw, 
                                                    channel );

        // initialize payload and phase for IRQ transaction
        m_irq_payload[channel].set_data_ptr(&m_irq_value[channel]);
        m_irq_phase[channel] = tlm::BEGIN_REQ;
    
        // create and initialize the local buffers for VCI transactions 
        m_vci_data_buf[channel] = new unsigned char[max_burst_length];
        m_vci_be_buf[channel]   = new unsigned char[max_burst_length];

        // initialize payload, phase and extension for VCI transactions
        m_vci_payload[channel].set_command(tlm::TLM_IGNORE_COMMAND);
        m_vci_payload[channel].set_data_ptr(m_vci_data_buf[channel]);
        m_vci_payload[channel].set_byte_enable_ptr(m_vci_be_buf[channel]);
        m_vci_payload[channel].set_extension(&m_vci_extension[channel]);

        m_vci_extension[channel].set_src_id(m_srcid);
        m_vci_extension[channel].set_trd_id(channel);
        m_vci_extension[channel].set_pkt_id(0);

        m_vci_phase[channel] = tlm::BEGIN_REQ;

        // initialize payload and phase for an irq message
        m_irq_payload[channel].set_data_ptr(&m_irq_value[channel]);
        m_irq_phase[channel] = tlm::BEGIN_REQ;

        // channel state registers initialisation 
        m_state[channel]        = STATE_IDLE;
        m_stop[channel]         = true;
        m_irq_disabled[channel] = false;
        m_rsp_received[channel] = false;
        m_irq_value[channel]    = 0;
        for ( size_t i=0 ; i<max_burst_length ; i++) 
        {
            m_vci_data_buf[channel][i] = 0x0;
            m_vci_be_buf[channel][i]   = 0xFF;
        }
    }

    // initialize payload, phase and extension for a null message
    m_null_payload.set_extension(&m_null_extension);
    m_null_extension.set_null_message();
    m_null_phase = tlm::BEGIN_REQ;

    // initialize payload, and phase for an activity message
    m_activity_payload.set_extension(&m_activity_extension);
    m_activity_phase = tlm::BEGIN_REQ;

    SC_THREAD(execLoop);
}

//////////////////////////////////////////
tmpl (void)::send_write( size_t channel )
//////////////////////////////////////////
{
    m_pdes_local_time->reset_sync();
    
    // send VCI command
    m_vci_payload[channel].set_address(m_destination[channel] & ~3);
    m_vci_payload[channel].set_byte_enable_length(m_burst_length[channel]);
    m_vci_payload[channel].set_data_length(m_burst_length[channel]);
    m_vci_extension[channel].set_write();
    m_vci_time[channel] = m_pdes_local_time->get();

    p_vci_initiator->nb_transport_fw( m_vci_payload[channel], 
                                      m_vci_phase[channel], 
                                      m_vci_time[channel] );
#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_vci_time[channel].value() 
          << " / WRITE COMMAND for channel " << channel
          << " / address = " << std::hex << m_destination[channel]
          << " / length = " << std::dec << m_burst_length[channel] << std::endl;
#endif
}

////////////////////////////////////////
tmpl (void)::send_read( size_t channel )
////////////////////////////////////////
{
    m_pdes_local_time->reset_sync();

    // send VCI read command
    m_vci_payload[channel].set_address(m_source[channel] & ~3);
    m_vci_payload[channel].set_byte_enable_length(m_burst_length[channel]);
    m_vci_payload[channel].set_data_length(m_burst_length[channel]);
    m_vci_extension[channel].set_read();
    m_vci_time[channel] = m_pdes_local_time->get();

    p_vci_initiator->nb_transport_fw( m_vci_payload[channel], 
                                      m_vci_phase[channel], 
                                      m_vci_time[channel] );
#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_vci_time[channel].value()
          << " / READ  COMMAND for channel " << channel
          << " / address = " << std::hex << m_source[channel] 
          << " / length = " << std::dec << m_burst_length[channel] << std::endl;
#endif
}

////////////////////////
tmpl (void)::send_null()
////////////////////////
{
    m_pdes_local_time->reset_sync();

    // send NULL message
    m_null_time = m_pdes_local_time->get();

    p_vci_initiator->nb_transport_fw( m_null_payload, 
                                      m_null_phase, 
                                      m_null_time );

    // send IRQ : not useful if the ICU implement a relaxed time filtering
    // m_irq_time = m_pdes_local_time->get();
    // p_irq->nb_transport_fw( m_irq_payload, 
    //                         m_irq_phase, 
    //                         m_irq_time);

#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_null_time.value()
          << " / NULL MESSAGE " << std::endl;
#endif
}

/////////////////////////////
tmpl (void)::send_activity( )
/////////////////////////////
{
    m_pdes_local_time->reset_sync();

    // send ACTIVITY message
    m_activity_time = m_pdes_local_time->get();

    p_vci_initiator->nb_transport_fw( m_activity_payload, 
                                      m_activity_phase, 
                                      m_activity_time );

#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_null_time.value()
          << " / ACTIVITY MESSAGE = " 
          << m_activity_extension.is_active() << std::endl;
#endif
}

///////////////////////////////////////
tmpl (void)::send_irq( size_t channel )
///////////////////////////////////////
{
    // send IRQ command
    m_irq_time[channel]  = m_pdes_local_time->get();

    (*p_irq[channel])->nb_transport_fw( m_irq_payload[channel], 
                                        m_irq_phase[channel], 
                                        m_irq_time[channel] );
#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_vci_time[channel].value()
          << " / SEND IRQ on channel " << channel
          << " / value = " << m_irq_value[channel] << std::endl;
#endif
}

//////////////////////////////////////////////////////////////////////////
//     PDES process
//////////////////////////////////////////////////////////////////////////
tmpl (void)::execLoop ()
{
    while(true)
    {
        // deactivation if all channels are stopped:
        // send desactivation message... and deschedule 
        if ( m_dma_activated and all_channels_stopped() )
        {
            m_activity_extension.set_active();
            send_activity();
            m_dma_activated = false;
            wait( m_event );
        }

        // activation if at least one channel not stopped
        // send activation message... and continue
        if ( not m_dma_activated and all_channels_idle() )
        {
            m_activity_extension.set_inactive();
            send_activity();
            m_dma_activated = true;
        }
        
        // enters the loop on all channels when activated
        for ( size_t k = 0 ; k < m_channels ; k++ )
        {
            // update local time : one cycle
            m_pdes_local_time->add(UNIT_TIME);

            switch ( m_state[k] ) {
            ////////////////
            case STATE_IDLE:
            {
                if ( not m_stop[k] ) m_state[k] = STATE_READ_CMD;
                break;
            } 
            ////////////////////
            case STATE_READ_CMD:
            {
                if ( m_length[k] >= m_max_burst ) m_burst_length[k] = m_max_burst;
                else                              m_burst_length[k] = m_length[k];
                send_read( k );
                m_state[k] = STATE_READ_RSP;
                break;
            }
            ////////////////////
            case STATE_READ_RSP:
            {
                if ( m_rsp_received[k] ) 
                {
                    m_rsp_received[k] = false;

                    if ( m_stop[k] )
                    {
                        m_state[k] = STATE_IDLE;
                    }
                    else if( not m_vci_payload[k].is_response_error() )
                    {
                        m_state[k] = STATE_WRITE_CMD;
                    }
                    else
                    {
                        m_state[k] = STATE_ERROR_WRITE;
                        m_irq_value[k] = 0xFF;
                        if ( not m_irq_disabled[k] ) send_irq( k );
                    }
                }
                break;
            }
            /////////////////////
            case STATE_WRITE_CMD:
            {
                send_write( k );
                m_state[k] = STATE_WRITE_RSP;
                break;
            }
            /////////////////////
            case STATE_WRITE_RSP:
            {
                if ( m_rsp_received[k] )
                {
                    m_rsp_received[k] = false;

                    if ( m_stop[k] ) 		
                    {
                        m_state[k] = STATE_IDLE;
                    }
                    else if ( m_vci_payload[k].is_response_error() )
                    {
                        m_state[k] = STATE_ERROR_WRITE;
                        m_irq_value[k] = 0xFF;
                        if ( not m_irq_disabled[k] ) send_irq( k );
                    }
                    else if ( m_length[k] == 0 )
                    {
                        m_state[k] = STATE_SUCCESS;
                        m_irq_value[k] = 0xFF;
                        if ( not m_irq_disabled[k] ) send_irq( k );
                    }
                    else
                    {
                        m_length[k]      = m_length[k]      - m_burst_length[k];
                        m_source[k]      = m_source[k]      + m_burst_length[k];
                        m_destination[k] = m_destination[k] + m_burst_length[k];
                        m_state[k]       = STATE_READ_CMD;
                    }
                }
                break;
            }
            //////////////////////
            case STATE_ERROR_READ:
            case STATE_ERROR_WRITE:
            case STATE_SUCCESS:
            {
                if ( not m_stop[k] ) 
                {
                    m_state[k] = STATE_IDLE;
                    m_irq_value[k] = 0x00;
                    if ( not m_irq_disabled[k] ) send_irq( k );
                }
            } } // end channel state switch
        }  // end loop on channels

        // send NULL and deschedule if required
        if ( m_pdes_local_time->need_sync() and m_dma_activated ) 
        {
            send_null();

            wait( sc_core::SC_ZERO_TIME );
        }
    } // end infinite while
} // end execLoop

///////////////////////////////////////////////////////////////////////////////////
// service functions
///////////////////////////////////////////////////////////////////////////////////
tmpl(bool)::all_channels_stopped()
{
    for( size_t channel = 0 ; channel < m_channels ; channel++ )
    {
        if( m_stop[channel] == false ) return false;
    }
    return true;
}

tmpl(bool)::all_channels_idle()
{
    for( size_t channel = 0 ; channel < m_channels ; channel++ )
    {
        if( m_state[channel] != STATE_IDLE ) return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response on the VCI initiator port
// If it is a VCI response, the Boolean associated to the channel is set.
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_bw ( tlm::tlm_generic_payload &payload, 
                                             tlm::tlm_phase           &phase,       
                                             sc_core::sc_time         &time)  
{
    // update local time and notify
    if( time.value() > m_pdes_local_time->get().value()) m_pdes_local_time->set(time);

    // test transaction type
    soclib_payload_extension *extension_pointer;
    payload.get_extension(extension_pointer);
    if( extension_pointer->is_read() or extension_pointer->is_write() )
    { 
        // get channel index 
        size_t channel = extension_pointer->get_trd_id();

        // set signal response received
        assert( ((m_state[channel] == STATE_READ_RSP) or 
                 (m_state[channel] == STATE_WRITE_RSP)) and
        "ERROR in VCI_MULTI_DMA : unexpected response received");

        m_rsp_received[channel] = true;
    }

    return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response to an IRQ transaction
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::irq_nb_transport_bw ( int                         id,
                                                tlm::tlm_generic_payload    &payload,
                                                tlm::tlm_phase              &phase,
                                                sc_core::sc_time            &time ) 
{
    // no action
    return tlm::TLM_COMPLETED;
}

////////////////////////////////////////////////////////////////////////////////////
// Interface function used when receiving a VCI command on the target port
// As the dated transactions on the VCI target port are used to update the local
// time when all DMA channels are IDLE, this component requires periodical
// NULL messages from the interconnect. 
////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw ( tlm::tlm_generic_payload &payload,
                                            tlm::tlm_phase           &phase, 
                                            sc_core::sc_time         &time)   
{
    size_t  cell;
    size_t  reg;
    size_t  channel;

    soclib_payload_extension* extension_pointer;
    payload.get_extension(extension_pointer);
    
    // Compute global state
    bool all_idle = true;
    for( size_t k = 0 ; k < m_channels ; k++ )
    {
        if( m_state[k] != STATE_IDLE ) all_idle = false;
    }

    // update local time if all channels IDLE 
    if ( all_idle and  (m_pdes_local_time->get().value() < time.value()) )
    {
        m_pdes_local_time->set( time );
    }

    // No other action on NULL messages
    if ( extension_pointer->is_null_message() ) 
    {

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << time.value() 
          << " Receive NULL message" << std::endl;
#endif
        return tlm::TLM_COMPLETED;
    }

    // address and length checking for a VCI command
    bool	one_flit = (payload.get_data_length() == 4);
    addr_t	address = payload.get_address();

    if ( m_segment.contains(address) && one_flit )
    {
        cell    = (size_t)((address - m_segment.baseAddress()) >> 2);
        reg     = cell % DMA_SPAN;
        channel = cell / DMA_SPAN;

        // checking channel overflow
        if ( channel < m_channels ) payload.set_response_status(tlm::TLM_OK_RESPONSE);
        else             payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);

        if ( extension_pointer->get_command() == VCI_READ_COMMAND )
        {

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << time.value() 
          << " Receive VCI read command : address = " << std::hex << address
          << " / channel = " << std::dec << channel
          << " / reg = "  << reg << std::endl;
#endif
        
            if ( reg == DMA_SRC )
            {
                utoa( m_source[channel], payload.get_data_ptr(), 0 );
            }
            else if ( reg == DMA_DST )
            {
                utoa( m_destination[channel], payload.get_data_ptr(), 0 );
            }
            else if ( reg == DMA_LEN )
            {
                utoa( m_state[channel], payload.get_data_ptr(), 0 );
            }
            else if ( reg == DMA_IRQ_DISABLED ) 
            {
                utoa( (int)(m_irq_disabled[channel]), payload.get_data_ptr(), 0 );
            }
            else    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        } // end read

        else if (extension_pointer->get_command() == VCI_WRITE_COMMAND)
        {
            uint32_t data = atou(payload.get_data_ptr(), 0);

#if SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = "  << time.value() 
          << " Receive VCI write command : address = " << std::hex << address
          << " / channel = " << std::dec << channel
          << " / reg = "  << reg 
          << " / data = " << data << std::endl;
#endif
            // configuration command other than soft reset
            // are ignored when the DMA channel is active
            if ( reg == DMA_RESET )
            {
                m_stop[channel] = true;
            }
            else if ( m_state[channel] != STATE_IDLE )
            {
                if      ( reg == DMA_SRC )  m_source[channel]      = data;
                else if ( reg == DMA_DST )  m_destination[channel] = data;
                else if ( reg == DMA_LEN ) 
                {
                    m_length[channel] = data;
                    m_stop[channel]   = false;
                    m_event.notify();
                }
                else if ( cell == DMA_IRQ_DISABLED )  m_irq_disabled[channel] = (data != 0);
                else    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            }
            else
            {
                std::cout << name() 
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

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// cle-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

