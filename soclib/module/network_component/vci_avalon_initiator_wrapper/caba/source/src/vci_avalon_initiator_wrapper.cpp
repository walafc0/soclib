/* -*- c++ -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as publishedﬁƒ
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
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
 * 	   
 * Wagner <wagner@irisa.fr>
 * 
 * Maintainer: wagner
 * 
 * File : vci_avalon_initiator_wrapper.cpp
 * Date : 20/11/2008
 *   
 * Date : 20/10/2009 
 *  ajout VCI advanced read 
 *
 * protocole VCI :  
 *			advanced read (plen > 4 et eop true)   et advanced write, avec attente de la reponse avant nouvelle requete
 *  		requete : validée par cmdack si rspval
 *          paquet : fin si reop
 * 			adresse : mise a jour dans le paquet, pointe sur le byte de poids faible
 * 
 * protocole AVALON :
 * 			waitrequest : commande executée
 *  		lecture : readdatvalid : donnee disponible
 * 			ecriture: waitrequest
 * 
 *  
 *	- burstcount = nombre de transferts Avalon ( >= 1,  1=non burst transfert, constant durant transfert)
 *	- plen = nombre de bytes du paquets
 * 
 *
 * 
 * */


#include "vci_avalon_initiator_wrapper.h"
#include "register.h"

namespace soclib { namespace caba {


#define tmpl(x) template<typename vci_param, typename avalon_param>  x VciAvalonInitiatorWrapper< vci_param, avalon_param>
    ////////////////////////////////
    //	constructor
    ////////////////////////////////
    tmpl(/**/)::VciAvalonInitiatorWrapper (sc_module_name insname)
	       : soclib::caba::BaseModule(insname)
    {


      SC_METHOD (transition);
      dont_initialize();
      sensitive << p_clk.pos();

      SC_METHOD (genMealy);
      dont_initialize();
      sensitive  << p_clk.neg();

      sensitive  << p_vci.address;
      sensitive  << p_vci.cmd;
      sensitive  << p_vci.cmdval;
      sensitive  << p_vci.wdata;
      sensitive  << p_vci.be;
      sensitive  << p_vci.plen;
      sensitive  << p_vci.eop;

      sensitive  << p_avalon.waitrequest;
      sensitive  << p_avalon.readdata;
      sensitive  << p_avalon.readdatavalid;


      // registres
      SOCLIB_REG_RENAME(r_fsm_state);
      SOCLIB_REG_RENAME(r_read_burstcount);
		
	// Charles
      SOCLIB_REG_RENAME(r_read_burst_count);
      
		SOCLIB_REG_RENAME(r_write_burstcount);
		
		// Charles
      SOCLIB_REG_RENAME(r_write_burst_count);

      SOCLIB_REG_RENAME(r_srcid);
      SOCLIB_REG_RENAME(r_pktid);
      SOCLIB_REG_RENAME(r_trdid);

      SOCLIB_REG_RENAME(r_read);
      SOCLIB_REG_RENAME(r_write);
		// Charles
		SOCLIB_REG_RENAME(r_byteenable);
		SOCLIB_REG_RENAME(r_address);
		
		
    } //  end constructor


    ////////////////////////////////
    //	transition
    ///////////////////////////////


    tmpl(void)::transition()

