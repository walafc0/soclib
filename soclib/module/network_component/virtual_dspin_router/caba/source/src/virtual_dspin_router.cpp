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
 * Authors      : alain.greiner@lip6.fr noe.girand@polytechnique.org
 * Date         : august 2010
 * Maintainers  : alexandre.joannou@lip6.fr
 * Copyright: UPMC - LIP6
 */

#include "alloc_elems.h"
#include "virtual_dspin_router.h"
#include <cstdlib>
#include <cstring>
#include <sstream>

namespace soclib { namespace caba {

using namespace soclib::caba;
using namespace soclib::common;

#define tmpl(x) template<int flit_width> x VirtualDspinRouter<flit_width>

    ///////////////////////////////////////////////////////////////////////////
    // Implementation Note :
    // The TDM (Time dependant Multiplexing) introduces a strong coupling
    // between the two FSMs controling the virtual channels
    // in the input port. Therefore, it is necessary to have several successive
    // loops to scan the nb_chan * 5 input FSMs:
    // - The first loop computes - put[i][k] : input(k,i) wishes to produce
    //                           - get[i][k] : output(k,i) wishes to consume
    // - The second loop uses these values to implement the TDM policy
    //   and compute final_put[i][k], final_data[i][k], in_fifo_read[i][k]
    //   and the next FSM state r_input_fsm[i][k].
    // In this implementation, there is only one comrbinational transfer
    // per cycle from input(i) to output(j) or from output(j) to input(i).
    ///////////////////////////////////////////////////////////////////////////
    // The xfirst_route(), broadcast_route() and is_broadcast() functions
    // defined below are used to decode the DSPIN first flit format:
    // - In case of a non-broadcast packet :
    //  |   X     |   Y     |---------------------------------------|BC |
    //  | x_width | y_width |  flit_width - (x_width + y_width + 2) | 0 |
    //
    //  - In case of a broacast
    //  |  XMIN   |  XMAX   |  YMIN   |  YMAX   |-------------------|BC |
    //  |   5     |   5     |   5     |   5     | flit_width - 22   | 1 |
    ///////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////
    tmpl(void)::print_trace(size_t channel)
    {
        const char * port_name[] = { "NORTH","SOUTH","EAST ","WEST ","LOCAL" };
        size_t k = channel%m_nb_chan;
        std::cout << "DSPIN_ROUTER " << name() << " : channel " << k;
        for (size_t i = 0; i < 5; i++)  // loop on output ports
        {
            if (r_output_alloc[i][k].read())
            {
                int j = r_output_index[i][k];
                std::cout << " / " << port_name[j] << " -> " << port_name[i] ;
            }
        }
        std::cout << std::endl;
    }

    /////////////////////////////////////
    tmpl(void)::debug_trace(size_t channel)
    {
        const char* input_fsm_state[] = {   "INFSM_IDLE",
                                            "INFSM_REQ",
                                            "INFSM_DT",
                                            "INFSM_REQ_FIRST",
                                            "INFSM_DT_FIRST",
                                            "INFSM_REQ_SECOND",
                                            "INFSM_DT_SECOND",
                                            "INFSM_REQ_THIRD",
                                            "INFSM_DT_THIRD",
                                            "INFSM_REQ_FOURTH",
                                            "INFSM_DT_FOURTH"};
        size_t k = channel%m_nb_chan;
        std::cout << "-- " << name() << " : channel " << k << std::endl;
        for( size_t i=0 ; i<5 ; i++)  // loop on input ports
        {
            std::cout << " | input[" << i << "] state = " << input_fsm_state[r_input_fsm[i][k]];
            if(in_fifo[i][k].rok())
            std::cout << " / dtin = " << std::hex << in_fifo[i][k].read().data;
        }
        std::cout << std::endl;
        for( size_t i=0 ; i<5 ; i++)  // loop on output ports
        {
            std::cout << " | output[" << i << "] alloc = " << r_output_alloc[i][k]
                      << " / index = " <<  r_output_index[i][k];
            if(out_fifo[i][k].rok())
            std::cout << " / dtout = " << std::hex << out_fifo[i][k].read().data;
        }
        std::cout << std::endl;
    }

