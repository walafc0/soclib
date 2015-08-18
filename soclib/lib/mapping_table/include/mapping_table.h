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
 *         Alain Greiner <alain.greiner@lip6.fr> 2005 
 *         Nicolas Pouillon <nipo@ssji.net> 2007
 *         Alain Greiner <alain.greiner@lip6.fr> 2013
 *
 * Maintainers: alain
 */

/////////////////////////////////////////////////////////////////////////
// Implementation Note (October 2013)
// 1) Regarding the various ADDRESS or SRCID decoding tables:
//   - ADDRESSES values are supposed to use uint64_t type
//   - SRCID values     are supposed to use uint32_t type
// 2) Regarding SRCID decoding, the m_srcid[array] is always used.
//    Identity mapping is handled as a default value in this array.
/////////////////////////////////////////////////////////////////////////

#ifndef SOCLIB_MAPPING_TABLE_H
#define SOCLIB_MAPPING_TABLE_H

#include <inttypes.h>
#include <list>
#include <sstream>

#include "segment.h"
#include "address_decoding_table.h"
#include "address_masking_table.h"
#include "int_tab.h"

namespace soclib { namespace common {

//////////////////////////////////////
class MappingTable
//////////////////////////////////////
{

private:
    std::list<soclib::common::Segment> m_segment_list;     // list of all segments
    size_t                             m_addr_width;       // number of bits
    size_t                             m_srcid_width;      // number of bits
    uint64_t                           m_addr_mask;
    IntTab                             m_level_addr_bits;  // nb  bits per level (addr)
    IntTab                             m_level_id_bits;    // nb  bits per level (srcid)
    uint64_t                           m_cacheability_mask;
    uint64_t                           m_rt_size;          // max segment size for 1 target
    bool                               m_used;             // no more modif when true
    size_t*                            m_srcid_array;      // array indexed by srcid,
                                                           // containing local port index

public:
    MappingTable( const MappingTable& );

    const MappingTable &operator=( const MappingTable & );

    MappingTable( size_t           addr_width,
                  const IntTab     &level_addr_bits,
                  const IntTab     &level_id_bits,
                  const uint64_t   cacheability_mask );
    
    ~MappingTable();

    //////////////////////////////////////////////
    void add( const soclib::common::Segment &seg );

    ////////////////////////////////////////////////////
    void srcid_map( const IntTab &srcid, 
                    const IntTab &portid );

    ///////////////////////////////////////////////////////////////
    std::list<Segment> getSegmentList( const IntTab &index ) const;

    ////////////////////////////////////////////////////
    const std::list<Segment> &getAllSegmentList() const;

    ////////////////////////////////////////////////////////////////
    soclib::common::Segment getSegment( const IntTab &index ) const;

    //////////////////////////////////////
    AddressDecodingTable<uint64_t, size_t>
    getLocalIndexFromAddress( const size_t cluster_id, 
                              const size_t default_id = 0 ) const;

    ///////////////////////////////////////
    AddressDecodingTable<uint32_t, size_t>
    getLocalIndexFromSrcid( const size_t cluster_id ) const;

    /////////////////////////////////////
    AddressDecodingTable<uint64_t, bool> 
    getLocalMatchFromAddress( const size_t cluster_id ) const;

    /////////////////////////////////////
    AddressDecodingTable<uint32_t, bool> 
    getLocalMatchFromSrcid( const size_t cluster_id ) const;

    //////////////////////////////////////
    AddressDecodingTable<uint64_t, size_t> 
    getGlobalIndexFromAddress( const size_t default_id = 0 ) const;
    
    //////////////////////////////////////
    AddressDecodingTable<uint32_t, size_t> 
    getGlobalIndexFromSrcid() const;
    
    ////////////////////////////////////
    AddressDecodingTable<uint64_t, bool> 
    getCacheabilityTable() const;

    ////////////////////////////////////
    void print( std::ostream &o ) const;

    //////////////////////////////
    size_t getAddressWidth() const
    {
        return m_addr_width;
    }

    inline const IntTab& getSrcidLevelBits() const
    {
        return m_level_id_bits;
    }

    //////////////////////////////////////////////////////////////////////////
    friend std::ostream &operator << (std::ostream &o, const MappingTable &mt)
    {
        mt.print(o);
        return o;
    }

    /////////////////////////////////////////////////////
    inline size_t indexForId( const IntTab &index ) const
    {
        return index*m_level_id_bits;
    }

    ///////////////////////////
    inline size_t level() const
    {
        return m_level_addr_bits.level();
    }
};

}}

#endif /* SOCLIB_MAPPING_TABLE_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

