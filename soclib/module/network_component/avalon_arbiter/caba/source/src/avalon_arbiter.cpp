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
 * File : avalon_arbiter.cpp
 * Date : 20/11/2008
 */


#include <systemc>
#include "../include/avalon_arbiter.h"

namespace soclib { namespace caba {

    using namespace sc_core;


#define tmpl(x) template<typename vci_param> x AvalonArbiter<vci_param>
    ////////////////////////////////
    //	constructor
    ////////////////////////////////
    tmpl(/**/)::AvalonArbiter(sc_module_name insname, int *routage, int n_master, int slave_index)
	       : soclib::caba::BaseModule(insname)

    {
#if DEBUG_ARBITER
      std::cout << std::endl<< std::endl<< "*************** AvalonArbiter constructeur " << std::endl;
#endif

      Nmaster = n_master;
      S_index = slave_index;


      route   = routage;
      requete	= new sc_signal<bool>[n_master];


       p_arbiter_master  = new AvalonArbiter_Master[n_master];
		// p_arbiter_master  = new AvalonArbiter_Master[n_master+1];
      p_arbiter_slave   = new AvalonArbiter_Slave[1];


      SC_METHOD (transition);
      dont_initialize();
      sensitive << p_clk.pos();


      SC_METHOD (genMealy);
      dont_initialize();
      sensitive << req_courante;


      for (int i=0; i< n_master; i++)
	//	for (int i=0; i< n_master+1; i++)
	{
	  sensitive << p_arbiter_master[i].address;
	  sensitive << p_arbiter_master[i].read;
	  sensitive << p_arbiter_master[i].chipselect;
	  sensitive << p_arbiter_master[i].write;
	  sensitive << p_arbiter_master[i].byteenable;
	  sensitive << p_arbiter_master[i].writedata;
	  sensitive << p_arbiter_master[i].burstcount;
	  // 	  sensitive << p_arbiter_master[i].flush;
	}

    }
    //  end constructor

    ////////////////////////////////
    //	transition
    ////////////////////////////////

    tmpl(void)::transition()

    {

      int i;
#if DEBUG_ARBITER
      std::cout << " ========================================================================================          AvalonArbiter transistion " << std::endl;
#endif

      if (p_resetn == false) {
	r_fsm_state = FSM_IDLE;
	req_courante = -1;


#if DEBUG_ARBITER
	std::cout << " ========================================================================================         AvalonArbiter transistion  reset " << std::endl;
#endif

	for( i=0; i<Nmaster; i++)
	  { requete[i]=false;	}
	return;
      } // end reset

#if DEBUG_ARBITER
      std::cout << " ========================================================================================          AvalonArbiter transistion NON reset " << std::endl;
#endif


#if DEBUG_ARBITER
      std::cout << " ========================================================================================         AvalonArbiter transistion fin s_master " << std::endl;
#endif

      switch (r_fsm_state) {
      case FSM_IDLE:

#if DEBUG_ARBITER
 	std::cout << " ====================================================================    AvalonArbiter transistion  IDDLE " << std::endl;
#endif
	for ( i=0; i<Nmaster; i++)
	  {

#if DEBUG_ARBITER
	    std::cout << " ==========================================================>             AvalonArbiter READ =  " <<  p_arbiter_master[i].read << std::endl;
#endif

	    if ( ((p_arbiter_master[i].read) || (p_arbiter_master[i].write)) && (p_arbiter_master[i].chipselect) )
	      {

		requete[i] = true;
		req_courante = i;  // la derniere !
		r_fsm_state = FSM_DEBUT;
	      }
	    else	{requete[i] = false;}
	  }

	break;


      case FSM_DEBUT://=====================

#if DEBUG_ARBITER
	std::cout << " ==================================================================================================================    AvalonArbiter transistion  DEBUT " << std::endl;
#endif

	req_nouv = false;

	// requete courante
	if ( ((p_arbiter_master[(req_courante)%Nmaster].read==true) || (p_arbiter_master[(req_courante)%Nmaster].write==true)) && (p_arbiter_master[(req_courante)%Nmaster].chipselect==true) )
	  {requete[(req_courante)%Nmaster] = true;}
	else
	  {

#if DEBUG_ARBITER
	    std::cout << " =============================================================================================================    AvalonArbiter transistion  DEBUT  abandon req_courante= " << req_courante<< std::endl;
#endif

	    requete[(req_courante)%Nmaster] = false;
	    req_nouv = true;    // variable mise à jour immediate
	    req_courante = -1;  // registre mis a jour cycle suivant
	    r_fsm_state = FSM_IDLE;
	  }


	// les autres
	for ( i=1; i<Nmaster; i++)
	  {
	    if ( ((p_arbiter_master[(i+req_courante)%Nmaster].read==true) || (p_arbiter_master[(i+req_courante)%Nmaster].write==true)) && (p_arbiter_master[(i+req_courante)%Nmaster].chipselect ==true)  )// une requete
	      {
		requete[(i+req_courante)%Nmaster] = true;
		if (req_nouv==true) {
		  req_courante = (i+req_courante)%Nmaster;

#if DEBUG_ARBITER
		  std::cout << " ============================================================================================================����������������������>    AvalonArbiter transistion  DEBUT  nouvelle  req_courante= " << ((i+req_courante)%Nmaster)<< std::endl;
#endif

		  req_nouv = false ;
		  r_fsm_state = FSM_DEBUT;
		}
	      }
	    else {
	      requete[(i+req_courante)%Nmaster] = false;
	    }
	  }


#if DEBUG_ARBITER
	std::cout << " requete courante = " << req_courante << std::endl;

	for ( i=0; i<Nmaster; i++)
	  {
	    std::cout << " requete " << i << " = " << requete[i] << std::endl;
	  }
#endif


	break;

      }// end switch fsm

    }; // end transition()



