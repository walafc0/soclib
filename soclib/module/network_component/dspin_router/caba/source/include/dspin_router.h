/* -*- c++ -*-
  *
  * File : dspin_router.h
  * Copyright (c) UPMC, Lip6
  * Authors : Alain Greiner, Abbas Sheibanyrad, Ivan Miro, Zhen Zhang
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

    ///////////////////////////////////////////////////////////////////////////
    // Implementation Note :
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

#ifndef DSPIN_ROUTER_H_
#define DSPIN_ROUTER_H_

#include <systemc>
#include "caba_base_module.h"
#include "generic_fifo.h"
#include "dspin_interface.h"
#include "alloc_elems.h"

namespace soclib { namespace caba {

using namespace sc_core;

template<int flit_width>
class DspinRouter
: public soclib::caba::BaseModule
{
	// Port indexing
	enum 
    {
		REQ_NORTH	= 0,
		REQ_SOUTH	= 1,
		REQ_EAST	= 2,
		REQ_WEST	= 3,
		REQ_LOCAL	= 4,
        REQ_NOP     = 5,
	};

    // Input Port FSM
    enum 
    {
        INFSM_IDLE,
        INFSM_REQ,
        INFSM_ALLOC,
        INFSM_REQ_FIRST,
        INFSM_ALLOC_FIRST,
        INFSM_REQ_SECOND,
        INFSM_ALLOC_SECOND,
        INFSM_REQ_THIRD,
        INFSM_ALLOC_THIRD,
        INFSM_REQ_FOURTH,
        INFSM_ALLOC_FOURTH,
    };

    protected:
    SC_HAS_PROCESS(DspinRouter);

    public:

	// ports
	sc_in<bool>                 p_clk;
	sc_in<bool>                 p_resetn;
	DspinInput<flit_width>      *p_in;
	DspinOutput<flit_width>	    *p_out;

	// constructor / destructor
	DspinRouter( sc_module_name  name, 
                 const size_t    x,
                 const size_t    y,
                 const size_t    x_width,
                 const size_t    y_width,
                 const size_t    in_fifo_depth,
                 const size_t    out_fifo_depth,
                 const bool      broadcast_supported = false );  // default value

    private:

    // define the FIFO flit
    typedef struct internal_flit_s 
    {
        sc_uint<flit_width>  data;
        bool                 eop;
    } internal_flit_t;
    
    // registers
	sc_signal<bool>				*r_alloc_out;   // output port is allocated
	sc_signal<size_t>           *r_index_out;   // index of owner input port

    sc_signal<int>              *r_fsm_in;      // input port state
	sc_signal<size_t>           *r_index_in;    // index of requested output port
    internal_flit_t             *r_buf_in;      // save first flit for a broadcast

	// fifos
	soclib::caba::GenericFifo<internal_flit_t>*  r_fifo_in;
	soclib::caba::GenericFifo<internal_flit_t>*  r_fifo_out;

	// structural parameters
	size_t	                    m_local_x;
	size_t	                    m_local_y;
	size_t	                    m_x_width;
	size_t	                    m_x_shift;
	size_t	                    m_x_mask;
	size_t	                    m_y_width;
	size_t	                    m_y_shift;
	size_t	                    m_y_mask;
    bool                        m_broadcast_supported;

    // methods 
    void    transition();
    void    genMoore();
    int     xfirst_route( sc_uint<flit_width> data );
    int     broadcast_route( int iter, int source, sc_uint<flit_width> data );
    bool    is_broadcast( sc_uint<flit_width> data );

    public:

    void    print_trace();
};

}} // end namespace
               
#endif // DSPIN_ROUTER_BC_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
