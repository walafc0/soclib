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

#ifndef _NI_PORT_H_
#define _NI_PORT_H_


/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include "anoc_common.h"
#include "tlm.h"

#ifdef TLM_TRANS_RECORD
#include "ni_transrecord_beginend.h"
#include "ni_transrecord_event.h"
#endif // TLM_TRANS_RECORD

#include "ni_transaction.h"

/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/
//#define VERBOSE



//---------------------------------------------------------------------------- 
// CLASS : ni_event_in_port, ni_event_out_port, (ni_event_in_port_id), ni_event_out_port_id
// 
//---------------------------------------------------------------------------- 

template<class TRANSACTION>
class ni_event_out_port;

template<class TRANSACTION>
class ni_event_in_port : public tlm_target_port<tlm_blocking_put_if<TRANSACTION> > {

 public:
  ni_event_out_port<TRANSACTION> *connected_port;
  ni_event_in_port<TRANSACTION> *hconnected_down_port;
  ni_event_in_port<TRANSACTION> *hconnected_up_port;


#ifdef TLM_TRANS_RECORD  
  // record database management
  ni_transrecord_event<TRANSACTION> *transrecord;
#endif // TLM_TRANS_RECORD

 public:
#ifdef TLM_TRANS_RECORD  
  ni_event_in_port(std::string port_name_, const char *transrecord_name_)
    : tlm_target_port<tlm_blocking_put_if<TRANSACTION> >(port_name_.c_str()), connected_port(0), hconnected_down_port(0), hconnected_up_port(0) {
    transrecord = new ni_transrecord_event<TRANSACTION>(transrecord_name_);
  }
  ~ni_event_in_port() { if (transrecord) delete transrecord; }
#else
  ni_event_in_port(std::string port_name_)
    : tlm_target_port<tlm_blocking_put_if<TRANSACTION> >(port_name_.c_str()), connected_port(0), hconnected_down_port(0), hconnected_up_port(0) {}
  ~ni_event_in_port() {}
#endif

  //----------------------
  // hierachical port to port binding (src module to dst module)
  //----------------------
  void hbind(ni_event_in_port<TRANSACTION> *child_recv_port) {
    tlm_target_port<tlm_blocking_put_if<TRANSACTION> >::bind(*child_recv_port);

    hconnected_down_port = child_recv_port;
    if (child_recv_port) {
      child_recv_port->hconnected_up_port = this;
    } else {
      cout << "ERROR : undefined child_recv_port" << endl;
      exit(0);
    }

  }

  //----------------------
  // Used to bind the target (recv) port to the slave module 
  // (as it implements the tlm_blocking_put_if interface)
  //----------------------
  inline void put_if_bind(tlm_blocking_put_if<TRANSACTION>& iface) {
    bind(iface);
  }
};

template<class TRANSACTION>
class ni_event_out_port : public tlm_initiator_port<tlm_blocking_put_if<TRANSACTION>, 1> {

 public:
  ni_event_in_port<TRANSACTION> *connected_port;
  ni_event_out_port<TRANSACTION> *hconnected_down_port;
  ni_event_out_port<TRANSACTION> *hconnected_up_port;


#ifdef TLM_TRANS_RECORD  
  // record database management
  ni_transrecord_event<TRANSACTION> *transrecord;
#endif // TLM_TRANS_RECORD

 public:
#ifdef TLM_TRANS_RECORD  
  ni_event_out_port(std::string port_name_, const char *transrecord_name_)
    : tlm_initiator_port<tlm_blocking_put_if<TRANSACTION>,1>(port_name_.c_str()), connected_port(0), hconnected_down_port(0), hconnected_up_port(0) {
    transrecord = new ni_transrecord_event<TRANSACTION>(transrecord_name_);
  }
  ~ni_event_out_port() { if (transrecord) delete transrecord; }
#else
  ni_event_out_port(std::string port_name_)
    : tlm_initiator_port<tlm_blocking_put_if<TRANSACTION>, 1>(port_name_.c_str()), connected_port(0), hconnected_down_port(0), hconnected_up_port(0) {}
  ~ni_event_out_port() {}
#endif // TLM_TRANS_RECORD

