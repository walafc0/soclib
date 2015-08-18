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

#ifndef _ANOC_NODE_H_
#define _ANOC_NODE_H_


/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include "anoc_common.h"
#include "anoc_transaction.h"
#include "anoc_in_port.h"
#include "anoc_out_port.h"

#ifdef TLM_POWER_ESTIMATION
#include "tlm_power_anoc.h"
using Power::tlm_power_anoc;
#endif //TLM_POWER_ESTIMATION

/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/
//#define VERBOSE


#define ANOC_NODE_NB_INPUTS 5	// number of inputs per node
#define ANOC_NODE_NB_OUTPUTS 5	// number of outputs per node

 

/*------------------------------------------------------------------------------
 * CLASS : anoc_node
 *----------------------------------------------------------------------------*/
/*
   This class implements the ANOC node architecture & protocol.

   A node contains :
   - an input port node_in list, for all directions N/E/S/W/RES
   - an output port node_out list, for all directions N/E/S/W/RES
   - all memory ressources to store incoming transactions and build outgoing transactions
   - synchronizing event lists input_evt & output_evt, for all directions N/E/S/W/RES
   - one SC_METHOD for each input (for timing purpose) and one for each output (for arbitration purpose):
	* detection of begin-of-packet
	* arbitration law between virtual channels
	* handling of send/accept protocol
	* emission of outgoing transaction on output port & of accept values on input port
   
   The node inherits from tlm_blocking_put_if and consequently implements the put() method
   for both DATA and ACCEPT transactions.
   The put function stores incoming transaction and notifies relevant event
*/


class anoc_node :
  public sc_module, 
  public virtual tlm_blocking_put_if<anoc_data_transaction>,
  public virtual tlm_blocking_put_if<anoc_accept_transaction>
{

 public: 
  
  //--------------------------------------------------
  /// node input & output port pointer list (declared public to allow binding)
  //--------------------------------------------------

  anoc_in_port *node_in[ANOC_NODE_NB_INPUTS];
  anoc_out_port *node_out[ANOC_NODE_NB_OUTPUTS];

 private:

  //--------------------------------------------------
  /// node parameters
  //--------------------------------------------------

  /// node wait state
  int node_wait_state;

  // node auto_power
  bool power_on;
  int node_nb_msg;

#ifdef TLM_POWER_ESTIMATION
  tlm_power_anoc *pw_estim;	
#endif // TLM_POWER_ESTIMATION

  //--------------------------------------------------
  // node internal data structures
  //--------------------------------------------------

  // - input transactions: input_data[input_index][channel] + current_dest[input_index][channel]
  anoc_data_transaction input_data[ANOC_NODE_NB_INPUTS][ANOC_NB_CHANNEL];
  anoc_dir current_dest[ANOC_NODE_NB_INPUTS][ANOC_NB_CHANNEL];

  // - output request FIFOs (circular buffers) for new packets
  //   to respect order of arrival: packet_src[output_index][channel][req_index]
  anoc_dir packet_src[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL][ANOC_NODE_NB_INPUTS + 1]; // FIXME: ANOC_NODE_NB_INPUTS+1 pour packet en cours ?
  int packet_src_rd_idx[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL];
  int packet_src_wr_idx[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL];
  bool packet_started[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL];

  // - output transactions from each input: output_data[output_index][channel][input_index]
  //   + validity table output_data_valid[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL][ANOC_NODE_NB_INPUTS]
  anoc_data_transaction output_data[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL][ANOC_NODE_NB_INPUTS];
  anoc_data_transaction last_output_data[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL];
  bool last_output_is_eom[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL];
  bool output_data_valid[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL][ANOC_NODE_NB_INPUTS];

  // - output accept states: output_accept[output_index][channel]
  bool output_accept[ANOC_NODE_NB_OUTPUTS][ANOC_NB_CHANNEL];

  //--------------------------------------------------
  /// synchronisation event
  //--------------------------------------------------
  sc_event input_evt[ANOC_NODE_NB_INPUTS][ANOC_NB_CHANNEL];
  sc_event output_evt[ANOC_NODE_NB_OUTPUTS];

  //--------------------------------------------------
  // Print basic information on node creation 
  //--------------------------------------------------
  void print_info(const char * description = NULL) {
    
    if (description==NULL)
      printf("ANOC node creation: %s\n",name());
    else
      printf("ANOC node creation: %s (%s)\n",name(), description);

    if (node_wait_state)
      printf("\t%s: node wait state = %d\n", name(), node_wait_state);
  }


  //--------------------------------------------------------
  // tlm_blocking_put_if<> methods implementation   
  //--------------------------------------------------------
  virtual void put(const anoc_data_transaction& transaction);
  virtual void put(const anoc_accept_transaction& transaction);


  //--------------------------------------------
  // node module members functions
  //--------------------------------------------
  void anoc_node_init();
  void anoc_node_input_stage(anoc_dir input_idx, int channel);
#define DECLARE_INPUT_METHOD(dir, ch) inline void anoc_node_input_stage_##dir##_##ch() { anoc_node_input_stage(dir, ch); }
  DECLARE_INPUT_METHOD(NORTH, 0);
  DECLARE_INPUT_METHOD(NORTH, 1);
  DECLARE_INPUT_METHOD(EAST, 0);
  DECLARE_INPUT_METHOD(EAST, 1);
  DECLARE_INPUT_METHOD(SOUTH, 0);
  DECLARE_INPUT_METHOD(SOUTH, 1);
  DECLARE_INPUT_METHOD(WEST, 0);
  DECLARE_INPUT_METHOD(WEST, 1);
  DECLARE_INPUT_METHOD(RES, 0);
  DECLARE_INPUT_METHOD(RES, 1);
#undef DECLARE_INPUT_METHOD
  void anoc_node_output_stage(anoc_dir output_idx);
#define DECLARE_OUTPUT_METHOD(dir) inline void anoc_node_output_stage_##dir() { anoc_node_output_stage(dir); }
  DECLARE_OUTPUT_METHOD(NORTH);
  DECLARE_OUTPUT_METHOD(EAST);
  DECLARE_OUTPUT_METHOD(SOUTH);
  DECLARE_OUTPUT_METHOD(WEST);
  DECLARE_OUTPUT_METHOD(RES);
#undef DECLARE_OUTPUT_METHOD

public: 

  //--------------
  /// Constructor 
  /**
     The constructor takes as arguments :
     - the name of the module, 
     - the value for the node number of wait states
   **/
  //--------------
  SC_HAS_PROCESS(anoc_node);

  anoc_node(sc_module_name module_name_,
			int node_wait_state_ );
 
  //destructor
  virtual ~anoc_node();

  //----------------------
  // sc_object kind infos
  //----------------------
  static const char* const kind_string;
  virtual const char* kind() const {
    return kind_string;
  }

};


#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _ANOC_NODE_H_ */