    ////////////////////////////////////////////////
    tmpl(int)::xfirst_route(sc_uint<flit_width> data)
    {
        int xdest = (int)(data >> m_x_shift) & m_x_mask;
        int ydest = (int)(data >> m_y_shift) & m_y_mask;
        return (xdest < m_local_x ? REQ_WEST :
               (xdest > m_local_x ? REQ_EAST :
               (ydest < m_local_y ? REQ_SOUTH :
               (ydest > m_local_y ? REQ_NORTH : REQ_LOCAL))));
    }

    /////////////////////////////////////////////////////////////////////////
    tmpl(int)::broadcast_route(int iter, int source, sc_uint<flit_width> data)
    {
        int sel = REQ_NOP;
        int xmin = (data >> (flit_width - 5 )) & 0x1F;
        int xmax = (data >> (flit_width - 10)) & 0x1F;
        int ymin = (data >> (flit_width - 15)) & 0x1F;
        int ymax = (data >> (flit_width - 20)) & 0x1F;

        switch(source) {
        case LOCAL :
            if      ( iter == 1 )    sel = REQ_NORTH;
            else if ( iter == 2 )    sel = REQ_SOUTH;
            else if ( iter == 3 )    sel = REQ_EAST;
            else if ( iter == 4 )    sel = REQ_WEST;
        break;
        case NORTH :
            if      ( iter == 1 )    sel = REQ_SOUTH;
            else if ( iter == 2 )    sel = REQ_LOCAL;
            else if ( iter == 3 )    sel = REQ_NOP;
            else if ( iter == 4 )    sel = REQ_NOP;
        break;
        case SOUTH :
            if      ( iter == 1 )    sel = REQ_NORTH;
            else if ( iter == 2 )    sel = REQ_LOCAL;
            else if ( iter == 3 )    sel = REQ_NOP;
            else if ( iter == 4 )    sel = REQ_NOP;
        break;
        case EAST :
            if      ( iter == 1 )    sel = REQ_WEST;
            else if ( iter == 2 )    sel = REQ_NORTH;
            else if ( iter == 3 )    sel = REQ_SOUTH;
            else if ( iter == 4 )    sel = REQ_LOCAL;
        break;
        case WEST :
            if      ( iter == 1 )    sel = REQ_EAST;
            else if ( iter == 2 )    sel = REQ_NORTH;
            else if ( iter == 3 )    sel = REQ_SOUTH;
            else if ( iter == 4 )    sel = REQ_LOCAL;
        break;
        }
        if      ( (sel == REQ_NORTH) && !(m_local_y < ymax) )     sel = REQ_NOP;
        else if ( (sel == REQ_SOUTH) && !(m_local_y > ymin) )     sel = REQ_NOP;
        else if ( (sel == REQ_EAST ) && !(m_local_x < xmax) )     sel = REQ_NOP;
        else if ( (sel == REQ_WEST ) && !(m_local_x > xmin) )     sel = REQ_NOP;

        return sel;
    }

    /////////////////////////////////////////////////////////
    tmpl(inline bool)::is_broadcast(sc_uint<flit_width> data)
    {
        return ( (data & 0x1) != 0);
    }

