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
 * Maintainers: nipo
 */
#ifndef SOCLIB_ADDRESS_MASKING_TABLE_H
#define SOCLIB_ADDRESS_MASKING_TABLE_H

#include <iostream>
#include <systemc>

namespace soclib { namespace common {

////////////////////////////////
template <typename data_t>
class AddressMaskingTable
////////////////////////////////
{
    size_t m_use_bits; 
    size_t m_drop_bits;
    data_t m_low_mask;

    void init( size_t use_bits, size_t drop_bits );

public:
    AddressMaskingTable();
    AddressMaskingTable( size_t use_bits, size_t drop_bits);
    AddressMaskingTable( data_t mask );
    AddressMaskingTable( const AddressMaskingTable &ref );
    const AddressMaskingTable<data_t> &operator=( const AddressMaskingTable &ref );

    inline data_t mask() const
    {
        return m_low_mask<<m_drop_bits;
    }

    inline size_t getUse() const
    {
        return m_use_bits;
    }
    
    inline size_t getDrop() const
    {
        return m_drop_bits;
    }

	void print( std::ostream &o ) const;

    //////////////////////////////////////////////
    inline data_t get_value( data_t where ) const 
    {
        return (where>>m_drop_bits)&m_low_mask;
    }
    
    //////////////////////////////////////////////
    inline data_t operator[]( data_t where ) const
    {
        return (where>>m_drop_bits)&m_low_mask;
    }

    ////////////////////////////////////////////////////////////////////////////////
    friend std::ostream &operator << (std::ostream &o, const AddressMaskingTable &t)
    {
        t.print(o);
        return o;
    }
};

}}

#endif /* SOCLIB_ADDRESS_MASKING_TABLE_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

