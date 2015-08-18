/* -*- mode: c++; coding: utf-8 -*-
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
 * Maintainers: fpecheux, nipo
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     Francois Pecheux <francois.pecheux@lip6.fr>
 *     Emmanuel Viaud <emmanuel.viaud@lip6.fr>
 */

#ifndef VCI_RSP_ARB_CMD_ROUT_H
#define VCI_RSP_ARB_CMD_ROUT_H

#include <tlmt>
#include "tlmt_base_module.h"
#include "vci_ports.h"

template<typename vci_param>
struct fifo_struct{
  bool event;
  typename soclib::tlmt::vci_cmd_packet<vci_param> *pkt;
  tlmt_core::tlmt_time time;
};

namespace soclib { namespace tlmt {

template<typename vci_param>
class VciRspArbCmdRout;

template<typename vci_param>
class VciCmdArbRspRout
  : public soclib::tlmt::BaseModule
{
  tlmt_core::tlmt_thread_context c0;
  sc_core::sc_event e0;
  std::vector<typename soclib::tlmt::VciRspArbCmdRout<vci_param> *> m_RspArbCmdRout;
  uint32_t m_index;
  uint32_t m_nbinit;
  size_t m_selected_port;
  tlmt_core::tlmt_time m_delay;
  fifo_struct<vci_param> *fifos;
  
protected:
  SC_HAS_PROCESS(VciCmdArbRspRout);

public:
  soclib::tlmt::VciInitiator<vci_param> p_vci;

  VciCmdArbRspRout( sc_core::sc_module_name name,
		    uint32_t idx,
		    uint32_t nb_init,
		    tlmt_core::tlmt_time dl );

  void callback(soclib::tlmt::vci_rsp_packet<vci_param> *pkt,
		const tlmt_core::tlmt_time &time,
		void *private_data);

  void behavior();
  
  void setRspArbCmdRout(std::vector<typename soclib::tlmt::VciRspArbCmdRout<vci_param> *> &RspArbCmdRout);
  
  void put(soclib::tlmt::vci_cmd_packet<vci_param> *pkt, uint32_t idx, const tlmt_core::tlmt_time &time);
  
  int through_fifo();

  tlmt_core::tlmt_time getTime();

};
  
}}

#endif
