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
 * Copyright (c) UPMC, Lip6, SoC
 *         Alain Greiner <alain.greiner@lip6.fr>, 2007
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////
// This hardware component is a VCI/PIBUS protocol converter.
// It behaves as a target on the PIBUS, and as an initiator  
// on the VCI side. It can be used by a VCI target to interface 
// a PIBUS based system. 
// It contains a single FSM that controls the three 
// interfaces: VCI command, VCI response, and PIBUS.        
// Therefore, the throuhput cannot be larger than 2 cycles     
// per 32 bits word, even in case of burst.
// The supported PIBUS response codes are PI_ACK_RDY, PI_ACK_WAT
// and PI_ACK_ERR.  
/////////////////////////////////////////////////////////////////////

#ifndef VCI_PI_TARGET_WRAPPER_H_
#define VCI_PI_TARGET_WRAPPER_H_

#include <systemc>
#include "pibus_target_ports.h"
#include "caba_base_module.h"
#include "vci_initiator.h"

namespace soclib { namespace caba {

	using namespace sc_core;

template<typename vci_param>
class VciPiTargetWrapper 
	: public soclib::caba::BaseModule
{

protected:
	SC_HAS_PROCESS(VciPiTargetWrapper);

public:
	// ports
	sc_in<bool> 				p_clk;
	sc_in<bool>   				p_resetn;
	sc_in<bool>				p_sel;
	soclib::caba::PibusTarget 	p_pi;
	soclib::caba::VciInitiator<vci_param>	p_vci;
	
	// constructor / destructor
	VciPiTargetWrapper(sc_module_name	insname);

	enum fsm_state_e {
	FSM_IDLE,
	FSM_CMD_READ,
	FSM_RSP_READ,
	FSM_CMD_WRITE,
	FSM_RSP_WRITE,
	};

private:
	// internal registers
	sc_signal<int>				r_fsm_state;
	sc_signal<int>				r_adr;
	sc_signal<int>				r_opc;

	// methods
	void transition();
	void genMealy();

	soclib_static_assert(vci_param::N == 32); // checking VCI address size
	soclib_static_assert(vci_param::B == 4); // checking VCI data size
};

}}
		
#endif // VCI_PI_TARGET_WRAPPER_H_
