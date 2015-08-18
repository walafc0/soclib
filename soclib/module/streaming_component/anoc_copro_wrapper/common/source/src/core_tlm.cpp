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
 * Comment : Resource Core base class (TLM interface)
 *
 */

#include  "core_tlm.h"

// blocking read for the fifo in
t_uint32 core_tlm::read_fifo_in(int num_fifo) {
  t_uint32 data_tr;

  data_tr = fifo_in[num_fifo]->read();
  EXPRINT(core, 2,"Read " << data_tr << " in fifo_in[" << num_fifo  << "]"); //FIXPRINT
  size_to_released[num_fifo]++;
  fifo_in_released_event.notify(0,STEP_SIM_UNIT);
  if(!tab_input_fifo_accept[num_fifo]) {
    bool_write_input_fifo_accept[num_fifo]=true;
    #ifdef TLM_DVFS_MODELISATION
    lpm->notify_n_cycles(accept_fifo_in_event, 0.5);
    #else
    accept_fifo_in_event.notify(clk_period/2,STEP_SIM_UNIT);
    #endif
    //write_data_accept_fifo_in(num_fifo);
  }
  return data_tr;
}

// non blocking write for the fifo in
bool core_tlm::nb_write_fifo_in(int num_fifo, t_uint32 data_tw) {
  bool b_return; 

  b_return = fifo_in[num_fifo]->nb_write(data_tw);
  if(b_return) {
    EXPRINT(core, 2,"Write " << data_tw << " in fifo_in[" << num_fifo  << "]"); //FIXPRINT
  } else {
    EXPRINT(core, 0,"WARNING : core_tlm::nb_write_fifo_in : data skipped");
  }
  write_fifo_in_event.notify(0,STEP_SIM_UNIT);
  return b_return;
}

// non blocking read for the fifo out
bool core_tlm::nb_read_fifo_out(int num_fifo, t_uint32 &data_tr) {
  bool b_return;

  b_return = fifo_out[num_fifo]->nb_read(data_tr);
  if(b_return) {
    EXPRINT(core, 2,"Read " << data_tr << " in fifo_out[" << num_fifo  << "]"); //FIXPRINT
  } else {
    EXPRINT(core, 0,"WARNING : core_tlm::nb_read_fifo_out : data skipped");
  }
  available_out_event.notify(0,STEP_SIM_UNIT);
  read_fifo_out_event.notify(0,STEP_SIM_UNIT);
  return b_return; 
}

// blocking write for the fifo out
void core_tlm::write_fifo_out(int num_fifo, t_uint32 data_tw) {

  if(num_fifo<nb_fifo_out) {
    fifo_out[num_fifo]->write(data_tw);  
    EXPRINT(core, 2,"Write " << data_tw << " in fifo_out[" << num_fifo  << "]"); //FIXPRINT
    available_out_event.notify(0,STEP_SIM_UNIT);
  } else {
    EXPRINT(core, 0,"WARNING : invalid out FIFO number : " << num_fifo);
  }
}

// Receive EXEC transaction (remote call)
void core_tlm::put(const ni_exec_transaction &transaction) {
  current_slot_id = transaction.get_slot_id();
  EXPRINT(core, 2,"CORE Recv exec transaction for slot id : " << current_slot_id);
  
  load_cfg_event.notify(0,STEP_SIM_UNIT);
  // reset status
  write_status(0);
}

// Receive CFG DUMP transaction (remote call)
void core_tlm::put(const ni_cfg_dump_transaction &transaction) {
  if (const ni_cfg_transaction *tr = dynamic_cast<const ni_cfg_transaction*>(&transaction))
    put(*tr);
  else if (const ni_dump_transaction *tr = dynamic_cast<const ni_dump_transaction*>(&transaction))
    put(*tr);
  else {
    EXPRINT(core, -1, "ERROR: Forbidden transaction type 'cfg_dump' for put function!"); //FIXPRINT
    exit(-1);
  }
}

// Receive CFG transaction (remote call)
void core_tlm::put(const ni_cfg_transaction &transaction) {
  //FIXME
  write_register(transaction.get_address(),transaction.get_data()); 
  dump_data = transaction.get_data();
  #ifdef TLM_DVFS_MODELISATION
  lpm->notify_n_cycles(dump_data_event, 1);
  #else
  dump_data_event.notify(clk_period,STEP_SIM_UNIT);
  #endif
}

