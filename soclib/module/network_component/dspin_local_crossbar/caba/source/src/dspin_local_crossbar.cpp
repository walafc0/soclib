/* -*- c++ -*-
  *
  * File : dspin_local_crossbar.cpp
  * Copyright (c) UPMC, Lip6
  * Authors : Alain Greiner
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
  */

#include "../include/dspin_local_crossbar.h"

namespace soclib { namespace caba {

using namespace soclib::common;
using namespace soclib::caba;

#define tmpl(x) template<size_t flit_width> x DspinLocalCrossbar<flit_width>

//////////////////////////////////////////////////////////
//                  constructor
//////////////////////////////////////////////////////////
tmpl(/**/)::DspinLocalCrossbar( sc_module_name       name, 
                                const MappingTable   &mt,
                                const size_t         x,
                                const size_t         y,
                                const size_t         x_width,
                                const size_t         y_width,
                                const size_t         l_width,
                                const size_t         nb_local_inputs,
                                const size_t         nb_local_outputs,
                                const size_t         in_fifo_depth,
                                const size_t         out_fifo_depth,
                                const bool           is_cmd,
                                const bool           use_routing_table,
                                const bool           broadcast_supported)
    : BaseModule(name),

      p_clk("p_clk"),
      p_resetn("p_resetn"),
      p_global_in("p_global_in"),
      p_global_out("p_global_out"),

      r_alloc_out(alloc_elems<sc_signal<bool> > ("r_alloc_out", nb_local_outputs + 1)),
      r_index_out(alloc_elems<sc_signal<size_t> > ("r_index_out", nb_local_outputs + 1)),
      r_fsm_in(alloc_elems<sc_signal<int> > ("r_fsm_in", nb_local_inputs + 1)),
      r_index_in(alloc_elems<sc_signal<size_t> > ("r_index_in", nb_local_inputs + 1)),

      m_local_x( x ),
      m_local_y( y ),
      m_x_width( x_width ),
      m_x_shift( flit_width - x_width ),
      m_x_mask( (0x1 << x_width) - 1 ),
      m_y_width( y_width ),
      m_y_shift( flit_width - x_width - y_width ),
      m_y_mask( (0x1 << y_width) - 1 ),
      m_l_width( l_width ),
      m_l_shift( flit_width - x_width - y_width - l_width ),
      m_l_mask( (0x1 << l_width) - 1 ),
      m_local_inputs( nb_local_inputs ),
      m_local_outputs( nb_local_outputs ),
      m_addr_width( mt.getAddressWidth() ),
      m_is_cmd( is_cmd ),
      m_use_routing_table( use_routing_table ),
      m_broadcast_supported( broadcast_supported )
    {
        std::cout << "  - Building DspinLocalCrossbar : " << name << std::endl;

        SC_METHOD (transition);
        dont_initialize();
        sensitive << p_clk.pos();

        SC_METHOD (genMoore);
        dont_initialize();
        sensitive  << p_clk.neg();

        r_buf_in = new internal_flit_t[nb_local_inputs + 1];

        // build routing table
        if ( ( m_local_outputs > 0 ) and use_routing_table )
        {
            size_t cluster_id = (x << y_width) + y;
            if ( is_cmd )
            {
                m_cmd_rt = mt.getLocalIndexFromAddress( cluster_id );
            }
            else
            {
                m_rsp_rt = mt.getLocalIndexFromSrcid( cluster_id );
            }
        }

        if ( m_local_inputs > 0 )
        {
            p_local_in = alloc_elems<DspinInput<flit_width> >(
                    "p_local_in", nb_local_inputs );
        }
        if ( m_local_outputs > 0 )
        {
            p_local_out = alloc_elems<DspinOutput<flit_width> >(
                    "p_local_out", nb_local_outputs );
        }

        // construct FIFOs
        r_fifo_in  = (GenericFifo<internal_flit_t>*)
        malloc(sizeof(GenericFifo<internal_flit_t>) * (m_local_inputs + 1));
        
        r_fifo_out = (GenericFifo<internal_flit_t>*)
        malloc(sizeof(GenericFifo<internal_flit_t>) * (m_local_outputs + 1));

        for (size_t i = 0; i <= m_local_inputs; i++)
        {
            std::ostringstream stri;
            stri << "r_in_fifo_" << i;
            new(&r_fifo_in[i])  GenericFifo<internal_flit_t>(stri.str(), in_fifo_depth);
        }

        for (size_t j = 0; j <= m_local_outputs; j++)
        {
            std::ostringstream stro;
            stro << "r_out_fifo_" << j;
            new(&r_fifo_out[j]) GenericFifo<internal_flit_t>(stro.str(), out_fifo_depth);
        }

        assert( (flit_width >= x_width + y_width + l_width) and
        "ERROR in DSPIN_LOCAL_CROSSBAR: flit_width < x_width + y_width + l_width");

    } //  end constructor


