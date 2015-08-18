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

#ifndef _NI_TRANSACTION_H_
#define _NI_TRANSACTION_H_

/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include <vector>
#include <list>
#include "anoc_common.h"
#ifdef TLM_TRANS_RECORD
#include "ni_transrecord_event.h"
#endif // TLM_TRANS_RECORD

/*------------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------------*/
//#define VERBOSE


//----------------------------------------------------------------------------
///  CLASS : ni_write_data_transaction
//----------------------------------------------------------------------------

class ni_write_data_transaction {

protected:
  t_uint32 data;
  t_uint16 source_id;
  t_uint32 core_id;
   
public:
  //----------------
  // constructors
  //----------------  
  ni_write_data_transaction();
  
  //----------------
  // destructor
  //----------------
  virtual ~ni_write_data_transaction();

  //----------------
  // copy operator
  //----------------
  ni_write_data_transaction& operator = (const ni_write_data_transaction& trans);

  //----------------
  // method
  //----------------
  inline void set_data(t_uint32 data_) { data=data_; }
  inline t_uint32 get_data() const { return data; }
  inline void set_source_id(t_uint16 source_id_) { source_id=source_id_; }
  inline t_uint16 get_source_id() const { return source_id; }
  inline void set_core_id(t_uint32 core_id_) { core_id=core_id_; }
  inline t_uint32 get_core_id() const { return core_id; }

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h);
  static void create_name(string *m_transaction_name);
#endif //TLM_TRANS_RECORD_SCV

};


//----------------------------------------------------------------------------
///  CLASS : ni_write_data_bop_eop_transaction
//----------------------------------------------------------------------------

class ni_write_data_bop_eop_transaction :
  public ni_write_data_transaction
{

protected:
  bool bop;
  bool eop;
  bool vc;

public:
  //----------------
  // constructors
  //----------------  
  ni_write_data_bop_eop_transaction(); 
  
  //----------------
  // destructor
  //----------------
  virtual ~ni_write_data_bop_eop_transaction();

  //----------------
  // copy operator
  //----------------
  ni_write_data_bop_eop_transaction& operator=(const ni_write_data_bop_eop_transaction& trans);

  //----------------
  // method
  //----------------
  inline void set_bop(bool bop_) { bop = bop_; }
  inline bool is_bop() const { return bop; }
  inline void set_eop(bool eop_) { eop = eop_; }
  inline bool is_eop() const { return eop; }
  inline void set_vc(bool vc_) { vc = vc_; }
  inline bool is_vc() const { return vc; }

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name);
#endif

};



/// Trace function for ni_write_data_bop_eop transactions (needed for sc_fifo tracing)
inline void sc_trace(sc_trace_file *tf, const ni_write_data_bop_eop_transaction& trans, const std::string& NAME) {
  sc_trace(tf, trans.get_data(), NAME + ".data");
  sc_trace(tf, trans.is_bop(), NAME + ".bop");
  sc_trace(tf, trans.is_eop(), NAME + ".eop");
  sc_trace(tf, trans.is_vc(), NAME + ".vc");
}

/// Output stream operator for ni_write_data_bop_eop transactions (needed for sc_fifo dump)
inline ostream& operator<< (ostream& os, const ni_write_data_bop_eop_transaction& trans) {
  os << ".data = " << trans.get_data() << endl;
  os << ".bop  = " << trans.is_bop() << endl;
  os << ".eop  = " << trans.is_eop() << endl;
  os << ".vc  = " << trans.is_vc() << endl;
  return os;
}

//----------------------------------------------------------------------------
///  CLASS : ni_released_transaction
//----------------------------------------------------------------------------

class ni_released_transaction :
  public ni_write_data_transaction
{
   
public:
  //----------------
  // constructors
  //----------------  
  ni_released_transaction();
  
  //----------------
  // destructor
  //----------------
  virtual ~ni_released_transaction();

  //----------------
  // copy operator
  //----------------
  ni_released_transaction& operator = (const ni_released_transaction& trans);

};


//----------------------------------------------------------------------------
///  CLASS : ni_available_transaction
//----------------------------------------------------------------------------

class ni_available_transaction :
  public ni_write_data_transaction
{
   
public:
  //----------------
  // constructors
  //----------------  
  ni_available_transaction();
  
  //----------------
  // destructor
  //----------------
  virtual ~ni_available_transaction();

  //----------------
  // copy operator
  //----------------
  ni_available_transaction& operator = (const ni_available_transaction& trans);

};


