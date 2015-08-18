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
 * Authors  : alain.greiner@lip6.fr 
 * Date     : august 2013
 * Copyright: UPMC - LIP6
 */

#ifndef DSPIN_PACKET_GENERATOR_H
#define DSPIN_PACKET_GENERATOR_H

#include <systemc>
#include "caba_base_module.h"
#include "dspin_interface.h"
#include "generic_fifo.h"
#include "alloc_elems.h"

////////////////////////////////////////////////////////////////////////////
// This component is a synthetic sender/receiver of DSPIN paquets.
// It has two DSPIN ports, and behaves as both a packet generator 
// (controled by the SEND_FSM), and a packet analyser (controled by
// the RECEIVE_FSM). The flit width can be different for send packets 
// and received packets. These flit widths are template parameters.
// - As a packet generator, it cand send unicast or broadcast packets.
//   A first GENERATOR FSM post dated requests in a FIFO. The CMD FSM
//   consume these dated requests as soon as the FIFO is not empty,
//   and try to send unicast or broadcast DSPIN packets. 
//   For an unicast packet, the number of flits is fixed and defined
//   as a constructor parameter (plen), It cannot be less than 2 flits.
//   The number of flits is exactly two flits for a broadcast packet.  
//   The flit width cannot be less than 33 bits (including EOP).
//   For both packet types, the first flit is the header, and the second 
//   flit contains the sender absolute date. 
//   The constructor parameter (load) define the requested (offered) load.
//   The accepted load is computed as : (NB_PACKETS * LENGTH / NB_CYCLES).
// - As a packet analyser, it computes the total number of received packets 
//   (for each type of packet), and the total cumulated latency.
////////////////////////////////////////////////////////////////////////////
// - first flit format in case of a unicast packet :
//  |EOP|   X     |   Y     |---------------------------------------|BC |
//  | 0 | x_width | y_width |  flit_width - (x_width + y_width + 2) | 1 |
//
// - first flit format in case of a broadcast packet :
//  |EOP|  XMIN   |  XMAX   |  YMIN   |  YMAX   |-------------------|BC |
//  | 0 |   5     |   5     |   5     |   5     | flit_width - 22   | 1 |
//
// - second flit format :
//  |EOP|-----------------|         date                                |
//  | * | flit_width - 33 |          32                                 |
////////////////////////////////////////////////////////////////////////////
// It has three constructors parameters :
// - size_t length == number of flits
// - size_t load == LOAD*1000
// - size_t bcp == NB_PACKETS / NB_BROACAST  (optionnal)
////////////////////////////////////////////////////////////////////////////

namespace soclib { namespace caba {

// FSM states
enum
{ 
	SEND_IDLE,
	SEND_UNICAST,
    SEND_BROADCAST,
};

enum
{
    RECEIVE_IDLE,
    RECEIVE_UNICAST,
    RECEIVE_BROADCAST,
    RECEIVE_WAIT_EOP,
};

template<int cmd_width, int rsp_width>
class DspinPacketGenerator
: public soclib::caba::BaseModule
{			

protected:

	SC_HAS_PROCESS(DspinPacketGenerator);

public:

	// ports
	sc_core::sc_in<bool>                  p_clk;
	sc_core::sc_in<bool>                  p_resetn;
	soclib::caba::DspinInput<rsp_width>   p_in;
	soclib::caba::DspinOutput<cmd_width>  p_out;

	// constructor 
	DspinPacketGenerator( sc_module_name  name,
                          const size_t          srcid,	     // source identifier
                          const size_t          length,      // unicast packet length
                          const size_t          load,        // requested load * 1000
                          const size_t          fifo_depth,  // fifo depth
                          const size_t          bcp );       // broadcast period
private:

	//  registers 
	sc_core::sc_signal<uint32_t>  r_cycles;	             // cycles counter (date)

    sc_core::sc_signal<uint32_t>  r_fifo_posted;         // number of posted requests

	sc_core::sc_signal<int>	      r_send_fsm;            // SEND state
	sc_core::sc_signal<size_t>    r_send_length;	     // flit counter
	sc_core::sc_signal<size_t>    r_send_dest;           // packet destination (x,y)
	sc_core::sc_signal<uint32_t>  r_send_date;           // request date 
	sc_core::sc_signal<uint32_t>  r_send_packets;        // number of unicast packets
	sc_core::sc_signal<uint32_t>  r_send_bc_packets;     // number of broadcast packets

	sc_core::sc_signal<int>	      r_receive_fsm;         // RECEIVE FSM state
	sc_core::sc_signal<size_t>    r_receive_packets;     // number of unicast packets
	sc_core::sc_signal<size_t>    r_receive_latency;     // cululated unicast latency
	sc_core::sc_signal<size_t>    r_receive_bc_packets;  // number of broadcast packets
	sc_core::sc_signal<size_t>    r_receive_bc_latency;  // cumulated broadcast latency

    // Fifo from GENERATOR FSM to SEND FSM
    GenericFifo<uint64_t>         r_date_fifo;

	// structural variables
	const size_t                  m_length;              // unicast packet length
	const size_t                  m_load;	             // requested load
    const size_t                  m_bcp;		         // broadcast period
    const size_t                  m_srcid;	             // seed for random

	// methods 
	void transition();
	void genMoore();

public:

    void print_trace();
    void print_stats();

}; // end class DspinPacketGenerator
	
}} // end namespace

#endif // end DSPIN_PACKET_GENERATOR_H
