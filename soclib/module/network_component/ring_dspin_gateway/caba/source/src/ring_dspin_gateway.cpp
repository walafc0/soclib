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
 * Authors  : Franck WAJSBÜRT, Abdelmalek SI MERABET, Alain Greiner
 * Date     : july 2010
 * Copyright: UPMC - LIP6
 */

#include "../include/ring_dspin_gateway.h"

namespace soclib { namespace caba {

using namespace soclib::common;
using namespace soclib::caba;

#define tmpl(x) template<int cmd_width, int rsp_width> x RingDspinGateway<cmd_width, rsp_width>

/////////////////////////////////////////////////////
tmpl(/**/)::RingDspinGateway( sc_module_name insname,
                                    const MappingTable 	&mt,
                                    const IntTab 	&ringid,
                                    bool  		alloc_init,
                                    bool  		alloc_target,
                                    bool  		local,
                                    const int 		&fifo_depth)
                        : soclib::caba::BaseModule(insname) 
{  
std::ostringstream o;
o << name() << "_hg_initiator";
hg_initiator= new RingDspinHalfGatewayInitiator<cmd_width, rsp_width>(o.str().c_str(), 
                                                                      alloc_init, 
                                                                      fifo_depth, 
                                                                      mt, 
                                                                      ringid, 
                                                                      local);
std::ostringstream p;
p << name() << "_hg_target";
hg_target =    new RingDspinHalfGatewayTarget<cmd_width, rsp_width>(p.str().c_str(), 
                                                                      alloc_target, 
                                                                      fifo_depth, 
                                                                      mt, 
                                                                      ringid, 
                                                                      local);

    hg_initiator->p_clk			(p_clk);
    hg_initiator->p_resetn		(p_resetn);
    hg_initiator->p_ring_in		(p_ring_in);
    hg_initiator->p_ring_out		(m_ring_signal);
    hg_initiator->p_gate_cmd_in		(p_gate_cmd_in);  
    hg_initiator->p_gate_rsp_out	(p_gate_rsp_out); 
		         
    hg_target->p_clk			(p_clk);
    hg_target->p_resetn			(p_resetn);
    hg_target->p_ring_in		(m_ring_signal);
    hg_target->p_ring_out		(p_ring_out);                 
    hg_target->p_gate_cmd_out		(p_gate_cmd_out);       
    hg_target->p_gate_rsp_in 		(p_gate_rsp_in);       
}

/////////////////////////////// 
tmpl(/**/)::~RingDspinGateway()
{
    delete hg_target;
    delete hg_initiator;
}

}} // end namespace
