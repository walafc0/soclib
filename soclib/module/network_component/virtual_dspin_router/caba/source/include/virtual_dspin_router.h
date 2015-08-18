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
 * Date         : july 2010
 * Maintainers  : alexandre.joannou@lip6.fr
 * Copyright: UPMC - LIP6
 */

#ifndef VIRTUAL_DSPIN_ROUTER_H
#define VIRTUAL_DSPIN_ROUTER_H

#include <systemc>
#include "caba_base_module.h"
#include "generic_fifo.h"
#include "dspin_interface.h"

namespace soclib { namespace caba {

template<int flit_width>
class VirtualDspinRouter: public soclib::caba::BaseModule
{			

protected:
	SC_HAS_PROCESS(VirtualDspinRouter);

public:

enum{   // request type (six values, can be encoded on 3 bits)
    REQ_NORTH,
    REQ_SOUTH,
    REQ_EAST,
	REQ_WEST,
	REQ_LOCAL,
    REQ_NOP,
};

enum{	// INFSM States 
	INFSM_IDLE,
    INFSM_REQ,
    INFSM_DT,
	INFSM_REQ_FIRST,
	INFSM_DT_FIRST,
	INFSM_REQ_SECOND,
	INFSM_DT_SECOND,
	INFSM_REQ_THIRD,
	INFSM_DT_THIRD,
	INFSM_REQ_FOURTH,
	INFSM_DT_FOURTH,
	};

enum{  // port indexing
	NORTH	= 0,
	SOUTH	= 1,
	EAST	= 2,
	WEST	= 3,
	LOCAL	= 4,
};

	// ports
	sc_core::sc_in<bool>             	    p_clk;
	sc_core::sc_in<bool>             	    p_resetn;
	soclib::caba::DspinOutput<flit_width>*  p_out[5];
	soclib::caba::DspinInput<flit_width>*   p_in[5];

	// constructor 
	VirtualDspinRouter( sc_module_name  insname,
                        int	            x,                  // x coordinate in the mesh
                        int	            y,                  // y coordinate in the mesh 
                        int	            x_width,            // number of bits for x field
                        int	            y_width,            // number of bits for y field
                        size_t          nb_chan,            // number of virtual channels
                        int	            in_fifo_depth,		// input fifo depth
                        int	            out_fifo_depth );	// output fifo depth

	// destructor 
	~VirtualDspinRouter();

	// public methods
        void	print_trace(size_t channel = 0);
        void	debug_trace(size_t channel = 0);

private:

    // define FIFO flit
    typedef struct internal_flit_s 
    {
        sc_uint<flit_width>  data;
        bool                 eop;
    } internal_flit_t;
    

	// input port registers & fifos
	sc_core::sc_signal<bool>*                   r_tdm[5];           // Time Multiplexing
	sc_core::sc_signal<int>*                    r_input_fsm[5];	    // FSM state
	internal_flit_t*                            r_buf[5];           // fifo extension


	// output port registers & fifos
	sc_core::sc_signal<int>*                    r_output_index[5];  // allocated input  
	sc_core::sc_signal<bool>*                   r_output_alloc[5];  // allocation status 

	soclib::caba::GenericFifo<internal_flit_t>* in_fifo[5];         // input fifos
	soclib::caba::GenericFifo<internal_flit_t>* out_fifo[5];        // output fifos

	// structural variables
	int	m_local_x;                          // router x coordinate
	int	m_local_y;                          // router y coordinate
    int	m_x_width;                          // number of bits for x field
    int	m_y_width;                          // number of bits for y field
    int	m_x_shift;                          // number of bits to shift for x field
    int	m_x_mask;                           // number of bits to mask for x field
    int	m_y_shift;                          // number of bits to shift for y field
    int	m_y_mask;                           // number of bits to mask for y field
    size_t	m_nb_chan;                      // number of bits to mask for y field

	// methods 
	void transition();
	void genMoore();

	// Utility functions
	int 	xfirst_route(sc_uint<flit_width> data);			
	int 	broadcast_route(int dst, int src, sc_uint<flit_width> data);	
	bool 	is_broadcast(sc_uint<flit_width> data);

}; // end class VirtualDspinRouter
	
}} // end namespace

#endif // end VIRTUAL_DSPIN_ROUTER_H_
