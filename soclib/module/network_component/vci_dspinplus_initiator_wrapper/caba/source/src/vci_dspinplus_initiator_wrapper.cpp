/* -*- c++ -*-
  * File : vci_dspinplus_initiator_warpper.cpp
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

#include "../include/vci_dspinplus_initiator_wrapper.h"
#include "register.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, int dspin_fifo_size, int dspin_yx_size> x VciDspinPlusInitiatorWrapper<vci_param, dspin_fifo_size, dspin_yx_size>

    ////////////////////////////////
    //      constructor
    ////////////////////////////////

    tmpl(/**/)::VciDspinPlusInitiatorWrapper(sc_module_name insname,
					 const soclib::common::MappingTable &mt)
	       : soclib::caba::BaseModule(insname),
	       fifo_req("FIFO_REQ", dspin_fifo_size),
	       fifo_rsp("FIFO_RSP", dspin_fifo_size)
    {
	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();
	SC_METHOD (genMoore);
	dont_initialize();
	sensitive  << p_clk.neg();

	SOCLIB_REG_RENAME(r_fsm_state_req);
	SOCLIB_REG_RENAME(r_fsm_state_rsp);
	SOCLIB_REG_RENAME(r_srcid);
	SOCLIB_REG_RENAME(r_pktid);
	SOCLIB_REG_RENAME(r_trdid);
	SOCLIB_REG_RENAME(r_error);

	m_routing_table = mt.getRoutingTable(soclib::common::IntTab(), 0);
	srcid_mask = 0x7FFFFFFF >> ( 31 - vci_param::S );
    } //  end constructor

    ////////////////////////////////
    //      transition
    ////////////////////////////////
    tmpl(void)::transition()
    {
	sc_uint<37>	req_fifo_data;
	bool		req_fifo_write;
	bool		req_fifo_read;
	sc_uint<33>	rsp_fifo_data;
	bool		rsp_fifo_write;
	bool		rsp_fifo_read;

	if (p_resetn == false) {
	    fifo_req.init();
	    fifo_rsp.init();
	    r_fsm_state_req = REQ_DSPINPLUS_HEADER;
	    r_fsm_state_rsp = RSP_DSPINPLUS_HEADER;
	    return;
	} // end reset

	// VCI request to DSPIN request
	// The VCI packet is analysed, translated,
	// and the DSPIN packet is stored in the fifo_req

	// req_fifo_read
	req_fifo_read = p_dspin_out.read.read();

	// r_fsm_state_req, req_fifo_write and req_fifo_data
	req_fifo_write = false;
	switch(r_fsm_state_req) {
	    case REQ_DSPINPLUS_HEADER :
		if(p_vci.cmdval == true && fifo_req.wok() == true) {
		    req_fifo_write = true;
		    req_fifo_data = ((sc_uint<37>(m_routing_table[(int)(p_vci.address.read())]))
				& (0x7FFFFFFF >> (31 - dspin_yx_size * 2)))
			    	<< (36 - dspin_yx_size * 2);
		    r_fsm_state_req = REQ_VCI_ADDRESS_HEADER;
		}
		break;
	    case REQ_VCI_ADDRESS_HEADER :
		if((p_vci.cmdval == true) && (fifo_req.wok() == true)) {
		    req_fifo_write = true;
		    req_fifo_data = (sc_uint<37>) p_vci.address.read();
		    if((p_vci.cmd.read() == vci_param::CMD_WRITE) || 
			    (p_vci.cmd.read() == vci_param::CMD_STORE_COND)) {r_fsm_state_req = REQ_VCI_CMD_WRITE_HEADER;} 
		    else                                	 {r_fsm_state_req = REQ_VCI_CMD_READ_HEADER;} 
		}
		break;
	    case REQ_VCI_CMD_WRITE_HEADER :
		if((p_vci.cmdval == true) && (fifo_req.wok() == true)) {
		    req_fifo_write = true;
		    req_fifo_data = ((sc_uint<37>(p_vci.srcid.read() & srcid_mask)) << 24) |
				    ((sc_uint<37>(p_vci.cmd.read()))                << 18) |
				    ((sc_uint<37>(p_vci.contig.read()))             << 17) |
				    ((sc_uint<37>(p_vci.cons.read()))               << 16) |
				    ((sc_uint<37>(p_vci.plen.read()  & 0xFF))       << 8)  |
				    ((sc_uint<37>(p_vci.trdid.read() &  0xF))       << 4)  |
				    (sc_uint<37>(p_vci.pktid.read() &  0xF));
		    r_fsm_state_req = REQ_VCI_DATA_PAYLOAD;
		}
		break;
	    case REQ_VCI_CMD_READ_HEADER :
		if((p_vci.cmdval == true) && (fifo_req.wok() == true)) {
		    req_fifo_write = true;
		    req_fifo_data = ((sc_uint<37>(DSPINPLUS_EOP))                   << 36) |
				    ((sc_uint<37>(p_vci.srcid.read() & srcid_mask)) << 24) |
				    ((sc_uint<37>(p_vci.cmd.read()))                << 18) |
				    ((sc_uint<37>(p_vci.contig.read()))             << 17) |
				    ((sc_uint<37>(p_vci.cons.read()))               << 16) |
				    ((sc_uint<37>(p_vci.plen.read()  & 0xFF))       << 8)  |
				    ((sc_uint<37>(p_vci.trdid.read() &  0xF))       << 4)  |
				    (sc_uint<37>(p_vci.pktid.read() &  0xF));
		    r_fsm_state_req = REQ_DSPINPLUS_HEADER;
		}
		break;
	    case REQ_VCI_DATA_PAYLOAD :
		if((p_vci.cmdval == true) && (fifo_req.wok() == true)) {
		    req_fifo_write = true;
		    req_fifo_data = ((sc_uint<37>(p_vci.be.read())) << 32) |
				    (sc_uint<37>(p_vci.wdata.read()));
		    if(p_vci.eop == true) {
			req_fifo_data = req_fifo_data | (((sc_uint<37>) DSPINPLUS_EOP) << 36);
			r_fsm_state_req = REQ_DSPINPLUS_HEADER;
		    }
		}
		break;
	} // end switch r_fsm_state_req
	
	// fifo_req
	if((req_fifo_write == true) && (req_fifo_read == false)) { fifo_req.simple_put(req_fifo_data); } 
	if((req_fifo_write == true) && (req_fifo_read == true))  { fifo_req.put_and_get(req_fifo_data); } 
	if((req_fifo_write == false) && (req_fifo_read == true)) { fifo_req.simple_get(); }

	// DSPIN response to VCI response
	// The DSPIN packet is stored in the fifo_rsp
	// The FIFO output is analysed and translated to a VCI packet

	// rsp_fifo_write, rsp_fifo_data
	rsp_fifo_write = p_dspin_in.write.read();
	rsp_fifo_data  = p_dspin_in.data.read();

	// r_fsm_state_rsp, BUF_RPKTID, rsp_fifo_read
	rsp_fifo_read = false;
	switch(r_fsm_state_rsp) {
	    case RSP_DSPINPLUS_HEADER :
		if( fifo_rsp.rok() == true ){
		    rsp_fifo_read = true;
		    r_fsm_state_rsp = RSP_VCI_HEADER;
		}
		break;
	    case RSP_VCI_HEADER :		
		if(fifo_rsp.rok() == true) {		    
		    rsp_fifo_read = true;
		    r_srcid = (uint32_t) ((fifo_rsp.read() >> 20) & srcid_mask);
		    r_error = (uint32_t) ((fifo_rsp.read() >> 8 ) & 0xF);
		    r_trdid = (uint32_t) ((fifo_rsp.read() >> 4 ) & 0xF);
		    r_pktid = (uint32_t)  (fifo_rsp.read()        & 0xF);
		    r_fsm_state_rsp = RSP_VCI_DATA_PAYLOAD;
		}
		break;
	    case RSP_VCI_DATA_PAYLOAD :
		if((fifo_rsp.rok() == true) && (p_vci.rspack.read() == true)) {
		    rsp_fifo_read = true;
		    if(((fifo_rsp.read() >> 32) & DSPINPLUS_EOP) == DSPINPLUS_EOP) { r_fsm_state_rsp = RSP_DSPINPLUS_HEADER;}
		    else							   { r_fsm_state_rsp = RSP_VCI_DATA_PAYLOAD;}
		}
		break;
	} // end switch r_fsm_state_rsp

	// fifo_rsp
	if((rsp_fifo_write == true) && (rsp_fifo_read == false)) { fifo_rsp.simple_put(rsp_fifo_data); } 
	if((rsp_fifo_write == true) && (rsp_fifo_read == true))  { fifo_rsp.put_and_get(rsp_fifo_data); } 
	if((rsp_fifo_write == false) && (rsp_fifo_read == true)) { fifo_rsp.simple_get(); }


    }; // end transition

    ////////////////////////////////
    //      genMealy
    ////////////////////////////////
    tmpl(void)::genMoore()
    {
	// VCI REQ interface

	switch(r_fsm_state_req) {
	    case REQ_DSPINPLUS_HEADER :
		p_vci.cmdack = false;
		break;
	    case REQ_VCI_ADDRESS_HEADER :
		p_vci.cmdack = false;
		break;
	    case REQ_VCI_CMD_READ_HEADER :
		p_vci.cmdack = fifo_req.wok();
		break;
	    case REQ_VCI_CMD_WRITE_HEADER :
		p_vci.cmdack = false;
		break;
	    case REQ_VCI_DATA_PAYLOAD :
		p_vci.cmdack = fifo_req.wok();
		break;
	} // end switch VCI_r_fsm_state_req

	// VCI RSP interface

	switch(r_fsm_state_rsp) {
	    case RSP_DSPINPLUS_HEADER :
	    case RSP_VCI_HEADER :
		p_vci.rspval = false;
		p_vci.rdata =  (sc_uint<vci_param::N>) 0;
		p_vci.rpktid = (sc_uint<vci_param::P>) 0;
		p_vci.rtrdid = (sc_uint<vci_param::T>) 0;
		p_vci.rsrcid = (sc_uint<vci_param::S>) 0;
		p_vci.rerror = (sc_uint<vci_param::E>) 0;
		p_vci.reop   = false;
		break;
	    case RSP_VCI_DATA_PAYLOAD :
		p_vci.rspval = fifo_rsp.rok();
		p_vci.rdata = (sc_uint<vci_param::N>) (fifo_rsp.read() & 0xffffffff);
		p_vci.rpktid = (sc_uint<vci_param::P>)r_pktid;
		p_vci.rtrdid = (sc_uint<vci_param::T>)r_trdid;
		p_vci.rsrcid = (sc_uint<vci_param::S>)r_srcid;
  	        p_vci.rerror = (sc_uint<vci_param::E>)r_error;
		if(((fifo_rsp.read() >> 32) & DSPINPLUS_EOP) == DSPINPLUS_EOP) 	p_vci.reop = true;
		else							 	p_vci.reop = false;
	} // end switch VCI_r_fsm_state_rsp

	// DSPIN_OUT interface

	p_dspin_out.write = fifo_req.rok();
	p_dspin_out.data = fifo_req.read();

	// DSPIN_IN interface

	p_dspin_in.read = fifo_rsp.wok();

    }; // end genMoore

}} // end namespace

