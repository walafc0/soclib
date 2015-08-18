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
 * Comment : Core base class (TLM interface)
 *
 */


#ifndef _CORE_TLM_H_
#define _CORE_TLM_H_

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "anoc_common.h"
#include "core_sysc.h"
#include "ni_port.h"
#include "res_path.h"
#include "ni_utils.h"
//#include "anoc_config_core.h"
#include "res_write_addr_map.h"

/*------------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------------*/
//#define VERBOSE

/*------------------------------------------------------------------------------
 * CLASS: core_tlm
 *
 *   
 *----------------------------------------------------------------------------*/

class core_tlm :
	public core_sysc,
  public virtual tlm_blocking_put_if<ni_cfg_dump_transaction>,
  public virtual tlm_blocking_put_if<ni_write_data_transaction>,
  public virtual tlm_blocking_put_if<ni_accept_data_transaction>,
  public virtual tlm_blocking_put_if<ni_exec_transaction>
{
/*------------------------------------------------------------------------------
 * Parameters
 *----------------------------------------------------------------------------*/

  /***********************************************/
  protected: // resource parameters


  /***********************************************/
  protected: // structural info


  /***********************************************/
  protected: // structure


  /***********************************************/
  protected: // internal programmation


  /***********************************************/
  protected: // internal parameters


 /***********************************************/
  protected: // internal state variables

    // synchronization events
    //sc_event load_cfg_event;


  /***********************************************/
  public: // ports 

/*------------------------------------------------------------------------------
 * Methods
 *----------------------------------------------------------------------------*/
		
    //--------------------------------------------------------------
    // put method implementations : slave part (receive) of the core
    //--------------------------------------------------------------

    // Receive EXEC transaction (remote call)
    virtual void put(const ni_exec_transaction &transaction);

    // Receive CFG DUMP transaction (use forbidden!)
    virtual void put(const ni_cfg_dump_transaction &transaction);

    // Receive CFG transaction (remote call)
    virtual void put(const ni_cfg_transaction &transaction);

    // Receive DUMP transaction (remote call)
    virtual void put(const ni_dump_transaction &transaction);

    // Receive WRITE DATA transaction (remote call)
    virtual void put(const ni_write_data_transaction &transaction);

    // Receive ACCEPT DATA transaction (remote call)
    virtual void put(const ni_accept_data_transaction &transaction);


  /***********************************************/
  protected: // synchronisation events

    sc_event fifo_out_event;
    sc_event dump_data_event;
    sc_event available_out_event;
    sc_event fifo_in_released_event;
    sc_event accept_fifo_in_event;

  /***********************************************/
  private: // internal parameters

    t_uint32 dump_data;

    // accept table for output fifo
    bool *tab_output_fifo_accept;

    // accept table for input fifo
    bool *tab_input_fifo_accept; //@[nb_fifo_in]
    bool *bool_write_input_fifo_accept; //@[nb_fifo_in]

/*------------------------------------------------------------------------------
 * Methods
 *----------------------------------------------------------------------------*/

  public:

    // FIFO IN access functions
    virtual t_uint32 read_fifo_in(int num_fifo);
    virtual bool nb_write_fifo_in(int num_fifo, t_uint32 data_tw);

    // FIFO OUT access functions
    virtual bool nb_read_fifo_out(int num_fifo, t_uint32 &data_tr);
    virtual void write_fifo_out(int num_fifo, t_uint32 data_tw);


    //
    virtual void reset_status();

  public: 
    //------------------------------------------------------------
    // Core to TLM interface
    //------------------------------------------------------------

    // Put DUMP_DATA transaction on cfg_dump_port (sc_method)
    virtual void put_dump_data();

    // Put WRITE_DATA transaction on fifo out (sc_method)
    virtual void put_data_out();

    // Put RELEASED transaction (sc_method)
    virtual void put_released_out();

    // Put AVAILABLE transaction on available_out_port (sc_method)
    virtual void put_available_out();

    // Write DATA_ACCEPT transaction on fifo_in (sc_method)
    virtual void put_accept_fifo_in();

    // Write EOC transaction on exec port
    virtual void write_eoc();

    // Write STATUS transaction on status port
    virtual void write_status(t_uint16 status_);

    

    //------------------------------------------------------------
    // Main thread
    //------------------------------------------------------------

    // Compute (sc_thread) wait for exec and launch computation
    //TODO : pure virtual or not ? could be overriden in derived class, with a default behaviour as consumer
    virtual void compute() = 0;

    // Constructor 
    SC_HAS_PROCESS(core_tlm);
    core_tlm(sc_module_name module_name_,
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
	     string sno_name_=""
			 );
    
    // Destructor
    virtual ~core_tlm();

    // Initialize configurations to load
    //virtual void init_config_to_load();

    //
    virtual void core_put_if_bind();

    // Address map
    virtual void write_register(t_uint32 addr, t_uint32 data);
    virtual t_uint32 read_register(t_uint32 addr);

};

#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _CORE_TLM_H_ */

