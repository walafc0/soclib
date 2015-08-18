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

////////////////////////////////////////////////////////////////////////////////
// File         : generic_cache.h
// Date         : 07/01/2012
// Authors      : Alain Greiner
/////////////////////////////////////////////////////////////////////////////////
// This object is a generic, set associative, cache providing read(), write(),
// inval(), victim_select() and update() access primitives. 
// The replacement policy is pseudo-LRU.
/////////////////////////////////////////////////////////////////////////////////
// Implementation note
// The DATA part is implemented as an uint32_t array[nways*nsets*nwords].
// The DIRECTORY part is implemented as an uint32_t array[nways*nsets].
// All methods requiring a dual port RAM or cache modification using
// an associative search have been deprecated.
/////////////////////////////////////////////////////////////////////////////////
// Constructor parameters are :
// - std::string    &name
// - size_t         nways   : number of associativity levels 
// - size_t         nsets   : number of sets
// - size_t         nwords  : number of words in a cache line
// The nways, nsets, nwords parameters must be power of 2
// The nsets parameter cannot be larger than 1024
// The nways parameter cannot be larger than 16
// The nwords parameter cannot be larger than 64
/////////////////////////////////////////////////////////////////////////////////
// Template parameter is :
// - addr_t : address format to access the cache 
// The address can be larger than 32 bits, but the TAG field 
// cannot be larger than 32 bits.
/////////////////////////////////////////////////////////////////////////////////
// Lines with the tag [memcheck] are present in order to avoid
// potential errors from memory checkers like valgrind
/////////////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_GENERIC_CACHE_H
#define SOCLIB_GENERIC_CACHE_H

#include <systemc>
#include <cassert>
#include "arithmetics.h"
#include "static_assert.h"
#include "mapping_table.h"
#include <cstring>

