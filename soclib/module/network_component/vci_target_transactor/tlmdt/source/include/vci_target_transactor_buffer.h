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
 * Maintainers: alinevieiramello@hotmail.com
 *
 * Copyright (c) UPMC / Lip6, 2010
 *     Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr>
 */

#ifndef VCI_TARGET_TRANSACTOR_BUFFER_H
#define VCI_TARGET_TRANSACTOR_BUFFER_H

#include <tlmdt>	             // TLM-DT headers

#define TAM_BUFFER 100
#define MAX_SIZE_PACKET 100

namespace soclib { namespace tlmdt {

//---------------------------------------------------------------------------
// enumeration types
//---------------------------------------------------------------------------

struct transaction_index_struct{
  int                             status;
  tlm::tlm_generic_payload       *payload;
  tlm::tlm_phase                  phase;
  sc_core::sc_time                time;
  sc_core::sc_time                initial_time;  // time of the first word transmission
};

template<typename vci_param_caba, typename vci_param_tlmdt>
class vci_target_transactor_buffer
{
private:
  transaction_index_struct *m_buffer;
  int                       m_nentries;
  int                       m_header_ptr;
  int                       m_rsp_ptr;

  void init();

public:

  enum buffer_status {
    EMPTY     = 0,
    OPEN      = 1,
    COMPLETED = 2,
  };

  vci_target_transactor_buffer();

  vci_target_transactor_buffer(int n);

  ~vci_target_transactor_buffer();

  void set_status(int idx, int status);

  void set_command(int idx, tlm::tlm_command cmd);

  void set_address(int idx, sc_dt::uint64 add);

  void set_ext_command(int idx, enum command cmd);

  void set_src_id(int idx, unsigned int src);

  void set_trd_id(int idx, unsigned int trd);

  void set_pkt_id(int idx, unsigned int pkt);

  void set_data(int idx, int idx_data, typename vci_param_caba::data_t int_data);

  void set_data_length(int idx, unsigned int length);

  void set_byte_enable(int idx, int idx_be, typename vci_param_caba::data_t int_be);

  void set_byte_enable_length(int idx, unsigned int length);

  void set_phase(int idx, tlm::tlm_phase phase);

  void set_time(int idx, sc_core::sc_time time);

  void set_response(tlm::tlm_generic_payload &payload, sc_core::sc_time &time);

  unsigned int get_src_id(int idx);

  unsigned int get_pkt_id(int idx);

  unsigned int get_trd_id(int idx);

  enum command get_ext_command(int idx);

  bool is_write_command(int idx);

  bool is_read_command(int idx);

  bool get_response_status(int idx);

  sc_dt::uint64 get_address(int idx);

  typename vci_param_caba::data_t get_data(int idx, int idx_data);

  unsigned int get_data_length(int idx);

  unsigned int get_nwords(int idx);

  tlm::tlm_generic_payload* get_payload(int idx);

  tlm::tlm_phase* get_phase(int idx);

  sc_core::sc_time* get_time(int idx);

  unsigned int get_time_value(int idx);

  unsigned int get_initial_time_value(int idx);

  unsigned int select_response(int clock_count);

  int get_empty_position();

  int get_index( unsigned int src_id, unsigned int trd_id);

  bool set_empty_position(int idx);
};

}}

#endif
