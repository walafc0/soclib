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
 * Copyright (c) UPMC, Lip6
 *         Aline Vieira de Mello <aline.vieira-de-mello@lip6.fr> Sep 2009
 *
 * Maintainers: alinevieiramello@hotmail.com
 */

////////////////////////////////////////////////////////////////////////////
// This object is a generic Write Buffer, providing a re-ordering service
// for write bursts.
// A write request is accepted if the buffer is empty, or if the buffer
// contains data in the same cache line.
//
// It has two constructor parameters :
// - std::string    name
// - size_t         nwords : number of words
// It has one template parameter :
// - addr_t defines the address format
////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_WRITE_BUFFER_H
#define SOCLIB_WRITE_BUFFER_H

#include <systemc>
#include <cassert>

namespace soclib { 

namespace tlmdt { 

using namespace sc_core;

//////////////////////////////
template<typename addr_t>
class WriteBuffer
//////////////////////////////
{
    typedef uint32_t    data_t;
    typedef uint32_t    be_t;
 
    addr_t   m_address;      // cache line base address
    size_t   m_min;          // smallest valid word index
    size_t   m_max;          // largest valid word index
    bool     m_empty;        // buffrer empty
    data_t   *m_data;        // write data  array
    be_t     *m_be;          // byte enable array

    size_t   m_nwords;       // buffer size (number of words)
    addr_t   m_mask;         // cache line mask
 
public:

    /////////////
    void print_status()
    /////////////
    {
        std::cout << "BUFFER PRINT STATUS" << std::endl;
        for( size_t i = 0 ; i < m_nwords ; i++ ) {
            std::cout << "data[" << i << "] = " << std::hex <<  m_data[i] << std::dec << std::endl ;
        }
        std::cout << std::endl;
    } 

    /////////////
    void reset()
    /////////////
    {
        //printf("reset\n");
        m_max   = 0 ;
        m_min   = m_nwords - 1 ;
        m_empty = true ;
        for( size_t i = 0 ; i < m_nwords ; i++ ) {
            m_be[i] = 0 ;
            m_data[i] = 0 ;
        }
    } 
    ///////////////////////////////////////////////////
    bool wok(addr_t addr)
    ///////////////////////////////////////////////////
    {
        addr_t address = addr & ~m_mask;
        return ( m_empty || ((addr_t)m_address == address) ) ;
        //return ( m_empty || (m_address == (addr & ~m_mask)) ) ;
    }
    ///////////////////////////////////////////////////
    bool rok()
    ///////////////////////////////////////////////////
    {
        return (!m_empty);
    }
    ///////////////////////////////////////////////////
    void write(addr_t addr, be_t be , data_t  data)
    ///////////////////////////////////////////////////
    {
        size_t word = (size_t)((addr &  m_mask) >> 2) ;
        addr_t line = addr & ~m_mask ;

        // update m_address & m_empty
        if ( m_empty ) {
            m_address = line ;
        } else {
            assert( (addr_t)m_address == line );
        }
        m_empty = false ;

        // update m_be
        m_be[word]   = m_be[word] | be ;

        // update m_data, building a mask from the be value
        data_t  data_mask = 0;
        be_t    be_up = (1<<(sizeof(data_t)-1));
        for (size_t i = 0 ; i < sizeof(data_t) ; ++i) {
            data_mask <<= 8;
            if ( be_up & be ) data_mask |= 0xff;
            be <<= 1;
        }
        m_data[word] = (m_data[word] & ~data_mask) | (data & data_mask) ;


        // update m_min & m_max
        if ( m_min > word ) m_min = word;
        if ( m_max < word ) m_max = word;

    }
    ///////////////////////////////////
    inline size_t getMin()
    ///////////////////////////////////
    {
        return  m_min;
    }
    ///////////////////////////////////
    inline size_t getMax()
    ///////////////////////////////////
    {
        return  m_max;
    }
    //////////////////////////////////////
    inline addr_t getAddress(size_t word)
    //////////////////////////////////////
    {
        addr_t address = m_address;
        address += (addr_t)(word << 2);
        return address;
        //return ( m_address + (addr_t)(word << 2) );
    } 
    ///////////////////////////////////
    data_t inline getData(size_t word)
    ///////////////////////////////////
    {
        return m_data[word];
    } 
    ///////////////////////////////////
    be_t inline getBe(size_t word)
    ///////////////////////////////////
    {
        return m_be[word];
    } 
    //////////////////////////////////////////////////// 
    WriteBuffer(const std::string &name, size_t nwords)
    //////////////////////////////////////////////////// 
    {
          m_data   = new data_t[nwords];
          m_be     = new be_t[nwords];
          m_nwords = nwords;
          m_mask   = (addr_t)((nwords << 2) - 1);
    }
    ////////////////
    ~WriteBuffer()
    ////////////////
    {
        delete [] m_data;
        delete [] m_be;
    }
};

}} // end name space soclib

#endif /* SOCLIB_WRITE_BUFFER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

