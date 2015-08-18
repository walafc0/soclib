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
 * Authors  : alain.greiner@lip6.fr 
 * Date     : july 2010
 * Copyright: UPMC - LIP6
 */

#include "../include/dspin_packet_generator.h"
#include <cassert>

namespace soclib { namespace caba {

using namespace soclib::caba;
using namespace soclib::common;

#define tmpl(x) template<int cmd_width, int rsp_width> x DspinPacketGenerator<cmd_width, rsp_width>


//////////////////////////////////////////////////////
tmpl(/**/)::DspinPacketGenerator( sc_module_name name,
                                  const size_t   srcid,      // srcid for random 
                                  const size_t   length,     // packet length (flits)
                                  const size_t   load,       // requested load * 1000
                                  const size_t   fifo_depth, // Fifo depth
                                  const size_t   bcp )       // broadcast period
    : BaseModule(name),

    p_clk( "clk" ),
    p_resetn( "resetn" ),
    p_in( "p_in" ),
    p_out( "p_out" ),

    r_cycles( "r_cycles" ),
    r_fifo_posted( "r_fifo_posted" ),

    r_send_fsm( "r_send_fsm" ),
    r_send_length( "r_send_length" ), 
    r_send_dest( "r_send_dest" ),
    r_send_date( "r_send_date" ),
    r_send_packets( "r_send_packets" ),
    r_send_bc_packets( "r_send_bc_packets" ),

    r_receive_fsm( "r_receive_fsm" ),
    r_receive_packets( "r_receive_packets" ),
    r_receive_latency( "r_receive_latency" ),
    r_receive_bc_packets( "r_receive_bc_packets" ),
    r_receive_bc_latency( "r_receive_bc_latency" ),
 
    r_date_fifo( "r_date_fifo", fifo_depth ),

