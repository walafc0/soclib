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
 * File : avalon_address_decoding_logic.h 
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_ADDRESS_DECODING_LOGIC_H_
#define SOCLIB_CABA_AVALON_ADDRESS_DECODING_LOGIC_H_

#define DEBUG_ADL 0

#include <systemc>
#include "caba_base_module.h"

#include "avalon_adl_in.h"
#include "avalon_adl_out.h"


namespace soclib { namespace caba {

    using namespace sc_core;

    template<typename avalon_param>
    class AvalonAddressDecodingLogic
      : public soclib::caba::BaseModule	

    {
      // mandatory SystemC construct
    protected:
      SC_HAS_PROCESS(AvalonAddressDecodingLogic);

    public:
      // ports
      sc_in<bool> 				p_clk;
      sc_in<bool>   				p_resetn;

      //	soclib::caba::AvalonAdlIn<32, 32, 8>   p_adl_in;
      //	soclib::caba::AvalonAdlOut<32, 32, 8>  p_adl_out;
      soclib::caba::AvalonAdlIn<avalon_param>   p_adl_in;
      soclib::caba::AvalonAdlOut<avalon_param>  p_adl_out;

      unsigned address; 

      unsigned BaseAddress;
      unsigned SpanAddress;

      // constructeur
      AvalonAddressDecodingLogic(sc_module_name insname, int Base_Address, int Span_Address);

    private:

      void transition();
      void genMealy();
    };

  }
} // end namespace

#endif // AVALON_ADDRESS_DECODING_LOGIC_H_
