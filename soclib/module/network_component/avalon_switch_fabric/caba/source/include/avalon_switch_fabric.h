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
 * File : avalon_switch_fabric.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_SWITCH_FABRIC_H_
#define SOCLIB_CABA_AVALON_SWITCH_FABRIC_H_

#define DEBUG_SWITCH 0

#include <systemc>

#include "caba_base_module.h"
#include "register.h"

#include "avalon_switch_slave.h"
#include "avalon_switch_master.h"
#include "avalon_address_decoding_logic.h"
#include "avalon_arbiter.h"
#include "avalon_switch_config.h"
#include "avalon_adl_signals.h"
#include "avalon_mux.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template<int NB_MASTER, int NB_SLAVE, typename avalon_param>
    class AvalonSwitchFabric
      : public soclib::caba::BaseModule
    {
      enum fsm_state_e {
	FSM_IDLE,
	FSM_ACQ1,
	FSM_WAIT1,
	FSM_WAIT2,
	FSM_ACQ2,
	FSM_OP2,
	FSM_ACQ3
      };


    protected:
      SC_HAS_PROCESS(AvalonSwitchFabric);

    public:

      //       typedef soclib::caba::AvalonSwitchMaster<32, 32, 8> AvalonSwitch_Master;
      //       typedef soclib::caba::AvalonSwitchSlave<32, 32, 8>  AvalonSwitch_Slave;
      //       typedef soclib::caba::AvalonAdlSignals<32, 32, 8>  AvalonAdl_Signals;
      //       typedef soclib::caba::AvalonMuxSignals<32>  AvalonMux_Signals;

      typedef soclib::caba::AvalonSwitchMaster<avalon_param> AvalonSwitch_Master;
      typedef soclib::caba::AvalonSwitchSlave<avalon_param>  AvalonSwitch_Slave;
      typedef soclib::caba::AvalonAdlSignals<avalon_param>  AvalonAdl_Signals;
      typedef soclib::caba::AvalonMuxSignals<avalon_param>  AvalonMux_Signals;


      // =============== ports ===============================================
      sc_in<bool> 				p_clk;
      sc_in<bool>   				p_resetn;
      AvalonSwitch_Master *  p_avalon_master;
      AvalonSwitch_Slave  *  p_avalon_slave;

      // constructor
      AvalonSwitchFabric(sc_module_name	insname, AvalonSwitchConfig<NB_MASTER, NB_SLAVE> config) ;

//    private:
      soclib::caba::AvalonArbiter< avalon_param> **to_arbiter;
      soclib::caba::AvalonMux< avalon_param> **to_mux;

      soclib::caba::AvalonAddressDecodingLogic< avalon_param> *adl;
      soclib::caba::AvalonArbiter< avalon_param> *arbiter;
      soclib::caba::AvalonMux< avalon_param> *mux;

      int *routage[NB_MASTER+1];
      int route;

      int master, slave;


    };

  }
} // end namespace

#endif // SOCLIB_CABA_AVALON_SWITCH_FABRIC_H_
