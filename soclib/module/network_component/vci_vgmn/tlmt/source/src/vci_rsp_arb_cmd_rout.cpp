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
 *     Nicolas Pouillon <nipo@ssji.net>
 */

#include "../include/vci_rsp_arb_cmd_rout.h"
#include "../include/vci_cmd_arb_rsp_rout.h"

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciRspArbCmdRout<vci_param>

tmpl(void)::callback(soclib::tlmt::vci_cmd_packet<vci_param> *pkt,
		     const tlmt_core::tlmt_time &time,
		     void *private_data)
{
	// std::cout << "VciRspArbCmdRout::callback" << std::endl;

	uint32_t address = pkt->address;
	//printf("address=%8.8x\n",address);
	unsigned int dest_index = m_routing_table[address];

	//printf("dest_index=%u\n",dest_index);
	if ( dest_index >= 0 && dest_index<m_CmdArbRspRout.size()) {
	  start_sending();
	  m_dest = dest_index;
	  m_CmdArbRspRout[dest_index]->put(pkt,m_index,time+m_delay);
	} else {
	  std::cout << "Erreur d'adressage " <<  address  << std::endl;
	}
}

tmpl(void)::setCmdArbRspRout(std::vector<typename soclib::tlmt::VciCmdArbRspRout<vci_param> *> &CmdArbRspRout)
{
	m_CmdArbRspRout=CmdArbRspRout;
}

tmpl(tlmt_core::tlmt_time)::getCmdTime()
{
	return m_CmdArbRspRout[m_dest]->getTime();
}

tmpl(bool)::is_sending()
{
	return m_sending;
}

tmpl(void)::start_sending()
{
	m_sending = true;
}

tmpl(void)::stop_sending()
{
	m_sending = false;
}

tmpl(/**/)::VciRspArbCmdRout( sc_core::sc_module_name name,
							  const soclib::common::MappingTable &mt,
							  uint32_t idx,
							  tlmt_core::tlmt_time dl )
		   : BaseModule(name),
		   m_routing_table(mt.getRoutingTable(soclib::common::IntTab(), -1)),
		   m_index(idx),
		   m_delay(dl),
		   p_vci("vci", new tlmt_core::tlmt_callback<VciRspArbCmdRout,soclib::tlmt::vci_cmd_packet<vci_param> *>(
				   this, &VciRspArbCmdRout<vci_param>::callback), &c0)
{
  m_sending = false;
  m_dest = 0;
}

}}
