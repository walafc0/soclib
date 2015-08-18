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
 *         Alain Greiner <alain.greiner@lip6.fr> July 2008
 *
 * Maintainers: alain
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

using namespace sc_core;

//////////////////////////////
template<typename addr_t>
class WriteBuffer
//////////////////////////////
{
    typedef uint32_t    data_t;
    typedef uint32_t    be_t;

    sc_signal<addr_t>   r_address;      // cache line base address
    sc_signal<size_t>   r_min;          // smallest valid word index
    sc_signal<size_t>   r_max;          // largest valid word index
    sc_signal<bool>     r_empty;        // buffrer empty
    sc_signal<data_t>   *r_data;        // write data  array
    sc_signal<be_t>     *r_be;          // byte enable array

    size_t              m_nwords;       // buffer size (number of words)
    addr_t              m_mask;         // cache line mask
 
public:

    //////////////////////// 
    void inline printTrace()
    //////////////////////// 
    {
        std::cout << "  Write Buffer / empty = " << r_empty.read() 
                  << " / addr = " << std::hex << r_address.read() 
                  << " / min = " << r_min.read() << " / max = " << r_max.read() 
                  << std::endl << "  data = / ";
        for( size_t w=0 ; w<m_nwords ; w++ )
        {
            std::cout << r_data[w].read() << " / ";
        }
        std::cout << std::endl << "  be   = ";
        for( size_t w=0 ; w<m_nwords ; w++ )
        {
            std::cout << r_be[w].read() << " / ";
        }
        std::cout << std::endl;
    }
    /////////////
    void print_status()
    /////////////
    {
        std::cout << "BUFFER PRINT STATUS" << std::endl;
        for( size_t i = 0 ; i < m_nwords ; i++ ) {
            std::cout << "data[" << i << "] = " << std::hex <<  r_data[i] << std::dec << std::endl ;
        }
        std::cout << std::endl;
    } 

    /////////////
    void reset()
    /////////////
    {
        r_max   = 0 ;
        r_min   = m_nwords - 1 ;
        r_empty = true ;
        for( size_t i = 0 ; i < m_nwords ; i++ ) {
            r_be[i] = 0 ;
            r_data[i] = 0 ;
        }
    } 
    ///////////////////////////////////////////////////
    bool wok(addr_t addr)
    ///////////////////////////////////////////////////
    {
        addr_t address = addr & ~m_mask;
        return ( r_empty || ((addr_t)r_address == address) ) ;
        //return ( r_empty || (r_address == (addr & ~m_mask)) ) ;
    }
    ///////////////////////////////////////////////////
    bool rok()
    ///////////////////////////////////////////////////
    {
        return (!r_empty);
    }
    ///////////////////////////////////////////////////
    bool empty()
    ///////////////////////////////////////////////////
    {
        return (r_empty);
    }
    ///////////////////////////////////////////////////
    void write(addr_t addr, be_t be , data_t  data)
    ///////////////////////////////////////////////////
    {
        size_t word = (size_t)((addr &  m_mask) >> 2) ;
        addr_t line = addr & ~m_mask ;

        // update r_address & r_empty
        if ( r_empty ) {
            r_address = line ;
        } else {
            assert( (addr_t)r_address == line );
        }
        r_empty = false ;

        // update r_be
        r_be[word]   = r_be[word] | be ;

        // update r_data, building a mask from the be value
        data_t  data_mask = 0;
        be_t    be_up = (1<<(sizeof(data_t)-1));
        for (size_t i = 0 ; i < sizeof(data_t) ; ++i) {
            data_mask <<= 8;
            if ( be_up & be ) data_mask |= 0xff;
            be <<= 1;
        }
        r_data[word] = (r_data[word] & ~data_mask) | (data & data_mask) ;

        // update r_min & r_max
        if ( r_min > word ) r_min = word;
        if ( r_max < word ) r_max = word;
    }
    ///////////////////////////////////
    inline size_t getMin()
    ///////////////////////////////////
    {
        return  r_min;
    }
    ///////////////////////////////////
    inline size_t getMax()
    ///////////////////////////////////
    {
        return  r_max;
    }
    //////////////////////////////////////
    inline addr_t getAddress(size_t word)
    //////////////////////////////////////
    {
        addr_t address = r_address;
        address += (addr_t)(word << 2);
        return address;
        //return ( r_address + (addr_t)(word << 2) );
    } 
    ///////////////////////////////////
    data_t inline getData(size_t word)
    ///////////////////////////////////
    {
        return r_data[word];
    } 
    ///////////////////////////////////
    be_t inline getBe(size_t word)
    ///////////////////////////////////
    {
        return r_be[word];
    } 
    //////////////////////////////////////////////////// 
    WriteBuffer(const std::string &name, size_t nwords)
    //////////////////////////////////////////////////// 
        : r_address((name+"_r_address").c_str()),
          r_min((name+"_r_min").c_str()),
          r_max((name+"_r_max").c_str()),
          r_empty((name+"_r_empty").c_str())
    {
          r_data   = new sc_signal<data_t>[nwords];
          r_be     = new sc_signal<be_t>[nwords];
          m_nwords = nwords;
          m_mask   = (addr_t)((nwords << 2) - 1);
    }
    ////////////////
    ~WriteBuffer()
    ////////////////
    {
        delete [] r_data;
        delete [] r_be;
    }
};

} // end name space soclib

#endif /* SOCLIB_WRITE_BUFFER_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

