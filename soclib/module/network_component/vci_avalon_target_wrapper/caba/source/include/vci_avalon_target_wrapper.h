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
 * Copyright (C) IRISA/INRIA, 2007-2008
 *         Francois Charot <charot@irisa.fr>
 * 	   Charles Wagner <wagner@irisa.fr>
 * 
 * Maintainer: wagner
 * 
 * File : vci_avalon_target_wrapper.h
 * Date : 23/05/2007
 *
 *
 * This hardware component is a VCI/AVALON protocol converter.
 * It behaves as an initiator on the AVALON and as a target on 
 * the VCI side. It can be used by a VCI initiator to interface
 * a AVALON based system. 
 * The VCI address and data fields must have 32 bits. 
 *
 * The supported AVALON response (...............
 * COMPLETER .......are PI_ACK_RDY, PI_ACK_WAT, PI_ACK_ERR.
 * 
 * The VCI target don't supporte split operation
 * 
 * The VCI initiator is supposed to be always "ready" :   
 * When a command packet starts, the CMDVAL signal is true until 
 * the end of packet(no buble), and the RSPACK signal is true 
 * until  the end of the response packet.   
 * The VCI packet can have any length, but only READ & WRITE  
 * VCI commands are supported (NOP & READLOCK are not supported)
 * Most output ports, are Mealy signals.  
 *
 * 
 */

#ifndef VCI_AVALON_TARGET_WRAPPER_H_
#define VCI_AVALON_TARGET_WRAPPER_H_

#define DEBUG_TARGET_WRAPPER 0

#include <systemc>

#include "caba_base_module.h"
#include "avalonbus_slave.h"
#include "vci_initiator.h"


namespace soclib { namespace caba {

    using namespace sc_core;

    //template<typename vci_param>
    template<typename vci_param, typename avalon_param>
    class VciAvalonTargetWrapper 
      : public soclib::caba::BaseModule
    {

      enum fsm_state_e {
	FSM_IDLE,
	FSM_WAIT_LEC,
	FSM_ACQ_LEC,
	FSM_BURST_RDATA,	 
	FSM_WAIT_BURST_RDATA,
	FSM_LAST_BURST_RDATA,
	FSM_WAIT_FIN_LEC,
	FSM_WAIT_ECR,
	FSM_ACQ_ECR,
	FSM_BURST_WDATA,	 
	FSM_WAIT_BURST_WDATA,
	FSM_LAST_BURST_WDATA,
	FSM_WAIT_FIN_ECR,
	FSM_VAL_ACK1,
	FSM_VAL_ACK2,
// Charles
	FSM_ADV_WRITE,
	FSM_LAST_ADV_WRITE
      };
 

    protected:
      SC_HAS_PROCESS(VciAvalonTargetWrapper);

    public:
      // ports
      sc_in<bool> 				p_clk;
      sc_in<bool>   				p_resetn;

      soclib::caba::VciInitiator<vci_param>	p_vci;

      //AvalonSlave<32,32, 8>	p_avalon;
      soclib::caba::AvalonSlave<avalon_param>	p_avalon;

      VciAvalonTargetWrapper(sc_module_name	insname);

  // Charles  private:

      sc_signal<int>				r_fsm_state;
      sc_signal<int>				r_burstcount;  // compteurs de requetes courantes
      sc_signal<int>				r_burst_count; // nombre de requetes
      sc_signal<bool>				r_read;
      sc_signal<bool>				r_write;
      sc_signal<int>				r_address;
      sc_signal<int>				r_byteenable;

      // methods
      void transition();
      void genMealy();

    };

  }} // end namespace
		
#endif // VCI_AVALON_TARGET_WRAPPER_H_
