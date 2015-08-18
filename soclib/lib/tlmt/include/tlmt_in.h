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


#ifndef TLMT_CORE_TLMT_IN_H
#define TLMT_CORE_TLMT_IN_H

#include <iostream>
#include <systemc>
#include "tlmt_if.h"

namespace tlmt_core {

template<typename data_t>
class tlmt_in
	: public sc_core::sc_port<tlmt_if<data_t>,0>
{
public:
	typedef tlmt_if<data_t> interf_t;
	typedef typename interf_t::callback_t callback_t;

private:
	interf_t m_interf;

public:
	tlmt_in(const std::string &name, callback_t cb)
		: sc_core::sc_port<tlmt_if<data_t>,0>(name.c_str()),
		  m_interf(cb)
	{
		// std::cout << "tlmt_in " << name << " ctor" << std::endl;
		bind(m_interf);
	}

	~tlmt_in()
	{}

	inline const tlmt_time peer_time() const
	{
		return m_interf.get_thread_time();
	}

	inline const bool peer_active() const
	{
		return m_interf.get_thread_active();
	}

	inline const bool peer_sending() const
	{
		return m_interf.get_thread_sending();
	}
};

}

#endif
