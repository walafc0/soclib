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
 *         Alain Greiner <alain.greiner@lip6.fr>, 2011
 *
 * Maintainers: alain
 */
#ifndef SOCLIB_CABA_GENERIC_CAM_H
#define SOCLIB_CABA_GENERIC_CAM_H

#include <systemc>

namespace soclib { namespace caba {

using namespace sc_core;

///////////////////////////////////////////////////////////////
// This hardware component implements a fully associative 
// registration buffer, providing four access functions:
// - write()
// - inval()
// - hit()
// - reset()
////////////////////////////////////////////////////////////////

template<typename DataType>
class GenericCam
{
    DataType*   m_value;
    bool*	    m_valid;
    size_t	    m_max;

public:

    //////////////////////////////////////////////////////////////////////////
    // This function test returns true, if a given value is registered 
    // in the registration buffer, The matching entry index is returned.
    //////////////////////////////////////////////////////////////////////////
    bool hit( DataType value,
              size_t* index)
    {
        for ( size_t i=0 ; i<m_max ; i++ )
        {
            if ( m_valid[i] and (m_value[i] == value) )
            {
                *index = i;
                return true;
            }
        }
        return false;
    } // end hit()

    //////////////////////////////////////////////////////////////////////////
    // This function try to register a new value in the registration buffer.
    // It returns false if there is no empty slot.
    // It returns true, and the slot index in case of success.
    //////////////////////////////////////////////////////////////////////////
    bool    write( DataType value,
                   size_t*  index )
    {
        for ( size_t i=0 ; i<m_max ; i++ )
        {
            if ( not m_valid[i] ) 
            {
                m_valid[i] = true;
                m_value[i] = value;
                *index = i;
                return true;
            }
        }
        return false;
    } // end write()
    
    ////////////////////////////////////////////////////////////////////
    // this function checks if the index argument is compatible
    // with the buffer size and invalidate the corresponding slot.
    // It returns true in case of success.
    ////////////////////////////////////////////////////////////////////
    bool inval( size_t index )
    {
        if ( index < m_max )
        {
            m_valid[index] = false;
            return true;
        }
        return false;
    } // end inval()

    ////////////
    void reset()
    {
        for ( size_t i=0 ; i<m_max ; i++ ) m_valid[i] = false;
    }

    ////// constructor //////////////////////////////
    GenericCam( size_t max)
        : m_max(max)
    {
        m_valid = new bool     [m_max];
        m_value = new DataType [m_max];
        for ( size_t i=0 ; i<m_max ; i++ ) m_valid[i] = false;
    }

    ///////////////
    ~GenericCam()
    {
        delete [] m_valid;
        delete [] m_value;
    }
};

}}

#endif 

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

