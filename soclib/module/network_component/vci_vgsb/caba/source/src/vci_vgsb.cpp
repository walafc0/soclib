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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2009
 *
 * Maintainers: alain 
 */

#include "vci_vgsb.h"
#include "alloc_elems.h"

using namespace sc_core;
using namespace soclib::caba;
using namespace soclib::common;

namespace soclib { namespace caba {

////////////////////////////
template<typename vci_param>
VciVgsb<vci_param>::VciVgsb ( sc_module_name    name,
                       	      MappingTable      &maptab,
                              size_t            nb_master,
                              size_t            nb_target,
                              size_t            default_target_id )
    : sc_core::sc_module(name),
      r_fsm( "r_fsm" ),
      r_initiator_index( "r_initiator_index" ),
      r_target_index( "r_target_index" ),
      r_vci_counter(alloc_elems<sc_signal<uint32_t> >("r_vci_counter", nb_master, nb_target)),
      r_cycle( "r_cycle" ),
      m_routing_table( maptab.getGlobalIndexFromAddress( default_target_id) ),
      m_nb_initiator( nb_master ),
      m_nb_target( nb_target ),
      p_clk( "clk" ),
      p_resetn( "resetn" ),
      p_to_target( alloc_elems<VciInitiator<vci_param> >("p_to_target", nb_target) ),
      p_to_initiator( alloc_elems<VciTarget<vci_param> >("p_to_initiator", nb_master) )
{
    std::cout << "  - Building VciVgsb : " << name << std::dec << std::endl
              << "    => targets        = "  << nb_target << std::endl
              << "    => initiators     = "  << nb_master << std::endl
              << "    => default target = "  << default_target_id << std::endl;

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMealy_rspval);
	dont_initialize();
	sensitive << p_clk.neg();
	for ( size_t i=0 ; i<nb_target  ; i++ ) sensitive << p_to_target[i];

	SC_METHOD(genMealy_rspack);
	dont_initialize();
	sensitive << p_clk.neg();
	for ( size_t i=0 ; i<nb_master ; i++ ) sensitive << p_to_initiator[i];

	SC_METHOD(genMealy_cmdval);
	dont_initialize();
	sensitive << p_clk.neg();
	for ( size_t i=0 ; i<nb_master ; i++ ) sensitive << p_to_initiator[i];

	SC_METHOD(genMealy_cmdack);
	dont_initialize();
	sensitive << p_clk.neg();
	for ( size_t i=0 ; i<nb_target  ; i++ ) sensitive << p_to_target[i];

