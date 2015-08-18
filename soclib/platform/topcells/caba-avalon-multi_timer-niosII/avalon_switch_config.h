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
 * File : avalon_switch_config.h dedicated to caba_avalon_multitimer_nios2 platform
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_SWITCH_CONFIG_H_
#define SOCLIB_CABA_AVALON_SWITCH_CONFIG_H_

#include <systemc.h>

#include "avalon_switch_fabric_param_master.h"
#include "avalon_switch_fabric_param_slave.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template<int NB_MASTER, int NB_SLAVE>
      class AvalonSwitchConfig {   

    public:	
      int n_master;
      int n_slave;

      int i, j;

      soclib::caba::SwitchFabricParamMaster<NB_MASTER, NB_SLAVE> ** SwitchFabricParam_Master;
      soclib::caba::SwitchFabricParamSlave<NB_MASTER, NB_SLAVE>   ** SwitchFabricParam_Slave;

      //constructeur
      AvalonSwitchConfig(){

	n_master = NB_MASTER;
	n_slave  = NB_SLAVE;

	SwitchFabricParam_Master = new soclib::caba::SwitchFabricParamMaster<NB_MASTER, NB_SLAVE>*[NB_MASTER+1];
	SwitchFabricParam_Slave  = new soclib::caba::SwitchFabricParamSlave<NB_MASTER, NB_SLAVE>*[NB_SLAVE+1];

	for (i=0; i<NB_MASTER; i++)
	  {
	    //std::cout << "                                                  *************** AvalonSwitchConfig : Master SwitchFabricParam " << std::endl;
	    SwitchFabricParam_Master[i]   =  new   SwitchFabricParamMaster<NB_MASTER, NB_SLAVE>;
	  }


	for (i=0; i<NB_SLAVE; i++)
	  {
	    //std::cout << "                                                  *************** AvalonSwitchConfig : Master SwitchFabricParam " << std::endl;
	    SwitchFabricParam_Slave[i]   =  new   SwitchFabricParamSlave<NB_MASTER, NB_SLAVE>;


	  }

	//=================== 4 Masters  (nios0  nios1 nios2 nios3 ) 5  Slaves (multiram0 multiram1 vcitty  vcitimer vcilocks )

	//
	//                     caba_avalon_mutitimer_nios2 plateform
	//


	//==============    MASTERS   ==================
		
	// nios0
	SwitchFabricParam_Master[0]->route[0] =  0;	                  // nios0  -> multiram0
	SwitchFabricParam_Master[0]->route[1] =  1;	                  // nios0  -> multiram1
	SwitchFabricParam_Master[0]->route[2] =  2;	                  // nios0  -> vcitty
	SwitchFabricParam_Master[0]->route[3] =  3;	                  // nios0  -> vcitimer
	SwitchFabricParam_Master[0]->route[4] =  4;	                  // nios0  -> vcilocks	
	SwitchFabricParam_Master[0]->route[5] =  -1;	              // end

	SwitchFabricParam_Master[0]->mux_n_slave =  5;	              // 5 esclaves



	//nios1
	SwitchFabricParam_Master[1]->route[0] =  0;	                  // nios1  -> multiram0
	SwitchFabricParam_Master[1]->route[1] =  1;	                  // nios1  -> multiram1
	SwitchFabricParam_Master[1]->route[2] =  2;	                  // nios1  -> vcitty
	SwitchFabricParam_Master[1]->route[3] =  3;	                  // nios1  -> vcitimer
	SwitchFabricParam_Master[1]->route[4] =  4;	                  // nios1  -> vcilocks	
	SwitchFabricParam_Master[1]->route[5] =  -1;	              // end

	SwitchFabricParam_Master[1]->mux_n_slave =  5;	              // 5 esclaves


	//nios2
	SwitchFabricParam_Master[2]->route[0] =  0;	                  // nios2  -> multiram0
	SwitchFabricParam_Master[2]->route[1] =  1;	                  // nios2  -> multiram1
	SwitchFabricParam_Master[2]->route[2] =  2;	                  // nios2  -> vcitty
	SwitchFabricParam_Master[2]->route[3] =  3;	                  // nios2  -> vcitimer
	SwitchFabricParam_Master[2]->route[4] =  4;	                  // nios2  -> vcilocks	
	SwitchFabricParam_Master[2]->route[5] =  -1;	              // end

	SwitchFabricParam_Master[2]->mux_n_slave =  5;	              // 5 esclaves


	//nios3
	SwitchFabricParam_Master[3]->route[0] =  0;	                  // nios3  -> multiram0
	SwitchFabricParam_Master[3]->route[1] =  1;	                  // nios3  -> multiram1
	SwitchFabricParam_Master[3]->route[2] =  2;	                  // nios3  -> vcitty
	SwitchFabricParam_Master[3]->route[3] =  3;	                  // nios3  -> vcitimer
	SwitchFabricParam_Master[3]->route[4] =  4;	                  // nios3  -> vcilocks	
	SwitchFabricParam_Master[3]->route[5] =  -1;	              // end

	SwitchFabricParam_Master[3]->mux_n_slave =  5;	              // 5 esclaves




	//==============    SLAVES   ==================

	// multiram0
	SwitchFabricParam_Slave[0]->route[0] =   0;	                  // multiram0  -> nios0
	SwitchFabricParam_Slave[0]->route[1] =   1;	                  // multiram0  -> nios1
	SwitchFabricParam_Slave[0]->route[2] =   2;	                  // multiram0  -> nios2
	SwitchFabricParam_Slave[0]->route[3] =   3;	                  // multiram0  -> nios3
	SwitchFabricParam_Slave[0]->route[4] =   -1;	              // fin

	SwitchFabricParam_Slave[0]->arbiter_n_master = 4;	          // 

	SwitchFabricParam_Slave[0]->Base_Address = 0x000000;              // N  --unknow-- and hexa address  default=0x000 	
	SwitchFabricParam_Slave[0]->Address_Span = 0x01001000;            // N  (0  - 2^32) default =2^Address_Width

	//	SwitchFabricParam_Slave[0]->Base_Address = 0x00800820;         	
	//SwitchFabricParam_Slave[0]->Address_Span = 0x0087E000;          
	 //SwitchFabricParam_Slave[0]->Address_Span = 0x0087F6E0; 
	//SwitchFabricParam_Slave[0]->Address_Span = 0x00880000;


	// mutiram1
	SwitchFabricParam_Slave[1]->route[0] =   0;	                  // multiram1  -> nios0
	SwitchFabricParam_Slave[1]->route[1] =   1;	                  // multiram1  -> nios1
	SwitchFabricParam_Slave[1]->route[2] =   2;	                  // multiram1  -> nios2
	SwitchFabricParam_Slave[1]->route[3] =   3;	                  // multiram1  -> nios3
	SwitchFabricParam_Slave[1]->route[4] =   -1;	              // fin

	SwitchFabricParam_Slave[1]->arbiter_n_master =  4;	         

	SwitchFabricParam_Slave[1]->Base_Address = 0x02000000;         	
	SwitchFabricParam_Slave[1]->Address_Span = 0x00080000;           
		
	// vcitty
	SwitchFabricParam_Slave[2]->route[0] =   0;	                  // vcitimer  -> nios0
	SwitchFabricParam_Slave[2]->route[1] =   1;	                  // vcitimer  -> nios1
	SwitchFabricParam_Slave[2]->route[2] =   2;	                  // vcitimer  -> nios2
	SwitchFabricParam_Slave[2]->route[3] =   3;	                  // vcitimer  -> nios3
	SwitchFabricParam_Slave[2]->route[4] =   -1;	              // fin
		
	SwitchFabricParam_Slave[2]->arbiter_n_master =  4;	         

	SwitchFabricParam_Slave[2]->Base_Address = 0xC0200000;         	
	SwitchFabricParam_Slave[2]->Address_Span = 0x00000258;  
		

	// vcitimer
	SwitchFabricParam_Slave[3]->route[0] =   0;	                  // vcitty  -> nios0
	SwitchFabricParam_Slave[3]->route[1] =   1;	                  // vcitty  -> nios1
	SwitchFabricParam_Slave[3]->route[2] =   2;	                  // vcitty  -> nios2
	SwitchFabricParam_Slave[3]->route[3] =   3;	                  // vcitty  -> nios3
	SwitchFabricParam_Slave[3]->route[4] =   -1;	              // fin
		
	SwitchFabricParam_Slave[3]->arbiter_n_master =  4;	         

	SwitchFabricParam_Slave[3]->Base_Address = 0xB0200000;         	
	SwitchFabricParam_Slave[3]->Address_Span = 0x00000100;           
		

	// vcilocks
	SwitchFabricParam_Slave[4]->route[0] =   0;	                  // vcilocks  -> nios0
	SwitchFabricParam_Slave[4]->route[1] =   1;	                  // vcilocks  -> nios1
	SwitchFabricParam_Slave[4]->route[2] =   2;	                  // vcilocks  -> nios2
	SwitchFabricParam_Slave[4]->route[3] =   3;	                  // vcilocks -> nios3
	SwitchFabricParam_Slave[4]->route[4] =  -1;	                  // fin
		
	SwitchFabricParam_Slave[4]->arbiter_n_master =  4;	         

	SwitchFabricParam_Slave[4]->Base_Address = 0xB2200000;         	
	SwitchFabricParam_Slave[4]->Address_Span = 0x00001000;  

      }
    };  
    
  }
}// end namespace

#endif