  //----------------------
  // hierachical port to port binding (src module to dst module)
  //----------------------
  void hbind(ni_event_out_port<TRANSACTION> *top_send_port) {
    tlm_initiator_port<tlm_blocking_put_if<TRANSACTION>, 1>::bind(*top_send_port);

    hconnected_up_port=top_send_port;
    if(top_send_port) {
      top_send_port->hconnected_down_port=this;
    } else {
      cout << "ERROR : undefined top_send_port" << endl;
      exit(0);
    }

  }

  //----------------------
  // port to port binding (always master module to slave module)
  //----------------------
  void bind(ni_event_in_port<TRANSACTION> *target_port) {
    tlm_initiator_port<tlm_blocking_put_if<TRANSACTION>, 1>::bind(*target_port);
    connected_port = target_port;
    if(target_port) {
    target_port->connected_port = this;
    } else {
      cout << "ERROR : undefined target_port" << endl;
      exit(0);
    }
  }

  //----------------------
  // put function
  //----------------------
  void put(TRANSACTION *transaction) {
#ifdef TLM_TRANS_RECORD 
    ni_event_in_port<TRANSACTION> *in_connected_port=NULL;
    ni_event_out_port<TRANSACTION> *out_connected_port=NULL;

    // record transaction on current port
    transrecord->record(*transaction);

    // can't have a put on a hierarchical port
    if(hconnected_down_port) {
      cout << "ERROR : ni_event_out_port : put transaction on a hierarchical port" << endl;
      exit(0);
    }

    // if current port is not connected record transaction on up ports and find connected port   
    if(!connected_port) {
      out_connected_port=hconnected_up_port;
      while(out_connected_port) {
	// record transaction in up port
	out_connected_port->transrecord->record(*transaction);
	in_connected_port=out_connected_port->connected_port;
	out_connected_port=out_connected_port->hconnected_up_port;
      }
    } else {
      in_connected_port=connected_port;
    }
    
    if(in_connected_port) {
     
      // record transaction on the connected port
      in_connected_port->transrecord->record(*transaction);

      // record transaction on the down ports of the connected port
      in_connected_port=in_connected_port->hconnected_down_port;
      while(in_connected_port) {
	// record transaction in down port
	in_connected_port->transrecord->record(*transaction);
	in_connected_port=in_connected_port->hconnected_down_port;
      }      

    } else {
      cout << "WARNING : ni_event_out_port : undefined connected_port, transaction not recorded" << endl;
    }
#endif // TLM_TRANS_RECORD
    (*this)->put(*transaction);
  }
};

// Useless...
// template<class TRANSACTION>
// class ni_event_in_port_id : public ni_event_in_port<TRANSACTION> {

//  private:
//   // port id for fifo identification
//   t_uint16 port_id;

//  public:
// #ifdef TLM_TRANS_RECORD  
//   ni_event_in_port_id(std::string port_name_, const char *transrecord_name_, t_uint16 port_id_)
//     : ni_event_in_port<TRANSACTION>(port_name_, transrecord_name_), port_id(port_id_) {}
// #else
//   ni_event_in_port_id(std::string port_name_, t_uint16 port_id_)
//     : ni_event_in_port<TRANSACTION>(port_name_), port_id(port_id_) {}
// #endif
//   ~ni_event_in_port_id() {}
// };

template<class TRANSACTION>
class ni_event_out_port_id : public ni_event_out_port<TRANSACTION> {

 private:
  // port id for fifo identification
  t_uint16 port_id;

 public:
#ifdef TLM_TRANS_RECORD  
  ni_event_out_port_id(std::string port_name_, const char *transrecord_name_, t_uint16 port_id_)
    : ni_event_out_port<TRANSACTION>(port_name_, transrecord_name_), port_id(port_id_) {}
#else
  ni_event_out_port_id(std::string port_name_, t_uint16 port_id_)
    : ni_event_out_port<TRANSACTION>(port_name_), port_id(port_id_) {}
#endif // TLM_TRANS_RECORD
  ~ni_event_out_port_id() {}

  //----------------------
  // put function
  //----------------------
  inline void put(TRANSACTION *transaction) {
    transaction->set_source_id(port_id);
    ni_event_out_port<TRANSACTION>::put(transaction);
  }
};


//---------------------------------------------------------------------------- 
// CLASS : ni_beginend_in_port, ni_beginend_out_port, ni_beginend_in_port_id, ni_beginend_out_port_id
// 
//---------------------------------------------------------------------------- 

