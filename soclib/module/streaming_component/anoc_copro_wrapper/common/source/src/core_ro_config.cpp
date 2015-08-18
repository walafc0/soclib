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
 *           These classes shall be used for reading a core configuration 
 *           from a text file, and generate a corresponding bitstream.
 *           Important : No SystemC shall be used, since this class is 
 *           also included in pure C++ programs.
 */

#include  "core_ro_config.h"

/*------------------------------------------------------------------------------
 * CLASS: core_ro_config
 *----------------------------------------------------------------------------*/

// Constructor
core_ro_config::core_ro_config() {
  //FIXME
  config_loaded = false;

  nb_config_block = 0;
  config_block_size.clear();
  config_block_addr.clear();
 
}

// Destructor
core_ro_config::~core_ro_config() {

}

t_uint16 core_ro_config::get_config_block_size(t_uint16 block_id) {
  t_uint16 block_size = 0;
  if(block_id<nb_config_block) {
    block_size = config_block_size[block_id];
  }
  return block_size; 
}

t_uint32 core_ro_config::get_config_block_addr(t_uint16 block_id) {
  t_uint32 block_addr = 0;
  if(block_id<nb_config_block) {
    block_addr = config_block_addr[block_id];
  }
  return block_addr;     
}

/*------------------------------------------------------------------------------
 * CLASS: core_ro_config_table
 *----------------------------------------------------------------------------*/

// Constructor
core_ro_config_table::core_ro_config_table( t_uint16 core_id_,
                                            t_uint16 nb_cfg_core_,
                                            bool multicore_) {
  core_id = core_id_;
  nb_cfg_core = nb_cfg_core_;
  multicore = multicore_;

  core_cfg_size = 0;

  config = new core_ro_config*[nb_cfg_core];
}

// Destructor
core_ro_config_table::~core_ro_config_table() {
  //FIXME
  for(int i=0; i<nb_cfg_core; i++) {
    delete config[i];
  }
  delete config;
}

// void core_ro_config_table::init_config(t_uint16 nb_cfg_core_to_load, t_uint16 *num_cfg_core, std::string ni_name) {
//   //FIXME
// }

void core_ro_config_table::set_multicore(t_uint16 id) {
  multicore = true;
  core_id = id;
}

// associative table : returns NULL pointer if config not present
core_ro_config *core_ro_config_table::get_config(t_uint16 slot_id) {

  core_ro_config *p_cfg;

  if(slot_id<nb_cfg_core) {
    p_cfg = config[slot_id];
    if(p_cfg == NULL) {
      cout << "ERROR : not enough config created in core_ro_config_table" << endl;
      exit(0);
    } else {
      if (p_cfg->is_loaded()) {
	return config[slot_id];
      }
    }
  } else {
    cout << "ERROR : invalid slot id " << slot_id << " for CORE configuration (max:" << nb_cfg_core-1 << ")" << endl;
    exit(0);
  }
  return NULL;
}

string core_ro_config_table::get_cfg_catalog_name( string core_name,
                                                   const char* core_cfg_file_name,
                                                   const char* multicore_cfg_file_name) {
  string file_name;
  file_name = PATH_CFG_CATALOG;
  file_name += utils_extract_subname(core_name,1);
  file_name += "/";
  if(!multicore) {
    file_name += core_cfg_file_name;
  } else {
    char multicore_name[80];
    sprintf(multicore_name, multicore_cfg_file_name, core_id);
    file_name+=multicore_name;
  }
  return file_name;
}

string core_ro_config_table::get_cfg_catalog_name( string core_name){
  return get_cfg_catalog_name( core_name,
                               NAME_CORE_CFG_CATALOG,
                               NAME_MULTICORE_CFG_CATALOG);
}

void core_ro_config::set_ro_config(const core_ro_config* const p_ro_config) {
  config_loaded = true;
  nb_config_block = p_ro_config->nb_config_block;
  config_block_size = p_ro_config->config_block_size;
  config_block_addr = p_ro_config->config_block_addr;
}
