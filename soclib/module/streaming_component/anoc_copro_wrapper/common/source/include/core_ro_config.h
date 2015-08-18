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
 * Comment : Template classes for core configurations in read-only mode.
 *           These classes shall be used for reading a core configuration from 
 *           a text file, and generate a corresponding bitstream.
 *           Important : No SystemC shall be used, since this class is also 
 *           included in pure C++ programs.
 *
 */

#ifndef _CORE_RO_CONFIG_H_
#define _CORE_RO_CONFIG_H_

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "types.h"
#include "res_path.h"
#include "ni_utils.h"

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/

#ifdef CORE_CFG_C
/* Definition of PRINT macros */
#define EXPRINTN(cat, print_level, name, str) \
{ \
  cout << std::setw(6) << ": [" << #cat << "] " \
  << name << ": " << str << endl; \
}

#define EXPRINTNL(cat, print_level, name, print_line, print_string) \
EXPRINTN(cat, print_level, name, "[LINE " << print_line << "] " << print_string)

#define PRINTN(print_level, name_string, print_string)	\
{ \
  cout << std::setw(6) << ": "	\
  << name_string << ": "	        \
  << print_string			\
  << endl; \
}

#define PRINTNL(print_level, name_string, print_line, print_string)	\
  PRINTN(print_level, name_string, "[LINE " << print_line << "] " << print_string)
#else
#include "anoc_common.h"
#endif

/*------------------------------------------------------------------------------
 * CLASS: core_ro_config
 *----------------------------------------------------------------------------*/
class core_ro_config {
/*------------------------------------------------------------------------------
 * Parameters
 *----------------------------------------------------------------------------*/
  public:

    t_uint16 nb_config_block;
    std::vector<t_uint16> config_block_size;
    std::vector<t_uint32> config_block_addr;

  protected:
    bool config_loaded;

/*------------------------------------------------------------------------------
 * Methods
 *----------------------------------------------------------------------------*/
  public:
    // Constructor
    core_ro_config();

    // Destructor
    virtual ~core_ro_config();

    void set_loaded() {config_loaded=true;}
    bool is_loaded() {return config_loaded;}

    // Address map
    virtual t_uint32 read_any_register(t_uint32 addr_, t_uint32 block_id_){
      cout << "FIXME : core_ro_config::read_any_register" << endl;
      return 0;
    }

    virtual t_uint32 get_config_size()=0;
    virtual t_uint16 get_config_flit_size()=0;

    inline virtual t_uint16 get_nb_config_block() {return nb_config_block;}
    virtual t_uint16 get_config_block_size(t_uint16 block_id);
    virtual t_uint32 get_config_block_addr(t_uint16 block_id);

    virtual void set_ro_config(const core_ro_config* const p_ro_config);
};

/*------------------------------------------------------------------------------
 * CLASS: core_ro_config_table
 *----------------------------------------------------------------------------*/
class core_ro_config_table {
/*------------------------------------------------------------------------------
 * Parameters
 *----------------------------------------------------------------------------*/
  public:
    core_ro_config **config; // @[nb_cfg]*

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
    core_ro_config_table( t_uint16 core_id_,
                          t_uint16 nb_cfg_core_,
                          bool multicore_);

    // Destructor
    virtual ~core_ro_config_table();

//     // Initialize configurations in child class
//     virtual void init_config(t_uint16 nb_cfg_core_to_load, t_uint16 *num_cfg_core, std::string ni_name);

    // Set size config core
    inline virtual void set_core_cfg_size(t_uint32 size) {core_cfg_size=size;}

    // associative table
    core_ro_config *get_config(t_uint16 slot_id);

    //
    t_uint16 get_nb_cfg_core() {return nb_cfg_core;}

    // multicore
    void set_multicore(t_uint16 id);

    // Config filename
    string get_cfg_catalog_name(string core_name);
    string get_cfg_catalog_name(string core_name,
                                const char* core_cfg_file_name,
                                const char* multicore_cfg_file_name);
};
#endif /* _CORE_RO_CONFIG_H_ */

