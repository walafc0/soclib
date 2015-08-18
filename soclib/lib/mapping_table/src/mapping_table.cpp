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
 *         Alain Greiner <alain.greiner@lip6.fr> 2005 
 *         Nicolas Pouillon <nipo@ssji.net> 2007
 *         Alain Greiner <alain.greiner@lip6.fr> 2013
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////////////
// Implementation Note (October 2013)
// 1) Regarding the various ADDRESS or SRCID decoding tables:
//   - ADDRESSES values are supposed to use uint64_t type
//   - SRCID values     are supposed to use uint32_t type
// 2) Regarding SRCID decoding, the m_srcid[array] is always used.
//    Identity mapping is handled as a default value: m_srcid_array[i] = i
/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <sstream>
#include "mapping_table.h"

namespace soclib { namespace common {

///////////////////////////
MappingTable::MappingTable( size_t         addr_width,
                            const IntTab   &level_addr_bits,
                            const IntTab   &level_id_bits,
                            const uint64_t cacheability_mask )
        : m_segment_list(),
          m_addr_width( addr_width ),
          m_srcid_width( level_id_bits.sum() ),
          m_addr_mask((addr_width == 64) ? ((uint64_t)-1) : (((uint64_t)1<<addr_width)-1)),
          m_level_addr_bits( level_addr_bits ),
          m_level_id_bits( level_id_bits ),
          m_cacheability_mask( cacheability_mask ),
          m_used( false )
{
    assert( (addr_width <= 64) and
    "ERROR in mapping table : address larger than 64 bits not supported\n");

    m_rt_size = 1ULL << (addr_width - m_level_addr_bits.sum());
    uint64_t cm_rt_size = 1 << AddressMaskingTable<uint64_t>(m_cacheability_mask).getDrop();
    m_rt_size = std::min<uint64_t>(cm_rt_size, m_rt_size);

    size_t srcid_size  = 1<<m_srcid_width;

    m_srcid_array = new size_t[srcid_size];

    // set identity default values
    for( size_t i=0 ; i<srcid_size ; i++ ) m_srcid_array[i] = i;
}

/////////////////////////////
MappingTable::~MappingTable() 
{
    delete [] m_srcid_array;
}

/////////////////////////////////////////////////////
MappingTable::MappingTable( const MappingTable &ref )
        : m_segment_list(ref.m_segment_list),
          m_addr_width(ref.m_addr_width),
          m_addr_mask(ref.m_addr_mask),
          m_level_addr_bits(ref.m_level_addr_bits),
          m_level_id_bits(ref.m_level_id_bits),
          m_cacheability_mask(ref.m_cacheability_mask),
          m_rt_size(ref.m_rt_size),
          m_used(ref.m_used)
{
}

//////////////////////////////////////////////////////////////////////
const MappingTable &MappingTable::operator=( const MappingTable &ref )
{
    m_segment_list = ref.m_segment_list;
    m_addr_width = ref.m_addr_width;
    m_addr_mask = ref.m_addr_mask;
    m_level_addr_bits = ref.m_level_addr_bits;
    m_level_id_bits = ref.m_level_id_bits;
    m_cacheability_mask = ref.m_cacheability_mask;
    m_rt_size = ref.m_rt_size;
    m_used = ref.m_used;
    return *this;
}

/////////////////////////////////////////////////////
void MappingTable::srcid_map( const IntTab   &srcid,
                              const IntTab   &portid )
{
    const int index = indexForId(srcid);
    assert((index < (1 << m_srcid_width)) &&
           "srcid do not fit the srcid width");
    m_srcid_array[index] = indexForId(portid);
}

/////////////////////////////////////////////
void MappingTable::add( const Segment &_seg )
{
    Segment seg = _seg.masked(m_addr_mask);
    std::list<Segment>::iterator i;

    assert(m_used == false && 
    "You must not add segments inside a mapping table once it is used.");

    if ( seg.index().level() != m_level_addr_bits.level() ) 
    {
        std::cout << "ERROR in mapping table for segment " << seg
                  << " : inconsistent level" << std::endl;
        exit(0);
    }

    for ( i = m_segment_list.begin();
          i != m_segment_list.end();
          i++ ) 
    {
        Segment &s = *i;
        if ( s.isOverlapping(seg) ) 
        {
            std::cout << "ERROR in mapping table for segment " << seg
                      << " : bumps in segment " << s << std::endl;
            exit(0);
        }

        for ( uint64_t address = s.baseAddress() & ~(m_rt_size-1);
              (address < s.baseAddress()+s.size()) &&
                  (address >= (s.baseAddress() & ~(m_rt_size-1)));
              address += m_rt_size ) 
        {
            for ( uint64_t segaddress = seg.baseAddress() & ~(m_rt_size-1);
                  (segaddress < seg.baseAddress()+seg.size()) &&
                      (segaddress >= (seg.baseAddress() & ~(m_rt_size-1)));
                  segaddress += m_rt_size ) 
            {
                if ( (m_cacheability_mask & address) == (m_cacheability_mask & segaddress)
                    && s.cacheable() != seg.cacheable() ) 
                {
                    std::cout << "ERROR in mapping table for segment " << seg
                              << " : has different cacheability with same mask "
                              << " bits than segment " << s << std::endl;
                    exit(0);
                }
            }
        }
    }
    m_segment_list.push_back(seg);
}

////////////////////////////////////////////////////////////////////////////
// This function returns a list of all segments defined in a mapping table.
////////////////////////////////////////////////////////////////////////////
const std::list<Segment> & MappingTable::getAllSegmentList() const
{
    const_cast<MappingTable*>(this)->m_used = true;
    return m_segment_list;
}

////////////////////////////////////////////////////////////////////////////
// This function returns a list containing all segments allocated
// to a target identified by its target index.
////////////////////////////////////////////////////////////////////////////
std::list<Segment> MappingTable::getSegmentList( const IntTab &index ) const
{
    std::list<Segment> ret;
    std::list<Segment>::const_iterator i;
    
    const_cast<MappingTable*>(this)->m_used = true;

    for ( i = m_segment_list.begin();
          i != m_segment_list.end();
          i++ ) 
    {
        if ( i->index() == index )
            ret.push_back(*i);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////////
// This function checks that only one segment is allocated to a target
// identified by its target index, ant returns this segment.
///////////////////////////////////////////////////////////////////////////
Segment MappingTable::getSegment( const IntTab &index ) const
{
    std::list<Segment> list = getSegmentList(index);

    const_cast<MappingTable*>(this)->m_used = true;

    assert( (list.size() == 1) and
    "ERROR in getSegment() : more than one segment allocated to target\n");

    return list.front();
}

///////////////////////////////////////////////////////////////////////////
// This function returns an ADT that can be used to get
// the local port_id for a target identified by a VCI address
// in a clusterized architecture (two levels interconnect).
// Only the local bits in the address are decoded.
// This ADT can depend on the cluster_id.
///////////////////////////////////////////////////////////////////////////
AddressDecodingTable<uint64_t,size_t> 
MappingTable::getLocalIndexFromAddress( const size_t cluster_id, 
                                        const size_t default_target_id ) const
{
    // checking two levels interconnect
    if ( m_level_addr_bits.level() != 2 )
    {
        std::cout << "ERROR in Mapping Table : the getLocalIndexFromAddress() function"
                  << " requires a two levels interconnect" << std::endl;
        exit(0);
    }

    const_cast<MappingTable*>(this)->m_used = true;

    size_t global_bits  = m_level_addr_bits[0];   // number of address global bits
    size_t local_bits   = m_level_addr_bits[1];   // number of address local bits

    // ADT to be returned
    AddressDecodingTable<uint64_t,size_t> 
    adt( local_bits, m_addr_width - global_bits - local_bits );
	adt.reset( default_target_id );

    // temporary ADT for checking
    AddressDecodingTable<uint64_t,bool>
    done( local_bits, m_addr_width - global_bits - local_bits );
	done.reset( false );

    // loop on all segments
    std::list<Segment>::const_iterator seg;
    for ( seg = m_segment_list.begin();
          seg != m_segment_list.end();
          seg++ ) 
    {
        // skip segment if cluster_id does not match
        if ( (size_t)seg->index()[0] != cluster_id )    continue; 

        // loop on all possible values for the address local bits
        for ( uint64_t addr = seg->baseAddress() & ~(m_rt_size-1);
              addr < (seg->baseAddress() + seg->size());
              addr += m_rt_size ) 
        {
            size_t port_id = seg->index()[1];

            if ( done[addr] and (adt[addr] != port_id) ) 
            {
                std::cout << "ERROR in Mapping Table : segment " << *seg
                          << " allocated to a different target than another segment"
                          << " with the same local routing bits" << std::endl;
                exit(0);
            }
            adt.set( addr, port_id );
            done.set( addr, true );
        }
	}
    return adt;
} // end getLocalIndexFromAddress()

///////////////////////////////////////////////////////////////////////////
// This function returns an ADT that can be used to get
// the local port_id for an initiator identified by the SRCID
// in a clusterized architecture (two levels interconnect).
// Only the local bits in the SRCID are decoded.
// This ADT can depend on the cluster_id.
///////////////////////////////////////////////////////////////////////////
AddressDecodingTable<uint32_t, size_t> 
MappingTable::getLocalIndexFromSrcid( const size_t cluster_id ) const
{
    // checking two levels interconnect
    if ( m_level_addr_bits.level() != 2 )
    {
        std::cout << "ERROR in Mapping Table : the getLocalIndexFromSrcid() function"
                  << " requires a two levels interconnect" << std::endl;
        exit(0);
    }

    const_cast<MappingTable*>(this)->m_used = true;

    size_t local_width  = m_level_id_bits[1];   // number of local bits in SRCID 
    size_t adt_size   = 1<<local_width;         // number of entries in adt
    size_t local_mask = adt_size - 1;           // SRCID mask for local bits

    // ADT to be returned
    AddressDecodingTable<uint32_t, size_t> 
    adt(local_width, 0);

    // loop on all possible local index values
    for ( size_t i = 0 ; i < adt_size ; i++ )
    {
        size_t srcid = (cluster_id<<local_width) + i;
        adt.set( srcid, m_srcid_array[srcid] & local_mask );
    }
    return adt;
} // end getLocalIndexFromSrcid()

////////////////////////////////////////////////////////////
// This function returns an ADT that can be used to get
// the "is_local" condition from a physical address
// in a clusterized architecture (two levels interconnect).
// Only the global bits in the address are decoded.
////////////////////////////////////////////////////////////
AddressDecodingTable<uint64_t, bool>
MappingTable::getLocalMatchFromAddress( const size_t cluster_id ) const
{
    // checking two levels interconnect
    if ( m_level_addr_bits.level() != 2 )
    {
        std::cout << "ERROR in Mapping Table : the getLocalMatchFromAddress() function"
                  << " requires a two levels interconnect" << std::endl;
        exit(0);
    }

    const_cast<MappingTable*>(this)->m_used = true;

    // number of global bits in physical address
	size_t global_bits = m_level_addr_bits[0];

    // ADT to be returned
    AddressDecodingTable<uint64_t, bool> 
    adt( global_bits, m_addr_width - global_bits );
	adt.reset(false);

    // temporary ADT for checking
    AddressDecodingTable<uint64_t, bool> 
    done( global_bits, m_addr_width - global_bits );
	done.reset(false);

    // loop on all segments
    std::list<Segment>::const_iterator seg;
    for ( seg = m_segment_list.begin();
          seg != m_segment_list.end();
          seg++ ) 
    {
        bool local = ( (size_t)(seg->index()[0]) == cluster_id );

        for ( uint64_t addr = seg->baseAddress() & ~(m_rt_size-1);
              addr < (seg->baseAddress() + seg->size());
              addr += m_rt_size ) 
        {
            if ( done[addr] && adt[addr] != local ) 
            {
                std::cout << "ERROR in Mapping Table : segment " << *seg
                          << " allocated to a different target than another segment"
                          << " with the same global routing bits" << std::endl;
                exit(0);
            }
            adt.set( addr, local );
            done.set( addr, true );
        }
	}
    return adt;
} // end getLocalMatchFromAddress()

////////////////////////////////////////////////////////////
// This function returns an ADT that can be used to get
// the "is_local" condition from the SRCID value
// in a clusterized architecture (two levels interconnect).
// Only the global bits in the SRCID are decoded.
// We do not asssume identity mapping for m_srcid_array[]
////////////////////////////////////////////////////////////
AddressDecodingTable<uint32_t, bool>
MappingTable::getLocalMatchFromSrcid( const size_t cluster_id ) const
{
    // checking two levels interconnect
    if ( m_level_addr_bits.level() != 2 )
    {
        std::cout << "ERROR in Mapping Table : the getLocalMatchFromSrcid() function"
                  << " requires a two levels interconnect" << std::endl;
        exit(0);
    }

    const_cast<MappingTable*>(this)->m_used = true;

	size_t global_width = m_level_id_bits[0];  // number of global bits in SRCID
	size_t local_width  = m_level_id_bits[1];  // number of local bits in SRCID
    size_t adt_size     = 1<<global_width;     // number of entries in adt

    // ADT to be returned
    AddressDecodingTable<uint32_t, bool> 
    adt( global_width, local_width );

    // loop on all possible global index values)
    for ( size_t i = 0 ; i < adt_size ; i++ )
    {
        bool match = ( cluster_id == (m_srcid_array[i<<local_width]>>local_width) );
        adt.set( i<<local_width , match );
    }
    return adt;
} // end getLocalMatchFromAddress()

//////////////////////////////////////////////////////////////////
// This function returns an ADT that can be used to get
// the target port index from the MSB bits of a physical address.
// It can be used in a flat (non clusterized) interconnect,
// or in a two level interconnect to perform global routing.
// Only the global bits in the address are decoded.
//////////////////////////////////////////////////////////////////
AddressDecodingTable<uint64_t, size_t>
MappingTable::getGlobalIndexFromAddress( const size_t default_id ) const
{
    const_cast<MappingTable*>(this)->m_used = true;

	size_t global_bits = m_level_addr_bits[0];

    //  ADT to be returned 
    AddressDecodingTable<uint64_t, size_t> 
    adt( global_bits, m_addr_width - global_bits );
	adt.reset( default_id );

    // temporary ADT for checking
    AddressDecodingTable<uint64_t, bool>
    done( global_bits, m_addr_width - global_bits );
	done.reset(false);

    // loop on all segments
    std::list<Segment>::const_iterator seg;
    for ( seg = m_segment_list.begin();
          seg != m_segment_list.end();
          seg++ ) 
    {
        size_t global_id = (size_t)(seg->index()[0]);

        for ( uint64_t addr = seg->baseAddress() & ~(m_rt_size-1);
              addr < (seg->baseAddress() + seg->size());
              addr += m_rt_size ) 
        {
            if ( done[addr] && adt[addr] != global_id ) 
            {
                std::cout << "ERROR in Mapping Table : segment " << *seg
                          << " allocated to a different target than another segment"
                          << " with the same global routing bits" << std::endl;
                exit(0);
            }
            adt.set( addr, global_id );
            done.set( addr, true );
        }
	}
    return adt;
} // end getGlobaIndexFromAddress()

//////////////////////////////////////////////////////////////////
// This function returns an ADT that can be used to get
// the initiator port index from the MSB bits of a SRCID.
// It can be used in a flat (non clusterized) interconnect,
// or in a two level interconnect to perform global routing.
// Only the global bits in the SRCID are decoded.
//////////////////////////////////////////////////////////////////
AddressDecodingTable<uint32_t, size_t>
MappingTable::getGlobalIndexFromSrcid() const
{
    const_cast<MappingTable*>(this)->m_used = true;

    size_t global_width = m_level_id_bits[0];
    size_t local_width  = m_srcid_width - global_width;
    size_t adt_size     = 1<<global_width;

    //  ADT to be returned 
    AddressDecodingTable<uint32_t, size_t> 
    adt( global_width, local_width );

    // loop on all possible global index values)
    for ( size_t i = 0 ; i < adt_size ; i++ )
    {
        size_t global_id = m_srcid_array[i<<local_width]>>local_width;
        adt.set( i<<local_width , global_id );
    }
    return adt;
} // end getGlobaIndexFromSrcid()

////////////////////////////////////////////////////////////////////////
// This function returns an ADT that can be used to get
// the cacheability attribute from a physical address.
// Only the bits corresponding to the cacheability mask are decoded.
////////////////////////////////////////////////////////////////////////
AddressDecodingTable<uint64_t, bool> 
MappingTable::getCacheabilityTable() const
{
    // ADT to be returned
    AddressDecodingTable<uint64_t, bool> 
    adt(m_cacheability_mask);
	adt.reset(false);

    // temporary ADT for checking
    AddressDecodingTable<uint64_t, bool> 
    done(m_cacheability_mask);
	done.reset(false);

    const_cast<MappingTable*>(this)->m_used = true;

    // loop on all segments
    std::list<Segment>::const_iterator seg;
    for ( seg = m_segment_list.begin();
          seg != m_segment_list.end();
          seg++ ) 
    {
        for ( uint64_t addr = seg->baseAddress() & ~(m_rt_size-1);
              addr < (seg->baseAddress() + seg->size());
              addr += m_rt_size ) 
        {
            if ( done[addr] and adt[addr] != seg->cacheable() ) 
            {
                std::cout << "ERROR in Mapping Table : segment " << *seg
                          << " has different cacheability than other segment "
                          << " with the same cacheability mask" << std::endl;
                exit(0);
            }
            adt.set( addr, seg->cacheable() );
            done.set( addr, true );
        }
    }
    return adt;
} // end getCacheabilityFromAddress() 

//////////////////////////////////////////////////
void MappingTable::print( std::ostream &o ) const
//////////////////////////////////////////////////
{
    std::list<Segment>::const_iterator i;

    o << "Mapping table: ad:" << m_level_addr_bits
      << " id:" << m_level_id_bits
      << " cacheability mask: " << std::hex << std::showbase << m_cacheability_mask
      << std::endl;
    for ( i = m_segment_list.begin();
          i != m_segment_list.end();
          i++ ) 
    {
        o << " " << (*i) << std::endl;
    }
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

