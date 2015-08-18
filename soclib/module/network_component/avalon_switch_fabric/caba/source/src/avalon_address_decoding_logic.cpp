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
 * File : avalon_address_decoding_logic.cpp
 * Date : 20/11/2008
 */

#include "../include/avalon_address_decoding_logic.h"

#include "register.h"
#include "base_module.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    ////////////////////////////////
    //	constructor
    ////////////////////////////////

#define tmpl(x) template<typename vci_param> x AvalonAddressDecodingLogic<vci_param>
    ////////////////////////////////
    //	constructor
    ////////////////////////////////
    tmpl(/**/)::AvalonAddressDecodingLogic(sc_module_name insname,  int Base_Address, int Span_Address)
	       : soclib::caba::BaseModule(insname)

    {
#if DEBUG_ADL
      std::cout << std::endl<< std::endl<< "*************** AvalonAddressDecodingLogic constructeur " << std::endl;
#endif

      SC_METHOD (transition);
      dont_initialize();
      sensitive << p_clk.pos();

      SC_METHOD (genMealy);
      dont_initialize();

      sensitive  <<  p_adl_in.address;
      sensitive  <<  p_adl_in.read;
      sensitive  <<  p_adl_in.write;
      sensitive  <<  p_adl_in.writedata;
      sensitive  <<  p_adl_in.byteenable;
//       sensitive  <<  p_adl_in.flush;
      sensitive  <<  p_adl_in.burstcount;

      BaseAddress =  Base_Address;
      SpanAddress =  Span_Address;

    }


    ////////////////////////////////
    //	transition 
    ////////////////////////////////


    tmpl(void)::transition()
	       //template<typename avalon_param>
	       //void AvalonAddressDecodingLogic< avalon_param>::transition()
    {
	
    };


    ////////////////////////////////
    //	genMealy
    ////////////////////////////////
    tmpl(void)::genMealy()

	       //template<typename avalon_param>
	       //void AvalonAddressDecodingLogic< avalon_param>::genMealy()
    {
#if DEBUG_ADL
      std::cout << "entree genMealy AvalonAddressDecodingLogic" << std::endl;
      std::cout << "Slave base address = " << std::hex <<  BaseAddress << std::endl;
#endif

      p_adl_out.address = p_adl_in.address;
      p_adl_out.read = p_adl_in.read;
      p_adl_out.write = p_adl_in.write;
      p_adl_out.writedata = p_adl_in.writedata;
      p_adl_out.byteenable = p_adl_in.byteenable;
//       p_adl_out.flush = p_adl_in.flush;
      p_adl_out.burstcount = p_adl_in.burstcount;


      address = p_adl_in.address.read();
      if ((address >= BaseAddress) && (address<=  ( BaseAddress+SpanAddress)))
	{
	  p_adl_out.chipselect = true; 

#if DEBUG_ADL
	  std::cout << "chipselect = " << true << std::endl;
#endif

	}
      else
	{ 
	  p_adl_out.chipselect = false;

#if DEBUG_ADL
	  std::cout << "chipselect = " << false << std::endl;
#endif
	}                               
    }; // end genMealy


  }// end caba
} // end namespace
