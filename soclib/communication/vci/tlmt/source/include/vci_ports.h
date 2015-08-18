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

#ifndef VCI_TARGET_PORT_H
#define VCI_TARGET_PORT_H

#include <tlmt>
#include "vci_packets.h"

namespace soclib { namespace tlmt {

template<typename vci_param>
class VciInitiator;

template<typename vci_param>
class VciTarget
{
	friend class VciInitiator<vci_param>;
public:
	typedef tlmt_core::tlmt_callback_base<vci_cmd_packet<vci_param>*> *callback_t;

private:
	tlmt_core::tlmt_out<vci_rsp_packet<vci_param>*> rsp_out;
	tlmt_core::tlmt_in<vci_cmd_packet<vci_param>*> cmd_in;

public:
	VciTarget(const std::string &name, callback_t cb, tlmt_core::tlmt_thread_context *opt_ref = NULL)
		: rsp_out(name+"_rsp_out", opt_ref),
		  cmd_in(name+"_cmd_in", cb)
	{}

	inline void send(vci_rsp_packet<vci_param> *pkt, const tlmt_core::tlmt_time &time)
	{
		rsp_out.send(pkt, time);
	}

	inline void operator()( VciInitiator<vci_param> &peer );

	inline tlmt_core::tlmt_time peer_time() const
	{
		return cmd_in.peer_time();
	}

	inline bool peer_active() const
	{
		return cmd_in.peer_active();
	}

	inline bool peer_sending() const
	{
		return cmd_in.peer_sending();
	}
};

template<typename vci_param>
class VciInitiator
{
public:
	typedef tlmt_core::tlmt_callback_base<vci_rsp_packet<vci_param>*> *callback_t;

private:
	tlmt_core::tlmt_out<vci_cmd_packet<vci_param>*> cmd_out;
	tlmt_core::tlmt_in<vci_rsp_packet<vci_param>*> rsp_in;

public:
	VciInitiator(const std::string &name, callback_t cb, tlmt_core::tlmt_thread_context *opt_ref = NULL)
		: cmd_out(name+"_cmd_out", opt_ref),
		  rsp_in(name+"_rsp_in", cb)
	{}

	inline void send(vci_cmd_packet<vci_param> *pkt, const tlmt_core::tlmt_time &time)
	{
		cmd_out.send(pkt, time);
	}

	inline void operator()( VciTarget<vci_param> &peer );

	inline const tlmt_core::tlmt_time &peer_time() const
	{
		return rsp_in.peer_time();
	}

	inline bool peer_active() const
	{
		return rsp_in.peer_active();
	}

	inline bool peer_sending() const
	{
		return rsp_in.peer_sending();
	}
};

template<typename vci_param>
void VciTarget<vci_param>::operator()( VciInitiator<vci_param> &peer )
{
	peer(*this);
}

template<typename vci_param>
void VciInitiator<vci_param>::operator()( VciTarget<vci_param> &peer )
{
	peer.rsp_out(rsp_in);
	cmd_out(peer.cmd_in);
}

}}

#endif
