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
           Alain Greiner <alain.greiner@lip6.fr> 2005 
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: alain
 */

///////////////////////////////////////////////////////////////////////////
// Implementation Note :
// This component is implemented as two independant combinational 
// crossbars, for VCI commands and VCI responses respectively.
// - The CMD crossbar has NI local plus one global input
// ports. It has NT local + one global output ports.
// - The RSP crossbar has NT local plus one global input
// ports. It has NI local + one global output ports.
// For each generic crossbar, the input and output ports are impemented
// as arrays of ports, and the last port (i.e. the largest index value)
// is the port connected to the global interconnect.
//
// This component does not contain FIFOs, and behaves as a Mealy FSM.
//
// It supportsi single flit VCI broadcast commands : If the
// two lsb bits of the VCI ADDRESS are non zero, the corresponding
// command is considered as a broadcast. 
// For a broadcast, the single VCI flit is SEQUENCIALLY transmitted 
// to the (NT+1) output ports, but not to the requesting input port.
// For each transmitted flit to a given output port, the standard 
// round-robin allocation policy is respected.
// As the broadcast command arriving on input port (i) should not be 
// transmitted to the requester, it is not transmitted on output port (i).
// Therefore, in case of broadcast, NI & NT must be equal, and all
// connected components mus have the same index for input & output ports.
///////////////////////////////////////////////////////////////////////////

#include <systemc>
#include <cassert>
#include "vci_buffers.h"
#include "../include/vci_local_crossbar.h"
#include "alloc_elems.h"

