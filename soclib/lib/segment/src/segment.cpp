/* -*- c++ -*-
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
 * Copyright (c) UPMC, Lip6, Asim
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Based on previous works by Francois Pecheux & Alain Greiner
 */

#include "segment.h"

namespace soclib { namespace common {

/////////////////////////////////////////////////////////
bool Segment::isOverlapping( const Segment &other ) const
{
    addr_t this_end = m_base_address+m_size;
    addr_t other_end = other.m_base_address+other.m_size;
    if ( other_end <= m_base_address )
        return false;
    if ( this_end <= other.m_base_address )
        return false;
    return true;
}

////////////////////////////////////////////
void Segment::print( std::ostream &o ) const
{
    o << "<Segment \""<<m_name<<"\": "
      << "base = " << std::hex << m_base_address 
      << " / size = " << m_size 
      << " / tgtid = " << m_target_index 
      << " / " << (m_cacheable?"cached":"uncached");
    if ( m_special ) o << " / special";
    o << ">";
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

