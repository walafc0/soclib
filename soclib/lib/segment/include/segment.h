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
 * Copyright (c) UPMC, Lip6, Asim
 *         Nicolas Pouillon <nipo@ssji.net>, 2007
 *
 * Based on previous works by Francois Pecheux & Alain Greiner
 *
 * Maintainers: nipo
 */
#ifndef SOCLIB_SEGMENT_H_
#define SOCLIB_SEGMENT_H_

#include "exception.h"
#include "int_tab.h"
#include "stdint.h"
#include <string>

namespace soclib { namespace common {

////////////////
class Segment
////////////////
{
    typedef uint64_t addr_t;
    
    std::string m_name;
    addr_t      m_base_address;
    size_t      m_size;
    IntTab      m_target_index;
    bool        m_cacheable;
    bool        m_special;

public:

    /////////////////////////////////
    Segment( const std::string &name,               // segment name
             addr_t            base_address,        // segment base address
             size_t            size,                // segment size (bytes)
             const IntTab      &target_index,       // VCI target composite index
             bool              cacheable,           // cacheable if true
             bool              special = false )    // context dependant
            : m_name(name), 
              m_base_address(base_address),
              m_size(size), 
              m_target_index(target_index),
              m_cacheable(cacheable),
              m_special( special )
    {
    }

    //////////////////////////////////////////////
    const Segment &operator=( const Segment &ref )
    {
        if ( &ref == this )
            return *this;

        m_name         = ref.m_name;
        m_base_address = ref.m_base_address;
        m_size         = ref.m_size;
        m_target_index = ref.m_target_index;
        m_cacheable    = ref.m_cacheable;
        m_special      = ref.m_special;

        return *this;
    }

    /////////////////////////////////
    inline addr_t baseAddress() const
    {
        return m_base_address;
    }

    //////////////////////////
    inline size_t size() const
    {
        return m_size;
    }

    /////////////////////////////
    inline bool cacheable() const
    {
        return m_cacheable;
    }

    ///////////////////////////
    inline bool special() const
    {
        return m_special;
    }

    //////////////////////////////////////
    inline const std::string &name() const
    {
        return m_name;
    }

    //////////////////////////////////
    inline const IntTab &index() const
    {
        return m_target_index;
    }

    /////////////////////////////////////////////////
    bool isOverlapping( const Segment &other ) const;

    ////////////////////////////////////
    void print( std::ostream &o ) const;

    friend std::ostream &operator << (std::ostream &o, const Segment &s)
    {
        s.print(o);
        return o;
    }

    /////////////////////////////////////////
    inline bool contains( addr_t addr ) const
    {
        return ( addr >= m_base_address &&
                 ( addr < m_base_address+m_size
                   || m_base_address+m_size < m_base_address) );
    }

    //////////////////////////////////////////
    inline Segment masked( addr_t addr ) const
    {
        Segment s = *this;
        s.m_base_address &= addr;
        return s;
    }

};

}}

#endif /* SOCLIB_SEGMENT_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