namespace soclib { 

//////////////////////////
template<typename addr_t>
class GenericCache 
//////////////////////////
{
    typedef uint32_t    data_t;
    typedef uint32_t    tag_t;
    typedef uint32_t    be_t;

    data_t              *r_data;
    tag_t               *r_tag;
    bool                *r_val;
    bool                *r_lru;

    size_t              m_ways;	
    size_t              m_sets;	
    size_t              m_words;

    const soclib::common::AddressMaskingTable<addr_t>  m_x ;
    const soclib::common::AddressMaskingTable<addr_t>  m_y ;
    const soclib::common::AddressMaskingTable<addr_t>  m_z ;

    //////////////////////////////////////////////////////////////
    inline data_t &cache_data(size_t way, size_t set, size_t word)
    {
        return r_data[(way*m_sets*m_words)+(set*m_words)+word];
    }

    //////////////////////////////////////////////
    inline tag_t &cache_tag(size_t way, size_t set)
    {
        return r_tag[(way*m_sets)+set];
    }

    //////////////////////////////////////////////
    inline bool &cache_val(size_t way, size_t set)
    {
        return r_val[(way*m_sets)+set];
    }

    //////////////////////////////////////////////
    inline bool &cache_lru(size_t way, size_t set)
    {
        return r_lru[(way*m_sets)+set];
    }

    //////////////////////////////////////////////
    inline void cache_set_lru(size_t way, size_t set)
    {
	    size_t way2;

        cache_lru(way, set) = true;

	    for (way2 = 0; way2 < m_ways; way2++ ) 
        {
	        if (cache_lru(way2, set) == false) return;
	    }
 	    // all lines are new -> they all become old 
	    for (way2 = 0; way2 < m_ways; way2++ ) 
        {
	        cache_lru(way2, set) = false;
	    }
    }

    /////////////////////////////////////////////
    inline data_t be2mask( be_t be )
    {
        data_t mask = 0;
        if ( (be & 0x1) == 0x1 ) mask = mask | 0x000000FF;
        if ( (be & 0x2) == 0x2 ) mask = mask | 0x0000FF00;
        if ( (be & 0x4) == 0x4 ) mask = mask | 0x00FF0000;
        if ( (be & 0x8) == 0x8 ) mask = mask | 0xFF000000;
        return mask;
    }

public:

    //////////////////////////////////////////////
    GenericCache(   const std::string   &name,
                    size_t              nways, 
                    size_t              nsets, 
                    size_t              nwords)
        : m_ways(nways),
          m_sets(nsets),
          m_words(nwords),

#define l2 soclib::common::uint32_log2

          m_x( l2(nwords), l2(sizeof(data_t))),
          m_y( l2(nsets), l2(nwords) + l2(sizeof(data_t))),
          m_z( 8*sizeof(addr_t) - l2(nsets) - l2(nwords) - l2(sizeof(data_t)),
               l2(nsets) + l2(nwords) + l2(sizeof(data_t)))

#undef l2
    {
        assert(IS_POW_OF_2(nways));
        assert(IS_POW_OF_2(nsets));
        assert(IS_POW_OF_2(nwords));
        assert(nwords);
        assert(nsets);
        assert(nways);
        assert(nwords <= 64);
        assert(nsets <= 1024);
        assert(nways <= 16);

#ifdef GENERIC_CACHE_DEBUG
        std::cout
            << " m_x: " << m_x
            << " m_y: " << m_y
            << " m_z: " << m_z
            << std::endl;
#endif

        r_data = new data_t[nways*nsets*nwords];
        r_tag  = new tag_t[nways*nsets];
        r_val  = new bool[nways*nsets];
        r_lru  = new bool[nways*nsets];
    }

    ////////////////
    ~GenericCache()
    {
        delete [] r_data;
        delete [] r_tag;
        delete [] r_val;
        delete [] r_lru;
    }

    ////////////////////
    inline void reset()
    {
        for (int i = 0; i < m_ways * m_sets; i++) {
            r_tag[i] = 0;
            r_val[i] = false;
            r_lru[i] = false;
        }

        for (int i = 0; i < m_ways * m_sets * m_words; i++) {
            r_data[i] = 0;
        }
    }

    ///////////////////////////////////////////////////////////////
    // Read a single 32 bits word, checking the miss condition.
    // Both data & directory are accessed. 
    ///////////////////////////////////////////////////////////////
    inline bool read( addr_t 	ad, 
                      data_t* 	dt)
    {
        const tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        // [memcheck]
        *dt = 0;

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                *dt = cache_data(way, set, word);
                cache_set_lru(way, set);
                return true;
            }
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////
    // Read a single 32 bits word, checking the miss condition.
    // Both data & directory are accessed. 
    // The selected way, set and word index are returned in case of hit.
    /////////////////////////////////////////////////////////////////////
    inline bool read( addr_t 	ad, 
                      data_t* 	dt,
                      size_t*   selway,
                      size_t*   selset,
                      size_t*   selword) 
    {
        const tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        // [memcheck]
        *dt = 0;
        *selway = 0;
        *selset = 0;
        *selword = 0;

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                *selway  = way;
                *selset  = set;
                *selword = word;
                *dt = cache_data(way, set, word);
                cache_set_lru(way, set);
                return true;
            }
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////
    // Read a single 32 bits word, checking the miss condition without
    // LRU update
    // Both data & directory are accessed. 
    // The selected way, set and word index are returned in case of hit.
    /////////////////////////////////////////////////////////////////////
    inline bool read_neutral( addr_t 	ad, 
			      data_t* 	dt,
			      size_t*   selway,
			      size_t*   selset,
			      size_t*   selword) 
    {
        const tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        // [memcheck]
        *dt = 0;
        *selway = 0;
        *selset = 0;
        *selword = 0;

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                *selway  = way;
                *selset  = set;
                *selword = word;
                *dt = cache_data(way, set, word);
                return true;
            }
        }
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Read one or two 32 bits word, checking the miss condition.
    // Both data & directory are accessed. 
    // If the addressed word is not the last in the cache line,
    // two successive words are returned.
    // The selected way, set and first word index are returned in case of hit.
    // This function is used by the cc_vcache to get a 64 bits page table entry.
    /////////////////////////////////////////////////////////////////////////////
    inline bool read( addr_t 	ad, 
                      data_t* 	dt, 
                      data_t*	dt_next,
                      size_t*	selway,
                      size_t*	selset,
                      size_t*   selword)
    {
        const tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        // [memcheck]
        *dt = 0;
        *dt_next = 0; 
        *selway = 0;
        *selset = 0;
        *selword = 0;

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                *dt      = cache_data(way, set, word);
                if ( word+1 < m_words) 
                {
                    *dt_next = cache_data(way, set, word+1);
                }
                *selway  = way;
                *selset  = set;
                *selword = word;
                cache_set_lru(way, set);
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Search one 32 bits word, checking the hit/miss condition.
    // Only the directory is accessed. 
    // The selected way, set and first word index are returned in case of hit.
    // This function can be used when we need to access the directory
    // while we write in the data part with a different address in the same cycle.
    ///////////////////////////////////////////////////////////////////////////////
    inline bool hit(  addr_t 	ad, 
                      size_t*	selway,
                      size_t*	selset,
                      size_t*   selword)
    {
        const tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                *selway  = way;
                *selset  = set;
                *selword = word;
                cache_set_lru(way, set);
                return true;
            }
        }
        return false;
    }

    ////////////////////////////////////////////
    inline tag_t get_tag(size_t way, size_t set)
    {
        return cache_tag(way, set);
    }

    ////////////////////////////////////////////////////////////////////////////
    // this function is deprecated, as it is difficult to implement in 1 cycle. 
    ////////////////////////////////////////////////////////////////////////////
    __attribute__((deprecated))
    inline bool write(addr_t 	ad, 
                      data_t 	dt)
    {
        const tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                cache_data(way, set, word) = dt;
                cache_set_lru(way, set);
                return true;
            }
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    // this function is deprecated, as it is difficult to implement in 1 cycle. 
    ////////////////////////////////////////////////////////////////////////////
    __attribute__((deprecated))
    inline bool write(addr_t 	ad, 
                      data_t 	dt, 
                      be_t 	be)
    {
        tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                data_t mask = be2mask(be);
                data_t prev = cache_data(way, set, word);
                cache_data(way, set, word) = (mask & dt) | (~mask & prev);
                cache_set_lru(way, set);
                return true;
            }
        }
        return false;
    }
    
    /////////////////////////////////////////////////////////////////////////////
    // this function is deprecated, as it is difficult to implement in 1 cycle. 
    /////////////////////////////////////////////////////////////////////////////
    __attribute__((deprecated))
    inline bool write(addr_t 	ad, 
                      data_t 	dt, 
                      size_t* 	nway)
    {
        const tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        // [memcheck]
        *nway = 0;

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                cache_data(way, set, word) = dt;
                cache_set_lru(way, set);
                *nway = way;
                return true;
            }
        }
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////
    // this function is deprecated, as it is difficult to implement in 1 cycle. 
    /////////////////////////////////////////////////////////////////////////////
    __attribute__((deprecated))
    inline bool write(addr_t 	ad, 
                      data_t 	dt, 
                      size_t* 	nway, 
                      be_t 	be)
    {
        const tag_t       tag  = m_z[ad];
        const size_t      set  = m_y[ad];
        const size_t      word = m_x[ad];

        // [memcheck]
        *nway = 0;

        for ( size_t way = 0; way < m_ways; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                data_t mask = be2mask(be);
                data_t prev = cache_data(way, set, word);
                cache_data(way, set, word) = (mask & dt) | (~mask & prev);
                cache_set_lru(way, set);
                *nway = way;
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////
    // This function writes a complete 32 bits word
    // It does not use the directory and cannot miss.
    //////////////////////////////////////////////////////////////////
    inline void write(size_t 	way, 
                      size_t 	set, 
                      size_t 	word, 
                      data_t 	data)
    {
        cache_data(way, set, word) = data;
        cache_set_lru(way, set);
    }

    ////////////////////////////////////////////////////////////////////////////
    // this function writes up to 4 bytes, taking into account the byte enable.
    // It does not use the directory and cannot miss.
    ////////////////////////////////////////////////////////////////////////////
    inline void write(size_t 	way, 
                      size_t 	set, 
                      size_t 	word, 
                      data_t 	data, 
                      be_t 	    be)
    {
        data_t mask = be2mask(be);
        data_t prev = cache_data(way, set, word);
        cache_data(way, set, word) = (mask & data) | (~mask & prev);
        cache_set_lru(way, set);
    }

    //////////////////////////////////////////////////////////////////////////
    // This function invalidates a cache line identified by the set and way.
    // It returns true if the line was valid, and returns the line index.
    //////////////////////////////////////////////////////////////////////////
    inline bool inval(size_t 	way, 
                      size_t 	set, 
                      addr_t* 	nline)
    {
        // [memcheck]
        *nline = 0;

        if ( cache_val(way,set) ) 
        {
            cache_val(way,set) = false;
            *nline = (data_t)cache_tag(way,set)* m_sets + set;
            return true;
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////
    // This function is deprecated as it is difficult to implement in 1 cycle.
    ///////////////////////////////////////////////////////////////////////////
    __attribute__((deprecated))
    inline bool inval(addr_t 	ad)
    {
        bool              hit = false;
        const tag_t       tag = m_z[ad];
        const size_t      set = m_y[ad];

        for ( size_t way = 0 ; way < m_ways && !hit ; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                hit     = true;
                cache_val(way, set) = false;
                cache_lru(way, set) = false;
            }
        }
        return hit;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // This function is deprecated as it is difficult to implement in 1 cycle.
    ////////////////////////////////////////////////////////////////////////////////
    __attribute__((deprecated))
    inline bool inval( addr_t 	ad, 
                       size_t* 	selway, 
                       size_t* 	selset )
    {
        bool    	hit = false;
        const tag_t     tag = m_z[ad];
        const size_t    set = m_y[ad];

        // [memcheck]
        *selway = 0;
        *selset = 0;

        for ( size_t way = 0 ; way < m_ways && !hit ; way++ ) 
        {
            if ( (tag == cache_tag(way, set)) && cache_val(way, set) ) 
            {
                hit                 = true;
                cache_val(way, set) = false;
                cache_lru(way, set) = false;
                *selway             = way;
                *selset             = set;
            }
        }
        return hit;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // This function is deprecated as the directory must be a dual port RAM...
    ////////////////////////////////////////////////////////////////////////////////
    __attribute__((deprecated))
    inline bool update( addr_t 	ad, 
                        data_t* buf, 
                        addr_t* victim )
    {
        // [memcheck]
        *victim = 0;

        size_t set, way;
        bool   cleanup = victim_select(ad, victim, &way, &set);

        victim_update_tag (ad, way, set);

        for ( size_t word = 0 ; word < m_words ; word++ ) {
            cache_data(way, set, word) = buf[word] ;
        }

        return cleanup;
    }

    //////////////////////////////////////////////////////////////////////////////////
    // This function selects a victim line, implementing a pseudo LRU policy,
    // and invalidates the selected victim line.
    // It returns the line index (Z + Y fields), the selected slot way and set, 
    // and a Boolean indicating that a cleanup is requested.
    //////////////////////////////////////////////////////////////////////////////////
    inline bool victim_select(addr_t 	ad, 
                              addr_t* 	victim, 
                              size_t*   way, 
                              size_t*   set)
    {
        bool   found   = false;
        bool   cleanup = false;

        *set = m_y[ad];

        // [memcheck]
        *victim = 0;
        *way = 0;

        // Search and invalid slot
        for ( size_t _way = 0 ; _way < m_ways && !found ; _way++ )
        {
            if ( !cache_val(_way, *set) )
            {
                found   = true;
                cleanup = false;
                *way    = _way;
            }
        }
        // No invalid slot, search the lru
        if ( !found )
        { 
            for ( size_t _way = 0 ; _way < m_ways && !found ; _way++ )
            {
                if ( !cache_lru(_way, *set) )
                {
                    found   = true;
                    cleanup = true;
                    *way    = _way;
                }
            }
        }
        assert(found && "all ways can't be new at the same time");
        *victim = (addr_t)((cache_tag(*way,*set) * m_sets) + *set);
        return cleanup;
    }

    //////////////////////////////////////////////////////////////////
    // This function update the directory part of a slot
    // identified by the way & set.
    //////////////////////////////////////////////////////////////////
    inline void victim_update_tag( addr_t 	ad, 
                                   size_t 	way, 
                                   size_t 	set )
    {
        tag_t  tag     = m_z[ad];

        cache_tag    (way, set) = tag;
        cache_val    (way, set) = true;
        cache_set_lru(way, set);
    }

    ///////////////////////////////////////////////////////////////////
    // This function writes a full cache line in one single cycle.
    // The target slot is identified by the way & set arguments.
    // Both DATA and DIRECTORY are written
    ///////////////////////////////////////////////////////////////////
    inline void update(addr_t 	ad, 
                       size_t 	way, 
                       size_t 	set, 
                       data_t* 	buf)
    {
        tag_t tag = m_z[ad];

        cache_tag(way, set) = tag;
        cache_val(way, set) = true;
        cache_set_lru(way, set);
        for ( size_t word = 0 ; word < m_words ; word++ ) 
        {
            cache_data(way, set, word) = buf[word] ;
        }
    }

    ///////////////////////////
    void fileTrace(FILE* file)
    {
        for( size_t nway = 0 ; nway < m_ways ; nway++) 
        {
            for( size_t nset = 0 ; nset < m_sets ; nset++) 
            {
                if( cache_val(nway, nset) ) fprintf(file, " V / ");
                else                        fprintf(file, "   / ");
                fprintf(file, "way %d / ", (int)nway);
                fprintf(file, "set %d / ", (int)nset);
                fprintf(file, "@ = %08zX / ", 
                        ((cache_tag(nway, nset)*m_sets+nset)*m_words*4));
                for( size_t nword = m_words ; nword > 0 ; nword--) 
                {
                    unsigned int data = cache_data(nway, nset, nword-1);
                    fprintf(file, "%08X ", data );
                }
                fprintf(file, "\n");
            }
        }
    }

    ////////////////////////
    inline void printTrace()
    {
        for ( size_t way = 0; way < m_ways ; way++ ) 
        {
            for ( size_t set = 0 ; set < m_sets ; set++ )
            {
                if ( cache_val(way,set) ) std::cout << "  * " ;
                else                      std::cout << "    " ;
                std::cout << std::dec << "way " << way << std::hex << " | " ;
                std::cout << "@ " << (cache_tag(way,set)*m_words*m_sets+m_words*set)*4 ;
                for ( size_t word = 0 ; word < m_words ; word++ )
                {
                    std::cout << " | " << cache_data(way,set,word) ;
                }
                std::cout << std::endl ;
            }
        }
    }
    
};

} // namespace soclib

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

