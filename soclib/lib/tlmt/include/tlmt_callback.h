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

#ifndef TLMT_CORE_TLMT_CALLBACK_H
#define TLMT_CORE_TLMT_CALLBACK_H

namespace tlmt_core {

template<typename param_t>
class tlmt_callback_base
{
public:
	virtual ~tlmt_callback_base() {}
	virtual void operator()(param_t param, const tlmt_time &) = 0;
};

template<typename class_t, typename param_t>
class tlmt_callback
	: public tlmt_callback_base<param_t>
{
public:
    typedef void (class_t::*func_t)(param_t, const tlmt_time &, void *);

private:
	class_t* m_inst;
	func_t m_func;
	void *m_data;

public:
    tlmt_callback(class_t* inst, func_t func, void* data=NULL)
		: m_inst(inst), m_func(func), m_data(data)
    {}

	~tlmt_callback()
	{}

    void operator()(param_t param, const tlmt_time &time)
    {
		(m_inst->*m_func)(param, time, m_data);
    }
};

}

#endif

