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
 * Comment : FAUST2 stopper to bind with unused node ports
 *
 */

#include  "anoc_stopper.h"


// Receive DATA transaction (remote call)
void anoc_stopper::put(const anoc_data_transaction& transaction) {

  PRINT(0,"WARNING : Resource Empty received new flit on channel " << transaction.get_channel());

  // new packet
  if (transaction.is_bop()) {
    t_uint16 param1 = transaction.get_param1();
    t_uint16 param2 = transaction.get_param2();

    PRINT(0,"Recv header flit with command : " << transaction.get_cmd() \
	  << " and params : " << param1 << " & "  << param2);
  }

  if(level!=NO_TRACE_STOPPER&&!stopper_data_stream) {
    create_stream();
  }

  if(level==DATA_TRACE_STOPPER) {
    // fill in file stopper_data_stream
    if(transaction.is_bop()) {
      if(transaction.get_cmd()==DATA) {
        current_packet_is_data=true;
      } else {
        current_packet_is_data=false;
      }
    }
    if (stopper_data_stream && current_packet_is_data==true && !transaction.is_bop()) {
      *stopper_data_stream << "> ";
      stopper_data_stream->width(8);
      stopper_data_stream->fill('0');
      *stopper_data_stream << hex << transaction.get_data() << dec << endl;
    }

    if(transaction.is_eop()) {
      current_packet_is_data=true;
    }
  }
  else if (level==ALL_TRACE_STOPPER) {
    transaction.trace_in_file(stopper_data_stream);
  }

  //notify accept event
  write_accept_event[transaction.get_channel()].notify(node_wait,STEP_SIM_UNIT);

}

// Receive ACCEPT transaction (remote call)
void anoc_stopper::put(const anoc_accept_transaction& transaction) {
  PRINT(2,"Received new accept on channel " << transaction.get_channel());
}

// Print basic information on resource creation
void anoc_stopper::print_resource_info(const char *description = NULL) const {

  if (description==NULL)
    printf("ANOC Resource creation: %s, res_id: %d \n",name(), res_id);
  else
    printf("ANOC Resource creation: %s, res_id: %d (%s)\n",name(), res_id, description);
}

// Constructor
anoc_stopper::anoc_stopper(sc_module_name module_name_,
                             int res_id_,
                             int clk_period_,  int node_wait_,
                             trace_level level_) :
  sc_module(module_name_),
  res_id(res_id_),
  clk_period(clk_period_),
  node_wait(node_wait_),
  level(level_) {

  // create noc ports
#ifdef TLM_TRANS_RECORD
  char port_in[80];
  char port_out[80];

  sprintf(port_in,"%s_IN", basename());
  sprintf(port_out,"%s_OUT", basename());
  noc_in  = new anoc_in_port("res_in_port", (char *)port_in);
  noc_out = new anoc_out_port("res_out_port", (char *)port_out);
#else
  noc_in  = new anoc_in_port("res_in_port");
  noc_out = new anoc_out_port("res_out_port");
#endif // TLM_TRANS_RECORD

  // bind input/output ports to the ressource module
  noc_in->slave_bind(*this);        // for the data target sub-port
  noc_out->slave_bind(*this);       // for the accept target sub-port

  // methods for automatic accept return
  SC_METHOD(write_accept_0);
  sensitive << write_accept_event[0];
  dont_initialize();
  SC_METHOD(write_accept_1);
  sensitive << write_accept_event[1];
  dont_initialize();

  // Remove file
  remove(get_file_name().c_str());
  stopper_data_stream = NULL;
  current_packet_is_data = true;
}

// Write ACCEPT transaction on channel 0 (sc_method)
void anoc_stopper::write_accept_0() {
  anoc_accept_transaction accept(0);
  noc_in->accept_port->write(accept);
  PRINT(2, "Accept sent on channel 0");
}

// Write ACCEPT transaction on channel 1 (sc_method)
void anoc_stopper::write_accept_1() {
  anoc_accept_transaction accept(1);
  noc_in->accept_port->write(accept);
  PRINT(2, "Accept sent on channel 1");
}

anoc_stopper::~anoc_stopper() {
  if(stopper_data_stream) {
    *stopper_data_stream << "######## end of data file" << endl;
    stopper_data_stream->close();
  }
}

std::string anoc_stopper::get_file_name() {
  std::string file_name;
  file_name = PATH_TRACE_FILE; file_name += basename(); file_name += EXT_TRACE_FILE;
  return file_name;
}

void anoc_stopper::create_stream() {
  std::string file_name;
  if(level != NO_TRACE_STOPPER) {
    file_name = get_file_name();
    stopper_data_stream = new ofstream(file_name.c_str());
    if (!stopper_data_stream) {
      EXPRINT(res, 0,"ERROR : could not open " << file_name);
      exit(0);
    } else {
      EXPRINT(res, 0,"Create trace file: " << file_name);
    }
    *stopper_data_stream << "# STOPPER TRACE FILE : " << basename() << endl;
    *stopper_data_stream << "########" << endl;
    if(level == ALL_TRACE_STOPPER) {
      *stopper_data_stream << "#> bop eop DATA" << endl;
    } else {
      *stopper_data_stream << "#> DATA" << endl;
    }
    *stopper_data_stream << "########" << endl;
  } else {
    stopper_data_stream = NULL;
  }
}

