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


/*------------------------------------------------------------------------------
 * Includes                                                             
 *----------------------------------------------------------------------------*/

#include  "trx_ofdm_core_config.h"

/*------------------------------------------------------------------------------
 * CLASS: trx_ofdm_core_config
 *----------------------------------------------------------------------------*/

// Constructor
trx_ofdm_core_config::trx_ofdm_core_config()
  : trx_ofdm_core_ro_config(),
    core_config() {
    }

// Destructor
trx_ofdm_core_config::~trx_ofdm_core_config() {
}


// Address map
void trx_ofdm_core_config::write_register(t_uint32 addr, t_uint32 data) {
  sc_lv<32> word;
  word = data;
	
  // ##########
  // code specific address map
  //
  switch(addr) {
  case 128:
     log2_size_fft = word.range(TRX_OFDM_CORE_LOG2_SIZE_FFT_POS+TRX_OFDM_CORE_LOG2_SIZE_FFT_SIZE-1,TRX_OFDM_CORE_LOG2_SIZE_FFT_POS).to_uint();
     norm_power = word[TRX_OFDM_CORE_NORM_POWER_POS].to_bool();
     if (word[TRX_OFDM_CORE_FFT_TYPE_POS].to_bool()==(bool)fft)
       fft_type = fft;
     else
       fft_type = ifft;
     bypass_fft = word[TRX_OFDM_CORE_BYPASS_FFT_POS].to_bool();
     shift_carrier = word[TRX_OFDM_CORE_SHIFT_CARRIER_POS].to_bool();
     shift_parity = word[TRX_OFDM_CORE_SHIFT_PARITY_POS].to_bool();
     config_loaded = true;
    break;
  case 129:
    gi_size = word.range(TRX_OFDM_CORE_GI_SIZE_POS+TRX_OFDM_CORE_GI_SIZE_SIZE-1,TRX_OFDM_CORE_GI_SIZE_POS).to_uint();
    gi_insertion = word[TRX_OFDM_CORE_GI_INSERT_POS].to_bool();
    break;
  case 130:
    if (word.range(TRX_OFDM_CORE_FLOC_POS+TRX_OFDM_CORE_FLOC_SIZE-1,TRX_OFDM_CORE_FLOC_POS).to_uint() == 0)
      floc = dp1fifo;
    else
      floc = dp2fifo;
    break;
  case 131:
    mask_IT = word[TRX_OFDM_CORE_MASKIT_POS].to_bool();
    print_config();
    break;
  default:
    if (addr>=0 && addr < (TRX_OFDM_CORE_NB_MASK)) 
      mask_data[addr] = word.to_uint();
    if (addr>=(TRX_OFDM_CORE_NB_MASK) && addr < (2*TRX_OFDM_CORE_NB_MASK)) 
      mask_pilot[addr-TRX_OFDM_CORE_NB_MASK] = word.to_uint();
    break;
  }
  // ...
  // ##########
}


t_uint32 trx_ofdm_core_config::read_register(t_uint32 addr) {
  return(0);
}

// Print configuration parameters
void trx_ofdm_core_config::print_config() {

  cout << "*** CORE config";
  if(config_loaded) {
    cout << " (loaded)";
  } else {
    cout << " (not loaded)";
  }
  cout << " ***" << endl;

  // ##########
  // print parameters
  cout << "fft_type: " << fft_type << endl;
  cout << "fft_size(log2): " << log2_size_fft << endl;
  cout << "norm_power: " << norm_power << endl;
  cout << "bypass_fft: " << bypass_fft << endl;
  cout << "shift_carrier: " << shift_carrier << endl;
  cout << "shift_parity: " << shift_parity << endl;
  cout << "gi_insertion: " << gi_insertion << endl;
  cout << "gi_size: " << gi_size << endl;
  cout << "floc: " << floc << endl;
  cout << "mask_IT: " <<mask_IT << endl;
  for (int i = 0; i<TRX_OFDM_CORE_NB_MASK; i++)
    cout << "mask_data[" << i << "]: " <<hex << mask_data[i] << endl;
  for (int i = 0; i<TRX_OFDM_CORE_NB_MASK; i++)
    cout << "mask_pilot[" << i << "]: " <<mask_pilot[i] << endl;
  cout << dec << endl;
  // ...
  // ##########
  
}

// Fill in the cfg_core table with objects of type "trx_ofdm_core_config"
void trx_ofdm_core_config::init_core_config_table(core_config_table *cfg_core) {

  t_uint16 nb_cfg_core;

  nb_cfg_core = cfg_core->get_nb_cfg_core();
  
  for (int i = 0; i < nb_cfg_core; i++) {
    cfg_core->config[i] = new trx_ofdm_core_config(); 
  }  
}

void trx_ofdm_core_config::init_config_from_file( t_uint16 level,
                                                  core_config_table *cfg_core_,
                                                  t_uint16 nb_cfg_core_to_load,
                                                  t_uint16 *num_cfg_core,
                                                  string core_name) {
  
  /* initialize core_ro_config_table */
  core_ro_config_table tmp_ro_cfg_core( cfg_core_->core_id,
                                        cfg_core_->get_nb_cfg_core(),
                                        cfg_core_->multicore);
  core_ro_config_table *cfg_core = &tmp_ro_cfg_core;
  trx_ofdm_core_ro_config::init_core_ro_config_table(cfg_core);
  trx_ofdm_core_ro_config::init_ro_config_from_file( level,
                                                     cfg_core,
                                                     nb_cfg_core_to_load,
                                                     num_cfg_core,
                                                     core_name);

  /* Fill core_cfg_ */
  for (unsigned int i=0; i<nb_cfg_core_to_load; i++){
    cfg_core_->config[i]->set_ro_config(cfg_core->config[i]);
  }
}

// ##########

