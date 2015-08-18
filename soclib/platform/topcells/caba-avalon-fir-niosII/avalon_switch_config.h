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
 * File : avalon_switch_config.h dedicated to caba_avalon_fir_niosII platform
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
	// std::cout << "*************** AvalonSwitchConfig constructeur " << std::endl;
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

	//===================   1 Maitre (nios) 3 Slaves (RAM, tty, timer)
	//
	//                caba_avalon_fir_nios2f platform
	//

	//==============    MASTERS   ==================
	// nios : master1
	SwitchFabricParam_Master[0]->route[0] =  0;	                  // master0  -> slave0
	SwitchFabricParam_Master[0]->route[1] =  1;	                  // master0  -> slave1	
	SwitchFabricParam_Master[0]->route[2] =  2;	                  // master0  -> slave2	
	SwitchFabricParam_Master[0]->route[3] =  -1;	                  // end

	SwitchFabricParam_Master[0]->mux_n_slave =  3;	                  // 3 esclaved


	
	//==============    SLAVES   ==================		
	// RAM : slave1
	SwitchFabricParam_Slave[0]->route[0] =   0;	                  // slave0  -> master 0
	SwitchFabricParam_Slave[0]->route[1] =   -1;	                  // end	
	SwitchFabricParam_Slave[0]->arbiter_n_master = 1;	          // 

	SwitchFabricParam_Slave[0]->Base_Address = 0x000000;              // N  --unknow-- and hexa address  default=0x000 	
	SwitchFabricParam_Slave[0]->Address_Span = 0x03000000;            // N  (0  - 2^32) default =2^Address_Width

	// slave 1
	SwitchFabricParam_Slave[1]->route[0] =  0;	                 // slave1  -> master0
	SwitchFabricParam_Slave[1]->route[1] = -1;	                 // end
	SwitchFabricParam_Slave[1]->arbiter_n_master =  1;	         // 

	SwitchFabricParam_Slave[1]->Base_Address = 0xC0200000;           // N  --unknow-- and hexa address  default=0x000 	
	SwitchFabricParam_Slave[1]->Address_Span = 0X256;                // N  (0  - 2^32) default =2^Address_Width
	
	// slave 2
	SwitchFabricParam_Slave[2]->route[0] =  0;	                 // slave2  -> master0
	SwitchFabricParam_Slave[2]->route[1] = -1;	                 // end
	SwitchFabricParam_Slave[2]->arbiter_n_master =  1;	         // 

	SwitchFabricParam_Slave[2]->Base_Address = 0xB0200000;           // N  --unknow-- and hexa address  default=0x000 	
	SwitchFabricParam_Slave[2]->Address_Span = 0x00000100;                // N  (0  - 2^32) default =2^Address_Width

      }
    };  
  
  
  }
}// end namespace

#endif

