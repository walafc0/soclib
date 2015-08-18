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

#include "../include/vci_anoc_wrapper.h"
#include "register.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, int anoc_fifo_size, int anoc_yx_size> x VciAnocWrapper<vci_param, anoc_fifo_size, anoc_yx_size>

    ////////////////////////////////
    //      constructor
    ////////////////////////////////

    tmpl(/**/)::VciAnocWrapper(sc_module_name insname,
                               const soclib::common::MappingTable &mt,
                               const size_t position_Y,
                               const size_t position_X) 
        : soclib::caba::BaseModule(insname),
        I_p_vci("I_vci"),
        T_p_vci("T_vci"),
        I_r_fsm_state_req("I_r_fsm_state_req"),
        I_r_fsm_state_rsp("I_r_fsm_state_rsp"),
        I_r_srcid("I_r_srcid"),
        I_r_pktid("I_r_pktid"),
        I_r_trdid("I_r_trdid"),
        I_r_error("I_r_error"),
        T_r_fsm_state_req("T_r_fsm_state_req"),
        T_r_fsm_state_rsp("T_r_fsm_state_rsp"),
        T_r_cmd("T_r_cmd"),
        T_r_srcid("T_r_srcid"),
        T_r_pktid("T_r_pktid"),
        T_r_trdid("T_r_trdid"),
        T_r_cons("T_r_cons"),
        T_r_contig("T_r_contig"),
        T_r_address("T_r_address"),
        T_r_plen("T_r_plen"),
        m_get_msb(mt.getIdMaskingTable(0)),
        I_fifo_req("I_FIFO_REQ", anoc_fifo_size),
        I_fifo_rsp("I_FIFO_RSP", anoc_fifo_size),
        T_fifo_req("T_FIFO_REQ", anoc_fifo_size),
        T_fifo_rsp("T_FIFO_RSP", anoc_fifo_size),
        m_position_Y(position_Y),
        m_position_X(position_X)
    {

        m_routing_table = mt.getRoutingTable(soclib::common::IntTab(), 0);
        srcid_mask = 0x7FFFFFFF >> ( 31 - vci_param::S );

        anoc_accept[0] = true;
        anoc_accept[1] = true;

        noc_data_valid[0] = false;
        noc_data_valid[1] = false;

        req_channel_allocated = 0;

        // create noc ports
#ifdef TLM_TRANS_RECORD
        char port_in[80];
        char port_out[80];

        sprintf(port_in,"%s_IN", basename());
        sprintf(port_out,"%s_OUT", basename());
        noc_in  = new anoc_in_port("res_in_port", (char *)port_in);
        noc_out = new anoc_out_port("res_out_port", (char *)port_out);
#else
        noc_in  = new anoc_in_port("res_in_port");
        noc_out = new anoc_out_port("res_out_port");
#endif // TLM_TRANS_RECORD

        // bind input/output ports to the ressource module
        noc_in->slave_bind(*this);        // for the data target sub-port
        noc_out->slave_bind(*this);       // for the accept target sub-port

        SC_METHOD (transition);
        dont_initialize();
        sensitive << p_clk.pos();
        SC_METHOD (genMoore);
        dont_initialize();
        sensitive  << p_clk.neg();
        // methods for automatic accept return
        SC_METHOD(write_accept_0);
        sensitive << write_accept_event[0];
        dont_initialize();
        SC_METHOD(write_accept_1);
        sensitive << write_accept_event[1];
        dont_initialize();

    } //  end constructor

    ////////////////////////////////
    //      transition
    ////////////////////////////////
    tmpl(void)::transition()
    {
        sc_uint<38>	I_req_fifo_data;
        bool		I_req_fifo_write;
        bool		I_req_fifo_read;
        sc_uint<38>	I_rsp_fifo_data;
        bool		I_rsp_fifo_write;
        bool		I_rsp_fifo_read;

        sc_uint<38>	T_req_fifo_data;
        bool		T_req_fifo_write;
        bool		T_req_fifo_read;
        sc_uint<38>	T_rsp_fifo_data;
        bool		T_rsp_fifo_write;
        bool		T_rsp_fifo_read;


        if (p_resetn == false) {
            I_fifo_req.init();
            I_fifo_rsp.init();
            I_r_fsm_state_req = I_REQ_ANOC_HEADER;
            I_r_fsm_state_rsp = I_RSP_ANOC_HEADER;
            T_fifo_req.init();
            T_fifo_rsp.init();
            T_r_fsm_state_req = T_REQ_ANOC_HEADER;
            T_r_fsm_state_rsp = T_RSP_ANOC_HEADER;

            return;
        } // end reset

        // VCI request to ANOC request
        // The VCI packet is analysed, translated,
        // and the ANOC packet is stored in the I_fifo_req


        ////////////////////////////////
        //Analyse the output of the ANOC
        //Store it into a FIFO
        ////////////////////////////////

        T_req_fifo_write = noc_data_valid[0];
        T_req_fifo_data  = noc_data[0];
        I_rsp_fifo_write = noc_data_valid[1];
        I_rsp_fifo_data  = noc_data[1];

        if ((noc_data_valid[0] == true) && (T_fifo_req.wok() == true) ) {
            write_accept_0();
            noc_data_valid[0] = false;
        }
        if ((noc_data_valid[1] == true) && (I_fifo_rsp.wok() == true)){
            write_accept_1();
            noc_data_valid[1] = false;
        }


        //////////////////////////////
        // VCI Initiator state machine
        //////////////////////////////

        // I_r_fsm_state_req, I_req_fifo_write and I_req_fifo_data
        I_req_fifo_write = false;
        switch(I_r_fsm_state_req) {
            case I_REQ_ANOC_HEADER :
                if(I_p_vci.cmdval == true && I_fifo_req.wok() == true) {
                    I_req_fifo_write = true;
                    I_req_fifo_data = ((sc_uint<38>)ANOC_BOP << 37) |
                                      ((sc_uint<38>(m_routing_table[(int)(I_p_vci.address.read())]))
                                         & (0x7FFFFFFF >> (31 - anoc_yx_size * 2)));
                    I_r_fsm_state_req = I_REQ_VCI_ADDRESS_HEADER;
                }
                break;
            case I_REQ_VCI_ADDRESS_HEADER :
                if((I_p_vci.cmdval == true) && (I_fifo_req.wok() == true)) {
                    I_req_fifo_write = true;
                    I_req_fifo_data = (sc_uint<38>) I_p_vci.address.read();
                    if((I_p_vci.cmd.read() == vci_param::CMD_WRITE) || 
                       (I_p_vci.cmd.read() == vci_param::CMD_STORE_COND)) {I_r_fsm_state_req = I_REQ_VCI_CMD_WRITE_HEADER;} 
                    else                                                  {I_r_fsm_state_req = I_REQ_VCI_CMD_READ_HEADER;} 
                }
                break;
            case I_REQ_VCI_CMD_WRITE_HEADER :
                if((I_p_vci.cmdval == true) && (I_fifo_req.wok() == true)) {
                    I_req_fifo_write = true;
                    I_req_fifo_data = ((sc_uint<38>(I_p_vci.srcid.read() & srcid_mask)) << 24) |
                                      ((sc_uint<38>(I_p_vci.cmd.read()))                << 18) |
                                      ((sc_uint<38>(I_p_vci.contig.read()))             << 17) |
                                      ((sc_uint<38>(I_p_vci.cons.read()))               << 16) |
                                      ((sc_uint<38>(I_p_vci.plen.read()  & 0xFF))       << 8)  |
                                      ((sc_uint<38>(I_p_vci.trdid.read() &  0xF))       << 4)  |
                                       (sc_uint<38>(I_p_vci.pktid.read() &  0xF));
                    I_r_fsm_state_req = I_REQ_VCI_DATA_PAYLOAD;
                }
                break;
            case I_REQ_VCI_CMD_READ_HEADER :
                if((I_p_vci.cmdval == true) && (I_fifo_req.wok() == true)) {
                    I_req_fifo_write = true;
                    I_req_fifo_data = ((sc_uint<38>(ANOC_EOP))                          << 36) |
                                      ((sc_uint<38>(I_p_vci.srcid.read() & srcid_mask)) << 24) |
                                      ((sc_uint<38>(I_p_vci.cmd.read()))                << 18) |
                                      ((sc_uint<38>(I_p_vci.contig.read()))             << 17) |
                                      ((sc_uint<38>(I_p_vci.cons.read()))               << 16) |
                                      ((sc_uint<38>(I_p_vci.plen.read()  & 0xFF))       << 8)  |
                                      ((sc_uint<38>(I_p_vci.trdid.read() &  0xF))       << 4)  |
                                       (sc_uint<38>(I_p_vci.pktid.read() &  0xF));
                    I_r_fsm_state_req = I_REQ_ANOC_HEADER;
                }
                break;
            case I_REQ_VCI_DATA_PAYLOAD :
                if((I_p_vci.cmdval == true) && (I_fifo_req.wok() == true)) {
                    I_req_fifo_write = true;
                    I_req_fifo_data = ((sc_uint<38>(I_p_vci.be.read())) << 32) |
                                       (sc_uint<38>(I_p_vci.wdata.read()));
                    if(I_p_vci.eop == true) {
                        I_req_fifo_data = I_req_fifo_data | (((sc_uint<38>)ANOC_EOP) << 36);
                        I_r_fsm_state_req = I_REQ_ANOC_HEADER;
                    }
                }
                break;
        } // end switch I_r_fsm_state_req

        // I_r_fsm_state_rsp, BUF_RPKTID, I_rsp_fifo_read
        I_rsp_fifo_read = false;
        switch(I_r_fsm_state_rsp) {
            case I_RSP_ANOC_HEADER :
                if( I_fifo_rsp.rok() == true ){
                    I_rsp_fifo_read = true;
                    I_r_fsm_state_rsp = I_RSP_VCI_HEADER;
                }
                break;
            case I_RSP_VCI_HEADER :		
                if(I_fifo_rsp.rok() == true) {		    
                    I_rsp_fifo_read = true;
                    I_r_srcid = (uint32_t) ((I_fifo_rsp.read() >> 20) & srcid_mask);
                    I_r_error = (uint32_t) ((I_fifo_rsp.read() >> 8 ) & 0xF);
                    I_r_trdid = (uint32_t) ((I_fifo_rsp.read() >> 4 ) & 0xF);
                    I_r_pktid = (uint32_t)  (I_fifo_rsp.read()        & 0xF);
                    I_r_fsm_state_rsp = I_RSP_VCI_DATA_PAYLOAD;
                }
                break;
            case I_RSP_VCI_DATA_PAYLOAD :
                if((I_fifo_rsp.rok() == true) && (I_p_vci.rspack.read() == true)) {
                    I_rsp_fifo_read = true;
                    if(((I_fifo_rsp.read() >> 36) & ANOC_EOP) == ANOC_EOP) { I_r_fsm_state_rsp = I_RSP_ANOC_HEADER;}
                    else							                       { I_r_fsm_state_rsp = I_RSP_VCI_DATA_PAYLOAD;}
                }
                break;
        } // end switch I_r_fsm_state_rsp


        //////////////////////////////
        // VCI Target state machine
        //////////////////////////////

        T_req_fifo_read = false;
        switch(T_r_fsm_state_req) {
            case T_REQ_ANOC_HEADER :
                if( T_fifo_req.rok() == true){
                    T_req_fifo_read = true;
                    T_r_fsm_state_req = T_REQ_VCI_ADDRESS_HEADER;
//                    printf("REQ: Header\n");
                }
                break;
            case T_REQ_VCI_ADDRESS_HEADER :
                if(T_fifo_req.rok() == true) {
                    T_req_fifo_read = true;
                    T_r_address = (sc_uint<vci_param :: N>) (T_fifo_req.read());
                    T_r_fsm_state_req = T_REQ_VCI_CMD_HEADER;
//                    printf("REQ: Address\n");
                }
                break;
            case T_REQ_VCI_CMD_HEADER : 
                if( T_fifo_req.rok() == true  ){
                    T_req_fifo_read = true;
                    T_r_pktid  = (sc_uint<vci_param::P>) ((T_fifo_req.read())       & 0xF );
                    T_r_trdid  = (sc_uint<vci_param::T>) ((T_fifo_req.read() >> 4)  & 0xF );
                    T_r_plen   = (sc_uint<vci_param::K>) ((T_fifo_req.read() >> 8)  & 0xFF);
                    T_r_cons   = (bool)         	     ((T_fifo_req.read() >> 16) & 0x1 );
                    T_r_contig = (bool)                  ((T_fifo_req.read() >> 17) & 0x1 );
                    T_r_cmd    = (sc_uint<2>)            ((T_fifo_req.read() >> 18) & 0x3 );
                    T_r_srcid  = (sc_uint<vci_param::S>) ((T_fifo_req.read() >> 24) & srcid_mask );
//                    printf("REQ: CMD\n");
                    if(((T_fifo_req.read() >> 36) & ANOC_EOP) == ANOC_EOP) { T_r_fsm_state_req = T_REQ_VCI_NOPAYLOAD;    }
                    else                                                   { T_r_fsm_state_req = T_REQ_VCI_DATA_PAYLOAD; }
                }
                break;
            case T_REQ_VCI_DATA_PAYLOAD :
                if((T_p_vci.cmdack.read() == true) && (T_fifo_req.rok() == true)) {
                    T_req_fifo_read = true;
                    T_r_address = T_r_address.read() + (sc_uint<vci_param :: N>)vci_param::B;
                    if(((T_fifo_req.read() >> 36) & ANOC_EOP) == ANOC_EOP) {T_r_fsm_state_req = T_REQ_ANOC_HEADER; }
                }
                break;
            case T_REQ_VCI_NOPAYLOAD :		
                if(T_p_vci.cmdack.read() == true)
                    T_r_fsm_state_req = T_REQ_ANOC_HEADER;
                break;
        } // end switch T_r_fsm_state_req



        // T_r_fsm_state_rsp, T_rsp_fifo_write and T_rsp_fifo_data
        T_rsp_fifo_write = false;
        switch(T_r_fsm_state_rsp) {
            case T_RSP_ANOC_HEADER :
                if((T_p_vci.rspval.read() == true) && (T_fifo_rsp.wok() == true)) { 
                    T_rsp_fifo_write = true;
                    T_rsp_fifo_data = ((sc_uint<38>)1 << 37) | 
                        (sc_uint<38>(m_get_msb[T_p_vci.rsrcid.read()])) 
                        &  (0x7FFFFFFF >> (31 - anoc_yx_size * 2));
                    T_r_fsm_state_rsp = T_RSP_VCI_HEADER;
//                    printf("[%i,%i] RSP T: ANOC header. From [%i,%i], to %.2x, %x\n", m_position_Y, m_position_X, m_position_Y, m_position_X, m_get_msb[T_p_vci.rsrcid.read()], (int)T_p_vci.rsrcid.read());
                }
                break;
            case T_RSP_VCI_HEADER :
                if((T_p_vci.rspval.read() == true) && (T_fifo_rsp.wok() == true)) { 
                    T_rsp_fifo_write = true;
                    T_rsp_fifo_data = ((sc_uint<38>(T_p_vci.rsrcid.read())) << 20) |
                        ((sc_uint<38>(T_p_vci.rerror.read())) << 8 ) |
                        ((sc_uint<38>(T_p_vci.rtrdid.read())) << 4 ) |
                        (sc_uint<38>(T_p_vci.rpktid.read()));
                    T_r_fsm_state_rsp = T_RSP_VCI_DATA_PAYLOAD;
//                    printf("[%i,%i] RSP T: rsp header\n", m_position_Y, m_position_X);
                }
                break;
            case T_RSP_VCI_DATA_PAYLOAD :
                if((T_p_vci.rspval.read() == true) && (T_fifo_rsp.wok() == true)) { 
                    T_rsp_fifo_write = true;
                    T_rsp_fifo_data = (sc_uint<38>) (T_p_vci.rdata.read()); 
//                    printf("[%i,%i] RSP T: payload\n", m_position_Y, m_position_X);
                    if(T_p_vci.reop.read() == true){
                        T_rsp_fifo_data = T_rsp_fifo_data | ((sc_uint<38>(ANOC_EOP)) << 36);
                        T_r_fsm_state_rsp = T_RSP_ANOC_HEADER;	    
                    }
                }
                break;
        } // end switch T_r_fsm_state_rsp


        ////////////////////////
        //ANOC write oprerations
        ////////////////////////

        sc_uint<38> data_to_send;
        trans_to_send.set_srcid(m_position_Y<<16+m_position_X);
        I_req_fifo_read = false;
        T_rsp_fifo_read = false;
        if (((I_fifo_req.rok() == true) && (anoc_accept[0]==true)) || ((T_fifo_rsp.rok() == true) && (anoc_accept[1]==true))) {
            //At least one channel can be allocated
            if (((I_fifo_req.rok() == true) && (anoc_accept[0]==true)) && ((T_fifo_rsp.rok() == true) && (anoc_accept[1]==true))) {
                //I can allocate both; Decide in round robin
                if (req_channel_allocated == 0) {
                    //Then allocate the channel 1
                    req_channel_allocated = 1;
                } else {
                    //allocate the channel 0
                    req_channel_allocated = 0;
                }
            } else {
                //Only one channel can be allocated
                if ((I_fifo_req.rok() == true) && (anoc_accept[0]==true)) {
                    //Channel 0 can be allocated
                    req_channel_allocated = 0;
                } else {
                    //Channel 1 can be allocated
                    req_channel_allocated = 1;
                }
            }
            if (req_channel_allocated == 0) {
                    data_to_send = I_fifo_req.read();
                    I_req_fifo_read = true;
            } else {
                    data_to_send = T_fifo_rsp.read();
                    T_rsp_fifo_read = true;
            }
            trans_to_send.set_channel(req_channel_allocated); 
            if (data_to_send.range(37,37) == (sc_uint<1>)1 ) {  //BOP
//                printf("Send BOP\n");
                //Begin on Packet
                trans_to_send.set_bop((t_bit)1);
                anoc_dir path_to_target[ANOC_PATH_LENGTH];
                ComputePathToTarget(path_to_target, data_to_send.range(anoc_yx_size*2,0));
                //Set the path to target
                trans_to_send.set_path_to_target(path_to_target);
                trans_to_send.set_data_36((sc_uint<36>)trans_to_send.get_tot_path());  //Path is send on the first flit
            } else {
                trans_to_send.set_bop((t_bit)0);
                trans_to_send.set_data_36(data_to_send.range(35,0));
            }
            if (data_to_send.range(36,36) == 1 ) {  //EOP
//                printf("Send EOP\n");
                //End on Packet
                trans_to_send.set_eop((t_bit)1);
            } else {
                trans_to_send.set_eop((t_bit)0);
            }
            anoc_accept[req_channel_allocated] = false;
//            printf("[%i,%i] REQ ch %i: data_to_send %.8x, BOP: %i, EOP: %i\n", m_position_Y, m_position_X, req_channel_allocated, (int)data_to_send.range(31,0), (int)data_to_send.range(37,37), (int)data_to_send.range(36,36));
            //Write the transaction into the ANOC
            noc_out->data_port->write(trans_to_send);
        }


        /////////////////
        //FIFO operations
        /////////////////

        // I_fifo_req
        if((I_req_fifo_write == true)  && (I_req_fifo_read == false)) { I_fifo_req.simple_put(I_req_fifo_data); } 
        if((I_req_fifo_write == true)  && (I_req_fifo_read == true))  { I_fifo_req.put_and_get(I_req_fifo_data); } 
        if((I_req_fifo_write == false) && (I_req_fifo_read == true))  { I_fifo_req.simple_get(); }
        // T_fifo_req
        if((T_req_fifo_write == true)  && (T_req_fifo_read == false)) { T_fifo_req.simple_put(T_req_fifo_data); } 
        if((T_req_fifo_write == true)  && (T_req_fifo_read == true))  { T_fifo_req.put_and_get(T_req_fifo_data); } 
        if((T_req_fifo_write == false) && (T_req_fifo_read == true))  { T_fifo_req.simple_get(); }


        // I_fifo_rsp
        if((I_rsp_fifo_write == true)  && (I_rsp_fifo_read == false)) { I_fifo_rsp.simple_put(I_rsp_fifo_data); } 
        if((I_rsp_fifo_write == true)  && (I_rsp_fifo_read == true))  { I_fifo_rsp.put_and_get(I_rsp_fifo_data); } 
        if((I_rsp_fifo_write == false) && (I_rsp_fifo_read == true))  { I_fifo_rsp.simple_get(); }

        // T_fifo_rsp
        if((T_rsp_fifo_write == true)  && (T_rsp_fifo_read == false)) { T_fifo_rsp.simple_put(T_rsp_fifo_data); } 
        if((T_rsp_fifo_write == true)  && (T_rsp_fifo_read == true))  { T_fifo_rsp.put_and_get(T_rsp_fifo_data); } 
        if((T_rsp_fifo_write == false) && (T_rsp_fifo_read == true))  { T_fifo_rsp.simple_get(); }

    }; // end transition

    ////////////////////////////////
    //      genMoore
    ////////////////////////////////
    tmpl(void)::genMoore()
    {
        // VCI REQ interface

        switch(I_r_fsm_state_req) {
            case I_REQ_ANOC_HEADER :
                I_p_vci.cmdack = false;
                break;
            case I_REQ_VCI_ADDRESS_HEADER :
                I_p_vci.cmdack = false;
                break;
            case I_REQ_VCI_CMD_READ_HEADER :
                I_p_vci.cmdack = I_fifo_req.wok();
                break;
            case I_REQ_VCI_CMD_WRITE_HEADER :
                I_p_vci.cmdack = false;
                break;
            case I_REQ_VCI_DATA_PAYLOAD :
                I_p_vci.cmdack = I_fifo_req.wok();
                break;
        } // end switch VCI_I_r_fsm_state_req

        // VCI RSP interface

        switch(I_r_fsm_state_rsp) {
            case I_RSP_ANOC_HEADER :
            case I_RSP_VCI_HEADER :
                I_p_vci.rspval = false;
                I_p_vci.rdata =  (sc_uint<vci_param::N>) 0;
                I_p_vci.rpktid = (sc_uint<vci_param::P>) 0;
                I_p_vci.rtrdid = (sc_uint<vci_param::T>) 0;
                I_p_vci.rsrcid = (sc_uint<vci_param::S>) 0;
                I_p_vci.rerror = (sc_uint<vci_param::E>) 0;
                I_p_vci.reop   = false;
                break;
            case I_RSP_VCI_DATA_PAYLOAD :
                I_p_vci.rspval = I_fifo_rsp.rok();
                I_p_vci.rdata = (sc_uint<vci_param::N>) (I_fifo_rsp.read() & 0xffffffff);
                I_p_vci.rpktid = (sc_uint<vci_param::P>)I_r_pktid;
                I_p_vci.rtrdid = (sc_uint<vci_param::T>)I_r_trdid;
                I_p_vci.rsrcid = (sc_uint<vci_param::S>)I_r_srcid;
                I_p_vci.rerror = (sc_uint<vci_param::E>)I_r_error;
                if(((I_fifo_rsp.read() >> 36) & ANOC_EOP) == ANOC_EOP) I_p_vci.reop = true;
                else                                                             I_p_vci.reop = false;
        } // end switch VCI_I_r_fsm_state_rsp

        switch(T_r_fsm_state_req) {
            case T_REQ_ANOC_HEADER :
            case T_REQ_VCI_ADDRESS_HEADER :
            case T_REQ_VCI_CMD_HEADER :
                T_p_vci.cmdval = false;
                break;
            case T_REQ_VCI_DATA_PAYLOAD :
                T_p_vci.cmdval = T_fifo_req.rok();
                T_p_vci.address = T_r_address;
                T_p_vci.be = (sc_uint<vci_param::B>)((T_fifo_req.read() >> 32) & 0xF);
                T_p_vci.cmd = T_r_cmd;
                T_p_vci.wdata = (sc_uint<8*vci_param::B>)(T_fifo_req.read());
                T_p_vci.pktid = T_r_pktid;
                T_p_vci.srcid = T_r_srcid;
                T_p_vci.trdid = T_r_trdid;
                T_p_vci.plen = T_r_plen;
                T_p_vci.clen = 0;
                T_p_vci.cfixed = false;
                T_p_vci.cons = T_r_cons;
                T_p_vci.contig = T_r_contig;
                T_p_vci.wrap = false;
                if(((T_fifo_req.read() >> 36) & ANOC_EOP) == ANOC_EOP ) { T_p_vci.eop = true; }
                else                                                              { T_p_vci.eop = false; }
                break;
            case T_REQ_VCI_NOPAYLOAD :
                T_p_vci.cmdval = true;
                T_p_vci.address = T_r_address;
                T_p_vci.be = 0xF;
                T_p_vci.cmd = T_r_cmd;
                T_p_vci.wdata = 0;
                T_p_vci.pktid = T_r_pktid;
                T_p_vci.srcid = T_r_srcid;
                T_p_vci.trdid = T_r_trdid;
                T_p_vci.plen = T_r_plen;
                T_p_vci.clen = 0;
                T_p_vci.cfixed = false;
                T_p_vci.cons = T_r_cons;
                T_p_vci.contig = T_r_contig;
                T_p_vci.wrap = false;
                T_p_vci.eop  = true;
                break;
        } // end switch T_r_fsm_state_req

        // VCI RSP interface
        //
        switch(T_r_fsm_state_rsp){
            case T_RSP_ANOC_HEADER :
            case T_RSP_VCI_HEADER :
                T_p_vci.rspack = false;
                break;
            case T_RSP_VCI_DATA_PAYLOAD :
                T_p_vci.rspack = T_fifo_rsp.wok();
                break;
        }

    }; // end genMoore

    tmpl(void)::ComputePathToTarget(anoc_dir* path_to_target, sc_uint<anoc_yx_size*2> destination)
    {
        anoc_dir dir = RES;
        int distance = 0;
        int vector_position = 0;
        int dest_y = (int)destination.range(anoc_yx_size*2-1,anoc_yx_size);
        int dest_x = (int)destination.range(anoc_yx_size-1, 0);
        for (int i=0; i< ANOC_PATH_LENGTH; i++) {
            path_to_target[i]=RES;      //Reset the path vector
        }
        //Routing X-First
        if (dest_x > m_position_X) {
            dir = EAST;
            distance = dest_x - m_position_X;
        } else {
            dir = WEST;
            distance = m_position_X - dest_x;
        }
        for(int i=0; i<distance; i++) {
            path_to_target[vector_position] = dir;
            vector_position++;
        }
        if (dest_y > m_position_Y) {
            dir = SOUTH;
            distance = dest_y - m_position_Y;
        } else {
            dir = NORTH;
            distance = m_position_Y - dest_y;
        }
        for(int i=0; i<distance; i++) {
            path_to_target[vector_position] = dir;
            vector_position++;
        }
        path_to_target[vector_position] = RES;
//        printf("[%i,%i] Origin: %x%x, Destination : %.2x,  Path: %i,%i,%i,%i,%i,%i,%i,%i\n", m_position_Y, m_position_X, m_position_Y, m_position_X, (int)destination, path_to_target[0], path_to_target[1], path_to_target[2], path_to_target[3], path_to_target[4], path_to_target[5], path_to_target[6], path_to_target[7]); 
    }

    // Receive DATA transaction (remote call)
    tmpl(void)::put(const anoc_data_transaction& transaction) {
        PRINT(1,"Received data transaction");
        sc_uint<36> data = transaction.get_data_36();
        int channel = transaction.get_channel();
        noc_data_valid[channel]= true;
        noc_data[channel] = data | ((sc_uint<1>)transaction.get_bop())<<37 | ((sc_uint<1>)transaction.get_eop())<<36;
//        printf("[%i,%i] Channel %i, Data %.8x, BOP %i, EOP %i\n", m_position_Y, m_position_X, channel, (int)data.range(31, 0), transaction.get_bop(), transaction.get_eop());
    }
    // Receive ACCEPT transaction (remote call)
    tmpl(void)::put(const anoc_accept_transaction& transaction) {
        anoc_accept[transaction.get_channel()] = true;
        PRINT(1,"Received new accept on channel " << transaction.get_channel());
    }

    // Write ACCEPT transaction on channel 0 (sc_method)
    tmpl(void)::write_accept_0() {
        anoc_accept_transaction accept(0);
        noc_in->accept_port->write(accept);
        PRINT(1, "Accept sent on channel 0");
    }

    // Write ACCEPT transaction on channel 1 (sc_method)
    tmpl(void)::write_accept_1() {
        anoc_accept_transaction accept(1);
        noc_in->accept_port->write(accept);
        PRINT(1, "Accept sent on channel 1");
    }

}} // end namespace

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
