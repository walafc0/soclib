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

#include  "trx_ofdm_core_ro_config.h"
#include <sstream>
#include <iostream>
#include <fstream>

/*------------------------------------------------------------------------------
 * CLASS: trx_ofdm_core_ro_config
 *----------------------------------------------------------------------------*/

// Constructor
trx_ofdm_core_ro_config::trx_ofdm_core_ro_config()
  : core_ro_config() {

  // global parameters
  //config_id = 0;
  config_loaded = false;

  // specific parameters

  // ##########
  // initialize parameters => simple ifft 1024 points without framing	
  log2_size_fft=10 ; // 1024 points FFT
  norm_power=false;
  fft_type=ifft;
  bypass_fft=false;
  shift_carrier=false;
  shift_parity=false;
  gi_size=0;
  gi_insertion=false;
  floc=dp1fifo;
  mask_IT=true;
  mask_data=new t_uint32[TRX_OFDM_CORE_NB_MASK];
  for (int i=0; i<TRX_OFDM_CORE_NB_MASK; i++)
    mask_data[i]=0xffffffff;
  mask_pilot=new t_uint32[TRX_OFDM_CORE_NB_MASK];
  for (int i=0; i<TRX_OFDM_CORE_NB_MASK; i++)
    mask_pilot[i]=0; 
  // ...
  // ##########

  nb_config_block = 1;
  config_block_size.push_back(TRX_OFDM_CORE_CFG_FLIT_SIZE);
  config_block_addr.push_back(0);
}

// Destructor
trx_ofdm_core_ro_config::~trx_ofdm_core_ro_config() {
  delete[] mask_pilot;
  delete[] mask_data;
}

// Fill in the cfg_core table with objects of type "trx_ofdm_core_ro_config"
void trx_ofdm_core_ro_config::init_core_ro_config_table(core_ro_config_table *cfg_core) {

  t_uint16 nb_cfg_core;

  nb_cfg_core = cfg_core->get_nb_cfg_core();
  
  for (int i = 0; i < nb_cfg_core; i++) {
    cfg_core->config[i] = new trx_ofdm_core_ro_config(); 
  }  
}


void trx_ofdm_core_ro_config::init_ro_config_from_file( t_uint16 level,
                             core_ro_config_table *cfg_core,
                             t_uint16 nb_cfg_core_to_load,
                             t_uint16 *num_cfg_core,
                             string core_name) {
  string file_name = cfg_core->get_cfg_catalog_name(core_name);
  trx_ofdm_core_ro_config::init_ro_config_from_file( level,
                                                     cfg_core,
                                                     nb_cfg_core_to_load,
                                                     num_cfg_core,
                                                     core_name,
                                                     file_name);
}

