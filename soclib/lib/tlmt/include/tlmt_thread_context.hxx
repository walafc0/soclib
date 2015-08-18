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

#ifndef TLMT_CORE_TLMT_THREAD_CONTEXT_HXX
#define TLMT_CORE_TLMT_THREAD_CONTEXT_HXX

#include "tlmt_thread_context.h"

namespace tlmt_core {

const tlmt_time &tlmt_thread_context::time() const
{
	return m_time;
}

const bool tlmt_thread_context::active() const
{
	return m_active;
}

const bool tlmt_thread_context::sending() const
{
	return m_sending;
}

void tlmt_thread_context::enable()
{
	m_active = true;
}

void tlmt_thread_context::disable()
{
	m_active = false;
}

void tlmt_thread_context::start_sending()
{
	m_sending = true;
}

void tlmt_thread_context::stop_sending()
{
	m_sending = false;
}

void tlmt_thread_context::add_time( const tlmt_time &offset )
{
	m_time += offset;
}

void tlmt_thread_context::set_time( const tlmt_time &t )
{
        m_time = t;
}

void tlmt_thread_context::update_time( const tlmt_time &t )
{
	if (t>m_time)
        	m_time = t;
}

}

#endif