    ////////////////////////////////
    //	genMealy
    ////////////////////////////////

    tmpl(void)::genMealy()


    {

      // sel_master = route[ req_courante];

#if DEBUG_ARBITER
      std::cout << "*************** AvalonArbiter entree genMealy " << std::endl;
      std::cout << "*************** sel_master =  " << 	sel_master << std::endl;
      std::cout << "*************** req_courante =  " <<        req_courante << std::endl;
      std::cout << "***************route0 =  " <<       route[0] << std::endl;
      std::cout << "***************route1 =  " <<       route[1] << std::endl;
      std::cout << "***************route2 =  " <<       route[2] << std::endl;
      std::cout << "***************route3 =  " <<       route[3] << std::endl;
      std::cout << "***************route4 =  " <<       route[4] << std::endl;


#endif

		//Charles : pas de requete : selection entree initialisation
      //if ( req_courante == -1 ){ select_master = 0;} else {select_master = req_courante ;}
      //if ( req_courante == -1 ){ sel_master = -1;} else {sel_master = route[req_courante] ;}
		
		if ( req_courante == -1 ){ select_master = Nmaster;} else {select_master = req_courante ;}
		if ( req_courante == -1 ){ sel_master = -1;} else {sel_master = route[req_courante] ;}
		
		
// 
		if ( req_courante == -1 )
		{
			p_arbiter_slave[0].chipselect = 0;	
			p_arbiter_slave[0].address    = 0;		
			p_arbiter_slave[0].read       = 0;
			p_arbiter_slave[0].write      = 0;
			p_arbiter_slave[0].writedata  = 0;
			p_arbiter_slave[0].byteenable = 0;
			p_arbiter_slave[0].burstcount = 0;
		}
		else
		{
			p_arbiter_slave[0].chipselect = p_arbiter_master[select_master].chipselect;	
			p_arbiter_slave[0].address    = p_arbiter_master[select_master].address;		
			p_arbiter_slave[0].read       = p_arbiter_master[select_master].read;
			p_arbiter_slave[0].write      = p_arbiter_master[select_master].write;
			p_arbiter_slave[0].writedata  = p_arbiter_master[select_master].writedata;
			p_arbiter_slave[0].byteenable = p_arbiter_master[select_master].byteenable;
			p_arbiter_slave[0].burstcount = p_arbiter_master[select_master].burstcount;
		}


#if DEBUG_ARBITER
      std::cout << "*************** AvalonArbiter  sortie genMealy " << std::endl;
#endif

    }; // end genMealy



  }// end caba
} // end namespace
