/* -*- c++ -*-
  * File : dspin_local_crossbar.h
  * Copyright (c) UPMC, Lip6
  * Author : Alain Greiner
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

#ifndef DSPIN_LOCAL_CROSSBAR_H_
#define DSPIN_LOCAL_CROSSBAR_H_

#include <systemc>
#include <cassert>
#include "alloc_elems.h"
#include "caba_base_module.h"
#include "generic_fifo.h"
#include "dspin_interface.h"
#include "mapping_table.h"
#include "address_decoding_table.h"

namespace soclib { namespace caba {

    using namespace sc_core;
    using namespace soclib::common;

    template<size_t flit_width>
	class DspinLocalCrossbar
	: public soclib::caba::BaseModule
	{
        // Input Port FSM
        enum 
        {
        INFSM_IDLE,
        INFSM_REQ,
        INFSM_ALLOC,
        INFSM_REQ_BC,
        INFSM_ALLOC_BC,
        };

	    protected:
	    SC_HAS_PROCESS(DspinLocalCrossbar);

	    public:

	    // ports
	    sc_in<bool>                     p_clk;
	    sc_in<bool>                     p_resetn;
	    DspinInput<flit_width>		    p_global_in;
	    DspinOutput<flit_width>			p_global_out;
	    DspinInput<flit_width>*		    p_local_in;
	    DspinOutput<flit_width>*		p_local_out;

        void      print_trace();

	    // constructor / destructor
	    DspinLocalCrossbar( sc_module_name      name, 
                            const MappingTable  &mt,
                            const size_t        x,              // cluster x coordinate
                            const size_t        y,              // cluster y coordinate
                            const size_t        x_width,        // x field width
                            const size_t        y_width,        // y field width        
                            const size_t        l_width,        // local field width
                            const size_t        nb_local_inputs,
                            const size_t        nb_local_outputs,
                            const size_t        in_fifo_depth,
                            const size_t        out_fifo_depth,
                            const bool          is_cmd,
                            const bool          use_routing_table,
                            const bool          broadcast_supported );

        ~DspinLocalCrossbar();

	    private:

        // define the FIFO flit
        typedef struct internal_flit_s 
        {
            sc_uint<flit_width>  data;
            bool                 eop;
        } internal_flit_t;

	    // internal registers
	    sc_signal<bool>			        *r_alloc_out;  // output port allocated
	    sc_signal<size_t>               *r_index_out;  // owner input port index
        internal_flit_t                 *r_buf_in;     // input port fifo extension
        sc_signal<int>                  *r_fsm_in;     // input port state
	    sc_signal<size_t>               *r_index_in;   // requested output port index

	    // fifos
	    soclib::caba::GenericFifo<internal_flit_t>*  r_fifo_in;
	    soclib::caba::GenericFifo<internal_flit_t>*  r_fifo_out;

	    // structural parameters
	    const size_t                            m_local_x;
	    const size_t                            m_local_y;
	    const size_t                            m_x_width;
	    const size_t                            m_x_shift;
	    const size_t                            m_x_mask;
	    const size_t                            m_y_width;
	    const size_t                            m_y_shift;
	    const size_t                            m_y_mask;
	    const size_t                            m_l_width;
	    const size_t                            m_l_shift;
	    const size_t                            m_l_mask;
        const size_t                            m_local_inputs;
        const size_t                            m_local_outputs;
        const size_t                            m_addr_width;
        const bool                              m_is_cmd;
        const bool                              m_use_routing_table;
        const bool                              m_broadcast_supported;

        // we define two routing tables, but only one is used
        AddressDecodingTable<uint64_t, size_t>  m_cmd_rt;
        AddressDecodingTable<uint32_t, size_t>  m_rsp_rt;

	    // methods 
	    void      transition();
	    void      genMoore();
        size_t    route( sc_uint<flit_width> data, size_t index );
        bool      is_broadcast( sc_uint<flit_width> data );
	};

}} // end namespace
               
#endif // DSPIN_LOCAL_CROSSBAR_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
