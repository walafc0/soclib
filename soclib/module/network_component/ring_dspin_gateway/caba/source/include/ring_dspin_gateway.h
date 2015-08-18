/* -*- c++ -*-
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
 * Authors  : Franck WAJSBÜRT, Abdelmalek SI MERABET, Alain GREINER 
 * Date     : July 2010
 * Copyright: UPMC - LIP6
 */

#ifndef RING_DSPIN_GATEWAY_H_
#define RING_DSPIN_GATEWAY_H_

#include <systemc>
#include "caba_base_module.h"
#include "ring_dspin_half_gateway_target.h"
#include "ring_dspin_half_gateway_initiator.h"
#include "ring_ports.h"
#include "dspin_interface.h"
#include "mapping_table.h"


namespace soclib { namespace caba {

template<int cmd_width, int rsp_width>
class RingDspinGateway : public soclib::caba::BaseModule
{
public:
    // ports
    sc_core::sc_in<bool>				p_clk;
    sc_core::sc_in<bool>				p_resetn;
    soclib::caba::RingIn				p_ring_in;
    soclib::caba::RingOut				p_ring_out;
    soclib::caba::DspinInput<cmd_width> 		p_gate_cmd_in;
    soclib::caba::DspinOutput<cmd_width>		p_gate_cmd_out;
    soclib::caba::DspinInput<rsp_width> 		p_gate_rsp_in;
    soclib::caba::DspinOutput<rsp_width>		p_gate_rsp_out;
		
    // components
    soclib::caba::RingDspinHalfGatewayInitiator<cmd_width,rsp_width> *	hg_initiator;
    soclib::caba::RingDspinHalfGatewayTarget<cmd_width,rsp_width>    *	hg_target;
                
protected:
    SC_HAS_PROCESS(RingDspinGateway);

private:  
    // ring signals
    soclib::caba::RingSignals 				m_ring_signal;

public:
    RingDspinGateway( sc_module_name insname,
                 	const soclib::common::MappingTable 	&mt,
                 	const soclib::common::IntTab 		&ringid,   
                 	bool  					alloc_init,
                 	bool  					alloc_target,
                 	bool  					local,
                 	const int 				&fifo_depth);  
                                          
    ~RingDspinGateway();
};

}} // end

#endif //RING_DSPIN_GATEWAY_H_
