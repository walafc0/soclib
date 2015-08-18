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
 * Copyright (c) Lab-STICC, UBS
 *         Caaliph Andriamisaina <andriami@univ-ubs.fr>, 2008
 *
 * Based on previous works by Sebastien Tregaro, 2005
 */
 
#include <stdlib.h>
#include "base_module.h"
#include "register.h"
#include "../include/mailbox.h"

namespace soclib {
namespace caba {

#define tmpl(x) template <typename vci_param, int Nirq> x mailbox<vci_param, Nirq>

//       Constructor   
tmpl()::mailbox(sc_module_name		insname, 		// instance name
		const soclib::common::IntTab 	&index,		// VCI target index
		const soclib::common::MappingTable 	&mt)    	// segment table
		: soclib::caba::BaseModule(insname),
		p_clk("clk"),
		p_resetn("resetn"),
		p_vci("vci"),
		m_segment(mt.getSegment(index))
{
	
	SC_METHOD(transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD(genMoore);
	dont_initialize();
	sensitive << p_clk.neg();
	NAME = (const char *) insname;	//instance name
	BASE = m_segment.baseAddress();
	SIZE = m_segment.size();

 	if ((Nirq < 1) || (Nirq > 256)) {
	  printf("Error in component mailbox %s\n", NAME);
	  printf("The Nirq parameter (number of independant mailbox) \n");
	  printf("cannot be larger than 256\n");
	  exit(1);
	}
  	if (SIZE < (Nirq<<5)) {
  	//A REVOIR ->multiple de 3! on pourra rajouter un registre de plus dans ce cas!
	  printf("Error in component mailbox %s\n", NAME);
	  printf("The segment SIZE allocated to this component\n");
	  printf("must be equal or larger than 32*Nirq bytes\n");
	  exit(1);
  	}
  	if ((BASE & 0x00000003) != 0x0) {	
  	//A REVOIR ->multiple de 3! on pourra rajouter un registre de plus dans ce cas!
	  printf("Error in component mailbox %s\n", NAME);
	  printf("The BASE address must be multiple of 4\n");
	  exit(1);
  	}
	
	printf("Successful Instanciation of mailbox: %s\n",NAME);
} 
// end constructor

tmpl(void)::transition()
{

	if(p_resetn == false) 
	{
        	r_vci_fsm = IDLE;	// initialization
		for(int i = 0;i < Nirq;i++)
		{
			p_irq[i] = false;
			write_en[i] = false;
			COMMAND[i] = 0;
			DATA_W[i] = 0;
		}
		return;
	}

	int address;


	switch(r_vci_fsm) 
	{
		case IDLE :
			if(p_vci.cmdval.read() == true) {
				address = (int)p_vci.address.read() & 0xFFFFFFFC;
				r_buf_srcid = (int)p_vci.srcid.read();
				r_buf_trdid = (int)p_vci.trdid.read();
				r_buf_pktid = (int)p_vci.pktid.read();
				r_buf_wdata = (int)p_vci.wdata.read();
				r_buf_num   = ((address - BASE) & 0x00000FF0) >> 4;
				r_buf_adr   =  (address - BASE) & 0x0000000F;
				if ((address < BASE) || (address >= (BASE + SIZE))) { 
					if (p_vci.eop.read() == true) 	{r_vci_fsm = ERROR_EOP; }
					else	{r_vci_fsm = ERROR_RSP; }
				} else if (p_vci.cmd.read() == vci_param::CMD_WRITE) {
					if (p_vci.eop.read() == true) 	{r_vci_fsm = WRITE_EOP; }
					else	{r_vci_fsm = WRITE_RSP; }
				} else {
					if (p_vci.eop.read() == true) 	{r_vci_fsm = READ_EOP; }
					else	{r_vci_fsm = READ_RSP; }
				}
			}
		break;
	
		case WRITE_RSP :
		case WRITE_EOP :				//adresses test to know which register is affected
			if (r_buf_adr == COMM_ADR) {
				COMMAND[r_buf_num] = r_buf_wdata;	// get command word
				write_en[r_buf_num] = true;	// a new write occurs
				//p_irq[r_buf_num] = true;		// generate an interrupt
			} else if (r_buf_adr == RESET_ADR) {
				if (p_irq[r_buf_num] == true) {
				p_irq[r_buf_num] = false;		// reset corresponding interrupt
				write_en[r_buf_num] = false;	//
				}
			} else if (r_buf_adr == DATA_ADR) {
				DATA_W[r_buf_num] = r_buf_wdata;	// get data word
			}if (p_vci.rspack.read() == true) { r_vci_fsm = IDLE; }
		break;
	
		case READ_RSP :
		case READ_EOP :
			if (p_vci.rspack.read() == true) { r_vci_fsm = IDLE; }
		break;
	
		case ERROR_RSP :
		case ERROR_EOP :
			if (p_vci.rspack.read() == true) { r_vci_fsm = IDLE; }
		break;
	}

	//Interrupt management
	for(int i = 0 ; i < Nirq ; i++) { 
		if(write_en[i]) p_irq[i] = true;
	} 
}

//genMoore
tmpl(void)::genMoore()
{

	// p_vci signals
	p_vci.rspSetIds(r_buf_srcid.read(), r_buf_trdid.read(), r_buf_pktid.read());
	switch (r_vci_fsm) {
		case IDLE:
		
			p_vci.rspval = false;				
			p_vci.rerror  = 0;
			p_vci.rdata  = 0;
			p_vci.reop   = false;
			break;
		case WRITE_RSP:
			p_vci.cmdack = false;
			p_vci.rspval = true;
			p_vci.rerror  = 0;
			p_vci.rdata  = 0;
			p_vci.reop   = false;	
			break;
		case WRITE_EOP:
			p_vci.rspval = true;
			p_vci.rerror  = 0;
			p_vci.rdata  = 0;
			p_vci.reop   = true;
			break;
		case READ_RSP:	//lecture des données
			p_vci.rspval = true;
			p_vci.rerror  = false;
			if (r_buf_adr == COMM_ADR)    	{ p_vci.rdata  = (sc_uint<32>)COMMAND[r_buf_num]; } 	// puts command word
			if (r_buf_adr == DATA_ADR)   	{ p_vci.rdata  = (sc_uint<32>)DATA_W[r_buf_num]; }		// puts data word
			if (r_buf_adr == RESET_ADR) 	{ p_vci.rdata  = (sc_uint<32>)(p_irq[r_buf_num] == true); }	
			p_vci.reop   = false;
			break;
		case READ_EOP:
			p_vci.rspval = true;
			p_vci.rerror  = 0;
			if (r_buf_adr == COMM_ADR)  	{ p_vci.rdata  = (sc_uint<32>)COMMAND[r_buf_num]; }
			if (r_buf_adr == DATA_ADR)  	{ p_vci.rdata  = (sc_uint<32>)DATA_W[r_buf_num]; }
			if (r_buf_adr == RESET_ADR) 	{ p_vci.rdata  = (sc_uint<32>)(p_irq[r_buf_num] == true); }
			p_vci.reop   = true;
			break;
		case ERROR_RSP:
			p_vci.rspval = true;
			p_vci.rerror  = 1;
			p_vci.rdata  = 0;
			p_vci.reop   = false;
			break;
		case ERROR_EOP:
			p_vci.rspval = true;
			p_vci.rerror  = 1;
			p_vci.rdata  = 0;
			p_vci.reop   = true;
			break;
		}
	p_vci.cmdack = (r_vci_fsm==IDLE);
}
// end genMoore
}} 