    tmpl(/**/)::~DspinLocalCrossbar() {
        for (size_t i = 0; i <= m_local_inputs; i++)
        {
            r_fifo_in[i].~GenericFifo<internal_flit_t>();
        }

        for (size_t j = 0; j <= m_local_outputs; j++)
        {
            r_fifo_out[j].~GenericFifo<internal_flit_t>();
        }

        free(r_fifo_in);
        free(r_fifo_out);

        if ( m_local_inputs > 0 )
        {
            dealloc_elems<DspinInput<flit_width> >(p_local_in, m_local_inputs);
        }
        if ( m_local_outputs > 0 )
        {
            dealloc_elems<DspinOutput<flit_width> >(p_local_out, m_local_outputs);
        }
        dealloc_elems<sc_signal<bool> >(r_alloc_out, m_local_outputs + 1);
        dealloc_elems<sc_signal<size_t> >(r_index_out, m_local_outputs + 1);
        dealloc_elems<sc_signal<int> >(r_fsm_in, m_local_inputs + 1);
        dealloc_elems<sc_signal<size_t> >(r_index_in, m_local_inputs + 1);
        delete [] r_buf_in;
    }


    ////////////////////////////////////////////////////////////////////////////
    tmpl(size_t)::route( sc_uint<flit_width> data,     // first flit
                         size_t              input )   // input port index 
    {
        size_t   output;   // selected output port
        size_t   x_dest  = (size_t)(data >> m_x_shift) & m_x_mask;
        size_t   y_dest  = (size_t)(data >> m_y_shift) & m_y_mask;

        if ( (x_dest == m_local_x) and (y_dest == m_local_y) and
             (m_local_outputs > 0) )          // local dest
        {
            if ( m_use_routing_table )
            {
                // address (for CMD) or srcid (for RSP) must be right-aligned
                if ( m_is_cmd )
                {
                    uint64_t address;
                    if (flit_width >= m_addr_width) 
                        address = data>>(flit_width - m_addr_width);
                    else                          
                        address = data<<(m_addr_width - flit_width);
                    output = m_cmd_rt[ address ];
                }
                else
                {   
                    uint32_t srcid = data >> m_l_shift;
                    output = m_rsp_rt[ srcid ];
                }
            }
            else
            {
                output = (size_t)(data >> m_l_shift) & m_l_mask;
 
                if ( output >= m_local_outputs )
                {
                    std::cout << "ERROR in DSPIN_LOCAL_CROSSBAR: " << name()
                              << " illegal local destination" << std::endl;
                    exit(0);
                }
            }
        }
        else                                                            // global dest
        {
            if ( input  == m_local_inputs )
            {
                std::cout << "ERROR in DSPIN_LOCAL_CROSSBAR: " << name()
                          << " illegal global to global request" << std::endl;
                exit(0);
            }

            output = m_local_outputs;
        }
        return output;
    }

