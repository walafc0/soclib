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


#ifndef TLMT_CORE_TLMT_IF_HXX
#define TLMT_CORE_TLMT_IF_HXX

#include "tlmt_if.h"
#include "tlmt_out"
#include "tlmt_thread_context"

namespace tlmt_core {

#define tmpl(...) template<typename data_t> __VA_ARGS__ tlmt_if<data_t>

tmpl(void)::receive(data_t cmd, const tlmt_time &time)
{
  (*m_cb)(cmd, time);
}

tmpl(const tlmt_time)::get_thread_time() const
{
	assert ( m_thread_context != NULL && "Asking time on a port without thread_context-aware interface" );
	return m_thread_context->time();
}

tmpl(const bool)::get_thread_active() const
{
	assert ( m_thread_context != NULL && "Asking activity on a port without thread_context-aware interface" );
	return m_thread_context->active();
}

tmpl(const bool)::get_thread_sending() const
{
	assert ( m_thread_context != NULL && "Asking sending state on a port without thread_context-aware interface" );
	return m_thread_context->sending();
}

/*
 * On end of elaboration, register_port() gets called for every port
 * associated to the interface. Thus we get the useful information in
 * peer tlmt_out ports.
 */
tmpl(void)::register_port(
	sc_core::sc_port_base& port_,
	const char*    if_typename_
	)
{
	if ( tlmt_out<data_t> *port = dynamic_cast<tlmt_out<data_t>* >(&port_) )
		if ( tlmt_thread_context *c = port->get_thread_context() )
			m_thread_context = c;
}

}

#undef tmpl

#endif

