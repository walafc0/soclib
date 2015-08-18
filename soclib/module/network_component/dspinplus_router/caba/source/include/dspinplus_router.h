/* -*- c++ -*-
  * File : dspinplus_router.h
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
  */

#ifndef DSPINPLUS_ROUTER_H_
#define DSPINPLUS_ROUTER_H_

#include <systemc>
#include "caba_base_module.h"
#include "generic_fifo.h"
#include "dspin_interface.h"

namespace soclib{ namespace caba {
    
    using namespace sc_core;

    template<int dspin_data_size, int dspin_fifo_size, int dspin_yx_size>
	class DspinPlusRouter
	: public soclib::caba::BaseModule
	{

	    // FSM of request
	    enum{
		NORTH	= 0,
		SOUTH	= 1,
		EAST	= 2,
		WEST	= 3,
		LOCAL	= 4
	    };

	    protected:
	    SC_HAS_PROCESS(DspinPlusRouter);

	    public:
	    // ports
	    sc_in<bool>                             	p_clk;
	    sc_in<bool>                             	p_resetn;

	    // fifo req ant rsp
	    DspinOutput<dspin_data_size>		*p_out;
	    DspinInput<dspin_data_size>			*p_in;

	    // constructor / destructor
	    DspinPlusRouter(sc_module_name    insname, int indent);

	    private:
	    // internal registers
	    sc_signal<int>				*r_alloc_out;
	    sc_signal<int>				*r_alloc_in;
	    sc_signal<int>              		*r_index_out;
	    sc_signal<int>              		*r_index_in;

	    sc_signal<int>				*r_bop;

	    // deux fifos req and rsp
	    soclib::caba::GenericFifo<sc_uint<dspin_data_size> > *fifo_in;
	    soclib::caba::GenericFifo<sc_uint<dspin_data_size> > *fifo_out;

	    // Index of router
	    int		XLOCAL;
	    int		YLOCAL;

	    // methods systemc
	    void transition();
	    void genMoore();

	    // checker
	    soclib_static_assert(dspin_fifo_size <= 256 && dspin_fifo_size >= 1);
	    soclib_static_assert(dspin_yx_size <= 6 && dspin_yx_size >= 1);
	};

}} // end namespace

#endif
