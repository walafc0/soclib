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
 * Comment : Resource Core base class
 *
 */

#include  "core_base.h"

// Constructor 
core_base::core_base(sc_module_name module_name_,
                     int clk_period_,
		     bool multicore_,
		     t_uint16 core_id_,
                     t_uint16 nb_fifo_in_,
                     t_uint16 nb_fifo_out_) :
  sc_module(module_name_),
  clk_period(clk_period_),
  multicore( multicore_),
  core_id(core_id_),
  nb_fifo_in(nb_fifo_in_),
  nb_fifo_out(nb_fifo_out_)
{

  EXPRINT(core, 0,"Initialize CORE");

  if(multicore) {
    EXPRINT(core, 0,"CORE ID : " << core_id);
  }

#ifdef TLM_TRANS_RECORD 
  char port_in[80];
  char port_out[80];
#endif // TLM_TRANS_RECORD
  char port_in_name[80];
  char port_out_name[80];

  // create ni => core ports
#ifdef TLM_TRANS_RECORD
  sprintf(port_in,"%s_exec_in", basename());
  ni_exec_in  = new ni_exec_in_port("ni_exec_in_port", (char *)port_in, "exec", "status");
#else
  ni_exec_in  = new ni_exec_in_port("ni_exec_in_port", "exec", "status");
#endif // TLM_TRANS_RECORD
  // bind input/output ports to the ressource module
  //ni_exec_in->slave_bind(*this);        // for the exec target sub-port

#ifdef TLM_TRANS_RECORD
  sprintf(port_in,"%s_cfg_dump_in", basename());
  ni_cfg_dump_in  = new ni_cfg_dump_in_port("ni_cfg_dump_in_port", (char *)port_in, "cfg_dump", "dump_data");
#else
  ni_cfg_dump_in  = new ni_cfg_dump_in_port("ni_cfg_dump_in_port", "cfg_dump", "dump_data");
#endif // TLM_TRANS_RECORD
  // bind input/output ports to the ressource module
  //ni_cfg_dump_in->slave_bind(*this);        // for the cfg_dump target sub-port


  ni_input_fifo_in = new ni_data_in_port*[nb_fifo_in];
  for(int i=0;i<nb_fifo_in;i++) {
    sprintf(port_in_name,"ni_input_fifo_in_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_in,"%s_input_fifo_in_%i", basename(), i);
    ni_input_fifo_in[i]  = new ni_data_in_port((char *) port_in_name, (char *)port_in, i);
#else
    ni_input_fifo_in[i]  = new ni_data_in_port((char *) port_in_name, i);
#endif // TLM_TRANS_RECORD
    // bind input/output ports to the ressource module
    //ni_input_fifo_in[i]->slave_bind(*this);        // for the data_port target sub-port
  }


  // create core => ni ports
  ni_output_fifo_out = new ni_data_out_port*[nb_fifo_out];
  for(int i=0;i<nb_fifo_out;i++) {
    sprintf(port_out_name,"ni_output_fifo_out_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_out,"%s_output_fifo_out_%i", basename(), i);
    ni_output_fifo_out[i]  = new ni_data_out_port((char *) port_out_name, (char *)port_out, i);
#else
    ni_output_fifo_out[i]  = new ni_data_out_port((char *) port_out_name, i);
#endif // TLM_TRANS_RECORD
    // bind output/output ports to the ressource module
    //ni_output_fifo_out[i]->slave_bind(*this);        // for the data_port target sub-port
  }

  ni_released_out = new ni_released_out_port*[nb_fifo_in];
  for(int i=0;i<nb_fifo_in;i++) {
    sprintf(port_out_name,"ni_released_out_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_out,"%s_released_out_%i", basename(), i);
    ni_released_out[i] = new ni_released_out_port((char *) port_out_name, (char *)port_out, i);
#else
    ni_released_out[i] = new ni_released_out_port((char *) port_out_name, i);
#endif // TLM_TRANS_RECORD
  }

  ni_available_out = new ni_available_out_port*[nb_fifo_out];
  for(int i=0;i<nb_fifo_out;i++) {
    sprintf(port_out_name,"ni_available_out_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_out,"%s_available_out_%i", basename(), i);
    ni_available_out[i] = new ni_available_out_port((char *) port_out_name, (char *)port_out, i);
#else
    ni_available_out[i] = new ni_available_out_port((char *) port_out_name, i);
#endif // TLM_TRANS_RECORD
  }

#ifdef TLM_TRANS_RECORD
  sprintf(port_out,"%s_status_out", basename());
  ni_status_out = new ni_status_out_port("ni_status_port_out", (char *)port_out);
#else
  ni_status_out = new ni_status_out_port("ni_status_port_out");
#endif // TLM_TRANS_RECORD



}

// Destructor
core_base::~core_base() {
  //FIXME: delete[] sur tous les new[]
}
