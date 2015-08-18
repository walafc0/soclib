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

#include  "core_sysc.h"

// Constructor 
core_sysc::core_sysc(sc_module_name module_name_,
                     int clk_period_,
                     bool multicore_,
                     t_uint16 core_id_,
                     t_uint16 nb_fifo_in_,
                     t_uint16 *size_fifo_in_,
                     t_uint16 nb_fifo_out_,
                     t_uint16 *size_fifo_out_,
                     t_uint16 nb_cfg_core_,
                     t_uint32 cfg_size_core_,
                     bool static_init_,
                     string sno_name_) :  core_base(module_name_,
                                                    clk_period_,
                                                    multicore_,
                                                    core_id_,
                                                    nb_fifo_in_,
                                                    //size_fifo_in_,
                                                    nb_fifo_out_),
                     //size_fifo_out_,
                     //nb_cfg_core_,
                     //cfg_size_core_,
                     //static_init_)
                     nb_cfg_core(nb_cfg_core_),
                     static_init(static_init_),
                     sno_name(sno_name_)
{
  
  // initialize size to released
  size_to_released = new t_uint32[nb_fifo_in];
  for(int i=0; i<nb_fifo_in; i++) {
    size_to_released[i]=0;
  }

  // initialize previous num available
  previous_num_available = new t_uint32[nb_fifo_out];
  for(int i=0; i<nb_fifo_out; i++) {
    previous_num_available[i]=0;
  }

  // Create Input FIFO
  size_fifo_in = new t_uint16[nb_fifo_in];
  if (size_fifo_in_) {
    for (int i = 0; i < nb_fifo_in; i++) {
      size_fifo_in[i] = size_fifo_in_[i];
    }
  }
  fifo_in = new sc_fifo<t_uint32> *[nb_fifo_in];
  if(nb_fifo_in==0) {
    EXPRINT(core, 0,"WARNING : CORE has no input FIFO");
  }
  for (int i = 0; i < nb_fifo_in; i++) {
    if (size_fifo_in[i]!=0) {
      EXPRINT(core, 0,"Creating FIFO IN " << i << " of size " << size_fifo_in[i]);
      fifo_in[i] = new sc_fifo<t_uint32>(size_fifo_in[i]);
    } else {
      EXPRINT(core, 0,"FIXME : size_fifo_in=0 => CORE memory");
      exit(0);    
    }
  }

  // Create Output FIFO
  size_fifo_out = new t_uint16[nb_fifo_out];
  if (size_fifo_out_) {
    for (int i = 0; i < nb_fifo_out; i++) {

      size_fifo_out[i] = size_fifo_out_[i];
    }
  }
  fifo_out = new sc_fifo<t_uint32> *[nb_fifo_out];
  if(nb_fifo_out==0) {
    EXPRINT(core, 0,"WARNING : CORE has no output FIFO");
  }
  for (int i = 0; i < nb_fifo_out; i++) {
    if(size_fifo_out[i]!=0) {
      EXPRINT(core, 0,"Creating FIFO OUT " << i << " of size " << size_fifo_out[i]);
      fifo_out[i] = new sc_fifo<t_uint32>(size_fifo_out[i]);
    } else {
      EXPRINT(core, 0,"FIXME : size_fifo_out=0 => CORE memory");  
      exit(0);      
    }
  }

//#ifdef TLM_POWER_ESTIMATION
#if 0
// to be removed
  for (int i = 0; i < nb_fifo_in; i++){
     fifo_in[i]->trace (pow_tf, ((std::string) name() + "_fifo_in_" +char ('0' + i)).c_str() );
  }
  for (int i = 0; i < nb_fifo_out; i++){
     fifo_out[i]->trace (pow_tf, ((std::string) name() + "_fifo_out_" +char ('0' + i)).c_str() );
  }
#endif
  //FIXME

  EXPRINT(core, 0,"Creating new config table of size " << nb_cfg_core << " (config size: " << cfg_size_core_ << ")");
  cfg_core = new core_config_table(nb_cfg_core);

  // Set size config core for config table

  cfg_core->set_core_cfg_size(cfg_size_core_);

  // Set multicore
  if(multicore) {
    cfg_core->set_multicore(core_id);    
  }

  // main communication thread
  SC_THREAD(compute);

}

// Destructor
core_sysc::~core_sysc() {
  //FIXME: delete[] sur tous les new[]

  cout << "-------------- DELETE " << name() << " -------------- " << endl;
  
  for(int i=0;i<nb_fifo_in;i++) {
    if(fifo_in[i]->num_available()!=0) {
      cout << "WARNING : " << fifo_in[i]->num_available() \
	   << " data available in fifo in " << i << " (/" << size_fifo_in[i] << ")" << endl;      
    }
  }
  for(int i=0;i<nb_fifo_out;i++) {
    if(fifo_out[i]->num_available()!=0) {
      cout << "WARNING : " << fifo_out[i]->num_available() \
	   << " data available in fifo out " << i << " (/" << size_fifo_out[i] << ")" << endl;  
    }    
  }
}


