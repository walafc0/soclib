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
 * Copyright (c) UPMC, Lip6, SoC
 *         Alain Greiner <alain.greiner@lip6.fr>, 2006
 *
 * Maintainers: alain nipo
 */

#include "../include/vci_pi_initiator_wrapper.h"
#include "register.h"
#include "arithmetics.h"

namespace soclib { namespace caba {

#define Pibus soclib::caba::Pibus

#define tmpl(x) template<typename vci_param> x VciPiInitiatorWrapper<vci_param>

////////////////////////////////
//	constructor
////////////////////////////////

tmpl(/**/)::VciPiInitiatorWrapper(sc_module_name insname)
		   : soclib::caba::BaseModule(insname),
		   p_clk("clk"),
		   p_resetn("resetn"),
		   p_gnt("gnt"),
		   p_req("req"),
		   p_pi("pi"),
		   p_vci("vci"),
		   r_fsm_state("fsm_state"),
		   r_wdata("wdata"),
		   r_srcid("srcid"),
		   r_pktid("pktid"),
		   r_trdid("trdid"),
		   r_opc("opc"),
		   r_read("read")
{
SC_METHOD (transition);
dont_initialize();
sensitive << p_clk.pos();

SC_METHOD (genMealy);
dont_initialize();
sensitive  << p_clk.neg();
sensitive  << p_vci.address;
sensitive  << p_vci.eop;
sensitive  << p_pi.ack;
sensitive  << p_pi.d;
}

////////////////////////////////
//	transition 
////////////////////////////////
tmpl(void)::transition()
{
if (p_resetn == false) {
	r_fsm_state = FSM_IDLE;
	return;
} // end reset

switch (r_fsm_state) {
	case FSM_IDLE:
        if (p_vci.cmdval) {
		r_fsm_state 	= FSM_REQ;
		r_srcid		= (int)p_vci.srcid.read();
		r_pktid		= (int)p_vci.pktid.read();
		r_trdid		= (int)p_vci.trdid.read();
		r_addr 		= (int)p_vci.address.read();
		r_cells_to_go= (int)((p_vci.plen.read()
			+ soclib::common::ctz(p_vci.be.read())
				+ vci_param::B-1)/vci_param::B) - 1;
		r_wdata = (int)p_vci.wdata.read();
		if 	(p_vci.cmd.read() == vci_param::CMD_READ)  r_read = true;
		else if (p_vci.cmd.read() == vci_param::CMD_WRITE) r_read = false;
		else 	{
      			printf("ERROR : The vci_pi_initiator_wrapper accepts only\n");
      			printf("vci_param::CMD_READ and vci_param::CMD_WRITE commands\n");
			exit(1);
        		} 
		if      (p_vci.be.read() == 0xF) 	r_opc = Pibus::OPC_WDU;
		else if (p_vci.be.read() == 0x3) 	r_opc = Pibus::OPC_HW0;
		else if (p_vci.be.read() == 0xC) 	r_opc = Pibus::OPC_HW1;
		else if (p_vci.be.read() == 0x1) 	r_opc = Pibus::OPC_BY0;
		else if (p_vci.be.read() == 0x2) 	r_opc = Pibus::OPC_BY1;
		else if (p_vci.be.read() == 0x4) 	r_opc = Pibus::OPC_BY2;
		else if (p_vci.be.read() == 0x8) 	r_opc = Pibus::OPC_BY3;
		else 	{
      			printf("ERROR : The vci_pi_initiator_wrapper accepts only VCI BE\n");
      			printf("corresponding to WDU, HW0, HW1, BY0, BY1, BY2, BY3 formats\n");
			exit(1);
        		} 
#ifdef SOCLIB_MODULE_DEBUG
	{
	uint32_t cells = (int)((p_vci.plen.read()
			+ soclib::common::ctz(p_vci.be.read())
				+ vci_param::B-1)/vci_param::B) - 1;
	std::cout << name() << ": start cells_to_go=" << cells << " addr=" << std::hex << (p_vci.address.read());
	if (p_vci.cmd.read() == vci_param::CMD_READ) std::cout << " read";
	else std::cout << " wdata=" << r_wdata;
	std::cout << std::endl;
	}
#endif
	} // end if cmdval
	break;

	case FSM_REQ:
        if (p_gnt) r_fsm_state = FSM_AD;
        break;

	case FSM_AD:
	r_addr = r_addr + vci_param::B;
        if (r_cells_to_go == 0) r_fsm_state = FSM_DT;
	else           r_fsm_state = FSM_AD_DT;
        break;

	case FSM_AD_DT:
	if (p_vci.rspack == false) {
      		printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
      		printf("the VCI initiator always accept the response packet\n");
		exit(1);
        	} 
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() <<": ad_dt cells_to_go=" << r_cells_to_go << " addr=" << std::hex << (r_addr - 4);
	if (r_read) std::cout << " rdata=" << p_pi.d;
	else std::cout << " wdata=" << r_wdata;
	std::cout << " ack=" << p_pi.ack.read();
	std::cout << std::endl;
#endif
	r_wdata = (int)p_vci.wdata.read();
	if (p_pi.ack.read() != Pibus::ACK_WAT) {
		r_cells_to_go = r_cells_to_go - 1;
		r_addr = r_addr + vci_param::B;
	}
	if (r_cells_to_go == 1) {
		if      (p_pi.ack.read() == Pibus::ACK_RDY) r_fsm_state = FSM_DT;
		else if (p_pi.ack.read() == Pibus::ACK_WAT) r_fsm_state = FSM_AD_DT;
		else if (p_pi.ack.read() == Pibus::ACK_ERR) r_fsm_state = FSM_DT;
		else	{
      			printf("ERROR : The vci_pi_initiator_wrapper accepts only\n");
      			printf("Pibus::ACK_RDY, Pibus::ACK_WAT & Pibus::ACK_ERR responses\n");
			exit(1);
        		} 
	} else {
		if      (p_pi.ack.read() == Pibus::ACK_RDY) r_fsm_state = FSM_AD_DT;
		else if (p_pi.ack.read() == Pibus::ACK_WAT) r_fsm_state = FSM_AD_DT;
		else if (p_pi.ack.read() == Pibus::ACK_ERR) r_fsm_state = FSM_AD_DT;
		else	{
      			printf("ERROR : The vci_pi_initiator_wrapper accepts only\n");
      			printf("Pibus::ACK_RDY, Pibus::ACK_WAT & Pibus::ACK_ERR responses\n");
			exit(1);
        		} 
	}
        break;

	case FSM_DT:
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << ": dt cells_to_go=" << r_cells_to_go << " addr=" << std::hex << (r_addr - 4);
	if (r_read) std::cout << " rdata=" << p_pi.d;
	else std::cout << " wdata=" << r_wdata;
	std::cout << " ack=" << p_pi.ack.read();
	std::cout << std::endl;
#endif
	if (p_vci.rspack == false) {
      		printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
      		printf("the VCI initiator always accept the response packet\n");
		exit(1);
        	} 
	if      (p_pi.ack.read() == Pibus::ACK_RDY)  r_fsm_state = FSM_IDLE;
	else if (p_pi.ack.read() == Pibus::ACK_WAT)  r_fsm_state = FSM_DT;
	else if (p_pi.ack.read() == Pibus::ACK_ERR)  r_fsm_state = FSM_IDLE;
	else	{
      		printf("ERROR : The vci_pi_initiator_wrapper accepts only\n");
      		printf("Pibus::ACK_RDY, Pibus::ACK_WAT & Pibus::ACK_ERR responses\n");
		exit(1);
        	} 
        break;
        } // end switch fsm
}

////////////////////////////////
//	genMealy
////////////////////////////////
tmpl(void)::genMealy()
{
switch (r_fsm_state) {
       
	case FSM_IDLE:
	p_req     	= false;
	p_vci.cmdack 	= false;
	p_vci.rspval 	= false;
	p_vci.reop	= false;
        break;

	case FSM_REQ:
	p_req     	= true;
	p_vci.cmdack 	= false;
	p_vci.rspval 	= false;
	p_vci.reop	= false;
        break;

	case FSM_AD:
	p_req     	= false;
	p_vci.cmdack 	= true;
	p_vci.rspval 	= false;
	p_pi.a 		= (sc_dt::sc_uint<32>)r_addr;
	p_pi.read	= r_read;
	p_pi.opc	= (sc_dt::sc_uint<4>)r_opc;
	p_pi.lock 	= (r_cells_to_go > 0);
	p_vci.reop	= false;
        break;

	case FSM_AD_DT:
	p_req     	= false;
	p_vci.reop	= false;
	p_vci.rsrcid	= r_srcid.read();
	p_vci.rpktid	= r_pktid.read();
	p_vci.rtrdid	= r_trdid.read();
	if (p_pi.ack.read() == Pibus::ACK_RDY) {
		p_vci.rspval	= r_read;
		p_vci.cmdack	= true;
		p_vci.rerror	= 0;
	} else if (p_pi.ack.read() == Pibus::ACK_ERR) {
		p_vci.rspval	= r_read;
		p_vci.cmdack	= true;
		p_vci.rerror	= 1;
	} else {
		p_vci.rspval	= false;
		p_vci.cmdack	= false;
		p_vci.rerror	= 0;
	}
	if (r_read == false) 	p_pi.d  	= r_wdata.read();
	else			p_vci.rdata 	= p_pi.d.read();
	p_pi.a 		= (sc_dt::sc_uint<32>)r_addr;
	p_pi.read	= r_read;
	p_pi.opc	= (sc_dt::sc_uint<4>)r_opc;
	p_pi.lock 	= (r_cells_to_go > 0);
        break;

	case FSM_DT:
	p_req     	= false;
	p_pi.lock 	= false;
	p_pi.opc    = Pibus::OPC_NOP;
	p_vci.reop	= true;
	p_vci.rsrcid	= r_srcid.read();
	p_vci.rpktid	= r_pktid.read();
	p_vci.rtrdid	= r_trdid.read();
	if (p_pi.ack.read() == Pibus::ACK_RDY) {
		p_vci.cmdack 	= true;
		p_vci.rspval 	= true;
		p_vci.rerror	= 0;
	} else if (p_pi.ack.read() == Pibus::ACK_ERR) {
		p_vci.cmdack 	= true;
		p_vci.rspval 	= true;
		p_vci.rerror	= 1;
	} else {
		p_vci.cmdack 	= false;
		p_vci.rspval	= false;
		p_vci.rerror	= 0;
	}
	if (r_read == false) 	p_pi.d  	= r_wdata.read();
	else			p_vci.rdata 	= p_pi.d.read();
	break;
        } // end switch
}

}} // end namespace
