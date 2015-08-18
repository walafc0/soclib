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
 *         Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>, 2010
 *         Alain Greiner <alain.greiner@lip6.fr>
 */

#include <stdint.h>
#include <iostream>
#include "register.h"
#include "../include/vci_block_device.h"
#include <fcntl.h>
#include "block_device.h"

//#define SOCLIB_MODULE_DEBUG 1

#define MAX_BURST_LENGTH 64

namespace soclib { namespace tlmdt {

    namespace {
        const char* SoclibBlockDeviceRegisters_str[] = {
            "BLOCK_DEVICE_BUFFER",
            "BLOCK_DEVICE_LBA",
            "BLOCK_DEVICE_COUNT",
            "BLOCK_DEVICE_OP",
            "BLOCK_DEVICE_STATUS",
            "BLOCK_DEVICE_IRQ_ENABLE",
            "BLOCK_DEVICE_SIZE",
            "BLOCK_DEVICE_BLOCK_SIZE"
        };
        const char* SoclibBlockDeviceOp_str[] = {
            "BLOCK_DEVICE_NOOP",
            "BLOCK_DEVICE_READ",
            "BLOCK_DEVICE_WRITE",
        };
        const char* SoclibBlockDeviceStatus_str[] = {
            "BLOCK_DEVICE_IDLE",
            "BLOCK_DEVICE_BUSY",
            "BLOCK_DEVICE_READ_SUCCESS",
            "BLOCK_DEVICE_WRITE_SUCCESS",
            "BLOCK_DEVICE_READ_ERROR",
            "BLOCK_DEVICE_WRITE_ERROR",
            "BLOCK_DEVICE_ERROR",
        };
    }

#define tmpl(t) template<typename vci_param> t VciBlockDevice<vci_param>

/////////////////////////
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

////////////////////////////////////////////////////
tmpl (void)::send_vci_command(bool read_from_memory)
{        
    m_pdes_local_time->reset_sync();

    // send VCI command
    size_t	length =  m_transfer_size - m_transfer_offset;
    if ( length > MAX_BURST_LENGTH) length = MAX_BURST_LENGTH;
    if ( read_from_memory ) 	m_vci_extension.set_read();
    else			m_vci_extension.set_write();
    m_vci_payload.set_data_ptr(m_vci_data_buf + m_transfer_offset);
    m_vci_payload.set_address(m_buffer + m_transfer_offset);
    m_vci_payload.set_data_length(length);
    m_vci_payload.set_byte_enable_length(length);
    m_vci_time = m_pdes_local_time->get();
    p_vci_initiator->nb_transport_fw(m_vci_payload, m_vci_phase, m_vci_time);

    // send IRQ 
    m_irq_time = m_pdes_local_time->get();
    p_irq->nb_transport_fw(m_irq_payload, m_irq_phase, m_irq_time);

#ifdef SOCLIB_MODULE_DEBUG
std::cout <<  "[" <<name() << "] time = " << std::dec << m_vci_time.value();
if ( read_from_memory )	std::cout << " VCI READ at address ";
else                   	std::cout << " VCI WRITE at address ";
std::cout << std::hex << m_buffer + m_transfer_offset;
std::cout << " / IRQ value = " << (int)m_irq_value << std::endl;
#endif

    m_transfer_offset += length;
}

/////////////////////////////
tmpl(void)::ended(int status)
{
    
#ifdef SOCLIB_MODULE_DEBUG
    std::cout 
        << name()
        << " finished current operation ("
        << SoclibBlockDeviceOp_str[m_current_op]
        << ") with the status "
        << SoclibBlockDeviceStatus_str[status]
        << std::endl;
#endif

    if ( m_irq_enabled ) m_irq_value = 0xFF;
    m_current_op = m_op = BLOCK_DEVICE_NOOP;
    m_status = status;
}

//////////////////////
tmpl(void)::next_req()
{
    switch (m_current_op) {
    case BLOCK_DEVICE_READ:
    {
        if ( m_count && m_transfer_offset == 0 ) 
        {
            if ( m_lba + m_count > m_device_size ) 
            {
                std::cerr << name() << " warning: trying to read beyond end of device" << std::endl;
                ended(BLOCK_DEVICE_READ_ERROR);
                break;
            }
            m_vci_data_buf = new uint8_t[m_transfer_size];
            lseek(m_fd, m_lba*m_block_size, SEEK_SET);
            int retval = ::read(m_fd, m_vci_data_buf, m_transfer_size);
            if ( retval < 0 ) 
            {
                ended(BLOCK_DEVICE_READ_ERROR);
                break;
            }
        }

        // blocking command
        bool read_from_memory = false;
        send_vci_command(read_from_memory);
        wait(m_rsp_received);

	m_status = BLOCK_DEVICE_BUSY;
        read_done();
        break;
    }
    case BLOCK_DEVICE_WRITE:
    {
        if ( m_count && m_transfer_offset == 0 ) 
        {
            if ( m_lba + m_count > m_device_size ) 
            {
                std::cerr << name() << " warning: trying to write beyond end of device" << std::endl;
                ended(BLOCK_DEVICE_WRITE_ERROR);
                break;
            }
            m_vci_data_buf = new uint8_t[m_transfer_size];
            lseek(m_fd, m_lba*m_block_size, SEEK_SET);
        }

        // blocking command
        bool read_from_memory = true;
        send_vci_command(read_from_memory);
        wait(m_rsp_received);

        
        m_status = BLOCK_DEVICE_BUSY;
        write_finish();
        break;
    }
    default:
        ended(BLOCK_DEVICE_ERROR);
        break;
    }
}
    
///////////////////////////
tmpl(void)::write_finish( )
{
    if ( ! m_vci_payload.is_response_error() && m_transfer_offset < m_transfer_size ) 
    {
        next_req();
        return;
    }

    if ( ! m_vci_payload.is_response_error() && m_fd >= 0 && 
          ::write( m_fd, (char *)m_vci_data_buf, m_transfer_size ) > 0 ) ended(BLOCK_DEVICE_WRITE_SUCCESS);
    else                                                         ended(BLOCK_DEVICE_WRITE_ERROR);

    delete m_vci_data_buf;
}

///////////////////////
tmpl(void)::read_done()
{
    if ( ! m_vci_payload.is_response_error() && m_transfer_offset < m_transfer_size ) 
    {
        next_req();
        return;
    }

    if ( m_vci_payload.is_response_error() ) ended(BLOCK_DEVICE_READ_ERROR);
    else                                     ended(BLOCK_DEVICE_READ_SUCCESS);

    delete m_vci_data_buf;
}

//////////////////////////////////////////////////////
// initiator thread
//////////////////////////////////////////////////////
tmpl (void)::execLoop ()
{
    while(true)
    {
        m_pdes_local_time->add(UNIT_TIME);

        if ( m_current_op == BLOCK_DEVICE_NOOP && m_op != BLOCK_DEVICE_NOOP ) 
        {
            if ( m_access_latency ) 
            {
                m_access_latency--;
            } 
            else 
            {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << name() << " launch an operation " << SoclibBlockDeviceOp_str[m_op] << std::endl;
#endif
                m_current_op 		= m_op;
                m_op 			= BLOCK_DEVICE_NOOP;
                m_transfer_offset 	= 0;
                m_transfer_size 	= m_count * m_block_size;
                next_req();
            }
        }        

        // send a null message if required
        if (m_pdes_local_time->need_sync())   
        {
            send_null(); 
            wait( m_rsp_received );
        }
    }
    sc_core::sc_stop();
} // end execLoop()

///////////////////////////////////////////////////////////////////////////////
// Interface function used when receiving a response on the initiator port
///////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::nb_transport_bw    
( tlm::tlm_generic_payload           &payload,     
  tlm::tlm_phase                     &phase,     
  sc_core::sc_time                   &time)    
{
    if(time > m_pdes_local_time->get()) m_pdes_local_time->set(time);
    m_rsp_received.notify (sc_core::SC_ZERO_TIME);
    return tlm::TLM_COMPLETED;
}

/////////////////////////////////////////////////////////////////////////////////////
// Interface function used when receiving a VCI command on the target port
/////////////////////////////////////////////////////////////////////////////////////
tmpl(tlm::tlm_sync_enum)::nb_transport_fw ( tlm::tlm_generic_payload &payload, 
                                            tlm::tlm_phase           &phase,  
                                            sc_core::sc_time         &time)  
{
    typename vci_param::addr_t	address = payload.get_address();
    int 			cell;
    bool			one_flit = (payload.get_data_length() == (unsigned int)vci_param::nbytes);

    soclib_payload_extension*	extension_pointer;
    payload.get_extension(extension_pointer);
    
    // No action and no response when receiving a null message
    if(extension_pointer->is_null_message()) return tlm::TLM_COMPLETED;
  
    // address  and pcket length checking
    if ( m_segment.contains(payload.get_address()) && one_flit )
    {
        cell = (int)((address - m_segment.baseAddress()) / vci_param::nbytes);
        payload.set_response_status(tlm::TLM_OK_RESPONSE);

        if (extension_pointer->get_command() == VCI_READ_COMMAND)
        {
                    
#ifdef SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = " << std::dec << time.value()
          << " STATUS REQUEST " << SoclibBlockDeviceRegisters_str[cell] << std::endl;
#endif
                    
            if      ( cell == BLOCK_DEVICE_SIZE ) utoa(m_device_size, payload.get_data_ptr(),0);
            else if ( cell == BLOCK_DEVICE_BLOCK_SIZE ) utoa(m_block_size, payload.get_data_ptr(),0);
            else if ( cell == BLOCK_DEVICE_STATUS ) 
            {
                utoa(m_status, payload.get_data_ptr(),0);
                if (m_status != BLOCK_DEVICE_BUSY) 
                {
                    m_status = BLOCK_DEVICE_IDLE;
		    m_irq_value = 0x00;
                }
            }
            else    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        } //end if READ
        else if (extension_pointer->get_command() == VCI_WRITE_COMMAND)
        {

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "[" << name() << "] time = " << std::dec << time.value()
          << " CONFIG REQUEST to " << SoclibBlockDeviceRegisters_str[cell] 
          << " with value = " << std::hex << atou(payload.get_data_ptr(),0) << std::endl;
#endif
            if ( m_status != BLOCK_DEVICE_BUSY ) 
            {
                data_t data = atou(payload.get_data_ptr(),0);
                if      ( cell == BLOCK_DEVICE_BUFFER ) m_buffer = data;
                else if ( cell == BLOCK_DEVICE_COUNT ) m_count = data;
                else if ( cell == BLOCK_DEVICE_LBA ) m_lba = data;
                else if ( cell == BLOCK_DEVICE_OP )
                {
                    m_op = data;
                    m_access_latency = m_lfsr % (m_average_latency+1);
                    m_lfsr = (m_lfsr >> 1) ^ ((-(m_lfsr & 1)) & 0xd0000001);
                }
                else if ( cell == BLOCK_DEVICE_IRQ_ENABLE ) m_irq_enabled = (bool)data;
                else    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            }
            else
            {
                std::cerr << name() 
                << " warning: receiving a new command while busy, ignored" << std::endl;
            } 
        } // end if WRITE
        else // illegal command
        {
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        }
    }
    else // illegal address
    {
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    }

    // update transaction phase and time
    phase = tlm::BEGIN_RESP;
    if ( time.value() < m_pdes_local_time->get().value() )  time = m_pdes_local_time->get();
    else                                                    time = time + UNIT_TIME;
    // send response
    p_vci_target->nb_transport_bw(payload, phase, time);
    return tlm::TLM_COMPLETED;

} // end nb_transport_fw

// Not implemented for this example but required by interface
tmpl(void)::b_transport
( tlm::tlm_generic_payload &payload,                // payload
  sc_core::sc_time         &_time)                  //time
{
  return;
}

// Not implemented for this example but required by interface
tmpl(bool)::get_direct_mem_ptr
( tlm::tlm_generic_payload &payload,                // address + extensions
  tlm::tlm_dmi             &dmi_data)               // DMI data
{ 
  return false;
}
    
// Not implemented for this example but required by interface
tmpl(unsigned int):: transport_dbg                            
( tlm::tlm_generic_payload &payload)                // debug payload
{
  return false;
}

// Not implemented for this example but required by interface
tmpl(void)::invalidate_direct_mem_ptr 
( sc_dt::uint64 start_range, 
  sc_dt::uint64 end_range) { }

/////////////////////////////////////////////////////////////////////////////////////
// Interface function executed when receiving a response to an IRQ transaction
/////////////////////////////////////////////////////////////////////////////////////
tmpl (tlm::tlm_sync_enum)::irq_nb_transport_bw    
( tlm::tlm_generic_payload           &payload,       // payload
  tlm::tlm_phase                     &phase,         // phase
  sc_core::sc_time                   &time)          // time
{
  return tlm::TLM_COMPLETED;
}

////////////////////////////
// constructor
////////////////////////////
tmpl(/**/)::VciBlockDevice(
    sc_module_name name,
    const soclib::common::MappingTable &mt,
    const soclib::common::IntTab &srcid,
    const soclib::common::IntTab &tgtid,
    const std::string &filename,
    const uint32_t block_size, 
    const uint32_t latency)
           : sc_core::sc_module(name)
          , m_block_size(block_size)
          , m_average_latency(latency)
          , m_srcid(mt.indexForId(srcid))
          , m_segment(mt.getSegment(tgtid))
          , p_vci_initiator("init") 
          , p_vci_target("tgt")   
          , p_irq("irq")        
{
    // bind vci initiator port
    p_vci_initiator(*this);                     
    
    // bind vci target port
    p_vci_target(*this);                     
    
    // register irq initiator port
    p_irq.register_nb_transport_bw(this, &VciBlockDevice::irq_nb_transport_bw);
    
    // PDES local time
    m_pdes_local_time = new pdes_local_time(100*UNIT_TIME);

    // create and initialize the be buffer for VCI transactions 
    m_vci_be_buf = new uint8_t[MAX_BURST_LENGTH];
    for ( size_t i=0 ; i<MAX_BURST_LENGTH ; i++) m_vci_be_buf[i] = 0xFF;

    // initialize payload, phase and extension for a VCI transaction
    m_vci_payload.set_command(tlm::TLM_IGNORE_COMMAND);
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

    // open file
    m_fd = ::open(filename.c_str(), O_RDWR);
    if ( m_fd < 0 ) 
    {
        std::cerr << "Unable to open block device image file " << filename << std::endl;
        m_device_size = 0;
    } 
    else 
    {
        m_device_size = lseek(m_fd, 0, SEEK_END) / m_block_size;
        if ( m_device_size > 0xFFFFFFFF)
        {
            std::cerr
                << "Warning: block device " << filename
                << " has more blocks than addressable with 32 bits" << std::endl;
            m_device_size = 0xFFFFFFFF;
        }
    }
    
    // initialise registers
    m_irq_enabled = false;
    m_irq_value = 0x00;
    m_op = BLOCK_DEVICE_NOOP;
    m_current_op = BLOCK_DEVICE_NOOP;
    m_access_latency = 0;
    m_status = BLOCK_DEVICE_IDLE;
    m_lfsr = -1;
    
#ifdef SOCLIB_MODULE_DEBUG
    std::cout 
        << name
        << " = Opened " 
        << filename
        << " which has "
        << m_device_size
        << " blocks of "
        << m_block_size
        << " bytes"
        << std::endl;
#endif

  SC_THREAD(execLoop);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

