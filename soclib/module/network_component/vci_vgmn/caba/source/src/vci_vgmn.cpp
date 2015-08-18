/*
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
 * Copyright (c) UPMC, Lip6, Asim
 *         Alain Greiner <alain.greiner@lip6.fr> 2005 
 *         Nicolas Pouillon <nipo@ssji.net> 2007-2009
 *         Alain Greiner <alain.greiner@lip6.fr> 2013
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////////////////
// Implementation note:
// The VciVgmn component contains two independant micro-network for CMD & RSP.
// Each micro-network is composed of a variable number of "input ports"
// and a variable number of "output ports".
// - Each Input Port contains an input generic fifo, a routing function, 
//   and a dedicated FSM to handle (sequencial) broadcast.
// - Each Output Port contains several intermediate fifos (one per input port),
//   an allocation FSM, and an output fifo acting as a delay line.
// The two micro-networks have three template parameters:
// - CMD network: vci_flit_t   == VciCmdBuffer
//                vci_input_t  == VciTarget
//                vci_output_t == VciInitiator
// - RSP network: vci_flit_t   == VciRspBuffer
//                vci_input_t  == VciInitiator
//                vci_output_t == VciTarget   
/////////////////////////////////////////////////////////////////////////////////

#include <systemc>
#include <vector>
#include <cassert>
#include "../include/vci_vgmn.h"
#include "alloc_elems.h"

namespace soclib { namespace caba {

using namespace sc_core;
using namespace soclib::common;

////////////////////////////////////////////
template<typename data_t> class DelayLine
////////////////////////////////////////////
{
    data_t*  r_data;
    size_t   r_ptr;
    size_t   m_size;
    data_t   m_default;     // invalid data value 

public:

    ///////////
    DelayLine()
    {}

    /////////////////////////////
    void init( size_t line_depth, 
               data_t default_data )
    {
        m_size    = line_depth;
        m_default = default_data;
        r_ptr     = 0;
        r_data    = new data_t[line_depth];
        for ( size_t i=0 ; i<line_depth ; i++ ) r_data[i] = default_data;
    }

    //////////// 
    ~DelayLine()
    {
        delete [] r_data;
    }

    //////////////////////////////////////////
    inline data_t shift( const data_t &input )
    {
        data_t tmp    = r_data[r_ptr];
        r_data[r_ptr] = input;
	    if (++r_ptr == m_size) r_ptr = 0;
        return tmp;
    }

    /////////////////////
    inline data_t head( )
    {
        return r_data[r_ptr];
    }

    ////////////
    void reset()
    {
        for ( size_t i=0; i<m_size; i++ ) r_data[i] = m_default;
        r_ptr = 0;
    }
};   // end class DelayLine

////////////////////////////////////////////////
template<typename data_t> class AdHocFifo
////////////////////////////////////////////////
{
    size_t    m_size;          // number of slots
    data_t*   m_data;          // data array
    size_t    m_rptr;          // read pointer
    size_t    m_wptr;          // write pointer
    size_t    m_usage;         // number of valid flits
    size_t*   m_global_flits;  // pointer on a variable containing the total number 
                               // flits in all mid FIFOs in the same output module
public:

    ///////////
    AdHocFifo()
    {}

    /////////////////////////////
    void init( size_t fifo_size,
               size_t *global_flits )
    {
        m_data         = new data_t[fifo_size];
        m_size         = fifo_size;
        m_rptr         = 0;
        m_wptr         = 0;
        m_usage        = 0;
        m_global_flits = global_flits;
    }

    ////////////
    ~AdHocFifo()
    {
        delete [] m_data;
    }

    ////////////
    void reset()
    {
        m_rptr = 0;
        m_wptr = 0;
        m_usage = 0;
    }

    ///////////////////
    inline data_t pop()
    {
        assert( not empty() );
        data_t tmp = m_data[m_rptr];
        --m_usage;
        --(*m_global_flits);
        // do {m_rptr = (m_rptr+1)%m_size;} without using %
	    if (++m_rptr == m_size) m_rptr = 0;
        return tmp;
    }

    /////////////////////////////////////
    inline void push( const data_t data )
    {
        assert( not full() );
        ++m_usage;
        ++(*m_global_flits);
        m_data[m_wptr] = data;
        // do {m_wptr = (m_wptr+1)%m_size;} without using %
	    if (++m_wptr == m_size) m_wptr = 0;
    }

    /////////////////////////
    inline bool empty() const
    {
        return m_usage == 0;
    }

    ////////////////////////
    inline bool full() const
    {
        return m_usage == m_size;
    }
};   // end class AdHocFifo

///////////////////////////////////////////////////////////////////////////
// An output module is associated to each output port of a micro-network 
// Each output module implements a separate allocation mechanism, and
// contains as many intermediate FIFOs as the number of input ports, 
// and one single output fifo acting as a delay line.
// For a CMD micro-network:  vci_flit_t   == VciCmdBuffer
//                           vci_output_t == VciInitiator
// For a RSP micro-network:  vci_flit_t   == VciRspBuffer
//                           vci_output_t == VciTarget
///////////////////////////////////////////////////////////////////////////
template<typename vci_flit_t,
         typename vci_output_t> class OutputModule
///////////////////////////////////////////////////////////////////////////
{
    AdHocFifo<vci_flit_t>*   r_mid_fifos;          // array of FIFOs competing for output 
    DelayLine<vci_flit_t>    r_out_delay_line;     // output delay line
    size_t                   r_n_inputs;           // number of mid FIFOs
    size_t                   r_total_flits;        // total number of flits in all FIFOs
    bool                     r_allocated;          // output port allocated
    size_t                   r_current_input;      // currently selected mid FIFO

public:

    /////////////////
    OutputModule()
    {}
    
    ///////////////////////////
    void init( size_t n_inputs, 
               size_t min_delay, 
               size_t fifo_size )
    {
        r_n_inputs      = n_inputs;
        r_current_input = 0;
        r_allocated     = false;

        vci_flit_t default_flit;
        default_flit.set_val( false );
        r_out_delay_line.init( min_delay, default_flit );

        r_mid_fifos = new AdHocFifo<vci_flit_t>[n_inputs];
        for ( size_t i=0; i<n_inputs; ++i ) 
        {
            r_mid_fifos[i].init( fifo_size, &r_total_flits );
        }
    }

    /////////////
    ~OutputModule()
    {
        delete [] r_mid_fifos;
    }


    ////////////
    void reset()
    {
        for ( size_t i=0; i<r_n_inputs; ++i ) r_mid_fifos[i].reset();
        r_out_delay_line.reset();
            r_current_input = 0;
            r_allocated     = false;
            r_total_flits   = 0;
        }

        ////////////////////////////////////////////////
        inline AdHocFifo<vci_flit_t>* getFifo( size_t n )
    {
        return &r_mid_fifos[n];
    }
    
    ///////////////////////////////
    void print_trace(size_t output)
    {
        if ( r_allocated )
            std::cout << std::dec << "in[" << r_current_input 
                      << "] => out[" << output << "] / ";
    }

    ////////////////////////////////////////////
    void transition( const vci_output_t  &port )
    {
        vci_flit_t out_flit;  // output flit from the delay line

        // frozen if a valid flit from delay line is not accepted on output port
        if( r_out_delay_line.head().val() and not port.peerAccepted() )   return;

        vci_flit_t in_flit;  // output flit from the selected input fifo

        // default value for the flit to be writen in delay line
        in_flit.set_val( false );

        if ( r_allocated ) // output port allocated : possible flit transfer
        {
            if (  not r_mid_fifos[r_current_input].empty() )
            {
                in_flit     = r_mid_fifos[r_current_input].pop();
                r_allocated = not (in_flit.eop() and in_flit.val() );
            }
        } 
        else if (r_total_flits > 0)  // output not allocated and flit available 
        {
            // round-robin allocation, but no flit transfer
	        size_t prio = r_current_input + 1;
	        if (prio == r_n_inputs) prio = 0;

	        size_t i = prio;
	        do 
            {
                if ( not r_mid_fifos[i].empty() ) 
                {
                    r_current_input = i;
                    r_allocated = true;;
                    break;
                }
		        if (++i == r_n_inputs) i = 0;
            } while (i != prio);
        }

        // systematic shift of delay line
        r_out_delay_line.shift( in_flit );
    }

    ////////////////////////////////////
    void genMoore( vci_output_t &port )
    {
        r_out_delay_line.head().writeTo( port );
    }
};    // end class OutputModule

//////////////////////////////////////////////////////////////////////////
// An input module is associated to each input port of a micro-network.
// It decode the VCI ADDRESS or the VCI RSRCID to select the requested 
// output module, and push the packet in the proper mid FIFO.
// For a CMD micro-network:  vci_flit_t  == VciCmdBuffer
//                           vci_input_t == VciTarget
// For a RSP micro-network:  vci_flit_t  == VciRspBuffer
//                           vci_input_t == VciInitiator
//////////////////////////////////////////////////////////////////////////
template<typename vci_flit_t,
         typename vci_input_t> class InputModule
////////////////////////////////////////////////////////////////////////
{
    bool                       r_is_cmd;        // micro-network type (CMD/RSP)
    size_t                     r_n_outputs;     // number of reachable output ports
    GenericFifo<vci_flit_t>*   r_input_fifo;    // input FIFO
    AdHocFifo<vci_flit_t>**    r_mid_fifos;     // array of pointers on the mid FIFOS 
    AdHocFifo<vci_flit_t>*     r_dest;          // pointer on the selected mid FIFO
    void*                      r_rt;            // pointer on routing/masking table
    bool*                      r_bc_waiting;    // array of bool (one per output port) 

public:

    //////////////////////
    InputModule()
    {
    }

    ////////////
    ~InputModule()
    {
        delete [] r_mid_fifos; 
        delete [] r_bc_waiting;
    }

    /////////////////////////////////////////////
    void init( size_t                     index,        // input port index
               size_t                     n_outputs,
               AdHocFifo<vci_flit_t>**    mid_fifos,
               void*                      rt,
               bool                       is_cmd )
    {
        std::ostringstream in_fifo_name;
        if( is_cmd ) in_fifo_name << "input_fifo_cmd_" << index;
        else         in_fifo_name << "input_fifo_rsp_" << index;

        r_is_cmd         = is_cmd;
        r_input_fifo     = new GenericFifo<vci_flit_t>(in_fifo_name.str(), 2);
        r_mid_fifos      = new AdHocFifo<vci_flit_t>*[n_outputs];
        r_bc_waiting     = new bool[n_outputs];
        r_rt             = rt;
        r_n_outputs      = n_outputs;
        r_dest           = NULL;

        for ( size_t i=0; i<n_outputs; ++i ) 
        {
            r_mid_fifos[i]  = mid_fifos[i];
            r_bc_waiting[i] = false;
        }
    }

    ////////////
    void reset()
    {
        for ( size_t i=0; i<r_n_outputs; ++i ) r_bc_waiting[i] = false;
        r_input_fifo->init();
        r_dest = NULL;
    }

    ///////////////////////////////
    size_t route( vci_flit_t flit )
    {
        size_t out;
        if( r_is_cmd )
        {
            AddressDecodingTable<uint64_t, size_t>* rt =
                (AddressDecodingTable<uint64_t, size_t>*)r_rt;
            out = rt->get_value( (uint64_t)flit.dest() );
        }
        else
        {
            AddressDecodingTable<uint32_t, size_t>* rt =
                (AddressDecodingTable<uint32_t, size_t>*)r_rt;
            out = rt->get_value( (uint32_t)flit.dest() );
        }  
        return out;
    }

    //////////////////////////////////////
    void transition( vci_input_t   &port )
    {
        // default value for input FIFO update
        vci_flit_t   in_fifo_wdata;
        vci_flit_t   in_fifo_rdata;
        bool         in_fifo_put = false;
        bool         in_fifo_get = false;

        // writing into input FIFO if possible
        if( port.getVal() ) // flit available
        {
            in_fifo_wdata.readFrom( port );
            in_fifo_put = true;
        }
        // consuming from input FIFO if possible 
        if ( r_input_fifo->rok() )                   // Flit available
        {
            in_fifo_rdata = r_input_fifo->read();

            // testing pending broadcast
            size_t next = r_n_outputs;
            for ( size_t j=0; j<r_n_outputs; ++j )
            {
                if ( r_bc_waiting[j] )
                {
                    next = j;
                    break;
                }
            }

            if ( next < r_n_outputs )                // pending broadcast
            {
                r_dest = r_mid_fifos[next];
                if( not r_dest->full() )      // transfer one flit if possible
                {
                    r_dest->push( in_fifo_rdata );
                    r_bc_waiting[next] = false;
                    if( next == r_n_outputs-1 ) 
                    {
                        r_dest      = NULL; 
                        in_fifo_get = true;
                    }
                }
            }
            else                                    // no pending broadcast
            {
                if( r_dest == NULL )      // new packet => routing required
                {
                    if( in_fifo_rdata.is_broadcast() ) // broadcast : transfer one flit
                    {
                        assert( in_fifo_rdata.eop() );
                        for( size_t j=0 ; j<r_n_outputs ; ++j ) r_bc_waiting[j] = true;
                        r_dest = r_mid_fifos[0];
                        if( not r_dest->full() )    // transfer one flit if possible
                        {
                            r_dest->push( in_fifo_rdata );
                            r_bc_waiting[0] = false;
                            if( r_n_outputs == 1 )
                            {
                                r_dest      = NULL; 
                                in_fifo_get = true;
                            }
                        }
                    }
                    else                 // not broadcast : route and transfer one flit
                    {
                        size_t dest = route( in_fifo_rdata );
                        r_dest = r_mid_fifos[dest];
                        if( not r_dest->full() )    // transfer one flit if possible
                        {
                            r_dest->push( in_fifo_rdata );
                            in_fifo_get = true;
                            if ( in_fifo_rdata.eop() ) r_dest = NULL;
                        }
                    }
                }
                else                      // not a new packet
                {
                    if( not r_dest->full() )    // transfer one flit if possible
                    {
                        r_dest->push( in_fifo_rdata );
                        in_fifo_get = true;
                        if ( in_fifo_rdata.eop() ) r_dest = NULL;
                    }
                }
            }
        }

        // update input FIFO
        r_input_fifo->update( in_fifo_get, in_fifo_put, in_fifo_wdata );

    }  // end transition

    ///////////////////////////////////
    void genMoore( vci_input_t &port )
    {
        port.setAck( r_input_fifo->wok() );
    }
};    // end class InputModule

/////////////////////////////////////////////////////////////////////////////////
template<typename vci_flit_t,   
         typename vci_input_t,
         typename vci_output_t> class VgmnMicroNetwork
/////////////////////////////////////////////////////////////////////////////////
{
    const size_t                             m_in_size;
    const size_t                             m_out_size;
    OutputModule<vci_flit_t, vci_output_t>*  m_outputs;      // array of output modules
    InputModule<vci_flit_t, vci_input_t>*    m_inputs;       // array of input modules
    const bool                               m_is_cmd;       // CMD micro-network if true

public:

    /////////////////////////////////
    VgmnMicroNetwork( size_t in_size,         // number of input ports
                      size_t out_size,        // number of output ports
                      size_t min_latency, 
                      size_t fifo_size,
                      void*  rt,              // routing table or masking table
                      bool   is_cmd )         // depending on is_cmd
    :   m_in_size(in_size),
        m_out_size(out_size),
        m_is_cmd( is_cmd )
    {
        // array of pointers on all the mid FIFOS targeted by a single input port
        AdHocFifo<vci_flit_t>*     fifos[out_size];

        // building input modules
        m_inputs  = new InputModule<vci_flit_t, vci_input_t>[m_in_size];

        // building output modules
        m_outputs = new OutputModule<vci_flit_t, vci_output_t>[m_out_size];

        for ( size_t j=0; j<out_size; ++j )
        {
            m_outputs[j].init(in_size, min_latency, fifo_size);
        }
        for ( size_t i=0; i<in_size; ++i ) 
        {
            for ( size_t j=0; j<out_size; ++j ) 
            {
                fifos[j] = m_outputs[j].getFifo(i);
            }
            m_inputs[i].init( i, out_size, &fifos[0], rt, is_cmd );
        }
    }

    ///////////////////
    ~VgmnMicroNetwork()
    {
        delete [] m_inputs;
        delete [] m_outputs;
    }

    ////////////
    void reset()
    {
        for ( size_t i=0; i<m_in_size; ++i )  m_inputs[i].reset();
        for ( size_t j=0; j<m_out_size; ++j ) m_outputs[j].reset();
    }

    //////////////////
    void print_trace()
    {
        for ( size_t j=0; j<m_out_size; ++j )
            m_outputs[j].print_trace(j);
    }

    /////////////////////////////////////////
    void transition( vci_input_t *input_port, 
                     vci_output_t *output_port )
    {
        for ( size_t i=0; i<m_in_size; ++i )  m_inputs[i].transition( input_port[i] );
        for ( size_t i=0; i<m_out_size; ++i ) m_outputs[i].transition( output_port[i] );
    }

    //////////////////////////////////////
    void genMoore( vci_input_t *input_port, 
                   vci_output_t *output_port )
    {
        for ( size_t i=0; i<m_in_size; ++i )  m_inputs[i].genMoore( input_port[i] );
        for ( size_t i=0; i<m_out_size; ++i ) m_outputs[i].genMoore( output_port[i] );
    }
};   // end class VgmnMicroNetwork

////////////////////////////////////////////////////////////////////////
// Methods for VciVgmn 
////////////////////////////////////////////////////////////////////////

#define tmpl(x) template<typename vci_param> x VciVgmn<vci_param>

////////////////////////
tmpl(void)::transition()
{
    if ( ! p_resetn.read() ) 
    {
        m_cmd_mn->reset();
        m_rsp_mn->reset();
        return;
    }

    m_cmd_mn->transition( p_to_initiator, p_to_target );
    m_rsp_mn->transition( p_to_target, p_to_initiator );
}

//////////////////////
tmpl(void)::genMoore()
{
    m_cmd_mn->genMoore( p_to_initiator, p_to_target );
    m_rsp_mn->genMoore( p_to_target, p_to_initiator );
}

/////////////////////////
tmpl(void)::print_trace()
{
    std::cout << "VGMN " << name() << std::endl;
    std::cout << "  CMD network : ";
    m_cmd_mn->print_trace();
    std::cout << std::endl;
    std::cout << "  RSP network : ";
    m_rsp_mn->print_trace();
    std::cout << std::endl;
}

//////////////////////////////////////////////////////////////
tmpl(/**/)::VciVgmn( sc_module_name                     name,
                     const soclib::common::MappingTable &mt,
                     size_t                             nb_attached_initiators,
                     size_t                             nb_attached_targets,
                     size_t                             min_latency,
                     size_t                             fifo_depth,
                     const size_t                       default_index)
   : soclib::caba::BaseModule(name),
     m_nb_initiators( nb_attached_initiators ),
     m_nb_targets( nb_attached_targets ),
     m_cmd_rt( mt.getGlobalIndexFromAddress( default_index ) ),
     m_rsp_rt( mt.getGlobalIndexFromSrcid() )
{
    std::cout << "  - Building VciVgmn : " << name << std::dec << std::endl
              << "    => targets        = "  << nb_attached_targets << std::endl
              << "    => initiators     = "  << nb_attached_initiators << std::endl
              << "    => default target = "  << default_index << std::endl;

    assert( (min_latency > 2) and
    "VCI_VGMN error : min_latency cannot be smaller than 3 cycles");

    assert( (fifo_depth  > 1) and
    "VCI_VGMN error : fifo_depth cannot be  smaller than 2 slots");

    p_to_initiator = soclib::common::alloc_elems<soclib::caba::VciTarget<vci_param> >(
                     "to_initiator", nb_attached_initiators);
    p_to_target = soclib::common::alloc_elems<soclib::caba::VciInitiator<vci_param> >(
                     "to_target", nb_attached_targets);

    // build  cmd network and rsp network 

    m_cmd_mn = new VgmnMicroNetwork<VciCmdBuffer<vci_param>,
                                    VciTarget<vci_param>,
                                    VciInitiator<vci_param> >( nb_attached_initiators, 
                                                               nb_attached_targets,
                                                               min_latency-2, 
                                                               fifo_depth,
                                                               (void*)(&m_cmd_rt),
                                                               true );
    m_rsp_mn = new VgmnMicroNetwork<VciRspBuffer<vci_param>, 
                                    VciInitiator<vci_param>,
                                    VciTarget<vci_param> >( nb_attached_targets, 
                                                            nb_attached_initiators,
                                                            min_latency-2, 
                                                            fifo_depth,
                                                            (void*)(&m_rsp_rt),
                                                            false );
    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMoore);
    dont_initialize();
    sensitive << p_clk.neg();
}

//////////////////////
tmpl(/**/)::~VciVgmn()
{
    delete m_rsp_mn;
    delete m_cmd_mn;
    soclib::common::dealloc_elems( p_to_initiator, m_nb_initiators );
    soclib::common::dealloc_elems( p_to_target, m_nb_targets );
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