namespace soclib { namespace caba {

using soclib::common::alloc_elems;
using soclib::common::dealloc_elems;

using namespace sc_core;

////////////////////////////////////////////////
template<typename pkt_t> 
class SimpleCrossbar
////////////////////////////////////////////////
{   
    const size_t 		    m_cluster_id;   // cluster index
    const size_t 		    m_in_size;		// total number of inputs (local + global)
    const size_t 		    m_out_size;		// total number of outputs (local + global)
    const void*             m_rt;           // routing table if cmd  / masking table if rsp
    const void* 	        m_lt;           // locality table if cmd / id_locality table if rsp
    const bool              m_is_cmd;       // cmd crossbar when true

    sc_signal<bool>*		r_allocated;	// for each output port: allocation state 
    sc_signal<size_t>*		r_origin;		// for each output port: input port index
    sc_signal<bool>*		r_bc_state;		// for each input port: broadcast requested
    sc_signal<size_t>*		r_bc_count;		// for each input port: requested output index

public:
    ////////////////////////////////
    SimpleCrossbar( size_t  cluster_id,     // cluster index
                    size_t  in_size,        // number of inputs
                    size_t  out_size,       // number of outputs
	                void*   rt,             // routing table 
	                void*   lt,             // locality table
                    bool    is_cmd )
	: m_cluster_id( cluster_id ),
      m_in_size( in_size ),
	  m_out_size( out_size ),
	  m_rt( rt ),
	  m_lt( lt ),
      m_is_cmd( is_cmd )
	{
	    r_allocated = new sc_signal<bool>[out_size];
	    r_origin	= new sc_signal<size_t>[out_size];
        r_bc_state	= new sc_signal<bool>[in_size];
	    r_bc_count	= new sc_signal<size_t>[in_size];
	} // end constructor 

    ////////////
    void reset()
    {
	    for (size_t i=0; i<m_out_size; ++i) 
        {
	        r_origin[i]    = 0;
	        r_allocated[i] = false;
	    }
	    for (size_t i=0; i<m_in_size; ++i) 
        {
	        r_bc_state[i] = false;
	        r_bc_count[i] = 0;
	    }
    } // end reset()

    //////////////////
    void print_trace()
    {
        for( size_t out=0 ; out<m_out_size ; out++)
        {
            if( r_allocated[out].read() ) 
            {
                if( m_is_cmd ) std::cout << std::dec 
                                         << "initiator " << r_origin[out].read()
                                         << " =>  target " << out;
                else           std::cout << std::dec 
                                         << "target " << r_origin[out].read()
                                         << " => initiator " << out; 
            }
        }
        for ( size_t in=0 ; in<m_in_size ; in++)
        {
            if( r_bc_state[in].read() )
            {
                if( m_is_cmd ) std::cout << " broadcast request from initiator " << in 
                                         << " requesting target " 
                                         << r_bc_count[in].read();
                else           std::cout << " broadcast request from target " << in 
                                         << " requesting initiator " 
                                         << r_bc_count[in].read();
            }
        }
    } // end print_trace()

    //////////////////////////
    size_t route( pkt_t flit )
    {
        if( m_is_cmd )  // we use a 64 bits AddressDecodingTable for ADDRESS
        {
            soclib::common::AddressDecodingTable<uint64_t, size_t>* rt =
                   (soclib::common::AddressDecodingTable<uint64_t, size_t>*)m_rt;
            return rt->get_value( (uint64_t)(flit.dest()) );
        }
        else            // we use a 32 bits AddressDecodingTable for SRCID
        {
            soclib::common::AddressDecodingTable<uint32_t, size_t>* rt =
                   (soclib::common::AddressDecodingTable<uint32_t, size_t>*)m_rt;
            return rt->get_value( (uint32_t)(flit.dest()) );
        }  
    } // end route()

    ///////////////////////////
    bool is_local( pkt_t flit )
    {
        if( m_is_cmd )  // we use a 64 bits AddressDecoding Table for ADDRESS
        {
            soclib::common::AddressDecodingTable<uint64_t, bool>* lt =
                   (soclib::common::AddressDecodingTable<uint64_t, bool>*)m_lt;
            return lt->get_value( (uint64_t)(flit.dest()) );
        }
        else            // we use a 32 bits AddressDecodingTable for SRCID
        {
            soclib::common::AddressDecodingTable<uint32_t, bool>* lt =
                   (soclib::common::AddressDecodingTable<uint32_t, bool>*)m_lt;
            return lt->get_value( (uint32_t)(flit.dest()) );
        }
    } // end is_local()

    //////////////////////////////////////////////////////////////
    void transition( typename pkt_t::input_port_t   **input_port, 
                     typename pkt_t::output_port_t  **output_port )
    {
        // loop on the input ports to handle r_bc_state[in] and r_bc_count[in]
        for( size_t in = 0 ; in < m_in_size ; in++ )
        {
            if ( input_port[in]->getVal() )
            {
                if ( r_bc_state[in].read() )	// pending broadcast
                {
                    size_t out = r_bc_count[in];
                    if ( ( r_allocated[out].read() ) &&
                         ( r_origin[out].read() == in ) &&
                         ( output_port[out]->toPeerEnd() ) )	// successfully transmitted 
                    {
                        // the broadcast should not be sent to the requester...
                        if ( (out == 0) || ((out == 1) && (in == 0)) ) 	r_bc_state[in] = false;
                        else if ( (out-1) != in )	r_bc_count[in] = out-1;
                        else                        r_bc_count[in] = out-2;
                    }
                }
                else				// no pending proadcast
                {
                    pkt_t tmp;
                    tmp.readFrom(*input_port[in]);
                    if ( tmp.is_broadcast() )		// broadcast request
                    {
                        assert( input_port[in]->eop && 
                        "error in vci_local_crossbar : VCI broacast packet must be one flit");

                        r_bc_state[in] = true;
                        // the broadcast should not be sent to the requester...
                        if ( in == m_in_size-1 ) r_bc_count[in] = m_out_size-2;  
                        else                     r_bc_count[in] = m_out_size-1; 
                    }
                }
            }
        }

        // loop on the output ports to handle r_allocated[out] and r_origin[out]
        for ( size_t out = 0; out < m_out_size; out++) 
        {
            //  de-allocation if the last flit is accepted
            if ( r_allocated[out] ) 
            {
                if ( output_port[out]->toPeerEnd() )   r_allocated[out] = false;
            } 
            // allocation respecting round-robin priority (even for broadcast)
            else 
            {
                for(size_t _in = 0; _in < m_in_size; _in++) 
                {
                    size_t in = (_in + r_origin[out] + 1) % m_in_size;
                    if ( input_port[in]->getVal() )
                    {
                        pkt_t tmp;
                        tmp.readFrom(*input_port[in]);
                        if ( (tmp.is_broadcast() and 
                              r_bc_state[in].read() and 
                              (r_bc_count[in].read() == out))   or   // broadcast request
                             (not tmp.is_broadcast() and 
                              not is_local( tmp ) and 
                              (out == m_out_size-1))            or   // to global network
                             (not tmp.is_broadcast() and
                              is_local( tmp ) and
                              (out == route( tmp )) ) )              // to local component
                        {
                            r_allocated[out] = true;
                            r_origin[out] = in;
                            break;
                        }
                    }
                }
            }
        }
    } // end transition

    /////////////////////////////////////////////////////////////
    void genMealy( typename pkt_t::input_port_t   **input_port, 
                   typename pkt_t::output_port_t  **output_port )
    {
        bool ack[m_in_size];
        for( size_t in = 0; in < m_in_size; in++) ack[in] = false;

        // transmit flits on output ports
        for( size_t out = 0; out < m_out_size; out++) 
        {
            if (r_allocated[out]) 
            {
		        size_t in = r_origin[out];
                pkt_t tmp;
                tmp.readFrom(*input_port[in]);
                tmp.writeTo(*output_port[out]);
                ack[in] = output_port[out]->getAck();
                if ( r_bc_state[in].read() )			// its a broacast
                {
                    // in case of broadcast, the flit must be consumed only
                    // if it is the last output port ...
                    ack[in] = ack[in] && ( (out == 0) || ((out == 1) && (in == 0)) );
                }
            } 
            else 
            {
                output_port[out]->setVal( false );
            }
        }

        // Send acknowledges on input ports
        for( size_t in = 0; in < m_in_size; in++) input_port[in]->setAck( ack[in] );
    } // en genmealy

}; // end class SimpleCrossbar

#define tmpl(x) template<typename vci_param> x VciLocalCrossbar<vci_param>

/////////////////////////
tmpl(void)::print_trace()
{
    std::cout << "LOCAL_CROSSBAR " << name() << " / ";
    m_cmd_crossbar->print_trace();
    m_rsp_crossbar->print_trace(); 
    std::cout << std::endl;
}

////////////////////////
tmpl(void)::transition()
{
    if ( ! p_resetn.read() ) 
    {
        m_cmd_crossbar->reset();
        m_rsp_crossbar->reset();
        return;
    }

    m_cmd_crossbar->transition( m_ports_to_initiator, m_ports_to_target );
    m_rsp_crossbar->transition( m_ports_to_target, m_ports_to_initiator );
}

//////////////////////
tmpl(void)::genMealy()
{
    m_cmd_crossbar->genMealy( m_ports_to_initiator, m_ports_to_target );
    m_rsp_crossbar->genMealy( m_ports_to_target, m_ports_to_initiator );
}

///////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciLocalCrossbar( sc_core::sc_module_name            name,
                              const soclib::common::MappingTable &mt,
                              const size_t                       cluster_id,
	                          const size_t                       nb_attached_initiators,
	                          const size_t                       nb_attached_targets,
                              const size_t                       default_target_id )
       : BaseModule(name),
       p_clk("clk"),
       p_resetn("resetn"),
	   p_to_target(soclib::common::alloc_elems<VciInitiator<vci_param> >(
                           "to_target", nb_attached_targets)),
	   p_to_initiator(soclib::common::alloc_elems<VciTarget<vci_param> >(
                           "to_initiator", nb_attached_initiators)),
       p_target_to_up("target_to_up"),
       p_initiator_to_up("initiator_to_up"),
       m_nb_attached_initiators(nb_attached_initiators),
       m_nb_attached_targets(nb_attached_targets),
       m_cmd_rt ( mt.getLocalIndexFromAddress( cluster_id, default_target_id ) ),
       m_cmd_lt ( mt.getLocalMatchFromAddress( cluster_id ) ),
       m_rsp_rt ( mt.getLocalIndexFromSrcid( cluster_id ) ),
       m_rsp_lt ( mt.getLocalMatchFromSrcid( cluster_id ) )
{
    std::cout << "  - Building VciLocalCrossbar " << name << std::dec << std::endl
              << "    => cluster_id     = " << cluster_id << std::endl
              << "    => targets        = " << nb_attached_targets << std::endl
              << "    => initiators     = " << nb_attached_initiators << std::endl
              << "    => default target = " << default_target_id << std::endl;

    SC_METHOD(transition);
    dont_initialize();
    sensitive << p_clk.pos();

    SC_METHOD(genMealy);
    dont_initialize();
    sensitive << p_clk.neg();

	for ( size_t i=0; i<nb_attached_initiators; ++i )
		sensitive << p_to_initiator[i];
	for ( size_t i=0; i<nb_attached_targets; ++i )
		sensitive << p_to_target[i];

    sensitive << p_target_to_up
              << p_initiator_to_up;

    // building cmd and rsp crossbars
	m_cmd_crossbar = new SimpleCrossbar<VciCmdBuffer<vci_param> >(
                         cluster_id,
                         nb_attached_initiators+1, 
		                 nb_attached_targets+1,
                         (void*)(&m_cmd_rt),
                         (void*)(&m_cmd_lt),
                         true );

	m_rsp_crossbar = new SimpleCrossbar<VciRspBuffer<vci_param> >(
		                 cluster_id,
                         nb_attached_targets+1, 
		                 nb_attached_initiators+1,
                         (void*)(&m_rsp_rt),
                         (void*)(&m_rsp_lt),
                         false );

    m_ports_to_initiator = new VciTarget<vci_param>*[nb_attached_initiators+1];
    for (size_t i=0; i<nb_attached_initiators; ++i)
        m_ports_to_initiator[i] = &p_to_initiator[i];
    m_ports_to_initiator[nb_attached_initiators] = &p_target_to_up;

    m_ports_to_target = new VciInitiator<vci_param>*[nb_attached_targets+1];
    for (size_t i=0; i<nb_attached_targets; ++i)
        m_ports_to_target[i] = &p_to_target[i];
    m_ports_to_target[nb_attached_targets] = &p_initiator_to_up;
}

///////////////////////////////
tmpl(/**/)::~VciLocalCrossbar()
{
    soclib::common::dealloc_elems(p_to_initiator, m_nb_attached_initiators);
    soclib::common::dealloc_elems(p_to_target, m_nb_attached_targets);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
