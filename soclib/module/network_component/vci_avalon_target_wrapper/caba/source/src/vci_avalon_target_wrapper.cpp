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
 * 	       Charles Wagner <wagner@irisa.fr>
 * 
 * Maintainer: wagner
 * 
 * File : vci_avalon_target_wrapper.cpp
 * Date : 20/11/2008
 * 
 * 
 */





#include "vci_avalon_target_wrapper.h"
#include "register.h"

namespace soclib { namespace caba {

#define Avalonbus soclib::caba:Avalonbus

    ////////////////////////////////
    //	constructor
    ////////////////////////////////

#define tmpl(x) template<typename vci_param, typename avalon_param>  x VciAvalonTargetWrapper< vci_param, avalon_param>
    ////////////////////////////////
    //	constructor
    ////////////////////////////////
    tmpl(/**/)::VciAvalonTargetWrapper (sc_module_name insname)
	       : soclib::caba::BaseModule(insname)

    {
      SC_METHOD (transition);
      dont_initialize();
      sensitive << p_clk.pos();

      SC_METHOD (genMealy);
      dont_initialize();
      sensitive  << p_clk.neg();

      sensitive  << p_avalon.address;
      sensitive  << p_avalon.byteenable;
      sensitive  << p_avalon.writedata;
      sensitive  << p_avalon.readdata;

      sensitive  << p_vci.cmdack;
      sensitive  << p_vci.rdata;
      sensitive  << p_vci.reop;
      sensitive  << p_vci.rspval;


      SOCLIB_REG_RENAME(r_fsm_state);
      SOCLIB_REG_RENAME(r_burstcount);
      SOCLIB_REG_RENAME(r_burst_count);
      SOCLIB_REG_RENAME(r_read);
      SOCLIB_REG_RENAME(r_address);
      //SOCLIB_REG_RENAME(r_byteenable);

    } //  end constructor

    ////////////////////////////////
    //	transition 
    ////////////////////////////////

    tmpl(void)::transition()

    {
#if DEBUG_TARGET_WRAPPER
      std::cout << "==============================================>   entree transition  vci_avalon_target" << std::endl;
#endif

      if (p_resetn == false) {
	r_fsm_state   = FSM_IDLE;
	r_burstcount  = 0;
	r_burst_count = 0;

	return;
      } // end reset

      switch (r_fsm_state) {
 
      case FSM_IDLE: //================
 
 	r_address = p_avalon.address.read();
   	r_burstcount   = p_avalon.burstcount.read();
	
	//r_burst_count  = p_avalon.burstcount.read();
			  
			  
			  
	r_read  = p_avalon.read.read();
   	r_write = p_avalon.write.read();	

	//Charles  r_byteenable = p_avalon.byteenable.read();
	 
	if  ((p_vci.cmdack)  && (p_avalon.chipselect))
	  {    
		  if ((p_avalon.read) == true) {r_burst_count  = p_avalon.burstcount.read();  r_fsm_state  = FSM_ACQ_LEC;}
		  // Charles : advanced write
		  
	    else if ((p_avalon.write) == true)
		{	if (p_avalon.burstcount.read() == 1) 
				{r_burst_count  = p_avalon.burstcount.read(); r_fsm_state  = FSM_ACQ_LEC;} 
				else {r_burst_count  = p_avalon.burstcount.read(); r_fsm_state  = FSM_ADV_WRITE;}
		}
	  }
 
	if  ((!p_vci.cmdack)  && (p_avalon.chipselect)) 
	  {    
	    if ((p_avalon.read) == true) {r_burst_count  = p_avalon.burstcount.read(); r_fsm_state  = FSM_WAIT_LEC;}
		else if ((p_avalon.write) == true)
		{	if (p_avalon.burstcount.read() == 1) 
		{r_burst_count  = p_avalon.burstcount.read(); r_fsm_state  = FSM_WAIT_LEC;} 
		else {r_fsm_state  = FSM_ADV_WRITE;}
		}
	  } 
	
	break;  //=========fin FSM_IDLE

 
 
      case FSM_WAIT_LEC: //==============
  	
 	//Charles	  r_byteenable = p_avalon.byteenable.read(); 
			  
 	r_address = p_avalon.address.read()+ vci_param::B ;
	r_fsm_state = FSM_BURST_RDATA;

	break;;  //=========fin FSM_WAIT_LEC



      case FSM_ACQ_LEC : //===============

	//rspval, reop positionné un cycle après 
	r_address = p_avalon.address.read()+ vci_param::B ;
	r_fsm_state  = FSM_BURST_RDATA;
	
			  //Charles   r_byteenable = p_avalon.byteenable.read(); 
 
	break;  //=========fin FSM_ACQ_LEC
	


      case FSM_BURST_RDATA: //===============

	if (p_vci.rspval) 
	  {
	    r_address = r_address + vci_param::B;
	    if (p_vci.reop) {
	      r_fsm_state = FSM_LAST_BURST_RDATA;
	    }
	  }
	else {r_fsm_state = FSM_WAIT_BURST_RDATA;}
		
	r_burstcount = r_burstcount - 1;
	
	break;  //=========fin FSM_BURST_RDATA
	
	          

      case FSM_WAIT_BURST_RDATA: //===============

	if ((p_vci.rspval) && (p_vci.reop)) {
	  r_address = r_address + vci_param::B;
	  r_fsm_state = FSM_LAST_BURST_RDATA;
	}
	if ((p_vci.rspval) && (!p_vci.reop)) {
	  r_address = r_address + vci_param::B;
	  r_fsm_state = FSM_BURST_RDATA;
	}	
	
	break;  //=========fin FSM_WAIT_BURST_RDATA



      case FSM_LAST_BURST_RDATA : //===============

	r_fsm_state = FSM_IDLE;	

	break;	 //=========fin FSM_LAST_BURST_RDATA   
			  
			  

		  case FSM_ADV_WRITE: //===============
			  
			r_burstcount = r_burstcount - 1;
			r_address = r_address + vci_param::B;
			
			  //Charles  	  r_byteenable = p_avalon.byteenable.read(); 
			  
			  if (r_burstcount== 1) {				  
				  r_fsm_state = FSM_LAST_ADV_WRITE;
			  }
			  				
			  break;  //=========fin FSM_ADV_WRITE
			  
			  
			  
		  case FSM_LAST_ADV_WRITE: //===============
			  

			  r_address = r_address + vci_param::B;			 				  
			 if (p_vci.rspval == true)  r_fsm_state = FSM_IDLE;
			
			  
			  break;  //=========fin FSM_LAST_ADV_WRITE
	
	
#if DEBUG_TARGET_WRAPPER
	std::cout << "==============================================>   sortie transition  vci_avalon_target" << std::endl;
#endif

      }   // end switch fsm
    };  // end transition



