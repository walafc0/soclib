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
 * File : avalon_switch_fabric.cpp
 * Date : 20/11/2008
 */

#include "../include/avalon_switch_fabric.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    ////////////////////////////////
    //	constructor
    ////////////////////////////////

#define tmpl(x) template<int NB_MASTER, int NB_SLAVE, typename avalon_param>  x AvalonSwitchFabric<NB_MASTER, NB_SLAVE, avalon_param>
    ////////////////////////////////
    //	constructor
    ////////////////////////////////
    tmpl(/**/)::AvalonSwitchFabric(sc_module_name insname, AvalonSwitchConfig<NB_MASTER, NB_SLAVE> config)
	       : soclib::caba::BaseModule(insname)
 

    {
#if DEBUG_SWITCH
      std::cout << std::endl << std::endl << "*************** AvalonSwitchFabric constructeur " << std::endl;
#endif

      //=============   ports  ==============================================
      p_avalon_master  = new AvalonSwitch_Master[config.n_master];
      p_avalon_slave   = new AvalonSwitch_Slave[config.n_slave];	
      // ============  composants  ===========================================

      soclib::caba::AvalonAddressDecodingLogic< avalon_param> **to_adl[config.n_slave];	

      to_mux = new AvalonMux< avalon_param>*[config.n_master];
      to_arbiter = new AvalonArbiter< avalon_param>*[config.n_slave];

      for ( slave = 0; slave < config.n_slave; slave++)
	{	
#if DEBUG_SWITCH
	  std::cout << std::endl<< std::endl<< " =========================ARBITRE ============================================================= " <<  std::endl;
#endif

	  arbiter = new AvalonArbiter< avalon_param>("Arbiter", config.SwitchFabricParam_Slave[slave]->route, config.SwitchFabricParam_Slave[slave]->arbiter_n_master, slave);
	  to_arbiter[slave] = arbiter; 	
	}

      for ( master = 0; master < config.n_master; master++)
	{

#if DEBUG_SWITCH
	  std::cout << std::endl<< std::endl<< " =========================MUX============================================================= " <<  std::endl;		
#endif

	  mux = new AvalonMux< avalon_param>("Mux", config.SwitchFabricParam_Master[master]->mux_n_slave, master);
	  to_mux[master] = mux; 	

#if DEBUG_SWITCH
	  std::cout << std::endl << "*************** CREATION MUX  " << mux <<  std::endl;	
#endif
	}


      // ============  bus ===================================================
      AvalonAdl_Signals  *adl_arbiter_bus[config.n_slave];	

      // adl vers arbitre
      for (int i = 0; i < config.n_slave; i++)	
	{
	  to_adl[i]  = new AvalonAddressDecodingLogic< avalon_param>*[config.n_master];
	  adl_arbiter_bus[i] = new AvalonAdl_Signals[config.n_master];
	}

      sc_signal<int>     *arbiter_mux_bus[config.n_master];
      //arbitre vers mux
      for (int i = 0; i < config.n_slave; i++)
	{
	  arbiter_mux_bus[i] = new sc_signal<int>;
	}


#if DEBUG_SWITCH
      std::cout << std::endl << "*************** TEST  " << std::endl;
      for (int i=0; i<config.n_master; i++)
	{
	  for (int j=0; j<config.n_slave; j++)
	    {
	      std::cout << "         master  " <<  i  <<  "        slave  " <<config.SwitchFabricParam_Master[i]->route[j]  << std::endl;
	    }  	
	}	
      std::cout << std::endl<< std::endl<< " =========================ADDRESS DECODING LOGIC============================================================= " <<  std::endl;	
#endif


      //=========================ADL=============================================================	
      for (master = 0; master < config.n_master; master++)
	{
	  for ( int i = 0; i < config.n_slave; i++)
	    {
	      slave = config.SwitchFabricParam_Master[master]->route[i];
	      if (slave != -1)
		{
		  //********** instance		
		  adl = new AvalonAddressDecodingLogic< avalon_param>("ADL", config.SwitchFabricParam_Slave[slave]->Base_Address, config.SwitchFabricParam_Slave[slave]->Address_Span);
		  to_adl[slave][master] = adl;		

#if DEBUG_SWITCH
		  std::cout << " CONNEXIONS ADL  master "  << master  <<" slave " <<slave  <<  " i= " << i << " :  " << adl << std::endl;
#endif

		  //std::cout << " CONNEXIONS  ADL   "<< std::endl;
		  (*adl).p_clk(p_clk);				 
		  (*adl).p_resetn(p_resetn);
		  //************* connexions ports ADL******************************

		  // voir ports in et out en attente (clock, resetrequest                
		  (*adl).p_adl_in(p_avalon_master[master]);

#if DEBUG_SWITCH
		  std::cout << " FIN CONNEXIONS  IN  ADL(master "  << master << "  to slave "<<slave  << ")    : clk, resetn, p_avalon_master  "<< std::endl<< std::endl<< std::endl;                
#endif
		}


	      else { 
#if DEBUG_SWITCH
		std::cout << " pas de ADL pour master"  << master << "  to slave "<<slave  << std::endl; 
#endif				
		break;} 

	    }
	}

      //=========================connexion ARBITER=============================================================	
      for ( slave = 0; slave < config.n_slave; slave++)
	{
	  arbiter = to_arbiter[slave];

#if DEBUG_SWITCH
	  std::cout << " CONNEXIONS  ARBITER " << arbiter << "   slave "<< slave << " avec "<< config.SwitchFabricParam_Slave[slave]->arbiter_n_master <<" master"<< std::endl;		 
	  std::cout << " CONNEXIONS  " <<  config.SwitchFabricParam_Slave[slave] << std::endl;	
	  std::cout << " CONNEXIONS  " <<  config.SwitchFabricParam_Slave[slave]->route << std::endl;
	  std::cout << " CONNEXIONS  " <<  config.SwitchFabricParam_Slave[slave]->route[0] << std::endl;
	  *routage = config.SwitchFabricParam_Slave[slave]->route;
	  route = *routage[0];
	  std::cout << " CONNEXIONS  " <<  "no master"  <<  route<<  std::endl;
#endif

	  //************* connexions ports ARBITER******************************
	  (*arbiter).p_clk(p_clk);
	  (*arbiter).p_resetn(p_resetn);			 

	  // connexions ARBITER - port SWITCH SLAVE
	  (*arbiter).p_arbiter_slave[0](p_avalon_slave[slave]); // 1 seul port slav			 
	  // connexions ADL - ARBITER			

	  // connexions arbiter - MUX
	  (*arbiter).sel_master(*arbiter_mux_bus[slave]);

	  for (int i = 0; i< config.SwitchFabricParam_Slave[slave]->arbiter_n_master; i++)
	    {
	      master = config.SwitchFabricParam_Slave[slave]->route[i];
	      if (master != -1)
		{

#if DEBUG_SWITCH
		  std::cout << " CONNEXIONS  port master  =  " <<  master << " slave =  " << slave << "  i=   " << i << " de adl " << to_adl[slave][master] << std::endl;			 		
#endif

		  (*arbiter).p_arbiter_master[i](adl_arbiter_bus[slave][master]);	
		  (*to_adl[slave][master]).p_adl_out(adl_arbiter_bus[slave][master]);	
		}		 		


	      else {
#if DEBUG_SWITCH
		std::cout << " CONNEXIONS  ARBITER  : erreur n_master !! " << std::endl;
#endif				
	      }


	    }
#if DEBUG_SWITCH
	  std::cout << " FIN CONNEXIONS  ARBITER   "<< std::endl;
#endif
	}


      //=========================MUX=============================================================	
      for ( master = 0; master < config.n_master; master++)
	{

#if DEBUG_SWITCH
	  std::cout << std::endl<< std::endl<< " =========================connexions MUX============================================================= " <<  std::endl;
#endif

	  //********** instance			
	  mux = to_mux[master];

#if DEBUG_SWITCH
	  std::cout << " CONNEXIONS  MUX " << mux << " master "<< master << " avec "<< config.SwitchFabricParam_Master[master]->mux_n_slave <<" slave"<< std::endl;		 
#endif

	  //************* connexions ports MUX****************************

	  // connexions MUX - port SWITCH MASTER		 
	  (*mux).p_mux_master[0].readdata(p_avalon_master[master].readdata);	 
	  (*mux).p_mux_master[0].readdatavalid(p_avalon_master[master].readdatavalid);
	  (*mux).p_mux_master[0].waitrequest(p_avalon_master[master].waitrequest);

	  // connexions SWITCH SLAVE- MUX			
	  for (int i = 0; i< config.SwitchFabricParam_Master[master]->mux_n_slave; i++)
	    {
	      slave = config.SwitchFabricParam_Master[master]->route[i];
	      arbiter = to_arbiter[slave]; 


	      if (slave != -1)
		{
		  //arbiter = to_arbiter[slave]; 

#if DEBUG_SWITCH
		  std::cout << " CONNEXIONS  MUX  port slave  =  " <<  slave << " arbiter = " << arbiter << " (i = "  << i <<  "    de mux  " << mux << std::endl;			 		
#endif

		  (*mux).p_mux_slave[i].readdata(p_avalon_slave[slave].readdata);	 
		  (*mux).p_mux_slave[i].readdatavalid(p_avalon_slave[slave].readdatavalid);
		  (*mux).p_mux_slave[i].waitrequest(p_avalon_slave[slave].waitrequest);

		  //arbiter->sel_slave(arbiter_mux_bus[master][i]);
		  (*mux).p_mux_sel_master[i](*arbiter_mux_bus[slave]);
		}
	      else {

#if DEBUG_SWITCH				
		std::cout << " CONNEXIONS  MUX  : erreur n_slave !! " << std::endl;
#endif				
	      }
	    }	

#if DEBUG_SWITCH
	  std::cout << " FIN CONNEXIONS  MUX   "<< std::endl;
#endif
	}

      // ----------------------------------fin MUX----------------------------------------------------$


    } //  end constructor


  }// end caba
} // end namespace
