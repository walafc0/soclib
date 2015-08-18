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
 * File : vci_avalon_initiator_wrapper.h
 * Date : 20/11/2008
 *
 *
 * This hardware component is a VCI/AVALON protocol converter.
 * It behaves as an initiator on the AVALON and as a target on 
 * the VCI side. It can be used by a VCI initiator to interface
 * a AVALON based system. 
 *  
 *
 * The VCI initiator is supposed to be always "ready" :   
 * When a command packet starts, the CMDVAL signal is true until 
 * the end of packet(no buble), and the RSPACK signal is true 
 * until  the end of the response packet.   
 * The VCI packet can have any length, but only READ & WRITE  
 * VCI commands are supported (NOP & READLOCK are not supported)
 * Most output ports, are Mealy signals.  
 *
 */

#ifndef VCI_AVALON_INITIATOR_WRAPPER_H_
#define VCI_AVALON_INITIATOR_WRAPPER_H_

#define DEBUG_INIT_WRAPPER 0

#include <systemc.h>

#include "caba_base_module.h"
#include "vci_target.h"
#include "avalonbus_master.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template<typename vci_param, typename avalon_param>

    class VciAvalonInitiatorWrapper 	
      : public soclib::caba::BaseModule
    {

      enum fsm_state_e {
	FSM_IDLE, // 0
	FSM_R_ACQ1, //1
	FSM_R_WAIT1, //2
	FSM_R_WAIT2, //3
	FSM_R_ACQ2, //4
	FSM_RDATA, //5
	FSM_WAIT_RDATA, //6
	FSM_LAST_RDATA, //7
		  // Charles
		  FSM_R_WAIT_ADV, //8
		  FSM_R_ACQ_ADV, //9
		  FSM_RDATA_ADV, //10
		  FSM_WAIT_RDATA_ADV,  // 11
		  FSM_LAST_RDATA_ADV // 12
	 	 
      };

      // mandatory SystemC construct
    protected:
      SC_HAS_PROCESS(VciAvalonInitiatorWrapper);

    public:
      // ports
      sc_in<bool> 				p_clk;
      sc_in<bool>   				p_resetn;


      soclib::caba::VciTarget<vci_param>	p_vci;

      //Address_Width, Data_Width, int Burstcount_Width 
      //AvalonMaster<32,32,8>	p_avalon;
      AvalonMaster<avalon_param>	p_avalon;
	
      VciAvalonInitiatorWrapper(sc_module_name	insname);

  // Charles  private:

      sc_signal<int>				r_srcid;
      sc_signal<int>				r_pktid;
      sc_signal<int>				r_trdid;
		
      sc_signal<bool>				r_read;
      sc_signal<bool>				r_write;
	  sc_signal<int>				r_address;
	  sc_signal<int>				r_byteenable;

      sc_signal<int>				r_fsm_state;
      sc_signal<int>				r_read_burstcount;  // nombre de requetes
      sc_signal<int>				r_read_burst_count; // compteur de requetes courantes
      sc_signal<int>				r_write_burstcount;
      sc_signal<int>				r_write_burst_count;

      //test
      sc_signal<int>				r_cmd;
      void transition();
      void genMealy();

    };

  }
} // end namespace
		
#endif // VCI_AVALON_INITIATOR_WRAPPER_H_