    /////////////////////////////////////////////////////////
    tmpl(inline bool)::is_broadcast(sc_uint<flit_width> data)
    {
        return ( (data & 0x1) != 0);
    }

    /////////////////////////
    tmpl(void)::print_trace()
    {
        const char* infsm_str[] = { "IDLE", "REQ", "ALLOC", "REQ_BC", "ALLOC_BC" };

        std::cout << "DSPIN_LOCAL_CROSSBAR " << name() << std::hex; 

        for( size_t i = 0 ; i <= m_local_inputs ; i++)  // loop on input ports
        {
            std::cout << " / infsm[" << std::dec << i 
                      << "] = " << infsm_str[r_fsm_in[i].read()];
        }

        for( size_t out = 0 ; out <= m_local_outputs ; out++)  // loop on output ports
        {
            if ( r_alloc_out[out].read() )
            {
                size_t in = r_index_out[out];
                std::cout << " / in[" << in << "] -> out[" << out << "]";
            }   
        }
        std::cout << std::endl;
    }

    /////////////////////////
    tmpl(void)::transition()
    {
        // Long wires connecting input and output ports
        size_t              req_in[m_local_inputs+1];   // input ports  -> output ports
        size_t              get_out[m_local_outputs+1]; // output ports -> input ports
        bool                put_in[m_local_inputs+1];   // input ports  -> output ports
        internal_flit_t     data_in[m_local_inputs+1];  // input ports  -> output ports

        // control signals for the input fifos
        bool                fifo_in_write[m_local_inputs+1];
        bool                fifo_in_read[m_local_inputs+1];    
        internal_flit_t     fifo_in_wdata[m_local_inputs+1];

        // control signals for the output fifos
        bool                fifo_out_write[m_local_outputs+1];
        bool                fifo_out_read[m_local_outputs+1];
        internal_flit_t     fifo_out_wdata[m_local_outputs+1];

        // reset 
        if ( p_resetn.read() == false ) 
        {
            for(size_t j = 0 ; j <= m_local_outputs ; j++) 
            {
                r_alloc_out[j] = false;
                r_index_out[j] = 0;
                r_fifo_out[j].init();
            }
            for(size_t i = 0 ; i <= m_local_inputs ; i++) 
            {
                r_index_in[i]  = 0;
                r_fsm_in[i]    = INFSM_IDLE;
                r_fifo_in[i].init();
            }
            return;
        }

        // fifo_in signals default values
        for(size_t i = 0 ; i < m_local_inputs ; i++) 
        {
            fifo_in_read[i]        = false;   
            fifo_in_write[i]       = p_local_in[i].write.read();
            fifo_in_wdata[i].data  = p_local_in[i].data.read();
            fifo_in_wdata[i].eop   = p_local_in[i].eop.read();
        }
        fifo_in_read[m_local_inputs]       = false; // default value
        fifo_in_write[m_local_inputs]      = p_global_in.write.read();
        fifo_in_wdata[m_local_inputs].data = p_global_in.data.read();
        fifo_in_wdata[m_local_inputs].eop  = p_global_in.eop.read();

        // fifo_out signals default values
        for(size_t j = 0 ; j < m_local_outputs ; j++) 
        {
            fifo_out_read[j]  = p_local_out[j].read.read();
            fifo_out_write[j] = false;  
        }
        fifo_out_read[m_local_outputs]  = p_global_out.read.read();
        fifo_out_write[m_local_outputs] = false;     

        // loop on the output ports:
        // compute get_out[j] depending on the output port state
        // and combining fifo_out_wok[j] and r_alloc_out[j]
        for ( size_t j = 0 ; j <= m_local_outputs ; j++ )
        {
            if( r_alloc_out[j].read() and (r_fifo_out[j].wok()) ) 
            {
                get_out[j] = r_index_out[j].read();
            }
            else
            {                       
                get_out[j] = 0xFFFFFFFF;  
            }
        }

        // loop on the input ports (including global input port, 
        // with the convention index[global] = m_local_inputs)
        // The port state is defined by r_fsm_in[i], r_index_in[i] 
        // The req_in[i] computation uses the route() function.
        // Both put_in[i] and req_in[i] depend on the input port state.

        for ( size_t i = 0 ; i <= m_local_inputs ; i++ )
        {
            switch ( r_fsm_in[i].read() )
            {
                case INFSM_IDLE:    // no output port allocated
                {
                    put_in[i] = false;
                    if ( r_fifo_in[i].rok() ) // packet available in input fifo
                    {
                        if ( is_broadcast(r_fifo_in[i].read().data ) and 
                             m_broadcast_supported )   // broadcast required
                        {
                            r_buf_in[i] = r_fifo_in[i].read();

                            if ( i == m_local_inputs ) // global input port
                            {
                                req_in[i]     = m_local_outputs - 1;
                            }
                            else                       // local input port
                            {
                                req_in[i]     = m_local_outputs;
                            }
                            r_index_in[i] = req_in[i];
                            r_fsm_in[i]   = INFSM_REQ_BC;
                        }
                        else                           // unicast routing
                        {
                            req_in[i]     = route( r_fifo_in[i].read().data, i );
                            r_index_in[i] = req_in[i];
                            r_fsm_in[i]   = INFSM_REQ;
                        }
                    }
                    else
                    {
                        req_in[i]     = 0xFFFFFFFF;  // no request
                    }
                    break;
                }
                case INFSM_REQ:   // waiting output port allocation
                {
                    data_in[i] = r_fifo_in[i].read();
                    put_in[i]  = r_fifo_in[i].rok();
                    req_in[i]  = r_index_in[i];
                    if ( get_out[r_index_in[i].read()] == i ) // first flit transfered
                    {
                        if ( r_fifo_in[i].read().eop )  r_fsm_in[i] = INFSM_IDLE;
                        else                            r_fsm_in[i] = INFSM_ALLOC;
                    }
                    break;
                }
                case INFSM_ALLOC:  // output port allocated
                {
                    data_in[i] = r_fifo_in[i].read();
                    put_in[i]  = r_fifo_in[i].rok();
                    req_in[i]  = 0xFFFFFFFF;                // no request
                    if ( r_fifo_in[i].read().eop and
                         r_fifo_in[i].rok() and 
                         (get_out[r_index_in[i].read()] == i) )  // last flit transfered
                    {
                        r_fsm_in[i] = INFSM_IDLE;
                    }
                    break;
                }
                case INFSM_REQ_BC:  // waiting output port allocation
                {
                    data_in[i] = r_buf_in[i];
                    put_in[i]  = true;
                    req_in[i]  = r_index_in[i];
                    if ( get_out[r_index_in[i].read()] == i ) // first flit transfered
                    {
                        r_fsm_in[i] = INFSM_ALLOC_BC;
                    }
                    break;
                }
                case INFSM_ALLOC_BC:  // output port allocated         
                {
                    data_in[i] = r_fifo_in[i].read();
                    put_in[i]  = r_fifo_in[i].rok();
                    req_in[i]  = 0xFFFFFFFF;                // no request

                    if ( r_fifo_in[i].rok() and 
                         get_out[r_index_in[i].read()] == i )  // last flit transfered
                    {
                        if ( not r_fifo_in[i].read().eop ) 
                        {
                            std::cout << "ERROR in DSPIN_LOCAL_CROSSBAR " << name()
                                      << " : broadcast packets must have 2 flits" << std::endl;
                        }
                        if ( r_index_in[i].read() == 0 ) r_fsm_in[i] = INFSM_IDLE;
                        else                             r_fsm_in[i] = INFSM_REQ_BC;
                        r_index_in[i] = r_index_in[i].read() - 1;
                    }
                    break;
                }
            } // end switch
        } // end for input ports
                                   
        // loop on the output ports (including global output port, 
        // with the convention index[global] = m_local_outputs)
        // The r_alloc_out[j] and r_index_out[j] computation
        // implements the round-robin allocation policy.
        // These two registers implement a 2*N states FSM.
        for( size_t j = 0 ; j <= m_local_outputs ; j++ ) 
        {
            if( not r_alloc_out[j].read() )  // not allocated: possible new allocation
            {
                for( size_t k = r_index_out[j].read() + 1 ; 
                     k <= (r_index_out[j].read() + m_local_inputs + 1) ; 
                     k++ ) 
                { 
                    size_t i = k % (m_local_inputs + 1);

                    if( req_in[i] == j ) 
                    {
                        r_alloc_out[j] = true;
                        r_index_out[j] = i;
                        break;
                    }
                } // end loop on input ports
            } 
            else                            // allocated: possible desallocation
            {
                if ( data_in[r_index_out[j]].eop and
                     r_fifo_out[j].wok() and 
                     put_in[r_index_out[j]] ) 
                {
                    r_alloc_out[j] = false;
                }
            }
        } // end loop on output ports

        // loop on input ports :
        // fifo_in_read[i] computation 
        // (computed here because it depends on get_out[])
        for( size_t i = 0 ; i <= m_local_inputs ; i++ ) 
        {
            if ( (r_fsm_in[i].read() == INFSM_REQ) or 
                 (r_fsm_in[i].read() == INFSM_ALLOC) or
                 ((r_fsm_in[i].read() == INFSM_ALLOC_BC) and (r_index_in[i].read() == 0)))
            {
                fifo_in_read[i] = (get_out[r_index_in[i].read()] == i);
            }
            if ( (r_fsm_in[i].read() == INFSM_IDLE) and
                 is_broadcast( r_fifo_in[i].read().data ) and 
                 m_broadcast_supported )   
            {
                fifo_in_read[i] = true;
            }
        }  // end loop on input ports

        // loop on the output ports :
        // The fifo_out_write[j] and fifo_out_wdata[j] computation
        // implements the output port mux
        for( size_t j = 0 ; j <= m_local_outputs ; j++ ) 
        {
            if( r_alloc_out[j] )  // output port allocated
            {
                fifo_out_write[j] = put_in[r_index_out[j]];
                fifo_out_wdata[j] = data_in[r_index_out[j]];
            }
        }  // end loop on the output ports

        //  input FIFOs update
        for(size_t i = 0 ; i <= m_local_inputs ; i++) 
        {
            r_fifo_in[i].update(fifo_in_read[i],
                                fifo_in_write[i],
                                fifo_in_wdata[i]);
        }

        //  output FIFOs update
        for(size_t j = 0 ; j <= m_local_outputs ; j++)
        { 
            r_fifo_out[j].update(fifo_out_read[j],
                                 fifo_out_write[j],
                                 fifo_out_wdata[j]);
        }
    } // end transition

    ///////////////////////
    tmpl(void)::genMoore()
    {
        // input ports
        for(size_t i = 0 ; i < m_local_inputs ; i++) 
        {
            p_local_in[i].read = r_fifo_in[i].wok();
        }
        p_global_in.read = r_fifo_in[m_local_inputs].wok();

        // output ports
        for(size_t j = 0 ; j < m_local_outputs ; j++) 
        {
            p_local_out[j].write = r_fifo_out[j].rok();
            p_local_out[j].data  = r_fifo_out[j].read().data;
            p_local_out[j].eop   = r_fifo_out[j].read().eop;
        }
        p_global_out.write = r_fifo_out[m_local_outputs].rok();
        p_global_out.data  = r_fifo_out[m_local_outputs].read().data;
        p_global_out.eop   = r_fifo_out[m_local_outputs].read().eop;

    } // end genMoore

}} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
