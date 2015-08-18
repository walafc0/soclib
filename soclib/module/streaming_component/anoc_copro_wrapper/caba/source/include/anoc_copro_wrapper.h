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

#ifndef _ANOC_COPRO_WRAPPER_H
#define _ANOC_COPRO_WRAPPER_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "ni_port.h"
#include "fifo_ports.h"
#include "alloc_elems.h"

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/

//-----------------------------------------
// NI parameters

#define NI_MAX_CREDIT_SIZE 127 // 7bits

#define NI_MAX_NB_FIFO_IN  4
#define NI_MAX_NB_FIFO_OUT 4
#define NI_MAX_NB_TASK 2

#define NI_FIFO_SIZE_REG_SIZE 16

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
 * CLASS: anoc_copro_wrapper
 *
 *   This class implements the anoc_copro_wrapper
 *----------------------------------------------------------------------------*/
namespace soclib { namespace caba {
using namespace sc_core;

class anoc_copro_wrapper :
  public sc_module,
//  public tlm_module,
  public virtual tlm_blocking_put_if<ni_dump_data_transaction>,
  public virtual tlm_blocking_put_if<ni_available_transaction>,
  public virtual tlm_blocking_put_if<ni_released_transaction>,
  public virtual tlm_blocking_put_if<ni_write_data_transaction>,
  public virtual tlm_blocking_put_if<ni_accept_data_transaction>,
  public virtual tlm_blocking_put_if<ni_status_transaction>,
  public virtual tlm_blocking_put_if<ni_eoc_transaction>
{
/*------------------------------------------------------------------------------
 * Parameters
 *----------------------------------------------------------------------------*/
  public: // ports

    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    FifoInput<uint32_t>  *p_from_MWMR;
    FifoOutput<uint32_t> *p_to_MWMR;
    FifoInput<uint32_t>   p_core_config;
    sc_core::sc_in<uint32_t>  *p_config;
    sc_core::sc_out<uint32_t> *p_status;

    // ni => core ports
    ni_exec_out_port **ni_exec_out; // @[nb_cores]
    ni_cfg_dump_out_port *ni_cfg_dump_out;
    ni_data_out_port **ni_input_fifo_out; //@[nb_fifo_in]

    // core => ni ports
    ni_data_in_port **ni_output_fifo_in; //@[nb_fifo_out]
    ni_released_in_port **ni_released_in; //@[nb_fifo_in]
    ni_available_in_port **ni_available_in; //@[nb_fifo_out]
    ni_status_in_port **ni_status_in;



  /***********************************************/
  private: // resource parameters

    // resource name 
    string res_name;


  /***********************************************/
  public: // structural info

    // cores
    t_uint16 nb_cores;

    // fifos in
    t_uint16 nb_fifo_in;
    t_uint16 *core_fifo_in; // @[nb_fifo_in]

    // fifos out
    t_uint16 nb_fifo_out;
    t_uint16 *core_fifo_out; // @[nb_fifo_out]

    t_uint32 clk_period;

  /***********************************************/
  private: // internal state variables

    // CORE => NI interface (update with transaction)
    t_uint32 *last_status; // @[nb_cores] // last status value
    // FIFO out
    t_uint32 *tab_out_data_fifo_out; //@[nb_fifo_out] // data from output fifo
    bool* flag_tab_out_data_fifo_out; //@[nb_fifo_out] // flag for new data in tab_out_data_fifo_out
    t_uint32 *tab_available_data_fifo_out; //@[nb_fifo_out] // available data in output fifos
    // FIFO in
    bool *tab_fifo_in_accept; //@[nb_fifo_in] // accept from core. FIXME: not fully implemented

    bool *cfg_flag_core_finished; // @[nb_cores] // true if current core config is finished

    bool write_data_update;
    bool accept_data_update;
    bool status_update;
    bool eoc_update;
    uint32_t exec_last;
    uint32_t last_address;
    uint32_t current_address;

  private:
    // internal events
    sc_event rtl_to_tlm_event;

/*------------------------------------------------------------------------------
 * Methods
 *----------------------------------------------------------------------------*/

  public:

    //------------------------------------------------------------
    // put method implementations: slave part (receive) of the ni
    //------------------------------------------------------------

    // NoC => NI

    // Core (or Wrapper) => NI

    // Receive STATUS transaction (remote call)
    virtual void put(const ni_status_transaction &transaction);

    // Receive EOC transaction (remote call)
    virtual void put(const ni_eoc_transaction &transaction);

    // Receive DUMP DATA transaction (remote call)
    virtual void put(const ni_dump_data_transaction &transaction);

    // Receive ACCEPT DATA transaction (remote call from CORE FIFO IN)
    virtual void put(const ni_accept_data_transaction &transaction);

    // Receive WRITE DATA transaction (remote call from CORE FIFO OUT)
    virtual void put(const ni_write_data_transaction &transaction);

    // Receive RELEASED transaction (remote call)
    virtual void put(const ni_released_transaction &transaction);

    // Receive AVAILABLE transaction (remote call)
    virtual void put(const ni_available_transaction &transaction);

    // Write DATA ACCEPT transaction for fifo out
    virtual void write_data_accept_fifo_out(int num_fifo);

    // Write WRITE DATA transaction for fifo in
    virtual void write_data_fifo_in(int num_fifo, t_uint32 data);

     //------------------------------------------------------------
    // internal sc_methods
    //------------------------------------------------------------  
    void tlm_to_rtl();
    void rtl_to_tlm();
 
    // Constructor 
    SC_HAS_PROCESS(anoc_copro_wrapper);
    anoc_copro_wrapper(sc_module_name module_name_,
                 t_uint16 nb_cores_,
                 t_uint16 nb_fifo_in_,
                 t_uint16 nb_fifo_out_,
                 t_uint16 n_config_,
                 t_uint16 n_status_,
                 t_uint32 clk_period_
                 );

    // Destructor
    virtual ~anoc_copro_wrapper();


};
}}

#endif /* _ANOC_COPRO_WRAPPER_H */
