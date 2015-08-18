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

#include <limits>

#ifndef TLMT_CORE_TLMT_TIME_HXX
#define TLMT_CORE_TLMT_TIME_HXX

namespace tlmt_core {

tlmt_time::tlmt_time( val_t val )
	: m_value(val)
{}

tlmt_time::tlmt_time( const tlmt_time &ref )
	: m_value(ref.m_value)
{}

tlmt_time::~tlmt_time()
{}

tlmt_time::operator val_t() const
{
	return m_value;
}

tlmt_time &tlmt_time::operator=(const tlmt_time &ref)
{
	m_value = ref.m_value;
	return *this;
}

tlmt_time tlmt_time::operator+( const tlmt_time &t ) const
{
	return tlmt_time(m_value+t.m_value);
}

tlmt_time tlmt_time::operator-( const tlmt_time &t ) const
{
	return tlmt_time(m_value-t.m_value);
}

tlmt_time &tlmt_time::operator+=( const tlmt_time &t )
{
	m_value += t.m_value;
	return *this;
}

tlmt_time &tlmt_time::operator-=( const tlmt_time &t )
{
	m_value -= t.m_value;
	return *this;
}

bool tlmt_time::operator<( const tlmt_time &t ) const
{
	return m_value < t.m_value;
}

bool tlmt_time::operator>( const tlmt_time &t ) const
{
	return m_value > t.m_value;
}

bool tlmt_time::operator<=( const tlmt_time &t ) const
{
	return m_value <= t.m_value;
}

bool tlmt_time::operator>=( const tlmt_time &t ) const
{
	return m_value >= t.m_value;
}

bool tlmt_time::operator==( const tlmt_time &t ) const
{
	return m_value == t.m_value;
}

bool tlmt_time::operator!=( const tlmt_time &t ) const
{
	return m_value != t.m_value;
}

const tlmt_time tlmt_time::max()
{
	return tlmt_time(std::numeric_limits<val_t>::max());
}

}

#endif

