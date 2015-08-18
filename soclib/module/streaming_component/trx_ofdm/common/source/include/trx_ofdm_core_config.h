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

#ifndef _TRX_OFDM_CORE_CONFIG_H_
#define _TRX_OFDM_CORE_CONFIG_H_

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "anoc_common.h"
#include "res_path.h"
#include "ni_utils.h"
#include "core_config.h"
#include "anoc_transaction.h"
#include "trx_ofdm_core_ro_config.h"

/*------------------------------------------------------------------------------
 * CLASS: trx_ofdm_core_config
 *----------------------------------------------------------------------------*/
class trx_ofdm_core_config :
  public virtual trx_ofdm_core_ro_config,
  public virtual core_config
{
/*------------------------------------------------------------------------------
 * Methods                                                             
 *----------------------------------------------------------------------------*/
  public:
    // Constructor
    trx_ofdm_core_config();

    // Destructor
    virtual ~trx_ofdm_core_config();

    // Address map
    virtual void write_register(t_uint32 addr, t_uint32 data);
    virtual t_uint32 read_register(t_uint32 addr);

    // Fill in the core config table with new core config objects of type trx_ofdm (static function)
    static void init_core_config_table(core_config_table *cfg_core);

    // Load configurations from catalog file (static function)
    static void init_config_from_file(t_uint16 level, core_config_table *cfg_core, 
				      t_uint16 nb_cfg_core_to_load, t_uint16 *num_cfg_core, 
				      string core_name);

    // Debug
    void print_config();

};

#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _TRX_OFDM_CORE_CONFIG_H_ */