//----------------------------------------------------------------------------
///  CLASS : ni_accept_data_transaction
//----------------------------------------------------------------------------

class ni_accept_data_transaction {

protected :
  t_uint16 source_id;
  t_uint32 core_id;
   
public:
  //----------------
  // constructors
  //----------------
  ni_accept_data_transaction();
  
  //----------------
  // destructor
  //----------------
  virtual ~ni_accept_data_transaction();

  //----------------
  // copy operator
  //----------------
  ni_accept_data_transaction& operator = (const ni_accept_data_transaction& trans);

  //----------------
  // method
  //----------------
  inline void set_source_id(t_uint16 source_id_) { source_id=source_id_; }
  inline t_uint16 get_source_id() const { return source_id; }
  inline void set_core_id(t_uint16 core_id_) { core_id=core_id_; }
  inline t_uint32 get_core_id() const { return core_id; }

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name);
#endif

};

//----------------------------------------------------------------------------
///  CLASS : ni_exec_transaction
//----------------------------------------------------------------------------

class ni_exec_transaction {

private :

  t_uint16 source_id;
  t_uint32 slot_id;

public:
  //----------------
  // constructors
  //----------------  
  ni_exec_transaction();

  //----------------
  // destructor
  //----------------
  virtual ~ni_exec_transaction();

  //----------------
  // copy operator
  //----------------
  ni_exec_transaction& operator = (const ni_exec_transaction& trans);

  //----------------
  // method
  //----------------
  inline void set_source_id(t_uint16 source_id_) { source_id=source_id_; }
  inline t_uint16 get_source_id() const { return source_id; }
  inline void set_slot_id(t_uint32 slot_id_) { slot_id=slot_id_; }
  inline t_uint32 get_slot_id() const { return slot_id; }
  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name);
#endif

};

//----------------------------------------------------------------------------
///  CLASS : ni_eoc_transaction
//----------------------------------------------------------------------------

class ni_eoc_transaction {

private :

  t_uint32 core_id;

public:

  //----------------
  // constructors
  //----------------  
  ni_eoc_transaction();

  //----------------
  // destructor
  //----------------
  virtual ~ni_eoc_transaction();

  //----------------
  // copy operator
  //----------------
  ni_eoc_transaction& operator = (const ni_eoc_transaction& trans);

  //----------------
  // method
  //----------------
  inline void set_core_id(t_uint32 core_id_) { core_id=core_id_; }
  inline t_uint32 get_core_id() const { return core_id; }

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name);
#endif

};

//----------------------------------------------------------------------------
///  CLASS : ni_status_transaction
//----------------------------------------------------------------------------

class ni_status_transaction {

protected:
  t_uint32 status;
  t_uint32 core_id;

public:
  //----------------
  // constructors
  //----------------
  ni_status_transaction();
  ni_status_transaction(t_uint16 status_);

  //----------------
  // destructor
  //----------------
  virtual ~ni_status_transaction();


  //----------------
  // copy operator
  //----------------
  ni_status_transaction& operator = (const ni_status_transaction& trans);

  //----------------
  // method
  //----------------
  inline void set_status(t_uint32 status_) { status=status_; }
  inline t_uint32 get_status() const { return status; }
  inline void set_core_id(t_uint32 core_id_) { core_id=core_id_; }
  inline t_uint32 get_core_id() const { return core_id; }

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name);
#endif

};

//----------------------------------------------------------------------------
///  CLASS : ni_cfg_dump_transaction
//----------------------------------------------------------------------------

class ni_cfg_dump_transaction {

protected:
  t_uint32 address;

public:
  //----------------
  // constructors
  //----------------
  ni_cfg_dump_transaction();

  //----------------
  // destructor
  //----------------
  virtual ~ni_cfg_dump_transaction();

  //----------------
  // copy operator
  //----------------
  virtual ni_cfg_dump_transaction& operator = (const ni_cfg_dump_transaction& trans);

  //----------------
  // method
  //----------------
  virtual inline void set_address(t_uint32 address_) {address=address_;}
  virtual inline t_uint32 get_address() const {return address;}

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  virtual void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  virtual void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name);
#endif

};

//----------------------------------------------------------------------------
///  CLASS : ni_cfg_transaction
//----------------------------------------------------------------------------

