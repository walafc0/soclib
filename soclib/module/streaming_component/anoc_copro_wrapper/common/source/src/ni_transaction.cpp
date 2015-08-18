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
 * Comment : Transactions for communication between NI and Core
 *
 */

/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include "ni_transaction.h"


//----------------------------------------------------------------------------
///  CLASS : ni_accept_data_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------
  
ni_accept_data_transaction::ni_accept_data_transaction() {
  source_id = 0;
  core_id = 0;
}


//----------------
// destructor
//----------------
ni_accept_data_transaction::~ni_accept_data_transaction() {
}

//----------------
// copy operator 
//----------------
ni_accept_data_transaction& ni_accept_data_transaction::operator=(const ni_accept_data_transaction& trans) {
  source_id = trans.source_id;
  core_id = trans.core_id;
  return (*this);
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_accept_data_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
					    vector<sdiT::attributeT*> *m_attributes) {

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "core_id",
					       sdiT::enumsT::UNSIGNED32,
					       "`x"
					       ));
}

//----------------
// set attributes for transrecord  
//----------------

void ni_accept_data_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {

  if(m_attributes->size()==1) {
    (*m_attributes)[0]->setValueC(&core_id);
  } else {
    cout << "ERROR : ni_accept_data_transaction::set_attributes : m_attributes not set!";
    exit(0);
  }

}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_accept_data_transaction::set_attributes(scv_tr_handle *h) 
{
	(*h).record_attribute("core_id", core_id); // record attribute name and value
}
void ni_accept_data_transaction::create_name(string *m_transaction_name)
{
	(*m_transaction_name)	= "ni_accept_data_transaction";
}
#endif // TLM_TRANS_RECORD_SCV


//----------------------------------------------------------------------------
///  CLASS : ni_write_data_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_write_data_transaction::ni_write_data_transaction() {
  source_id = 0;
  core_id = 0; 
 }

//----------------
// destructor
//----------------
ni_write_data_transaction::~ni_write_data_transaction() {
}

//----------------
// copy operator  
//----------------
ni_write_data_transaction& ni_write_data_transaction::operator = (const ni_write_data_transaction& trans) {

  source_id=trans.source_id;
  core_id=trans.core_id;
  data=trans.data;

  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_write_data_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
					    vector<sdiT::attributeT*> *m_attributes) {

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "data",
					       sdiT::enumsT::UNSIGNED32,
					       "`x"
					       ));

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "core_id",
					       sdiT::enumsT::UNSIGNED32,
					       "`x"
					       ));

}

//----------------
// set attributes for transrecord  
//----------------
void ni_write_data_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {

  if (m_attributes->size() == 2) {
    (*m_attributes)[0]->setValueC(&data);
    (*m_attributes)[1]->setValueC(&core_id);
  } else {
    cout << "ERROR: ni_write_data_transaction::set_attributes: m_attributes not set!";
    exit(1);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_write_data_transaction::set_attributes(scv_tr_handle *h) 
{
	char displayed_data[10];
	sprintf(displayed_data, "0x%X", data);
	(*h).record_attribute("data", (string) displayed_data); // record attribute name and value
	(*h).record_attribute("core_id", core_id); // record attribute name and value
}
void ni_write_data_transaction::create_name(string *m_transaction_name)
{
	(*m_transaction_name)	= "ni_write_data_transaction";
}
#endif //TLM_TRANS_RECORD_SCV


//----------------------------------------------------------------------------
///  CLASS : ni_write_data_bop_eop_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------
ni_write_data_bop_eop_transaction::ni_write_data_bop_eop_transaction()
  :vc(true)
{}

//----------------
// destructor
//----------------
ni_write_data_bop_eop_transaction::~ni_write_data_bop_eop_transaction() {}

//----------------
// copy operator  
//----------------
ni_write_data_bop_eop_transaction& ni_write_data_bop_eop_transaction::operator=(const ni_write_data_bop_eop_transaction& trans) {
  this->ni_write_data_transaction::operator=(trans);
  bop = trans.bop;
  eop = trans.eop;

  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_write_data_bop_eop_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
							  vector<sdiT::attributeT*> *m_attributes) {
  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					      "data",
					      sdiT::enumsT::UNSIGNED32,
					      "`x"
					      )); 
  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					      "bop",
					      sdiT::enumsT::BOOL,
					      "`b"
					      )); 
  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					      "eop",
					      sdiT::enumsT::BOOL,
					      "`b"
					      )); 
  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					      "vc",
					      sdiT::enumsT::BOOL,
					      "`b"
					      )); 
}

