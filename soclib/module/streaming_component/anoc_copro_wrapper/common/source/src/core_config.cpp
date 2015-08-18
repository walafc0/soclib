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

#include  "core_config.h"

/*------------------------------------------------------------------------------
 * CLASS: core_config
 *----------------------------------------------------------------------------*/

// Constructor
core_config::core_config()
  : core_ro_config()
{
  
}

// Destructor
core_config::~core_config() {

}

// Address map
void core_config::write_register(t_uint32 addr, t_uint32 data) {

}

t_uint32 core_config::read_register(t_uint32 addr){
  cout << "FIXME : core_config::read_register" << endl;
  return 0;
}

// Debug
void core_config::print_config() {
  cout << "FIXME : default core config print" << endl;
}

// Create transaction from params
void core_config::set_data_config_trans(anoc_data_transaction *trans_, t_uint16 flit_id_) {
  set_data_config_trans(trans_, flit_id_, 0);
}

void core_config::set_data_config_trans(anoc_data_transaction *trans_, t_uint16 flit_id_, t_uint16 block_id_) {
  t_uint32 data_ts = read_any_register(flit_id_, block_id_);
  trans_->set_data(data_ts);	
}

/*------------------------------------------------------------------------------
 * CLASS: core_config_table
 *----------------------------------------------------------------------------*/

// Constructor
core_config_table::core_config_table(t_uint16 nb_cfg_core_) {
  nb_cfg_core = nb_cfg_core_;

  core_cfg_size = 0;

  config = new core_config*[nb_cfg_core];

  multicore = false;
  core_id = 0;

}

// Destructor
core_config_table::~core_config_table() {
  //FIXME
  for(int i=0; i<nb_cfg_core; i++) {
    delete config[i];
  }
  delete config;
}

void core_config_table::init_config(t_uint16 nb_cfg_core_to_load, t_uint16 *num_cfg_core, std::string ni_name) {
  //FIXME
}

void core_config_table::set_multicore(t_uint16 id) {
  multicore = true;
  core_id = id;
}


// associative table : returns NULL pointer if config not present
core_config *core_config_table::get_config(t_uint16 slot_id) {

  core_config *p_cfg;

  if(slot_id<nb_cfg_core) {
    p_cfg = config[slot_id];
    if(p_cfg == NULL) {
      cout << "ERROR : not enough config created in core_config_table" << endl;
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


void core_config_table::write_register(t_uint32 addr, t_uint32 data) {

  t_uint32 cfg_register;

  // current configuration position for write_register
  t_uint16 current_cfg_pos;

  if(core_cfg_size==0) {
    cout << "ERROR : core_cfg_size not set! (core_config_table::write_register)" << endl;
    exit(0);
  }

  cfg_register = addr%core_cfg_size;
  current_cfg_pos = addr/core_cfg_size;

  switch(cfg_register) {
  case 0:

    if(current_cfg_pos<nb_cfg_core) {
      //cout << addr << endl;
      //cout << core_cfg_size << endl;
      //cout << current_cfg_pos << endl;
      //cout << "write first word" << endl;
      config[current_cfg_pos]->set_loaded(); 
      config[current_cfg_pos]->write_register(cfg_register,data);
    } else {
      cout << "WARNING : Address overflow in CORE config table!" << endl;
    }
    break;
  default:
    // other config word
    if(cfg_register<core_cfg_size) {
      if(current_cfg_pos<nb_cfg_core) {
	//cout << "write word " << cfg_register << endl;
	config[current_cfg_pos]->write_register(cfg_register,data);
      } else {
	cout << "WARNING : Address overflow in CORE config table!" << endl;
      }
    }
    else {
      cout << "WARNING : core cfg register overflow!" << endl;
    }
    break;
  }
}

string core_config_table::get_cfg_catalog_name(string core_name) {
  string file_name;
  file_name = PATH_CFG_CATALOG;
  file_name += utils_extract_subname(core_name,1);
  file_name += "/";
  if(!multicore) {
    file_name += NAME_CORE_CFG_CATALOG;
  } else {
    char multicore_name[80];
    sprintf(multicore_name, NAME_MULTICORE_CFG_CATALOG, core_id);
    file_name+=multicore_name;
  }
  return file_name;
}

