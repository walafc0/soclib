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
 * Copyright (c) UPMC / Lip6, 2011
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

/////////////////////////////////////////////////////////////////////////////////////
// Implementation Note
// This component is a VCI target. It has a variable number of IRQ ports,
// asociated to a variable number of TTY termnals (one IRQ per terminal).
// It is modeled as a purely reactive interface function : no thread, no local time.
// To synchronizes with the system, it uses the NULL messages received on the VCI 
// port : to support the relaxed PDES time filtering on the ICU component, the IRQ 
// values are transmitted on the IRQ ports each time a NULL message is received.
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_TLMDT_VCI_MULTI_TTY_H
#define SOCLIB_TLMDT_VCI_MULTI_TTY_H

#include <stdarg.h>
#include <tlmdt>             
#include "mapping_table.h"   
#include "tty.h"
#include "tty_wrapper.h"

namespace soclib { namespace tlmdt {

template <typename vci_param>
class VciMultiTty
  : public sc_core::sc_module 
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types>
  , virtual public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
{

private:

    typedef typename vci_param::addr_t addr_t;
    typedef typename vci_param::data_t data_t;

    // Member Variables
    std::vector<soclib::common::TtyWrapper*>    m_term;			// terminals
    soclib::common::Segment       	            m_segment;		// associated segment

    // IRQ transactions (one per terminal)
    uint8_t*					                m_irq_value; 	// interrupt value
    tlm::tlm_generic_payload*                   m_irq_payload;
    tlm::tlm_phase*                             m_irq_phase;
    sc_core::sc_time*                           m_irq_time;

    // Instrumentation counters
    size_t                                      m_cpt_read;
    size_t                                      m_cpt_write;

    //////////////////////////////////////////////////////////////////////////////
    //  Functions
    //////////////////////////////////////////////////////////////////////////////
    void init(const std::vector<std::string> &names);

    //////////////////////////////////////////////////////////////////////////////
    // Interface function executed when receiving a command on p_vci
    //////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum nb_transport_fw ( tlm::tlm_generic_payload &payload,
                                         tlm::tlm_phase           &phase,  
                                         sc_core::sc_time         &time);   

    //////////////////////////////////////////////////////////////////////////////
    // Interface function executed when receiving a response on p_irq[id]
    //////////////////////////////////////////////////////////////////////////////
    tlm::tlm_sync_enum irq_nb_transport_bw ( int                      id,
                                             tlm::tlm_generic_payload &payload,
                                             tlm::tlm_phase           &phase,  
                                             sc_core::sc_time         &time);   

    //////////////////////////////////////////////////////////////////////////////
    // Not implemented but required by the interface
    //////////////////////////////////////////////////////////////////////////////
    void b_transport ( tlm::tlm_generic_payload &payload,
                       sc_core::sc_time         &time);

    bool get_direct_mem_ptr ( tlm::tlm_generic_payload &payload,
                              tlm::tlm_dmi             &dmi_data);

    unsigned int transport_dbg ( tlm::tlm_generic_payload &payload);

    void invalidate_direct_mem_ptr ( sc_dt::uint64 start_range,
                                     sc_dt::uint64 end_range);

    tlm::tlm_sync_enum nb_transport_bw ( tlm::tlm_generic_payload &payload,
                                         tlm::tlm_phase           &phase,
                                         sc_core::sc_time         &time);

protected:

    SC_HAS_PROCESS(VciMultiTty);

public:

    // ports
    tlm::tlm_target_socket<32,tlm::tlm_base_protocol_types>	   p_vci;

    std::vector<tlm_utils::simple_initiator_socket_tagged
    <VciMultiTty,32,tlm::tlm_base_protocol_types> *>           p_irq; 

    // constructors
    VciMultiTty(sc_core::sc_module_name name,
	            const soclib::common::IntTab &index,
	            const soclib::common::MappingTable &mt,
	            const char *first_name,
	            ...);

    VciMultiTty(sc_core::sc_module_name name,
	            const soclib::common::IntTab &index,
	            const soclib::common::MappingTable &mt,
	            const std::vector<std::string> &names);
 
    // Instrumentation functions
    size_t getNRead();
    size_t getNWrite();
    void print_stats();
};

}}

#endif /* __VCI_MULTI_TTY_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