    ////////////////////////////////////////////////////////////
    tmpl(/**/)::VirtualDspinRouter( sc_module_name  name,
                                    int             x,
                                    int             y,
                                    int             x_width,
                                    int             y_width,
                                    size_t          nb_chan,
                                    int             in_fifo_depth,
                                    int             out_fifo_depth)
    : BaseModule(name),
      p_clk("clk"),
      p_resetn("resetn")
    {
        std::cout << "  - Building VirtualDspinRouter : " << name << std::endl;

        SC_METHOD (transition);
        dont_initialize();
        sensitive << p_clk.pos();

        SC_METHOD (genMoore);
        dont_initialize();
        sensitive  << p_clk.neg();

        // maximal width of the x & y fields (to support limited broadcast)
        if ( (x_width > 5) || (y_width > 5) )
        {
            std::cout << "ERROR in virtual_dspin_router" << name << std::endl;
            std::cout << "x_width & y_width parameters larger than 5" << std::endl;
            exit(0);
        }

        // minimal width of a flit
        if ( flit_width < 22 )
        {
            std::cout << "Error in the virtual_dspin_router" << name << std::endl;
            std::cout << "The flit_width cannot be smaller than 22 bits" << std::endl;
            exit(0);
        }

        // minimal number of virtual channels
        if ( nb_chan <= 0 )
        {
            std::cout << "Error in the virtual_dspin_router" << name << std::endl;
            std::cout << "The nb_chan must be > 0" << std::endl;
            exit(0);
        }

        // ports
        for (int i = 0; i < 5; i++)
        {
            p_in[i]  = (DspinInput<flit_width>*)
                         malloc(sizeof(DspinInput<flit_width>)*nb_chan);
            p_out[i] = (DspinOutput<flit_width>*)
                         malloc(sizeof(DspinOutput<flit_width>)*nb_chan);
            for (size_t k = 0; k < nb_chan; k++)
            {
                std::ostringstream stri;
                stri << "p_in_" << name << "_" << i << "_" << k;
                new(&p_in[i][k]) DspinInput<flit_width>(stri.str());
                std::ostringstream stro;
                stro << "p_out_" << name << "_" << i << "_" << k;
                new(&p_out[i][k]) DspinOutput<flit_width>(stro.str());
            }
        }

        // Time Multiplexing
        for (int i = 0; i < 5; i++)
        {
            r_tdm[i]  = (sc_signal<bool>*) malloc(sizeof(sc_signal<bool>)*nb_chan);
            for (size_t k = 0; k < nb_chan; k++)
            {
                std::ostringstream stri;
                stri << "r_tdm_" << name << "_" << i << "_" << k;
                new(&r_tdm[i][k]) sc_signal<bool>(stri.str().c_str());
            }
        }

        // FSM state registers
        for (int i = 0; i < 5; i++)
        {
            r_input_fsm[i]  = (sc_signal<int>*) malloc(sizeof(sc_signal<int>)*nb_chan);
            for (size_t k = 0; k < nb_chan; k++)
            {
                std::ostringstream stri;
                stri << "r_input_fsm_" << name << "_" << i << "_" << k;
                new(&r_input_fsm[i][k]) sc_signal<int>(stri.str().c_str());
            }
        }

        // fifo extensions
        for (int i = 0; i < 5; i++)
        {
            r_buf[i]  = (internal_flit_t*) malloc(sizeof(internal_flit_t)*nb_chan);
            for (size_t k = 0; k < nb_chan; k++)
            {
                new(&r_buf[i][k]) internal_flit_t();
            }
        }

        // output index & alloc
        for (int i = 0; i < 5; i++)
        {
            r_output_index[i] = (sc_signal<int>*) malloc(sizeof(sc_signal<int>)*nb_chan);
            r_output_alloc[i] = (sc_signal<bool>*) malloc(sizeof(sc_signal<bool>)*nb_chan);
            for (size_t k = 0; k < nb_chan; k++)
            {
                std::ostringstream stri;
                stri << "r_output_index_" << name << "_" << i << "_" << k;
                new(&r_output_index[i][k]) sc_signal<int>(stri.str().c_str());
                std::ostringstream stro;
                stri << "r_output_alloc_" << name << "_" << i << "_" << k;
                new(&r_output_alloc[i][k]) sc_signal<bool>(stri.str().c_str());
            }
        }

        // input & output fifos
        for (int i = 0; i < 5; i++)
        {
            in_fifo[i]  = (GenericFifo<internal_flit_t> *)
                         malloc(sizeof(GenericFifo<internal_flit_t>) * nb_chan);
            out_fifo[i] = (GenericFifo<internal_flit_t> *)
                         malloc(sizeof(GenericFifo<internal_flit_t>) * nb_chan);
            for (size_t k = 0; k < nb_chan; k++)
            {
                std::ostringstream stri;
                stri << "in_fifo_" << name << "_" << i << "_" << k;
                new(&in_fifo[i][k]) GenericFifo<internal_flit_t>(stri.str(), in_fifo_depth);
                std::ostringstream stro;
                stro << "out_fifo_" << name << "_" << i << "_" << k;
                new(&out_fifo[i][k]) GenericFifo<internal_flit_t>(stro.str(), out_fifo_depth);
            }
        }

        m_local_x  = x;
        m_local_y  = y;
        m_x_width  = x_width;
        m_y_width  = y_width;
        m_x_shift  = flit_width - x_width;
        m_y_shift  = flit_width - x_width - y_width;
        m_x_mask   = (0x1 << x_width) - 1;
        m_y_mask   = (0x1 << y_width) - 1;
        m_nb_chan  = nb_chan;

    } //  end constructor

