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

#ifndef _ANOC_TRANSACTION_H_
#define _ANOC_TRANSACTION_H_

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include "anoc_common.h"
#ifdef TLM_TRANS_RECORD
#include "tlm_transrecord.h"
#endif // TLM_TRANS_RECORD

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/

#define ANOC_NB_CHANNEL 2       // number of virtual channel (index 0 is the most prioritary)


/*------------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------------*/


// For a DATA type transfer, this defines the control field of the packet header
typedef enum  {
        DATA        = 0,
        CREDIT      = 1,
        DUMP_REQ    = 2,
        DUMP_RESP   = 3,
        IT          = 4,
        SESSION_REQ = 5,
        TASK_SEL    = 6,
        REQ_MOVE    = 7,
        DENY_MOVE   = 8,
        TR_END_MOVE = 9,
        MOVE        = 10,
        TEST        = 11
        } anoc_cmd;

// For a DATA type transfer, in the packet header, this defines BOM/EOM status of the packet

#define MTYPE_BOM 0x02
#define MTYPE_EOM 0x01

typedef enum
{
  USUAL0 = 0,                    /*BOM = 0, EOM = 0 */ 
  EOM    = MTYPE_EOM,            /*BOM = 0, EOM = 1 */ 
  BOM    = MTYPE_BOM,            /*BOM = 1, EOM = 0 */ 
  USUAL1 = MTYPE_BOM |MTYPE_EOM  /*BOM = 1, EOM = 1 */ 
} anoc_mtype;

typedef enum
{
  SCAN     = 0,
  PINGPONG = 1,
  BIST     = 2
} test_type;

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

class anoc_transaction_if {

private :

  /// virtual channel number.
  int  m_channel;

  /// target port id (mandatory: allows a node to know where a transaction is coming from.
  int  m_target_port_id;

  /// Time equivalent value for one timed transaction. 
  /// This timing values is used for each individual transfer (flit per packet)
  /** Timing "weight" of the transaction
      used by the slave and the channel to compute the duration of
      the wait() statement when accesses are timed. See anoc_timing class
  */
  unsigned int m_step_sim_value;

  /// Time equivalent unit for one timed transaction
  sc_time_unit m_step_sim_unit;


public:

  //----------------
  // constructors
  //---------------

  /// Default constructor 
  anoc_transaction_if();

  /// constructor
  anoc_transaction_if(int channel);


  //----------------
  // destructor
  //---------------
  virtual ~anoc_transaction_if();


  //------------------------------------------
  // Data members access methods
  //------------------------------------------

  /// Returns the channel field 
  inline int get_channel() const {
    return m_channel;
  }

  /// Sets the channel field 
  inline void set_channel(int channel) { 
    m_channel = channel;
  }

  /// Returns the target port ID 
  inline int get_target_port_id() const {
    return m_target_port_id;
  }
  /// Sets the target port ID 
  inline void set_target_port_id(int target_port_id) { 
    m_target_port_id = target_port_id;
  }

  /// Returns the time equivalent value for one timed transaction
  inline unsigned int get_step_sim_value() const {
    return m_step_sim_value;
  }

  /// Sets the time equivalent value for one timed transaction
  inline void set_step_sim_value(unsigned int step_sim_value) { 
    m_step_sim_value = step_sim_value;
  }

  /// Returns the time equivalent unit for one timed transaction
  inline sc_time_unit get_step_sim_unit() const {
    return m_step_sim_unit;
  }

  /// Sets the time equivalent unit for one timed transaction
  inline void set_step_sim_unit(sc_time_unit step_sim_unit) { 
    m_step_sim_unit = step_sim_unit;
  }

  //------------------------------------------
  // copy operator :
  //   - allows to copy a transaction content, field by field, like: tr_data_2 = tr_data_1;
  //------------------------------------------
  anoc_transaction_if& operator = (const anoc_transaction_if& data);

};



//----------------------------------------------------------------------------
///  CLASS : anoc_accept_transaction
///
///  anoc ACCEPT transaction data definition class
/**
   This class defines the content of an ACCEPT transaction in the context of the
   ANOC protocol.
 **/
//----------------------------------------------------------------------------