//----------------
// set attributes for transrecord  
//----------------
void ni_write_data_bop_eop_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {
  if (m_attributes->size() == 4) {
    (*m_attributes)[0]->setValueC(&data);
    (*m_attributes)[1]->setValueC(&bop);
    (*m_attributes)[2]->setValueC(&eop);
    (*m_attributes)[3]->setValueC(&vc);
  } else {
    cout << "ERROR: ni_write_data_bop_eop_transaction::set_attributes: m_attributes not created!";
    exit(1);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_write_data_bop_eop_transaction::set_attributes(scv_tr_handle *h) 
{
	char displayed_data[10];
	sprintf(displayed_data, "0x%X", data);
	(*h).record_attribute("data", (string) displayed_data); // record attribute name and value
	(*h).record_attribute("bop", bop); // record attribute name and value
	(*h).record_attribute("eop", eop); // record attribute name and value
	(*h).record_attribute("vc", vc); // record attribute name and value
}
void ni_write_data_bop_eop_transaction::create_name(string *m_transaction_name)
{
	(*m_transaction_name)	= "ni_write_data_bop_eop_transaction";
}
#endif //TLM_TRANS_RECORD_SCV


//----------------------------------------------------------------------------
///  CLASS : ni_available_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_available_transaction::ni_available_transaction() { 
}

//----------------
// destructor
//----------------
ni_available_transaction::~ni_available_transaction() {
}

//----------------
// copy operator
//----------------
ni_available_transaction& ni_available_transaction::operator=(const ni_available_transaction& trans) {
  ni_write_data_transaction::operator=(trans);
  return *this;
}


//----------------------------------------------------------------------------
///  CLASS : ni_released_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_released_transaction::ni_released_transaction() { 
}

//----------------
// destructor
//----------------
ni_released_transaction::~ni_released_transaction() {
}

//----------------
// copy operator
//----------------
ni_released_transaction& ni_released_transaction::operator=(const ni_released_transaction& trans) {
  ni_write_data_transaction::operator=(trans);
  return *this;
}


//----------------------------------------------------------------------------
///  CLASS : ni_status_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_status_transaction::ni_status_transaction() {
  core_id = 0;
  status = 0;
}

ni_status_transaction::ni_status_transaction(t_uint16 status_) {
  core_id = 0;
  status = status_;
}

//----------------
// destructor
//----------------
ni_status_transaction::~ni_status_transaction() {
}

//----------------
// copy operator 
//----------------
ni_status_transaction& ni_status_transaction::operator=(const ni_status_transaction& trans) {
  status = trans.status;
  core_id = trans.core_id;
  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_status_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
					    vector<sdiT::attributeT*> *m_attributes) {

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "core_id",
					       sdiT::enumsT::UNSIGNED32,
					       "`x"
					       ));
  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "status",
					       sdiT::enumsT::UNSIGNED32,
					       "`x"
					       ));

}

//----------------
// set attributes for transrecord  
//----------------
void ni_status_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {

  if(m_attributes->size() == 2) {
    (*m_attributes)[0]->setValueC(&core_id);
    (*m_attributes)[1]->setValueC(&status);
  } else {
    cout << "ERROR : ni_status_transaction::set_attributes : m_attributes not set!";
    exit(0);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_status_transaction::set_attributes(scv_tr_handle *h) 
{
	(*h).record_attribute("core_id", core_id); // record attribute name and value
	(*h).record_attribute("status", status); // record attribute name and value
}
void ni_status_transaction::create_name(string *m_transaction_name)
{
	(*m_transaction_name)	= "ni_status_transaction";
}
#endif //TLM_TRANS_RECORD_SCV

//----------------------------------------------------------------------------
///  CLASS : ni_exec_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_exec_transaction::ni_exec_transaction() { }

//----------------
// destructor
//----------------
ni_exec_transaction::~ni_exec_transaction() {
}

//----------------
// copy operator  
//----------------
ni_exec_transaction& ni_exec_transaction::operator=(const ni_exec_transaction& trans) {
  source_id=trans.source_id;
  slot_id=trans.slot_id;
  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_exec_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
					    vector<sdiT::attributeT*> *m_attributes) {

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "type",
					       sdiT::enumsT::STRING,
					       "`a"
					       ));

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "slot_id",
					       sdiT::enumsT::UNSIGNED32,
					       "`d"
					       ));

}

