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
 * File : avalon_mux.cpp
 * Date : 20/11/2008
 */



#include "../include/avalon_mux.h"

namespace soclib { namespace caba {

    using namespace sc_core;

#define tmpl(x) template<typename vci_param> x AvalonMux<vci_param>
    ////////////////////////////////
    //	constructor
    ////////////////////////////////
    tmpl(/**/)::AvalonMux(sc_module_name insname, int n_slave, int master_index)
	       : soclib::caba::BaseModule(insname)


    {
      //std::cout << std::endl<< std::endl<< "*************** AvalonMux constructeur " << std::endl;

      Nslave = n_slave;
      M_index = master_index;

      //std::cout << std::endl<< std::endl<< "*************** AvalonMux Mindex= " << M_index  << std::endl;


      p_mux_master      = new AvalonMux_Master[1];
      p_mux_slave       = new AvalonMux_Slave[n_slave];
      p_mux_sel_master  = new AvalonMux_Sel[n_slave];


      SC_METHOD (genMealy);
      dont_initialize();

      for (int i=0; i< Nslave; i++)
	{
	  sensitive << p_mux_slave[i].readdata;
	  sensitive << p_mux_slave[i].waitrequest;
	  sensitive << p_mux_slave[i].readdatavalid;
	}

    } //  end constructor

    ////////////////////////////////
    //	genMealy
    ////////////////////////////////
    tmpl(void)::genMealy()
    {

#if DEBUG_MUX
      std::cout << std::endl<< std::endl<< "***************{{{{{{{{{{            entree AvalonMux genMealy                  }}}}}}}}}}}}}}}}}}" << std::endl;
#endif


      int i;
      sel_slave = -1;


#if DEBUG_MUX
      std::cout << std::endl<< std::endl<< "*************** DEBUG_MUX  = " << DEBUG_MUX  << std::endl;
      std::cout << std::endl<< std::endl<< "*************** Mindex = " << M_index  << std::endl;
      for (i=0; i< Nslave; i++)
	{
	  std::cout << std::endl<< std::endl<< "*************** sel_master = " << p_mux_sel_master[i]  << std::endl;
	}
#endif


      for (i=0; i< Nslave; i++)
	{

	  if (p_mux_sel_master[i] == M_index)
	    {
	      if (sel_slave == -1) { 
		sel_slave = i;
#if DEBUG_MUX
		std::cout << std::endl<< std::endl<< "*************** MATCH " << DEBUG_MUX  << std::endl;
		
#endif	      
	      }
	      else {
		std::cout << std::endl<< "*************** MUX MASTER SELECT ERROR !  M_index =  " << M_index << "  i=  " << i<< "  sel_slave = " << sel_slave << " NSlave =   " << Nslave << std::endl;}
	    }

	  if ( sel_slave == -1 )
	    { // aucun maitre selectionne : p_mux_slave[-1]  --> incident de segmentation
	      p_mux_master[0].waitrequest    = true ;
	      p_mux_master[0].readdatavalid  = p_mux_slave[0].readdatavalid;
	      p_mux_master[0].readdata       = p_mux_slave[0].readdata;
	    }
	  else
	    {
	      p_mux_master[0].waitrequest    = p_mux_slave[sel_slave].waitrequest;
	      p_mux_master[0].readdatavalid  = p_mux_slave[sel_slave].readdatavalid;
	      p_mux_master[0].readdata       = p_mux_slave[sel_slave].readdata;
	    }


#if DEBUG_MUX
	  std::cout << std::endl<< std::endl<< "*************** sortie AvalonMux genMealy " << std::endl;
#endif
	} // end for

    }; // end genMealy

  }// end caba
} // end namespace
