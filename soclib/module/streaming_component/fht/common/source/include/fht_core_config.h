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

#ifndef _FHT_CORE_CONFIG_H_
#define _FHT_CORE_CONFIG_H_

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "anoc_common.h"
#include "res_path.h"
#include "ni_utils.h"
#include "core_config.h"
#include "anoc_transaction.h"

/*------------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------------*/

// start address for core configuration
#define FHT_CORE_CFG_BEGIN 0

// configuration slot size
#define FHT_CORE_CFG_SIZE 4

// configuration flit size
#define FHT_CORE_CFG_FLIT_SIZE 3

// configuration parameters : position and size in flits
#define FHT_CORE_FHT_SZ           0
#define FHT_CORE_FHT_SZ_SIZE      2
#define FHT_CORE_NB_SHIFT         2
#define FHT_CORE_NB_SHIFT_SIZE    3
#define FHT_CORE_NB_BUF           5
#define FHT_CORE_NB_BUF_SIZE      12
#define FHT_CORE_RST_AT_LOAD      17
#define FHT_CORE_RST_AT_LOAD_SIZE 0


// ##########
// FIXME : set specific parameters
// examples:
#define FHT_CORE_DATA_IN_POS      0
#define FHT_CORE_DATA_IN_SIZE    16
#define FHT_CORE_DATA_OUT_POS     0
#define FHT_CORE_DATA_OUT_SIZE   16
// ...
// ##########


/*------------------------------------------------------------------------------
 * CLASS: fht_core_config
 *
 *   This class is the base class for all core configs
 *----------------------------------------------------------------------------*/
class fht_core_config :
  public core_config
{
/*------------------------------------------------------------------------------
 * Parameters                                                             
 *----------------------------------------------------------------------------*/
  public:
	
		// configuration parameters : variables declaration

    t_uint16 end_status;

		int nb_fht;
		int fht_sz;
		int nb_shift;
		bool sat_rst_at_load;
		bool * mask_user0;
		bool * mask_user1;
		// ressource 
		t_uint32 block_fht_length;

// 		// ##########
// 	  // FIXME : declare specific parameters
// 		// examples :
//     t_uint16 data_in;
//     t_uint16 data_out;
// 		// ...
// 		// ##########

/*------------------------------------------------------------------------------
 * Methods                                                             
 *----------------------------------------------------------------------------*/
  public:
    // Constructor
    fht_core_config();

    // Destructor
    virtual ~fht_core_config();

    // Address map
    virtual void write_register(t_uint32 addr, t_uint32 data);
    virtual t_uint32 read_register(t_uint32 addr);

    // Fill in the core config table with new core config objects of type fht (static function)
    static void init_core_config_table(core_config_table *cfg_core);

    // Load configurations from catalog file (static function)
    static void init_config_from_file(t_uint16 level, core_config_table *cfg_core, 
				      t_uint16 nb_cfg_core_to_load, t_uint16 *num_cfg_core, 
				      string core_name);

    // Debug
    void print_config();

    // Get config parameters
    inline t_uint32 get_config_size() {return FHT_CORE_CFG_SIZE;}
    inline t_uint16 get_config_flit_size() {return FHT_CORE_CFG_FLIT_SIZE;}
    
    // Create transaction from params
    // generic
    void set_data_config_trans(anoc_data_transaction *trans_, t_uint16 flit_id_);
    // specific
    void set_data_config_0_trans(anoc_data_transaction *trans_);
    void set_data_config_1_trans(anoc_data_transaction *trans_);
    void set_data_config_2_trans(anoc_data_transaction *trans_);
};


#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _FHT_CORE_CONFIG_H_ */

