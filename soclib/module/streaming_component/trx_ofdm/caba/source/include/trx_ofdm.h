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

#ifndef _TRX_OFDM_H_
#define _TRX_OFDM_H_

// ------------------------------------------
// -- includes
// ------------------------------------------
#include "trx_ofdm_core.h"
#include "anoc_copro_wrapper.h"
#include "caba_base_module.h"
#include "fifo_ports.h"

// ------------------------------------------
// -- typedefs
// ------------------------------------------
typedef trx_ofdm_core core;


namespace soclib { namespace caba {
using namespace sc_core;

class trx_ofdm :
  public sc_module {

 private:
  t_uint16 *size_fifo_in;
  t_uint16 *size_fifo_out;

  
// ------------------------------------------
// -- resource instances
// ------------------------------------------
  core                  *res_core;
  anoc_copro_wrapper    *res_wrapper;
  core_base             **wrapper_cores;


  int nb_cores;
  int nb_fifo_in;
  int nb_fifo_out;
  int clk_period;

 public:

// ------------------------------------------
// -- external ports instances
// ------------------------------------------
    sc_in<bool>                  p_clk;
    sc_in<bool>                  p_resetn;
    FifoInput<uint32_t>         *p_from_MWMR;
    FifoOutput<uint32_t>        *p_to_MWMR;
    FifoInput<uint32_t>          p_core_config;
    sc_core::sc_in<uint32_t>    *p_config;
    sc_core::sc_out<uint32_t>   *p_status;


// ------------------------------------------
// -- encapsulated tops instances
// ------------------------------------------


// ------------------------------------------
// -- methods
// ------------------------------------------
  void get_size_of_fifos(t_uint16* size_fifo_in, t_uint16* size_fifo_out);
  void bind_cores_to_wrapper(core_base **cores);

// ------------------------------------------
// -- constructor
// ------------------------------------------
  SC_HAS_PROCESS(trx_ofdm);
  trx_ofdm(sc_module_name name, t_uint32 clk_period_);

// ------------------------------------------
// -- destructor
// ------------------------------------------
  ~trx_ofdm();

};
}}
#endif // _TRX_OFDM_H_