// Address map
void core_sysc::write_register(t_uint32 addr, t_uint32 data) {
  //EXPRINT(core, 0,"Write register, addr:" << addr << " data:" << data); //FIXPRINT
  //if(addr==CORE_ADDR_NUM_CFG) {
  //  EXPRINT(core, 0,"Write current slot id register : " << data);
  //  current_slot_id = data;
  //}
  // else
  if(addr>=addr_core_cfg_begin) {
    cfg_core->write_register(addr-addr_core_cfg_begin,data);
  }
  else {
    EXPRINT(core, 0,"WARNING : invalid register address " << addr);    
  }
}

t_uint32 core_sysc::read_register(t_uint32 addr) {
    EXPRINT(core, 0,"WARNING : invalid read register address");
    return 0;
}


// Initialize configurations to load
void core_sysc::init_config_to_load() {

  //int nb_max_char = 150;
  string file_name;
  string keyword;

  string string_temp;

  bool skipped_line = false;
  string read_line;
  int nb_read_line = 0;
  std::istringstream config_line(read_line);

  int current_core_id;
  
  nb_cfg_core_to_load=0;
  
  EXPRINT(core, 0,"Configuration initialization");
  
  //file_name = PATH_CONFIG_SYMBOL; 
  file_name = PATH_APPLI_SNO;

  if(sno_name=="") {
    // -----
    // code to extract .CORE from file_name...
    // this file is shared with the NI...
    file_name +=utils_extract_subname(name(),1);
    //-----
  } else {
    file_name +=sno_name;
  }
  
  file_name += EXT_SNO;
  
  // open configuration file
  std::ifstream config_file(file_name.c_str());
  if (!config_file) {
    EXPRINT(core, 0,"ERROR : the configuration file does not exist : " << file_name);
    exit(0);
  }
  
  EXPRINT(core, 0,"Reading scenario file  " << file_name); //FIXPRINT

  while (config_file.good()) {
  
    nb_read_line++;
    getline(config_file,read_line);
    
    //while(config_file.peek()=='#'||config_file.peek()==' '||config_file.peek()=='\n' ) {
    //  if(config_file.peek()=='#') {
    //	config_file.ignore(nb_max_char,'\n');
    //  } else {
    //	config_file.get();
    //  }
    //}    
    //config_file >> keyword;

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
    }
    else if (keyword == "cfg_core") {

      if(multicore) {
	config_line >> string_temp;
	current_core_id = utils_remove_brace(string_temp);
      } else {
	current_core_id = core_id;
      }

      if(current_core_id==core_id) {
	
	config_line >> string_temp;
	nb_cfg_core_to_load = utils_remove_bracket(string_temp);	
	num_cfg_core = new t_uint16[nb_cfg_core_to_load];
	if(nb_cfg_core_to_load>nb_cfg_core) {
	  EXPRINT(core, 0,"ERROR : too many CORE configurations to load! " 
		  << nb_cfg_core_to_load << " > " << nb_cfg_core);
	  exit(0);
	} else {
	  EXPRINT(core, 0,"| " << nb_cfg_core_to_load << " CORE configurations to load");
	}
	if(nb_cfg_core_to_load!=0) {
	  for (int i = 0; i < nb_cfg_core_to_load; i++) {
	    config_line >> num_cfg_core[i];
	    if(!config_line) {
	      PRINTL(0,nb_read_line,"ERROR : integer expected");
	      exit(0);	  
	    }
	  }
	} else {
	  EXPRINT(core, 0,"No CORE configuration to load");
	} 
      }
    }
  }
  config_file.close();
  
  EXPRINT(core, 0,"End reading (" << nb_read_line << " lines)"); //FIXPRINT
}



t_uint16 core_sysc::num_available_fifo_in(int num_fifo) {
  
  if(num_fifo<nb_fifo_in) {
    return fifo_in[num_fifo]->num_available();  
  } else {
    EXPRINT(core, 0,"WARNING : invalid FIFO IN number : " << num_fifo);
  }
  return 0;
  
}

t_uint16 core_sysc::num_free_fifo_out(int num_fifo) {
  
  if(num_fifo<nb_fifo_out) {
    return fifo_out[num_fifo]->num_free();  
  } else {
    EXPRINT(core, 0,"WARNING : invalid FIFO OUT number : " << num_fifo);
  }
  return 0;
 
}