template<class TRANS_BEGIN, class TRANS_END>
class ni_beginend_out_port;

template<class TRANS_BEGIN, class TRANS_END>
class ni_beginend_in_port : public tlm_target_port<tlm_blocking_put_if<TRANS_BEGIN> >,
			    public tlm_initiator_port<tlm_blocking_put_if<TRANS_END>, 1> {
 public:
  ni_beginend_out_port<TRANS_BEGIN, TRANS_END> *connected_port;
  ni_beginend_in_port<TRANS_BEGIN, TRANS_END> *hconnected_down_port;
  ni_beginend_in_port<TRANS_BEGIN, TRANS_END> *hconnected_up_port;


#ifdef TLM_TRANS_RECORD  
  // record database management
  ni_transrecord_beginend<TRANS_BEGIN, TRANS_END> *transrecord;
#endif // TLM_TRANS_RECORD

 public:
#ifdef TLM_TRANS_RECORD  
  ni_beginend_in_port(std::string port_name_, const char *transrecord_name_,
		      const char *begin_name = "data", const char *end_name = "accept")
    : tlm_target_port<tlm_blocking_put_if<TRANS_BEGIN> >((port_name_ + "_" + begin_name).c_str()),
      tlm_initiator_port<tlm_blocking_put_if<TRANS_END>, 1>((port_name_ + "_" + end_name).c_str()), connected_port(0), hconnected_down_port(0), hconnected_up_port(0) {
    transrecord = new ni_transrecord_beginend<TRANS_BEGIN, TRANS_END>(transrecord_name_);
  }
  ~ni_beginend_in_port() {
    if (transrecord)    delete transrecord;
  }
#else
  ni_beginend_in_port(std::string port_name_,
		      const char *begin_name = "data", const char *end_name = "accept")
    : tlm_target_port<tlm_blocking_put_if<TRANS_BEGIN> >((port_name_ + "_" + begin_name).c_str()),
      tlm_initiator_port<tlm_blocking_put_if<TRANS_END>, 1>((port_name_ + "_" + end_name).c_str()), connected_port(0), hconnected_down_port(0), hconnected_up_port(0) {}
  ~ni_beginend_in_port() {}
#endif // TLM_TRANS_RECORD 

  //----------------------
  // hierachical port to port binding (src module to dst module)
  //----------------------
  void hbind(ni_beginend_in_port *data_in_port) {
    tlm_target_port<tlm_blocking_put_if<TRANS_BEGIN> >::bind(*data_in_port);
    data_in_port->tlm_initiator_port<tlm_blocking_put_if<TRANS_END>, 1>::bind(*this);
 
    hconnected_down_port=data_in_port;
    data_in_port->hconnected_up_port=this;

  }

  //----------------------
  // Used to bind the target (data) port to the slave module 
  // (as it implements the tlm_blocking_put_if interface)
  //----------------------
  inline void put_if_bind(tlm_blocking_put_if<TRANS_BEGIN>& iface) {
    tlm_target_port<tlm_blocking_put_if<TRANS_BEGIN> >::bind(iface);
  }

  //----------------------
  // put functions
  //----------------------
  void put(TRANS_END *transaction) {
#ifdef TLM_TRANS_RECORD 
    ni_beginend_out_port<TRANS_BEGIN, TRANS_END> *out_connected_port=NULL;
    ni_beginend_in_port<TRANS_BEGIN, TRANS_END> *in_connected_port=NULL;

    // record transaction on current port
    transrecord->record(*transaction);

    // can't have a put on a hierarchical port
    if(hconnected_down_port) {
      cout << "ERROR : ni_beginend_in_port : put transaction on a hierarchical port" << endl;
      exit(0);
    }

    // if current port is not connected record transaction on up ports and find connected port   
    if(!connected_port) {
      in_connected_port=hconnected_up_port;
      while(in_connected_port) {
	// record transaction an up port
	in_connected_port->transrecord->record(*transaction);
	out_connected_port=in_connected_port->connected_port;
	in_connected_port=in_connected_port->hconnected_up_port;
      }
    } else {
      out_connected_port=connected_port;
    }
    
    if(out_connected_port) {
     
      // record transaction on the connected port
      out_connected_port->transrecord->record(*transaction);

      // record transaction on the down ports of the connected port
      out_connected_port=out_connected_port->hconnected_down_port;
      while(out_connected_port) {
	// record transaction out down port
	out_connected_port->transrecord->record(*transaction);
	out_connected_port=out_connected_port->hconnected_down_port;
      }      

    } else {
      cout << "WARNING : ni_beginend_in_port : undefined connected_port, transaction not recorded" << endl;
    }
#endif // TLM_TRANS_RECORD 
    tlm_initiator_port<tlm_blocking_put_if<TRANS_END>, 1>::operator->()->put(*transaction);
  }
};

