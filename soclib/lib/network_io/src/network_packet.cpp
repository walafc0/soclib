/* -*- c++ -*-
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
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 */

#include "network_io.h"

#include <cstring>
#include <iostream>
#include <iomanip>
#include <stdint.h>

namespace soclib { namespace common {

NetworkPacket::NetworkPacket( const void *data, size_t size )
    : m_data( new uint8_t[size] ),
      m_size( size )
{
    std::memcpy( m_data, data, size );
}

NetworkPacket::NetworkPacket( const NetworkPacket &ref )
    : m_data( new uint8_t[ref.m_size] ),
      m_size( ref.m_size )
{
    std::memcpy( m_data, ref.m_data, ref.m_size );
}

NetworkPacket::~NetworkPacket()
{
    delete [] m_data;
}

void NetworkPacket::print( std::ostream &o ) const
{
	const uint8_t *data = (const uint8_t *)this->data();
	size_t len = size();

    o << "<Packet: " << std::dec << len << " bytes" << std::endl;
	for ( size_t i=0; i<len; i+=16 ) {
		o << std::setfill('0') << std::setw(3) << std::hex << i << " |";
		size_t end = (i+16>len)?len:(i+16), j;
		for ( j=i; j<end; ++j )
			o << ' ' << std::setfill('0') << std::setw(2) << std::hex << (int)data[j];
		for ( ; j<i+16; ++j) 
			o << "   ";
		o <<  " | " << std::setw(1);
		for ( j=i; j<end; ++j )
			o << (char)(((int)data[j]<32 || (int)data[i]>126)?'.':data[j]);
		o << std::endl;
	}
    o << ">" << std::endl;
}

const NetworkPacket& NetworkPacket::operator=( const NetworkPacket &ref )
{
    delete [] m_data;
    m_data = new uint8_t[ref.m_size];
    std::memcpy( m_data, ref.m_data, ref.m_size );
    m_size = ref.m_size;
    return *this;
}

const void *NetworkPacket::data() const
{
    return m_data;
}

const size_t NetworkPacket::size() const
{
    return m_size;
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

