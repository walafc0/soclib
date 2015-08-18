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

#ifndef _ANOC_TARGET_PORT_H_
#define _ANOC_TARGET_PORT_H_

/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include "anoc_common.h"
#include "anoc_basic_port.h"


/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/
// #define VERBOSE


//----------------------------------------------------------------------------
///  CLASS : anoc_target_port
///
///  ANOC target port class
///  
/**
   This class models a target port based on the ANOC protocol. 
   An object of this class must be instantiated in a anoc target module to enable 
   the reception of transactions from the interconnect
   This can be bound to several tlm_blocking_put_if interface without limitation of number.
???? => Apparently not, see end_of_elaboration()

   It inherits from anoc_basic_port for all the features shared by all the ports. 
**/   
   
//----------------------------------------------------------------------------

template<class TRANSACTION>
class anoc_target_port
  : public anoc_basic_port,
    public tlm_target_port<tlm_blocking_put_if<TRANSACTION> > {

public:

  //--------------
  /// constructor
  //--------------
#ifdef TLM_TRANS_RECORD
  anoc_target_port(const char *port_name, anoc_transrecord *transrecord) : 
    anoc_basic_port(transrecord),
    tlm_target_port<tlm_blocking_put_if<TRANSACTION> >(port_name)
  {
  }
#else
  anoc_target_port(const char * port_name) : 
    anoc_basic_port(),
    tlm_target_port<tlm_blocking_put_if<TRANSACTION> >(port_name)
  {
  }
#endif // TLM_TRANS_RECORD


  //----------------------------
  // sc_port method overload
  //----------------------------
  /// Called by elaboration_done 
  void end_of_elaboration() {
    // Maximum number of channels (tlm interfaces) that can be bound test
    if (this->get_target_port_list().size()>1) {
      // Uses SystemC SC_REPORT_ERROR
      SC_REPORT_ERROR( SC_ID_BIND_IF_TO_PORT_, "maximum reached" );
    }
#ifdef VERBOSE
    printf("\t%s: %s Supported protocol: \"%s\" - Port ID: %d\n",
	   name(),
	   "Target -",
	   ANOC_SUPPORTED_PROTOCOL,
	   get_port_id()
	   );
#endif
  }

};

// sc_object kind string property
// const char* const anoc_target_port::kind_string = "anoc_target_port";

#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _ANOC_TARGET_PORT_H_ */
