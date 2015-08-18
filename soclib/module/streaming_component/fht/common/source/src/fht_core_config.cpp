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

#include  "fht_core_config.h"

/*------------------------------------------------------------------------------
 * CLASS: fht_core_config
 *----------------------------------------------------------------------------*/

// Constructor
fht_core_config::fht_core_config()
  : core_config() {

  // global parameters
  //config_id = 0;
  config_loaded = false;

  // specific parameters

   mask_user0 = new bool[32];
   mask_user1 = new bool[32];
   nb_fht = 0;
   fht_sz = 0;
   nb_shift = 0;
   sat_rst_at_load = 0;
   block_fht_length = 5;
// 	// ##########
// 	// FIXME : initialize parameters	
// 	// examples :
//  data_in = 0;
//  data_out = 0;
// 	// ...
// 	// ##########
	
}


// Destructor
fht_core_config::~fht_core_config() {
  //FIXME
}


// Address map
void fht_core_config::write_register(t_uint32 addr, t_uint32 data) {
  sc_lv<32> word;
  word = data;
	
	// ##########
	// FIXME : code specific address map
	// examples :
	// fixme, in case config is not read from file!!!
//	cout << "### fht_core_config::write_register FIXME!!!";
  switch(addr) {
      case 0:
          nb_shift = word.range(FHT_CORE_NB_SHIFT+FHT_CORE_NB_SHIFT_SIZE-1,FHT_CORE_NB_SHIFT).to_uint();
          nb_fht = word.range(FHT_CORE_NB_BUF+FHT_CORE_NB_BUF_SIZE-1,FHT_CORE_NB_BUF).to_uint();
          sat_rst_at_load = word[FHT_CORE_RST_AT_LOAD].to_bool();
          switch(data & 3) {
              case 0: 
                  fht_sz = 8;
                  break;
              case 1:
                  fht_sz = 16;
                  break;
              case 2:
                  fht_sz = 32;
                  break;
              default:   
                  break;
          }
          break;

      case 1:
          for (int i=0; i<32; i++) {
              mask_user0[i] = word[i].to_bool();
          }
          break;
      case 2:
          for (int i=0; i<32; i++) {
              mask_user1[i] = word[i].to_bool();
          }
          break;
      default:   
          break;
  }
	// ...
	// ##########
}


t_uint32 fht_core_config::read_register(t_uint32 addr) {
  //FIXME
	return 0;
}

// Print configuration parameters
void fht_core_config::print_config() {

  cout << "*** CORE config";
  if(config_loaded) {
    cout << " (loaded)";
  } else {
    cout << " (not loaded)";
  }
  cout << " ***" << endl;

  cout << "************* RX_FHT config *********" << endl;
  cout << "nb_fht = " << nb_fht << " and fht_sz = " << fht_sz << " and nb_shift = " << nb_shift << endl;
  cout << "sat_rst_at_load = " << sat_rst_at_load << endl;
  for (int i=0; i<32; i++) 
    cout << "mask_user0 " << i << " = " << hex << mask_user0[i] << dec << endl;
  for (int i=0; i<32; i++) 
    cout << "mask_user1 " << i << " = " << hex << mask_user1[i] << dec << endl; 
  cout << "************* end of RX_FHT config *********" << endl;
}

// Fill in the cfg_core table with objects of type "fht_core_config"
void fht_core_config::init_core_config_table(core_config_table *cfg_core) {

  t_uint16 nb_cfg_core;

  nb_cfg_core = cfg_core->get_nb_cfg_core();
  
  for (int i = 0; i < nb_cfg_core; i++) {
    cfg_core->config[i] = new fht_core_config(); 
  }  
  
}


