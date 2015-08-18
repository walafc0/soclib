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
#ifndef SOCLIB_INT_TAB_H_
#define SOCLIB_INT_TAB_H_

#include <vector>
#include "exception.h"
#include <cstdlib>

namespace soclib { namespace common {

//////////////
class IntTab
//////////////
{
public:
    typedef int value_t;

private:
    static const size_t s_max_level = 10;
    value_t             m_values[s_max_level];
    size_t              m_level;

public:
    /////////////////////////
    IntTab( value_t val0 = -1,
            value_t val1 = -1,
            value_t val2 = -1,
            value_t val3 = -1,
            value_t val4 = -1,
            value_t val5 = -1,
            value_t val6 = -1,
            value_t val7 = -1,
            value_t val8 = -1,
            value_t val9 = -1 )
    {
        value_t vals[s_max_level];
        vals[0] = val0;
        vals[1] = val1;
        vals[2] = val2;
        vals[3] = val3;
        vals[4] = val4;
        vals[5] = val5;
        vals[6] = val6;
        vals[7] = val7;
        vals[8] = val8;
        vals[9] = val9;
        init(vals);
    }

    ///////////////////////////
    IntTab( const IntTab &ref )
    {
        init(&ref.m_values[0]);
    }

    ///////////////////////////////////////
    IntTab( const IntTab &ref, int nindex )
    {
        init(&ref.m_values[0]);
        m_values[m_level] = nindex;
        m_values[++m_level] = -1;
    }

    template<typename T>
    IntTab( const std::vector<T> &v )
    {
        value_t vals[s_max_level];
        size_t i=0;
        while ( i < v.size() ) {
            vals[i] = v[i];
            ++i;
        }
        while ( i < s_max_level ) {
            vals[i] = -1;
            ++i;
        }
        init(vals);
    }

    IntTab( const value_t *vals )
    {
        init(vals);
    }

    const IntTab &operator=( const IntTab &ref )
    {
        init(&ref.m_values[0]);
        return *this;
    }

private:
    ////////////////////////////////
    void init( const value_t *vals )
    {
        for ( size_t i=0; i<s_max_level; ++i ) 
        {
            m_values[i] = vals[i];
            if ( vals[i] == -1 ) 
            {
                m_level = i;
                return;
            }
        }
        std::cout << "ERROR IntTab.init() function : Too much levels" << std::endl;
        exit(0);
    }

public:
    ////////////////////////////////////////
    value_t operator[]( size_t level ) const
    {
        if ( level > m_level )
        {
            std::cout << "ERROR IntTab.[] operator : level too large" << std::endl;
            exit(0);
        }
        return m_values[level];
    }

    ///////////////////////////////////////////////
    value_t operator*( const IntTab &widths ) const
    {
        if ( widths.level() != m_level )
        {
            std::cout << "ERROR IntTab.* operator : levels not matching" << std::endl;
            exit(0);
        }
        value_t ret = 0;

        for ( size_t l=0; l<m_level; ++l ) {
            ret <<= widths[l];
            ret += m_values[l];
        }
        return ret;
    }

    ///////////////////////////////////////////////
    value_t sum( size_t level = s_max_level ) const
    {
		if ( m_level < level ) level = m_level;
        value_t s = 0;
        for ( size_t i=0; i<level; ++i )
            s += m_values[i];
        return s;        
    }

    ///////////////////////////
    inline size_t level() const
    {
        return m_level;
    }

    ////////////////////////////////////////////
    bool operator==( const IntTab &other ) const
    {
        if ( m_level != other.m_level )
            return false;
        return idMatches(other);
    }

    ////////////////////////////////////////////
    bool operator!=( const IntTab &other ) const
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////
    bool idMatches( const IntTab &other ) const
    {
        size_t m = (m_level < other.m_level)?m_level:other.m_level;

        for ( size_t i=0; i<m; ++i )
            if ( m_values[i] != other.m_values[i] )
                return false;
        return true;
    }

    ///////////////////////////////////
    void print( std::ostream &o ) const
    {
        o << '(';
        for ( size_t i=0; i<m_level; ++i ) {
            o << m_values[i];
            if ( i < m_level-1 )
                o << ',';
        }
        o << ')';
    }

    friend std::ostream &operator << (std::ostream &o, const IntTab &it)
    {
        it.print(o);
        return o;
    }
};

}}

#endif /* SOCLIB_INT_TAB_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

