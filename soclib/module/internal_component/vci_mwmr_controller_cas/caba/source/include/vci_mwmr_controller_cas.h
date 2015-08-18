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
 * Maintainers: nipo
 *
 * Based on previous works by Etienne Faure & Alain Greiner, 2005
 *
 * E. Faure: Communications matérielles-logicielles dans les systèmes
 * sur puce orientés télécommunications.  PhD thesis, UPMC, 2007
 *
 * Maintainers: nipo
 *              karaoui mohamed
 */
#ifndef SOCLIB_VCI_MWMR_CONTROLLER_CAS_H
#define SOCLIB_VCI_MWMR_CONTROLLER_CAS_H

#include <systemc>
#include "vci_target_fsm.h"
#include "vci_initiator.h"
#include "caba_base_module.h"
#include "mapping_table.h"
#include "fifo_ports.h"

#include "mwmr_controller_cas.h"

namespace soclib { namespace caba {

using namespace sc_core;

namespace Mwmr {
struct fifo_state_s;
}

    template<typename vci_param>
class VciMwmrControllerCas
	: public caba::BaseModule
{
private:
    soclib::caba::VciTargetFsm<vci_param, true> m_vci_target_fsm;
    const uint32_t m_ident;

    bool on_write(int seg, typename vci_param::addr_t addr, typename vci_param::data_t data, int be);
    bool on_read(int seg, typename vci_param::addr_t addr, typename vci_param::data_t &data);
    void reset();
    void transition();
    void genMoore();
    void elect();

    const size_t m_fifo_to_coproc_depth;
    const size_t m_fifo_from_coproc_depth;
    const size_t m_plaps;
    const size_t m_n_to_coproc;
    const size_t m_n_from_coproc;
    const size_t m_n_all;
    const size_t m_n_config;
    const size_t m_n_status;

	typedef struct Mwmr::fifo_state_s fifo_state_t;
	fifo_state_t * const m_all_state;
	fifo_state_t * const m_to_coproc_state;
	fifo_state_t * const m_from_coproc_state;

	sc_signal<uint32_t> *r_config;

	sc_signal<bool> r_pending_reset;

    sc_signal<bool>     r_status_modified;
	sc_signal<uint32_t> r_init_fsm;
	sc_signal<uint32_t> r_rsp_fsm;
	sc_signal<uint32_t> r_plen;
	sc_signal<uint32_t> r_cmd_count;
	sc_signal<uint32_t> r_part_count;
	sc_signal<uint32_t> r_rsp_count;
	sc_signal<uint32_t> r_current_rptr;
	sc_signal<uint32_t> r_current_wptr;
	sc_signal<uint32_t> r_current_usage;

    uint32_t m_n_elect;
    uint32_t m_n_lock_spin;
    uint32_t m_n_bailout;
    uint32_t m_n_xfers;

	enum SoclibMwmrWay m_config_way;
	size_t m_config_no;
	fifo_state_t *m_config_fifo;

	fifo_state_t *m_current;
	size_t m_last_elected;
    size_t m_max_burst;

	void rehashConfigFifo();

    soclib_static_assert(vci_param::K >= 2);

protected:
    SC_HAS_PROCESS(VciMwmrControllerCas);

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    VciTarget<vci_param> p_vci_target;
    VciInitiator<vci_param> p_vci_initiator;
	FifoInput<uint32_t> *p_from_coproc;
	FifoOutput<uint32_t> *p_to_coproc;
	sc_core::sc_out<uint32_t> *p_config;
	sc_core::sc_in<uint32_t> *p_status;

	~VciMwmrControllerCas();

	VciMwmrControllerCas(
		sc_module_name name,
		const MappingTable &mt,
		const IntTab &srcid,
		const IntTab &tgtid,
		const size_t plaps,
        const size_t fifo_to_coproc_depth,//word size(4 bytes)
        const size_t fifo_from_coproc_depth,//word size
		const size_t n_to_coproc,
		const size_t n_from_coproc,
		const size_t n_config,
		const size_t n_status,
        const size_t max_burst_size = 128);//word
};

}}

#endif /* SOCLIB_VCI_MWMR_CONTROLLER_CAS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

