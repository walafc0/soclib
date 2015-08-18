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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2015
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////////
// This component implements a simple hardware coprocessor
// performing the Greater Common Divider computation on two vectors
// OPA & OPB containing 32 bits unsigned integers.
// It should be connected to a vci_mwmr_dma component.
// - It reads the two OPA & OPB vectors (N words) on two input ports.
// - It returns the result vector (N words) on the output port.
// - It has only one configuration register: RUNNING
// - It has no status register.
// The N value is defined by the "burst_size" hardware parameter.
//////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_COPROC_GCD_H_
#define SOCLIB_COPROC_GCD_H_

#include <systemc>

#include "caba_base_module.h"
#include "coproc_ports.h"

namespace soclib { namespace caba {

///////////////////////////////////////////////////
class CoprocGcd : public soclib::caba::BaseModule
///////////////////////////////////////////////////
{
public:
    sc_core::sc_in<bool>                               p_clk;
    sc_core::sc_in<bool>                               p_resetn;
  	soclib::caba::ToCoprocInput<uint32_t,uint8_t>      p_opa;
  	soclib::caba::ToCoprocInput<uint32_t,uint8_t>      p_opb;
  	soclib::caba::FromCoprocOutput<uint32_t,uint8_t>   p_res;
    sc_core::sc_in<uint32_t>                           p_config;

private:

	enum fsm_states
    {
		IDLE,
		REQ_AB,
        LOAD,
		COMPARE,
		DECR_A,
		DECR_B,
		REQ_RES,
        STORE,
	}; 

    // hardware constants
    uint32_t                m_words_per_burst;    // number of words

    // registers
    sc_signal<int>          r_fsm;      // FSM state
	sc_signal<uint32_t>     r_pta;      // index in bufa
	sc_signal<uint32_t>     r_ptb;      // index in bufb
	uint32_t *              r_bufa;     // buffer[m_burst] for opa
	uint32_t *              r_bufb;     // buffer[m_burst] for opb

protected:
    SC_HAS_PROCESS( CoprocGcd );

public:
    CoprocGcd( sc_core::sc_module_name name,
               const uint32_t          burst_size );

    ~CoprocGcd();

    void print_trace();

private:
    void transition();
	void genMoore();
};

}}

#endif 
