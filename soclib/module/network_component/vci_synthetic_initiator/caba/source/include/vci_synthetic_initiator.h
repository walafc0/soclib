/* -*- c++ -*-
 * File         : vci_synthetic_initiator.h
 * Date         : march/2013
 * Copyright    : UPMC / LIP6
 * Authors      : Alain Greiner
 * Version	    : 1.0
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
 * Maintainers: alain.greiner@lip6.fr
 */

#ifndef SOCLIB_CABA_SYNTHETIC_INITIATOR_H
#define SOCLIB_CABA_SYNTHETIC_INITIATOR_H

#include <systemc>
#include "generic_fifo.h"
#include "vci_initiator.h"
#include "caba_base_module.h"

namespace soclib {  namespace caba {

    using namespace sc_core;

    template<typename vci_param>
    class VciSyntheticInitiator
      : public soclib::caba::BaseModule
    {

        typedef sc_dt::sc_uint<vci_param::N> addr_t;

        enum cmd_fsm_state_e
        {
	        VCI_IDLE,
	        VCI_READ,
	        VCI_WRITE,
        };

        enum pending_fsm_state_e
        {
	        PENDING_IDLE,
	        PENDING_READ,
	        PENDING_WRITE,
        };

    protected:

      SC_HAS_PROCESS(VciSyntheticInitiator);
    
    public:
        sc_in<bool> 			  	            p_clk;
        sc_in<bool> 			  	            p_resetn;
        soclib::caba::VciInitiator<vci_param> 	p_vci;	

        VciSyntheticInitiator(
		sc_module_name name,
		const uint32_t srcid,                    // VCI SRCID 
		const uint32_t length,                   // Packet length (flit numbers)
		const uint32_t load,                     // load * 1000
		const uint32_t fifo_depth );             // Fifo depth

        void transition();
        void genMoore();
        void print_stats();
        void print_trace();   

    private:

        // Component attributes
        const size_t		    m_srcid;
        const size_t            m_length;		 // Number of words for VCI transaction
        const size_t		    m_load;	         // offered load * 1000
        const size_t            m_pending_size;  // max number of simultaneous transactions

        // Fifo transmitting requests from the generator FSM to the VCI FSM
        GenericFifo<uint64_t>   r_date_fifo;

        // VCI CMD FSM
        sc_signal<int>          r_vci_fsm;
        sc_signal<uint64_t>		r_vci_address;   // Address for the single transaction
        sc_signal<size_t>		r_vci_trdid;	 // TRDID for the single transaction
        sc_signal<size_t>    	r_vci_count;     // Numbers of words sent

        // Pending transaction FSMs
        sc_signal<int>*		    r_pending_fsm;   // FSM states
        sc_signal<uint64_t>*	r_pending_date;  // single transaction requested date

        // Traffic generator 
        sc_signal<uint64_t>     r_nb_posted;     // Total number of posted transactions

        // Instrumentation registers
        sc_signal<uint64_t>	    r_cycles;        // Local time 
        sc_signal<uint64_t>    	r_nb_read;	     // Total number of VCI read
        sc_signal<uint64_t>    	r_nb_write;      // Total number of VCI write
        sc_signal<uint64_t>  	r_latency_read;  // Total cumulated read latency 
        sc_signal<uint64_t>  	r_latency_write; // Total cumulated write latency 

    }; // end class VciSyntheticInitiator
 
  }}

#endif
