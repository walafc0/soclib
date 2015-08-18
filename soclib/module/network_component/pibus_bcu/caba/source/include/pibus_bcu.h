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
// This component is a simplified PIBUS controler.
// The Three basic functionnalities are :
// - arbitration between masters requests.
// - selection of the target by decoding the MSB address bits.
// - Time-out when the target does not complete the transaction.
// The simplifications and modifications are :
// - The default master mechanism is not supported.
// - Only four values are supported for the ACK signal:
//   PI_ACK_RDY, PI_ACK_WAT, PI_ACK_ERR, PI_ACK_RTR.
// - The arbitration policy between masters is round-robin.
// The bus is granted to a new master in the FSM_IDLE state 
// (the bus is not used), and in the FSM_DT state (last cycle 
// of a transaction) when the ACK signal is not PI_ACK-WAT.
// The COUNT_REQ[i] register counts the total number of transaction 
// requests for master i. The COUNT_WAIT[i] register counts the total
// number of wait cycles for master i.
// This component use the Segment Table to build the Target ROM table, 
// that decode the address MSB bits and gives the the selected target 
// index to generate the SEL[i] signals.
//////////////////////////////////////////////////////////////////////////
// This component has 5 "constructor" parameters :
// - sc_module_name	name		: instance name
// - pibusSegmentTable	segtab		: segment table
// - int 		nb_master       : number of PIBUS masters   
// - int 		nb_slave        : number of PIBUS slaves  
// - int 		time_out	: maximum number of wait cycles   
//////////////////////////////////////////////////////////////////////////

#ifndef PIBUS_BCU_H_
#define PIBUS_BCU_H_

#include <inttypes.h>
#include <systemc>
#include "mapping_table.h"
#include "pibus_signals.h"
#include "pibus_bcu_ports.h"
#include "caba_base_module.h"

namespace soclib { namespace caba {

	using namespace sc_core;

class PibusBcu 
	: public soclib::caba::BaseModule
{

	//	STRUCTURAL PARAMETERS
	const soclib::common::AddressDecodingTable<uint32_t, int> m_target_table;
	const size_t m_nb_master;	// number of masters on the bus
	const size_t m_nb_target;	// number of slaves on the bus
	const uint32_t m_time_out;	// Time-out value

protected:

	SC_HAS_PROCESS(PibusBcu);

public:

	//	I/O PORTS
	sc_in<bool>  				p_clk;	 
	sc_in<bool>  				p_resetn;  
	sc_in<bool>				*p_req;
	sc_out<bool>				*p_gnt;
	sc_out<bool>				*p_sel;

	//sc_in<sc_dt::sc_uint<32> >			p_a;
	//sc_in<bool>				p_lock;
	//sc_in<sc_dt::sc_uint<2> >			p_ack;
	//sc_out<bool>				p_tout;
	PibusBcuPort				p_pi;
	

	//	CONSTRUCTOR
	PibusBcu (	sc_module_name 				name,
			const soclib::common::MappingTable 	&segtab,
			size_t					nb_master,
			size_t					nb_slave,
			uint32_t				time_out);
	~PibusBcu();

private:
	// 	REGISTERS
	sc_signal<int> 	r_fsm_state;		// FSM state
	sc_signal<int>	r_current_master;	// current master index
	sc_signal<size_t>	r_tout_counter;		// time-out counter
	sc_signal<size_t>	*r_req_counter;		// number of requests (per master)
	sc_signal<uint32_t>	*r_wait_counter;	// number of wait cycles (per master)

	// 	FSM states
	enum fms_state_e {
	FSM_IDLE	= 0,
	FSM_AD		= 1,
	FSM_DTAD	= 2,
	FSM_DT		= 3,
	};

	// 	METHODS
	void transition(); 
	void genMealy_gnt(); 
	void genMealy_sel();
	void genMoore();

}; // end class PibusBcu

}} // end namespace

#endif
