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
 *         From a prvious work by Nicolas Pouillon (fifo_idct)
 *
 * Maintainers: alain
 */

///////////////////////////////////////////////////////////////////////////
// This component implements a simple hardware coprocessor
// performing the 2D Discrete Cosinus Transform on a 8*8 block of pixels.
// It should be connected to a vci_mwmr_dma component.
// - It reads the input vector (64 32 bits words) on the input port.
// - It returns the result vector (N words) on one output port.
// - It has only one configuration register: RUNNING
// - It has no status register.
// This component does no define a specific hardware architecture for
// the DCT algorithm: The computation is done in zero cycles, 
// and the execution latency can be emulated with the "exec_latency"
// constructor parameter. The read and write latencies are accurately 
// reprocduced by this component.
//////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_COPROC_DCT_H_
#define SOCLIB_COPROC_DCT_H_

#include <systemc>

#include "caba_base_module.h"
#include "coproc_ports.h"

namespace soclib { namespace caba {

///////////////////////////////////////////////////
class CoprocDct : public soclib::caba::BaseModule
///////////////////////////////////////////////////
{
public:
    sc_core::sc_in<bool>                              p_clk;
    sc_core::sc_in<bool>                              p_resetn;

	soclib::caba::ToCoprocInput<uint32_t, uint8_t>    p_in;
	soclib::caba::FromCoprocOutput<uint32_t, uint8_t> p_out;
    sc_core::sc_in<uint32_t>                          p_config;

private:
	uint32_t m_exec_latency;        // cycles
	uint32_t m_words_per_burst;     // number of words in a burst
	uint32_t m_nb_bursts;           // number of requested bursts 

	enum fsm_states
    {
        IDLE,
        REQ_READ,
        LOAD,
        EXEC,
        REQ_WRITE,
        STORE,
	};

    // registers
    sc_signal<int>       r_fsm;
	sc_signal<size_t>    r_ptr;
	sc_signal<size_t>    r_exec_count;
	int32_t              r_bufin[64];
	int32_t              r_bufout[64];

protected:
    SC_HAS_PROCESS( CoprocDct );

public:
    CoprocDct( sc_core::sc_module_name  insname,
               const uint32_t           burst_size, 
               const uint32_t           exec_latency );

    void print_trace();

private:
    void do_dct_8x8();
    void transition();
	void genMoore();

};

}}

#endif 
