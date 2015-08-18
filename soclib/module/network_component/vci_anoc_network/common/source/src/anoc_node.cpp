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
#include "anoc_node.h"

/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/
//#define VERBOSE


/*------------------------------------------------------------------------------
 * Global Variables							       
 *----------------------------------------------------------------------------*/

// sc_object kind string property
const char* const anoc_node::kind_string = "anoc_node";


//-------------------------------------------------
// anoc_node Constructor
//-------------------------------------------------

anoc_node::anoc_node(sc_module_name module_name_,	
                     int node_wait_state_ ) :
  sc_module(module_name_)
{
#ifdef TLM_POWER_ESTIMATION
  pw_estim = new tlm_power_anoc( (std::string) ((const char *) module_name_ ));
#endif // TLM_POWER_ESTIMATION

  // wait state
  node_wait_state = node_wait_state_;

  // input/output ports creation and binding
  char tmp_name[80];
#ifdef TLM_TRANS_RECORD
  // create names ...
  char port_recorder_name[ANOC_NODE_NB_INPUTS + ANOC_NODE_NB_OUTPUTS][80];
  sprintf(port_recorder_name[0],"%s_in_north", basename());
  sprintf(port_recorder_name[1],"%s_in_east", basename());
  sprintf(port_recorder_name[2],"%s_in_south", basename());
  sprintf(port_recorder_name[3],"%s_in_west", basename());
  sprintf(port_recorder_name[4],"%s_in_res", basename());
  sprintf(port_recorder_name[ANOC_NODE_NB_INPUTS + 0],"%s_out_north", basename());
  sprintf(port_recorder_name[ANOC_NODE_NB_INPUTS + 1],"%s_out_east", basename());
  sprintf(port_recorder_name[ANOC_NODE_NB_INPUTS + 2],"%s_out_south", basename());
  sprintf(port_recorder_name[ANOC_NODE_NB_INPUTS + 3],"%s_out_west", basename());
  sprintf(port_recorder_name[ANOC_NODE_NB_INPUTS + 4],"%s_out_res", basename());
#endif // TLM_TRANS_RECORD

  for (int i = 0; i < ANOC_NODE_NB_INPUTS; i++) {
    sprintf(tmp_name, "anoc_node_in_%d", i);
#ifdef TLM_TRANS_RECORD
    node_in[i] = new anoc_in_port(tmp_name, port_recorder_name[i]);
#else   // TLM_TRANS_RECORD
    node_in[i] = new anoc_in_port(tmp_name);
#endif  // TLM_TRANS_RECORD

    node_in[i]->slave_bind(*this);	// for the data target sub-port

#if MAGALI_PLATFORM == MAGALI_PLATFORM_SIMU_V2
    node_in[i]->data_port->set_port_id(i);
#else
    node_in[i]->data_port->set_tlm_export_id(i);
#endif
  }
  
  for (int i = 0; i < ANOC_NODE_NB_OUTPUTS; i++) {
    sprintf(tmp_name,"anoc_node_out_%d", i);
#ifdef TLM_TRANS_RECORD
    node_out[i] = new anoc_out_port(tmp_name, port_recorder_name[ANOC_NODE_NB_INPUTS + i]);
#else // TLM_TRANS_RECORD
    node_out[i] = new anoc_out_port(tmp_name);
#endif // TLM_TRANS_RECORD

    node_out[i]->slave_bind(*this);	// for the accept target sub-port
    
#if MAGALI_PLATFORM == MAGALI_PLATFORM_SIMU_V2
    node_out[i]->accept_port->set_port_id(i);
#else
    node_out[i]->accept_port->set_tlm_export_id(i);    
#endif
  }

  // register methods
#define REGISTER_INPUT_METHOD(dir, ch) SC_METHOD(anoc_node_input_stage_##dir##_##ch); \
                                       sensitive << input_evt[dir][ch]; \
                                       dont_initialize();
  REGISTER_INPUT_METHOD(NORTH, 0);
  REGISTER_INPUT_METHOD(NORTH, 1);
  REGISTER_INPUT_METHOD(EAST, 0);
  REGISTER_INPUT_METHOD(EAST, 1);
  REGISTER_INPUT_METHOD(SOUTH, 0);
  REGISTER_INPUT_METHOD(SOUTH, 1);
  REGISTER_INPUT_METHOD(WEST, 0);
  REGISTER_INPUT_METHOD(WEST, 1);
  REGISTER_INPUT_METHOD(RES, 0);
  REGISTER_INPUT_METHOD(RES, 1);
#undef REGISTER_INPUT_METHOD

#define REGISTER_OUTPUT_METHOD(dir) SC_METHOD(anoc_node_output_stage_##dir); \
                                    sensitive << output_evt[dir]; \
                                    dont_initialize();
  REGISTER_OUTPUT_METHOD(NORTH);
  REGISTER_OUTPUT_METHOD(EAST);
  REGISTER_OUTPUT_METHOD(SOUTH);
  REGISTER_OUTPUT_METHOD(WEST);
  REGISTER_OUTPUT_METHOD(RES);
#undef REGISTER_OUTPUT_METHOD

  // initialise node ressources
  anoc_node_init();
      
}

