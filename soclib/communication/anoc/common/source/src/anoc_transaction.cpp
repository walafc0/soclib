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

/*------------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------------*/
#include "anoc_transaction.h"

//----------------------------------------------------------------------------
///  CLASS : anoc_transaction_if
///
///  anoc transaction data definition class
/**
   This class defines the content common in all transactions in the context of the
   ANOC protocol.
   It is encompassed in the tlm_transaction object that is passed to the put() method.
   When a ANOC target module receives the tlm_transaction object, it extracts the 
   useful information to processs that transaction.
 **/
//----------------------------------------------------------------------------
// constructors
anoc_transaction_if::anoc_transaction_if() {}

anoc_transaction_if::anoc_transaction_if(int channel) {
  m_channel = channel;
  m_target_port_id = -1;

  m_step_sim_value = 1;     // Smallest SystemC time unit by default -> should be
  m_step_sim_unit = SC_FS;  // set by the channel with the channel step sim parameters 
}

// destructor
anoc_transaction_if::~anoc_transaction_if() {
}

//------------------------------------------
// copy operator : 
//   - allows to copy a transaction content, field by field, like: tr_data_2 = tr_data_1;
//------------------------------------------
anoc_transaction_if& anoc_transaction_if::operator= (const anoc_transaction_if& data) {
  m_channel = data.m_channel;
  m_step_sim_value = data.m_step_sim_value;
  m_step_sim_unit  = data.m_step_sim_unit;

  return *this;
}

//----------------------------------------------------------------------------
///  CLASS : anoc_accept_transaction
///
///  anoc ACCEPT transaction data definition class
/**
   This class defines the content of an ACCEPT transaction in the context of the
   ANOC protocol.
 **/
//----------------------------------------------------------------------------
//----------------
// constructors
//---------------
  
/// Default constructor 
/// - no member initialization
anoc_accept_transaction::anoc_accept_transaction() : anoc_transaction_if() {}

/// ACCEPT type transaction constructor
anoc_accept_transaction::anoc_accept_transaction(int channel) : anoc_transaction_if(channel) {}


//----------------
// destructor
//---------------
anoc_accept_transaction::~anoc_accept_transaction() {
}

//------------------------------------------
// copy operator : 
//   - allows to copy an ACCEPT transaction content, field by field, like:
//     accept_tr_data_2 = accept_tr_data_1;
//------------------------------------------
anoc_accept_transaction& anoc_accept_transaction::operator= (const anoc_accept_transaction& data) {

  anoc_transaction_if::operator = (data);

  return(*this);
}


//----------------------------------------------------------------------------
///  CLASS : anoc_data_transaction
///
///  anoc DATA transaction data definition class
/**
   This class defines the content of a DATA transaction in the context of the
   ANOC protocol.
 **/
//----------------------------------------------------------------------------

//----------------
// constructors
//---------------

/// Default constructor 
anoc_data_transaction::anoc_data_transaction() : anoc_transaction_if() {
#ifdef TLM_TRANS_RECORD_SDI2
  m_last_record_valid = false;
#endif // TLM_TRANS_RECORD_SDI2
  m_param0 = 0;
  m_mtype = USUAL0;
}
 
/// DATA header type transaction constructor
anoc_data_transaction::anoc_data_transaction(int channel,
                                              t_bit bop,
                                              t_bit eop,
                                              anoc_mtype mtype,
                                              anoc_cmd cmd,
                                              t_uint16 param1,
                                              t_uint16 param2,
                                              int srcid,
                                              int dstid,
                                              anoc_dir *path_to_target,
                                              t_uint32 data
                                              ) : anoc_transaction_if(channel) {
#ifdef TLM_TRANS_RECORD_SDI2
  m_last_record_valid = false;
#endif // TLM_TRANS_RECORD_SDI2
  m_bop = bop;
  m_eop = eop;
  m_mtype = mtype;
  m_cmd = cmd;
  m_param0 = 0;
  m_param1 = param1;
  m_param2 = param2;
  m_srcid = srcid;
  m_dstid = dstid;
  if (path_to_target) {
    for (int i = 0; i < ANOC_PATH_LENGTH; i++) {
      m_path_to_target[i] = path_to_target[i];
    }
  }
  m_data = data;
}

