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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2003-2007
 *         Nicolas Pouillon <nipo@ssji.net>, 2006
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_CABA_VCI_LOCKS_H
#define SOCLIB_CABA_VCI_LOCKS_H

#define SOCLIB_CABA_VCI_LOCKS_DEBUG 0

#include <systemc>
#include "caba_base_module.h"
#include "vci_target.h"
#include "mapping_table.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciLocks
	: public soclib::caba::BaseModule
{
	enum vci_target_fsm_state_e {
		IDLE,
		WAIT_IDLE,
		WRITE_RSP,
		READ_RSP,
		ERROR_RSP,
	};

	sc_signal<int> r_vci_fsm;
    sc_signal<typename vci_param::srcid_t> r_buf_srcid;
    sc_signal<typename vci_param::trdid_t> r_buf_trdid;
    sc_signal<typename vci_param::pktid_t> r_buf_pktid;
    sc_signal<typename vci_param::eop_t>   r_buf_eop;
    sc_signal<bool>   r_buf_value;

    // Activity counters
    uint32_t m_cpt_read;   // READ (teset and set) access 
    uint32_t m_cpt_write;  // WRITE (reset) access
    uint32_t m_cpt_error;  // ERROR count
    uint32_t m_cpt_idle;   // IDLE cycles count

	soclib::common::Segment m_segment;
	bool *m_contents;

#if SOCLIB_CABA_VCI_LOCKS_DEBUG
    uint32_t *m_taker_srcid;
    size_t m_max_seen;
#endif

protected:
	SC_HAS_PROCESS(VciLocks);

public:
    sc_in<bool> p_resetn;
    sc_in<bool> p_clk;
    soclib::caba::VciTarget<vci_param> p_vci;

    VciLocks(
        sc_module_name insname,
        const soclib::common::IntTab &index,
        const soclib::common::MappingTable &mt);
    ~VciLocks();

private:
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_VCI_LOCKS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

