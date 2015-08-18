/* -*- c++ -*-
  * File : dspinplus_router.cpp
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

#include "../include/dspinplus_router.h"
#include "register.h"
#include <cstdlib>
#include <cassert>
#include <sstream>
#include "alloc_elems.h"
#include <new>

namespace soclib { namespace caba {

#define tmpl(x) template<int dspin_data_size, int dspin_fifo_size, int dspin_yx_size> x DspinPlusRouter<dspin_data_size, dspin_fifo_size, dspin_yx_size>

    ////////////////////////////////
    //      constructor
    ////////////////////////////////

    tmpl(/**/)::DspinPlusRouter(sc_module_name insname, int indent)
	: soclib::caba::BaseModule(insname)
    {
	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();
	SC_METHOD (genMoore);
	dont_initialize();
	sensitive  << p_clk.neg();

	fifo_in = (soclib::caba::GenericFifo<sc_uint<dspin_data_size> >*)
	  malloc(sizeof(soclib::caba::GenericFifo<sc_uint<dspin_data_size> >)*5);
	fifo_out = (soclib::caba::GenericFifo<sc_uint<dspin_data_size> >*)
	  malloc(sizeof(soclib::caba::GenericFifo<sc_uint<dspin_data_size> >)*5);

	for( int i = 0 ;i < 5; i++ ){
		std::ostringstream o;
		o << i;
	    new(&fifo_in[i]) soclib::caba::GenericFifo<sc_uint<dspin_data_size> >(std::string("FIFO_IN_")+o.str(), dspin_fifo_size) ;
	    new(&fifo_out[i]) soclib::caba::GenericFifo<sc_uint<dspin_data_size> >(std::string("FIFO_OUT")+o.str(), dspin_fifo_size) ;
	}

	p_out = soclib::common::alloc_elems<DspinOutput<dspin_data_size> >("out", 5);
	p_in = soclib::common::alloc_elems<DspinInput<dspin_data_size> >("in", 5);
	r_alloc_out = soclib::common::alloc_elems<sc_signal<int> >("alloc_out", 5);
	r_alloc_in = soclib::common::alloc_elems<sc_signal<int> >("alloc_in", 5);
	r_index_out = soclib::common::alloc_elems<sc_signal<int> >("index_out", 5);
	r_index_in = soclib::common::alloc_elems<sc_signal<int> >("index_in", 5);
	r_bop = soclib::common::alloc_elems<sc_signal<int> >("bop", 5);

	XLOCAL =  indent & 0x0000000F;
	YLOCAL = (indent & 0x000000F0) >> 4;

	assert( XLOCAL <= 15 && XLOCAL >= 0);
	assert( YLOCAL <= 15 && YLOCAL >= 0);
    } //  end constructor

    ////////////////////////////////
    //      transition
    ////////////////////////////////
    tmpl(void)::transition()
    {
	int			i,j,k;
	int			xreq,yreq;
	bool			fifo_in_write[5];	// control signals
	bool    		fifo_in_read[5];	// for the input fifos
	sc_uint<dspin_data_size>	fifo_in_data[5];
	bool			fifo_out_write[5];	// control signals
	bool    		fifo_out_read[5];	// for the output fifos
	sc_uint<dspin_data_size>	fifo_out_data[5];
	bool			req[5][5];		// REQ[i][j] signals from

	// input i to output j
	if(p_resetn == false) {
	    for(i = 0 ; i < 5 ; i++) {
		r_alloc_in[i] = false;
		r_alloc_out[i] = false;
		r_index_in[i] = 0;
		r_index_out[i] = 0;
		fifo_in[i].init();
		fifo_out[i].init();
		r_bop[i] = false;
	    }
	} else {

	    // fifo_in_write[i] and fifo_in_data[i]

	    for(i = 0 ; i < 5 ; i++) {
		fifo_in_write[i] = p_in[i].write.read();
		fifo_in_data[i] = p_in[i].data.read();
	    }

	    // fifo_out_read[i] 

	    for(i = 0 ; i < 5 ; i++) {
		fifo_out_read[i] = p_out[i].read.read();
	    }

	    // req[i][j]  : implement the X first routing algorithm

	    for(i = 0 ; i < 5 ; i++) { // loop on the input ports
		if((fifo_in[i].rok() == true) && 
			(((fifo_in[i].read() >> (dspin_data_size-1) ) & DSPINPLUS_EOP) != DSPINPLUS_EOP) &&
			(r_bop[i] == false)) {

		    xreq = (int)((fifo_in[i].read() >> (dspin_data_size - 1 - dspin_yx_size * 2)) & (0x7FFFFFFF >> (31 - dspin_yx_size)));
		    yreq = (int)((fifo_in[i].read() >> (dspin_data_size - 1 - dspin_yx_size)) & (0x7FFFFFFF >> (31 - dspin_yx_size)));

		    if(xreq < XLOCAL) {
			req[i][LOCAL] = false;
			req[i][NORTH] = false;
			req[i][SOUTH] = false;
			req[i][EAST]  = false;
			req[i][WEST]  = true;
		    } else if(xreq > XLOCAL) {
			req[i][LOCAL] = false;
			req[i][NORTH] = false;
			req[i][SOUTH] = false;
			req[i][EAST]  = true;
			req[i][WEST]  = false;
		    } else if(yreq < YLOCAL) {
			req[i][LOCAL] = false;
			req[i][NORTH] = false;
			req[i][SOUTH] = true;
			req[i][EAST]  = false;
			req[i][WEST]  = false;
		    } else if(yreq > YLOCAL) {
			req[i][LOCAL] = false;
			req[i][NORTH] = true;
			req[i][SOUTH] = false;
			req[i][EAST]  = false;
			req[i][WEST]  = false;
		    } else {
			req[i][LOCAL] = true;
			req[i][NORTH] = false;
			req[i][SOUTH] = false;
			req[i][EAST]  = false;
			req[i][WEST]  = false;
		    }
		} else {
		    req[i][LOCAL] = false;
		    req[i][NORTH] = false;
		    req[i][SOUTH] = false;
		    req[i][EAST]  = false;
		    req[i][WEST]  = false;
		}
	    } // end loop on the inputs

	    // fifo_in_read[i]

	    for(i = 0 ; i < 5 ; i++) { // loop on the inputs
		if(r_alloc_in[i] == true) {
		    fifo_in_read[i] = fifo_out[r_index_in[i]].wok();
		} else {
		    fifo_in_read[i] = false;
		}
	    } // end loop on the inputs

	    // fifo_out_write[j] and fifo_out_data[j]

	    for(j = 0 ; j < 5 ; j++) { // loop on the outputs
		if(r_alloc_out[j] == true) {
		    fifo_out_write[j] = fifo_in[r_index_out[j]].rok();
		    fifo_out_data[j]  = fifo_in[r_index_out[j]].read();
		} else {
		    fifo_out_write[j] = false;
		}
	    } // end loop on the outputs

	    // r_alloc_in, r_alloc_out, r_index_in et r_index_out : implements the round-robin allocation policy

	    for(j = 0 ; j < 5 ; j++) { // loop on the outputs
		if(r_alloc_out[j] == false) { // possible new allocation

		    //Routage par round-robin
		    for(k = r_index_out[j]+1 ; k < (r_index_out[j] + 6) ; k++) { // loop on the inputs
			i = k % 5;
			if(req[i][j] == true ) {
			    r_bop[i] = true;
			    r_alloc_out[j] = true;
			    r_index_out[j] = i;
			    r_alloc_in[i] = true;
			    r_index_in[i] = j;
			    break;
			}
		    } // end loop on the inputs



		} else { // possible desallocation
		  if((((fifo_in[r_index_out[j]].read() >> (dspin_data_size-1) ) & DSPINPLUS_EOP) == DSPINPLUS_EOP) && 
			  (fifo_out[j].wok() == true ) && fifo_in[r_index_out[j]].rok()) {

		        r_bop[r_index_out[j]] = false;
			r_alloc_out[j] = false;
			r_alloc_in[r_index_out[j]] = false;			
		    }
		}
	    } // end loop on the outputs

	    //  FIFOS

	    for(i = 0 ; i < 5 ; i++) {
	      if((fifo_in_write[i] == true ) && (fifo_in_read[i] == true ))
		{fifo_in[i].put_and_get(fifo_in_data[i]);}
	      if((fifo_in_write[i] == true ) && (fifo_in_read[i] == false))
		{fifo_in[i].simple_put(fifo_in_data[i]);}
	      if((fifo_in_write[i] == false) && (fifo_in_read[i] == true ))
		{fifo_in[i].simple_get();}
	    }
	    for(i = 0 ; i < 5 ; i++) {
	      if((fifo_out_write[i] == true ) && (fifo_out_read[i] == true ))
		{fifo_out[i].put_and_get(fifo_out_data[i]);}
	      if((fifo_out_write[i] == true ) && (fifo_out_read[i] == false))
		{fifo_out[i].simple_put(fifo_out_data[i]);}
	      if((fifo_out_write[i] == false) && (fifo_out_read[i] == true ))
		{fifo_out[i].simple_get();}
	    }
	}
    } // end transition

    ////////////////////////////////
    //      genMealy
    ////////////////////////////////
    tmpl(void)::genMoore()
    {
      int	i;

      // input ports : READ signals
      for(i = 0 ; i < 5 ; i++) { 
	p_in[i].read = fifo_in[i].wok();
      }
      
      // output ports : DATA & WRITE signals
      for(i = 0 ; i < 5 ; i++) { 
	p_out[i].data = fifo_out[i].read(); 
	p_out[i].write =  fifo_out[i].rok();
      }
      
    } // end genMoore

}} // end namespace

