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

#ifndef __VCI_TARGET_TRANSACTOR_H__ 
#define __VCI_TARGET_TRANSACTOR_H__

#include <tlmdt>	   	             // TLM-DT headers
#include "vci_target.h"
#include "vci_target_transactor_buffer.h"

namespace soclib { namespace tlmdt {

template<typename vci_param_caba, typename vci_param_tlmdt>
class VciTargetTransactor                        
  : public sc_core::sc_module               // inherit from SC module base class
  , virtual public tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> // inherit from TLM "forward interface"
{
 private:
  pdes_local_time                   *m_pdes_local_time; // local time

  sc_core::sc_event                  m_rsp_event;   // response event

  unsigned int                       m_clock_count;

  //variables of CMD fsm
  int                                m_cmd_index;
  bool                               has_cmd_transaction;
  int                                m_cmd_count;
  int                                m_cmd_nbytes;

  //variables of RSP fsm
  int                                m_rsp_index;
  bool                               has_rsp_transaction;
  bool                               is_rsp_eop;
  int                                m_rsp_count;
  int                                m_rsp_nwords;

  //IRQ member variables
  unsigned int                       m_nirq;
  std::map<sc_core::sc_time, std::pair<int, bool> > m_pending_irqs;

  //pending transaction buffer
  vci_target_transactor_buffer<vci_param_caba, vci_param_tlmdt >  m_buffer;

  //null message variables
  tlm::tlm_generic_payload          *m_null_payload_ptr;
  soclib_payload_extension          *m_null_extension_ptr;
  tlm::tlm_phase                     m_null_phase;
  sc_core::sc_time                   m_null_time;

  /////////////////////////////////////////////////////////////////////////////////////
  // Fuctions
  /////////////////////////////////////////////////////////////////////////////////////
  void init();
  void cmd();
  void rsp();
  void send_null_message();
  void interruption();
  void print_transaction(bool fw, tlm::tlm_generic_payload &payload);

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_fw_transport_if (VCI INITIATOR SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum nb_transport_bw        // receive response from target
  ( tlm::tlm_generic_payload &payload,      // payload
    tlm::tlm_phase           &phase,        // phase
    sc_core::sc_time         &time);        // time

  void invalidate_direct_mem_ptr           // invalidate_direct_mem_ptr
  ( sc_dt::uint64 start_range,             // start range
    sc_dt::uint64 end_range);              // end range

  /////////////////////////////////////////////////////////////////////////////////////
  // Virtual Fuctions  tlm::tlm_fw_transport_if (IRQ TARGET SOCKET)
  /////////////////////////////////////////////////////////////////////////////////////
  tlm::tlm_sync_enum irq_nb_transport_fw
  ( int                      id,         // interruption id
    tlm::tlm_generic_payload &payload,   // payload
    tlm::tlm_phase           &phase,     // phase
    sc_core::sc_time         &time);     // time

 protected:
  SC_HAS_PROCESS(VciTargetTransactor);
 public:

  // PORTS CABA 
  sc_core::sc_in<bool>                                       p_clk;
  sc_core::sc_in<bool>                                       p_resetn;
  sc_core::sc_out<bool>                                     *p_irq_initiator;

  // PORTS TLMDT
  soclib::caba::VciTarget<vci_param_caba>                    p_vci_target;    // CABA -> TLMDT
  tlm::tlm_initiator_socket<32,tlm::tlm_base_protocol_types> p_vci_initiator; // TLMDT -> INTERCONNECT
  std::vector<tlm_utils::simple_target_socket_tagged<VciTargetTransactor,32,tlm::tlm_base_protocol_types> *> p_irq_target;  // IRQ target port

  VciTargetTransactor(sc_core::sc_module_name name);

  VciTargetTransactor(sc_core::sc_module_name name, size_t n_irq);
  
  ~VciTargetTransactor();

};
}}
#endif