//-------------------------------------------------
// anoc_node Destructor
//-------------------------------------------------

anoc_node::~anoc_node(){
#ifdef TLM_POWER_ESTIMATION
  delete pw_estim;
  for (int i = 0; i < ANOC_NODE_NB_OUTPUTS; i++)
    delete node_out[i];
#endif // TLM_POWER_ESTIMATION
}


//-------------------------------------------------
// anoc_node_init() function
// - initialization of all memory ressources
//   => output accept values are initialised to true : always ready to accept a new value
//-------------------------------------------------

void anoc_node::anoc_node_init() {

  // init node power state
  power_on = false; 

  // init number of messages
  node_nb_msg = 0;

  for (int i = 0; i < ANOC_NODE_NB_INPUTS; i++) {
    for (int j = 0; j < ANOC_NB_CHANNEL; j++) {
      current_dest[i][j] = NORTH;
    }
  }

 for (int i = 0; i < ANOC_NODE_NB_OUTPUTS; i++) {
    for (int j = 0; j < ANOC_NB_CHANNEL; j++) {
      for (int k = 0; k < ANOC_NODE_NB_INPUTS; k++) {
        output_data_valid[i][j][k] = false;
      }
      packet_src_rd_idx[i][j] = 0;
      packet_src_wr_idx[i][j] = 0;
      packet_started[i][j] = false;
      output_accept[i][j] = true; // output accept is initialized to true: ready to accept a new value
    }
  }
  EXPRINT(node, 0, "anoc node initialisation completed");
}

//-------------------------------------------------
//
// tlm_blocking_put_if<> methods implementation   
//
// Implementation of the put() method as defined in the tlm_blocking_put_if interface.
// - It basically extracts the transaction payload from the transaction 
//   and writes corresponding values locally within the anoc node.
//
// - for accept type transaction, fill the output_accept[][] table, and wake up output arbitration.
// - for data type transaction, fill the input_data[][] table
//   => it is necessary to use a temporary table for inputs to be able to wait for some time before transfer to arbiter
//
// - after some delay (node_wait_state), input_evt is raised and packet_src[][][] (for a header) and output_data[][]
//   are updated, and output arbitration is waken up.
//   => arbitration of each output is independent, so that arbitration is observed.
//-----------------------------------------------------------------
inline void anoc_node::put(const anoc_data_transaction& transaction) {
  if (transaction.is_bop() && transaction.is_bom()) {
    node_nb_msg++;
    EXPRINT(node, 2, "BOM BOM " << node_nb_msg);
    if (power_on == false) {
      // power on
      EXPRINT(node, 1, "POWER ON ");
      power_on = true;
#ifdef TLM_POWER_ESTIMATION
      pw_estim->new_power_mode(Power::tlm_power_anoc::ON);
#endif
    }
  }
#ifdef TLM_POWER_ESTIMATION
  pw_estim->new_power_phase(Power::tlm_power_anoc::PROCEED);
#endif
  int i, j;
  // get the input channel number (target port id) from which communication is received
  // (target port id corresponds to port anoc_in.data id number)
  if (transaction.get_target_port_id() != -1)
    i = transaction.get_target_port_id();
  else
    i = RES;

  j = transaction.get_channel();
  // Store the whole transaction and wait for some time before waking up input method...
  input_data[i][j] = transaction;
  EXPRINT(node, 2, "New DATA on input " << (anoc_dir)i << ", channel " << j << ": " << input_data[i][j]);
#ifdef TLM_POWER_ESTIMATION
  if (power_on)
    input_evt[i][j].notify(pw_estim->node_wait_state_on, SC_PS);
  else
    input_evt[i][j].notify(pw_estim->node_wait_state_off, SC_PS);			
#else
  input_evt[i][j].notify(node_wait_state, STEP_SIM_UNIT);
#endif
}


inline void anoc_node::put(const anoc_accept_transaction& transaction) {
  // get the output channel number (target port id) from which communication is received
  // (target port id correspond to port anoc_out.accept id number)
  int i = transaction.get_target_port_id();
  // get the virtual channel number
  int j = transaction.get_channel();

  // copy accept value from transaction
  output_accept[i][j] = true;
  EXPRINT(node, 2, "New ACCEPT on output " << (anoc_dir)i << ", channel " << j << ": " << transaction);

  // check for end of packet and end of message
  if (last_output_data[i][j].is_eop()) {
    // free packet request
    packet_started[i][j] = false;
    EXPRINT(node, 2, "End of packet, from " << packet_src[i][j][packet_src_rd_idx[i][j]]
            << " to " << (anoc_dir)i << " (channel " << j << ")");
    packet_src_rd_idx[i][j] = (packet_src_rd_idx[i][j] + 1) % (ANOC_NODE_NB_INPUTS + 1);
    EXPRINT(node, 2, "(" << ((packet_src_wr_idx[i][j] + ANOC_NODE_NB_INPUTS + 1 - packet_src_rd_idx[i][j])
                             % (ANOC_NODE_NB_INPUTS + 1)) << " packet(s) in FIFO to this output & channel)");
    if (last_output_is_eom[i][j]) {
      // notify EOM event
      node_nb_msg--;
      EXPRINT(node, 2, "EOM EOM " << node_nb_msg);
      if (node_nb_msg == 0) {
        // power off
        EXPRINT(node, 1, "POWER OFF");
        power_on = false;
#ifdef TLM_POWER_ESTIMATION
        pw_estim->new_power_mode(Power::tlm_power_anoc::IDLE);
#endif
      }
    }
  }
  // wake up relevant arbitration method
  output_evt[i].notify(SC_ZERO_TIME);
}