class anoc_accept_transaction
  : public anoc_transaction_if {

public:

  //----------------
  // constructors
  //---------------

  /// Default constructor 
  /// - no member initialization
  anoc_accept_transaction();

  /// ACCEPT type transaction constructor
  anoc_accept_transaction(int channel);


  //----------------
  // destructor
  //---------------
  virtual ~anoc_accept_transaction();


  //------------------------------------------
  // copy operator : 
  //   - allows to copy an ACCEPT transaction content, field by field, like:
  //     accept_tr_data_2 = accept_tr_data_1;
  //------------------------------------------
  anoc_accept_transaction& operator = (const anoc_accept_transaction& data);
};


//----------------------------------------------------------------------------
///  CLASS : anoc_data_transaction
///
///  anoc DATA transaction data definition class
/**
   This class defines the content of a DATA transaction in the context of the
   ANOC protocol.
 **/
//----------------------------------------------------------------------------

class anoc_data_transaction
  : public anoc_transaction_if {

private:

#ifdef TLM_TRANS_RECORD_SDI2
  /// handle to last SST2 transaction record related to this transaction.
  sdiT::transactionHandleT   m_last_record; 
  bool m_last_record_valid;
#endif // TLM_TRANS_RECORD_SDI2

  /// control bits (begin-of-packet, end-of-packet)
  t_bit    m_bop;
  t_bit    m_eop;


  /// header fields

  anoc_mtype m_mtype;           /// message bits 
  anoc_cmd  m_cmd;                  /// ANOC packet type
  t_uint16  m_param0;           /// ANOC packet new parameter (for sel_credit in credit packet)
  t_uint16  m_param1;           /// ANOC packet short parameter
  t_uint16  m_param2;           /// ANOC packet long parameter

  int m_srcid;                  /// packet emitter id
  int m_dstid;                  /// packet destination id
  anoc_dir m_path_to_target[ANOC_PATH_LENGTH];   /// packet path_to_target within the switch network

  /// data field
  t_uint32 m_data;              // data for remaining flits in a packet
  sc_uint<36> m_data_36;        // data for VCI data compatibility

public:


  //----------------
  // constructors
  //---------------

  /// Default constructor 
  anoc_data_transaction();

  /// DATA type transaction constructor
  anoc_data_transaction(int channel,
                        t_bit bop,
                        t_bit eop,
                        anoc_mtype mtype,
                        anoc_cmd cmd,
                        t_uint16 param1,
                        t_uint16 param2,
                        int srcid,
                        int dstid,
                        anoc_dir *path_to_target,
                        t_uint32 data);

  anoc_data_transaction(int channel,
                        t_bit bop,
                        t_bit eop,
                        anoc_mtype mtype,
                        anoc_cmd cmd,
                        t_uint16 param0,
                        t_uint16 param1,
                        t_uint16 param2,
                        int srcid,
                        int dstid,
                        anoc_dir *path_to_target);

  anoc_data_transaction(int channel,
                        t_bit eop,
                        t_uint32 data);

  //----------------
  // destructor
  //---------------
  virtual ~anoc_data_transaction();

  //------------------------------------------
  // useful methods
  //------------------------------------------

  /// Returns true when bop=1
  inline bool is_bop() const {
    return (m_bop == true);
  }

  /// Returns true when eop=1
  inline bool is_eop() const {
    return (m_eop == true);
  }

  /// Returns true when m_mtype=BOM
  inline bool is_bom() const {
    return (m_mtype == BOM);
  }

  /// Returns true when m_mtype=EOM
  inline bool is_eom() const {
    return (m_mtype == EOM);
  }

  /// Returns true when m_type=EOM or USUAL1
  inline bool is_last_packet() const {
    return ((m_mtype == EOM)||(m_mtype == USUAL1));
  }

  /// Returns true when m_type=BOM or USUAL1
  inline bool is_first_packet() const {
    return ((m_mtype == BOM)||(m_mtype == USUAL1));
  }

  /// Returns current path value (first element in path table)
  inline anoc_dir get_path() const {
    return (m_path_to_target[0]);
  }

  inline t_uint32 get_tot_path() const {
    sc_uint<32> data;
    for (int i = 0; i < ANOC_PATH_LENGTH; i++) {
      data = data << 2;
      data = data.to_uint() + m_path_to_target[ANOC_PATH_LENGTH - 1 - i];
    }
    return ((t_uint32)data.to_uint());
  }

  /// Assign the current path value (first element in path table)
  inline void set_path(anoc_dir path) {
    m_path_to_target[0] = path;
  }


  //------------------------------------------
  // Data members access methods
  //------------------------------------------

#ifdef TLM_TRANS_RECORD_SDI2
  /// Returns the last SST2 transaction handle
  inline sdiT::transactionHandleT get_last_record() const {
    return m_last_record;
  }
  /// Sets the last SST2 transaction handle
  inline void set_last_record(sdiT::transactionHandleT last_record) { 
    m_last_record = last_record;
  }

  /// Returns the last SST2 transaction handle validity
  inline bool get_last_record_valid() const {
    return m_last_record_valid;
  }
  /// Sets the last SST2 transaction handle validity
  inline void set_last_record_valid(bool last_record_valid) { 
    m_last_record_valid = last_record_valid;
  }
#endif // TLM_TRANS_RECORD_SDI2

  /// Returns the bop field 
  inline t_bit get_bop() const {
    return m_bop;
  }
  /// Sets the bop field 
  inline void set_bop(t_bit bop) { 
    m_bop = bop;
  }

  /// Returns the eop field 
  inline t_bit get_eop() const {
    return m_eop;
  }
  /// Sets the eop field 
  inline void set_eop(t_bit eop) { 
    m_eop = eop;
  }

  /// Returns the mtype field
  inline anoc_mtype get_mtype() const {
    return m_mtype;
  }
  /// Sets the mtype field
  inline void set_mtype(anoc_mtype mtype) {
    m_mtype = mtype;
  }

  /// Returns the ANOC cmd (credit, data, ...)
  inline anoc_cmd get_cmd() const {
    return m_cmd;
  }
  /// Sets the ANOC cmd (credit, data, ...)
  inline void set_cmd(anoc_cmd cmd) { 
    m_cmd = cmd;
  }

  /// Returns the param1 field 
  inline t_uint16 get_param0() const {
    return m_param0;
  }
  /// Sets the param1 field 
  inline void set_param0(t_uint16 param0) { 
    m_param0 = param0;
  }

  /// Returns the param1 field 
  inline t_uint16 get_param1() const {
    return m_param1;
  }
  /// Sets the param1 field 
  inline void set_param1(t_uint16 param1) { 
    m_param1 = param1;
  }

  /// Returns the param2 field 
  inline t_uint16 get_param2() const {
    return (m_param2);
  }
  /// Sets the param2 field 
  inline void set_param2(t_uint16 param2) { 
    m_param2 = param2;
  }

  /// Returns the srcid field 
  inline int get_srcid() const {
    return (m_srcid);
  }
  /// Sets the srcid field 
  inline void set_srcid(int srcid) { 
    m_srcid = srcid;
  }

  /// Returns the dstid field 
  inline int get_dstid() const {
    return (m_dstid);
  }
  /// Sets the dstid field 
  inline void set_dstid(int dstid) { 
    m_dstid = dstid;
  }

  /// Returns the path_to_target field 
  inline const anoc_dir *get_path_to_target() const {
    return m_path_to_target;
  }
  /// Sets the path_to_target field 
  inline void set_path_to_target(const anoc_dir * path_to_target) { 
    if (path_to_target) {
      for (int i = 0; i < ANOC_PATH_LENGTH; i++) {
        m_path_to_target[i] = path_to_target[i];
      }
    }
  }

  /// Returns the data field 
  inline t_uint32 get_data() const {
    return m_data;
  }
  /// Sets the data field 
  inline void set_data(t_uint32 data) { 
    m_data = data;
  }

  /// Returns the data field 
  inline sc_uint<36> get_data_36() const {
    return m_data_36;
  }
  /// Sets the data field 
  inline void set_data_36(sc_uint<36> data) { 
    m_data_36 = data;
  }

  //------------------------------------------
  // shift path : 
  //   - for a data type transaction, when the flit is a header,
  //     this function shifts the path
  //------------------------------------------
  void shift_path();


  // Conversion to sc_lv<34>
  void to_bits(sc_lv<34>& word) const;

  // Conversion from sc_lv<34>
  void set_from_bits(sc_lv<34>& word);

  //------------------------------------------
  // copy operator : 
  //   - allows to copy an DATA transaction content, field by field, like:
  //     data_tr_data_2 = data_tr_data_1;
  //------------------------------------------
  anoc_data_transaction& operator = (const anoc_data_transaction& data);

  // Trace transaction
  void trace_in_file(std::ofstream *data_stream) const;

};


