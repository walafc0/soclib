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
 * Maintainers: fpecheux, alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef __VCI_RAM_H__ 
#define __VCI_RAM_H__

#include <tlmdt>                    
#include "mapping_table.h"         
#include "loader.h"               
#include "linked_access_buffer.h" 
#include "soclib_endian.h"

namespace soclib { namespace tlmdt {

////////////////////////////////
template<typename vci_param>
class VciRam 
////////////////////////////////
  : public sc_core::sc_module 
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> 
{
 private:

    typedef typename vci_param::data_t ram_t;

    /////////////////////////////////////////////////////////////////////////////////////
    // Member Variables
    /////////////////////////////////////////////////////////////////////////////////////
    soclib::common::MappingTable                                              m_mt;
    soclib::common::Loader                                                   *m_loader;
    std::list<soclib::common::Segment>                                        m_segments;
    ram_t                                                                   **m_contents;
    soclib::common::LinkedAccessBuffer<typename vci_param::addr_t,uint32_t>   m_atomic;
    uint32_t                                                                  m_latency;

    // local time
    pdes_local_time                                                          *m_pdes_local_time;

    //counters
    uint32_t                                                                  m_cpt_read;
    uint32_t                                                                  m_cpt_write;

    /////////////////////////////////////////////////////////////////////////////////////
    // Fuction executed when receiving a command on p_vci
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload &payload,
                                        tlm::tlm_phase           &phase,
                                        sc_core::sc_time         &time );

    /////////////////////////////////////////////////////////////////////////////////////
    // Not used but required by interface
    /////////////////////////////////////////////////////////////////////////////////////
    void b_transport( tlm::tlm_generic_payload &payload,
                      sc_core::sc_time         &time );
  
    bool get_direct_mem_ptr( tlm::tlm_generic_payload &payload,
                             tlm::tlm_dmi             &dmi_data );
  
    unsigned int transport_dbg( tlm::tlm_generic_payload &payload);

protected:

    SC_HAS_PROCESS(VciRam);

public:

    tlm::tlm_target_socket<32,tlm::tlm_base_protocol_types> p_vci; 

    VciRam( sc_core::sc_module_name            name,
            const soclib::common::IntTab       &index,
	        const soclib::common::MappingTable &mt,
	        const soclib::common::Loader       &loader,
            const uint32_t                     latency = 1);
  
    ~VciRam();

    void print_stats();
};
}}

#endif
