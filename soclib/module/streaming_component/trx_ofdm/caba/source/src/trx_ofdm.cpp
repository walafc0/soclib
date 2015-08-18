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

#include "trx_ofdm.h"
#include "alloc_elems.h"

//--------------------------------------------------
// exports for cosimulation
//--------------------------------------------------
#ifdef NC_SYSTEMC
NCSC_MODULE_EXPORT(trx_ofdm)
#endif
#ifdef MENTORG
SC_MODULE_EXPORT(trx_ofdm);
#endif

//--------------------------------------------------
// resource parameters
//--------------------------------------------------

//Structural parameters
#define TRX_OFDM_NB_FIFO_IN       2
#define TRX_OFDM_SIZE_FIFO_IN0   16
#define TRX_OFDM_SIZE_FIFO_IN1   16
#define TRX_OFDM_NB_FIFO_OUT      2
#define TRX_OFDM_SIZE_FIFO_OUT0  16
#define TRX_OFDM_SIZE_FIFO_OUT1  16
#define TRX_OFDM_NB_CFG_MONOCORE  3
#define TRX_OFDM_SNO_NAME "res_trx_ofdm"

#define TRX_OFDM_NB_CORES        1
#define TRX_OFDM_NB_CONFIG_REGS  6
#define TRX_OFDM_NB_STATUS_REGS  5