    m_length( length ),
    m_load( load ),
    m_bcp( bcp ),
    m_srcid( srcid )
{
    assert( (load <= 1000 ) and
    "DSPIN PACKET GENERATOR ERROR: The load should be between 0 and 1000" );

    assert( (cmd_width >= 33) and
    "DSPIN PACKET GENERATOR ERROR: CMD flit width smaller than 33 bits");

    assert( (length > 1 ) and
    "DSPIN PACKET GENERATOR ERROR: Packet length smaller than 2 flits");

    SC_METHOD (transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD (genMoore);
    dont_initialize();
    sensitive  << p_clk.neg();
} //  end constructor

////////////////////////
tmpl(void)::transition()
{
    if ( not p_resetn.read() )
    {
        r_send_fsm            = SEND_IDLE;
        r_receive_fsm         = RECEIVE_IDLE;
        r_cycles              = 0;
        r_fifo_posted         = 0;
        r_send_packets        = 0;
        r_send_bc_packets     = 0;
        r_receive_packets     = 0;
        r_receive_latency     = 0;
        r_receive_bc_packets  = 0;
        r_receive_bc_latency  = 0;
        srandom( m_srcid + cmd_width );
        return;
    }

    // default values
    bool   fifo_put = false;
    bool   fifo_get = false;

    uint32_t alea = random();

    /////////////////////////   GENERATOR FSM
    size_t accepted_load = (r_fifo_posted.read() * m_length * 1000) / (r_cycles.read() + 1);

    if( (accepted_load + (alea>>16 & 0xF)) < (m_load) )
    {
       fifo_put = true ;
       if( r_date_fifo.wok() ) r_fifo_posted = r_fifo_posted.read() + 1;
    }

    /////////////////////////// CMD FSM
    switch( r_send_fsm.read() ) 
    {
        case SEND_IDLE:
            if ( r_date_fifo.rok() )
            {
                fifo_get       = true;
                r_send_date    = r_date_fifo.read();

                if ( (m_bcp != 0) and 
                     (((r_send_packets.read() + r_send_bc_packets + 1) % m_bcp) == 0) )
                {
                    r_send_length     = 2;
                    r_send_fsm        = SEND_BROADCAST;
                    r_send_bc_packets = r_send_bc_packets.read() + 1;
                }
                else
                {
                    r_send_length  = m_length;
                    r_send_fsm     = SEND_UNICAST;
                    r_send_dest    = alea & 0xFFFF;
                    r_send_packets = r_send_packets.read() + 1;
                }
            }
        break;
        case SEND_UNICAST:
        case SEND_BROADCAST:
            if( p_out.read.read() ) 
            {
                r_send_length = r_send_length.read() - 1;
                if( r_send_length.read() == 1 )  r_send_fsm = SEND_IDLE;
            }
        break;
    }  // end SEND FSM

    //////////////////////////////	RECEIVE FSM
    switch( r_receive_fsm.read() ) 
    {
        case RECEIVE_IDLE:
            if ( p_in.write.read() )
            {
                if ( (p_in.data.read() & sc_uint<rsp_width>(1)) ) 
                    r_receive_fsm = RECEIVE_BROADCAST;
                else
                    r_receive_fsm = RECEIVE_UNICAST;
            }
        break;
        case RECEIVE_BROADCAST:
            if ( p_in.write.read() )
            {
                r_receive_bc_packets  = r_receive_bc_packets.read() + 1;
                r_receive_bc_latency  = r_receive_bc_latency.read() + 
                                        r_cycles.read() - (uint32_t)p_in.data.read();
                r_receive_fsm = RECEIVE_IDLE;
            }
        break;
        case RECEIVE_UNICAST:
            if ( p_in.write.read() )
            {
                r_receive_packets = r_receive_packets.read() + 1;
                r_receive_latency = r_receive_latency.read() + 
                                    r_cycles.read() - (uint32_t)p_in.data.read();

                if ( p_in.eop.read() ) r_receive_fsm = RECEIVE_IDLE;
                else                   r_receive_fsm = RECEIVE_WAIT_EOP;
            }
        break;
        case RECEIVE_WAIT_EOP:
            if ( p_in.write.read() and  p_in.eop.read() )
            {
                r_receive_fsm = RECEIVE_IDLE;
            }
        break;
    } // end RECEIVE FSM

    // increment date
    r_cycles = r_cycles.read() + 1;

    //  update fifos
    r_date_fifo.update( fifo_get, fifo_put, r_cycles.read() );

} // end transition

//////////////////////
tmpl(void)::genMoore()
{
    // p_out
    sc_uint<cmd_width>	data;
    bool                write;
    bool                eop;

    if ( r_send_fsm.read() == SEND_IDLE )
    {
        data  = 0;
        eop   = false;
        write = false;
    }
    else if ( r_send_fsm.read() == SEND_UNICAST )    // N+2 flits
    {
        write = true;
        if ( r_send_length.read() == m_length )  // first flit
        {
            data = ((sc_uint<cmd_width>)r_send_dest.read()) << (cmd_width - 17);
        }
        else if ( r_send_length.read() == (m_length-1) )  // second flit
        {
            data = (sc_uint<cmd_width>)r_send_date.read();
        }
        else											// other flit
        {
            data = 0;
        }

        if( r_send_length.read() == 1 ) eop = true;
        else                            eop = false;
    }
    else  // SEND_BROADCAST  (two flits)
    {
        write = true;
        if ( r_send_length.read() == 2 )  // first flit
        {
            data = sc_uint<cmd_width>(0x07C1F) << (cmd_width - 20) |
                       sc_uint<cmd_width>(1);
            eop  = false;
        }
        else                              // second flit
        {
            data = (sc_uint<cmd_width>)r_send_date.read() |
                       (sc_uint<cmd_width>(1)<<(cmd_width-1));
            eop  = true;
        }
    }
    p_out.data  = data;
    p_out.eop   = eop;
    p_out.write = write;

    // p_in
    p_in.read = true;

} // end genMoore

/////////////////////////
tmpl(void)::print_trace()
{
    const char* cmd_str[] = { "IDLE", "SEND_UNICAST", "SEND_BROADCAST" };
    const char* rsp_str[] = { "IDLE", "RECEIVE_UNICAST", "RECEIVE_BROADCAST", "RECEIVE_WAIT" };

    std::cout << "DSPIN_GENERATOR " << name() 
              << " : send_fsm = " << cmd_str[r_send_fsm.read()] 
              << " / recv_fsm = " << rsp_str[r_receive_fsm.read()] 
              << " / fifo_content = " << r_date_fifo.filled_status() << std::endl;
} // end print_trace

/////////////////////////
tmpl(void)::print_stats()
{
    size_t   load       = (r_send_packets.read() * m_length * 1000) / r_cycles.read();
    uint32_t latency    = r_receive_latency.read() / (r_receive_packets.read() + 1);
    uint32_t bc_latency = r_receive_bc_latency.read() / (r_receive_bc_packets.read() + 1);

    std::cout << "DSPIN_GENERATOR " << name() << std::dec << std::endl
          << " - unicast sent packets       = " << r_send_packets.read() << std::endl
          << " - broadcast sent packets     = " << r_send_bc_packets.read() << std::endl
          << " - offered load               = " << m_load << std::endl
          << " - accepted load              = " << load << std::endl
          << " - unicast received packets   = " << r_receive_packets.read() << std::endl
          << " - unicast latency            = " << latency << std::endl
          << " - broadcast received packets = " << r_receive_bc_packets.read() << std::endl
          << " - broadcast latency          = " << bc_latency << std::endl;
} // end print_stats


}} // end namespaces :w

