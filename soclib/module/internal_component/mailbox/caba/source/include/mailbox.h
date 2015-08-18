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

#ifndef mailbox_H
#define mailbox_H

#include <systemc.h>
#include "caba_base_module.h"
#include "vci_target.h"
#include "vci_initiator.h"
#include "mapping_table.h"
#include "segment.h"

namespace soclib {
namespace caba {

using namespace sc_core;

template <typename vci_param, int Nirq> 
class mailbox 
	: public soclib::caba::BaseModule {

// EXTERNAL PORTS
public:
	sc_in<bool>                  	p_clk;		//clock
	sc_in<bool>                  	p_resetn;		//reset
	sc_out<bool>		    	p_irq[Nirq];	//Interrupt output
	soclib::caba::VciTarget<vci_param>	p_vci;		//VCI bus
	
// STRUCTURAL PARAMETERS
	const	char	*NAME;				//component's name
	int		BASE;				//base address
	int		SIZE;				//allocated memory size
	
// REGISTERS
	sc_signal<int>	r_vci_fsm;
	sc_signal<int>	COMMAND[Nirq];			//To generate an IRQ
	sc_signal<int>	DATA_W[Nirq];			//If datas for processor
	int		write_en[Nirq];			//to know if a new write occurs
	
	sc_signal<int>	r_buf_srcid;			// save SCRID
	sc_signal<int>	r_buf_trdid;			// save TRDID
	sc_signal<int>	r_buf_pktid;			// save PKTID
	sc_signal<int>	r_buf_num;			// save selected MailBox index
	sc_signal<int>	r_buf_adr;			// save selected register address
	sc_signal<int>	r_buf_wdata;			// save data

//	FSM states
	enum{
		IDLE           = 0,
		ERROR_RSP      = 1,
		ERROR_EOP      = 2,
		WRITE_RSP      = 3,
		WRITE_EOP      = 4,
		READ_RSP       = 5,
		READ_EOP       = 6
	};
	
//	register mapping	
	enum{
		COMM_ADR	= 0,
		DATA_ADR	= 4,
		// free space	= 8
		RESET_ADR	= 12
	};
	soclib::common::Segment m_segment;
	mailbox(sc_module_name 		insname, 		// instance name
		const soclib::common::IntTab 	&index,		// VCI target index
		const soclib::common::MappingTable 	&mt);    	// segment table
protected:
	SC_HAS_PROCESS(mailbox);
private:
	void transition();
	void genMoore();
};
}}
#endif
	