namespace soclib { namespace caba {
using soclib::common::alloc_elems;
using soclib::common::dealloc_elems;


//--------------------------------------------------
// -- constructor
//--------------------------------------------------
trx_ofdm::trx_ofdm(sc_module_name name, 
                           t_uint32 clk_period_):
                             sc_module(name),
                             clk_period(clk_period_),
                             p_clk("p_clk"),
                             p_resetn("p_resetn"),
                             p_from_MWMR(alloc_elems<FifoInput<uint32_t> >("p_from_MWMR", TRX_OFDM_NB_FIFO_IN)),
                             p_to_MWMR(alloc_elems<FifoOutput<uint32_t>  >("p_to_MWMR", TRX_OFDM_NB_FIFO_OUT)),
                             p_core_config("p_core_config"),
                             p_config(alloc_elems<sc_in<uint32_t>  >("p_config", TRX_OFDM_NB_CONFIG_REGS)),
                             p_status(alloc_elems<sc_out<uint32_t> >("p_status", TRX_OFDM_NB_STATUS_REGS))
{
  printf("Constructor of: trx_ofdm\n");

  nb_cores    = TRX_OFDM_NB_CORES;
  nb_fifo_in  = TRX_OFDM_NB_FIFO_IN;
  nb_fifo_out = TRX_OFDM_NB_FIFO_OUT;

  size_fifo_in  = new t_uint16[nb_fifo_in];
  size_fifo_out = new t_uint16[nb_fifo_out];

  get_size_of_fifos(size_fifo_in,size_fifo_out);

  wrapper_cores = new core_base*[nb_cores];

  // Resource creation
  res_wrapper = new anoc_copro_wrapper("res_wrapper", nb_cores, nb_fifo_in, nb_fifo_out, TRX_OFDM_NB_CONFIG_REGS, TRX_OFDM_NB_STATUS_REGS, clk_period);
  res_core    = new core ("CORE_TLM",
                           clk_period,
                           false, 0,
                           nb_fifo_in,
                           size_fifo_in,
                           nb_fifo_out,
                           size_fifo_out,
                           TRX_OFDM_NB_CFG_MONOCORE,
                           false,
                           TRX_OFDM_SNO_NAME);


  wrapper_cores[0] = (core_base *)res_core;

  // Binding
  // Core to anoc_copro_wrapper
  bind_cores_to_wrapper(wrapper_cores);
  EXPRINT(top, 0, "res_trx_ofdm binded");

  // External port binding
  res_wrapper->p_clk(p_clk);
  res_wrapper->p_resetn(p_resetn);
  for (int i=0; i< nb_fifo_in; i++) {
    res_wrapper->p_from_MWMR[i](p_from_MWMR[i]);
  }
  for (int i=0; i< nb_fifo_out; i++) {
    res_wrapper->p_to_MWMR[i](p_to_MWMR[i]);
  }

  res_wrapper->p_core_config(p_core_config);

  for (int i=0; i< TRX_OFDM_NB_CONFIG_REGS; i++) {
    res_wrapper->p_config[i](p_config[i]);
  }
  for (int i=0; i< TRX_OFDM_NB_STATUS_REGS; i++) {
    res_wrapper->p_status[i](p_status[i]);
  }
  // message
  EXPRINT(top, 0,"trx_ofdm created\n");

} // end


// Bind ports to core
void trx_ofdm::bind_cores_to_wrapper(core_base **cores) {

//  t_uint16 core_id;

  for(int i=0; i < nb_cores; i++) {
    if (!cores[i]) {
      EXPRINT(res, 0,"ERROR : CORE " << i << " not allocated");
      exit(0);
    }
  }
//  if(nb_cores>1) {
//    if(!arbiter) {
//      EXPRINT(res, 0,"ERROR : CORE arbiter not allocated");
//      exit(0);
//    }
//  }
  if(!res_wrapper) {
    EXPRINT(res, 0,"ERROR : NI not allocated");
    exit(0);
  } else {
    EXPRINT(res, 0, "CORE Binding to Wrappers, Arbiter and NI");
  }
  int nb_fifo_in_cores=0;
  for(int i=0; i < nb_cores; i++) {
    nb_fifo_in_cores+=cores[i]->nb_fifo_in;
  }
  if(nb_fifo_in_cores!=res_wrapper->nb_fifo_in) {
    EXPRINT(res, 0,"ERROR : nb_fifo_in in CORE(S) and NI do not match");
    exit(0);
  }
  int nb_fifo_out_cores=0;
  for(int i=0; i < nb_cores; i++) {
    nb_fifo_out_cores+=cores[i]->nb_fifo_out;
  }
  if(nb_fifo_out_cores!=res_wrapper->nb_fifo_out) {
    EXPRINT(res, 0,"ERROR : nb_fifo_out in CORE(S) and NI do not match");
    exit(0);
  }

  // bind ports wrapper => core
  for(int i=0; i<nb_cores; i++) {
    res_wrapper->ni_exec_out[i]->bind(cores[i]->ni_exec_in);
  }
//  t_uint16 fifo_id[nb_cores];
  if(nb_cores>1) {
//    res_wrapper->ni_cfg_dump_out->bind(arbiter->ni_cfg_dump_in);
//    for(int i=0; i<nb_cores; i++) {
//      arbiter->ni_cfg_dump_out[i]->bind(cores[i]->ni_cfg_dump_in);
//    }
//    for(int i=0; i<nb_cores; i++) {
//      fifo_id[i]=0;
//    }
//    for(int i=0;i<nb_fifo_in;i++) {
//      core_id = core_fifo_in[i];
//      res_wrapper->ni_input_fifo_out[i]->bind(cores[core_id]->ni_input_fifo_in[fifo_id[core_id]]);
//      fifo_id[core_id]++;
//    }
//
  } else {
    res_wrapper->ni_cfg_dump_out->bind(cores[0]->ni_cfg_dump_in);
    for(int i=0;i<nb_fifo_in;i++) {
      res_wrapper->ni_input_fifo_out[i]->bind(cores[0]->ni_input_fifo_in[i]);
      cores[0]->ni_released_out[i]->bind(res_wrapper->ni_released_in[i]);
    }
  }

  // bind ports core => wrapper
  for(int i=0; i<nb_cores; i++) {
    cores[i]->ni_status_out->bind(res_wrapper->ni_status_in[i]);
  }
  if(nb_cores>1) {
//    for(int i=0; i<nb_cores; i++) {
//      fifo_id[i]=0;
//    }    
//    for(int i=0;i<nb_fifo_in;i++) {
//      core_id = core_fifo_in[i];
//      cores[core_id]->ni_released_out[fifo_id[core_id]]->bind(res_wrapper->ni_released_in[i]);
//      fifo_id[core_id]++;
//    }
//    for(int i=0; i<nb_cores; i++) {
//      fifo_id[i]=0;
//    } 
//    for(int i=0;i<nb_fifo_out;i++) {
//      core_id = core_fifo_out[i];
//      cores[core_id]->ni_output_fifo_out[fifo_id[core_id]]->bind(res_wrapper->ni_output_fifo_in[i]);
//      cores[core_id]->ni_available_out[fifo_id[core_id]]->bind(res_wrapper->ni_available_in[i]);
//      fifo_id[core_fifo_out[i]]++;    
//    }
  } else {
    for(int i=0;i<nb_fifo_out;i++) {
      cores[0]->ni_output_fifo_out[i]->bind(res_wrapper->ni_output_fifo_in[i]);      
      cores[0]->ni_available_out[i]->bind(res_wrapper->ni_available_in[i]);  
    }
  }
  EXPRINT(res, 0,"Binding done");
 
}

void trx_ofdm::get_size_of_fifos(t_uint16* size_fifo_in,t_uint16* size_fifo_out) {
#ifdef TRX_OFDM_SIZE_FIFO_IN0
  size_fifo_in[0] = TRX_OFDM_SIZE_FIFO_IN0;
#endif
#ifdef TRX_OFDM_SIZE_FIFO_IN1
  size_fifo_in[1] = TRX_OFDM_SIZE_FIFO_IN1;
#endif
#ifdef TRX_OFDM_SIZE_FIFO_IN2
  size_fifo_in[2] = TRX_OFDM_SIZE_FIFO_IN2;
#endif
#ifdef TRX_OFDM_SIZE_FIFO_IN3
  size_fifo_in[3] = TRX_OFDM_SIZE_FIFO_IN3;
#endif
#ifdef TRX_OFDM_SIZE_FIFO_OUT0
  size_fifo_out[0] = TRX_OFDM_SIZE_FIFO_OUT0;
#endif
#ifdef TRX_OFDM_SIZE_FIFO_OUT1
  size_fifo_out[1] = TRX_OFDM_SIZE_FIFO_OUT1;
#endif
#ifdef TRX_OFDM_SIZE_FIFO_OUT2
  size_fifo_out[2] = TRX_OFDM_SIZE_FIFO_OUT2;
#endif
#ifdef TRX_OFDM_SIZE_FIFO_OUT3
  size_fifo_out[3] = TRX_OFDM_SIZE_FIFO_OUT3;
#endif

}


// ------------------------------------------
// -- destructor
// ------------------------------------------
trx_ofdm::~trx_ofdm() {

    // delete resources
    delete res_core;
    delete res_wrapper;
    delete size_fifo_in;
    delete size_fifo_out;

};
}}