//-------------------------------------------------
// anoc_node_input_stage(input, channel) method
// - store incomin on channel 1g flit, and if it is a header shift paht and add flit to request FIFO
// - wake up relevant output arbitration method
//-------------------------------------------------
void anoc_node::anoc_node_input_stage(anoc_dir input_idx, int channel) {
  if (input_data[input_idx][channel].is_bop()) {
    // handle the 'path' information :
    // => in case the incoming transaction path information corresponds to the port id (path == NORTH on port NORTH),
    //    the path actually correspond to the RESSOURCE target.
    anoc_dir dest = input_data[input_idx][channel].get_path();
    if (dest == input_idx)
      dest = RES;
    current_dest[input_idx][channel] = dest;
    packet_src[dest][channel][packet_src_wr_idx[dest][channel]] = input_idx;
    packet_src_wr_idx[dest][channel] = (packet_src_wr_idx[dest][channel] + 1) % (ANOC_NODE_NB_INPUTS + 1);
    if (packet_src_wr_idx[dest][channel] == packet_src_rd_idx[dest][channel]) {
      EXPRINT(node, -1, "BUG in anoc_node model: packet_src[" << dest << "][" << channel << "] is full!");
      exit(1);
    }
    input_data[input_idx][channel].shift_path();
    EXPRINT(node, 2, "New begin of packet, from " << input_idx << " to " << dest << " (channel " << channel << ")");
    EXPRINT(node, 2, "(" << ((packet_src_wr_idx[dest][channel] + ANOC_NODE_NB_INPUTS + 1 - packet_src_rd_idx[dest][channel])
                             % (ANOC_NODE_NB_INPUTS + 1)) << " packet(s) in FIFO to this output & channel)");
  }
  // store incoming transaction
  output_data[current_dest[input_idx][channel]][channel][input_idx] = input_data[input_idx][channel];
  if (output_data_valid[current_dest[input_idx][channel]][channel][input_idx]) {
    EXPRINT(node, -1, "BUG in anoc_node model: output_data_valid[" << current_dest[input_idx][channel]
            << "][" << channel << "][" << input_idx << "] is already true!");
    exit(1);
  }
  output_data_valid[current_dest[input_idx][channel]][channel][input_idx] = true;
  // wake up output stage method in next delta cycle
  output_evt[current_dest[input_idx][channel]].notify(SC_ZERO_TIME);
#ifdef TLM_POWER_ESTIMATION
	pw_estim->new_power_phase(Power::tlm_power_anoc::WAIT);
#endif
}


//-------------------------------------------------
// anoc_node_output_stage(anoc_dir) method
// - arbitrates packet requests to output
// - forward the data transactions accordingly until end of packet
//-------------------------------------------------
void anoc_node::anoc_node_output_stage(anoc_dir output_idx) {
  // for each virtual channel, in priority order
  for (int channel = 0; channel < ANOC_NB_CHANNEL; channel++) {
    // if no packet is started and packet_src FIFO is not empty, then deal with next packet
    if (!packet_started[output_idx][channel] && (packet_src_wr_idx[output_idx][channel]
                                                 != packet_src_rd_idx[output_idx][channel]))
      packet_started[output_idx][channel] = true;

    // if a packet is being forwarded, check output accept state
    if (packet_started[output_idx][channel] && output_accept[output_idx][channel]) {
      anoc_dir input_idx = packet_src[output_idx][channel][packet_src_rd_idx[output_idx][channel]];
      // check availability of input data
      if (output_data_valid[output_idx][channel][input_idx]) {
        EXPRINT(node, 2, "Sending flit from input " << input_idx
                << " to output " << output_idx << "(channel " << channel << ")");
        // write data on output port
        output_data_valid[output_idx][channel][input_idx] = false;
        last_output_data[output_idx][channel] = output_data[output_idx][channel][input_idx];
        if (last_output_data[output_idx][channel].is_bop()) // keep track of EOM for this packet
          last_output_is_eom[output_idx][channel] = last_output_data[output_idx][channel].is_eom();
        node_out[output_idx]->data_port->write(output_data[output_idx][channel][input_idx]);
        output_accept[output_idx][channel] = false;
        // acknowledge transaction to the sender
        anoc_accept_transaction accept(channel);
        node_in[input_idx]->accept_port->write(accept);
        // arbitration done, no other virtual channel can be served for the moment
        break;
      }
    }
  }
}

