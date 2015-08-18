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
#ifndef SOCLIB_TLMT_VCI_VGMN_H
#define SOCLIB_TLMT_VCI_VGMN_H

#include <tlmt>
#include <vector>
#include <sstream>

#include "mapping_table.h"
#include "vci_cmd_arb_rsp_rout.h"
#include "vci_rsp_arb_cmd_rout.h"
#include "pointer_proxy.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
class VciVgmn
{
public:
    std::vector<typename soclib::tlmt::VciCmdArbRspRout<vci_param> *> m_CmdArbRspRout;
    std::vector<typename soclib::tlmt::VciRspArbCmdRout<vci_param> *> m_RspArbCmdRout;

	soclib::common::PointerProxy<soclib::tlmt::VciInitiator<vci_param> > p_to_target;
	soclib::common::PointerProxy<soclib::tlmt::VciTarget<vci_param> > p_to_initiator;

    VciVgmn(int nb_init,
			int nb_target,
			const soclib::common::MappingTable &mt,
			tlmt_core::tlmt_time delay);


};


}}

#endif
