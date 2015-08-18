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

#ifndef FIFO_TARGET_PORT_H
#define FIFO_TARGET_PORT_H

#include <tlmt>
#include "fifo_packets.h"

namespace soclib { namespace tlmt {

template<typename data_t>
class FifoInitiator;

template<typename data_t>
class FifoTarget
{
	friend class FifoInitiator<data_t>;
public:
	typedef tlmt_core::tlmt_callback_base<fifo_cmd_packet<data_t>*> *callback_t;

private:
	tlmt_core::tlmt_out<int> rsp_out; // pas de paquet réponse
	tlmt_core::tlmt_in<fifo_cmd_packet<data_t>*> cmd_in;

public:
	FifoTarget(const std::string &name, callback_t cb )
		: rsp_out(name+"_rsp_out", NULL ),
		  cmd_in(name+"_cmd_in", cb)
	{}

	void send(int pkt, const tlmt_core::tlmt_time &time)
	{
	  rsp_out.send(pkt, time);
	}
	inline void operator() (FifoInitiator<data_t> &peer ) ;
};

template<typename data_t>
class FifoInitiator
{
public:
	typedef tlmt_core::tlmt_callback_base<int> *callback_t;

private:
	tlmt_core::tlmt_out<fifo_cmd_packet<data_t>*> cmd_out;
	tlmt_core::tlmt_in<int> rsp_in;

public:
	FifoInitiator(const std::string &name, callback_t cb )
		: cmd_out(name+"_cmd_out", NULL ),
		  rsp_in(name+"_rsp_in", cb)
	{}

	void send(fifo_cmd_packet<data_t> *pkt, const tlmt_core::tlmt_time &time)
	{
	  cmd_out.send(pkt, time);
	}
	inline void operator() (FifoTarget<data_t> &peer ) ;
};

template<typename data_t>
void FifoTarget<data_t>::operator()( FifoInitiator<data_t> &peer )
{
	peer(*this);
}

template<typename data_t>
void FifoInitiator<data_t>::operator()( FifoTarget<data_t> &peer )
{
	peer.rsp_out(rsp_in);
	cmd_out(peer.cmd_in);
}

}}

#endif