    {
#if DEBUG_INIT_WRAPPER
      std::cout << "=====================================================================================================>   entree transition  vci_avalon_initiator" << std::endl;
#endif

      if (p_resetn == false) {

#if DEBUG_INIT_WRAPPER
	std::cout << "RESET : entree transition  FSM_IDLE" << std::endl;
#endif

	r_fsm_state = FSM_IDLE;
	r_read = false;
	r_write = false;
	r_address = 0;
	r_byteenable = 0;
	r_read_burstcount = 0;
	r_write_burstcount = 0;

	return;
      } // end reset


      switch (r_fsm_state) {

      case FSM_IDLE: //=================
			  
			  			  

#if DEBUG_INIT_WRAPPER
	std::cout << "################################################################################################  transition  FSM_IDLE" << std::endl;
	std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
	std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
	std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
	std::cout << "################################################################################################      p_vci.address = " <<   std::hex <<p_vci.address << std::endl;
	std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
#endif

			  r_read = false;
			  r_write = false;

	if (p_vci.plen.read()%vci_param::B != 0) // B : cellsize
	  {
	    r_read_burstcount  = (p_vci.plen.read()/vci_param::B) +1;
	    r_write_burstcount = (p_vci.plen.read()/vci_param::B) +1;
	  }
	else
	  {
	    r_read_burstcount  = p_vci.plen.read()/vci_param::B;
	    r_write_burstcount = p_vci.plen.read()/vci_param::B;
	  }


	//LECTURE
	if ((p_vci.cmdval) && (p_vci.cmd.read() == vci_param::CMD_READ))
	  {
	    r_cmd = p_vci.cmd.read();	
		  
		  if ((p_vci.eop) && (p_vci.plen.read()  <= vci_param::B  ) && (!p_avalon.waitrequest))			  
	      {// un transfert

#if DEBUG_INIT_WRAPPER
		std::cout << "entree transition  FSM_IDLE vers R_ACQ1" << std::endl;
#endif
	       	r_fsm_state  = FSM_R_ACQ1;
	      }

	 
		  
		   if ((!p_vci.eop) && (p_vci.plen.read() > vci_param::B) && (!p_avalon.waitrequest))		  
	      {// paquet

#if DEBUG_INIT_WRAPPER
		std::cout << "entree transition  FSM_IDLE vers R_ACQ2" << std::endl;
#endif
	       	r_fsm_state  = FSM_R_ACQ2;
	      }

		if ((p_vci.eop) && (p_vci.plen.read() > 1) && (!p_avalon.waitrequest)) 
		  {//ADVCI advanced read
			  
#if DEBUG_INIT_WRAPPER
			  std::cout << "entree transition  FSM_IDLE vers R_ACQ_ADV" << std::endl;
#endif
			  r_address  = p_vci.address.read();
			  r_fsm_state  = FSM_R_ACQ_ADV;
	      }
		  
		  
				  if ((p_vci.eop) && (p_vci.plen.read() <= vci_param::B ) &&  (p_avalon.waitrequest))
		  
	      {
#if DEBUG_INIT_WRAPPER
		std::cout << "entree transition  FSM_IDLE vers R_WAIT1" << std::endl;
#endif
	       	r_fsm_state  = FSM_R_WAIT1;
	      }

	  
		  if ((!p_vci.eop) && (p_vci.plen.read() > vci_param::B) &&  (p_avalon.waitrequest))		  
		  
	      { // paquet
#if DEBUG_INIT_WRAPPER
		std::cout << "entree transition  FSM_IDLE vers R_WAIT2" << std::endl;
#endif
	       	r_fsm_state = FSM_R_WAIT2;
	      }
		  
		  if ((p_vci.eop) && (p_vci.plen.read() > 1) &&  (p_avalon.waitrequest))		  
			  
	      { // ADVCI advanced read
#if DEBUG_INIT_WRAPPER
			  std::cout << "entree transition  FSM_IDLE vers R_WAIT_ADV" << std::endl;
#endif
			  r_address  = p_vci.address.read();
			  r_fsm_state = FSM_R_WAIT_ADV;
	      }
		  
	  } // fin LECTURE
	r_cmd = p_vci.cmd.read();


	//ECRITURE

	if ((p_vci.cmdval) && (p_vci.cmd.read() == vci_param::CMD_WRITE))  // ecriture
	  {
	    if ((p_vci.eop) && (!p_avalon.waitrequest))
	      {// un transfert

#if DEBUG_INIT_WRAPPER
		std::cout << "entree transition  FSM_IDLE vers R_ACQ1" << std::endl;
#endif

	       	r_fsm_state  = FSM_R_ACQ1;
	      }

	    if ((p_vci.eop) &&  (p_avalon.waitrequest))
	      {

#if DEBUG_INIT_WRAPPER
		std::cout << "entree transition  FSM_IDLE vers WAIT_R_WAIT1" << std::endl;
#endif

	       	r_fsm_state  = FSM_R_WAIT1;
	      }

	    if ((!p_vci.eop) && (!p_avalon.waitrequest))
	      {// paquet

#if DEBUG_INIT_WRAPPER
		std::cout << "entree transition  FSM_IDLE vers RDATA" << std::endl;
#endif

	       	r_fsm_state  = FSM_RDATA;
	      }


	    if ((!p_vci.eop) &&  (p_avalon.waitrequest))
	      {

#if DEBUG_INIT_WRAPPER
		std::cout << "entree transition  FSM_IDLE vers WAIT_RDATA" << std::endl;
#endif

	       	r_fsm_state = FSM_WAIT_RDATA;
	      }

	  } // fin ECRITURE


	r_srcid = (int)p_vci.srcid.read();
	r_pktid = (int)p_vci.pktid.read();
	r_trdid = (int)p_vci.trdid.read();

	break;  //=========fin FSM_IDDLE


	//++++++++++++++++++++++++++++++++++++++++++++++++++   1 cellule

      case FSM_R_ACQ1: //=================

#if DEBUG_INIT_WRAPPER
	std::cout << "################################################################################################  transition  FSM_R_ACQ1" << std::endl;
	std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
	std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
	std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
	std::cout << "################################################################################################      p_vci.address = " <<   std::hex <<p_vci.address << std::endl;
#endif

	//LECTURE
	if  ((p_avalon.readdatavalid) &&(r_cmd == vci_param::CMD_READ)) {r_fsm_state = FSM_IDLE;}

	//ECRITURE
	if  (r_cmd == vci_param::CMD_WRITE) {r_fsm_state = FSM_IDLE;}
	break;  //=========fin FSM_R_ACQ1


      case FSM_R_WAIT1: //=================
	// waitrequest : commande prise en compte;
	// readdatavalid : donnee disponible
#if DEBUG_INIT_WRAPPER
	std::cout << "################################################################################################  transition  FSM_R_WAIT1" << std::endl;
	std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
	std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
	std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
	std::cout << "################################################################################################      p_vci.address = " <<   std::hex <<p_vci.address << std::endl;
#endif

	//LECTURE

	if ((!(p_avalon.waitrequest)) && (r_cmd == vci_param::CMD_READ))
	  {
	    if (p_avalon.readdatavalid) r_fsm_state = FSM_IDLE;
	    else r_fsm_state = FSM_R_ACQ1;
	  }
	//ECRITURE
	if ((!(p_avalon.waitrequest)) && (r_cmd == vci_param::CMD_WRITE))
	 // Charles  { r_fsm_state = FSM_R_ACQ1;}
		{ r_fsm_state = FSM_IDLE;}


	break; //=========fin FSM_R_WAIT1



	//+++++++++++++++++++++++++++++++++++++++++++   paquet

      case FSM_R_WAIT2 : //=================
	// attente traitement  cellule

#if DEBUG_INIT_WRAPPER
 	std::cout << "################################################################################################  transition  FSM_R_WAIT2" << std::endl;
 	std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
	std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
	std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
	std::cout << "################################################################################################      p_vci.address = " <<   std::hex <<p_vci.address << std::endl;
	std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
	std::cout << "===================================================================================>   entree transition  FSM_WAIT2" << std::endl;
#endif

	//LECTURE

	if (r_cmd == vci_param::CMD_READ)
	{
	    if (p_vci.plen.read()%vci_param::B != 0) {r_read_burstcount = (p_vci.plen.read()/vci_param::B) +1;}
	    else {r_read_burstcount = p_vci.plen.read()/vci_param::B;}

	    if (!(p_avalon.waitrequest))
		{
			if (p_avalon.readdatavalid) 
			{
			r_fsm_state = FSM_RDATA;
			r_read_burst_count = r_read_burst_count + 1;			   
			}
			else {r_fsm_state = FSM_WAIT_RDATA;}
		}

	    if (p_vci.cmdval == false)
		{
		printf("ERROR  : cmdval 5 : The vci_avalon_initiator_wrapper assumes that\n");
		printf("there s no \"buble\" in a VCI command packet\n");
		exit(1);
		}
	}


	if (p_vci.cmdval == false)
	{
	    printf("ERROR  : cmdval 5 : The vci_avalon_initiator_wrapper assumes that\n");
	    printf("there s no \"buble\" in a VCI command packet\n");
	    exit(1);
	}

	break; //=========fin FSM_R_WAIT2

			  
			  
		  case FSM_R_WAIT_ADV : //=================
			  // attente traitement  cellule
			  
#if DEBUG_INIT_WRAPPER
			  std::cout << "################################################################################################  transition  FSM_R_WAIT_ADV" << std::endl;
			  std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
			  std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
			  std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
			  std::cout << "################################################################################################      p_vci.address = " <<   std::hex <<p_vci.address << std::endl;
			  std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
			  std::cout << "===================================================================================>   entree transition  FSM_WAIT_ADV" << std::endl;
#endif
			  
			  //LECTURE
			  
			  if (r_cmd == vci_param::CMD_READ)
			  {
				  if (p_vci.plen.read()%vci_param::B != 0) {r_read_burstcount = (p_vci.plen.read()/vci_param::B) +1;}
				  else {r_read_burstcount = p_vci.plen.read()/vci_param::B;}
				  if (!(p_avalon.waitrequest))
				  {
						  if (p_avalon.readdatavalid)  
						  // Charles
						  //if (p_vci.plen.read()%vci_param::B == 0 ) { r_fsm_state = FSM_LAST_RDATA_ADV;}
						  {
							  if (p_vci.plen.read() == 4 ) { r_fsm_state = FSM_LAST_RDATA_ADV;}
							  else {r_fsm_state = FSM_RDATA_ADV;} 
	
							  //Charles	   
							  r_read_burst_count = r_read_burst_count + 1;
						  }
				  else {r_fsm_state = FSM_WAIT_RDATA_ADV;}	
				  }
			  }
	
			  r_address  = p_vci.address.read();
			  r_byteenable = p_vci.be.read();
			  if (r_cmd == vci_param::CMD_READ) r_read = true; r_write = false;
			  if (r_cmd == vci_param::CMD_WRITE) r_read = false; r_write = true;
			  if (r_cmd == vci_param::CMD_NOP) r_read = false; r_write = false;
				  
	    
			  break; //=========fin FSM_R_WAIT_ADV
			  
			  
			  
			  
			  

			  

      case FSM_R_ACQ2 : //=================
	// traitement cellule

	//LECTURE
	if ((r_cmd == vci_param::CMD_READ))
	{
	    if (p_vci.plen.read()%vci_param::B != 0) {r_read_burstcount = (p_vci.plen.read()/vci_param::B) +1;}
	    else {r_read_burstcount = p_vci.plen.read()/vci_param::B;}

	    if (p_avalon.readdatavalid == false) {r_fsm_state = FSM_WAIT_RDATA;}
	    else   
		{
			r_fsm_state = FSM_RDATA;
			r_read_burst_count = r_read_burst_count + 1;
		}
	}


	if (p_vci.cmdval == false)
	  {
	    printf("ERROR : cmdval  6 : The vci_avalon_initiator_wrapper assumes that\n");
	    printf("there s no \"buble\" in a VCI command packet\n");
	    exit(1);
	  }

	break; //=========fin FSM_R_ACQ2

				  
						  
		case FSM_R_ACQ_ADV : //=================
				  // traitement cellule
				  
				  //LECTURE
				  if ((r_cmd == vci_param::CMD_READ))
				  {
					  if (p_vci.plen.read()%vci_param::B != 0) {r_read_burstcount = (p_vci.plen.read()/vci_param::B) +1;}
					  else {r_read_burstcount = p_vci.plen.read()/vci_param::B;}
					  
					  if (p_avalon.readdatavalid == false) {r_fsm_state = FSM_WAIT_RDATA_ADV;}
					  else  
					  {
						  if (p_vci.plen.read() == vci_param::B ) { r_fsm_state = FSM_LAST_RDATA_ADV;}						  
						  else {r_fsm_state = FSM_RDATA_ADV; }
						  r_read_burst_count = r_read_burst_count + 1;						  
					  }
				  }
					  
			  r_address  = p_vci.address.read();
			  r_byteenable = p_vci.be.read();
			  if (r_cmd == vci_param::CMD_READ) r_read = true; r_write = false;
			  if (r_cmd == vci_param::CMD_WRITE) r_read = false; r_write = true;
			  if (r_cmd == vci_param::CMD_NOP) r_read = false; r_write = false;
			  
			  
				  
				  break; //=========fin FSM_R_ACQ_ADV			  


      case FSM_RDATA : //=================

#if DEBUG_INIT_WRAPPER
	std::cout << "################################################################################################  transition  FSM_RDATA" << std::endl;
	std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
	std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
	std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
	//std::cout << "################################################################################################      p_vci.address = " <<  std::hex << p_vci.address << std::endl;
	//std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
	std::cout << "################################################################################################      p_vci.eop = " <<  std::hex << p_vci.eop << std::endl;
	std::cout << "################################################################################################      p_vci.cmdack= " <<  std::hex << p_vci.cmdack<< std::endl;
	std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
#endif


	//LECTURE
	if (r_cmd == vci_param::CMD_READ)
	{
	    if (!p_avalon.readdatavalid ){r_fsm_state = FSM_WAIT_RDATA;}
	    else
		{
		r_read_burst_count = r_read_burst_count + 1;
		if (r_read_burst_count== r_read_burstcount)	r_fsm_state = FSM_LAST_RDATA;

#if DEBUG_INIT_WRAPPER
		  //std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
		  std::cout << "################################################################################################      on va aller a FSM_LAST_RDATA;" << std::endl;
#endif
		}
	}

	//ECRITURE
	if (r_cmd == vci_param::CMD_WRITE)
	{
	    if (p_avalon.waitrequest ) {r_fsm_state = FSM_WAIT_RDATA;}
	    else
		{
		//r_write_burst_count = r_write_burst_count + 1;
		if (p_vci.eop)
		{
		    r_fsm_state = FSM_LAST_RDATA;

#if DEBUG_INIT_WRAPPER
		    //std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
		    std::cout << "################################################################################################      on va aller a FSM_LAST_RDATA;" << std::endl;
#endif
		}
		}
	}


 	if (p_vci.rspack == false)
	  {
	    printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
	    printf("the VCI initiator always accept the response packet\n");
	    exit(1);
	  }

	if (p_vci.cmdval == false)
	  {
	    printf("ERROR : cmdval 1 :The vci_avalon_initiator_wrapper assumes that\n");
	    printf("there s no \"buble\" in a VCI command packet\n");
	  }

	break; //=========fin FSM_RDATA


				 
				  
			  case FSM_RDATA_ADV : //=================
				  
#if DEBUG_INIT_WRAPPER
				  std::cout << "################################################################################################  transition  FSM_RDATA_ADV" << std::endl;
				  std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
				  std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
				  std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
				  //std::cout << "################################################################################################      p_vci.address = " <<  std::hex << p_vci.address << std::endl;
				  //std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
				  std::cout << "################################################################################################      p_vci.eop = " <<  std::hex << p_vci.eop << std::endl;
				  std::cout << "################################################################################################      p_vci.cmdack= " <<  std::hex << p_vci.cmdack<< std::endl;
				  std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
#endif
				  
				  
				  //LECTURE
				  if (r_cmd == vci_param::CMD_READ)
				  {
					  if (!p_avalon.readdatavalid ){r_fsm_state = FSM_WAIT_RDATA_ADV;}
					  else
					  {	  r_read_burst_count = r_read_burst_count + 1;
						  if (r_read_burst_count== r_read_burstcount - 1)	r_fsm_state = FSM_LAST_RDATA_ADV;
							  
#if DEBUG_INIT_WRAPPER
							  //std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
							  std::cout << "################################################################################################      on va aller a FSM_LAST_RDATA_ADV;" << std::endl;
#endif
					  }
				  }
				  
				 				  
				  
				  if (p_vci.rspack == false)
				  {
					  printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
					  printf("the VCI initiator always accept the response packet\n");
					  exit(1);
				  }
				  
			 
			  
				  break; //=========fin FSM_RDATA_ADV
				  
				  
				  
				  
			  case FSM_WAIT_RDATA : //=================
				  // traitement cellule
				  
#if DEBUG_INIT_WRAPPER
				  std::cout << "################################################################################################  transition  FSM_WAIT_RDATA" << std::endl;
				  std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
				  std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
				  std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
				  std::cout << "################################################################################################      p_vci.address = " <<  std::hex << p_vci.address << std::endl;
				  std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
#endif
				  
				  
				  //LECTURE
				  if (r_cmd == vci_param::CMD_READ)
				  {
					  if (p_avalon.readdatavalid == true)
					  {
						  if (p_vci.eop){r_fsm_state = FSM_LAST_RDATA;}
						  else  {r_fsm_state = FSM_RDATA; }
					  }
					  if (p_vci.rspack == false) {
						  printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
						  printf("the VCI initiator always accept the response packet\n");
						  exit(1);
					  }
				  }
				  
				  
				  
				  //ECRITURE
				  if (r_cmd == vci_param::CMD_WRITE)
				  {
					  if (p_avalon.waitrequest == false)
					  {
						  //r_write_burst_count = r_write_burst_count + 1;
						  if (p_vci.eop)
						  {r_fsm_state = FSM_LAST_RDATA;}
						  else    {r_fsm_state = FSM_RDATA; }
					  }
				  }
				  
				  
				  if (p_vci.rspack == false)
				  {
					  printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
					  printf("the VCI initiator always accept the response packet\n");
					  exit(1);
				  }
				  
				  if (p_vci.cmdval == false)
				  {
					  printf("ERROR : cmdval 2 : The vci_avalon_initiator_wrapper assumes that\n");
					  printf("there s no \"buble\" in a VCI command packet\n");
					  exit(1);
				  }

	break; //=========fin FSM_WAIT_RDATA

				  
				  
				  

      case FSM_WAIT_RDATA_ADV : //=================
	// traitement cellule

#if DEBUG_INIT_WRAPPER
	std::cout << "################################################################################################  transition  FSM_WAIT_RDATA_ADV" << std::endl;
	std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
	std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
	std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
	std::cout << "################################################################################################      p_vci.address = " <<  std::hex << p_vci.address << std::endl;
	std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
#endif


	//LECTURE
	if (r_cmd == vci_param::CMD_READ)
	  {
	    if (p_avalon.readdatavalid == true)
		{
			   
	    // Charles
			r_read_burst_count = r_read_burst_count + 1;
			   
			   
		// Charles  if (p_vci.eop){r_fsm_state = FSM_LAST_RDATA;}
			  if (r_read_burst_count== r_read_burstcount - 1){r_fsm_state = FSM_LAST_RDATA_ADV;}			   
			   else  {r_fsm_state = FSM_RDATA_ADV; }
	      }
		  
	    if (p_vci.rspack == false) 
		{
	      printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
	      printf("the VCI initiator always accept the response packet\n");
	      exit(1);
	    }
		  
	  }

	break; //=========fin FSM_WAIT_RDATA_ADV

				  
				  
				  


      case FSM_LAST_RDATA : //=================
	// derniere cellule du paquet

#if DEBUG_INIT_WRAPPER
	std::cout << "################################################################################################  transition  FSM_LAST_RDATA" << std::endl;
	std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
	std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
	std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
	std::cout << "################################################################################################      p_vci.address = " <<   std::hex <<p_vci.address << std::endl;
	std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
#endif

	// Charles
	r_read_burst_count = 0;
	
			   
	//r_write_burst_count = 0;
 	if (p_vci.rspack == false)
	  {
	    printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
	    printf("the VCI initiator always accept the response packet\n");
	    exit(1);
	  }

				
			  // Charlesif (p_avalon.waitrequest == true ) r_fsm_state = FSM_IDLE;  // advanced write  attente rspval target qui reset waitrequestÒ
			  r_fsm_state = FSM_IDLE;
			  
			  
			  
	break; //=========fin FSM_LAST_RDATAÒ
				  
		  
				  
	case FSM_LAST_RDATA_ADV : //=================
				  // derniere cellule du paquet
				  
#if DEBUG_INIT_WRAPPER
				  std::cout << "################################################################################################  transition  FSM_LAST_RDATA_ADV" << std::endl;
				  std::cout << "################################################################################################      p_vci.eop = " <<  p_vci.eop << std::endl;
				  std::cout << "################################################################################################      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
				  std::cout << "################################################################################################      p_vci.cmd = " <<  p_vci.cmd << std::endl;
				  std::cout << "################################################################################################      p_vci.address = " <<   std::hex <<p_vci.address << std::endl;
				  std::cout << "################################################################################################      FSM = " <<  std::hex << r_fsm_state<< std::endl;
#endif

		r_read_burst_count = 0;
	
		if (p_vci.rspack == false)
		{
			printf("ERROR : The vci_pi_initiator_wrapper assumes that\n");
			printf("the VCI initiator always accept the response packet\n");
			exit(1);
		}
				  
		r_fsm_state = FSM_IDLE;
			
	break; //=========fin FSM_LAST_RDATA_ADV			  

			  }// end switch fsm
	  }; // end transition


