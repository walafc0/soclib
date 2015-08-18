/* -*- c++ -*-
  * File : vci_dspinplus_initiator_warpper.h
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

#include "../include/vci_dspinplus_target_wrapper.h"
#include "register.h"

namespace soclib { namespace caba {

#define tmpl(x) template<typename vci_param, int dspin_fifo_size, int dspin_yx_size> x VciDspinPlusTargetWrapper<vci_param, dspin_fifo_size, dspin_yx_size>

    ////////////////////////////////
    //      constructor
    ////////////////////////////////

    tmpl(/**/)::VciDspinPlusTargetWrapper(sc_module_name insname,
	    const soclib::common::MappingTable &mt)
	       : soclib::caba::BaseModule(insname),
			   p_clk("clk"),
			   p_resetn("resetn"),
			   p_dspin_out("dspin_out"),
			   p_dspin_in("dspin_in"),
			   p_vci("vci"),
			   r_fsm_state_req("fsm_state_req"),
			   r_fsm_state_rsp("fsm_state_rsp"),
			   r_cmd("cmd"),
			   r_srcid("srcid"),
			   r_pktid("pktid"),
			   r_trdid("trdid"),
			   r_cons("cons"),
			   r_contig("contig"),
			   r_address("address"),
    			   r_plen("plen"),
			   m_get_msb(mt.getIdMaskingTable(0)),
			   fifo_req("FIFO_REQ", dspin_fifo_size),
			   fifo_rsp("FIFO_RSP", dspin_fifo_size)
    {
	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();
	SC_METHOD (genMoore);
	dont_initialize();
	sensitive  << p_clk.neg();

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

	// DSPIN request to VCI request 
	// The DSPIN packet is written into the fifo_req
	// and the FIFO output is analysed and translated

	// req_fifo_write and req_fifo_data
	req_fifo_write = p_dspin_in.write.read();
	req_fifo_data  = p_dspin_in.data.read();

	// r_fsm_state_req, req_fifo_read, BUF_CMD, BUF_SRCID, BUF_PKTID, BUF_TRDID, BUF_MSBAD, BUF_LSBAD
	req_fifo_read = false;
	switch(r_fsm_state_req) {
	    case REQ_DSPINPLUS_HEADER :
		if( fifo_req.rok() == true){
		    req_fifo_read = true;
		    r_fsm_state_req = REQ_VCI_ADDRESS_HEADER;
		}
		break;
	    case REQ_VCI_ADDRESS_HEADER :
		if(fifo_req.rok() == true) {
		    req_fifo_read = true;
		    r_address = (sc_uint<vci_param :: N>) (fifo_req.read());
		    r_fsm_state_req = REQ_VCI_CMD_HEADER;
		}
		break;
	    case REQ_VCI_CMD_HEADER : 
		if( fifo_req.rok() == true  ){
		    req_fifo_read = true;
		    r_pktid  = (sc_uint<vci_param::P>) ((fifo_req.read())       & 0xF );
		    r_trdid  = (sc_uint<vci_param::T>) ((fifo_req.read() >> 4)  & 0xF );
		    r_plen   = (sc_uint<vci_param::K>) ((fifo_req.read() >> 8)  & 0xFF);
		    r_cons   = (bool)         	       ((fifo_req.read() >> 16) & 0x1 );
		    r_contig = (bool)                  ((fifo_req.read() >> 17) & 0x1 );
		    r_cmd    = (sc_uint<2>)            ((fifo_req.read() >> 18) & 0x3 );
		    r_srcid  = (sc_uint<vci_param::S>) ((fifo_req.read() >> 24) & srcid_mask );
		    if(((fifo_req.read() >> 36) & DSPINPLUS_EOP) == DSPINPLUS_EOP) { r_fsm_state_req = REQ_VCI_NOPAYLOAD;    }
		    else							   { r_fsm_state_req = REQ_VCI_DATA_PAYLOAD; }
		}
		break;
	    case REQ_VCI_DATA_PAYLOAD :
		if((p_vci.cmdack.read() == true) && (fifo_req.rok() == true)) {
		    req_fifo_read = true;
		    r_address = r_address.read() + (sc_uint<vci_param :: N>)vci_param::B;
		    if(((fifo_req.read() >> 36) & DSPINPLUS_EOP) == DSPINPLUS_EOP) {r_fsm_state_req = REQ_DSPINPLUS_HEADER; }
		}
		break;
	    case REQ_VCI_NOPAYLOAD :		
		if(p_vci.cmdack.read() == true)
		    r_fsm_state_req = REQ_DSPINPLUS_HEADER;
		break;
	} // end switch r_fsm_state_req
	
	// fifo_req
	if((req_fifo_write == true) && (req_fifo_read == false)) { fifo_req.simple_put(req_fifo_data); } 
	if((req_fifo_write == true) && (req_fifo_read == true))  { fifo_req.put_and_get(req_fifo_data); } 
	if((req_fifo_write == false) && (req_fifo_read == true)) { fifo_req.simple_get(); }


	// VCI response to DSPIN response 
	// The VCI packet is analysed, translated, and
	// the SPIN packet is written into the fifo_rsp
	// 
	// rsp_fifo_read 
	rsp_fifo_read  = p_dspin_out.read.read();

	// r_fsm_state_rsp, rsp_fifo_write and rsp_fifo_data
	rsp_fifo_write = false;
	switch(r_fsm_state_rsp) {
	    case RSP_DSPINPLUS_HEADER :
		if((p_vci.rspval.read() == true) && (fifo_rsp.wok() == true)) { 
		    rsp_fifo_write = true;
		    rsp_fifo_data = (((sc_uint<33>(m_get_msb[p_vci.rsrcid.read()])) 
				&  (0x7FFFFFFF >> (31 - dspin_yx_size * 2))) 
			    << (32 - dspin_yx_size * 2));
		    r_fsm_state_rsp = RSP_VCI_HEADER;
		}
		break;
	    case RSP_VCI_HEADER :
		if((p_vci.rspval.read() == true) && (fifo_rsp.wok() == true)) { 
		    rsp_fifo_write = true;
		    rsp_fifo_data = ((sc_uint<33>(p_vci.rsrcid.read())) << 20) |
		  		    ((sc_uint<33>(p_vci.rerror.read())) << 8 ) |
				    ((sc_uint<33>(p_vci.rtrdid.read())) << 4 ) |
			             (sc_uint<33>(p_vci.rpktid.read()));
		    r_fsm_state_rsp = RSP_VCI_DATA_PAYLOAD;
		}
		break;
	    case RSP_VCI_DATA_PAYLOAD :
		if((p_vci.rspval.read() == true) && (fifo_rsp.wok() == true)) { 
		    rsp_fifo_write = true;
		    rsp_fifo_data = (sc_uint<33>) (p_vci.rdata.read()); 
		    if(p_vci.reop.read() == true){
			rsp_fifo_data = rsp_fifo_data | ((sc_uint<33>(DSPINPLUS_EOP)) << 32);
			r_fsm_state_rsp = RSP_DSPINPLUS_HEADER;	    
		    }
		}
		break;
	} // end switch r_fsm_state_rsp

	// fifo_rsp
	if((rsp_fifo_write == true) && (rsp_fifo_read == false)) { fifo_rsp.simple_put(rsp_fifo_data); } 
	if((rsp_fifo_write == true) && (rsp_fifo_read == true))  { fifo_rsp.put_and_get(rsp_fifo_data); } 
	if((rsp_fifo_write == false) && (rsp_fifo_read == true)) { fifo_rsp.simple_get(); }
    } // end transition

    ////////////////////////////////
    //      genMealy
    ////////////////////////////////
    tmpl(void)::genMoore()
    {
	// VCI REQ interface

	switch(r_fsm_state_req) {
	    case REQ_DSPINPLUS_HEADER :
	    case REQ_VCI_ADDRESS_HEADER :
	    case REQ_VCI_CMD_HEADER :
		p_vci.cmdval = false;
		break;
	    case REQ_VCI_DATA_PAYLOAD :
		p_vci.cmdval = fifo_req.rok();
		p_vci.address = r_address;
		p_vci.be = (sc_uint<vci_param::B>)((fifo_req.read() >> 32) & 0xF);
		p_vci.cmd = r_cmd;
		p_vci.wdata = (sc_uint<8*vci_param::B>)(fifo_req.read());
		p_vci.pktid = r_pktid;
		p_vci.srcid = r_srcid;
		p_vci.trdid = r_trdid;
		p_vci.plen = r_plen;
		p_vci.clen = 0;
		p_vci.cfixed = false;
		p_vci.cons = r_cons;
		p_vci.contig = r_contig;
		p_vci.wrap = false;
		if(((fifo_req.read() >> 36) & DSPINPLUS_EOP) == DSPINPLUS_EOP ) { p_vci.eop = true; }
		else                                                            { p_vci.eop = false; }
		break;
	    case REQ_VCI_NOPAYLOAD :
		p_vci.cmdval = true;
		p_vci.address = r_address;
		p_vci.be = 0xF;
		p_vci.cmd = r_cmd;
		p_vci.wdata = 0;
		p_vci.pktid = r_pktid;
		p_vci.srcid = r_srcid;
		p_vci.trdid = r_trdid;
		p_vci.plen = r_plen;
		p_vci.clen = 0;
		p_vci.cfixed = false;
		p_vci.cons = r_cons;
		p_vci.contig = r_contig;
		p_vci.wrap = false;
		p_vci.eop  = true;
		break;
	} // end switch r_fsm_state_req

	// VCI RSP interface
	//
	switch(r_fsm_state_rsp){
	    case RSP_DSPINPLUS_HEADER :
	    case RSP_VCI_HEADER :
		p_vci.rspack = false;
		break;
	    case RSP_VCI_DATA_PAYLOAD :
		p_vci.rspack = fifo_rsp.wok();
		break;
	}

	// p_dspin_in interface

	p_dspin_in.read = fifo_req.wok();

	// p_dspin_out interface

	p_dspin_out.write = fifo_rsp.rok();
	p_dspin_out.data = fifo_rsp.read();

    } // end genMoore

}} // end namespace