    ////////////////////////////////
    //	genMealy
    ////////////////////////////////
    tmpl(void)::genMealy()


    {
#if DEBUG_TARGET_WRAPPER
      std::cout << "==============================================>   entree genMealy  vci_avalon_target" << std::endl;
#endif

      switch (r_fsm_state) {
       
      case FSM_IDLE: //===============

#if DEBUG_TARGET_WRAPPER
	std::cout << "==============================================>   genMealy   FSM_IDLE " << std::endl;
#endif

	// bus VCI =============================================
	p_vci.cmdval = false;
	p_vci.cmd = vci_param::CMD_NOP;
	p_vci.wdata = p_avalon.writedata.read();
	p_vci.address = p_avalon.address.read();
	p_vci.be = p_avalon.byteenable.read();
	p_vci.eop = true;
	p_vci.contig = true; // addresses contigues
	p_vci.cons = false;
	p_vci.wrap = false;
	p_vci.plen = 0;
	p_vci.clen = 0; // pas de chainage de paquets
	p_vci.rspack =   false;

	// bus AVALON  ==========================================
 	p_avalon.readdata =  p_vci.rdata.read() ; 
   	p_avalon.waitrequest = true;
	p_avalon.readdatavalid = false;

	break;	 //=========fin  FSM_IDLE
   


      case FSM_WAIT_LEC: //===============

#if DEBUG_TARGET_WRAPPER
	std::cout << "==============================================>   genMealy   FSM_WAIT_LEC " << std::endl;
#endif

	// bus VCI : avalon waitrequest
	p_vci.cmdval = true;

	if (r_write){p_vci.cmd = vci_param::CMD_WRITE;}
	if (r_read) {p_vci.cmd = vci_param::CMD_READ;}	
	p_vci.wdata = p_avalon.writedata.read();	
	p_vci.address = p_avalon.address.read();
	p_vci.be = p_avalon.byteenable.read();
	if ((r_burstcount == 0) || (r_burstcount == 1)){p_vci.eop = true;}
	else {p_vci.eop = false;}
	p_vci.contig = true; // addresses contigues
	p_vci.cons = false;
	p_vci.wrap = false;
	p_vci.plen = r_burstcount.read()*vci_param::B;
	p_vci.clen = 0; // pas de chainage de paquets
	p_vci.rspack =   true;
	
 	// bus AVALON  ==========================================
	p_avalon.readdata =  p_vci.rdata.read() ; 
	if ((p_vci.cmdack))  // on ira dans ACQ_LEC
	  {
	    if (r_read)  
	      {p_avalon.readdatavalid = true; p_avalon.waitrequest = true;}       
																			
	    else if (r_write) 													// ecriture : waitrequest
	      {p_avalon.waitrequest = false; p_avalon.readdatavalid = false;}
	    else{p_avalon.readdatavalid = false; p_avalon.waitrequest = true;}
	  }
	else {p_avalon.readdatavalid = false; p_avalon.waitrequest = true;}
					
	break;	 //=========fin FSM_WAIT_LEC



      case FSM_ACQ_LEC: //===============

#if DEBUG_TARGET_WRAPPER
	std::cout << "==============================================>   genMealy   FSM_ACQ_LEC " << std::endl;
#endif

	// bus VCI : avalon waitrequest
	p_vci.cmdval = true;
	
	if (r_write){p_vci.cmd = vci_param::CMD_WRITE;}
	if (r_read) {p_vci.cmd = vci_param::CMD_READ;}	
	p_vci.address = p_avalon.address.read();
	p_vci.be = p_avalon.byteenable.read();
	if ((r_burstcount == 0) || (r_burstcount == 1)){p_vci.eop = true;}
	else {p_vci.eop = false;}
	p_vci.wdata = p_avalon.writedata;
	p_vci.contig = true; // addresses contigues
	p_vci.cons = false;
	p_vci.wrap = false;
	p_vci.clen = 0; // pas de chainage de paquets	
	p_vci.plen = r_burstcount.read()*vci_param::B;
	p_vci.rspack =   true;
	
	// bus AVALON  ==========================================
 	p_avalon.readdata =  p_vci.rdata.read() ; 	
	p_avalon.readdatavalid = false; p_avalon.waitrequest = true;
   	break;	 //=========fin FSM_ACQ_LEC


      case FSM_BURST_RDATA: //===============

#if DEBUG_TARGET_WRAPPER
	std::cout << "==============================================>   genMealy   FSM_BURST_RDATA " << std::endl;
#endif

	// bus VCI : avalon waitrequest
	p_vci.cmdval = true;
	if (r_write){p_vci.cmd = vci_param::CMD_WRITE;}
	if (r_read) {p_vci.cmd = vci_param::CMD_READ;}	
	p_vci.wdata = p_avalon.writedata;	
	p_vci.address = r_address.read();
	
	//Charles  		  p_vci.be = r_byteenable.read();
	p_vci.be = p_avalon.byteenable.read();
			  
			  if ((r_burstcount == 2) || (r_burstcount == 1)) {p_vci.eop = true;}
	else {	p_vci.eop = false;}
	p_vci.contig = true; // addresses contigues
	p_vci.cons = false;
	p_vci.wrap = false;
	p_vci.plen = r_burst_count.read()*vci_param::B;
	p_vci.clen = 0; // pas de chainage de paquets
	p_vci.rspack =  true;	
	
	
	// bus AVALON  ==========================================
 	p_avalon.readdata =  p_vci.rdata.read() ; 
	if (r_read)   {p_avalon.readdatavalid = true;  p_avalon.waitrequest = false;}
	if (r_write)  {p_avalon.readdatavalid = false; p_avalon.waitrequest = false;}

  	break;	 //=========fin FSM_BURST_RDATA



      case FSM_WAIT_BURST_RDATA: //===============

#if DEBUG_TARGET_WRAPPER
	std::cout << "==============================================>   genMealy   FSM_WAIT_BURST_RDATA " << std::endl;
#endif

	// bus VCI 
	  p_vci.cmdval = false;
	
			  
	if (r_write){p_vci.cmd = vci_param::CMD_WRITE;}
	if (r_read) {p_vci.cmd = vci_param::CMD_READ;}	
	p_vci.wdata = p_avalon.writedata;	
	p_vci.address = r_address.read();
	
	//Charles  	  p_vci.be = r_byteenable.read();
	p_vci.be = p_avalon.byteenable.read();
			  
			  
	p_vci.eop = false;
	p_vci.contig = true; // addresses contigues
	p_vci.cons = false;
	p_vci.wrap = false;
	p_vci.plen = r_burst_count.read()*vci_param::B;
	p_vci.clen = 0; // pas de chainage de paquets	
	p_vci.rspack =   true;
	
	// bus AVALON  ==========================================
   	p_avalon.readdata =  p_vci.rdata.read() ; 
	p_avalon.waitrequest = true;
	if (p_vci.rspval.read() ) {p_avalon.readdatavalid = true;} // on ira en FSM_BURST_RDATA ou FSM_LAST_BURST_RDATA
	else {p_avalon.readdatavalid = false;}
	
   	break;	 //=========fin FSM_WAIT_BURST_RDATA



      case FSM_LAST_BURST_RDATA: //===============

#if DEBUG_TARGET_WRAPPER
	std::cout << "==============================================>   genMealy   FSM_LAST_BURST_RDATA " << std::endl;
#endif

	// bus VCI 
	p_vci.cmdval = false;
	if (r_write){p_vci.cmd = vci_param::CMD_WRITE;}
	if (r_read) {p_vci.cmd = vci_param::CMD_READ;}	
	p_vci.wdata = p_avalon.writedata;	
	p_vci.address = r_address.read();
	
	//Charles  	  p_vci.be = r_byteenable.read();
	p_vci.be = p_avalon.byteenable.read();
			  
			  
	p_vci.eop = true;
	p_vci.contig = true; // addresses contigues
	p_vci.cons = false;
	p_vci.wrap = false;
	p_vci.plen = r_burst_count.read()*vci_param::B;
	p_vci.clen = 0; // pas de chainage de paquets
	p_vci.rspack =  true;

	// bus AVALON  ==========================================
   	p_avalon.readdata =  p_vci.rdata.read() ; 
	p_avalon.waitrequest = true;
	p_avalon.readdatavalid = false;
	
  	break;	 //=========fin FSM_LAST_BURST_RDATA
			  

			  
			  
    case FSM_ADV_WRITE: //===============
			  
#if DEBUG_TARGET_WRAPPER
			  std::cout << "==============================================>   genMealy   FSM_LAST_BURST_RDATA " << std::endl;
#endif
			  
			  // bus VCI 
			  p_vci.cmdval = true;
			  p_vci.cmd = vci_param::CMD_WRITE;
			  p_vci.wdata = p_avalon.writedata;	
			  p_vci.address = r_address.read();
			  
			 // Charles  p_vci.be = r_byteenable.read();
			 p_vci.be = p_avalon.byteenable.read();
			  
			  if (r_burstcount == 1 ) p_vci.eop = true; 
			 else p_vci.eop = false;
			  p_vci.contig = true; // addresses contigues
			  p_vci.cons = false;
			  p_vci.wrap = false;
			  p_vci.plen = r_burst_count.read()*vci_param::B;
			  p_vci.clen = 0; // pas de chainage de paquets
			  p_vci.rspack = false;
			  
			  // bus AVALON  ==========================================
			  p_avalon.readdata =  p_vci.rdata.read() ; 
			  p_avalon.waitrequest = false;
			  p_avalon.readdatavalid = false;
			  
			  break;	 //=========fin FSM_ADV_WRITE		  
			  
			  
		  case FSM_LAST_ADV_WRITE: //===============
			  
#if DEBUG_TARGET_WRAPPER
			  std::cout << "==============================================>   genMealy   FSM_LAST_BURST_RDATA " << std::endl;
#endif
			  
			  // bus VCI 
			  p_vci.cmdval = false;
			  p_vci.cmd = vci_param::CMD_WRITE;
			  p_vci.wdata = p_avalon.writedata;	
			  p_vci.address = r_address.read();
			 
			  //Charles   p_vci.be = r_byteenable.read();
			  p_vci.be = p_avalon.byteenable.read();
			  
			  p_vci.eop = true;
			  p_vci.contig = true; // addresses contigues
			  p_vci.cons = false;
			  p_vci.wrap = false;
			  p_vci.plen = r_burst_count.read()*vci_param::B;
			  p_vci.clen = 0; // pas de chainage de paquets
			  if (p_vci.rspval == true) p_vci.rspack = true; else p_vci.rspack = false;
			  
			  // bus AVALON  ==========================================
			  p_avalon.readdata =  p_vci.rdata.read() ; 
			  p_avalon.waitrequest = false;
			  p_avalon.readdatavalid = false;
			  
			  break;	 //=========fin FSM_LAST_ADV_WRITE		  
			  
   		
      } // end switch

#if DEBUG_TARGET_WRAPPER
      std::cout << "==============================================>   sortie genMealy   vci_valon_target" << std::endl;
#endif

    }; // end genMealy
  }
} // end namespace