// ---------
// package functions
// ---------


// specialized sc_trace() function for 'anoc_accept_transaction'
// and 'anoc_data_transaction' types
extern void sc_trace(sc_trace_file* tf, const anoc_accept_transaction& transaction, const std::string& NAME);
extern void sc_trace(sc_trace_file* tf, const anoc_data_transaction& transaction, const std::string& NAME);


// specialized io stream function for 'anoc_dir' type
inline ostream& operator<< (ostream& os, const anoc_dir& dir) {
  switch (dir) {
  case NORTH : os << "NORTH"; break;
  case EAST  : os << "EAST"; break;
  case WEST  : os << "WEST"; break;
  case SOUTH : os << "SOUTH"; break;
  case RES   : os << "RES"; break;
  }
  return os;
}

inline anoc_dir& operator>> (istream& is, anoc_dir& dir) {
  int tmp;

  is >> tmp; 
  switch (tmp) {
  case 0 : dir = NORTH; break;
  case 1 : dir = EAST; break;
  case 2 : dir = SOUTH; break;
  case 3 : dir = WEST; break;
  case 4 : dir = RES; break;
  }
  return dir;
}

// specialized io stream function for 'anoc_cmd' type
inline ostream& operator<< (ostream& os, const anoc_cmd& cmd) {
  switch (cmd) {
  case CREDIT      : os << "CREDIT"      ; break;
  case DATA        : os << "DATA"        ; break;
  case DUMP_REQ    : os << "DUMP_REQ"    ; break;
  case DUMP_RESP   : os << "DUMP_RESP"   ; break;
  case IT          : os << "IT"          ; break;
  case SESSION_REQ : os << "SESSION_REQ" ; break;
  case TASK_SEL    : os << "TASK_SEL"    ; break;
  case REQ_MOVE    : os << "REQ_MOVE"    ; break;
  case DENY_MOVE   : os << "DENY_MOVE"   ; break;
  case TR_END_MOVE : os << "TR_END_MOVE" ; break;
  case MOVE        : os << "MOVE"        ; break;
  case TEST        : os << "TEST"        ; break;
  }
  return (os);
}