template<class TRANS_BEGIN, class TRANS_END>
class ni_beginend_out_port : public tlm_initiator_port<tlm_blocking_put_if<TRANS_BEGIN>, 1>,
			     public tlm_target_port<tlm_blocking_put_if<TRANS_END> > {
 public:
  ni_beginend_in_port<TRANS_BEGIN, TRANS_END> *connected_port;
  ni_beginend_out_port<TRANS_BEGIN, TRANS_END> *hconnected_down_port;
  ni_beginend_out_port<TRANS_BEGIN, TRANS_END> *hconnected_up_port;

#ifdef TLM_TRANS_RECORD  
  // record database management
  ni_transrecord_beginend<TRANS_BEGIN, TRANS_END> *transrecord;
#endif // TLM_TRANS_RECORD 

 public:
#ifdef TLM_TRANS_RECORD  
  ni_beginend_out_port(std::string port_name_, const char *transrecord_name_,
		       const char *begin_name = "data", const char *end_name = "accept")
    : tlm_initiator_port<tlm_blocking_put_if<TRANS_BEGIN>, 1>((port_name_ + "_" + begin_name).c_str()),
      tlm_target_port<tlm_blocking_put_if<TRANS_END> >((port_name_ + "_" + end_name).c_str()), connected_port(0), hconnected_down_port(0), hconnected_up_port(0) {
    transrecord = new ni_transrecord_beginend<TRANS_BEGIN, TRANS_END>(transrecord_name_);
  }
  ~ni_beginend_out_port() {
    if (transrecord)    delete transrecord;
  }
#else
  ni_beginend_out_port(std::string port_name_,
		       const char *begin_name = "data", const char *end_name = "accept")
    : tlm_initiator_port<tlm_blocking_put_if<TRANS_BEGIN>, 1>((port_name_ + "_" + begin_name).c_str()),
      tlm_target_port<tlm_blocking_put_if<TRANS_END> >((port_name_ + "_" + end_name).c_str()), connected_port(0), hconnected_down_port(0), hconnected_up_port(0) {}
  ~ni_beginend_out_port() {}
#endif // TLM_TRANS_RECORD 

  //----------------------
  // port to port binding (always master module to slave module)
  //----------------------
  void bind(ni_beginend_in_port<TRANS_BEGIN, TRANS_END> *target_port) {
    tlm_initiator_port<tlm_blocking_put_if<TRANS_BEGIN>, 1>::bind(*target_port);
    target_port->tlm_initiator_port<tlm_blocking_put_if<TRANS_END>, 1>::bind(*this);
    connected_port = target_port;
    target_port->connected_port = this;
  }

  //----------------------
  // hierachical port to port binding (src module to dst module)
  //----------------------
  void hbind(ni_beginend_out_port *data_out_port) {
    tlm_initiator_port<tlm_blocking_put_if<TRANS_BEGIN>, 1>::bind(*data_out_port);
    data_out_port->tlm_target_port<tlm_blocking_put_if<TRANS_END> >::bind(*this);

    hconnected_up_port=data_out_port;
    data_out_port->hconnected_down_port=this;

  }


  //----------------------
  // Used to bind the target (status) port to the slave module 
  // (as it implements the tlm_blocking_put_if interface)
  //----------------------
  inline void put_if_bind(tlm_blocking_put_if<TRANS_END>& iface) {
    tlm_target_port<tlm_blocking_put_if<TRANS_END> >::bind(iface);
  }

  //----------------------
  // put functions
  //----------------------
  void put(TRANS_BEGIN *transaction) {
#ifdef TLM_TRANS_RECORD  

    ni_beginend_in_port<TRANS_BEGIN, TRANS_END> *in_connected_port=NULL;
    ni_beginend_out_port<TRANS_BEGIN, TRANS_END> *out_connected_port=NULL;

    // record transaction on current port
    transrecord->record(*transaction);

    // can't have a put on a hierarchical port
    if(hconnected_down_port) {
      cout << "ERROR : ni_beginend_out_port : put transaction on a hierarchical port" << endl;
      exit(0);
    }

    // if current port is not connected record transaction on up ports and find connected port   
    if(!connected_port) {
      out_connected_port=hconnected_up_port;
      while(out_connected_port) {
	// record transaction in up port
	out_connected_port->transrecord->record(*transaction);
	in_connected_port=out_connected_port->connected_port;
	out_connected_port=out_connected_port->hconnected_up_port;
      }
    } else {
      in_connected_port=connected_port;
    }
   
    if(in_connected_port) {
     
      // record transaction on the connected port
      in_connected_port->transrecord->record(*transaction);

      // record transaction on the down ports of the connected port
      in_connected_port=in_connected_port->hconnected_down_port;
      while(in_connected_port) {	
	// record transaction in down port
	in_connected_port->transrecord->record(*transaction);	
	in_connected_port=in_connected_port->hconnected_down_port;;
      }    

    } else {
      cout << "WARNING : ni_beginend_out_port : undefined connected_port, transaction not recorded" << endl;
    }

#endif // TLM_TRANS_RECORD 
    tlm_initiator_port<tlm_blocking_put_if<TRANS_BEGIN>, 1>::operator->()->put(*transaction);

  }
};

