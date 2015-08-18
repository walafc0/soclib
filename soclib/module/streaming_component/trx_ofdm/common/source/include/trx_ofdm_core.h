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

#ifndef _TRX_OFDM_CORE_H_
#define _TRX_OFDM_CORE_H_

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "core_tlm.h"
#include "trx_ofdm_core_config.h"
#include "fft_radix_4_2.h"

#ifdef TLM_POWER_ESTIMATION
#include "tlm_power_core.h"
#endif
/*------------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------------*/
#define NB_BUF 2     // number of buffers for multiple buffering computation
#define NB_SLOT 3    // number of configuration slots : must be >=3
/*------------------------------------------------------------------------------
 * CLASS: trx_ofdm_core
 *
 *----------------------------------------------------------------------------*/
class trx_ofdm_core :
  public core_tlm
#ifdef TLM_POWER_ESTIMATION
  ,public tlm_power_core
#endif
{

 private:
  // internal buffers for double buffering
  t_uint32 ** fft_in_buf;  
  t_uint32 ** fft_out_buf;
  // flags for input, output and compute buffer selection
  int cur_buf_in;
  int cur_buf_out;
  int cur_buf_fft;
  sc_fifo<int> *buf_fft;
  sc_fifo<int> *buf_deframing;
  // slot_id 
  int framing_slot_id;
  sc_fifo<int> *fft_slot_id;
  sc_fifo<int> *deframing_slot_id;
  
  // flags and associated events for framing to fft and fft to framing synchro
  int nb_buf_in_free;
  sc_event buf_in_free;

  bool *buf_out_free_flag;
  sc_event buf_out_free;

/*------------------------------------------------------------------------------
 * Methods                                                             
 *----------------------------------------------------------------------------*/

  public: 
    //------------------------------------------------------------
    // Main thread
    //------------------------------------------------------------

    virtual void compute();
    void compute_fft();
    void deframing();
    void framing(fft_t fft_type, t_uint16 log2_size_fft, framing_loc floc, t_uint32 * mask_data, t_uint32 * mask_pilot);

    // Constructor 
    SC_HAS_PROCESS(trx_ofdm_core);
    trx_ofdm_core(sc_module_name module_name_,
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
    virtual ~trx_ofdm_core() {};

    // Address map
    virtual void write_register(t_uint32 addr, t_uint32 data);
    virtual t_uint32 read_register(t_uint32 addr);
		
    void end_of_elaboration(){
#ifdef TLM_POWER_ESTIMATION
      sc_spawn_options opt;
      opt.spawn_method();
      opt.set_sensitivity(&(lpm->new_power_mode_evt));
      opt.dont_initialize();
      sc_spawn(sc_bind(&Power::tlm_power_mode::new_power_mode, this),sc_gen_unique_name("new_power_mode_trx_ofdm"), &opt);
#endif
    }
};

#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _TRX_OFDM_CORE_H_ */