	if ( !m_routing_table.isAllBelow( nb_target ) ) {
		std::cout << "error in vci_gsb component" << std::endl;
		std::cout << "one target index is larger than the number of targets" << std::endl;
		exit(0);
	}
} // end constructor

////////////////////////////
template<typename vci_param>
VciVgsb<vci_param>::~VciVgsb()
{
    soclib::common::dealloc_elems(p_to_target, m_nb_target);
    soclib::common::dealloc_elems(p_to_initiator, m_nb_initiator);
    soclib::common::dealloc_elems(r_vci_counter, m_nb_initiator, m_nb_target);
} // end destructor

////////////////////////////
template<typename vci_param>
void VciVgsb<vci_param>::print_trace()
{
    const char* state_str[] = { "IDLE", "CMD", "RSP" };
    std::cout << "Vgsb : state = " << state_str[r_fsm] 
              << " / index_ini = " << r_initiator_index 
              << " / index_tgt = " << r_target_index << std::endl;
}

///////////////////////////
template<typename vci_param>
void VciVgsb<vci_param>::transition()
{
    if (p_resetn == false) 
    {
        r_fsm = FSM_IDLE;
        r_initiator_index = 0;
        r_target_index = 0;
	    r_cycle = 0;
        for(size_t i=0 ; i<(m_nb_initiator) ; i++) 
        {
            for(size_t j=0 ; j<(m_nb_target) ; j++) 
            { 
		        r_vci_counter[i][j] = 0; 
            }
        }
        return;
    } 

#ifdef SOCLIB_MODULE_DEBUG
std::cout << "*********************************** cycle = " << r_cycle.read() << std::endl;
std::cout << "vgsb fsm = " << r_fsm.read() << std::endl;
std::cout << "vgsb ini = " << r_initiator_index.read() << std::endl;
std::cout << "vgsb tgt = " << r_target_index.read() << std::endl;
#endif

    r_cycle = r_cycle + 1;

    switch( r_fsm.read() ) {
    //////////////
	case FSM_IDLE:
    {
        for (size_t x = 0 ; x < m_nb_initiator ; x++) 
        {
            size_t ini = ( x + 1 + r_initiator_index.read() ) % m_nb_initiator;
            if( p_to_initiator[ini].cmdval.read() ) 
            {
                size_t tgt = m_routing_table[p_to_initiator[ini].address.read()];
                r_initiator_index = ini;
                r_target_index = tgt;
                r_vci_counter[ini][tgt] = r_vci_counter[ini][tgt].read() + 1;
                r_fsm = FSM_CMD;
                break;
            } // end if
        } // end for
    }
    break;
    /////////////
	case FSM_CMD:
    {
        if ( p_to_initiator[r_initiator_index.read()].eop.read() && 
             p_to_initiator[r_initiator_index.read()].cmdval.read() && 
             p_to_target[r_target_index.read()].cmdack.read() ) r_fsm = FSM_RSP;  
    }
    break;
    /////////////
	case FSM_RSP:
    {
        if ( p_to_target[r_target_index.read()].reop && 
             p_to_target[r_target_index.read()].rspval && 
             p_to_initiator[r_initiator_index.read()].rspack ) 
        { 
            bool found = false; 
	        for (size_t x = 0 ; (x < m_nb_initiator) and not found ; x++) 
            {
                size_t ini = ( x + 1 + r_initiator_index.read() ) % m_nb_initiator;
                if( p_to_initiator[ini].cmdval.read() ) 
                {
                    size_t tgt = m_routing_table[p_to_initiator[ini].address.read()];
                    r_initiator_index = ini;
                    r_target_index = tgt;
                    r_vci_counter[ini][tgt] = r_vci_counter[ini][tgt].read() + 1;
                    r_fsm = FSM_CMD;
                    found = true;
                } // end if
            } // end for
            if ( not found ) r_fsm = FSM_IDLE;
        } // end if
        break;
    }
    } // end switch FSM

} // end transition()

////////////////////////////
template<typename vci_param>
void VciVgsb<vci_param>::genMealy_cmdval()
{
    size_t ini = (size_t)r_initiator_index.read();
    size_t tgt = (size_t)r_target_index.read();

    if ( r_fsm.read() == FSM_CMD ) {	// cmd packet transfer
        for (size_t x = 0 ; x<m_nb_target ; x++) {
	    if ( x == tgt ) {
                p_to_target[x].cmdval  = p_to_initiator[ini].cmdval.read();
                p_to_target[x].wdata   = p_to_initiator[ini].wdata.read();
                p_to_target[x].address = p_to_initiator[ini].address.read();
                p_to_target[x].be      = p_to_initiator[ini].be.read();
                p_to_target[x].plen    = p_to_initiator[ini].plen.read();
                p_to_target[x].srcid   = p_to_initiator[ini].srcid.read();
                p_to_target[x].trdid   = p_to_initiator[ini].trdid.read();
                p_to_target[x].pktid   = p_to_initiator[ini].pktid.read();
                p_to_target[x].cmd     = p_to_initiator[ini].cmd.read();
                p_to_target[x].contig  = p_to_initiator[ini].contig.read();
                p_to_target[x].eop     = p_to_initiator[ini].eop.read();
            } else {
                p_to_target[x].cmdval = false;
            }
        }
    } else {				// no cmd packet transfer
        for (size_t x = 0 ; x<m_nb_target ; x++) {
            p_to_target[x].cmdval = false;
        }
    }
} // end genMealy_cmdval() 

////////////////////////////
template<typename vci_param>
void VciVgsb<vci_param>::genMealy_cmdack()
{
    size_t ini = (size_t)r_initiator_index.read();
    size_t tgt = (size_t)r_target_index.read();

    if ( r_fsm.read() == FSM_CMD ) {   // cmd packet transfer
        for (size_t x = 0 ; x<m_nb_initiator ; x++) {
	    if ( x == ini ) {
                p_to_initiator[x].cmdack = p_to_target[tgt].cmdack.read();
            } else {
                p_to_initiator[x].cmdack = false;
            }
        }
    } else {				// no cmd packet transfer
        for (size_t x = 0 ; x<m_nb_initiator ; x++) {
            p_to_initiator[x].cmdack = false;
	}
    }
} // end genMealy_cmdack()

////////////////////////////
template<typename vci_param>
void VciVgsb<vci_param>::genMealy_rspval()
{
    size_t ini = (size_t)r_initiator_index.read();
    size_t tgt = (size_t)r_target_index.read();

    if ( r_fsm.read() == FSM_RSP ) {	// response packet transfer
        for (size_t x = 0 ; x<m_nb_initiator ; x++) {
	    if ( x == ini ) {
                p_to_initiator[x].rspval = p_to_target[tgt].rspval.read();
                p_to_initiator[x].rdata  = p_to_target[tgt].rdata.read();
                p_to_initiator[x].rerror = p_to_target[tgt].rerror.read();
                p_to_initiator[x].rsrcid = p_to_target[tgt].rsrcid.read();
                p_to_initiator[x].rtrdid = p_to_target[tgt].rtrdid.read();
                p_to_initiator[x].rpktid = p_to_target[tgt].rpktid.read();
                p_to_initiator[x].reop   = p_to_target[tgt].reop.read();
            } else {
                p_to_initiator[x].rspval = false;
            }
        }
    } else { 				// no response packet transfer
        for (size_t x = 0 ; x<m_nb_initiator ; x++) {
            p_to_initiator[x].rspval = false;
        }
    }
} // end genMealy_rspval()

////////////////////////////
template<typename vci_param>
void VciVgsb<vci_param>::genMealy_rspack()
{
    size_t ini = (size_t)r_initiator_index.read();
    size_t tgt = (size_t)r_target_index.read();

    if ( r_fsm.read() == FSM_RSP ) {	// response packet transfer
        for (size_t x = 0 ; x<m_nb_target ; x++) {
            if ( x == tgt ) {
                p_to_target[x].rspack = p_to_initiator[ini].rspack.read();
            } else {
                p_to_target[x].rspack = false;
            }
        }
    } else { 				// no response packet transfer
        for (size_t x = 0 ; x<m_nb_target ; x++) {
            p_to_target[x].rspack = false;
        }
    }
} // end genMealy_rspack()

}} // end namespace


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