class ni_cfg_transaction :
  public ni_cfg_dump_transaction
{

protected:
  t_uint32 data;

public:
  //----------------
  // constructors
  //----------------
  ni_cfg_transaction();

  //----------------
  // destructor
  //----------------
  virtual ~ni_cfg_transaction();

  //----------------
  // copy operator
  //----------------
  virtual ni_cfg_transaction& operator = (const ni_cfg_transaction& trans);

  //----------------
  // method
  //----------------
  virtual inline void set_data(t_uint32 data_) {data=data_;}
  virtual inline t_uint32 get_data() const {return  data;}

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
#endif

};

//----------------------------------------------------------------------------
///  CLASS : ni_dump_transaction
//----------------------------------------------------------------------------

class ni_dump_transaction :
  public ni_cfg_dump_transaction
{

public:
  //----------------
  // constructors
  //----------------
  ni_dump_transaction();

  //----------------
  // destructor
  //----------------
  virtual ~ni_dump_transaction();

  //----------------
  // copy operator
  //----------------
  virtual ni_dump_transaction& operator = (const ni_dump_transaction& trans);

  //----------------
  // method
  //----------------

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
#endif

};

//----------------------------------------------------------------------------
///  CLASS : ni_dump_data_transaction
//----------------------------------------------------------------------------

class ni_dump_data_transaction
{

protected:
  t_uint32 data;

public:
  //----------------
  // constructors
  //----------------
  ni_dump_data_transaction();

  //----------------
  // destructor
  //----------------
  virtual ~ni_dump_data_transaction();

  //----------------
  // copy operator
  //----------------
  virtual ni_dump_data_transaction& operator = (const ni_dump_data_transaction& trans);

  //----------------
  // method
  //----------------

  virtual inline void set_data(t_uint32 data_) {data=data_;}
  virtual inline t_uint32 get_data() const {return  data;}

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name) ;
#endif

};


//----------------------------------------------------------------------------
///  CLASS : ni_write_data_ch_transaction
//----------------------------------------------------------------------------

class ni_write_pkt_ctrl_transaction {

protected:
  t_uint16 dest_id;
  t_uint16 pkt_sz;
  t_uint16 source_id;

public:
  //----------------
  // constructors
  //----------------  
  ni_write_pkt_ctrl_transaction();

  //----------------
  // destructor
  //----------------
  virtual ~ni_write_pkt_ctrl_transaction();

  //----------------
  // copy operator
  //----------------
  ni_write_pkt_ctrl_transaction& operator=(const ni_write_pkt_ctrl_transaction& trans);

  //----------------
  // method
  //----------------
  inline void set_dest_id(t_uint16 dest_id_) { dest_id = dest_id_; }
  inline t_uint16 get_dest_id() const { return dest_id; }
  inline void set_pkt_sz(t_uint16 pkt_sz_) { pkt_sz = pkt_sz_; }
  inline t_uint16 get_pkt_sz() const { return pkt_sz; }
  inline void set_source_id(t_uint16 source_id_) { source_id = source_id_; }
  inline t_uint16 get_source_id() const { return source_id; }

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name);
#endif

};

//----------------------------------------------------------------------------
///  CLASS : ni_accept_pkt_ctrl_transaction
//----------------------------------------------------------------------------

class ni_accept_pkt_ctrl_transaction {

protected :
  t_uint16 source_id;

public:
  //----------------
  // constructors
  //----------------
  ni_accept_pkt_ctrl_transaction();

  //----------------
  // destructor
  //----------------
  virtual ~ni_accept_pkt_ctrl_transaction();

  //----------------
  // copy operator
  //----------------
  ni_accept_pkt_ctrl_transaction& operator=(const ni_accept_pkt_ctrl_transaction& trans);

  //----------------
  // method
  //----------------
  inline void set_source_id(t_uint16 source_id_) { source_id = source_id_; }
  inline t_uint16 get_source_id() const { return source_id; }

  //----------------------
  // transrecord operators
  //----------------------
#ifdef TLM_TRANS_RECORD_SDI2
  static void create_attributes(sdiT::transactionTypeT *m_trans_type,
				vector<sdiT::attributeT*>* m_attributes);
  void set_attributes(vector<sdiT::attributeT*>* m_attributes) const;
#endif // TLM_TRANS_RECORD_SDI2  

#ifdef TLM_TRANS_RECORD_SCV
  void set_attributes(scv_tr_handle *h) ;
  static void create_name(string *m_transaction_name);
#endif

};

#endif /* _NI_TRANSACTION_H_ */
