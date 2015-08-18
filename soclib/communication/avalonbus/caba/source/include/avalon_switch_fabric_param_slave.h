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
 * 		   Charles Wagner <wagner@irisa.fr>
 * 
 * Maintainer: wagner
 * 
 * File : avalon_switch_fabric_param_slave.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_SWITCH_FABRIC_PARAM_SLAVE_H_
#define SOCLIB_CABA_AVALON_SWITCH_FABRIC_PARAM_SLAVE_H_

#include <systemc>
#include "static_assert.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template<int NB_MASTER, int NB_SLAVE>
    class SwitchFabricParamSlave
    {
    public :

      // routage : slave --> master
      int route[NB_MASTER+1];            //liste slave  connect� � maitre  X,- 1 fin
      int arbiter_n_master;              // nombre de maitres connect�s � l'arbitre- a deduire de route : -1 pas d'arbitre

      // all
      int Address_Width;                // Y  1-32  default=0
      int Data_Width;                   // Y  slaves:8,16,32  masters: 16,32
      int direction;                    // Y  inout, input, output
      int width;                        // Y  1-infinity

      int Base_Address;                 // N  --unknow-- and hexa address  default=0x000 	
      int Register_Incoming_Signals;    // N  0,1  default=0 
      int Register_Outgoing_Signals;    // N  0,1  default=0 

      // slave  
      int Address_Alignment;               // Y  dynamic, native default=native
      int IRQ_Number;                      // Y   16-63
      int priority;                        // Y   0-100 default=0 ou 1   
      int Active_CS_Through_Read_Latency;  // N  0,1  default=0
      int Address_Span;                    // N  (0  - 2^32) default =2^Address_Width
      int Has_Base_Address;                // N   0,1  default=0
      int Has_IRQ;                         // N   0,1  
      int Hold_Time;                       // N   0-256  default=0
      int Minimum_Span;                    // N   (0  - 2^32) default=0
      int Read_Latency;                    // N   1-8  default=0
      int Read_Wait_States;                // N   0-255  default=0
      int Setup_Time;                      // N   0-256  default=0
      int Write_Wait_State;                // N   0-255 

      // constructor
      SwitchFabricParamSlave<NB_MASTER, NB_SLAVE>()
      {
	for (int i=0; i < NB_MASTER+1; i++)
	  {
	    route[i] = -1;
	  }
      }
    };

  }}

#endif /* AVALON_SWITCH_FABRIC_PARAM_SLAVE */

