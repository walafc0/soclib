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

#ifndef _ANOC_OUT_PORT_H_
#define _ANOC_OUT_PORT_H_


/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include "anoc_common.h"
#include "anoc_init_port.h"
#include "anoc_target_port.h"
#include "anoc_in_port.h"


/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/
//#define VERBOSE



//---------------------------------------------------------------------------- 
// CLASS : anoc_out_port
//
// A wrapper to encapsulate in a single port interface the two ANOC ports  :
//  - data of type anoc_init_port
//  - accept of type anoc_target_port,
// 
// This is just a facility to ease the binding at the netlist level
// => the binding at port level must be done like : anoc_out_port.bind(anoc_in_port)
// => this port must always be binded to the module containing it (use slave_bind() method).
//
// This port contains the transrecord facility. This object is instanciated in this class,
// in order to be shared by data port and accept port in order to begin record and end record separately
//---------------------------------------------------------------------------- 

class anoc_out_port {

public:

  // port fields 
  anoc_init_port<anoc_data_transaction>      *data_port;
  anoc_target_port<anoc_accept_transaction>  *accept_port;


#ifdef TLM_TRANS_RECORD  
  // record database management
  anoc_transrecord *transrecord;
#endif // TLM_TRANS_RECORD 
  

public:

  //----------------------
  // constructor
  //----------------------
#ifdef TLM_TRANS_RECORD  
  anoc_out_port(std::string port_name_, const char *transrecord_name_) {
    transrecord = new anoc_transrecord(transrecord_name_);
    data_port   = new anoc_init_port<anoc_data_transaction>((port_name_ + "_data").c_str(),
							    transrecord);
    accept_port = new anoc_target_port<anoc_accept_transaction>((port_name_ + "_accept").c_str(),
								transrecord);
  }
#else
  anoc_out_port(std::string port_name_) {
    data_port   = new anoc_init_port<anoc_data_transaction>((port_name_ + "_data").c_str());
    accept_port = new anoc_target_port<anoc_accept_transaction>((port_name_ + "_accept").c_str());
  }
#endif // TLM_TRANS_RECORD


  //----------------------
  // destructor
  //----------------------
  virtual ~anoc_out_port() {
#ifdef TLM_TRANS_RECORD  
    delete transrecord;
#endif // TLM_TRANS_RECORD
    delete data_port;
    delete accept_port;
  }


  //----------------------
  // port to port binding (always master module to slave module)
  //----------------------
  void bind (anoc_in_port *target_port) {
    data_port->bind(*target_port->data_port);
    target_port->accept_port->bind(*accept_port);
  }

  //----------------------
  // FIXME hierachical port to port binding (src module to dst module)
  //----------------------
  void hbind(anoc_out_port *out_port) {
    data_port->bind(*out_port->data_port);
    accept_port->bind(*this->accept_port);
  }


  //----------------------
  // Used to bind the target (accept) port to the slave module 
  // (as it implements the tlm_blocking_put_if interface)
  //----------------------
  inline void slave_bind(tlm_blocking_put_if<anoc_accept_transaction>& iface) {
    accept_port->bind(iface);
  }

};

#endif /* _ANOC_OUT_PORT_H_ */
