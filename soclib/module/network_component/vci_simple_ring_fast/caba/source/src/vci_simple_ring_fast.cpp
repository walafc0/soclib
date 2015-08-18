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
 * Author   : Abdelmalek SI MERABET 
 * Date     : March 2010
 * Copyright: UPMC - LIP6
 */

#include <iostream> 
#include <stdarg.h>
#include "alloc_elems.h"
#include "../include/vci_simple_ring_fast.h"

namespace soclib { namespace caba {

using soclib::common::alloc_elems;
using soclib::common::dealloc_elems;

#define tmpl(x) template<typename vci_param, int ring_cmd_data_size, int ring_rsp_data_size > x VciSimpleRingFast<vci_param, ring_cmd_data_size, ring_rsp_data_size >

////////////////////////////////
//      constructor
//
////////////////////////////////
tmpl(/**/)::VciSimpleRingFast( 
        sc_module_name insname,
        const soclib::common::MappingTable &mt,
        const soclib::common::IntTab &ringid,
        const int &wrapper_fifo_depth,
        int nb_attached_initiator,
        int nb_attached_target)                                             
                : soclib::caba::BaseModule(insname),
                p_clk("clk"),
                p_resetn("resetn"), 
                m_ns(nb_attached_initiator+nb_attached_target), 
                m_nai(nb_attached_initiator),  
                m_nat(nb_attached_target)
{
	p_to_initiator = soclib::common::alloc_elems<soclib::caba::VciTarget<vci_param> >("p_to_initiator", m_nai);
	p_to_target    = soclib::common::alloc_elems<soclib::caba::VciInitiator<vci_param> >("p_to_target", m_nat);

//-- to keep trace on ring traffic
        init_cmd     = new cmd_str[m_nai];
        tgt_rsp      = new rsp_str[m_nat];
	//init_cmd_val = new bool[m_nai];
	tgt_cmd_val  = new bool[m_nat];
	init_rsp_val = new bool[m_nai];
	//tgt_rsp_val  = new bool[m_nat];
//--

	m_ring_initiator = new ring_initiator_t*[m_nai];
	m_ring_target    = new ring_target_t*[m_nat]; 
        m_ring_signal    = new ring_signal_t[m_ns];

        for(int i=0; i<m_nai; ++i) {
                bool alloc_init = (i==0);
		std::ostringstream o;
		o << name() << "_init_" << i;
                m_ring_initiator[i] = new ring_initiator_t(o.str().c_str(), alloc_init, wrapper_fifo_depth, mt, ringid, i, nb_attached_target);
        }

        for(int i=0; i<m_nat; ++i) {
                bool alloc_target = (i==0);
		std::ostringstream o;
		o << name() << "_target_" << i;
                m_ring_target[i]  = new ring_target_t(o.str().c_str(), alloc_target, wrapper_fifo_depth, mt, ringid, i);
        }

        SC_METHOD(transition);
        dont_initialize();
        sensitive << p_clk.pos();

        SC_METHOD(genMoore);
        dont_initialize();
        sensitive << p_clk.neg();
 
}

tmpl(void)::transition()
{


        if ( ! p_resetn.read() ) {
                for(int i=0;i<m_nai;i++) 
                        m_ring_initiator[i]->reset();
                for(int t=0;t<m_nat;t++)
                        m_ring_target[t]->reset();
                return;
        }

// update ring signals Ninitiators-1 iterations
// this is intended in order to break the loop due to dependency existing between ring signals
// this rule is based based on relaxation principle

       for (int niter = 0; niter < m_nai - 1; niter++) {
           for(int i=0;i<m_nai;i++) {
                int h = 0;
                if(i == 0) h = m_ns-1;
                else h = i-1;
                m_ring_initiator[i]->update_ring_signals(m_ring_signal[h], m_ring_signal[i]);
            }

            for(int i=0;i<m_nat;i++){
                m_ring_target[i]->update_ring_signals(m_ring_signal[m_nai+i-1], m_ring_signal[m_nai+i] );
            }
        }

// transition
        for(int i=0;i<m_nai;i++) {
                int h = 0;
                if(i == 0) h = m_ns-1;
                else h = i-1;
                m_ring_initiator[i]->transition(p_to_initiator[i], m_ring_signal[h], init_cmd[i], init_rsp_val[i]);
        }
        for(int t=0;t<m_nat;t++) {
                m_ring_target[t]->transition(p_to_target[t], m_ring_signal[m_nai+t-1], tgt_cmd_val[t], tgt_rsp[t]);
        }

}

tmpl(void)::genMoore()
{


        for(int i=0;i<m_nai;i++) 
                m_ring_initiator[i]->genMoore(p_to_initiator[i]);
        for(int t=0;t<m_nat;t++)
                m_ring_target[t]->genMoore(p_to_target[t]);    

}

//---------------- destructor
tmpl(/**/)::~VciSimpleRingFast()
{

	for(int x = 0; x < m_nai; x++)
		delete m_ring_initiator[x];
	
	for(int x = 0; x < m_nat; x++)
		delete m_ring_target[x];

	delete [] m_ring_initiator;
	delete [] m_ring_target;
        delete [] m_ring_signal;
	
	dealloc_elems(p_to_initiator,m_nai);
	dealloc_elems(p_to_target, m_nat);
}

tmpl(void)::print_trace()
{
	int init_cmd_index = 0;
	bool init_cmd_found   = false;
	int tgt_rsp_index = 0;
	bool tgt_rsp_found = false;

	// cmd trace
	//*-- one initiator has token at one time 
	for(int i=0;i<m_nai;i++) {
	       if(init_cmd[i].cmdval) {
			init_cmd_index = i;
			init_cmd_found = true;
			break;
		}
	
	}

	// rsp trace
	//*-- one target has token at one time 
	for(int t=0;t<m_nat;t++) {
	       if(tgt_rsp[t].rspval) {
			tgt_rsp_index = t;
			tgt_rsp_found = true;
			break;
		}
	
	}
	
	// cmd display
	if(init_cmd_found) {
		//*-- in case of broadcast (on coherence ring), all targets can receive the command at the same time
		for(int t=0;t<m_nat;t++) {
	        	if(tgt_cmd_val[t]) {
				std::cout << "RING " << name() 
			  		  << " -- initiator_" << std::dec << init_cmd_index
			  		  << " ... cmd to ... target_" << t 
                                          << " -state : " << init_cmd[init_cmd_index].state     
                                          << " -flit : "  << std::hex << init_cmd[init_cmd_index].flit
			  		  << std::endl;
			}
		}
	}

	// rsp display
	if(tgt_rsp_found) {
		for(int i=0;i<m_nai;i++) {
	        	if(init_rsp_val[i]) {
				std::cout << "RING " << name() 
			  		  << " ++ target_" << std::dec << tgt_rsp_index
			  		  << " ... rsp to ... initiator_" << i
                                          << " +state : " << tgt_rsp[tgt_rsp_index].state
                                          << " +flit : "  << std::hex << tgt_rsp[tgt_rsp_index].flit 
			  		  << std::endl;
			}
		}
	}

}
}} // end namespace
