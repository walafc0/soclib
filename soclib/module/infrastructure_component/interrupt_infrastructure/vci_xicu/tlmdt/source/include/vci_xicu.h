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
 * Copyright (c) UPMC, Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 *
 * Maintainers: alinev
 */
#ifndef SOCLIB_VCI_XICU_H
#define SOCLIB_VCI_XICU_H

#include <tlmdt>
#include "mapping_table.h"

namespace soclib {
namespace tlmdt {

template<typename vci_param>
class VciXicu
    : public sc_core::sc_module             // inherit from SC module base clase
    , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "forward interface"
{
private:
    typedef typename vci_param::data_t data_t;

    /////////////////////////////////////////////////////////////////////////////////////
    // Member Variables
    /////////////////////////////////////////////////////////////////////////////////////
    const size_t m_pti_count;
    const size_t m_hwi_count;
    const size_t m_wti_count;
    const size_t m_irq_count;

    uint32_t *m_msk_pti;
    uint32_t *m_msk_wti;
    uint32_t *m_msk_hwi;
    uint32_t  m_pti_pending;
    uint32_t  m_wti_pending;
    uint32_t  m_hwi_pending;
    uint32_t *m_pti_per;
    uint32_t *m_pti_val;
    uint32_t *m_wti_reg;

    std::list<soclib::common::Segment>  m_segments;

    std::map<sc_core::sc_time, std::pair<int, bool> > m_pending_irqs;
    pdes_local_time                    *m_pdes_local_time;

    //FIELDS OF AN IRQ TRANSACTION
    tlm::tlm_generic_payload            m_irq_payload;
    tlm::tlm_phase                      m_irq_phase;
    sc_core::sc_time                    m_irq_time;

    /////////////////////////////////////////////////////////////////////////////////////
    // Local Fuctions
    /////////////////////////////////////////////////////////////////////////////////////
    void verify_period_interruption(int idx, uint32_t old_msk, uint32_t msk);
    void send_interruption(int idx, bool val, sc_core::sc_time time);
    void behavior();

    /////////////////////////////////////////////////////////////////////////////////////
    // Virtual Fuctions  tlm::tlm_fw_transport_if  (VCI TARGET SOCKET)
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_fw        // receive vci command from initiator
    ( tlm::tlm_generic_payload &payload,    // payload
      tlm::tlm_phase           &phase,      // phase
      sc_core::sc_time         &time);      // time
    
    // Not implemented for this example but required by interface
    void b_transport                          // b_transport() - Blocking Transport
    ( tlm::tlm_generic_payload &payload,      // payload
      sc_core::sc_time         &time);        // time
    
    // Not implemented for this example but required by interface
    bool get_direct_mem_ptr
    ( tlm::tlm_generic_payload &payload,      // payload
      tlm::tlm_dmi             &dmi_data);    // DMI data
    
    // Not implemented for this example but required by interface
    unsigned int transport_dbg                            
    ( tlm::tlm_generic_payload &payload);     // payload
    
    /////////////////////////////////////////////////////////////////////////////////////
    // Virtual Fuctions  tlm::tlm_fw_transport_if (IRQ TARGET SOCKET)
    /////////////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum hwi_nb_transport_fw    // receive interruption from initiator
    ( int                         id,         // interruption id
      tlm::tlm_generic_payload   &payload,    // payload
      tlm::tlm_phase             &phase,      // phase
      sc_core::sc_time           &time);      // time
    
protected:
    SC_HAS_PROCESS(VciXicu);

public:
    tlm::tlm_target_socket<32, tlm::tlm_base_protocol_types> p_vci;   // VCI TARGET socket
    std::vector<tlm_utils::simple_initiator_socket_tagged<VciXicu,32,tlm::tlm_base_protocol_types> *> p_irq; // IRQ INITIATOR socket
    std::vector<tlm_utils::simple_target_socket_tagged<VciXicu,32,tlm::tlm_base_protocol_types> *> p_hwi; // IRQ TARGET socket

	~VciXicu();

	VciXicu(
		sc_core::sc_module_name name,
		const soclib::common::MappingTable &mt,
		const soclib::common::IntTab &index,
        size_t pti_count,
        size_t hwi_count,
        size_t wti_count,
        size_t irq_count);

    //soclib_static_assert(vci_param::nbytes == 4);
};

}}

#endif /* SOCLIB_VCI_XICU_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

