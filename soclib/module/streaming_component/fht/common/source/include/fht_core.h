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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors :
 * 
 * History :
 *
 * Comment :
 *
 */

#ifndef _FHT_CORE_H_
#define _FHT_CORE_H_

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "core_tlm.h"
#include "fht_core_config.h"

#ifdef TLM_POWER_ESTIMATION
#include "tlm_power_core.h"
#endif
/*------------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * CLASS: fht_core
 *
 *----------------------------------------------------------------------------*/
class fht_core :
  public core_tlm
  #ifdef TLM_POWER_ESTIMATION
  ,public tlm_power_core
  #endif
{

  //SC_HAS_PROCESS(fht_core);

 private:
  // internal parameters
  //  Matrice Walsh Hadamard
  int  C [32][32] ;  

 // read and write timing

  // internal paremeters
  t_uint32 write_cycles;
  t_uint32 read_cycles;
  t_uint32 compute_cycles;
//   int write_wait_state;
//   int read_wait_state;
  
/*------------------------------------------------------------------------------
 * Methods                                                             
 *----------------------------------------------------------------------------*/

  public: 
    //------------------------------------------------------------
    // Main thread
    //------------------------------------------------------------

    virtual void compute();

    // Constructor 
    fht_core(sc_module_name module_name_,
              int clk_period_,
              bool multicore_,
              t_uint16 core_id_,
              t_uint16 nb_fifo_in_,
              t_uint16 *size_fifo_in_,
              t_uint16 nb_fifo_out_,
              t_uint16 *size_fifo_out_,
              t_uint16 nb_cfg_core_,
              bool static_init_,
              string sno_name_);

    // Destructor
    virtual ~fht_core() {};

    // Address map
    virtual void write_register(t_uint32 addr, t_uint32 data);
    virtual t_uint32 read_register(t_uint32 addr);

    void end_of_elaboration(){
#ifdef TLM_POWER_ESTIMATION
      sc_spawn_options opt;
      opt.spawn_method();
      opt.set_sensitivity(&(lpm->new_power_mode_evt));
      opt.dont_initialize();
      sc_spawn(sc_bind(&Power::tlm_power_mode::new_power_mode, this), sc_gen_unique_name("new_power_mode_fht"), &opt);
#endif
    }
};

#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _FHT_CORE_H_ */


