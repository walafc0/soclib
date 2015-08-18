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
 * Comment : Core base class
 *
 */

#ifndef _CORE_BASE_H_
#define _CORE_BASE_H_

/*------------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------------*/

#include "anoc_common.h"
#include "ni_port.h"
//#include "ni_monocore.h"
#include "res_path.h"
#include "ni_utils.h"
#include "core_config.h"
#include "res_write_addr_map.h"

#ifdef TLM_DVFS_MODELISATION
#include "lp_manager.h"
#endif //TLM_DVFS_MODELISATION

/*------------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------------*/
//#define VERBOSE


//
// Typedefs for ni internal ports
//
typedef ni_beginend_out_port<ni_exec_transaction, ni_eoc_transaction> ni_exec_out_port;
typedef ni_beginend_in_port<ni_exec_transaction, ni_eoc_transaction> ni_exec_in_port;
typedef ni_beginend_out_port<ni_cfg_dump_transaction, ni_dump_data_transaction> ni_cfg_dump_out_port;
typedef ni_beginend_in_port<ni_cfg_dump_transaction, ni_dump_data_transaction> ni_cfg_dump_in_port;
typedef ni_beginend_out_port_id<ni_write_data_transaction, ni_accept_data_transaction> ni_data_out_port;
typedef ni_beginend_in_port_id<ni_write_data_transaction, ni_accept_data_transaction> ni_data_in_port;
typedef ni_event_out_port_id<ni_released_transaction> ni_released_out_port;
typedef ni_event_in_port<ni_released_transaction> ni_released_in_port;
typedef ni_event_out_port_id<ni_available_transaction> ni_available_out_port;
typedef ni_event_in_port<ni_available_transaction> ni_available_in_port;
typedef ni_event_out_port<ni_status_transaction> ni_status_out_port;
typedef ni_event_in_port<ni_status_transaction> ni_status_in_port;


/*------------------------------------------------------------------------------
 * CLASS: core_base
 *
 *   This class implements the ANOC network interface
 *----------------------------------------------------------------------------*/
class core_base :  
	public sc_module
{
/*------------------------------------------------------------------------------
 * Parameters                                                             
 *----------------------------------------------------------------------------*/
  
  /***********************************************/
  public: // resource parameters
  
    // clock period
    int clk_period;

    // multicore parameters
    bool multicore;
    t_uint16 core_id;


  /***********************************************/
  public: // structural info

    // fifos in
    t_uint16 nb_fifo_in;

    //fifos out
    t_uint16 nb_fifo_out;

  /***********************************************/
  public: // ports 

    // ni => core ports
    ni_exec_in_port *ni_exec_in;
    ni_cfg_dump_in_port *ni_cfg_dump_in;
    ni_data_in_port **ni_input_fifo_in; //@[nb_fifo_in];

    // core => ni ports
    ni_data_out_port **ni_output_fifo_out; //@[nb_fifo_out];
    ni_released_out_port **ni_released_out; //@[nb_fifo_in];
    ni_available_out_port **ni_available_out; //@[nb_fifo_out]; 
    ni_status_out_port *ni_status_out;

#ifdef TLM_DVFS_MODELISATION
  public:
    /// pointer to the ressource where this core is instantiated 
    lp_manager* lpm;

    ///  port out for lpm
    sc_out<bool> internal_core_task_synch;
#endif

/*------------------------------------------------------------------------------
 * Methods                                                             
	 *----------------------------------------------------------------------------*/
		
		
    //------------------------------------------------------------
    // Main thread
    //------------------------------------------------------------

    // Constructor 
    SC_HAS_PROCESS(core_base);
    core_base(sc_module_name module_name_,
              int clk_period_,
	      bool multicore_,
	      t_uint16 core_id_,
              t_uint16 nb_fifo_in_,
              t_uint16 nb_fifo_out_);

    // Destructor
    virtual ~core_base();

};

#endif /* _CORE_BASE_H_ */

