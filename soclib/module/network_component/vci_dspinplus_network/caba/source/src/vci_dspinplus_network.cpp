/* -*- c++ -*-
  * File : vci_dspinplus_network.cpp
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

#include <sstream>
#include <cassert>
#include "alloc_elems.h"
#include "../include/vci_dspinplus_network.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, int dspin_fifo_size, int dspin_yx_size> x VciDspinPlusNetwork<vci_param, dspin_fifo_size, dspin_yx_size>

    ////////////////////////////////
    //      constructor
    //
    ////////////////////////////////

    tmpl(/**/)::VciDspinPlusNetwork(sc_module_name insname,
	    const soclib::common::MappingTable &mt,
	    size_t width_network,
	    size_t height_network) 
	: soclib::caba::BaseModule(insname),
	m_width_network(width_network),
	m_height_network(height_network)
    {
	assert( width_network <= 15 && height_network <= 15 );

	//
	// VCI_Interfaces
	//
	p_to_initiator = soclib::common::alloc_elems<soclib::caba::VciTarget<vci_param> >   ("p_to_initiator", height_network, width_network);
	p_to_target    = soclib::common::alloc_elems<soclib::caba::VciInitiator<vci_param> >("p_to_target"   , height_network, width_network);

	//
	// DSPIN_Signals
	//
	s_req_NS = soclib::common::alloc_elems<soclib::caba::DspinSignals<37> >( "s_req_NS", height_network + 2, width_network + 2); 
	s_req_EW = soclib::common::alloc_elems<soclib::caba::DspinSignals<37> >( "s_req_EW", height_network + 2, width_network + 2);
	s_req_SN = soclib::common::alloc_elems<soclib::caba::DspinSignals<37> >( "s_req_SN", height_network + 2, width_network + 2);
	s_req_WE = soclib::common::alloc_elems<soclib::caba::DspinSignals<37> >( "s_req_WE", height_network + 2, width_network + 2);

	s_req_RW = soclib::common::alloc_elems<soclib::caba::DspinSignals<37> >( "s_req_RW", height_network    , width_network    );
	s_req_WR = soclib::common::alloc_elems<soclib::caba::DspinSignals<37> >( "s_req_WR", height_network    , width_network    );

	s_rsp_NS = soclib::common::alloc_elems<soclib::caba::DspinSignals<33> >( "s_rsp_NS", height_network + 2, width_network + 2);
	s_rsp_EW = soclib::common::alloc_elems<soclib::caba::DspinSignals<33> >( "s_rsp_EW", height_network + 2, width_network + 2);
	s_rsp_SN = soclib::common::alloc_elems<soclib::caba::DspinSignals<33> >( "s_rsp_SN", height_network + 2, width_network + 2);
	s_rsp_WE = soclib::common::alloc_elems<soclib::caba::DspinSignals<33> >( "s_rsp_WE", height_network + 2, width_network + 2);

	s_rsp_RW = soclib::common::alloc_elems<soclib::caba::DspinSignals<33> >( "s_rsp_RW", height_network    , width_network    );
	s_rsp_WR = soclib::common::alloc_elems<soclib::caba::DspinSignals<33> >( "s_rsp_WR", height_network    , width_network    );  

	//
	// Dspin_wrapper
	//
	t_initiator_wrapper = new soclib::caba::VciDspinPlusInitiatorWrapper<vci_param, dspin_fifo_size, dspin_yx_size>**[height_network];
	t_target_wrapper    = new soclib::caba::VciDspinPlusTargetWrapper<vci_param, dspin_fifo_size, dspin_yx_size>**[height_network];

	//
	// Dspin_Router
	//
	t_req_router = new soclib::caba::DspinPlusRouter<37, dspin_fifo_size, dspin_yx_size>**[height_network];
	t_rsp_router = new soclib::caba::DspinPlusRouter<33, dspin_fifo_size, dspin_yx_size>**[height_network];

	for( size_t y = 0; y < height_network; y++ ){

	    t_initiator_wrapper[y] = new soclib::caba::VciDspinPlusInitiatorWrapper<vci_param, dspin_fifo_size, dspin_yx_size>*[width_network];
	    t_target_wrapper[y]    = new soclib::caba::VciDspinPlusTargetWrapper<vci_param, dspin_fifo_size, dspin_yx_size>*[width_network];

	    t_req_router[y] = new soclib::caba::DspinPlusRouter<37, dspin_fifo_size, dspin_yx_size>*[width_network];
	    t_rsp_router[y] = new soclib::caba::DspinPlusRouter<33, dspin_fifo_size, dspin_yx_size>*[width_network];


	    for( size_t x = 0; x < width_network ; x++ ){
		char str_initiator_wrapper[32];
		sprintf(str_initiator_wrapper, "t_initiator_wrapper[%i][%i]", (int)y,(int)x);
		t_initiator_wrapper[y][x] = new soclib::caba::VciDspinPlusInitiatorWrapper<vci_param, dspin_fifo_size, dspin_yx_size>(str_initiator_wrapper, mt);
		char str_target_wrapper[32];
		sprintf(str_target_wrapper, "t_target_wrapper[%i][%i]", (int)y,(int)x);
		t_target_wrapper[y][x] = new soclib::caba::VciDspinPlusTargetWrapper<vci_param, dspin_fifo_size, dspin_yx_size>(str_target_wrapper, mt);

		char str_req_router[32];
		sprintf(str_req_router, "t_req_router[%i][%i]", (int)y,(int)x);
		t_req_router[y][x] = new soclib::caba::DspinPlusRouter<37, dspin_fifo_size, dspin_yx_size>(str_req_router, ((y<<4) & 0xF0) | (x & 0x0F) );

		char str_rsp_router[32];
		sprintf(str_rsp_router, "t_rsp_router[%i][%i]", (int)y,(int)x);
		t_rsp_router[y][x] = new soclib::caba::DspinPlusRouter<33, dspin_fifo_size, dspin_yx_size>(str_rsp_router, ((y<<4) & 0xF0) | (x & 0x0F) );
	    }
	}

	//
	// NETLIST
	//
	for( size_t y = 0 ; y < height_network ; y++ ){
	    for( size_t x = 0 ; x < width_network ; x++ ){
		//
		// CLK RESETN
		//
		t_initiator_wrapper[y][x]->p_clk(p_clk);
		t_initiator_wrapper[y][x]->p_resetn(p_resetn);

		t_target_wrapper[y][x]->p_clk(p_clk);
		t_target_wrapper[y][x]->p_resetn(p_resetn);

		t_rsp_router[y][x]->p_clk(p_clk);
		t_rsp_router[y][x]->p_resetn(p_resetn);

		t_req_router[y][x]->p_clk(p_clk);
		t_req_router[y][x]->p_resetn(p_resetn);

		//
		// VCI <=> Wrapper
		//
		t_initiator_wrapper[y][x]->p_vci(p_to_initiator[y][x]);
		t_target_wrapper[y][x]->p_vci(p_to_target[y][x]);

		//
		// DSPIN <=> Wrapper
		//		

		t_initiator_wrapper[y][x]->p_dspin_out(s_req_WR[y][x]);
		t_req_router[y][x]->p_in[LOCAL](s_req_WR[y][x]);

		t_req_router[y][x]->p_out[LOCAL](s_req_RW[y][x]);
		t_target_wrapper[y][x]->p_dspin_in(s_req_RW[y][x]);

		t_target_wrapper[y][x]->p_dspin_out(s_rsp_WR[y][x]);
		t_rsp_router[y][x]->p_in[LOCAL](s_rsp_WR[y][x]);

		t_rsp_router[y][x]->p_out[LOCAL](s_rsp_RW[y][x]);
		t_initiator_wrapper[y][x]->p_dspin_in(s_rsp_RW[y][x]);


		//
		// DSPIN <=> DSPIN
		//

		t_req_router[y][x]->p_in[NORTH](s_req_SN[y+2][x+1]);
		t_req_router[y][x]->p_in[SOUTH](s_req_NS[y][x+1]);
		t_req_router[y][x]->p_in[EAST](s_req_WE[y+1][x+2]);
		t_req_router[y][x]->p_in[WEST](s_req_EW[y+1][x]);

		t_req_router[y][x]->p_out[NORTH](s_req_NS[y+1][x+1]);
		t_req_router[y][x]->p_out[SOUTH](s_req_SN[y+1][x+1]);
		t_req_router[y][x]->p_out[EAST](s_req_EW[y+1][x+1]);
		t_req_router[y][x]->p_out[WEST](s_req_WE[y+1][x+1]);

		t_rsp_router[y][x]->p_in[NORTH] (s_rsp_SN[y+2][x+1]);
		t_rsp_router[y][x]->p_in[SOUTH] (s_rsp_NS[y][x+1]);
		t_rsp_router[y][x]->p_in[EAST]  (s_rsp_WE[y+1][x+2]);
		t_rsp_router[y][x]->p_in[WEST]  (s_rsp_EW[y+1][x]);

		t_rsp_router[y][x]->p_out[NORTH](s_rsp_NS[y+1][x+1]);
		t_rsp_router[y][x]->p_out[SOUTH](s_rsp_SN[y+1][x+1]);
		t_rsp_router[y][x]->p_out[EAST] (s_rsp_EW[y+1][x+1]);
		t_rsp_router[y][x]->p_out[WEST] (s_rsp_WE[y+1][x+1]);
	    }
	}
    }

    tmpl(/**/)::~VciDspinPlusNetwork()
    {
	for( size_t y = 0; y < m_height_network; y++ ){
	    soclib::common::dealloc_elems( p_to_initiator[y], m_width_network);
	    soclib::common::dealloc_elems( p_to_target[y], m_width_network);

	    for( size_t x = 0; x < m_width_network ; x ++ ){
		delete t_initiator_wrapper[y][x];
		delete t_target_wrapper[y][x];
		delete t_req_router[y][x];
		delete t_rsp_router[y][x];
	    }

	    delete [] t_initiator_wrapper[y];
	    delete [] t_target_wrapper[y];
	    delete [] t_req_router[y];
	    delete [] t_rsp_router[y];
	}

	delete [] p_to_initiator;
	delete [] p_to_target;

	delete [] t_initiator_wrapper;
	delete [] t_target_wrapper;
	delete [] t_req_router;
	delete [] t_rsp_router;

	soclib::common::dealloc_elems(s_req_NS, m_height_network + 2, m_width_network + 2);
	soclib::common::dealloc_elems(s_req_EW, m_height_network + 2, m_width_network + 2);
	soclib::common::dealloc_elems(s_req_SN, m_height_network + 2, m_width_network + 2);
	soclib::common::dealloc_elems(s_req_WE, m_height_network + 2, m_width_network + 2);

	soclib::common::dealloc_elems(s_req_RW, m_height_network    , m_width_network    );
	soclib::common::dealloc_elems(s_req_WR, m_height_network    , m_width_network    );

	soclib::common::dealloc_elems(s_rsp_NS, m_height_network + 2, m_width_network + 2);
	soclib::common::dealloc_elems(s_rsp_EW, m_height_network + 2, m_width_network + 2);
	soclib::common::dealloc_elems(s_rsp_SN, m_height_network + 2, m_width_network + 2);
	soclib::common::dealloc_elems(s_rsp_WE, m_height_network + 2, m_width_network + 2);

	soclib::common::dealloc_elems(s_rsp_RW, m_height_network    , m_width_network    );
	soclib::common::dealloc_elems(s_rsp_WR, m_height_network    , m_width_network    );

    }

    tmpl(void)::trace(sc_core::sc_trace_file* tf)
    {
	char tmp[100];
	for ( size_t y  = 0 ; y < m_height_network; y++ )
	    for ( size_t x = 0 ; x < m_width_network ; x ++ )
	    {
		if( y + 1 < m_height_network )
		{
		    // --- CMD --- //
		    sprintf(tmp,"cmd_router(%d,%d)_N_input",y,x);
		    s_req_SN[y+2][x+1].trace(tf,tmp);

		    sprintf(tmp,"cmd_router(%d,%d)_N_output",y,x);
		    s_req_NS[y+1][x+1].trace(tf,tmp);

		    sprintf(tmp,"rsp_router(%d,%d)_N_input",y,x);
		    s_rsp_SN[y+2][x+1].trace(tf,tmp);

		    sprintf(tmp,"rsp_router(%d,%d)_N_output",y,x);
		    s_rsp_NS[y+1][x+1].trace(tf,tmp);

		}

		if( y > 0 )
		{
		    sprintf(tmp,"cmd_router(%d,%d)_S_input",y,x);
		    s_req_NS[y][x+1].trace(tf,tmp);

		    sprintf(tmp,"cmd_router(%d,%d)_S_output",y,x);
		    s_req_SN[y+1][x+1].trace(tf,tmp);

		    sprintf(tmp,"rsp_router(%d,%d)_S_input",y,x);
		    s_rsp_NS[y][x+1].trace(tf,tmp);

		    sprintf(tmp,"rsp_router(%d,%d)_S_output",y,x);
		    s_rsp_SN[y+1][x+1].trace(tf,tmp);
		}

		if( x + 1 < m_width_network )
		{

		    sprintf(tmp,"cmd_router(%d,%d)_E_input",y,x);
		    s_req_WE[y+1][x+2].trace(tf,tmp);

		    sprintf(tmp,"cmd_router(%d,%d)_E_output",y,x);
		    s_req_EW[y+1][x+1].trace(tf,tmp);

		    sprintf(tmp,"rsp_router(%d,%d)_E_input",y,x);
		    s_rsp_WE[y+1][x+2].trace(tf,tmp);

		    sprintf(tmp,"rsp_router(%d,%d)_E_output",y,x);
		    s_rsp_EW[y+1][x+1].trace(tf,tmp);
		}

		if( x > 0 )
		{
		    sprintf(tmp,"cmd_router(%d,%d)_W_input",y,x);
		    s_req_EW[y+1][x].trace(tf,tmp);

		    sprintf(tmp,"cmd_router(%d,%d)_W_output",y,x);
		    s_req_WE[y+1][x+1].trace(tf,tmp);

		    sprintf(tmp,"rsp_router(%d,%d)_W_input",y,x);
		    s_rsp_EW[y+1][x].trace(tf,tmp);

		    sprintf(tmp,"rsp_router(%d,%d)_W_output",y,x);
		    s_rsp_WE[y+1][x+1].trace(tf,tmp);

		}

		sprintf(tmp,"cmd_router(%d,%d)_L_input",y,x);
		s_req_WR[y][x].trace(tf,tmp);

		sprintf(tmp,"cmd_router(%d,%d)_L_output",y,x);
		s_req_RW[y][x].trace(tf,tmp);


		sprintf(tmp,"rsp_router(%d,%d)_L_input",y,x);
		s_rsp_WR[y][x].trace(tf,tmp);

		sprintf(tmp,"rsp_router(%d,%d)_L_output",y,x);
		s_rsp_RW[y][x].trace(tf,tmp);
	    }
    }
}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