/// DATA header type transaction constructor, with all paramX (X=0,1,3), without data (useless)
anoc_data_transaction::anoc_data_transaction(int channel,
                                              t_bit bop,
                                              t_bit eop,
                                              anoc_mtype mtype,
                                              anoc_cmd cmd,
                                              t_uint16 param0,
                                              t_uint16 param1,
                                              t_uint16 param2,
                                              int srcid,
                                              int dstid,
                                              anoc_dir *path_to_target
					     ) : anoc_transaction_if(channel)
{
#ifdef TLM_TRANS_RECORD_SDI2
  m_last_record_valid = false;
#endif // TLM_TRANS_RECORD_SDI2
  m_bop = bop;
  m_eop = eop;
  m_mtype = mtype;
  m_cmd = cmd;
  m_param0 = param0;
  m_param1 = param1;
  m_param2 = param2;
  m_srcid = srcid;
  m_dstid = dstid;
  if (path_to_target) {
    for (int i = 0; i < ANOC_PATH_LENGTH; i++) {
      m_path_to_target[i] = path_to_target[i];
    }
  }
}

/// DATA flit type transaction constructor
anoc_data_transaction::anoc_data_transaction( int channel,
		       t_bit eop,
		       t_uint32 data) : anoc_transaction_if(channel)
{
#ifdef TLM_TRANS_RECORD_SDI2
  m_last_record_valid = false;
#endif // TLM_TRANS_RECORD_SDI2
  m_bop = 0;
  m_eop = eop;
  m_data = data;
}

//----------------
// destructor
//---------------
anoc_data_transaction::~anoc_data_transaction() {
}


//------------------------------------------
// copy operator : 
//   - allows to copy an DATA transaction content, field by field, like:
//     data_tr_data_2 = data_tr_data_1;
//------------------------------------------
anoc_data_transaction& anoc_data_transaction::operator = (const anoc_data_transaction& data) {

  anoc_transaction_if::operator = (data);
#ifdef TLM_TRANS_RECORD_SDI2
  m_last_record_valid = data.m_last_record_valid;
  if (data.m_last_record_valid)
    m_last_record = data.m_last_record;
#endif // TLM_TRANS_RECORD_SDI2
  m_bop     = data.m_bop;
  m_eop     = data.m_eop;
  m_mtype   = data.m_mtype;
  m_cmd     = data.m_cmd;
  m_param0  = data.m_param0;
  m_param1  = data.m_param1;
  m_param2  = data.m_param2;
  m_srcid   = data.m_srcid;
  m_dstid   = data.m_dstid;
  m_data    = data.m_data;
  m_data_36 = data.m_data_36;

  // for the path_to_target table, we copy the table content
  for (int i = 0; i < ANOC_PATH_LENGTH; i++) {
    m_path_to_target[i] = data.m_path_to_target[i];
  }

  return *this;
}


//------------------------------------------
// shift path : 
//   - for a data type transaction, when the flit is a header,
//     this function shifts the path
//------------------------------------------
void anoc_data_transaction::shift_path() {
  for (int i = 0; i < ANOC_PATH_LENGTH - 1; i++) {
    m_path_to_target[i] = m_path_to_target[i+1];
  }
  m_path_to_target[ANOC_PATH_LENGTH - 1] = NORTH;
}

// Conversion to sc_lv<34>
void anoc_data_transaction::to_bits(sc_lv<34>& word) const {
  word = 0;
  word[33] = m_bop;
  word[32] = m_eop;
  if (m_bop == false) { //data flit
    word.range(31,0) = m_data;
  } else {
    switch (m_cmd) {
      case DATA        : word.range(32,29) =  0 /* "0000" */   ; word.range(26,25) = m_param1 %  4 ; break;
      case CREDIT      : word.range(32,29) =  8 /* "1000" */   ; word.range(26,25) = m_param1 %  4 ;  word.range(28,27) = m_param0 % 4 ; break;
      case DUMP_REQ    : word.range(32,27) =  4 /* "000100" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case DUMP_RESP   : word.range(32,27) =  5 /* "000101" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case IT          : word.range(32,27) =  6 /* "000110" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case SESSION_REQ : word.range(32,27) = 36 /* "100100" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case TASK_SEL    : word.range(32,27) = 37 /* "100101" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case REQ_MOVE    : word.range(32,27) = 28 /* "011100" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case DENY_MOVE   : word.range(32,27) = 63 /* "111111" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case TR_END_MOVE : word.range(32,27) = 29 /* "011101" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case MOVE        : word.range(32,27) =  7 /* "000111" */ ; word.range(26,25) = m_param1 %  4 ; break;
      case TEST        : word.range(32,27) = 30 /* "011110" */ ; word.range(26,25) = m_param1 %  4 ; break;
    }
    if (m_mtype == BOM||m_mtype == USUAL1) {
      word[31] = 1;
    }
    if (m_mtype == EOM||m_mtype == USUAL1) {
      word[30] = 1;
    }
    word.range(24,18) = m_param2 % 128;
    word.range(17,0) = get_tot_path();
  }
}