// specialized io stream function for 'anoc_mtype' type
inline ostream& operator<< (ostream& os, const anoc_mtype& type) {
  switch (type) {
  case USUAL0 : os << "USUAL0" ; break;
  case EOM    : os << "EOM"    ; break;
  case BOM    : os << "BOM"    ; break;
  case USUAL1 : os << "USUAL1" ; break;
  }
  return (os);
}

// specialized io stream function for 'anoc_accept_transaction'
// and 'anoc_data_transaction' types
inline ostream& operator<< (ostream& os, const anoc_accept_transaction& trans) {
  os << "ACCEPT transaction, channel " << trans.get_channel()/* << ", accept = " << trans.get_accept()*/ << ", target port ID = " << trans.get_target_port_id() << endl;
  return os;
}

inline ostream& operator<< (ostream& os, const anoc_data_transaction& trans) {
  char tmp[10];
  sc_lv <34> bits;
  trans.to_bits(bits);

  os << "DATA transaction, channel " << trans.get_channel() << ", target port ID = " << trans.get_target_port_id() << endl;
  os << ".bop     = " << trans.get_bop() << endl;
  os << ".eop     = " << trans.get_eop() << endl;
  if (trans.get_bop()) {
    os << ".mtype   = " << trans.get_mtype() << endl;
    os << ".srcid   = " << trans.get_srcid() << endl;
    os << ".dstid   = " << trans.get_dstid() << endl;
    os << ".path[0] = " << trans.get_path() << endl;
    os << ".cmd     = " << trans.get_cmd() << endl;
    os << ".param0  = " << trans.get_param0() << endl;
    os << ".param1  = " << trans.get_param1() << endl;
    os << ".param2  = " << trans.get_param2() << endl;
    os << ".data    = 0x" << hex << bits.range(31,0).to_uint() << dec << endl;
  } else {
    sprintf(tmp,"0x%X",trans.get_data());
    os << ".data    = " << (char*)tmp << endl;
  }
  return os;
}


#endif /* _ANOC_TRANSACTION_H_ */
