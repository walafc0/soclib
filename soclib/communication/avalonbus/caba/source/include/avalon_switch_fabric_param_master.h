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
 * File : avalon_switch_fabric_param_master.h
 * Date : 20/11/2008
 */

#ifndef SOCLIB_CABA_AVALON_SWITCH_FABRIC_PARAM_MASTER_H_
#define SOCLIB_CABA_AVALON_SWITCH_FABRIC_PARAM_MASTER_H_

#include <systemc>
#include "static_assert.h"

namespace soclib { namespace caba {

    using namespace sc_core;

    template<int NB_MASTER, int NB_SLAVE>
    class SwitchFabricParamMaster
    {
    public :

      // routage : master --> slaves 
      int route[NB_SLAVE+1];          // master connect� � esclave  X,-1 fin
      int mux_n_slave;                // nombre d'esclave connect�s au MUX - a deduire de route : -1 pas de MUX

      // all
      int Address_Width;              // Y  1-32  default=0
      int Data_Width;                 // Y  slaves:8,16,32  masters: 16,32
      int direction;                  // Y  inout, input, output
      int width;                      // Y  1-infinity

      int Base_Address;                // N  --unknow-- and hexa address  default=0x000 	
      int Register_Incoming_Signals;   // N  0,1  default=0 
      int Register_Outgoing_Signals;   // N  0,1  default=0 

      // master
      int Do_Stream_Reads;             // N  0,1  default=0
      int Do_Stream_Writes;            // N  0,1 default=0
      int Interrupts_Enabled;          // N  0,1 default=0
      int Irq_Schem;                   // N  individual_requests
      int irq0;                        // N  <module_name>/<slave_name>  default=""
      //     idem pour irq1, irq2,....irqN
      int Max_Address_Width;             // N  1-32  default=16 or 32

      // constructeur
      SwitchFabricParamMaster<NB_MASTER, NB_SLAVE>()
      {
      }
    };
  }}

#endif /* AVALON_SWITCH_FABRIC_PARAM_MASTER_*/


