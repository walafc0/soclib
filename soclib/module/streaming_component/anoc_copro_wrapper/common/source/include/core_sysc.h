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

#ifndef _CORE_SYSC_H_
#define _CORE_SYSC_H_

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "anoc_common.h"
#include "core_base.h"
#include "core_config.h"


/*------------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------------*/
//#define VERBOSE

/*------------------------------------------------------------------------------
 * CLASS: core_sysc
 *
 *
 *----------------------------------------------------------------------------*/
class core_sysc :
  public core_base
{
/*------------------------------------------------------------------------------
 * Parameters                                                             
 *----------------------------------------------------------------------------*/

  /***********************************************/
  protected: // structure

    // core fifos
    sc_fifo<t_uint32> **fifo_in;  // @[nb_fifo_in]
    t_uint16 *size_fifo_in; // @[nb_fifo_in]
    sc_fifo<t_uint32> **fifo_out; // @[nb_fifo_out]
    t_uint16 *size_fifo_out; // @[nb_fifo_out]

  /***********************************************/
  protected: // internal programmation

    // core config table instanciated in child class
    core_config_table *cfg_core; // @[nb_cfg_core]
    t_uint16 nb_cfg_core;
    t_uint32 addr_core_cfg_begin;

  /***********************************************/
  protected: // internal parameters

    // size to released / fifo_in
    t_uint32 *size_to_released; //@[nb_fifo_in]
    // previous num available / fifo_out
    t_uint32 *previous_num_available; //@[nb_fifo_out]


 /***********************************************/
  protected: // internal state variables

    // core
    //core_config *current_core_config;
    //FIXME

    t_uint32 current_slot_id;

    // synchronization events
    sc_event load_cfg_event;
    sc_event write_fifo_in_event;
    sc_event read_fifo_out_event;

    // configuration initialization (used in child class)
    int nb_cfg_core_to_load;
    t_uint16 *num_cfg_core; 

  /***********************************************/
  public: // resource parameters
  
    // static initialization
    bool static_init;
    string sno_name; 

/*------------------------------------------------------------------------------
 * Methods
 *----------------------------------------------------------------------------*/

  public:

    // FIFO IN access functions
    virtual t_uint32 read_fifo_in(int num_fifo) = 0;
    virtual bool nb_write_fifo_in(int num_fifo, t_uint32 data_tw) = 0;
    t_uint16 num_available_fifo_in(int num_fifo);

    // FIFO OUT access functions
    virtual bool nb_read_fifo_out(int num_fifo, t_uint32 &data_tr) = 0;
    virtual void write_fifo_out(int num_fifo, t_uint32 data_tw) = 0;
    t_uint16 num_free_fifo_out(int num_fifo);

    // STATUS and EOC functions
    virtual void write_status(t_uint16 status_) = 0;

  public: 

    //------------------------------------------------------------
    // Main thread
    //------------------------------------------------------------

    // Compute (sc_thread) wait for exec and launch computation
    //TODO : pure virtual or not ? could be overriden in derived class, with a default behaviour as consumer
    virtual void compute() = 0;

    // Constructor 
    SC_HAS_PROCESS(core_sysc);
    core_sysc(sc_module_name module_name_,
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
	      string sno_name_="");

    // Destructor
    virtual ~core_sysc();

    // Initialize configurations to load
    virtual void init_config_to_load();

    //
    //virtual void core_put_if_bind();

    // Address map
    virtual void write_register(t_uint32 addr, t_uint32 data);
    virtual t_uint32 read_register(t_uint32 addr);

};

#endif /* _CORE_SYSC_H_ */

