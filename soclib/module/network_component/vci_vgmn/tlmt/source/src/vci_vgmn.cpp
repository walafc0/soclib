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

#include "../include/vci_vgmn.h"

namespace soclib { namespace tlmt {

#define tmpl(x) template<typename vci_param> x VciVgmn<vci_param>

tmpl(/**/)::VciVgmn(
	int nb_init,
	int nb_target,
	const soclib::common::MappingTable &mt,
	tlmt_core::tlmt_time delay )
		   : p_to_target(nb_target),
		   p_to_initiator(nb_init)
{
	// Phase 1, allocate nb_target CmdArbRspRout blocks
	for (int i=0;i<nb_target;i++)
	{
		std::ostringstream tmpName;
		tmpName << "VgmnCmdArbRspRout" << i;
		m_CmdArbRspRout.push_back(new VciCmdArbRspRout<vci_param>(tmpName.str().c_str(),i,nb_init,delay));
	}
	
	// Phase 2, allocate nb_init RspArbCmdRout blocks
	for (int i=0;i<nb_init;i++)
	{
		std::ostringstream tmpName;
		tmpName << "VgmnRspArbCmdRout" << i;
		m_RspArbCmdRout.push_back(new VciRspArbCmdRout<vci_param>(tmpName.str().c_str(),mt,i,delay));
	}

	// Phase 3, each cmdArbRspRout sees all the RspArbCmdRout
	for (int i=0;i<nb_target;i++)
	{
		p_to_target.set(i, &m_CmdArbRspRout[i]->p_vci),
		m_CmdArbRspRout[i]->setRspArbCmdRout(m_RspArbCmdRout);
	}

	// Phase 4, each rspArbCmdRout sees all the CmdArbRspRout
	for (int i=0;i<nb_init;i++)
	{
		p_to_initiator.set(i, &m_RspArbCmdRout[i]->p_vci);
		m_RspArbCmdRout[i]->setCmdArbRspRout(m_CmdArbRspRout);
	}
}

}}
