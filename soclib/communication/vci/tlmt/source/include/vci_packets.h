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
 * Maintainers: fpecheux, alain
 *
 * Copyright (c) UPMC / Lip6, 2008
 *     Francois Pecheux <francois.pecheux@lip6.fr>
 *     Alain Greiner <alain.greiner@lip6.fr>
 */

#ifndef VCI_PACKETS_H
#define VCI_PACKETS_H

#include "vci_param.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
class vci_cmd_packet
{
public:
	typename vci_param::cmd_t cmd;
	typename vci_param::addr_t address;
	uint32_t be;
	bool contig;
	typename vci_param::data_t *buf;
	size_t   nwords;
	uint32_t srcid;
	uint32_t trdid;
	uint32_t pktid;
};

template<typename vci_param>
class vci_rsp_packet
{
public:
	size_t   nwords;
	uint32_t error;
	uint32_t srcid;
	uint32_t trdid;
	uint32_t pktid;
};

}}

#endif
