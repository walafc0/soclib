/* -*- c++ -*-
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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors : Ivan MIRO PANADES
 * 
 * History :
 *
 * Comment :
 *
 */

#ifndef VCI_ANOC_WRAPPER_H_
#define VCI_ANOC_WRAPPER_H_

#include "anoc_common.h"
#include "anoc_transaction.h"
#include "anoc_in_port.h"
#include "anoc_out_port.h"
//#include "anoc_debug.h"

#include <systemc>
#include "caba_base_module.h"
#include "generic_fifo.h"
#include "vci_target.h"
#include "vci_initiator.h"
#include "mapping_table.h"

#define ANOC_EOP 1
#define ANOC_BOP 1

namespace soclib { namespace caba {

    using namespace sc_core;

    template<typename vci_param, int anoc_fifo_size, int anoc_yx_size>
        class VciAnocWrapper
        : public soclib::caba::BaseModule,
        // public tlm_module,
        public virtual tlm_blocking_put_if<anoc_data_transaction>,
        public virtual tlm_blocking_put_if<anoc_accept_transaction>
    {

        // FSM of request
        enum I_fsm_state_req{
            I_REQ_ANOC_HEADER,
            I_REQ_VCI_ADDRESS_HEADER,
            I_REQ_VCI_CMD_READ_HEADER,
            I_REQ_VCI_CMD_WRITE_HEADER,
            I_REQ_VCI_DATA_PAYLOAD,
        };
        // FSM of request
        enum T_fsm_state_req{
            T_REQ_ANOC_HEADER,
            T_REQ_VCI_ADDRESS_HEADER,
            T_REQ_VCI_CMD_HEADER,
            T_REQ_VCI_DATA_PAYLOAD,
            T_REQ_VCI_NOPAYLOAD,
        };

        // FSM of response
        enum I_fsm_state_rsp{
            I_RSP_ANOC_HEADER,
            I_RSP_VCI_HEADER,          
            I_RSP_VCI_DATA_PAYLOAD,
        };
        // FSM of response
        enum T_fsm_state_rsp{
            T_RSP_ANOC_HEADER,
            T_RSP_VCI_HEADER,
            T_RSP_VCI_DATA_PAYLOAD,
        };

        protected:
        SC_HAS_PROCESS(VciAnocWrapper);

        public:
        // ports
        sc_in<bool>                 p_clk;
        sc_in<bool>                 p_resetn;

        // ports vci
        soclib::caba::VciTarget<vci_param>     I_p_vci;
        soclib::caba::VciInitiator<vci_param>  T_p_vci;

        // noc input & output ports
        anoc_in_port*  noc_in;
        anoc_out_port* noc_out;

        // debug port
 //       anoc_debug_in_port *noc_debug_in;

        // constructor / destructor
        VciAnocWrapper(sc_module_name    insname,
                       const soclib::common::MappingTable &mt,
                       const size_t position_Y,
                       const size_t position_X); 


        private:
        // internal registers
        sc_signal<int>             I_r_fsm_state_req;
        sc_signal<int>             I_r_fsm_state_rsp;
        sc_signal<int>             I_r_srcid;
        sc_signal<int>             I_r_pktid;
        sc_signal<int>             I_r_trdid;
        sc_signal<int>             I_r_error;


        // internal registers
        sc_signal<int>                    T_r_fsm_state_req;
        sc_signal<int>                    T_r_fsm_state_rsp;
        sc_signal<sc_uint<2> >            T_r_cmd;
        sc_signal<sc_uint<vci_param::S> > T_r_srcid;


        sc_signal<sc_uint<vci_param::P> > T_r_pktid;
        sc_signal<sc_uint<vci_param::T> > T_r_trdid;
        sc_signal<bool >                  T_r_cons;
        sc_signal<bool >                  T_r_contig;

        sc_signal<sc_uint<vci_param::N> > T_r_address;
        sc_signal<sc_uint<vci_param::K> > T_r_plen;

        // SRCID extraction utility
        soclib::common::AddressMaskingTable<uint32_t> m_get_msb;


        // deux fifos req and rsp
        soclib::caba::GenericFifo<sc_uint<38> >  I_fifo_req;
        soclib::caba::GenericFifo<sc_uint<38> >  I_fifo_rsp;
        soclib::caba::GenericFifo<sc_uint<38> >  T_fifo_req;
        soclib::caba::GenericFifo<sc_uint<38> >  T_fifo_rsp;

        sc_uint<38> noc_data[2];
        bool        noc_data_valid[2];


        const int m_position_Y;
        const int m_position_X;
        bool anoc_accept[2];
        int req_channel_allocated;

        anoc_data_transaction trans_to_send;

        // methods systemc
        void transition();
        void genMoore();

        // routing table
        soclib::common::AddressDecodingTable<uint32_t, int>      m_routing_table;
        int     srcid_mask;

        // checker
        soclib_static_assert(vci_param::N == 32 || vci_param::N == 36); // checking VCI address size
        soclib_static_assert(vci_param::B == 4);   // checking VCI data size
        soclib_static_assert(anoc_fifo_size <= 256 && anoc_fifo_size >= 1); // checking FIFO size
        soclib_static_assert(anoc_yx_size <= 6 && anoc_yx_size >= 1);  // checking DSPIN index size

        // event
        sc_event write_accept_event[ANOC_NB_CHANNEL];

        public:
        // ANOC functions
        void ComputePathToTarget(anoc_dir*, sc_uint<anoc_yx_size*2>);

        // Receive DATA transaction (remote call)
        virtual void put(const anoc_data_transaction& transaction);

        // Receive ACCEPT transaction (remote call)
        virtual void put(const anoc_accept_transaction& transaction);

        private:

        // Write ACCEPT transaction on channel 0 (sc_method)
        virtual void write_accept_0();

        // Write ACCEPT transaction on channel 1 (sc_method)
        virtual void write_accept_1();

    };

}} // end namespace

#endif // VCI_ANOC_WRAPPER_H_

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