template<class TRANS_BEGIN, class TRANS_END>
class ni_beginend_in_port_id : public ni_beginend_in_port<TRANS_BEGIN, TRANS_END> {

 private:
  // port id for fifo identification
  t_uint16 port_id;

 public:
#ifdef TLM_TRANS_RECORD  
  ni_beginend_in_port_id(std::string port_name_, const char *transrecord_name_, t_uint16 port_id_,
			 const char *begin_name = "data", const char *end_name = "accept")
    : ni_beginend_in_port<TRANS_BEGIN, TRANS_END>(port_name_, transrecord_name_, begin_name, end_name), port_id(port_id_) {}
#else
  ni_beginend_in_port_id(std::string port_name_, t_uint16 port_id_,
			 const char *begin_name = "data", const char *end_name = "accept")
    : ni_beginend_in_port<TRANS_BEGIN, TRANS_END>(port_name_, begin_name, end_name), port_id(port_id_) {}
#endif // TLM_TRANS_RECORD
  ~ni_beginend_in_port_id() {}

  //----------------------
  // put functions
  //----------------------
  inline void put(TRANS_END *transaction) {
    transaction->set_source_id(port_id);
    ni_beginend_in_port<TRANS_BEGIN, TRANS_END>::put(transaction);
  }
};

template<class TRANS_BEGIN, class TRANS_END>
class ni_beginend_out_port_id : public ni_beginend_out_port<TRANS_BEGIN, TRANS_END> {

 private:
  // port id for fifo identification
  t_uint16 port_id;

 public:
#ifdef TLM_TRANS_RECORD  
  ni_beginend_out_port_id(std::string port_name_, const char *transrecord_name_, t_uint16 port_id_,
			  const char *begin_name = "data", const char *end_name = "accept")
    : ni_beginend_out_port<TRANS_BEGIN, TRANS_END>(port_name_, transrecord_name_, begin_name, end_name), port_id(port_id_) {}
#else
  ni_beginend_out_port_id(std::string port_name_, t_uint16 port_id_,
			  const char *begin_name = "data", const char *end_name = "accept")
    : ni_beginend_out_port<TRANS_BEGIN, TRANS_END>(port_name_, begin_name, end_name), port_id(port_id_) {}
#endif // TLM_TRANS_RECORD 
  ~ni_beginend_out_port_id() {}

  //----------------------
  // put functions
  //----------------------
  inline void put(TRANS_BEGIN *transaction) {
    transaction->set_source_id(port_id);
    ni_beginend_out_port<TRANS_BEGIN, TRANS_END>::put(transaction);
  }
};

#endif /* _NI_PORT_H_ */
