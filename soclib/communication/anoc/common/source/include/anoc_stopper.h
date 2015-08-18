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

#ifndef _ANOC_STOPPER_H_
#define _ANOC_STOPPER_H_

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "anoc_common.h"
#include "anoc_transaction.h"
//#include "tlm_module.h"
#include "anoc_in_port.h"
#include "anoc_out_port.h"
#include "res_path.h"

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/

//#define VERBOSE

typedef enum {NO_TRACE_STOPPER, ALL_TRACE_STOPPER, DATA_TRACE_STOPPER} trace_level;

/*------------------------------------------------------------------------------
 * CLASS: anoc_stopper
 *
 *   This class instanciates the core and the NI of the resource
 *----------------------------------------------------------------------------*/
class anoc_stopper :
  public sc_module,
 // public tlm_module,
  public virtual tlm_blocking_put_if<anoc_data_transaction>,
  public virtual tlm_blocking_put_if<anoc_accept_transaction>
{
/*------------------------------------------------------------------------------
 * Parameters
 *----------------------------------------------------------------------------*/
  public: // resource ports

    // noc input & output ports
    anoc_in_port*  noc_in;
    anoc_out_port* noc_out;

  /***********************************************/
  protected: // resource parameters

    // resource id
    int res_id;

    // clock period
    int clk_period;

    // node wait
    int node_wait;

    // trace file variables
    trace_level level;
    std::ofstream *stopper_data_stream;
    bool current_packet_is_data;

  /***********************************************/
  private: //  parameters

    // event
    sc_event write_accept_event[ANOC_NB_CHANNEL];



/*------------------------------------------------------------------------------
 * Methods
 *----------------------------------------------------------------------------*/

  public:
    // Print basic information on resource creation
    virtual void print_resource_info(const char *description) const;

    // Receive DATA transaction (remote call)
    virtual void put(const anoc_data_transaction& transaction);

    // Receive ACCEPT transaction (remote call)
    virtual void put(const anoc_accept_transaction& transaction);

  private:

    // Write ACCEPT transaction on channel 0 (sc_method)
    virtual void write_accept_0();

    // Write ACCEPT transaction on channel 1 (sc_method)
    virtual void write_accept_1();

    // Trace stream file methods
    virtual std::string get_file_name();
    virtual void create_stream();

  /***********************************************/
  public:
    // Constructor
    SC_HAS_PROCESS(anoc_stopper);
    anoc_stopper(sc_module_name module_name_,
                  int res_id_,
                  int clk_period_, int node_wait_,
                  trace_level level_=NO_TRACE_STOPPER);

    // Destructor
    virtual ~anoc_stopper();

};

#ifdef VERBOSE
#undef VERBOSE
#endif

#endif /* _ANOC_STOPPER_H_ */
