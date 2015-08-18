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
 *     François Pêcheux <francois.pecheux@lip6.fr>
 *     Nicolas Pouillon <nipo@ssji.net>
 */
#ifndef SOCLIB_TLMT_SIMPLE_INITIATOR_H
#define SOCLIB_TLMT_SIMPLE_INITIATOR_H

#include <tlmt>
#include "tlmt_base_module.h"
#include "vci_ports.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
class VciSimpleInitiator
    : public soclib::tlmt::BaseModule
{
    tlmt_core::tlmt_thread_context c0;
    sc_core::sc_event e0;

protected:
    SC_HAS_PROCESS(VciSimpleInitiator);

public:
    soclib::tlmt::VciInitiator<vci_param> p_vci;

    VciSimpleInitiator( sc_core::sc_module_name name );

    void callback(soclib::tlmt::vci_rsp_packet<vci_param> *pkt,
		  const tlmt_core::tlmt_time &time,
		  void *private_data);

    void behavior();
};

}}

#endif