// Conversion from sc_lv<34>
void anoc_data_transaction::set_from_bits(sc_lv<34>& word) {
  m_bop = word[33] == true;
  m_eop = word[32] == true;
  if (m_bop == false) { //body flit
    m_mtype = USUAL0; m_cmd = DATA; m_param0 = 0; m_param1 = 0; m_param2 = 0;
    m_data = word.range(31,0).to_uint();
  } else { //header flit
    if(word[29].to_bool() == false) { // short format
      switch (m_eop) {
        case false : m_cmd = DATA  ; break;
        case true :  m_cmd = CREDIT; break;
        default : cout << "WARNING : invalid eop value" << endl; 
      }
      m_param0 = word.range(28,27).to_uint();
    } else { // long format
      if (word.range(29,27).to_uint() == 7 /* "111" */ && !m_eop) // Move Packet
        m_cmd = MOVE;
      else
        switch (word.range(32,27).to_uint()) {
          case  4 /* "000100" */ : m_cmd = DUMP_REQ    ; break;
          case  5 /* "000101" */ : m_cmd = DUMP_RESP   ; break;
          case  6 /* "000110" */ : m_cmd = IT          ; break;
          case 36 /* "100100" */ : m_cmd = SESSION_REQ ; break;
          case 37 /* "100101" */ : m_cmd = TASK_SEL    ; break;
          case 28 /* "011100" */ : m_cmd = REQ_MOVE    ; break;
          case 63 /* "111111" */ : m_cmd = DENY_MOVE   ; break;
          case 29 /* "011101" */ : m_cmd = TR_END_MOVE ; break;
          case 30 /* "011110" */ : m_cmd = TEST        ; break;
        }
      m_param0 = 0;
    }
    switch (word.range(31,30).to_uint()) {
      case 0 /* "00" */ : m_mtype = USUAL0; break;
      case 1 /* "01" */ : m_mtype = EOM; break;
      case 2 /* "10" */ : m_mtype = BOM; break;
      case 3 /* "11" */ : m_mtype = USUAL1; break;
      default :  cout << "WARNING : invalid m_mtype value" << endl; 
    }
    for (int i = 0; i < ANOC_PATH_LENGTH; i++) {
      m_path_to_target[i] = (anoc_dir)(word.range(2*i+1, 2*i).to_uint());
    }
    m_param1 = word.range(26,25).to_uint();
    m_param2 = word.range(24,18).to_uint();
  }
}

// Trace transaction in file
// Format : > BOP EOP DATA # CMD
void anoc_data_transaction::trace_in_file(std::ofstream *data_stream) const {
  char data_str[20];
  sc_lv<34> word;
  *data_stream << "> ";
  *data_stream << m_bop << " " << m_eop << " ";
  to_bits(word);
  sprintf(data_str,"%x",word.to_uint());
  data_stream->width(8);
  data_stream->fill('0');
  *data_stream << data_str;
  if(m_bop) {
    *data_stream << " # " << m_cmd << " (" << m_mtype << ")";
  }
  *data_stream << endl;
}

// ---------
// package functions
// ---------

// specialized sc_trace() function for data types
//   'anoc_accept_transaction_data' and 'anoc_data_transaction_data'

void sc_trace(sc_trace_file* tf, const anoc_accept_transaction& transaction, const std::string& NAME) {
  sc_trace(tf, transaction.get_channel(), NAME + ".channel");
}

void sc_trace(sc_trace_file* tf, const anoc_data_transaction& transaction, const std::string& NAME) {
  sc_trace(tf, transaction.get_channel(), NAME + ".channel");
  sc_trace(tf, transaction.get_bop(), NAME + ".bop");
  sc_trace(tf, transaction.get_eop(), NAME + ".eop");
  sc_trace(tf, transaction.get_mtype(), NAME + ".mtype");
  sc_trace(tf, transaction.get_cmd(), NAME + ".cmd");
  sc_trace(tf, transaction.get_param1(), NAME + ".param1");
  sc_trace(tf, transaction.get_param2(), NAME + ".param2");
  sc_trace(tf, transaction.get_srcid(), NAME + ".srcid");
  sc_trace(tf, transaction.get_dstid(), NAME + ".dstid");
  sc_trace(tf, transaction.get_path(), NAME + ".path");
  sc_trace(tf, transaction.get_data(), NAME + ".data");
}