void trx_ofdm_core_ro_config::init_ro_config_from_file( t_uint16 level,
                             core_ro_config_table *cfg_core,
                             t_uint16 nb_cfg_core_to_load,
                             t_uint16 *num_cfg_core,
                             string core_name,
                             string file_name) {
  string keyword;
  string str_value;

  bool skipped_line = false;
  string read_line;
  int nb_read_line = 0;
  std::istringstream config_line(read_line);

  bool load_in_progress;
  bool load_current_cfg;

  trx_ofdm_core_ro_config *config;

  t_uint16 config_id;
  t_uint16 load_pos;
  int pos_counter;

  int beg_mask = 0;
  int end_mask = 0;
  t_uint32 value;

  EXPRINTN(core, level, core_name, "CORE configuration table initialization (" << nb_cfg_core_to_load << " cfg to load)" );

 // open configuration file
  std::ifstream config_file(file_name.c_str());
  if (!config_file) {
    cout << "ERROR : the configuration file does not exist : " << file_name << endl;
    exit(0);
  }

  EXPRINTN(core, level, core_name, "Reading configuration catalog file " << file_name  );

  pos_counter = 0;
  load_in_progress=false;
  while (config_file.good()) {
    
    nb_read_line++;
    getline(config_file,read_line);  
    config_line.clear();
    config_line.str(read_line + "\n");
    // add end of line '\n' character otherwise single keyword line failed
    config_line >> keyword;

    skipped_line = false;
    if(!config_line) {
      // no keyword read => blank line skipped    
      skipped_line = true;
    }
    else if(keyword[0]=='#') {
      // comments => line skipped
      skipped_line = true;
    } else if (keyword == "config_id") {

      config_line >> config_id ; 

      load_current_cfg=false;
      if(nb_cfg_core_to_load!=0) {
	for(int j=0; j<nb_cfg_core_to_load; j++) {
	  if(num_cfg_core[j]==config_id) {
	    load_current_cfg=true;
	    load_pos=j;
	  }
	}
      } else {
	// special mode
	load_current_cfg=true;
	load_pos=pos_counter;
	pos_counter++;
      }
      if(load_current_cfg) {
	EXPRINTN(core, level, core_name, "| Loading configuration " << config_id << " in config_table_core[" << load_pos << "]"  );

        config = dynamic_cast<trx_ofdm_core_ro_config*> (cfg_core->config[load_pos]);
	load_in_progress=true;   
        config->set_loaded();
      } else {
	EXPRINTN(core, level, core_name, "  | Configuration " << config_id << " doesn't need to be loaded"  );
	load_in_progress=false;  
      }
    } 
    // ##########
    // load parameters
    // 
    else if (keyword=="fft_type") {
      if(load_in_progress) {
	config_line >> str_value;
	if (str_value == "fft")
	  config->fft_type = fft;
	else if (str_value == "ifft")
	  config->fft_type = ifft;
	else {
	  EXPRINTN(core, 0, core_name, "  | WARNING : invalid value : " << str_value << " for field fft_type");	
	}  
      }
    } 
    else if (keyword=="log2_size_fft") {
      if(load_in_progress) {
	config_line >> config->log2_size_fft;
      }
    } 
     else if (keyword=="norm_power") {
      if(load_in_progress) {
	config_line >> config->norm_power;
      }
    } 
     else if (keyword=="bypass_fft") {
      if(load_in_progress) {
	config_line >> config->bypass_fft;
      }
    }
     else if (keyword=="shift_carrier") {
      if(load_in_progress) {
	config_line >> config->shift_carrier;
      }
    }
     else if (keyword=="shift_parity") {
      if(load_in_progress) {
	config_line >> config->shift_parity;
      }
    }
     else if (keyword=="gi_insertion") {
      if(load_in_progress) {
	config_line >> config->gi_insertion;
      }
    }
     else if (keyword=="gi_size") {
      if(load_in_progress) {
	config_line >> config->gi_size;
      }
    }
    else if (keyword=="framing_loc") {
      if(load_in_progress) {
	config_line >> str_value;
	if (str_value == "dp1fifo")
	  config->floc = dp1fifo;
	else if (str_value == "dp2fifo")
	  config->floc = dp2fifo;
	else {
	  EXPRINTN(core, 0, core_name, "  | WARNING : invalid value : " << str_value << " for field framing_location");	
	}  
      }
    }
     else if (keyword=="mask_IT") {
      if(load_in_progress) {
	config_line >> config->mask_IT;
      }
    }
     else if (keyword=="mask_data") {
      if(load_in_progress) {
	config_line >> beg_mask;
	config_line >> end_mask;
	config_line >> hex >> value >> dec;
  EXPRINTN(core, 0, core_name, "  | loading " << hex << value << dec 
           << " as mask_data from " << beg_mask << " to " << end_mask);
	for (int i=beg_mask; i<end_mask; i++)
	  config->mask_data[i] = value;	
      }
    }
     else if (keyword=="mask_pilot") {
      if(load_in_progress) {
	config_line >> beg_mask;
	config_line >> end_mask;
	config_line >> hex >> value >> dec;
	for (int i=beg_mask; i<end_mask; i++)
	  config->mask_pilot[i] = value;	
      }
    }
    // ##########
    else if(keyword!="") {
      EXPRINTN(core, 0, core_name, "  | WARNING : invalid keyword : " << keyword);
      //config_line.ignore(nb_max_char,'\n');
    }
    if(!skipped_line) {
      // test the end of line
      config_line >> keyword;
      if(load_in_progress && config_line && keyword[0]!='#') {
	EXPRINTNL(core, 0, core_name, nb_read_line, "WARNING : too many arguments");
      }
    }
  }
  config_file.close();
  
  EXPRINTN(core, level, core_name, "End reading (" << nb_read_line << " lines)");

  core_ro_config *p_cfg;

  for(int j=0; j<nb_cfg_core_to_load; j++)
    {
      p_cfg=cfg_core->get_config(j);
      if(p_cfg==NULL) {
	EXPRINTN(core, 0, core_name, "ERROR : core configuration " << num_cfg_core[j] << " not found in core configuration catalog");
	exit(0);
      }
    }
}

