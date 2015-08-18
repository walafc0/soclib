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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2006
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////////
// This component is a generic VCI compliant system bus interconnect
// (one single VCI transaction at a given time), but the general 
// behaviour reproduce the PIBUS behaviour.
// The four VCI commands (READ, WRITE, LL, SC) are supported.
// The two main functionnalities are :
// - arbitration between masters requests with a round robin priority.
// - selection of the target by decoding the VCI address MSB bits.
// The bus is allocated to one initiator/target couple for all the
// duration of the transaction. The selected initiator and target
// are identified by the r_init_index & r_target_index registers.
// The bus total throughput is N VCI word per (N+1) cycles
// (one lost cycle between two transactions).
// To reach this throughput, the bus is allocated in the IDLE state
// (if the bus is not used), and in the RSP state (when the last word 
// of the previous response packet is transfered).
// With a fast target (no wait state) all transaction have a latency
// of N+1 cycles (for VCI advanced packets) :
// - READ transaction : one cycle for command, N cycles for response.
// - WRITE transaction : N cycles for command, one cycle for response.
//
// The r_vci_counter[i][j] registers count the total number of transaction 
// requests for initiator i and target j. 
// The r_cycle counter register counts the number of cycles from RESET.
//
// Implementation note :
// This component uses the Segment Table to build the routing table ROM, 
// that decode the address MSB bits and gives the target index.
// The data-path is implemented as two purely combinational multiplexors
// for command & response packets respectively. Those multiplexors
// are controled by  the r_init_index & r_target_index registers.
// Therefore, this component is implement by four Mealy functions:
// - genMealy_cmdval controls the cmdval output signals
// - genMealy_cmdack controls the cmdack output signals
// - genMealy_rspval controls the rspval output signals
// - genMealy_rspack controls the rspack output signals
//////////////////////////////////////////////////////////////////////////
// This component has 4 "constructor" parameters :
// - sc_module_name	name		: instance name
// - pibusSegmentTable	segtab		: segment table
// - int 		nb_master       : number of VCI initiators  
// - int 		nb_slave        : number of VCI targets  
//////////////////////////////////////////////////////////////////////////

#ifndef VCI_VGSB_H
#define VCI_VGSB_H

#include <inttypes.h>
#include <systemc>
#include "mapping_table.h"
#include "address_decoding_table.h"
#include "vci_initiator.h"
#include "vci_target.h"

namespace soclib { namespace caba {

/////////////////////////////
template<typename vci_param>
class VciVgsb  
	: public sc_core::sc_module
{
	// FSM states
	enum vgsb_fms_state_e {
	FSM_IDLE,
	FSM_CMD,
	FSM_RSP,
	};

	// Registers
	sc_signal<int> 							r_fsm;
	sc_signal<size_t>						r_initiator_index;
	sc_signal<size_t>						r_target_index;
	sc_signal<uint32_t>						**r_vci_counter;	
	sc_signal<uint32_t>						r_cycle;
	
	// constants
	const soclib::common::AddressDecodingTable<uint64_t, size_t> m_routing_table;
	const size_t 							                     m_nb_initiator;	
	const size_t 							                     m_nb_target;

protected:

	SC_HAS_PROCESS(VciVgsb);

public:

	// Ports
	sc_in<bool>  							p_clk;	 
	sc_in<bool>  							p_resetn;  
	soclib::caba::VciInitiator<vci_param>   *p_to_target;
	soclib::caba::VciTarget<vci_param>      *p_to_initiator;

	// constructor & destructor
	VciVgsb( sc_module_name                 name,
             soclib::common::MappingTable 	&maptab,
             size_t	                        nb_master,
             size_t	                        nb_slave,
             size_t                         default_target = 0 );

	~VciVgsb();

	// member functions
	void transition(); 
	void genMealy_rspval();
	void genMealy_rspack();
	void genMealy_cmdval();
	void genMealy_cmdack();
    void print_trace();

}; // end class VciVgsb

}} // end namespace

#endif
