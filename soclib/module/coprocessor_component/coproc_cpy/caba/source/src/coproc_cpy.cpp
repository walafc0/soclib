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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2015
 *
 * Maintainers: alain
 */

#include "coproc_cpy.h"
#include <assert.h>


namespace soclib { namespace caba {


////////////////////////////
void CoprocCpy::transition()
{
	if ( p_resetn.read() == false ) 
    {
		r_load_fsm  = IDLE;
		r_store_fsm = IDLE;
        return;
	}

    ////////////////////////////
    switch ( r_load_fsm.read() )
    ////////////////////////////
    {
        //////////
        case IDLE:    // test coprocessor running
        {
            if ( p_config.read() != 0 ) 
            {
                r_load_bufid = 0;
                r_load_word  = 0;
                r_load_fsm   = WAIT;
            }
            break;
        }
        //////////
        case WAIT:   // wait buf[bufid] empty
        {
            if ( p_config.read() != 0 ) 
            {
                size_t bufid = r_load_bufid.read();
                if ( r_full[bufid].read() == false ) r_load_fsm = REQ;
            }
            else  // soft reset
            {
                r_load_fsm = IDLE;
            }
            break;
        }
        /////////
        case REQ:   // request one burst
        {
            if ( p_config.read() != 0 ) 
            {
                if ( (p_load.ack) == true ) r_load_fsm = MOVE;
            }
            else  // soft reset
            {
                r_load_fsm = IDLE;
            }
            break;
        }
        //////////
        case MOVE:    // load one burst to buf[id]
        {
            if ( p_config.read() != 0 ) 
            {
                if ( p_load.rok.read() )
                {
                    size_t bufid = r_load_bufid.read();
                    size_t word  = r_load_word.read();

                    // write one word into buf[bufid]
                    r_buf[bufid][word] = p_load.data.read();

                    // check burst completion
                    if ( word == (m_burst - 1) ) 
                    {
                        r_full[bufid] = true;
                        r_load_word   = 0;
                        r_load_bufid  = (bufid + 1) % 2;
                        r_load_fsm    = WAIT;
                    }
                    else
                    {
                        r_load_word   = word + 1;
                    }
                }
            }
            else  // soft reset
            {
                r_load_fsm = IDLE;
            }
            break;   
        }
	}  // end switch( r_load_fsm )

    ////////////////////////////
    switch ( r_store_fsm.read() )
    ////////////////////////////
    {
        //////////
        case IDLE:    // test coprocessor running
        {
            if ( p_config.read() != 0 ) 
            {
                r_store_bufid = 0;
                r_store_word  = 0;
                r_store_fsm   = WAIT;
            }
            break;
        }
        //////////
        case WAIT:   // wait buf[bufid] full 
        {
            if ( p_config.read() != 0 ) 
            {
                size_t bufid = r_store_bufid.read();
                if ( r_full[bufid].read() == true ) r_store_fsm = REQ;
            }
            else  // soft reset
            {
                r_store_fsm = IDLE;
            }
            break;
        }
        /////////
        case REQ:   // request one burst
        {
            if ( p_config.read() != 0 ) 
            {
                if ( (p_store.ack) == true ) r_store_fsm = MOVE;
            }
            else  // soft reset
            {
                r_store_fsm = IDLE;
            }
            break;
        }
        //////////
        case MOVE:    // store one burst from buf[id]
        {
            if ( p_config.read() != 0 ) 
            {
                if ( p_store.wok.read() )
                {
                    size_t bufid = r_store_bufid.read();
                    size_t word  = r_store_word.read();

                    // check burst completion
                    if ( word == (m_burst - 1) ) 
                    {
                        r_full[bufid]  = false;
                        r_store_word   = 0;
                        r_store_bufid  = (bufid + 1) % 2;
                        r_store_fsm    = WAIT;
                    }
                    else
                    {
                        r_store_word = word + 1;
                    }
                }
            }
            else  // soft reset
            {
                r_store_fsm = IDLE;
            }
            break;   
        }
	}  // end switch( r_store_fsm )

}  // end transition

//////////////////////////
void CoprocCpy::genMoore()
{
    // p_load port
    p_load.req    = (r_load_fsm.read() == REQ);
    p_load.bursts = 1;
    p_load.r      = (r_load_fsm.read() == MOVE);

    // p_store port
    size_t  bufid = r_store_bufid.read();
    size_t  word  = r_store_word.read();
    p_store.req    = (r_store_fsm.read() == REQ);
    p_store.bursts = 1;
    p_store.w      = (r_store_fsm.read() == MOVE);
    p_store.data   = r_buf[bufid][word];
}



///////////////////////////////////////////////////
CoprocCpy::CoprocCpy( sc_core::sc_module_name name,
                      const uint32_t          burst_size )
    : soclib::caba::BaseModule(name),

      p_clk( "clk" ),
      p_resetn( "resetn" ),
      p_load( "p_load" ),
      p_store( "p_store" ),
      p_config( "p_config" ),

      m_burst( burst_size>>2 )
{     
    std::cout << "  - Building CoprocCpy : " << name << std::endl;

    assert ( ((burst_size == 4) or (burst_size == 8) or (burst_size == 16)
              or (burst_size == 32) or (burst_size == 64)) and
           "ERROR in COPROC_CPY : illegal burst_size parameter" );

	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
}

/////////////////////////////
void CoprocCpy::print_trace()
{
    const char* cpy_states[] =
    {
    "IDLE",
    "WAIT",
    "REQ",
    "MOVE",
    };

    std::cout << "CPY " << name() << " : running = " << std::dec << p_config.read() << std::endl
              << "  load_fsm  = " << cpy_states[r_load_fsm.read()]
              << " / bufid = " << r_load_bufid.read()
              << " / word = " << r_load_word.read() 
              << " / buf[bufid][word] = " 
              << r_buf[r_load_bufid.read()][r_load_word.read()] << std::endl
              << "  store_fsm = " << cpy_states[r_store_fsm.read()]
              << " / bufid = " << r_store_bufid.read()
              << " / word = " << r_store_word.read() 
              << " / buf[bufid][word] = " 
              << r_buf[r_load_bufid.read()][r_load_word.read()] << std::endl;

}  // end print_trace()



}}
