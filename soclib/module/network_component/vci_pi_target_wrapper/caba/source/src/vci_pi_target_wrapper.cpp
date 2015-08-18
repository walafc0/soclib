/* -*- c++ -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Alain Greiner <alain.greiner@lip6.fr>, 2006
 *
 * Maintainers: alain nipo
 */

#include "../include/vci_pi_target_wrapper.h"  
#include "register.h"

#define Pibus soclib::caba::Pibus

namespace  soclib { namespace caba {

#define tmpl(x) template<typename vci_param> x VciPiTargetWrapper<vci_param>

/////////////////////////////////////////////////////////////////////
//	constructor
/////////////////////////////////////////////////////////////////////
tmpl(/**/)::VciPiTargetWrapper(sc_module_name insname)
		   : soclib::caba::BaseModule(insname),
		   p_clk("clk"),
		   p_resetn("resetn"),
		   p_sel("sel"),
		   p_pi("pi"),
		   p_vci("vci"),
		   r_fsm_state("fsm_state"),
		   r_adr("adr"),
		   r_opc("opc")
{
SC_METHOD (transition);
dont_initialize();
sensitive << p_clk.pos();

SC_METHOD (genMealy);
dont_initialize();
sensitive << p_clk.neg();
sensitive << p_vci.rdata;
sensitive << p_vci.rspval;
sensitive << p_vci.rerror;
sensitive << p_pi.d;
sensitive << p_pi.opc;
sensitive << p_sel;
}

/*****************************************************************************
	transition 
******************************************************************************/
tmpl(void)::transition()
{

if (p_resetn == false) {
	r_fsm_state = FSM_IDLE;
	return;
}

switch (r_fsm_state) {
	case FSM_IDLE:
		if (p_sel.read() && p_pi.opc.read() != Pibus::OPC_NOP) {
		r_adr 	= p_pi.a.read();
		r_opc	= p_pi.opc.read();
		if (p_pi.read == true)  r_fsm_state = FSM_CMD_READ;
		else			r_fsm_state = FSM_CMD_WRITE;
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << ": start " << (p_pi.read ? "read" : "write")
		<< " addr=0x" << std::hex << p_pi.a.read() << std::endl;
#endif
	}
	break;

	case FSM_CMD_READ:
	if (p_vci.cmdack) r_fsm_state = FSM_RSP_READ;
	break;

	case FSM_RSP_READ:
	if (p_vci.rspval) {
		r_adr 	= p_pi.a.read();
		r_opc	= p_pi.opc.read();
		if (p_vci.reop) r_fsm_state = FSM_IDLE;
		else		r_fsm_state = FSM_CMD_READ;
	}
	break;

	case FSM_CMD_WRITE:
	if (p_vci.cmdack) r_fsm_state = FSM_RSP_WRITE;
	break;

	case FSM_RSP_WRITE:
	if (p_vci.rspval) {
		r_adr 	= p_pi.a.read();
		r_opc	= p_pi.opc.read();
		if (p_vci.reop) r_fsm_state = FSM_IDLE;
		else		r_fsm_state = FSM_CMD_WRITE;
	}
	break;
} // end switch FSM

}

/************************************************************************
	genMealy
************************************************************************/
tmpl(void)::genMealy()
{
switch (r_fsm_state) {
	case FSM_IDLE:
	p_vci.cmdval = false;
	p_vci.rspack = false;
	p_vci.eop = false;
	if (p_sel.read() && p_pi.opc.read() != Pibus::OPC_NOP) {
		p_pi.ack = Pibus::ACK_WAT;
	}
	break;

	case FSM_CMD_READ:
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << ": CMD_READ addr=0x" << std::hex << r_adr.read() << std::endl;
#endif
	p_vci.cmdval = true;
	p_vci.rspack = false;
	p_vci.address	= r_adr.read();
	p_vci.wdata	= 0;
	p_vci.plen	= 0;
	p_vci.be	= 0xF;
	p_vci.cmd	= vci_param::CMD_READ;
	if (p_pi.lock.read() == true)	p_vci.eop = false;
	else			p_vci.eop = true;
	p_pi.ack = Pibus::ACK_WAT;
	p_pi.d   = 0;
	break;
	
	case FSM_RSP_READ:
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << ": RSP_READ addr=0x" << std::hex << r_adr.read() << std::endl;
#endif
	p_vci.cmdval = false;
	p_vci.rspack = true;
	if (p_vci.rspval) {
		p_pi.d = p_vci.rdata.read();
		if (p_vci.rerror.read() != 0)	{
			p_pi.ack = Pibus::ACK_ERR;
			std::cout << name() << ": read addr=0x" << std::hex << p_vci.address.read()
				<< " rerror=" << (unsigned int)p_vci.rerror.read() << std::endl;
		} else				p_pi.ack = Pibus::ACK_RDY;
	} else {
		p_pi.ack = Pibus::ACK_WAT;
		p_pi.d   = 0;
	}
	break;
	
	case FSM_CMD_WRITE:
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << ": CMD_WRITE addr=0x" << std::hex << r_adr.read() << std::endl;
#endif
	p_vci.cmdval = true;
	p_vci.rspack = false;
	p_vci.address	= r_adr.read();
	p_vci.wdata	= p_pi.d.read();
	p_vci.plen	= 0;
	if 	((r_opc.read() & 0x8) == 0)		p_vci.be = 0xF;
	else if (r_opc.read() == Pibus::OPC_HW0)	p_vci.be = 0x3;
	else if (r_opc.read() == Pibus::OPC_HW1)	p_vci.be = 0xC;
	else if (r_opc.read() == Pibus::OPC_BY0)	p_vci.be = 0x1;
	else if (r_opc.read() == Pibus::OPC_BY1)	p_vci.be = 0x2;
	else if (r_opc.read() == Pibus::OPC_BY2)	p_vci.be = 0x4;
	else if (r_opc.read() == Pibus::OPC_BY3)	p_vci.be = 0x8;
	else						p_vci.be = 0x0;
	p_vci.cmd	= vci_param::CMD_WRITE;
	if (p_pi.lock.read() == true)	p_vci.eop = false;
	else			p_vci.eop = true;
	p_pi.ack = Pibus::ACK_WAT;
	break;

	case FSM_RSP_WRITE:
#ifdef SOCLIB_MODULE_DEBUG
	std::cout << name() << ": RSP_WRITE addr=0x" << std::hex << r_adr.read() << std::endl;
#endif
	p_vci.cmdval = false;
	p_vci.rspack = true;
	if (p_vci.rspval) {
		if (p_vci.rerror.read() != 0) {
			p_pi.ack = Pibus::ACK_ERR;
			std::cout << name() << ": write rerror=" << (unsigned int)p_vci.rerror.read() << std::endl;
		} else
			p_pi.ack = Pibus::ACK_RDY;
	} else {
		p_pi.ack = Pibus::ACK_WAT;
		p_pi.d   = 0;
	}
	break;
} // end switch FSM

}

}} // end namespace
