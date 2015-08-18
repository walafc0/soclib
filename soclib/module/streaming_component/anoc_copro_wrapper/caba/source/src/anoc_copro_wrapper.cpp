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

#include  "anoc_copro_wrapper.h"
namespace soclib { namespace caba {
using soclib::common::alloc_elems;
using soclib::common::dealloc_elems;

/*******************************************************************************/
/*******************************************************************************/
/// Constructor
anoc_copro_wrapper::anoc_copro_wrapper(sc_module_name module_name_,
                           t_uint16 nb_cores_,
                           t_uint16 nb_fifo_in_,
                           t_uint16 nb_fifo_out_,
                           t_uint16 n_config_,
                           t_uint16 n_status_,
                           t_uint32 clk_period_) :
                             sc_module(module_name_),
                             p_from_MWMR(alloc_elems<FifoInput<uint32_t> >("p_from_MWMR", nb_fifo_in_)),
                             p_to_MWMR(alloc_elems<FifoOutput<uint32_t> >("p_to_MWMR", nb_fifo_out_ )),
                             p_config(alloc_elems<sc_in<uint32_t> >("p_config", n_config_)),
                             p_status(alloc_elems<sc_out<uint32_t> >("p_status", n_status_)),
                             nb_cores(nb_cores_),
                             nb_fifo_in(nb_fifo_in_),
                             nb_fifo_out(nb_fifo_out_),
                             clk_period(clk_period_)
{
  core_fifo_in = new t_uint16[nb_fifo_in];
  for(int i=0; i<nb_fifo_in; i++) {
    if(nb_cores>1) {
//      core_fifo_in[i] = core_fifo_in_[i];
    } else {
      core_fifo_in[i] = 0; // monocore, core_fifo_in_=NULL
    }
  }
  core_fifo_out = new t_uint16[nb_fifo_out];
  for(int i=0; i<nb_fifo_out; i++) {
    if(nb_cores>1) {
//      core_fifo_out[i] = core_fifo_out_[i];
    } else {
      core_fifo_out[i] = 0; // monocore, core_fifo_out_=NULL
    }
  }

  cfg_flag_core_finished = new bool [nb_cores];
  for(int i = 0; i < nb_cores; i++) {
    cfg_flag_core_finished[i]=true;
  }


  // create ni => core ports
#ifdef TLM_TRANS_RECORD
  char port_in[80];
  char port_out[80];
  char port_debug[80];
#endif
  char port_out_name[80];
  char port_in_name[80];

#ifdef TLM_TRANS_RECORD
  sprintf(port_in,"%s_IN", basename());
  sprintf(port_out,"%s_OUT", basename());
  sprintf(port_debug,"%s_DEBUG", basename());
#endif // TLM_TRANS_RECORD
  
  ni_exec_out = new ni_exec_out_port*[nb_cores];
  for(int i=0;i<nb_cores;i++) {
    sprintf(port_out_name,"ni_exec_out_port_%i", i);
    ni_exec_out[i] = new ni_exec_out_port((char *) port_out_name, "exec", "eoc");
    ni_exec_out[i]->put_if_bind(*this);       // for the status target sub-port
  }

  ni_cfg_dump_out = new ni_cfg_dump_out_port("ni_cfg_dump_out_port", "cfg_dump", "dump_data");
  ni_cfg_dump_out->put_if_bind(*this);       // for the status target sub-port


  t_uint16 fifo_id[nb_cores];
  for (int i=0; i<nb_cores; i++) {
    fifo_id[i]=0;
  }
  ni_input_fifo_out = new ni_data_out_port*[nb_fifo_in];
  for(int i=0;i<nb_fifo_in;i++) {
    sprintf(port_out_name,"ni_input_fifo_out_port_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_out,"%s_input_fifo_out_%i", basename(), i);
    ni_input_fifo_out[i] = new ni_data_out_port((char *) port_out_name, (char *)port_out, fifo_id[core_fifo_in[i]]);  
#else
    ni_input_fifo_out[i] = new ni_data_out_port((char *) port_out_name, fifo_id[core_fifo_in[i]]);
#endif // TLM_TRANS_RECORD
    fifo_id[core_fifo_in[i]]++;
    ni_input_fifo_out[i]->put_if_bind(*this);       // for the status target sub-port
  }

  // create core => ni ports
  for (int i=0; i<nb_cores; i++) {
    fifo_id[i]=0;
  }
  ni_output_fifo_in = new ni_data_in_port*[nb_fifo_out];
  for(int i=0;i<nb_fifo_out;i++) {
    sprintf(port_in_name,"ni_output_fifo_in_port_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_in,"%s_output_fifo_in_%i", basename(), i);
    ni_output_fifo_in[i] = new ni_data_in_port((char *) port_in_name, (char *)port_in, fifo_id[core_fifo_out[i]]);
#else
    ni_output_fifo_in[i] = new ni_data_in_port((char *) port_in_name, fifo_id[core_fifo_out[i]]);
#endif // TLM_TRANS_RECORD
    fifo_id[core_fifo_out[i]]++;
    ni_output_fifo_in[i]->put_if_bind(*this);       // for the status target sub-port
  }

  ni_released_in = new ni_released_in_port*[nb_fifo_in];  
  for(int i=0;i<nb_fifo_in;i++) {
    sprintf(port_in_name,"ni_released_in_port_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_in,"%s_released_in_%i", basename(), i);
    ni_released_in[i] = new  ni_released_in_port((char *) port_in_name, (char *)port_in);
#else
    ni_released_in[i] = new  ni_released_in_port((char *) port_in_name);
#endif // TLM_TRANS_RECORD
    ni_released_in[i]->put_if_bind(*this);       // for the status target sub-port
  }

  ni_available_in = new ni_available_in_port*[nb_fifo_out];   
  for(int i=0;i<nb_fifo_out;i++) {
    sprintf(port_in_name,"ni_available_in_port_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_in,"%s_available_in_%i", basename(), i);
    ni_available_in[i] = new ni_available_in_port((char *) port_in_name, (char *)port_in);
#else
    ni_available_in[i] = new ni_available_in_port((char *) port_in_name);
#endif // TLM_TRANS_RECORD
    ni_available_in[i]->put_if_bind(*this);       // for the status target sub-port
  }

  ni_status_in = new ni_status_in_port*[nb_cores];   
  for(int i=0;i<nb_cores;i++) {
    sprintf(port_in_name,"ni_status_in_port_%i", i);
#ifdef TLM_TRANS_RECORD
    sprintf(port_in,"%s_status_in_%i", basename(), i);
    ni_status_in[i] = new ni_status_in_port((char *) port_in_name, (char *)port_in);
#else
    ni_status_in[i] = new ni_status_in_port((char *) port_in_name);
#endif // TLM_TRANS_RECORD
    ni_status_in[i]->put_if_bind(*this);       // for the status target sub-port 
  }


  /***********************************************/
  // internal programmation

  /***********************************************/
  // internal state variables

  last_status = new t_uint32[nb_cores];
  for(int i=0; i<nb_cores; i++) {
    last_status[i] = 0;
  }

  tab_out_data_fifo_out = new t_uint32[nb_fifo_out];
  flag_tab_out_data_fifo_out = new bool[nb_fifo_out];
  tab_available_data_fifo_out = new t_uint32[nb_fifo_out];
  for (int i = 0; i < nb_fifo_out; i++) {
    tab_out_data_fifo_out[i] = 0;    
    flag_tab_out_data_fifo_out[i] = false;
    tab_available_data_fifo_out[i]=0;
  }
  tab_fifo_in_accept = new bool[nb_fifo_in];
  for (int i = 0; i < nb_fifo_in; i++) {
    // fifo accept is initialised to true: ready to accept a new value
    tab_fifo_in_accept[i] = true;
  }

  
  /***********************************************/
  // threads & methods
  SC_METHOD(rtl_to_tlm);
  sensitive << rtl_to_tlm_event;
  dont_initialize();

  SC_METHOD(tlm_to_rtl);
  sensitive << p_clk.neg();  
  dont_initialize();

  EXPRINT(ni, 2,"anoc_copro_wrapper CREATED");
}

/*******************************************************************************/
/*******************************************************************************/
/// Destructor
anoc_copro_wrapper::~anoc_copro_wrapper() {

  cout << "-------------- DELETE " << name() << " -------------- " << endl;



  delete core_fifo_in;
  delete core_fifo_out;
  for(int i=0;i<nb_cores;i++) {
    delete ni_exec_out[i];
  }
  delete ni_exec_out;
  delete ni_cfg_dump_out;
  for(int i=0;i<nb_fifo_in;i++) {
    delete ni_input_fifo_out[i];
  }
  delete ni_input_fifo_out;
  for(int i=0;i<nb_fifo_out;i++) {
    delete ni_output_fifo_in[i];
  }
  delete ni_output_fifo_in;
  for(int i=0;i<nb_fifo_in;i++) {
    delete ni_released_in[i];
  }
  delete ni_released_in;
  for(int i=0;i<nb_fifo_out;i++) {
    delete ni_available_in[i];
  }
  delete ni_available_in;
  for(int i=0;i<nb_cores;i++) {
    delete ni_status_in[i];
  }
  delete ni_status_in;

}


/*******************************************************************************/
/*******************************************************************************/
/// Receive DUMP DATA transaction (remote call)
void anoc_copro_wrapper::put(const ni_dump_data_transaction &transaction) {
  //Not yet implemented
#ifdef _ANOC_COPRO_DEBUG
  printf("DUMP DATA transaction\n");
#endif
}


/*******************************************************************************/
/*******************************************************************************/
/// Receive RELEASED transaction (remote call)
void anoc_copro_wrapper::put(const ni_released_transaction &transaction) {
#ifdef _ANOC_COPRO_DEBUG
  printf("RELEASED transaction\n");
#endif
  t_uint16 fifo_id = 0;
  t_uint16 source_id;
  t_uint16 core_id;
  t_uint32 size_released;
  bool fifo_ok = false;

  source_id = transaction.get_source_id();
  core_id = transaction.get_core_id();
  size_released = transaction.get_data();

  int j=0;
  for(int i=0; i<nb_fifo_in; i++) {
    if(core_fifo_in[i]==core_id) {
      if(j==source_id) {
        fifo_id = i;
        fifo_ok=true;
      }
      j++;
    }
  }

  if(fifo_ok) {
    EXPRINT(ni, 1,"Receive released ("<< size_released  <<") from fifo: " << fifo_id);
//    tab_available_size_fifo_in[fifo_id]+=size_released;
  } else {
    EXPRINT(ni, 0,"ERROR : invalid source id " << source_id << " for released from CORE " << core_id);
    exit(0);
  }
}

/*******************************************************************************/
/*******************************************************************************/
/// Receive AVAILABLE transaction (remote call)
void anoc_copro_wrapper::put(const ni_available_transaction &transaction) {
#ifdef _ANOC_COPRO_DEBUG
  printf("AVAILABLE transaction\n");
#endif
  t_uint16 fifo_id = 0;
  t_uint16 source_id;
  t_uint16 core_id;
  t_uint32 available_data;
  bool fifo_ok = false;

  source_id = transaction.get_source_id();
  core_id = transaction.get_core_id();
  available_data = transaction.get_data();

  int j=0;
  for(int i=0; i<nb_fifo_out; i++) {
    if(core_fifo_out[i]==core_id) {
      if(j==source_id) {
        fifo_id = i;
        fifo_ok=true;
      }
      j++;
    }
  }
  if(fifo_ok) {
    EXPRINT(ni, 1,"Receive available (" << available_data << ") from fifo: " << fifo_id);
    tab_available_data_fifo_out[fifo_id] = available_data;
  } else {
    EXPRINT(ni, 0,"ERROR : invalid source id " << source_id << " for avaivable from CORE " << core_id);
    exit(0);
  }
}

/*******************************************************************************/
/*******************************************************************************/
/// Receive WRITE DATA transaction (remote call)
void anoc_copro_wrapper::put(const ni_write_data_transaction &transaction) {
#ifdef _ANOC_COPRO_DEBUG
  printf("Received DATA transaction\n");
#endif

  t_uint16 fifo_id = 0;
  t_uint16 source_id;
  t_uint16 core_id;
  t_uint32 data;
  bool fifo_ok = false;

  source_id = transaction.get_source_id();
  core_id = transaction.get_core_id();
  data = transaction.get_data();

  int j=0;
  for(int i=0; i<nb_fifo_out; i++) {
    if(core_fifo_out[i]==core_id) {
      if(j==source_id) {
        fifo_id = i;
        fifo_ok=true;
      }
      j++;
    }
  }

  if(fifo_ok) {
    EXPRINT(ni, 1,"Receive write data: " << data << " from fifo: " << fifo_id);
    tab_out_data_fifo_out[fifo_id] = data;
    flag_tab_out_data_fifo_out[fifo_id] = true;
    //Requires a data update on the RTL ports
    write_data_update = true;
  } else {
    EXPRINT(ni, 0,"ERROR : invalid source id " << source_id << " for write data from CORE " << core_id);
    exit(0);
  }

}

/*******************************************************************************/
/*******************************************************************************/
/// Write DATA ACCEPT transaction for fifo out
void anoc_copro_wrapper::write_data_accept_fifo_out(int num_fifo) {
#ifdef _ANOC_COPRO_DEBUG
  printf("DATA ACCEPT transaction\n");
#endif
  ni_accept_data_transaction accept_trans;
  ni_output_fifo_in[num_fifo]->put(&accept_trans);
}


/*******************************************************************************/
/*******************************************************************************/
/// Receive ACCEPT DATA transaction (remote call)
void anoc_copro_wrapper::put(const ni_accept_data_transaction &transaction) {
#ifdef _ANOC_COPRO_DEBUG
  printf("ACCEPT DATA transaction\n");
#endif
  t_uint16 source_id;
  t_uint16 fifo_id = 0;
  t_uint16 core_id;
  bool fifo_ok=false;

  source_id = transaction.get_source_id();
  core_id = transaction.get_core_id();

  int j=0;
  for(int i=0; i<nb_fifo_in; i++) {
    if(core_fifo_in[i]==core_id) {
      if(j==source_id) {
        fifo_id = i;
        fifo_ok=true;
      }
      j++;
    }
  }

  if(fifo_ok) {
    EXPRINT(ni, 1,"Receive accept data from fifo: " << fifo_id); //FIXPRINT
    tab_fifo_in_accept[fifo_id] = true;
    //Requires to update the accept_data (Ivan) 
    accept_data_update = true;
  } else {
    EXPRINT(ni, 0,"ERROR : invalid source id " << source_id << " for accept data from CORE " << core_id );
    exit(0);
  }

}

/*******************************************************************************/
/*******************************************************************************/
/// Write WRITE DATA transaction for fifo in
void anoc_copro_wrapper::write_data_fifo_in(int num_fifo, t_uint32 data) {
#ifdef _ANOC_COPRO_DEBUG
  printf("WRITE DATA transaction\n");
#endif

  ni_write_data_transaction data_trans;
  data_trans.set_data(data);
  EXPRINT(ni, 1,"Write data " << data << " in fifo_in[" << num_fifo << "]"); //FIXPRINT
  ni_input_fifo_out[num_fifo]->put(&data_trans);

}

/*******************************************************************************/
/*******************************************************************************/
/// Receive STATUS transaction (remote call)
void anoc_copro_wrapper::put(const ni_status_transaction &transaction) {
#ifdef _ANOC_COPRO_DEBUG
  printf("STATUS transaction\n");
#endif

  t_uint16 core_id;
  core_id = transaction.get_core_id();
  if(core_id>=nb_cores) {
    EXPRINT(ni, 0,"ERROR : Invalid core id for status");
    exit(0);
  }

  last_status[core_id] = transaction.get_status();
  //Requires to update the status port
  status_update = true;

  EXPRINT(ni, 0,"New status from CORE " << core_id  <<" : " << last_status[core_id]); //FIXPRINT

}

/*******************************************************************************/
/*******************************************************************************/
/// Receive EOC transaction (remote call)
void anoc_copro_wrapper::put(const ni_eoc_transaction &transaction) {
#ifdef _ANOC_COPRO_DEBUG
  printf("EOC transaction\n");
#endif

  t_uint16 core_id;
  core_id = transaction.get_core_id();
  if(core_id>=nb_cores) {
    EXPRINT(ni, 0,"ERROR : Invalid core_id for eoc");
    exit(0);
  }

  EXPRINT(ni, 0,"CORE " << core_id << " has finished"); //FIXPRINT
  cfg_flag_core_finished[core_id] = true;
  //Requires to update the eoc
  eoc_update = true;

}

/*******************************************************************************/
/*******************************************************************************/
// Update the RTL signals when a TLM transcation arrive 
/// Sensitive_neg << p_clk
void anoc_copro_wrapper::tlm_to_rtl() {
  //Output FIFOs
  //////////////
  // The Data port need to be updated ?
//  if (write_data_update == true) {
//    printf("write_data_update, %i\n",(int)flag_tab_out_data_fifo_out[0]);
//    for(int i=0; i<nb_fifo_out; i++) {
//      p_to_MWMR[i].w    = flag_tab_out_data_fifo_out[i];
//      p_to_MWMR[i].data =      tab_out_data_fifo_out[i];
//    }
//    //the Data is correctly updated
//    write_data_update = false;
//  }
  //Input FIFOs
  /////////////
  // The Accept port need to be updated ?
//  if (accept_data_update == true) {
//    for(int i=0; i<nb_fifo_in; i++) {
//      p_from_MWMR[i].r = tab_fifo_in_accept[i];
//    }
//    //the Data is correctly updated
//    accept_data_update = false;
//  }
  //Status port
  /////////////
  // The Accept port need to be updated ?
  if (status_update == true) {
    for (int i=0; i<nb_cores; i++) {
      p_status[i+1] = last_status[i];
    }
    status_update = false;
  }
  //EOC port
  /////////////
  if (eoc_update == true) {
    uint32_t value = 0;
    for (int i=0; i<nb_cores; i++) {
      if (cfg_flag_core_finished[i] == true) {
        value += 1 << i;
      }
    }
    p_status[0] = value;
    eoc_update = false;
  }

  //Core configuration
  ////////////////////
  p_core_config.r = true;

  //Start the rtl_to_tlm process
  rtl_to_tlm_event.notify(clk_period/4,SC_PS);
}


/*******************************************************************************/
/*******************************************************************************/
// Generate a TLM transaction when the RTL signals change
/// Sensitive_pos << 3/4*p_clk
void anoc_copro_wrapper::rtl_to_tlm() {

  //Output FIFOs
  //////////////
  //Test if a Send signal is active
  for(int i=0; i<nb_fifo_out; i++) {
    //A FIFO was ready to send data ?
    p_to_MWMR[i].w    = false;
    if (flag_tab_out_data_fifo_out[i]) {
      //Chech if the accept signal is High
      if (p_to_MWMR[i].wok.read() == true) {
        //The FIFO data is correcly transfered
        p_to_MWMR[i].w    = true;
        p_to_MWMR[i].data = tab_out_data_fifo_out[i];

        //Invalidate the send signal
        flag_tab_out_data_fifo_out[i] = false;
        //Requires to update the Data port
        write_data_update = true;
        //Send the TLM Accept transaction to the Core
        if (tab_available_data_fifo_out[i] > 1 )
          write_data_accept_fifo_out(i);
      }
    } else if (tab_available_data_fifo_out[i] > 0 ) {
      //New data can be read?
      //Send the TLM Accept transaction to the Core
      write_data_accept_fifo_out(i);
    }
  }

  //Input FIFOs
  /////////////
  //Test if a new Data is present on the Input FIFOs
  for(int i=0; i<nb_fifo_in; i++) {
    p_from_MWMR[i].r = tab_fifo_in_accept[i];
    if (p_from_MWMR[i].rok.read() == true) {
      //I am ready to accept this new data?
      if (tab_fifo_in_accept[i] == true) {
        //Read the data and send a TLM transcation
        p_from_MWMR[i].r = true;
        t_uint32 data = (t_uint32)p_from_MWMR[i].data.read();
        write_data_fifo_in(i, data);
        //Lower the ROK signal
        tab_fifo_in_accept[i] = false;
        //Requires a Accept RTL port update
        accept_data_update = true;
      }
    }
  }

  //Exec port
  ///////////////
  //RTL signals changed?
  if (exec_last != p_config[0].read()) {
    ni_exec_transaction exec_trans;
    //Test bit by bit to identify the source
    for(int i=0; i<nb_cores; i++) {
      if (((exec_last & (1<<i)) == 0) && ((p_config[0].read() & (1<<i)) == 1)) {
        //A new lot_id is ready to be executed on core 'i'
        EXPRINT(ni, 1,"CORE configuration event");
        //Get the slot_id
        t_uint16 slot_id = (t_uint16)p_config[i+1].read();
	EXPRINT(ni, 0,"CORE " << i << " execute configuration on slot id " << slot_id);
	// write exec config
	exec_trans.set_slot_id(slot_id);
	ni_exec_out[i]->put(&exec_trans);
        //The CORE is nolonger finished
        cfg_flag_core_finished[i] = false;
        //Requires a RTL signal update
        eoc_update = true;
      }
    }
    exec_last = p_config[0].read();
  }

  //Core config address
  /////////////////////
  if (last_address != p_config[5].read()) {
    //The address have been changed
    current_address = p_config[5].read();
    last_address = p_config[5].read();
  }

        
  //Core configuration
  ////////////////////
  if (p_core_config.rok.read() == true) {
    //A new configuration data is ready
    ni_cfg_transaction cfg_trans;
    t_uint32 data = p_core_config.data.read();
    cfg_trans.set_data(data);
    cfg_trans.set_address(current_address);
#ifdef _ANOC_COPRO_DEBUG
    printf("Ivan: Config data: %.8x on address %.8x\n", data, current_address);
#endif
    ni_cfg_dump_out->put(&cfg_trans);
    current_address++;
  }

}
}}