void fht_core_config::init_config_from_file(t_uint16 level, core_config_table *cfg_core, 
						      t_uint16 nb_cfg_core_to_load, t_uint16 *num_cfg_core, 
						      string core_name) {

  string file_name;
  string keyword;

  bool skipped_line = false;
  string read_line;
  int nb_read_line = 0;
  std::istringstream config_line(read_line);

  bool load_in_progress;
  bool load_current_cfg;

  fht_core_config *config;

  t_uint16 config_id;
  t_uint16 load_pos;
  int pos_counter;


  PRINTN(level, core_name, "CORE configuration table initialization (" << nb_cfg_core_to_load << " cfg to load)" );

  file_name += PATH_CFG_CATALOG;
  file_name += utils_extract_subname(core_name,1);
  file_name += "/";
  if(!cfg_core->multicore) {
    file_name += NAME_CORE_CFG_CATALOG;
  } else {
    char multicore_name[80];
    sprintf(multicore_name, NAME_MULTICORE_CFG_CATALOG, cfg_core->core_id);
    file_name+=multicore_name;
  }

 // open configuration file
  std::ifstream config_file(file_name.c_str());
  if (!config_file) {
    cout << "ERROR : the configuration file does not exist : " << file_name << endl;
    exit(0);
  }

  PRINTN(level, core_name, "Reading configuration catalog file " << file_name  );

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
      if(load_in_progress) {
	PRINTN(0, core_name, "WARNING : config id " << config_id << " not fully loaded");
      }
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
	PRINTN(level, core_name, "| Loading configuration " << config_id << " in config_table_core[" << load_pos << "]"  );

        config = dynamic_cast<fht_core_config*> (cfg_core->config[load_pos]);
	load_in_progress=true;   
	//config->config_id=config_id;

      } else {
	PRINTN(level, core_name, "  | Configuration " << config_id << " doesn't need to be loaded"  ); 
      }
    } 
		// ##########
		// FIXME : load parameters
		//         for the last parameter -> update set_loaded and load_in_progress
		// examples :
    else if (keyword=="FHT_SZ") {
      if(load_in_progress) {
				config_line >> config->fht_sz;
      }
    } 
    else if (keyword=="SAT_RST_AT_LOAD") {
      if(load_in_progress) {
				config_line >> config->sat_rst_at_load;
      }
    }
    else if (keyword=="NB_FHT") {
      if(load_in_progress) {
				config_line >> config->nb_fht;
      }
    }
    else if (keyword=="NB_SHIFT") {
      if(load_in_progress) {
				config_line >> config->nb_shift;
			  config->set_loaded();
	      load_in_progress=false; 
      }
    }
    else if (keyword=="MASK_USER_0") {
      if(load_in_progress) {
				bool tmp;
				int beg_mask;
				int end_mask;
				config_line >> beg_mask;
				config_line >> end_mask;
				config_line >> tmp;
				for (int i=beg_mask;i<end_mask;i++)
					config->mask_user0[i]=tmp;							
      }
    }
		else if (keyword=="MASK_USER_1") {
      if(load_in_progress) {
				bool tmp;
				int beg_mask;
				int end_mask;
				config_line >> beg_mask;
				config_line >> end_mask;
				config_line >> tmp;
				for (int i=beg_mask;i<end_mask;i++)
					config->mask_user1[i]=tmp;							
      }
    } 
		// ...
		// ##########
    else if(keyword!="") {
      PRINTN(0, core_name, "  | WARNING : invalid keyword : " << keyword);
      //config_line.ignore(nb_max_char,'\n');
    }
    if(!skipped_line) {
      // test the end of line
      config_line >> keyword;
      if(load_in_progress && config_line && keyword[0]!='#') {
	PRINTNL(0, core_name, nb_read_line, "WARNING : too many arguments");
      }
    }
  }
  config_file.close();
  
  PRINTN(level, core_name, "End reading (" << nb_read_line << " lines)");

  core_config *p_cfg;

  for(int j=0; j<nb_cfg_core_to_load; j++)
    {
      p_cfg=cfg_core->get_config(j);
      if(p_cfg==NULL) {
	PRINTN(0, core_name, "ERROR : core configuration " << num_cfg_core[j] << " not found in core configuration catalog");
	exit(0);
      }
    }
}

// Create transaction from params
void fht_core_config::set_data_config_trans(anoc_data_transaction *trans_, t_uint16 flit_id_) {
  switch(flit_id_) {
	// ##########
	// FIXME : call set_data_config_XXX_trans function
	// examples :
  case 0:
    set_data_config_0_trans(trans_);
    break;
  case 1:
    set_data_config_1_trans(trans_);
    break;
	// ...
  // ###########
  default:
    PRINTN(0, "fht_core", "ERROR : set_data_config_trans() : invalid flid_id=" << flit_id_);
    exit(0);
    break;
  }
  
  
}

// ##########
// FIXME : write set_data_config_XXX_trans functions
// examples :

void fht_core_config::set_data_config_0_trans(anoc_data_transaction *trans_) {
  t_uint32 data_ts;

 // data_ts = data_in << FHT_CORE_DATA_IN_POS;

  trans_->set_data(data_ts);
}

void fht_core_config::set_data_config_1_trans(anoc_data_transaction *trans_) {
  t_uint32 data_ts;

  //data_ts = data_out << FHT_CORE_DATA_OUT_POS;

  trans_->set_data(data_ts);
}
// ...
// ##########



