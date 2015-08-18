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
#ifndef SOCLIB_ADDRESS_DECODING_TABLE_H
#define SOCLIB_ADDRESS_DECODING_TABLE_H

#include <iostream>

namespace soclib { namespace common {

/////////////////////////////////////////////////
template <typename input_t, typename output_t>
class AddressDecodingTable
/////////////////////////////////////////////////
{
    int          m_use_bits; 
    int          m_drop_bits;
    input_t      m_low_mask;
    output_t*    m_table;

    void init( int use_bits, int drop_bits );

    ///////////////////////////
    inline input_t mask() const
    {
        return m_low_mask<<m_drop_bits;
    }

    //////////////////////////////////
    inline int id( input_t val ) const
    {
        return (val>>m_drop_bits)&m_low_mask;
    }

public:
    AddressDecodingTable();
    AddressDecodingTable( int use_bits, int drop_bits );
    AddressDecodingTable( input_t mask );
    void reset( output_t value = 0 );
    void set( input_t where, output_t value );
    AddressDecodingTable( const AddressDecodingTable &ref );
    const AddressDecodingTable<input_t, output_t> &operator=( const AddressDecodingTable &ref );
    ~AddressDecodingTable();
    bool isAllBelow( output_t val ) const;

	void print( std::ostream &o ) const;

    ////////////////////////////////////////////////
    inline output_t get_value( input_t where ) const
    {
        return m_table[id(where)];

    }

    /////////////////////////////////////////////////
    inline output_t operator[]( input_t where ) const
    {
        return m_table[id(where)];
    }

    /////////////////////////////////////////////////////////////////////////////////
    friend std::ostream &operator << (std::ostream &o, const AddressDecodingTable &t)
    {
        t.print(o);
        return o;
    }
};

}}

#endif /* SOCLIB_ADDRESS_DECODING_TABLE_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

