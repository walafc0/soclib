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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2009
 *
 * Maintainers: alain
 */

#ifndef VCI_GCD_COPROCESSOR_H
#define VCI_GCD_COPROCESSOR_H

#include <systemc>
#include "vci_target.h"
#include "mapping_table.h"
#include "int_tab.h"
#include "segment.h"

namespace soclib {
namespace caba {

using namespace sc_core;

/////////////////////////////
template<typename vci_param>
class VciGcdCoprocessor
	: public sc_core::sc_module
{
	enum gcd_coproc_vci_fsm_state_e {
		VCI_GET_CMD,
		VCI_RSP_OPA,
		VCI_RSP_OPB,
		VCI_RSP_START,
		VCI_RSP_STATUS,
		VCI_RSP_RESULT,
	};
	enum gcd_coproc_exe_fsm_state_e {
		EXE_IDLE,
		EXE_COMPARE,
		EXE_DECA,
		EXE_DECB,
	};

	// Registers
	sc_core::sc_signal<int>					r_vci_fsm;
	sc_core::sc_signal<int>					r_exe_fsm;
        sc_core::sc_signal<typename vci_param::srcid_t> 	r_srcid;
        sc_core::sc_signal<typename vci_param::trdid_t> 	r_trdid;
        sc_core::sc_signal<typename vci_param::pktid_t> 	r_pktid;
        sc_core::sc_signal<typename vci_param::data_t>   	r_opa;
        sc_core::sc_signal<typename vci_param::data_t>   	r_opb;

	soclib::common::Segment 				m_segment;

protected:
	SC_HAS_PROCESS(VciGcdCoprocessor);

public:
	// ports
        sc_core::sc_in<bool> 					p_resetn;
        sc_core::sc_in<bool> 					p_clk;
        soclib::caba::VciTarget<vci_param> 			p_vci;

	// constructor & destructor
	VciGcdCoprocessor( 	sc_core::sc_module_name insname,
        			const soclib::common::IntTab &index,
        			const soclib::common::MappingTable &mt);
	~VciGcdCoprocessor();

        void print_trace();

private:
	// member functions
	void transition();
	void genMoore();

}; // end class VciGcdCoprocessor

}}

#endif /* SOCLIB_CABA_VCI_LOCKS_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

