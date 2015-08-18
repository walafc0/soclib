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
 * Copyright (c) UPMC, Lip6
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_VCI_XICU_H
#define SOCLIB_VCI_XICU_H

#include <systemc>
#include "vci_target_fsm.h"
#include "caba_base_module.h"
#include "mapping_table.h"

namespace soclib {
namespace caba {

template<typename vci_param>
class VciXicu
	: public caba::BaseModule
{
private:
    std::list<soclib::common::Segment>              m_seglist;

    const size_t                                    m_pti_count;
    const size_t                                    m_hwi_count;
    const size_t                                    m_wti_count;
    const size_t                                    m_irq_count;

    sc_signal<int>                                  r_fsm;
    sc_signal<typename vci_param::data_t>           r_data;
    sc_signal<typename vci_param::srcid_t>          r_srcid; 
    sc_signal<typename vci_param::trdid_t>          r_trdid; 
    sc_signal<typename vci_param::pktid_t>          r_pktid; 
    sc_signal<uint32_t>*                            r_msk_pti;
    sc_signal<uint32_t>*                            r_msk_wti;
    sc_signal<uint32_t>*                            r_msk_hwi;
    sc_signal<uint32_t>                             r_pti_pending;
    sc_signal<uint32_t>                             r_wti_pending;
    sc_signal<uint32_t>                             r_hwi_pending;
    sc_signal<uint32_t>*                            r_pti_per;
    sc_signal<uint32_t>*                            r_pti_val;
    sc_signal<uint32_t>*                            r_wti_reg;


    void transition();
    void genMoore();

    enum fsm_state_e
    {
        IDLE,
        RSP_READ,
        RSP_WRITE,
        RSP_ERROR
    };

protected:
    SC_HAS_PROCESS(VciXicu);

public:
    sc_core::sc_in<bool>                           p_clk;
    sc_core::sc_in<bool>                           p_resetn;
    soclib::caba::VciTarget<vci_param>             p_vci;
    sc_core::sc_out<bool>*                         p_irq;
    sc_core::sc_in<bool>*                          p_hwi;

    void print_trace( size_t detail );

	~VciXicu();

	VciXicu(
		sc_core::sc_module_name name,
		const soclib::common::MappingTable &mt,
		const soclib::common::IntTab &index,
        size_t pti_count,
        size_t hwi_count,
        size_t wti_count,
        size_t irq_count);

    soclib_static_assert(vci_param::B == 4);
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

