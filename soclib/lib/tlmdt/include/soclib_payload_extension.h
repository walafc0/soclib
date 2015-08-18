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
 * Maintainers: fpecheux, alinev
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef __SOCLIB_PAYLOAD_EXTENSION_H__
#define __SOCLIB_PAYLOAD_EXTENSION_H__

#include <tlmdt>                              // TLM-DT header

//---------------------------------------------------------------------------
// enumeration types
//---------------------------------------------------------------------------
enum command {
  VCI_READ_COMMAND = 0,
  VCI_WRITE_COMMAND = 1,
  VCI_LINKED_READ_COMMAND = 2,
  VCI_STORE_COND_COMMAND = 3,
  PDES_NULL_MESSAGE = 4,
  PDES_ACTIVE = 5,
  PDES_INACTIVE = 6,
  PDES_TOKEN_MESSAGE = 7,
  PDES_ROUNDTRIP_LATENCY = 8
};

class soclib_payload_extension                        // extension class 
  : public tlm::tlm_extension<soclib_payload_extension> // tlm extension
{
public:
  
  soclib_payload_extension                            // constructor
  ( void )
  {
  }
   
  ~soclib_payload_extension                           // destructor
  ( void )
  {
  }

  void
  copy_from                                    // copy_from operation
  ( const tlm_extension_base &extension
    )
  {
    m_src_id = static_cast<soclib_payload_extension const &>(extension).m_src_id;
  }

  tlm::tlm_extension_base*
  clone                                        // clone operation
  ( void
    ) const
  {
    return new soclib_payload_extension ( *this );
  }


  // Command related methods
  bool         is_read() const {return (m_soclib_command == VCI_READ_COMMAND);}
  void         set_read() {m_soclib_command = VCI_READ_COMMAND;}
  bool         is_write() const {return (m_soclib_command == VCI_WRITE_COMMAND);}
  void         set_write() {m_soclib_command = VCI_WRITE_COMMAND;}
  bool         is_locked_read() const {return (m_soclib_command == VCI_LINKED_READ_COMMAND);}
  void         set_locked_read() {m_soclib_command = VCI_LINKED_READ_COMMAND;}
  bool         is_store_cond() const {return (m_soclib_command == VCI_STORE_COND_COMMAND);}
  void         set_store_cond() {m_soclib_command = VCI_STORE_COND_COMMAND;}
  bool         is_null_message() const {return (m_soclib_command == PDES_NULL_MESSAGE);}
  void         set_null_message() {m_soclib_command = PDES_NULL_MESSAGE;}
  bool         is_active() const {return (m_soclib_command == PDES_ACTIVE);}
  void         set_active() {m_soclib_command = PDES_ACTIVE;}
  bool         is_inactive() const {return (m_soclib_command == PDES_INACTIVE);}
  void         set_inactive() {m_soclib_command = PDES_INACTIVE;}
  bool         is_token_message() const {return (m_soclib_command == PDES_TOKEN_MESSAGE);}
  void         set_token_message() {m_soclib_command = PDES_TOKEN_MESSAGE;}
  bool         is_roundtrip() const {return (m_soclib_command == PDES_ROUNDTRIP_LATENCY);}
  void         set_roundtrip() {m_soclib_command = PDES_ROUNDTRIP_LATENCY;}
  enum command get_command() const {return m_soclib_command;}
  void         set_command(const enum command c) {m_soclib_command = c;}

  // identification related methods
  unsigned int get_src_id() const {return m_src_id;}
  void         set_src_id(unsigned int id) {m_src_id = id;}
  unsigned int get_trd_id() const {return m_trd_id;}
  void         set_trd_id(unsigned int id) {m_trd_id = id;}
  unsigned int get_pkt_id() const {return m_pkt_id;}
  void         set_pkt_id(unsigned int id) {m_pkt_id = id;}

 private:
  // member variables
  enum command           m_soclib_command;
  unsigned int           m_src_id;  
  unsigned int           m_trd_id;  
  unsigned int           m_pkt_id;  
};

#endif /* __SOCLIB_PAYLOAD_EXTENSION_H__ */
