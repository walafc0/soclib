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
 * Maintainers: alinev
 *
 * Copyright (c) UPMC / Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef VCI_INITIATOR_TRANSACTOR_BUFFER_H
#define VCI_INITIATOR_TRANSACTOR_BUFFER_H

#include <tlmdt>	             // TLM-DT headers

#define TAM_BUFFER 100

namespace soclib { namespace tlmdt {

class vci_initiator_transactor_buffer
{
private:

  enum transaction_buffer_status {
    EMPTY     = 0,
    OPEN      = 1,
    COMPLETED = 2,
  };

  struct transaction_buffer{
    transaction_buffer_status  status;
    tlm::tlm_generic_payload  *payload;
    tlm::tlm_phase            *phase;
    sc_core::sc_time          *time;
  };

  int                       m_nentries;
  int                       m_header_ptr;
  int                       m_cmd_ptr;
  int                       m_rsp_ptr;
  transaction_buffer       *m_table;

public:

  vci_initiator_transactor_buffer(int n);

  vci_initiator_transactor_buffer();

  ~vci_initiator_transactor_buffer();

  void init();

  bool push
  ( tlm::tlm_generic_payload &payload,
    tlm::tlm_phase           &phase,
    sc_core::sc_time         &time);

  bool get_cmd_payload
  ( unsigned int               local_time,
    tlm::tlm_generic_payload *&payload,
    tlm::tlm_phase           *&phase,
    sc_core::sc_time         *&time );

  int get_rsp_payload
  ( unsigned int               src_id,
    unsigned int               trd_id,
    tlm::tlm_generic_payload *&payload,
    tlm::tlm_phase           *&phase,
    sc_core::sc_time         *&time );
    
  bool pop( int idx );

  bool waiting_response();

};
}}
#endif
