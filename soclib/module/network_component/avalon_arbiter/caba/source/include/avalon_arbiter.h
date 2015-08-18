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
 * File : avalon_arbiter.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_ARBITER_H_
#define SOCLIB_CABA_AVALON_ARBITER_H_

#define DEBUG_ARBITER 0

#include <systemc>

#include "caba_base_module.h"
#include "register.h"

#include "avalon_arbiter_master.h"
#include "avalon_arbiter_slave.h"
#include "avalon_arbiter_signals.h"
#include "avalon_switch_fabric_param_slave.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template< typename avalon_param>
    class AvalonArbiter	
      : public soclib::caba::BaseModule
    {
      enum fsm_state_e {
	FSM_IDLE,
	FSM_DEBUT,
	FSM_SUITE,
	FSM_FIN
      };

      // mandatory SystemC construct
    protected:
      SC_HAS_PROCESS(AvalonArbiter);

    public:

      typedef soclib::caba::AvalonArbiterMaster< avalon_param> AvalonArbiter_Master;
      typedef soclib::caba::AvalonArbiterSlave<avalon_param>  AvalonArbiter_Slave;
      typedef soclib::caba::AvalonArbiterSignals<avalon_param>  AvalonArbiter_Signals;

      // =============== ports===============================================
      sc_in<bool> 				p_clk;
      sc_in<bool>   				p_resetn;


      AvalonArbiter_Master *  p_arbiter_master;
      AvalonArbiter_Slave  *  p_arbiter_slave;

      // parametrer template en accord avec avalon_mux_master	
      sc_out<int> 	sel_master;


      // =============== ports===============================================

      // constructeur
      AvalonArbiter(sc_module_name insname,  int *routage, int n_master, int slave_index ) ;
	

    public :
      sc_signal<bool> * requete;
      sc_signal<int>	req_courante; //registre
      bool req_nouv;                // pas de memorisation	
      sc_signal<int>				r_fsm_state;

      sc_signal<sc_dt::sc_uint<32> > data;
      sc_signal<sc_dt::sc_uint<32> > address;
      int select_master;
      int S_index;

      int *route; 

    private:	
      int Nmaster;
	

      // the FSM functions
      void transition();
      void genMealy();
      void genMoore();
    };

  }
} // end namespace

#endif // AVALON_ARBITER_H_
