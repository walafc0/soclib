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


#ifndef _TLMT_CORE_TLMT_IF_H_
#define _TLMT_CORE_TLMT_IF_H_

#include <systemc>
#include <cassert>
#include "tlmt_thread_context.h"
#include "tlmt_callback.h"
#include "tlmt_module.h"

namespace tlmt_core {

template<typename data_t>
class tlmt_if
	: virtual public sc_core::sc_interface
{
public:
	typedef tlmt_callback_base<data_t> *callback_t;
private:
	callback_t m_cb;
	tlmt_thread_context *m_thread_context;

public:  

	tlmt_if(callback_t cb)
		: m_cb(cb), m_thread_context(NULL)
	{}

	virtual ~tlmt_if()
	{}

	inline void receive(data_t cmd, const tlmt_time &time);

    virtual void register_port(
		sc_core::sc_port_base& port_,
		const char*    if_typename_ );

	inline const tlmt_time get_thread_time() const;

	inline const bool get_thread_active() const;

	inline const bool get_thread_sending() const;
};

}

#endif /* _TLMT_CORE_TLMT_IF_H_ */