t_uint32  trx_ofdm_core_ro_config::read_any_register(t_uint32 addr_, t_uint32 block_id_){
  t_uint32 data_ts;
  
  addr_++; // HUGE FIXME !!!

  switch(addr_) {
    // ##########
    //call set_data_config_XXX_trans function
  case 129:
    //create the transaction
    data_ts = shift_parity;
    data_ts<<= (TRX_OFDM_CORE_SHIFT_PARITY_POS-TRX_OFDM_CORE_SHIFT_CARRIER_POS);
    data_ts += shift_carrier;
    data_ts<<= (TRX_OFDM_CORE_SHIFT_CARRIER_POS-TRX_OFDM_CORE_BYPASS_FFT_POS);
    data_ts += bypass_fft;
    data_ts<<= (TRX_OFDM_CORE_BYPASS_FFT_POS-TRX_OFDM_CORE_FFT_TYPE_POS);
    data_ts += fft_type;
    data_ts<<= (TRX_OFDM_CORE_FFT_TYPE_POS-TRX_OFDM_CORE_NORM_POWER_POS);
    data_ts += norm_power;
    data_ts<<= (TRX_OFDM_CORE_NORM_POWER_POS-TRX_OFDM_CORE_LOG2_SIZE_FFT_POS);
    data_ts += log2_size_fft;
    break;
  case 130:
    //create the transaction
    data_ts = gi_insertion;
    data_ts<<= (TRX_OFDM_CORE_GI_INSERT_POS-TRX_OFDM_CORE_GI_SIZE_POS);
    data_ts += gi_size;    
    break;
  case 131:
    //create the transaction
    data_ts = floc;
    break;
   case 132:
    //create the transaction
    data_ts = mask_IT;
    break;
    // ###########
  default:
    if (addr_> 0 && addr_<= TRX_OFDM_CORE_NB_MASK){
      data_ts = mask_data[addr_- 1];
    }
    else if (addr_> (TRX_OFDM_CORE_NB_MASK) && addr_<= (2*TRX_OFDM_CORE_NB_MASK)) {
      data_ts = mask_pilot[addr_- 1 - TRX_OFDM_CORE_NB_MASK];
    }
    else {
      EXPRINTN(core, 0, "trx_ofdm_core", "ERROR : set_data_config_trans() : invalid flid_id=" <<addr_);
      return 0; // anti-warning
      exit(0);
    }
  }  
 
  return data_ts;
}

/************************************************/
// output stream functions
ostream& operator << (ostream& os, const fft_t& fft_type) {
  if (fft_type == fft)
    os << "fft";
  else if (fft_type == ifft)
    os << "ifft";
  else
    os << "unknown";
  return os;
}

ostream& operator << (ostream& os, const framing_loc& floc) {
  if (floc == dp1fifo)
    os << "dp1fifo";
  else if (floc == dp2fifo)
    os << "dp2fifo";
  else
    os << "unknown";
  return os;
}   

// ##########

void trx_ofdm_core_ro_config::set_ro_config(const core_ro_config* const p_ro_config){
  core_ro_config::set_ro_config(p_ro_config);

  const trx_ofdm_core_ro_config* p_trx_ofdm_ro_config = dynamic_cast<const trx_ofdm_core_ro_config*>(p_ro_config); 

  config_loaded = true;

  // specific parameters

  // ##########
  // initialize parameters => simple ifft 1024 points without framing	
  log2_size_fft = p_trx_ofdm_ro_config->log2_size_fft;
  norm_power    = p_trx_ofdm_ro_config->norm_power;
  fft_type      = p_trx_ofdm_ro_config->fft_type;
  bypass_fft    = p_trx_ofdm_ro_config->bypass_fft;
  shift_carrier = p_trx_ofdm_ro_config->shift_carrier;
  shift_parity  = p_trx_ofdm_ro_config->shift_parity;
  gi_size       = p_trx_ofdm_ro_config->gi_size;
  gi_insertion  = p_trx_ofdm_ro_config->gi_insertion;
  floc          = p_trx_ofdm_ro_config->floc;
  mask_IT       = p_trx_ofdm_ro_config->mask_IT;
  mask_data=new t_uint32[TRX_OFDM_CORE_NB_MASK];
  memcpy(mask_data, p_trx_ofdm_ro_config->mask_data, TRX_OFDM_CORE_NB_MASK * sizeof(t_uint32));
  mask_pilot=new t_uint32[TRX_OFDM_CORE_NB_MASK];
  memcpy(mask_pilot, p_trx_ofdm_ro_config->mask_pilot, TRX_OFDM_CORE_NB_MASK * sizeof(t_uint32));
}

