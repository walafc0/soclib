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

#ifndef _TRX_OFDM_CORE_RO_CONFIG_H_
#define _TRX_OFDM_CORE_RO_CONFIG_H_

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "core_ro_config.h"

/*------------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------------*/

// start address for core configuration
#define TRX_OFDM_CORE_CFG_BEGIN 0 

// configuration slot size
#define TRX_OFDM_CORE_CFG_SIZE 256 

// configuration flit size
#define TRX_OFDM_CORE_CFG_FLIT_SIZE 132

// configuration parameters : position and size in flits
// FFT_CFG
#define TRX_OFDM_CORE_LOG2_SIZE_FFT_POS   0
#define TRX_OFDM_CORE_LOG2_SIZE_FFT_SIZE  4
#define TRX_OFDM_CORE_NORM_POWER_POS      5
#define TRX_OFDM_CORE_FFT_TYPE_POS        6
#define TRX_OFDM_CORE_BYPASS_FFT_POS      7
#define TRX_OFDM_CORE_SHIFT_CARRIER_POS   8
#define TRX_OFDM_CORE_SHIFT_PARITY_POS    9
// GI_CFG
#define TRX_OFDM_CORE_GI_SIZE_POS         0
#define TRX_OFDM_CORE_GI_SIZE_SIZE        11
#define TRX_OFDM_CORE_GI_INSERT_POS       31
// FRAMING_CFG
#define TRX_OFDM_CORE_FLOC_POS            0
#define TRX_OFDM_CORE_FLOC_SIZE           2
// IT CFG
#define TRX_OFDM_CORE_MASKIT_POS          0

#define TRX_OFDM_CORE_NB_MASK             64   // 64*32 => 2048 possible mask values

typedef enum  {dp1fifo=0, dp2fifo=1} framing_loc; // data, pilot meme fifo 0 / data fifo 0, pilot fifo 1
typedef enum  {fft=1, ifft=0} fft_t; 
// ...
// ##########


/*------------------------------------------------------------------------------
 * CLASS: trx_ofdm_core_ro_config
 *----------------------------------------------------------------------------*/
class trx_ofdm_core_ro_config :
  public virtual core_ro_config
{
  /*------------------------------------------------------------------------------
   * Parameters                                                             
   *----------------------------------------------------------------------------*/
  public:

    // configuration parameters : variables declaration

    // ##########
    // declare specific parameters
    t_uint16 log2_size_fft;
    bool  norm_power;
    fft_t fft_type;
    bool bypass_fft;
    bool shift_carrier;
    bool shift_parity;
    t_uint16 gi_size;
    bool gi_insertion;
    framing_loc floc;
    bool mask_IT;
    t_uint32 *mask_data;
    t_uint32 *mask_pilot;
    // ...
    // ##########

    /*------------------------------------------------------------------------------
     * Methods                                                             
     *----------------------------------------------------------------------------*/
  public:
    // Constructor
    trx_ofdm_core_ro_config();

    // Destructor
    virtual ~trx_ofdm_core_ro_config();

    // Address map
    virtual t_uint32 read_any_register(t_uint32 addr_, t_uint32 block_id_);

    // Fill in the core config table with new core config objects of type trx_ofdm (static function)
    static void init_core_ro_config_table(core_ro_config_table *cfg_core);

    // Load configurations from catalog file (static function)
    static void init_ro_config_from_file( t_uint16 level,
                                          core_ro_config_table *cfg_core,
                                          t_uint16 nb_cfg_core_to_load,
                                          t_uint16 *num_cfg_core,
                                          string core_name);

    static void init_ro_config_from_file( t_uint16 level,
                                          core_ro_config_table *cfg_core,
                                          t_uint16 nb_cfg_core_to_load,
                                          t_uint16 *num_cfg_core,
                                          string core_name,
                                          string file_name);

    // Get config parameters
    inline virtual t_uint32 get_config_size() {return TRX_OFDM_CORE_CFG_SIZE;}
    inline virtual t_uint16 get_config_flit_size() {return TRX_OFDM_CORE_CFG_FLIT_SIZE;}

    virtual void set_ro_config(const core_ro_config* const p_ro_config);
};

// trace functions
ostream& operator << (ostream& os, const fft_t& fft_type);
ostream& operator << (ostream& os, const framing_loc& floc);

#endif /* _TRX_OFDM_CORE_RO_CONFIG_H_ */


