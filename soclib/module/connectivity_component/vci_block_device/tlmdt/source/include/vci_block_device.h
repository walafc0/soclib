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
 *
 * Maintainers: alinev, alain
 */
#ifndef SOCLIB_VCI_BLOCK_DEVICE_H
#define SOCLIB_VCI_BLOCK_DEVICE_H

#include <stdint.h>
#include <systemc>
#include <tlmdt>
#include "mapping_table.h"

namespace soclib {
namespace tlmdt {

using namespace sc_core;

template<typename vci_param>
class VciBlockDevice
  : public sc_core::sc_module
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> 
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> 
{
    typedef typename vci_param::addr_t addr_t;
    typedef typename vci_param::data_t data_t;

private:
	
    // Structural constants
    const uint32_t 		m_block_size;		// physical block size
    const uint32_t 		m_average_latency;	// requested latency
    const uint32_t 		m_srcid;		// VCI source id
    int      			m_fd;			// file descriptor
    uint32_t 			m_device_size;		// max number of blocks on device

    // Registers
    int      			m_status;		// initiator status
    int      			m_op;			// operation requested (read/write)
    uint32_t 			m_buffer;		// buffer address in memory
    uint32_t 			m_count;		// number of blocks to transfer 
    uint32_t 			m_lba;			// base block index 
    uint32_t 			m_transfer_offset;	// number of bytes already transfered
    uint32_t 			m_transfer_size;	// number of bytes to be transfered
    uint32_t 			m_access_latency;	// actual latency (after random)
    bool     			m_irq_enabled;		// no comment
    uint32_t 			m_lfsr;			// randomization of the latency
    int      			m_current_op;		// running operation
    uint8_t*             	m_vci_be_buf;		// local be buffer
    uint8_t* 			m_vci_data_buf;		// local buffer (disk cache)
    uint8_t      		m_irq_value;		// IRQ current value

    soclib::common::Segment	m_segment;
    pdes_local_time*		m_pdes_local_time;
    sc_core::sc_event         	m_rsp_received;

    // VCI TRANSACTION
    tlm::tlm_generic_payload	m_vci_payload;
    soclib_payload_extension	m_vci_extension;
    tlm::tlm_phase		m_vci_phase;
    sc_core::sc_time		m_vci_time;
    
    // NULL MESSAGE
    tlm::tlm_generic_payload 	m_null_payload;
    soclib_payload_extension 	m_null_extension;
    tlm::tlm_phase            	m_null_phase;
    sc_core::sc_time          	m_null_time;
    
    // IRQ TRANSACTION
    tlm::tlm_generic_payload  	m_irq_payload;
    tlm::tlm_phase            	m_irq_phase;
    sc_core::sc_time          	m_irq_time;

    // Functions
    void execLoop();
    void send_null();
    void send_vci_command(bool read_from_memory);
    void ended(int status);
    void write_finish();
    void read_done();
    void next_req();

    // Virtual Fuction  to receive response on the VCI initiator port
    tlm::tlm_sync_enum nb_transport_bw 
    ( tlm::tlm_generic_payload   &payload,   
      tlm::tlm_phase             &phase,   
      sc_core::sc_time           &time);  
    
    // Virtual Fuctions  to receive command on the VCI target port    
    tlm::tlm_sync_enum nb_transport_fw 
    ( tlm::tlm_generic_payload &payload, 
      tlm::tlm_phase           &phase,   
      sc_core::sc_time         &time);  
    
    // Virtual Fuction to receive response on the IRQ port
    tlm::tlm_sync_enum irq_nb_transport_bw    
    ( tlm::tlm_generic_payload           &payload, 
      tlm::tlm_phase                     &phase, 
      sc_core::sc_time                   &time); 

    // Not implemented for this example but required by interface
    void b_transport                       
    ( tlm::tlm_generic_payload &payload, 
      sc_core::sc_time         &time); 
    
    bool get_direct_mem_ptr
    ( tlm::tlm_generic_payload &payload,  
      tlm::tlm_dmi             &dmi_data); 
    
    unsigned int transport_dbg                            
    ( tlm::tlm_generic_payload &payload); 
    
    void invalidate_direct_mem_ptr      
    ( sc_dt::uint64 start_range,      
      sc_dt::uint64 end_range);     
    
protected:
    SC_HAS_PROCESS(VciBlockDevice);
    
public:
    tlm::tlm_initiator_socket<32, tlm::tlm_base_protocol_types> 			p_vci_initiator; 
    tlm::tlm_target_socket<32,tlm::tlm_base_protocol_types> 				p_vci_target;      
    tlm_utils::simple_initiator_socket<VciBlockDevice,32, tlm::tlm_base_protocol_types> p_irq; 

	VciBlockDevice(
	sc_module_name name,
	const soclib::common::MappingTable &mt,
	const soclib::common::IntTab &srcid,
	const soclib::common::IntTab &tgtid,
        const std::string &filename,
        const uint32_t block_size = 512,
        const uint32_t latency = 0);
};

}}

#endif /* SOCLIB_VCI_BLOCK_DEVICE_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

