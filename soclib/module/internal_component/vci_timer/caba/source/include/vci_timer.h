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
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Based on previous works by Alain Greiner, 2004
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_VCI_TIMER_H
#define SOCLIB_VCI_TIMER_H

#include <systemc>
#include "vci_target_fsm.h"
#include "caba_base_module.h"
#include "mapping_table.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciTimer
	: public caba::BaseModule
{
private:
    soclib::caba::VciTargetFsm<vci_param, true> m_vci_fsm;

    bool on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be);
    bool on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data);
    void transition();
    void genMoore();

	size_t m_ntimer;

    sc_signal<typename vci_param::data_t> *r_value;
    sc_signal<typename vci_param::data_t> *r_period;
    sc_signal<typename vci_param::data_t> *r_counter;
    sc_signal<int> *r_mode;
    sc_signal<bool> *r_irq;

    size_t m_reset_counter_no;
    size_t m_reset_value_no;

    // Activity counters
    uint32_t m_cpt_read;   // Count READ access
    uint32_t m_cpt_write;  // Count WRITE access
    uint32_t m_cpt_cycles; // Count cycles

protected:
    SC_HAS_PROCESS(VciTimer);

public:
    	sc_in<bool> p_clk;
    	sc_in<bool> p_resetn;
    	soclib::caba::VciTarget<vci_param> p_vci;
    	sc_out<bool> *p_irq;

	VciTimer(
		sc_module_name name,
		const soclib::common::IntTab &index,
		const soclib::common::MappingTable &mt,
        size_t nirq);

	~VciTimer();

        void print_trace();
};

}}

#endif /* SOCLIB_VCI_TIMER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