// Receive DUMP transaction (remote call)
void core_tlm::put(const ni_dump_transaction &transaction) {
  //FIXME
  EXPRINT(core, 3, "New Dump!"); //FIXPRINT
  dump_data = read_register(transaction.get_address());
  #ifdef TLM_DVFS_MODELISATION
  lpm->notify_n_cycles(dump_data_event, 1);
  #else
  dump_data_event.notify(clk_period,STEP_SIM_UNIT);
  #endif
}


// Put DUMP_DATA transaction on cfg_dump_port (sc_method)
void core_tlm::put_dump_data() {
  ni_dump_data_transaction dump_data_trans;

  dump_data_trans.set_data(dump_data);
  ni_cfg_dump_in->put(&dump_data_trans);
}

// Put WRITE_DATA transaction on fifo out (sc_method)
void core_tlm::put_data_out() {
  ni_write_data_transaction data_trans;
  t_uint32 data;

  EXPRINT(core, 3,"Put_data_out!"); //FIXPRINT
  for(int i=0; i<nb_fifo_out; i++) {
    if(tab_output_fifo_accept[i]) {

      if(!nb_read_fifo_out(i,data)) {
	EXPRINT(core, -1,"ERROR : try to read fifo out " << i << " but no data!");
	exit(0);
      }
      data_trans.set_data(data);
      data_trans.set_core_id(core_id);
      ni_output_fifo_out[i]->put(&data_trans);
      tab_output_fifo_accept[i]=false;
    }
  }
}

// Put RELEASED transaction (sc_method)
void core_tlm::put_released_out() {
  ni_released_transaction released_trans;  

  EXPRINT(core, 3,"Put released out!");
  for(int i=0; i<nb_fifo_in; i++) {
    if(size_to_released[i]!=0) {
      released_trans.set_data(size_to_released[i]);
      released_trans.set_core_id(core_id);
      ni_released_out[i]->put(&released_trans);
      size_to_released[i]=0;
    }
  }

}

// Write DATA ACCEPT transaction on fifo in
void core_tlm::put_accept_fifo_in() {
  t_uint16 num_fifo;
  ni_accept_data_transaction accept_trans;

  for(num_fifo=0; num_fifo<nb_fifo_in; num_fifo++) {
    if(bool_write_input_fifo_accept[num_fifo]==true) {
      bool_write_input_fifo_accept[num_fifo]=false;
      
      EXPRINT(core, 2,"Write data accept FIFO" << num_fifo);
      tab_input_fifo_accept[num_fifo]=true;
      accept_trans.set_core_id(core_id);
      ni_input_fifo_in[num_fifo]->put(&accept_trans);
      
    }
  }
  
}

// Write EOC transaction on exec port
void core_tlm::write_eoc() {
    ni_eoc_transaction eoc_trans;
    eoc_trans.set_core_id(core_id);
    ni_exec_in->put(&eoc_trans);  

}

// Write STATUS transaction on status port
void core_tlm::write_status(t_uint16 status_) {
    ni_status_transaction status_trans;
    status_trans.set_core_id(core_id);
    status_trans.set_status(status_);
    ni_status_out->put(&status_trans);  

}

void core_tlm::reset_status() {
  // Nothing to do!
}


// Put AVAILABLE transaction on available_out_port (sc_method)
void core_tlm::put_available_out() {
  ni_available_transaction available_trans;
  t_uint32 num_available;
  
  for(int i=0; i<nb_fifo_out; i++) {
    //update size available for fifo i
    num_available = fifo_out[i]->num_available();
    if(num_available!=previous_num_available[i]) {
      available_trans.set_data(num_available);
      available_trans.set_core_id(core_id);
      ni_available_out[i]->put(&available_trans);
      previous_num_available[i]=num_available;
    }
  }
  
  //fifo_out_event.notify(0,STEP_SIM_UNIT);
}


// Receive WRITE DATA transaction (remote call)
void core_tlm::put(const ni_write_data_transaction &transaction) {
  t_uint16 fifo_id;
  t_uint32 data;
  fifo_id = transaction.get_source_id();
  data = transaction.get_data();

  if(fifo_id<nb_fifo_in) {
    EXPRINT(core, 2,"Receive data " << data << " for FIFO " << fifo_id); //FIXPRINT
    
    //FIXME: write fifo in
    if(!nb_write_fifo_in(fifo_id, data)) {
      // ERROR or WARNING?
      EXPRINT(core, 0,"WARNING : try to write fifo_in " << fifo_id << " but full! (data dropped)");
      //exit(0);
    }
    if(fifo_in[fifo_id]->num_free()!=0) {
      bool_write_input_fifo_accept[fifo_id]=true;
      #ifdef TLM_DVFS_MODELISATION
      //lpm->notify_n_cycles(accept_fifo_in_event, read_rate);
      lpm->notify_n_cycles(accept_fifo_in_event, 0.5);
      #else
      accept_fifo_in_event.notify(clk_period/2,STEP_SIM_UNIT);
      #endif
      //write_data_accept_fifo_in(fifo_id); 
    } else {
      tab_input_fifo_accept[fifo_id]=false;
    }
  } else {
    EXPRINT(core, -1,"ERROR : invalid FIFO id for write data : " << fifo_id);
    exit(0);
  }

}


