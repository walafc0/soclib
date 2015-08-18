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
 * WITHOUT ANY WARr_opaNTY; without even the implied warranty of
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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2009
 *
 * Maintainers: alain
 */

#include "coproc_gcd.h"
#include "alloc_elems.h"

using namespace sc_core;

namespace soclib { namespace caba {

using soclib::common::alloc_elems;
using soclib::common::dealloc_elems;

//////////////////////////////////////////
CoprocGcd::CoprocGcd( sc_module_name name,
                      const uint32_t burst_size )
	: caba::BaseModule( name ),
      p_clk( "clk" ),
      p_resetn( "resetn" ),
      p_opa( "p_opa" ),
      p_opb( "p_opb" ),
      p_res( "p_res" ),
      p_config( "p_config" ),

      m_words_per_burst( burst_size>>2 ),

      r_fsm( "r_fsm" ),
      r_pta( "r_pta" ),
      r_ptb( "r_ptb" )
{
    std::cout << "  - Building CoprocGcd : " << name << std::endl;

    r_bufa = new uint32_t[m_words_per_burst];
    r_bufb = new uint32_t[m_words_per_burst];

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();
	
	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

///////////////////////
CoprocGcd::~CoprocGcd()
{
    delete[] r_bufa;
    delete[] r_bufb;
}

////////////////////////////
void CoprocGcd::transition()
{
	if ( !p_resetn.read() ) 
    {
		r_fsm = IDLE;
		return;
	}

    switch ( r_fsm.read() )
    {
        //////////
        case IDLE:   // test configuration register
        {
            if ( p_config.read() != 0 )
            {
                r_pta = 0;
                r_ptb = 0;
                r_fsm = REQ_AB;
            }
            break;
        }
        ////////////
        case REQ_AB:   // request a burst for both A and B operands
        {
            if ( p_config.read() == 0 )  // soft reset
            {
                r_fsm = IDLE;
                break;
            }

            if ( (p_opa.ack) == true and (p_opb.ack == true) ) 
            {
                r_fsm = LOAD;
            }
            break;
        }
        //////////
        case LOAD:    // load operands into buffers from DMA 
        {
            if ( p_config.read() == 0 )  // soft reset
            {
                r_fsm = IDLE;
                break;
            }

            // get one word from opa fifo
            if ( p_opa.rok  and (r_pta.read() < m_words_per_burst) )
            {
                r_bufa[r_pta.read()] = p_opa.data.read();
                r_pta = r_pta.read() + 1;
            }

            // get one word from opb fifo
            if ( p_opb.rok  and (r_ptb.read() < m_words_per_burst) )
            {
                r_bufb[r_ptb.read()] = p_opb.data.read();
                r_ptb = r_ptb.read() + 1;
            }

            // check load completion
            if ( (r_pta.read() == m_words_per_burst)  and 
                 (r_ptb.read() == m_words_per_burst) )
            {
                r_pta = 0;
                r_ptb = 0;
                r_fsm = COMPARE;
            }
            break;   
        }
        /////////////
        case COMPARE:    // compare bufa[pta] and bufb[pta]   
                         // using only the pta pointer
        {
            if ( p_config.read() == 0 )  // soft reset
            {
                r_fsm = IDLE;
                break;
            }

            if      ( r_bufa[r_pta.read()] < r_bufb[r_ptb.read()] )  // B > A
            {
                r_fsm = DECR_B;
            }
            else if ( r_bufa[r_pta.read()] > r_bufb[r_ptb.read()] )  // A > B
            {
                r_fsm = DECR_A;
            }
            else if ( r_pta.read() == m_words_per_burst-1 )             // A = B / last 
            {
                r_pta = 0;
                r_ptb = 0;
                r_fsm = REQ_RES;
            }
            else                                          // A = B / not last
            {
                r_pta = r_pta.read() + 1;
                r_ptb = r_ptb.read() + 1;
            }
		    break;
        }
        ////////////
        case DECR_A:    // decrement A
        {
            if ( p_config.read() == 0 )  // soft reset
            {
                r_fsm = IDLE;
                break;
            }

            r_bufa[r_pta.read()] = r_bufa[r_pta.read()] - 
                                   r_bufb[r_ptb.read()] ;
            r_fsm = COMPARE;
            break;
        }
        ////////////   // decrement B
        case DECR_B:
        {
            if ( p_config.read() == 0 )  // soft reset
            {
                r_fsm = IDLE;
                break;
            }

            r_bufb[r_pta.read()] = r_bufb[r_ptb.read()] - 
                                   r_bufa[r_pta.read()] ;
            r_fsm = COMPARE;
            break;
        }
        /////////////
        case REQ_RES:   // request a burst for result
        {
            if ( p_config.read() == 0 )  // soft reset
            {
                r_fsm = IDLE;
                break;
            }

            if ( p_res.ack )
            {
                r_fsm = STORE;
            }
            break;
        }
        ///////////
        case STORE:     // store result into DMA
        {
            if ( p_config.read() == 0 )  // soft reset
            {
                r_fsm = IDLE;
                break;
            }

            // put one word into res fifo
            if ( p_res.wok and (r_pta.read() < m_words_per_burst) )
            {
                r_pta = r_pta.read() + 1;
            }

            // check completion
            if ( r_pta.read() == m_words_per_burst-1 )
            {
                r_fsm = IDLE;
            }
            break;
        }
	}
}  // end transition

//////////////////////////
void CoprocGcd::genMoore()
{
    // p_opa port
    p_opa.req    = (r_fsm.read() == REQ_AB);
    p_opa.bursts = 1;
    p_opa.r      = (r_fsm.read() == LOAD);

    // p_opb port
    p_opb.req    = (r_fsm.read() == REQ_AB);
    p_opb.bursts = 1;
    p_opb.r      = (r_fsm.read() == LOAD);

    // p_res port
    p_res.req    = (r_fsm.read() == REQ_RES);
    p_res.bursts = 1;
    p_res.w      = (r_fsm.read() == STORE);
    p_res.data   = r_bufa[r_pta.read()];

}  // end genMoore

/////////////////////////////
void CoprocGcd::print_trace()
{
    const char* gcd_states[] =
    {
    "IDLE",
    "REQ_AB",
    "LOAD",
    "COMPARE",
    "DECR_A",
    "DECR_B",
    "REQ_RES",
    "STORE",
    };

    std::cout << "GCD " << name() << " : " << gcd_states[r_fsm.read()] << std::dec 
              << " / opa[" << r_pta.read() << "] = " << r_bufa[r_pta.read()] 
              << " / opb[" << r_ptb.read() << "] = " << r_bufb[r_ptb.read()] 
              << std::endl;

}  // end print_trace()

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

