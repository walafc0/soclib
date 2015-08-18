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
 * File : avalonbus_slave.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_MUX_H_
#define SOCLIB_CABA_AVALON_MUX_H_

#define DEBUG_MUX 0

#include <systemc>

#include "caba_base_module.h"
#include "register.h"
#include "avalon_mux_master.h"
#include "avalon_mux_slave.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template< typename avalon_param>
    class AvalonMux
      : public soclib::caba::BaseModule {

      // mandatory SystemC construct
    protected:
      SC_HAS_PROCESS(AvalonMux);

    public:

      //	typedef soclib::caba::AvalonMuxMaster<32> AvalonMux_Master;
      //	typedef soclib::caba::AvalonMuxSlave<32>  AvalonMux_Slave;
      typedef soclib::caba::AvalonMuxMaster<avalon_param> AvalonMux_Master;
      typedef soclib::caba::AvalonMuxSlave<avalon_param>  AvalonMux_Slave;
	
      typedef sc_in<int> AvalonMux_Sel;

      // =============== ports===============================================

      AvalonMux_Master *  p_mux_master;
      AvalonMux_Slave  *  p_mux_slave;
      AvalonMux_Sel    *  p_mux_sel_master;
      // =============== ports===============================================

      // constructeur
      AvalonMux(sc_module_name insname, int n_slave, int master_index ) ;

    private:	
      int Nslave;
      int M_index;	
      int sel_slave;


      // the FSM functions
      //	void transition();
      void genMealy();
      //	void genMoore();
    };

  }
} // end namespace

#endif // AVALON_MUX_H_