// Receive ACCEPT DATA transaction (remote call)
void core_tlm::put(const ni_accept_data_transaction &transaction) {
  t_uint16 fifo_id;
  fifo_id = transaction.get_source_id();

  if(fifo_id<nb_fifo_out) {
    EXPRINT(core, 2,"Receive accept for FIFO " << fifo_id); //FIXPRINT
    tab_output_fifo_accept[fifo_id]=true;

    // wait time before notification
    #ifdef TLM_DVFS_MODELISATION
    lpm->notify_n_cycles(fifo_out_event, 1);
    #else
    fifo_out_event.notify(clk_period,STEP_SIM_UNIT);
    #endif
    //fifo_out_event.notify(0,STEP_SIM_UNIT);
  } else {
    EXPRINT(core, -1,"ERROR : invalid FIFO id for accept data : " << fifo_id);
    exit(0);
  }

}


// Constructor 
core_tlm::core_tlm(sc_module_name module_name_,

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
			 string sno_name_
			 ) : core_sysc(module_name_,
				       clk_period_,
				       multicore_,
				       core_id_,
				       nb_fifo_in_,
				       size_fifo_in_,
				       nb_fifo_out_,
				       size_fifo_out_,
				       nb_cfg_core_,
				       cfg_size_core_,
				       static_init_,
				       sno_name_)

{

  // Bind Core ports to Core interface (for put function calls)
  core_put_if_bind();

  tab_input_fifo_accept = new bool[nb_fifo_in];
  for (int i=0; i<nb_fifo_in; i++) {
    tab_input_fifo_accept[i]=true;
  }  

  bool_write_input_fifo_accept = new bool[nb_fifo_in];
  for (int i=0; i<nb_fifo_in; i++) {
    bool_write_input_fifo_accept[i]=false;
  }  


  // intialize accept table for output fifo
  // table is updated when an accept transaction is received
  tab_output_fifo_accept = new bool[nb_fifo_out];
  for (int i=0; i<nb_fifo_out; i++) {
    tab_output_fifo_accept[i]=false;
  }  
 
  // method to put dump data
  SC_METHOD(put_dump_data);
  sensitive << dump_data_event;
  dont_initialize();

  // method to send data from fifo out
  SC_METHOD(put_data_out);
  sensitive << fifo_out_event;
  dont_initialize();

  // method to send released from fifo in
  SC_METHOD(put_released_out);
  sensitive << fifo_in_released_event;
  dont_initialize();

  // method to write accept for fifo in
  SC_METHOD(put_accept_fifo_in);
  sensitive << accept_fifo_in_event;
  dont_initialize();

  // method to send available
  SC_METHOD(put_available_out);
  sensitive << available_out_event;
  dont_initialize();
	
}


// Associate put functions with core ports
void core_tlm::core_put_if_bind() {

  EXPRINT(core, 0,"CORE put_if_bind");  
  ni_exec_in->put_if_bind(*this);        // for the exec target sub-port
  ni_cfg_dump_in->put_if_bind(*this);    // for the cfg_dump target sub-port
  for(int i=0; i<nb_fifo_in; i++) {
    ni_input_fifo_in[i]->put_if_bind(*this);        // for the data_port target sub-port
  }
  for(int i=0; i<nb_fifo_out; i++) {
    ni_output_fifo_out[i]->put_if_bind(*this);      // for the data_port target sub-port
  }
  EXPRINT(core, 0,"CORE put_if_bind done"); 

}

// Destructor
core_tlm::~core_tlm() {

}


// Address map
void core_tlm::write_register(t_uint32 addr, t_uint32 data) {
  core_sysc::write_register(addr,data);
}


t_uint32 core_tlm::read_register(t_uint32 addr) {
  EXPRINT(core, 0,"WARNING : invalid read register address");
  return 0;
}