    ////////////////////////////////
    //	genMealy
    ////////////////////////////////
    tmpl(void)::genMealy()

    {
#if DEBUG_INIT_WRAPPER
      std::cout << "=====================================================================================================entree genMealy vci_avalon_initiator" << std::endl;
#endif
      /*
	p_vci.rsrcid = r_srcid.read(); // rebouclage
	p_vci.rtrdid = r_trdid.read();
	p_vci.rpktid = r_pktid.read();
      */
      p_vci.rsrcid = 0; // connection d'un AVCI initiator a un BVCI target
      p_vci.rtrdid = 0;
      p_vci.rpktid = 0;

      p_vci.rerror = vci_param::ERR_NORMAL;

    p_avalon.address = p_vci.address.read();



      if (p_vci.cmdval)
	{
		
		
	  switch (p_vci.cmd.read())
	    {

	    case vci_param::CMD_NOP :
	      printf("ERROR : VCI initiator CMD_NOP non supporte\n");
	      exit(1);
	      break;


	    case vci_param::CMD_READ :
	      p_avalon.read = true; p_avalon.write = false;

#if DEBUG_INIT_WRAPPER
	      std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator  vci_param::CMD_READ " << std::endl;
#endif

	      break;


	    case vci_param::CMD_WRITE :
	      p_avalon.read = false; p_avalon.write = true;

#if DEBUG_INIT_WRAPPER
	      std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator FSM_LAST_WDATA vci_param::CMD_WRITE" << std::endl;
#endif

	      break;


	    case vci_param::CMD_LOCKED_READ :
	      //printf("ERROR : VCI initiator CMD_LOCKED_READ non supporte\n");
			// Charles
				std::cout << "                               ##################################################                      p_vci.cmd = " <<  p_vci.cmd << std::endl;
				 std::cout << "                              #################################################                       p_vci.address = " <<  p_vci.address << std::endl;
				 std::cout << "                              ##################################################                      p_vci.eop = " <<  p_vci.eop << std::endl;
				 std::cout << "                              ##################################################                      p_vci.cmdval = " <<  p_vci.cmdval << std::endl;
				 std::cout << "                              ##################################################                      p_vci.reop = " <<  p_vci.reop << std::endl;
				 std::cout << "                              ##################################################                      p_vci.cmdack = " <<  p_vci.cmdack<< std::endl;
				 std::cout << "                              ##################################################                      p_vci.rspval = " <<  p_vci.rspval<< std::endl;
				
				
				
				
				
	      //exit(1);

	      break;

	    }
	}


      else
	{
	  p_avalon.read = false;
	  p_avalon.write = false;
	}

      p_avalon.writedata = p_vci.wdata.read();
      p_avalon.byteenable = p_vci.be.read();
      //p_avalon.flush = false;
      //p_avalon.resetrequest = false;


      switch (r_fsm_state)
	{

	case FSM_IDLE: //=================
//::cout << "################################################################################################    IDLE " << std::endl;
#if DEBUG_INIT_WRAPPER
	  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator FSM_IDLE" << std::endl;
#endif

	p_vci.cmdack = false; 		

	
			
	// Charles_1  p_vci.rspval = false;
			if ((p_vci.cmdval) && (!p_vci.eop) && (!p_avalon.waitrequest) ) p_vci.rspval = true;  // on ira en R_DATA
			else p_vci.rspval = false;
	
			
	p_vci.rdata = p_avalon.readdata.read();
	p_vci.reop = false;

	  //***************************************************positionnement du bus AVALON

	  if (p_vci.plen.read()%vci_param::B != 0) {p_avalon.burstcount = (p_vci.plen.read()/vci_param::B) +1;}
	  else {p_avalon.burstcount = p_vci.plen.read()/vci_param::B;}


	  break; //=========fin FSM_IDDLE



	case FSM_R_ACQ1: //=================

#if DEBUG_INIT_WRAPPER
	  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator FSM_R_ACQ1" << std::endl;
#endif

	  //****************************************************positionnement du bus VCI

	  //LECTURE
	  if (r_cmd == vci_param::CMD_READ)
	    {
	      if  (p_avalon.readdatavalid) 	{p_vci.rspval = true; p_vci.cmdack = true;}
	      else {	p_vci.rspval = false; p_vci.cmdack = false;}
	    }

	  //ECRITURE
	  if (r_cmd == vci_param::CMD_WRITE)
	    {
	      if  (!p_avalon.waitrequest) 	{p_vci.rspval = true;  p_vci.cmdack = true;}
	      else {	p_vci.rspval = false; p_vci.cmdack = false;}
	    }

	  p_vci.reop = true;

	  //***************************************************positionnement du bus AVALON
	  p_avalon.burstcount = 1;

	  break; //=========fin FSM_R_ACQ1


	case FSM_R_WAIT1: //=================

#if DEBUG_INIT_WRAPPER
	  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator FSM_R_WAIT1" << std::endl;
#endif

	  //****************************************************positionnement du bus VCI
	  // ===================requete
	  //LECTURE
	  if (r_cmd == vci_param::CMD_READ)
	    {
	      if ((!p_avalon.waitrequest) && (p_avalon.readdatavalid)){p_vci.cmdack = true;}
	      else {p_vci.cmdack = false; }

	      // ===================reponse
	      p_vci.rdata = p_avalon.readdata.read();
	      if (!(p_avalon.waitrequest)) 
		  {
			  if (p_avalon.readdatavalid) 
			  {  p_vci.rspval = true ; p_vci.reop = true; } // on ira en FSM_IDLE
			  else {p_vci.rspval = false;}
		  }
	    }

	  //ECRITURE
	  if (r_cmd == vci_param::CMD_WRITE)
	    {
	      if (!p_avalon.waitrequest)
		  {p_vci.cmdack = true;	  }
	      else {p_vci.cmdack = false; }

	      // ===================reponse
	      if (!(p_avalon.waitrequest))  
		  {p_vci.rspval = true ;  p_vci.reop = true; } // on ira en FSM_IDLE
	      else {p_vci.rspval = false;  p_vci.reop = false;}
	    }

	 

	  //***************************************************positionnement du bus AVALON
	  p_avalon.burstcount = 1;


	  break; //=========fin FSM_R_WAIT1



	case FSM_R_WAIT2: //=================

#if DEBUG_INIT_WRAPPER
	  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++�genMealy vci_avalon_initiator R_WAIT2" << std::endl;
#endif

	  //****************************************************positionnement du bus VCI
	  // ===================requete

	  //LECTURE
	  if (r_cmd == vci_param::CMD_READ)
	    {
			//=================requete
			// gestion VCI cmdack
					
					
			// paquet transfert
			
	      if((!p_avalon.waitrequest) && (p_avalon.readdatavalid)){p_vci.cmdack = true;}
	      else {p_vci.cmdack = false;}

	      // ===================reponse
	      // gestion VCI rspval
	      if (!(p_avalon.waitrequest))
		{
		  if (p_avalon.readdatavalid) // on ira dans FSM_RDATA
		    {
		      p_vci.rspval = true;

#if DEBUG_INIT_WRAPPER
		      std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  WAIT2    READBURSCOUNT + 1" << std::endl;
		      //std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
		      std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
#endif
		    }
		  else {p_vci.rspval = false;}
		  p_vci.rdata = p_avalon.readdata.read();
		}
	    }

	  //ECRITURE
	  // ne se produit pas
	  if (r_cmd == vci_param::CMD_WRITE)
	    {
	      printf("ERROR : ecriture R_WAIT2 non atteignable ! \n");
	      //if (!p_avalon.waitrequest) {p_vci.cmdack = true; p_vci.rspval = true;}
	      //else {p_vci.cmdack = false; p_vci.rspval = false;}
	    }

	  p_vci.reop = false;


			
			
	  //***************************************************positionnement du bus AVALON

	  if (p_vci.plen.read()%vci_param::B != 0) {p_avalon.burstcount = (p_vci.plen.read()/vci_param::B) +1;}
	  else {p_avalon.burstcount = p_vci.plen.read()/vci_param::B;}



	  break; //=========fin FSM_R_WAIT2


			
		case FSM_R_WAIT_ADV: //=================
			
#if DEBUG_INIT_WRAPPER
			std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++�genMealy vci_avalon_initiator R_WAIT_ADV" << std::endl;
#endif
			
			//****************************************************positionnement du bus VCI
			// ===================requete
			
			//LECTURE
			if (r_cmd == vci_param::CMD_READ)
			{
				//=================requete
				// gestion VCI cmdack
				
				
				// paquet transfert
				
				if((!p_avalon.waitrequest) && (p_avalon.readdatavalid)){p_vci.cmdack = true;}
				else {p_vci.cmdack = false;}
				
				// ===================reponse
				// gestion VCI rspval
				if (!(p_avalon.waitrequest))
				{
					if (p_avalon.readdatavalid) 
					{							
						// Charles 15/01/2010   if (p_vci.plen.read() == 4 ) {p_vci.rspval = false; p_vci.reop = true;} // on ira dans FSM_LAST_RDATA_ADV
						if (p_vci.plen.read() == 4 ) {p_vci.rspval = true; p_vci.reop = true;} // on ira dans FSM_LAST_RDATA_ADV
						
						else  {p_vci.rspval = true; p_vci.reop = false; }  // on ira dans FSM__RDATA_ADV

						#if DEBUG_INIT_WRAPPER
						std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  WAIT2    READBURSCOUNT + 1" << std::endl;
						//std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
						std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
						#endif
					}
					else {p_vci.rspval = false; p_vci.reop = false;}

					p_vci.rdata = p_avalon.readdata.read();
				}
			}
		
			
			//***************************************************positionnement du bus AVALON
			
			if (p_vci.plen.read()%vci_param::B != 0) {p_avalon.burstcount = (p_vci.plen.read()/vci_param::B) +1;}
			else {p_avalon.burstcount = p_vci.plen.read()/vci_param::B;}
					
			
			p_avalon.byteenable = r_byteenable.read();
			p_avalon.address  = r_address.read();
			//p_avalon.read = r_read;
			//p_avalon.write = r_write;
		
			
			
			
			
			break; //=========fin FSM_R_WAIT_ADV
			

	case FSM_R_ACQ2: //=================

#if DEBUG_INIT_WRAPPER
	  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator R_ACQ2" << std::endl;
#endif
	  //****************************************************positionnement du bus VCI
	  //====================requete
	  p_vci.cmdack = true;
	  // ===================reponse


	  //LECTURE
	  if (r_cmd == vci_param::CMD_READ)
	    {
	      if (p_avalon.readdatavalid )
		{
		  p_vci.rspval = true;
		  p_vci.cmdack = true;

#if DEBUG_INIT_WRAPPER
		  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  ACQ2   READBURSCOUNT + 1" << std::endl;
		  //std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
		  std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
#endif
		}
	      else   {p_vci.rspval = false; p_vci.cmdack = false;}

	      p_vci.rdata = p_avalon.readdata.read();
	    }

	  //ECRITURE
	  // ne se produit pas
	  if (r_cmd == vci_param::CMD_WRITE)
	    {
	      printf("ERROR : ecriture R_ACQ2 non atteignable ! \n");
	      //p_vci.rspval = true;
	      //p_vci.cmdack = true;
	    }
	  //else   {p_vci.rspval = false; p_vci.cmdack = false;}

	  p_vci.reop = false;

	  //***************************************************positionnement du bus AVALON


	  if (p_vci.plen.read()%vci_param::B != 0) {p_avalon.burstcount = (p_vci.plen.read()/vci_param::B) +1;}
	  else {p_avalon.burstcount = p_vci.plen.read()/vci_param::B;}


	  break; //=========fin FSM_R_ACQ2


			
		
			
		case FSM_R_ACQ_ADV: //=================
			
#if DEBUG_INIT_WRAPPER
			std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator R_ACQ_ADV" << std::endl;
#endif
			//****************************************************positionnement du bus VCI
			//====================requete
			 p_vci.cmdack = true;
			
			// ===================reponse
			
			
			//LECTURE
			if (r_cmd == vci_param::CMD_READ)
			{
				if (p_avalon.readdatavalid )
				{
					p_vci.rspval = true;
									
#if DEBUG_INIT_WRAPPER
					std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  ACQ_ADV  READBURSCOUNT + 1" << std::endl;
					//std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
					std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
#endif
				}
				else   {p_vci.rspval = false; }
				
				p_vci.rdata = p_avalon.readdata.read();
			}
				
			p_vci.reop = false;
			
			//***************************************************positionnement du bus AVALON
			
			
			if (p_vci.plen.read()%vci_param::B != 0) {p_avalon.burstcount = (p_vci.plen.read()/vci_param::B) +1;}
			else {p_avalon.burstcount = p_vci.plen.read()/vci_param::B;}
			
			
			p_avalon.byteenable = r_byteenable.read();
			p_avalon.address  = r_address.read();
			//p_avalon.read = r_read;
			//p_avalon.write = r_write;
			break; //=========fin FSM_R_ACQ_ADV
			
			

	case FSM_LAST_RDATA: //=================

#if DEBUG_INIT_WRAPPER
	  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator LAST_RDATA" << std::endl;
#endif

	  //****************************************************positionnement du bus VCI
	  // ===================requete
			p_vci.cmdack = false;

	  // ===================reponse
			p_vci.rdata = p_avalon.readdata.read();

			//  CHarles  if (p_avalon.waitrequest == true) {p_vci.rspval= true;} // on ira en IDLE
			//else {p_vci.rspval= false ;}
	        // p_vci.rspval= true;
			// Charles_1 p_vci.rspval= false;			
			//p_vci.reop = true;
			// Charles_1 p_vci.reop = false;

	  //***************************************************positionnement du bus AVALON
			if (r_cmd == vci_param::CMD_READ)		   { p_avalon.burstcount = r_read_burstcount.read(); p_vci.rspval= true; p_vci.reop = true;}
			if (r_cmd == vci_param::CMD_WRITE)		   {p_avalon.burstcount = r_write_burstcount.read(); p_vci.rspval= false; p_vci.reop = false; }

	  break; //=========fin FSM_LAST_RDATA

			
	
		case FSM_LAST_RDATA_ADV: //=================
			
#if DEBUG_INIT_WRAPPER
			std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator LAST_RDATA_ADV" << std::endl;
#endif
			
			//****************************************************positionnement du bus VCI
			// ===================requete
			p_vci.cmdack = false;
			// ===================reponse
			p_vci.rdata = p_avalon.readdata.read();
			
			p_vci.rspval= false;
			p_vci.reop = false;
			
			
			//***************************************************positionnement du bus AVALON
			if (r_cmd == vci_param::CMD_READ)		p_avalon.burstcount = r_read_burstcount.read();
			if (r_cmd == vci_param::CMD_WRITE)		p_avalon.burstcount = r_write_burstcount.read();
			
			
			
			p_avalon.byteenable = r_byteenable.read();
			p_avalon.address  = r_address.read();
			p_avalon.read = r_read;
			p_avalon.write = r_write;
			
			
			break; //=========fin FSM_LAST_RDATA_ADV
			


	case FSM_RDATA: //=================

#if DEBUG_INIT_WRAPPER
	  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator FSM_RDATA" << std::endl;
#endif

	  //****************************************************positionnement du bus VCI
	  // ===================requete
	  //LECTURE
	  if (r_cmd == vci_param::CMD_READ)
	    {
	      // ===================reponse
	      if (!p_avalon.readdatavalid.read() )
		{
		  p_vci.rspval = false;
		  p_vci.cmdack = false;
		  p_vci.reop = false;
		} 	// on ira en FSM_WAITRDATA


	      else
		{
		  p_vci.rspval = true; p_vci.cmdack = true;
			
			if (p_vci.eop)			
		  		  
		  {p_vci.reop = true; }// on ira en FSM_LAST_RDATA
		  else 	{p_vci.reop = false;  }                                           // on ira en FSM_RDATA

#if DEBUG_INIT_WRAPPER
		  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ RDATA     READBURSCOUNT + 1" << std::endl;
		  //std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
		  std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
#endif
		}


	      p_vci.rdata = p_avalon.readdata.read();
	      p_avalon.burstcount = r_read_burstcount.read();
			
	    }// fin LECTURE



		//ECRITURE
			
				if (r_cmd == vci_param::CMD_WRITE)
				{
					// Charles_1	
					//p_vci.rspval = true; 					
			
					//std::cout << "################################################################################################    ECRITURE " <<  std::endl;
					// ===================reponse

				if (p_avalon.waitrequest == true)
					{
						// Charles_1 
						p_vci.rspval = false;
						//std::cout << "################################################################################################    FALSE rspval = " << p_vci.rspval << std::endl;
						p_vci.cmdack = false;
						p_vci.reop = false;
					} 	// on ira en FSM_WAITRDATA
				else
					{  // Charles_1 
					p_vci.rspval = true; 
					//std::cout << "################################################################################################  TRUE rspval = " << p_vci.rspval << std::endl;
						if (p_vci.eop)
							// Charles { p_vci.reop = true; p_vci.rspval = false; p_vci.cmdack = true;}   // on ira en FSM_LAST_RDATA
							// Charles_1 
						{ p_vci.reop = true; p_vci.cmdack = true;}   // on ira en FSM_LAST_RDATA
					//{ p_vci.reop = false ; p_vci.cmdack = true;}   // on ira en FSM_LAST_RDATA
						else
							// Charles
							// {p_vci.reop = false; p_vci.rspval = false; p_vci.cmdack = true;}  // on ira en FSM_RDATA
						{ p_vci.reop = false;  p_vci.cmdack = true;}  // on ira en FSM_RDATA
					}

					p_avalon.burstcount = r_write_burstcount.read();
				}
			//fin ECRITURE

	  //***************************************************positionnement du bus AVALON


	  break; //=========fin FSM_RDATA


			
			
			
		case FSM_RDATA_ADV: //=================
			
#if DEBUG_INIT_WRAPPER
			std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator FSM_RDATA_ADV" << std::endl;
#endif
			
			//****************************************************positionnement du bus VCI
			// ===================requete
			//LECTURE
			
				p_vci.cmdack = false;	
			
			if (r_cmd == vci_param::CMD_READ)
			{
				// ===================reponse
				if (!p_avalon.readdatavalid.read() )
				{
					p_vci.rspval = false;			
					p_vci.reop = false;
				} 	// on ira en FSM_WAITRDATA_ADV
				
				
				else
				{
					p_vci.rspval = true;
			
					if (r_read_burst_count== r_read_burstcount - 1)	{p_vci.reop = true; } // on ira en FSM_LAST_RDATA_ADV
					else 	{p_vci.reop = false;  }                                       // on ira en FSM_RDATA_ADV
					
#if DEBUG_INIT_WRAPPER
					std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ RDATA_ADV     READBURSCOUNT + 1" << std::endl;
					std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
#endif
				}
			
				p_vci.rdata = p_avalon.readdata.read();
				p_avalon.burstcount = r_read_burstcount.read();
			}// fin LECTURE
			
			
	
			//***************************************************positionnement du bus AVALON
			p_avalon.byteenable = r_byteenable.read();
			p_avalon.address  = r_address.read();
			p_avalon.read = r_read;
			p_avalon.write = r_write;
			
			
			break; //=========fin FSM_RDATA_ADV
			
			
			
			
			

	case FSM_WAIT_RDATA: //=================

#if DEBUG_INIT_WRAPPER
	  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator FSM_WAIT_RDATA" << std::endl;
#endif

	  //****************************************************positionnement du bus VCI
	  // ===================requete

	  // ===================reponse

	  //LECTURE
	  if (r_cmd == vci_param::CMD_READ)
	    {
	      if (p_avalon.readdatavalid == true)
		{
		  if (p_vci.eop)
		    {
		      p_vci.reop = false;	p_vci.rspval = false; p_vci.cmdack = false;
		    }// on ira en FSM_LAST_RDATA
		  else
		    { p_vci.reop = false;   p_vci.rspval = true; p_vci.cmdack = true; } // on ira en FSM_RDATA

#if DEBUG_INIT_WRAPPER
		std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  WAIT_RDATA  READBURSCOUNT + 1" << std::endl;			std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
		 std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
#endif

		}
	      else
		{

#if DEBUG_INIT_WRAPPER
		  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  WAIT_RDATA  readdatavalid = 0" << std::endl;
#endif

		  p_vci.rspval = false; 	p_vci.cmdack = false;
		  p_vci.reop = false;
		}

	      p_vci.rdata = p_avalon.readdata.read();
	      p_avalon.burstcount = r_read_burstcount.read();
	    }//fin LECTURE


	  //ECRITURE
	  if (r_cmd == vci_param::CMD_WRITE)
	    {
			// Charles_1
			 // p_vci.rspval = false;
			
	      if (p_avalon.waitrequest == false)
		{
			// Charles_1 
			p_vci.rspval = true;
		  if (p_vci.eop)  { p_vci.reop = true;	p_vci.cmdack = true; }      // on ira en FSM_LAST_RDATA
		  else	          { p_vci.reop = false; p_vci.cmdack = true; }       // on ira en FSM_RDATA

#if DEBUG_INIT_WRAPPER
		  //std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  WAIT_RDATA  READBURSCOUNT + 1" << std::endl;			std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
		  std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
#endif

		}
	      else
		{
#if DEBUG_INIT_WRAPPER
		  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  WAIT_RDATA  waitrequest  = 0" << std::endl;
#endif

		  // Charles_1 
		  p_vci.rspval = false; 	p_vci.cmdack = false;
		  p_vci.reop = false;
		  p_avalon.burstcount = r_write_burstcount.read();
		}
	    }//fin ECRITURE

	  //***************************************************positionnement du bus AVALON

	  break; //=========fin FSM_WAIT_RDATA


			
		
			
			
		case FSM_WAIT_RDATA_ADV: //=================
			
#if DEBUG_INIT_WRAPPER
			std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++genMealy vci_avalon_initiator FSM_WAIT_RDATA_ADV" << std::endl;
#endif
						
			
			//****************************************************positionnement du bus VCI
			// ===================requete
			p_vci.cmdack = false;
			// ===================reponse
			
			//LECTURE
			if (r_cmd == vci_param::CMD_READ)
			{
				
				if (p_avalon.readdatavalid == true)
				{
					// Charrles if (p_vci.eop) { p_vci.reop = false; p_vci.rspval = false; } // on ira en FSM_LAST_RDATA_ADV
					
					// Charles  14/01/2010  if (p_vci.eop) { p_vci.reop = true; p_vci.rspval = false; } // on ira en FSM_LAST_RDATA_ADV
					if (p_vci.eop) {p_vci.reop = true; p_vci.rspval = true; } // on ira en FSM_LAST_RDATA_ADV				
					else { p_vci.reop = false;   p_vci.rspval = true;  } // on ira en FSM_RDATA_ADV
					
#if DEBUG_INIT_WRAPPER
					//std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  WAIT_RDATA_ADV  READBURSCOUNT + 1" << std::endl;			std::cout << "################################################################################################      r_read_burst_count = " <<  std::hex << r_read_burst_count << std::endl;
					std::cout << "################################################################################################      p_avalon.readdatavalid = " <<  p_avalon.readdatavalid << std::endl;
#endif
					
				}
				else
				{
					
#if DEBUG_INIT_WRAPPER
					std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  WAIT_RDATA_ADV  readdatavalid = 0" << std::endl;
#endif
					
					p_vci.rspval = false; 
					p_vci.reop = false;
				}
				
				p_vci.rdata = p_avalon.readdata.read();
				p_avalon.burstcount = r_read_burstcount.read();
			}//fin LECTURE
			
			
			//***************************************************positionnement du bus AVALON
			
			p_avalon.byteenable = r_byteenable.read();
			p_avalon.address  = r_address.read();
			p_avalon.read = r_read;
			p_avalon.write = r_write;
			
			
			break; //=========fin FSM_WAIT_RDATA_ADV
			
	} // end switch
    }; // end genMealy

  }// end caba

} // end namespace