    /////////////////////////////////
    tmpl(/**/)::~VirtualDspinRouter()
    {
        // ports
        for (int i = 0; i < 5; i++)
        {
            for (size_t k = 0; k < m_nb_chan; k++)
            {
               p_in[i][k].~DspinInput<flit_width>();
               p_out[i][k].~DspinOutput<flit_width>();
            }
            free(p_in[i]);
            free(p_out[i]);
        }

        // Time Multiplexing
        for (int i = 0; i < 5; i++)
        {
           for (size_t k = 0; k < m_nb_chan; k++)
           {
              r_tdm[i][k].~sc_signal<bool>();
           }
           free(r_tdm[i]);
        }

        // FSM state registers
        for (int i = 0; i < 5; i++)
        {
            for (size_t k = 0; k < m_nb_chan; k++)
            {
               r_input_fsm[i][k].~sc_signal<int>();
            }
            free(r_input_fsm[i]);
        }

        // fifo extensions
        for (int i = 0; i < 5; i++)
        {
            for (size_t k = 0; k < m_nb_chan; k++)
            {
               r_buf[i][k].~internal_flit_t();
            }
            free(r_buf[i]);
        }

        // output index & alloc
        for (int i = 0; i < 5; i++)
        {
            for (size_t k = 0; k < m_nb_chan; k++)
            {
               r_output_index[i][k].~sc_signal<int>();
               r_output_alloc[i][k].~sc_signal<bool>();
            }
            free(r_output_index[i]);
            free(r_output_alloc[i]);
        }

        // input & output fifos
        for (int i = 0; i < 5; i++)
        {
            for (size_t k = 0; k < m_nb_chan; k++)
            {
               in_fifo[i][k].~GenericFifo<internal_flit_t>();
               out_fifo[i][k].~GenericFifo<internal_flit_t>();
            }
            free(in_fifo[i]);
            free(out_fifo[i]);
        }
    }


