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


#ifndef TLMT_CORE_TLMT_OUT_H
#define TLMT_CORE_TLMT_OUT_H

#include <iostream>
#include <systemc>
#include "tlmt_thread_context.h"
#include "tlmt_if.h"

namespace tlmt_core {

template<typename data_t>
class tlmt_out
	: public sc_core::sc_port<tlmt_if<data_t>,0>
{
	tlmt_thread_context *m_thread_context;

public:
	tlmt_out(const std::string &name, tlmt_thread_context *opt_ref = NULL)
		: sc_core::sc_port<tlmt_if<data_t>,0>(name.c_str()),
		  m_thread_context(opt_ref)
	{
		// std::cout << "tlmt_out " << name << " ctor" << std::endl;
	}

	~tlmt_out()
	{}

	void send(data_t data, const tlmt_time &time)
	{
	  (*this)->receive(data, time);
	}

	tlmt_thread_context *get_thread_context() const
	{
		return m_thread_context;
	}
};

}

#endif

