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


#ifndef TLMT_CORE_TLMT_TIME_H
#define TLMT_CORE_TLMT_TIME_H

#include <iostream>

namespace tlmt_core {

class tlmt_time
{
public:
	typedef uint32_t val_t;

	static inline const tlmt_time max();

private:
	val_t m_value;

public:
	inline tlmt_time( val_t val = 0 );

	inline tlmt_time( const tlmt_time &ref );

	inline ~tlmt_time();

	inline operator val_t() const;

	inline tlmt_time &operator=(const tlmt_time &ref);

	inline tlmt_time operator+( const tlmt_time &t ) const;

	inline tlmt_time operator-( const tlmt_time &t ) const;

	inline tlmt_time &operator+=( const tlmt_time &t );

	inline tlmt_time &operator-=( const tlmt_time &t );

	inline bool operator<( const tlmt_time &t ) const;

	inline bool operator>( const tlmt_time &t ) const;

	inline bool operator<=( const tlmt_time &t ) const;

	inline bool operator>=( const tlmt_time &t ) const;

	inline bool operator==( const tlmt_time &t ) const;

	inline bool operator!=( const tlmt_time &t ) const;

	inline friend std::ostream &operator<<(std::ostream &o, const tlmt_time &t)
	{
		o << t.m_value;
		return o;
	}
};

}

#endif

