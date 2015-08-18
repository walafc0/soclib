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

#ifndef _ANOC_INIT_PORT_H_
#define _ANOC_INIT_PORT_H_

/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include "anoc_common.h"
#include "anoc_transaction.h"
#include "anoc_basic_port.h"
#include "anoc_target_port.h"
#ifdef TLM_POWER_ESTIMATION
#include "tlm_power_link.h"
using namespace Power;
//#include <string>
#endif// TLM_POWER_ESTIMATION

/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/
// #define VERBOSE



//----------------------------------------------------------------------------
///  CLASS : anoc_init_port
///
///  ANOC init port class
///  
/**
   This class models the initiator-side communication port built on the top of the
   tlm_blocking_put_if interface, with support of the ANOC protocol. An anoc_init_port
   object must be instantiated by a module and bound to an interface compatible with
   the ANOC protocol to offer ANOC communication capabilities to this module.
   One interface maximum can be bound to this port.

   It inherits from tlm_initiator_port. 

   It provides the write() user function.

**/
//----------------------------------------------------------------------------

template<class TRANSACTION>
class anoc_init_port
  : public anoc_basic_port,
    public tlm_initiator_port<tlm_blocking_put_if<TRANSACTION>, 1> 
{
  SC_HAS_PROCESS(anoc_init_port<anoc_data_transaction>);

  /// transaction container
  TRANSACTION m_transaction;

 public:
  
  //--------------
  /// constructor
  /**
     An instance of TRANSACTION is automatically built by the constructor
     to avoid the dynamic creation of such an object each time a transaction is
     generated, to improve the simulation performance.

   **/
  //--------------
  
#ifdef TLM_TRANS_RECORD  
	anoc_init_port(const char *port_name, anoc_transrecord *transrecord) :
	  anoc_basic_port(transrecord),
	  tlm_initiator_port<tlm_blocking_put_if<TRANSACTION>, 1>(port_name), 
	  m_transaction()
	  {
	  }
#else
	anoc_init_port(const char *port_name) : 
	  anoc_basic_port(),
	  tlm_initiator_port<tlm_blocking_put_if<TRANSACTION>, 1>(port_name),
	  m_transaction()
	  {
	  }
#endif // TLM_TRANS_RECORD 

	
  //----------------------------
  // sc_port method overload
  //----------------------------
  // Called by elaboration_done 
	void end_of_elaboration() {
	  // Maximum number of channels (tlm interfaces) that can be bound test
	  if (this->m_initiator_port_list.size() > 1) {
		// Uses SystemC SC_REPORT_ERROR
		SC_REPORT_ERROR( SC_ID_BIND_IF_TO_PORT_, "maximum reached" );
	  }
#ifdef VERBOSE
	  printf("\t%s: %s Supported protocol: \"%s\"\n",
			 name(),
			 "Initiator -",
			 ANOC_SUPPORTED_PROTOCOL
			 );
#endif
	}


	// ***********************************
	// User interface
	// ***********************************

	// -----------------------------------------------------------------
	// write transaction through channel.
	inline void write(TRANSACTION& transaction) {
	  m_transaction = transaction;
	  if (this->get_target_port_list().size() != 1) // One target should be connected to this port
		ERROR("should never occur !!!");
#if MAGALI_PLATFORM == MAGALI_PLATFORM_SIMU_V2
	  m_transaction.set_target_port_id(this->get_target_port_list()[0]->get_port_id());
#else
	  m_transaction.set_target_port_id(this->get_target_port_list()[0]->get_tlm_export_id());	  
#endif

#ifdef TLM_TRANS_RECORD  
	  record(m_transaction);
	  static_cast< anoc_target_port< TRANSACTION >* >(this->get_target_port_list()[0])->record(m_transaction);
#endif // TLM_TRANS_RECORD
	  (*this)->put(m_transaction);
	}
    
};


