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

#ifndef VCI_CMD_ARB_RSP_ROUT_H
#define VCI_CMD_ARB_RSP_ROUT_H

#include <tlmt>
#include "tlmt_base_module.h"
#include "mapping_table.h"
#include "vci_ports.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
class VciCmdArbRspRout;

template<typename vci_param>
class VciRspArbCmdRout
  : public soclib::tlmt::BaseModule
{
  std::vector<typename soclib::tlmt::VciCmdArbRspRout<vci_param> *> m_CmdArbRspRout;
  tlmt_core::tlmt_thread_context c0;
  const soclib::common::AddressDecodingTable<uint32_t, int> m_routing_table;
  uint32_t m_index;
  tlmt_core::tlmt_time m_delay;
  int m_dest;
  bool m_sending;

protected:
  SC_HAS_PROCESS(VciRspArbCmdRout);

public:
  soclib::tlmt::VciTarget<vci_param> p_vci;

  VciRspArbCmdRout( sc_core::sc_module_name name,
		    const soclib::common::MappingTable &mt,
		    uint32_t idx,
		    tlmt_core::tlmt_time dl );

  void callback(soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		const tlmt_core::tlmt_time &time,
		void *private_data);

  void setCmdArbRspRout(std::vector<typename soclib::tlmt::VciCmdArbRspRout<vci_param> *> &CmdArbRspRout);

  bool is_sending();

  void start_sending();

  void stop_sending();

  tlmt_core::tlmt_time getCmdTime();

};

}}

#endif
