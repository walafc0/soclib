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

#ifndef _ANOC_BASIC_PORT_H_
#define _ANOC_BASIC_PORT_H_

/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include "anoc_common.h"
#include "tlm.h"
#include "anoc_transaction.h"

#ifdef TLM_TRANS_RECORD  
#include "anoc_transrecord.h"
#endif // TLM_TRANS_RECORD


/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/
#define VERBOSE
#define ANOC_SUPPORTED_PROTOCOL "CEA-LETI/ANOC/1.0"


//----------------------------------------------------------------------------
///  CLASS : anoc_basic_port
///
///  ANOC basic port class
///  
/**
   This class models the common characteristics of the basic communication port
   for the ANOC protocol. An anoc_basic_port-derived object must be instantiated
   by a module and bound to an interface compatible with the ANOC protocol to
   offer ANOC communication capabilities to this module. One interface maximum
   can be bound to this port.
**/
//----------------------------------------------------------------------------

class anoc_basic_port
{
  
protected:
#ifdef TLM_TRANS_RECORD  
  // record database pointer (pointer to transrecord from in/out port containing this init port)
  anoc_transrecord *m_transrecord;
#endif // TLM_TRANS_RECORD 

public:
  
  //--------------
  /// constructor
  //--------------
  
#ifdef TLM_TRANS_RECORD  
  anoc_basic_port(anoc_transrecord *transrecord) :
    m_transrecord(transrecord)
  {
  }
#else
  anoc_basic_port()
  {
  }
#endif // TLM_TRANS_RECORD 

  //------------------

  //-----------------------------------------------------------------
  /// record a transaction.
  inline void record(const anoc_data_transaction& transaction) {
#ifdef TLM_TRANS_RECORD  
    m_transrecord->record(transaction);
#endif // TLM_TRANS_RECORD 
  }
  inline void record(const anoc_accept_transaction& transaction) {
#ifdef TLM_TRANS_RECORD  
    m_transrecord->record(transaction);
#endif // TLM_TRANS_RECORD 
  }

};

#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _ANOC_BASIC_PORT_H_ */