// here is a specialization for a initiator port of a data transaction
// in order to estimate power for links.
// accpet transaction are ignored
#ifdef TLM_POWER_ESTIMATION	
template <>
class anoc_init_port<anoc_data_transaction>
  : public anoc_basic_port,
    public tlm_initiator_port<tlm_blocking_put_if<anoc_data_transaction>, 1>,
		public tlm_power_link
{

  //SC_HAS_PROCESS(static_cast<sc_core::sc_module>(anoc_init_port<anoc_data_transaction>));
  //SC_HAS_PROCESS(anoc_init_port<anoc_data_transaction>);
  SC_HAS_PROCESS(anoc_init_port);
  /// transaction container
  anoc_data_transaction m_transaction;

 private:
  sc_event link_on_evt;
  void link_on(){
		new_power_mode(tlm_power_link::OFF);
  }

 public:
  
  //--------------
  /// constructor
  /**
     An instance of TRANSACTION is automatically built by the constructor
     to avoid the dynamic creation of such an object each time a transaction is
     generated, to improve the simulation performance.

   **/
  //--------------
  
#ifdef TLM_TRANS_RECORD  
	anoc_init_port(const char *port_name, anoc_transrecord *transrecord) :
	  anoc_basic_port(transrecord),
    tlm_initiator_port<tlm_blocking_put_if<anoc_data_transaction>, 1>(port_name),
		tlm_power_link((std::string) port_name), 
	  m_transaction()
		{
			sc_spawn_options opt;
			opt.spawn_method();
			opt.set_sensitivity(&link_on_evt);
			opt.dont_initialize();
      sc_spawn(sc_bind(&anoc_init_port::link_on, this),sc_gen_unique_name("link_on"), &opt);
	  }
#else
	anoc_init_port(const char *port_name) : 
	  anoc_basic_port(),
	  tlm_initiator_port<tlm_blocking_put_if<anoc_data_transaction>, 1>(port_name),
	  tlm_power_link((std::string) port_name),
		m_transaction()
	  {
			sc_spawn_options opt;
			opt.spawn_method();
			opt.set_sensitivity(&link_on_evt);
			opt.dont_initialize();
      sc_spawn(sc_bind(&anoc_init_port::link_on, this),sc_gen_unique_name("link_on"), &opt);
	  }
#endif // TLM_TRANS_RECORD 

	
  //----------------------------
  // sc_port method overload
  //----------------------------
  // Called by elaboration_done 
	void end_of_elaboration() {
	  // Maximum number of channels (tlm interfaces) that can be bound test
	  if (this->m_initiator_port_list.size() > 1) {
		// Uses SystemC SC_REPORT_ERROR
		SC_REPORT_ERROR( SC_ID_BIND_IF_TO_PORT_, "maximum reached" );
	  }
#ifdef VERBOSE
	  printf("\t%s: %s Supported protocol: \"%s\"\n",
			 name(),
			 "Initiator -",
			 ANOC_SUPPORTED_PROTOCOL
			 );
#endif
	}


	// ***********************************
	// User interface
	// ***********************************

	// -----------------------------------------------------------------
	// write DATA transaction through channel.
	inline void write(anoc_data_transaction& transaction) {
		new_power_mode(tlm_power_link::ON);
    link_on_evt.notify(1,SC_NS);

	  m_transaction = transaction;
	  if (this->get_target_port_list().size() != 1) // One target should be connected to this port
		ERROR("should never occur !!!");
#if MAGALI_PLATFORM == MAGALI_PLATFORM_SIMU_V2
	  m_transaction.set_target_port_id(this->get_target_port_list()[0]->get_port_id());
#else
	  m_transaction.set_target_port_id(this->get_target_port_list()[0]->get_tlm_export_id());
#endif

#ifdef TLM_TRANS_RECORD  
	  record(m_transaction);
	  static_cast< anoc_target_port< anoc_data_transaction >* >(this->get_target_port_list()[0])->record(m_transaction);
#endif // TLM_TRANS_RECORD
//
//
	  (*this)->put(m_transaction);
	}

};
#endif // TLM_POWER_ESTIMATION

#ifdef VERBOSE
#undef VERBOSE
#endif 

#endif /* _ANOC_INIT_PORT_H_ */