//----------------
// set attributes for transrecord  
//----------------
void ni_exec_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {

  char type[]="CORE_EXEC";

  if(m_attributes->size() > 0) {
    (*m_attributes)[0]->setValue(type);
    (*m_attributes)[1]->setValue(const_cast<t_uint32*>(&slot_id)); // Due to SDI2 declaration of setValue, but we won't change data...
  } else {
    cout << "ERROR : ni_exec_transaction::set_attributes : m_attributes not set!";
    exit(0);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_exec_transaction::set_attributes(scv_tr_handle *h) 
{
	char type[]="CORE_EXEC";
	(*h).record_attribute("type",(string) type); // record attribute name and value
	(*h).record_attribute("slot_id", slot_id); // record attribute name and value
}
void ni_exec_transaction::create_name(string *m_transaction_name)
{
	(*m_transaction_name)	= "ni_exec_transaction";
}
#endif // TLM_TRANS_RECORD_SCV

//----------------------------------------------------------------------------
///  CLASS : ni_eoc_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_eoc_transaction::ni_eoc_transaction() {
  core_id = 0;
}
 
//----------------
// destructor
//----------------
ni_eoc_transaction::~ni_eoc_transaction() {
}

//----------------
// copy operator  
//----------------
ni_eoc_transaction& ni_eoc_transaction::operator=(const ni_eoc_transaction& trans) {

  core_id = trans.core_id;
  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_eoc_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
					    vector<sdiT::attributeT*> *m_attributes) {
  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "core_id",
					       sdiT::enumsT::UNSIGNED32,
					       "`x"
					       ));
}

//----------------
// set attributes for transrecord  
//----------------
void ni_eoc_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {

  if(m_attributes->size()==1) {
    (*m_attributes)[0]->setValueC(&core_id);
  } else {
    cout << "ERROR : ni_eoc_transaction::set_attributes : m_attributes not set!";
    exit(0);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_eoc_transaction::set_attributes(scv_tr_handle *h) 
{
	(*h).record_attribute("core_id", core_id); // record attribute name and value
}
void ni_eoc_transaction::create_name(string *m_transaction_name)
{
	(*m_transaction_name)	= "ni_eoc_transaction";
}
#endif // TLM_TRANS_RECORD_SCV

//----------------------------------------------------------------------------
///  CLASS : ni_cfg_dump_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_cfg_dump_transaction::ni_cfg_dump_transaction() {}

//----------------
// destructor
//----------------
ni_cfg_dump_transaction::~ni_cfg_dump_transaction() {
}

//----------------
// copy operator 
//----------------
ni_cfg_dump_transaction& ni_cfg_dump_transaction::operator=(const ni_cfg_dump_transaction& trans) {
  address = trans.address;
  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_cfg_dump_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
						vector<sdiT::attributeT*> *m_attributes) {

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "type",
					       sdiT::enumsT::STRING,
					       "`a"
					       ));

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "cfgdump_address",
					       sdiT::enumsT::UNSIGNED32,
					       "`d"
					       ));

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "cfg_data",
					       sdiT::enumsT::UNSIGNED32,
					       "`x"
					       ));
}

