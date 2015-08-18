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
// This component implements an MWMR compliant hardware coprocessor, 
// emulating a single channel DMA controller: It move a burst (N words) 
// from the input port to the output port, without modification. 
// It can provide a better throughput than a "classical" single channel
// DMA controller, because it contains two buffers acting as a two slots 
// FIFO, and two separated FSMs : Load and Store.
// As each buffer can store a complete burst, it supports parallel 
// pipe-lined read and write transactions on the VCI network.
// It should be connected to a vci_mwmr_dma component.
// - It reads a burst (N words) on the p_load port.
// - It write a burst (N words) on the p_store port.
// - It has only one configuration register: RUNNING
// - It has no status register.
// The N value is defined by the "burst_size" hardware parameter.
//////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_COPROC_CPY_H_
#define SOCLIB_COPROC_CPY_H_

#include <systemc>

#include "caba_base_module.h"
#include "coproc_ports.h"

namespace soclib { namespace caba {

///////////////////////////////////////////////////
class CoprocCpy : public soclib::caba::BaseModule
///////////////////////////////////////////////////
{
public:
    sc_core::sc_in<bool>                               p_clk;
    sc_core::sc_in<bool>                               p_resetn;
  	soclib::caba::ToCoprocInput<uint32_t,uint8_t>      p_load;
  	soclib::caba::FromCoprocOutput<uint32_t,uint8_t>   p_store;
    sc_core::sc_in<uint32_t>                           p_config;

private:

	enum fsm_states  // for both load and store FSMs
    {
		IDLE,
        WAIT,
		REQ,
        MOVE,
	}; 

    // hardware constants
    const uint32_t          m_burst;         // number of words

    // registers
    sc_signal<int>          r_load_fsm;      // FSM state
    sc_signal<size_t>       r_load_bufid;    // current buffer index for load
    sc_signal<size_t>       r_load_word;     // current word index for load

    sc_signal<int>          r_store_fsm;     // FSM state
    sc_signal<size_t>       r_store_bufid;   // current buffer index for load
    sc_signal<size_t>       r_store_word;    // current word index for load

    sc_signal<bool>         r_full[2];       // set/reset flip-flop for the two buffers

	uint32_t                r_buf[2][64];    // data buffers (two bursts)

protected:
    SC_HAS_PROCESS( CoprocCpy );

public:
    CoprocCpy( sc_core::sc_module_name name,
               const uint32_t          burst_size );

    void print_trace();

private:
    void transition();
	void genMoore();
};

}}

#endif 
