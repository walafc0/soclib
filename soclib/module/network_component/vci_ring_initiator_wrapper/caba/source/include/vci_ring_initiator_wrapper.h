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
 * Authors  : Franck WAJSBÜRT, Abdelmalek SI MERABET 
 * Date     : september 2008
 * Copyright: UPMC - LIP6
 */

#ifndef VCI_RING_INITIATOR_WRAPPER_H_
#define VCI_RING_INITIATOR_WRAPPER_H_

#include <systemc.h>
#include "caba_base_module.h"
#include "ring_ports.h"
#include "vci_target.h"
#include "generic_fifo.h"
#include "mapping_table.h"

namespace soclib { namespace caba {

using namespace sc_core;

template<typename vci_param>
class VciRingInitiatorWrapper 
	: public soclib::caba::BaseModule
{

protected:
	SC_HAS_PROCESS(VciRingInitiatorWrapper);

public:
    // ports
    sc_in<bool> 	                	p_clk;
    sc_in<bool> 	                	p_resetn;
    soclib::caba::RingIn                        p_ring_in;
    soclib::caba::RingOut                       p_ring_out;
    soclib::caba::VciTarget<vci_param>          p_vci;

    // constructor
    VciRingInitiatorWrapper(sc_module_name	insname,
                            bool            alloc_init,
                            const int       &wrapper_fifo_depth,
                            const soclib::common::MappingTable &mt,
                            const soclib::common::IntTab &ringid,
                            const uint32_t &srcid);
    // charge reseau
    void print_charge();
    uint32_t get_tot_cmd_flits();
    uint32_t get_tot_cycles();
   
private:
    enum vci_cmd_fsm_state_e {
        CMD_FIRST_HEADER,     // first  flit for a ring cmd packet (read or write)
        CMD_SECOND_HEADER,   //  second flit for a ring cmd packet 
        WDATA,              //   data flit for a ring cmd write packet
    };

    enum vci_rsp_fsm_state_e {
        RSP_HEADER,     // first flit for a ring rsp packet (read or write)
        RSP_DATA,       // next  flit for a ring rsp packet
        
    };

    enum ring_rsp_fsm_state_e {
		RSP_IDLE,    // waiting for first flit of a response packet
		LOCAL,      // next flit of a local rsp packet
		RING,  	    // next flit of a ring rsp packet
	};

    // cmd token allocation fsm
    enum ring_cmd_fsm_state_e {
		CMD_IDLE,	    
         	DEFAULT,  	
		KEEP,          	    
	};

    // structural parameters
    bool                m_alloc_init;
    uint32_t            m_srcid;

 
    // internal registers
    sc_signal<int>	    r_ring_cmd_fsm;    // ring command packet FSM (distributed)
    sc_signal<int>	    r_ring_rsp_fsm;    // ring response packet FSM
    sc_signal<int>	    r_vci_cmd_fsm;    // vci command packet FSM
    sc_signal<int>	    r_vci_rsp_fsm;    // vci response packet FSM

    sc_signal<sc_uint<vci_param::S> >      r_srcid_save;
    sc_signal<sc_uint<vci_param::T> >      r_trdid_save;
    sc_signal<sc_uint<vci_param::P> >      r_pktid_save;
    sc_signal<sc_uint<vci_param::E> >      r_error_save;
   
    // internal fifos 
    GenericFifo<sc_uint<37> > m_cmd_fifo;     // fifo for the local command packet
    GenericFifo<sc_uint<33> > m_rsp_fifo;     // fifo for the local response packet

    // routing table
    soclib::common::AddressMaskingTable<uint32_t> m_rt;
    soclib::common::AddressDecodingTable<uint32_t, bool> m_lt;
    
    // methods
    void transition();
    void genMoore();
    void genMealy_vci_cmd();
    void genMealy_rsp_out();
    void genMealy_rsp_in();
    void genMealy_cmd_out();
    void genMealy_cmd_in();
    void genMealy_cmd_grant();
    void genMealy_rsp_grant();

};

}} // end namespace
		
#endif // VCI_RING_INITIATOR_WRAPPER_H_