//----------------
// set attributes for transrecord  
//----------------
void ni_cfg_dump_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {
  cout << "ERROR : ni_cfg_dump_transaction::set_attributes" << endl;
  exit(0);
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_cfg_dump_transaction::set_attributes(scv_tr_handle *h)
{
        char type[]="CFG_DUMP";
        (*h).record_attribute("type",(string) type); // record attribute name and value
        char displayed_data[10];
        sprintf(displayed_data, "0x%X", address);
        (*h).record_attribute("cfgdump_address", (string) displayed_data); // record attribute name and value
}
void ni_cfg_dump_transaction::create_name(string *m_transaction_name)
{
        (*m_transaction_name)	= "ni_cfg_dump_transaction";
}
#endif // TLM_TRANS_RECORD_SCV

//----------------------------------------------------------------------------
///  CLASS : ni_cfg_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_cfg_transaction::ni_cfg_transaction() {}

//----------------
// destructor
//----------------
ni_cfg_transaction::~ni_cfg_transaction() {
}

//----------------
// copy operator 
//----------------
ni_cfg_transaction& ni_cfg_transaction::operator=(const ni_cfg_transaction& trans) {
  ni_cfg_dump_transaction::operator=(trans);
  data = trans.data;
  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// set attributes for transrecord  
//----------------
void ni_cfg_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {
  char type[]="CFG";

  if(m_attributes->size() == 3) {
    (*m_attributes)[0]->setValue(type);
    (*m_attributes)[1]->setValueC(&address);
    (*m_attributes)[2]->setValueC(&data);
  } else {
    cout << "ERROR: ni_cfg_transaction::set_attributes: m_attributes not created!";
    exit(0);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_cfg_transaction::set_attributes(scv_tr_handle *h) 
{
	char type[]="CFG";
	(*h).record_attribute("type", (string) type); // record attribute name and value
	
	char displayed_data[10];
	sprintf(displayed_data, "0x%X", address);
	(*h).record_attribute("address", (string) displayed_data); // record attribute name and value

	sprintf(displayed_data, "0x%X", data);
	(*h).record_attribute("data", (string) displayed_data); // record attribute name and value
}
#endif // TLM_TRANS_RECORD_SCV

//----------------------------------------------------------------------------
///  CLASS : ni_dump_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_dump_transaction::ni_dump_transaction() {}

//----------------
// destructor
//----------------
ni_dump_transaction::~ni_dump_transaction() {
}

//----------------
// copy operator 
//----------------
ni_dump_transaction& ni_dump_transaction::operator=(const ni_dump_transaction& trans) {
  ni_cfg_dump_transaction::operator=(trans);
  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// set attributes for transrecord  
//----------------
void ni_dump_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {
  char type[]="DUMP";

  if(m_attributes->size()>0) {
    (*m_attributes)[0]->setValue(type);
    (*m_attributes)[1]->setValueC(&address);
  } else {
    cout << "ERROR: ni_dump_transaction::set_attributes: m_attributes not created!";
    exit(0);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_dump_transaction::set_attributes(scv_tr_handle *h) 
{
	char type[]="DUMP";
	(*h).record_attribute("type", (string) type); // record attribute name and value

	char displayed_data[10];
	sprintf(displayed_data, "0x%X", address);
	(*h).record_attribute("address", (string) displayed_data); // record attribute name and value
}
#endif // TLM_TRANS_RECORD_SCV

//----------------------------------------------------------------------------
///  CLASS : ni_dump_data_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_dump_data_transaction::ni_dump_data_transaction() {}

//----------------
// destructor
//----------------
ni_dump_data_transaction::~ni_dump_data_transaction() {
}

//----------------
// copy operator 
//----------------
ni_dump_data_transaction& ni_dump_data_transaction::operator= (const ni_dump_data_transaction& trans) {

  data = trans.data;

  return(*this);
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_dump_data_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
						 vector<sdiT::attributeT*> *m_attributes) {

  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "dump_data",
					       sdiT::enumsT::UNSIGNED32,
					       "`x"
					       ));
}

//----------------
// set attributes for transrecord  
//----------------
void ni_dump_data_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {

  if(m_attributes->size() == 1) {
    (*m_attributes)[0]->setValueC(&data);
  } else {
    cout << "ERROR: ni_dump_data_transaction::set_attributes: m_attributes not created!";
    exit(0);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_dump_data_transaction::set_attributes(scv_tr_handle *h)  
{
	char displayed_data[10];
	sprintf(displayed_data, "0x%X", data);
	(*h).record_attribute("dump_data", (string) displayed_data); // record attribute name and value
}
void ni_dump_data_transaction::create_name(string *m_transaction_name)
{
	(*m_transaction_name)	= "ni_dump_data_transaction";
}
#endif // TLM_TRANS_RECORD_SCV

//----------------------------------------------------------------------------
///  CLASS : ni_write_pkt_ctrl_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_write_pkt_ctrl_transaction::ni_write_pkt_ctrl_transaction() {
  dest_id = 0;
  pkt_sz = 0;
  source_id = 0;
}

//----------------
// destructor
//----------------
ni_write_pkt_ctrl_transaction::~ni_write_pkt_ctrl_transaction() {
}

//----------------
// copy operator  
//----------------
ni_write_pkt_ctrl_transaction& ni_write_pkt_ctrl_transaction::operator=(const ni_write_pkt_ctrl_transaction& trans) {
  dest_id = trans.dest_id;
  pkt_sz = trans.pkt_sz;
  source_id = trans.source_id;
  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_write_pkt_ctrl_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
						      vector<sdiT::attributeT*> *m_attributes) {
  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "dest_id",
					       sdiT::enumsT::UNSIGNED32,
					       "`d"
					       ));
  m_attributes->push_back(new sdiT::attributeT(m_trans_type,
					       "pkt_sz",
					       sdiT::enumsT::UNSIGNED32,
					       "`d"
					       ));
}

//----------------
// set attributes for transrecord  
//----------------
void ni_write_pkt_ctrl_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {
  if (m_attributes->size() == 2) {
    t_uint32 val = dest_id; // t_uint16 => t_uint32 conversion needed for recording...
    (*m_attributes)[0]->setValueC(&val);
    val = pkt_sz;
    (*m_attributes)[1]->setValueC(&val);
  } else {
    cout << "ERROR: ni_write_pkt_ctrl_transaction::set_attributes: m_attributes not created!";
    exit(1);
  }
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_write_pkt_ctrl_transaction::set_attributes(scv_tr_handle *h) 
{
	t_uint32 val = dest_id; // t_uint16 => t_uint32 conversion needed for recording...
	(*h).record_attribute("dest_id", val); // record attribute name and value
	val = pkt_sz;
	(*h).record_attribute("pkt_sz", val); // record attribute name and value
}
void ni_write_pkt_ctrl_transaction::create_name(string *m_transaction_name)
{
	(*m_transaction_name)	= "ni_write_pkt_ctrl_transaction";
}
#endif // TLM_TRANS_RECORD_SCV

//----------------------------------------------------------------------------
///  CLASS : ni_accept_pkt_ctrl_transaction
//----------------------------------------------------------------------------

//----------------
// constructors
//----------------

ni_accept_pkt_ctrl_transaction::ni_accept_pkt_ctrl_transaction() {
}


//----------------
// destructor
//----------------
ni_accept_pkt_ctrl_transaction::~ni_accept_pkt_ctrl_transaction() {
}

//----------------
// copy operator 
//----------------
ni_accept_pkt_ctrl_transaction& ni_accept_pkt_ctrl_transaction::operator=(const ni_accept_pkt_ctrl_transaction& trans) {
  source_id = trans.source_id;
  return *this;
}

#ifdef TLM_TRANS_RECORD_SDI2
//----------------
// create attributes for transrecord  
//----------------
void ni_accept_pkt_ctrl_transaction::create_attributes(sdiT::transactionTypeT *m_trans_type,
						       vector<sdiT::attributeT*> *m_attributes) {
  //NOTHING
}

//----------------
// set attributes for transrecord  
//----------------

void ni_accept_pkt_ctrl_transaction::set_attributes(vector<sdiT::attributeT*>* m_attributes) const {
  //NOTHING
}
#endif // TLM_TRANS_RECORD_SDI2 

#ifdef TLM_TRANS_RECORD_SCV
void ni_accept_pkt_ctrl_transaction::set_attributes(scv_tr_handle *h) 
{
	//NOTHING
}
void ni_accept_pkt_ctrl_transaction::create_name(string *m_transaction_name)
{
	//NOTHING
}
#endif //TLM_TRANS_RECORD_SCV
