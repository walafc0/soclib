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
 * Author   : Abdelmalek SI MERABET 
 * Date     : March 2010
 * Copyright: UPMC - LIP6
 */
#ifndef VCI_SIMPLE_RING_FAST_H_
#define VCI_SIMPLE_RING_FAST_H_

#include <systemc>
#include "caba_base_module.h"
#include "mapping_table.h"
#include "address_decoding_table.h"
#include "address_masking_table.h"
#include "ring_signals_fast.h"
#include "vci_simple_ring_initiator_fast.h"
#include "vci_simple_ring_target_fast.h"


namespace soclib { namespace caba {

using namespace sc_core;

template<typename vci_param, int ring_cmd_data_size, int ring_rsp_data_size>
class VciSimpleRingFast
        : public soclib::caba::BaseModule
{


        public:
                sc_in<bool>             p_clk;
                sc_in<bool>             p_resetn;
                
                soclib::caba::VciInitiator<vci_param> * p_to_target;
                soclib::caba::VciTarget<vci_param>   * p_to_initiator;
                
        protected:
                SC_HAS_PROCESS(VciSimpleRingFast);

        private:                
                int m_ns;  // number of ring signals
                int m_nai; // number of attached initiators 
                int m_nat; // number of attached targets

                
                cmd_str *init_cmd;
                rsp_str *tgt_rsp;
               	//bool *init_cmd_val; // valid command sent
		bool *tgt_cmd_val;  // valid command received
		bool *init_rsp_val; // valid response received
		//bool *tgt_rsp_val;  // valid response sent
                
//--
                typedef SimpleRingSignals ring_signal_t;
                typedef VciSimpleRingInitiatorFast<vci_param, ring_cmd_data_size, ring_rsp_data_size> ring_initiator_t;
                typedef VciSimpleRingTargetFast<vci_param, ring_cmd_data_size, ring_rsp_data_size>    ring_target_t;

                void transition();
                void genMoore();

                ring_signal_t    *m_ring_signal; 
                ring_initiator_t **m_ring_initiator;
                ring_target_t    **m_ring_target;

        public:

                VciSimpleRingFast(   sc_module_name insname,
                                        const soclib::common::MappingTable &mt,
                                        const soclib::common::IntTab &ringid,   
                                        const int &wrapper_fifo_depth,
                                        int nb_attached_initiator,
                                        int nb_attached_target);
                                                                   
                ~VciSimpleRingFast();
                void print_trace();
};
}} // end namespace

#endif //VCI_SIMPLE_RING_FAST_H_