    ////////////////////////
    tmpl(void)::transition()
    {
        if (!p_resetn.read())
        {
            for (int i = 0; i < 5; i++) // both input & output ports
            {
                for (size_t k = 0; k < m_nb_chan; k++)
                {
                    r_tdm[i][k]          = false;
                    r_input_fsm[i][k]    = INFSM_IDLE;
                    r_output_index[i][k] = 0;
                    r_output_alloc[i][k] = false;
                    in_fifo[i][k].init();
                    out_fifo[i][k].init();
                }
                r_tdm[i][0] = true;
            }
            return;
        }

        // internal variables used in each input port module
        // they will not be implemented as inter-module signals in the RTL
        bool            in_fifo_read[5][m_nb_chan];     // wishes to consume data in in_fifo
        bool            in_fifo_write[5][m_nb_chan];    // writes data in in_fifo
        internal_flit_t in_fifo_wdata[5][m_nb_chan];    // data to be written in in_fifo

        bool            put[5][m_nb_chan];              // input port wishes to transmit data
        bool            get[5][m_nb_chan];              // output port wishes to consume data

        // internal variables used in each output port module
        // they will not be implemented as inter-module signals in the RTL
        bool            out_fifo_write[5][m_nb_chan];   // write data in out_fifo
        bool            out_fifo_read[5][m_nb_chan];    // consume data in out_fifo
        internal_flit_t out_fifo_wdata[5][m_nb_chan];   // data to be written in out_fifo

        // signals between input port modules & output port modules
        // They must be implemented as inter-modules signals in the RTL
        bool            output_get[5][5][m_nb_chan];    // output j consume data from input i in channel k
        int             input_req[5][m_nb_chan];        // requested output port (NOP means no request)
        internal_flit_t final_data[5];                  // per input : data value
        bool            final_put[5][m_nb_chan];        // per input : data valid

        ///////////////////////////////////////////////////////////////
        // fifo signals default values (both input & output)
        for (size_t i = 0; i < 5; i++)
        {
            for (size_t k = 0; k < m_nb_chan; k++)
            {
                in_fifo_read[i][k]        = false;
                in_fifo_write[i][k]       = p_in[i][k].write.read();
                in_fifo_wdata[i][k].data  = p_in[i][k].data.read();
                in_fifo_wdata[i][k].eop   = p_in[i][k].eop.read();

                out_fifo_read[i][k]       = p_out[i][k].read.read();
                out_fifo_write[i][k]      = false;
                out_fifo_wdata[i][k].data = 0;
                out_fifo_wdata[i][k].eop  = false;
            }
        }

        ///////////////////////////////////////////////////////////////
        // output_get[i][j][k] : depends only on the output port states

        for(int i=0; i<5; i++)  // loop on inputs
        {
            for(int j=0; j<5; j++)  // loop on outputs
            {
                for(size_t k=0; k<m_nb_chan; k++)  // loop on channels
                {
                    output_get[i][j][k] = (r_output_index[j][k].read() == i) &&
                                          r_output_alloc[j][k] &&
                                          out_fifo[j][k].wok();
                }
            }
        }

        ///////////////////////////////////////////////////////////////////
        //  The following signals depend on the input FSM & FIFO states
        //  but do not depend on r_tdm :
        //  in_fifo_write[i][k], input_req[i][k], get[i][k], put[i][k]

        for(int i=0; i<5; i++)  // loop on the input ports
        {
            for(size_t k=0; k<m_nb_chan; k++)  // loop on the channels
            {
                get[i][k] = ( output_get[i][0][k]) ||
                            ( output_get[i][1][k]) ||
                            ( output_get[i][2][k]) ||
                            ( output_get[i][3][k]) ||
                            ( output_get[i][4][k]);

                switch( r_input_fsm[i][k] ) {
                case INFSM_IDLE:
                    put[i][k] = false;
                    if( in_fifo[i][k].rok() )
                    {
                        if( is_broadcast(in_fifo[i][k].read().data) )
                            input_req[i][k] = broadcast_route(1, i, in_fifo[i][k].read().data);
                        else
                            input_req[i][k] = xfirst_route(in_fifo[i][k].read().data);
                    }
                    else    input_req[i][k] = REQ_NOP;
                    break;
                case INFSM_REQ:
                    put[i][k] = true;
                    input_req[i][k] = xfirst_route(in_fifo[i][k].read().data);
                    break;
                case INFSM_DT:
                    put[i][k] = in_fifo[i][k].rok();
                    input_req[i][k] = REQ_NOP;
                    break;
                case INFSM_REQ_FIRST:
                    put[i][k] = true;
                    input_req[i][k] = broadcast_route(1, i, r_buf[i][k].data);
                    break;
                case INFSM_REQ_SECOND:
                    put[i][k] = true;
                    input_req[i][k] = broadcast_route(2, i, r_buf[i][k].data);
                    break;
                case INFSM_REQ_THIRD:
                    put[i][k] = true;
                    input_req[i][k] = broadcast_route(3, i, r_buf[i][k].data);
                    break;
                case INFSM_REQ_FOURTH:
                    put[i][k] = true;
                    input_req[i][k] = broadcast_route(4, i, r_buf[i][k].data);
                    break;
                case INFSM_DT_FIRST:
                case INFSM_DT_SECOND:
                case INFSM_DT_THIRD:
                case INFSM_DT_FOURTH:
                    put[i][k] = in_fifo[i][k].rok();
                    input_req[i][k] = REQ_NOP;
                    if( in_fifo[i][k].rok() and not in_fifo[i][k].read().eop )
                    {
                        std::cout << "ERROR in virtual_dspin_router " << name() << std::endl;
                        std::cout << "Broadcast packet longer than 2 flits received on input port["
                                  << k << "][" << i << "]" << std::endl;
                        exit(0);
                    }
                    break;
                } // end switch infsm
            } // end for channels
        } // end for inputs

        ////////////////////////////////////////////////////////////////
        // Time multiplexing in input ports :
        // final_put[i][k] & final_data[i], final_fifo_read[i][k],
        // and r_input_fsm[i][k] depend on r_tdm.

        //using namespace std;
        for(size_t i=0; i<5; i++) // loop on input ports
        {
            // Virtual channel has priority when r_tdm is true
            // The r_tdm[i][k] flip-flop toggle each time the owner uses the physical channel
            size_t token_pos = 0;
            bool tdm_ok[m_nb_chan];
            memset(tdm_ok, false, m_nb_chan * sizeof(bool));
            // compute current priority token position
            for(size_t k=0 ; k < m_nb_chan ; k++)
            {
                if (r_tdm[i][k].read())
                {
                    token_pos = k;
                    break;
                }
            }
            // select channel to be accepted
            for(size_t k=token_pos ; k < m_nb_chan + token_pos; k++)
            {
                if(put[i][k % m_nb_chan])
                {
                    tdm_ok[k % m_nb_chan] = true;

                    // compute next priority token position
                    r_tdm[i][token_pos] = false;
                    r_tdm[i][(token_pos + 1) % m_nb_chan] = true;

                    break;
                }
            }

            for(size_t k=0 ; k<m_nb_chan ; k++)
            {
                switch( r_input_fsm[i][k] )
                {
                case INFSM_IDLE:    // does not depend on tdm in IDLE state
                    final_put[i][k] = false;
                    if( in_fifo[i][k].rok() )
                    {
                        if( is_broadcast(in_fifo[i][k].read().data) )  // broadcast request
                        {
                            in_fifo_read[i][k] = true;

                            r_buf[i][k] = in_fifo[i][k].read();

                            if(input_req[i][k] == REQ_NOP) r_input_fsm[i][k] = INFSM_REQ_SECOND;
                            else                           r_input_fsm[i][k] = INFSM_REQ_FIRST;
                        }
                        else                 // not a broadcast request
                        {
                            in_fifo_read[i][k] = false;
                            r_input_fsm[i][k] = INFSM_REQ;
                        }
                    }
                break;
                case INFSM_REQ:
                    in_fifo_read[i][k] = get[i][k] && tdm_ok[k];
                    final_put[i][k] = tdm_ok[k];
                    if ( get[i][k] && tdm_ok[k] )
                    {
                        final_data[i] = in_fifo[i][k].read();
                        if(in_fifo[i][k].read().eop) r_input_fsm[i][k] = INFSM_IDLE;
                        else                         r_input_fsm[i][k] = INFSM_DT;
                    }
                break;
                case INFSM_DT:
                    in_fifo_read[i][k] = get[i][k] && tdm_ok[k];
                    final_put[i][k] = put[i][k] && tdm_ok[k];
                    if ( get[i][k] && put[i][k] && tdm_ok[k] )
                    {
                        final_data[i] = in_fifo[i][k].read();
                        if(in_fifo[i][k].read().eop) r_input_fsm[i][k] = INFSM_IDLE;
                    }
                break;
                case INFSM_REQ_FIRST:
                    in_fifo_read[i][k] = false;
                    if( input_req[i][k] == REQ_NOP )
                    {
                        final_put[i][k] = false;
                        r_input_fsm[i][k] = INFSM_REQ_SECOND;
                    }
                    else
                    {
                        final_put[i][k] = tdm_ok[k];
                        if( get[i][k] && tdm_ok[k] )
                        {
                            final_data[i] = r_buf[i][k];
                            r_input_fsm[i][k] = INFSM_DT_FIRST;
                        }
                    }
                    break;
                case INFSM_DT_FIRST:
                    in_fifo_read[i][k] = false;
                    final_put[i][k] = put[i][k] && tdm_ok[k];
                    if( get[i][k] && put[i][k] && tdm_ok[k] )
                    {
                        final_data[i] = in_fifo[i][k].read();
                        r_input_fsm[i][k] = INFSM_REQ_SECOND;
                    }
                    break;
                case INFSM_REQ_SECOND:
                    in_fifo_read[i][k] = false;
                    if( input_req[i][k] == REQ_NOP )
                    {
                        final_put[i][k] = false;
                        r_input_fsm[i][k] = INFSM_REQ_THIRD;
                    }
                    else
                    {
                        final_put[i][k] = tdm_ok[k];
                        if( get[i][k] && tdm_ok[k] )
                        {
                            final_data[i] = r_buf[i][k];
                            r_input_fsm[i][k] = INFSM_DT_SECOND;
                        }
                    }
                    break;
                case INFSM_DT_SECOND:
                    in_fifo_read[i][k] = false;
                    final_put[i][k] = put[i][k] && tdm_ok[k];
                    if( get[i][k] && put[i][k] && tdm_ok[k] )
                    {
                        final_data[i] = in_fifo[i][k].read();
                        r_input_fsm[i][k] = INFSM_REQ_THIRD;
                    }
                    break;
                case INFSM_REQ_THIRD:
                    in_fifo_read[i][k] = false;
                    if( input_req[i][k] == REQ_NOP )
                    {
                        final_put[i][k] = false;
                        r_input_fsm[i][k] = INFSM_REQ_FOURTH;
                    }
                    else
                    {
                        final_put[i][k] = tdm_ok[k];
                        if( get[i][k] && tdm_ok[k] )
                        {
                            final_data[i] = r_buf[i][k];
                            r_input_fsm[i][k] = INFSM_DT_THIRD;
                        }
                    }
                    break;
                case INFSM_DT_THIRD:
                    in_fifo_read[i][k] = false;
                    final_put[i][k] = put[i][k] && tdm_ok[k];
                    if( get[i][k] && put[i][k] && tdm_ok[k] )
                    {
                        final_data[i] = in_fifo[i][k].read();
                        r_input_fsm[i][k] = INFSM_REQ_FOURTH;
                    }
                    break;
                case INFSM_REQ_FOURTH:
                    if( input_req[i][k] == REQ_NOP )
                    {
                        in_fifo_read[i][k] = true;
                        final_put[i][k] = false;
                        r_input_fsm[i][k] = INFSM_IDLE;
                    }
                    else
                    {
                        in_fifo_read[i][k] = false;
                        final_put[i][k] = tdm_ok[k];
                        if( get[i][k] && tdm_ok[k] )
                        {
                            final_data[i] = r_buf[i][k];
                            r_input_fsm[i][k] = INFSM_DT_FOURTH;
                        }
                    }
                    break;
                case INFSM_DT_FOURTH:
                    in_fifo_read[i][k] = get[i][k] && tdm_ok[k];
                    final_put[i][k] = put[i][k] && tdm_ok[k];
                    if( get[i][k] && put[i][k] && tdm_ok[k] )
                    {
                        final_data[i] = in_fifo[i][k].read();
                        r_input_fsm[i][k] = INFSM_IDLE;
                    }
                    break;
                } // end switch infsm
            } // end for channels
        } // end for inputs

        ////////////////////////////////////////////////
        // output ports registers and fifos
        // r_output_index , r_output_alloc
        // out_fifo_read, out_fifo_write, out_fifo_wdata

        for(int j=0; j<5; j++) // loop on output ports
        {
            for(size_t k=0; k<m_nb_chan; k++) // loop on channels
            {
                // out_fifo_read[i][k], out_fifo_write[i][k], out_fifo_wdata[i][k]
                out_fifo_read[j][k] = p_out[j][k].read;
                out_fifo_write[j][k] = (output_get[0][j][k] && final_put[0][k]) ||
                                       (output_get[1][j][k] && final_put[1][k]) ||
                                       (output_get[2][j][k] && final_put[2][k]) ||
                                       (output_get[3][j][k] && final_put[3][k]) ||
                                       (output_get[4][j][k] && final_put[4][k]) ;
                for(int i=0; i<5; i++)  // loop on input ports
                {
                    if( output_get[i][j][k] && final_put[i][k] )     out_fifo_wdata[j][k] = final_data[i];
                }
                // r_output_alloc[j][k] & r_output_index[j][k]
                int index = r_output_index[j][k];
                if( !r_output_alloc[j][k] )         // allocation
                {

                    for(int n = index+1; n < index+6; n++) // loop on input ports
                    {
                        int x = n % 5;
                        if( input_req[x][k] == j )
                        {
                            r_output_index[j][k] = x;
                            r_output_alloc[j][k] = true;
                            break;
                        }
                    }
                }
                else if( out_fifo_write[j][k] and
                         out_fifo_wdata[j][k].eop and
                         out_fifo[j][k].wok() )     // de-allocation
                {
                    r_output_alloc[j][k] = false;
                }

            } // end for channels
        } // end for outputs

        ////////////////////////
        //  Updating fifos
        for(int i=0; i<5; i++)
        {
            for(size_t k=0; k<m_nb_chan; k++)
            {
                in_fifo[i][k].update( in_fifo_read[i][k],
                                      in_fifo_write[i][k],
                                      in_fifo_wdata[i][k] );
                out_fifo[i][k].update( out_fifo_read[i][k],
                                      out_fifo_write[i][k],
                                      out_fifo_wdata[i][k] );
            }
        }
    } // end transition

    //////////////////////
    tmpl(void)::genMoore()
    {
        for(int i=0; i<5; i++)
        {
            for(size_t k=0; k<m_nb_chan; k++)
            {
                // input ports
                p_in[i][k].read   = in_fifo[i][k].wok();

                // output ports
                p_out[i][k].write = out_fifo[i][k].rok();
                p_out[i][k].data  = out_fifo[i][k].read().data;
                p_out[i][k].eop   = out_fifo[i][k].read().eop;
            }
        }
    } // end genMoore

}} // end namespaces
