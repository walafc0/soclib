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
 * Comment : Template classes for core configurations
 *
 */


#ifndef _CORE_CONFIG_H_
#define _CORE_CONFIG_H_

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "anoc_common.h"
#include "anoc_transaction.h"
#include "ni_utils.h"

#include "core_ro_config.h"

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/
//#define VERBOSE

/*------------------------------------------------------------------------------
 * CLASS: core_config
 *
 *   This class is the base class for all core configs
 *----------------------------------------------------------------------------*/
class core_config : virtual public core_ro_config {
/*------------------------------------------------------------------------------
 * Methods
 *----------------------------------------------------------------------------*/
  public:
    // Constructor
    core_config();

    // Destructor
    virtual ~core_config();

    // Address map
    virtual void write_register(t_uint32 addr, t_uint32 data);
    virtual t_uint32 read_register(t_uint32 addr);

    // Set data transaction from parameters
    virtual void set_data_config_trans(anoc_data_transaction *trans_, t_uint16 flit_id_);
    virtual void set_data_config_trans(anoc_data_transaction *trans_, t_uint16 flit_id_, t_uint16 block_id_);

    // Debug
    virtual void print_config();

};

/*------------------------------------------------------------------------------
 * CLASS: core_config_table
 *
 *   This class implements
 *----------------------------------------------------------------------------*/
class core_config_table {
/*------------------------------------------------------------------------------
 * Parameters
 *----------------------------------------------------------------------------*/
  public:
    core_config **config; // @[nb_cfg]*

    t_uint16 core_id;
    bool multicore;

  protected: // structural info
    t_uint16 nb_cfg_core;

    t_uint32 core_cfg_size;


/*------------------------------------------------------------------------------
 * Methods
 *----------------------------------------------------------------------------*/
  public:
    // Constructor
    core_config_table(t_uint16 nb_cfg_core_);

    // Destructor
    virtual ~core_config_table();
 
    // Initialize configurations in child class
    virtual void init_config(t_uint16 nb_cfg_core_to_load, t_uint16 *num_cfg_core, std::string ni_name);

    // Set size config core
    inline virtual void set_core_cfg_size(t_uint32 size) {core_cfg_size=size;}

    // associative table
    core_config *get_config(t_uint16 slot_id);
    //FIXME

    //
    t_uint16 get_nb_cfg_core() {return nb_cfg_core;}

    // Address map
    virtual void write_register(t_uint32 addr, t_uint32 data);

    // multicore
    void set_multicore(t_uint16 id);

    // Config filename
    string get_cfg_catalog_name(string core_name);

};


#ifdef VERBOSE
#undef VERBOSE
#endif

#endif /* _CORE_CONFIG_H_ */
