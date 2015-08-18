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
#ifndef SOCLIB_COMMON_LINKED_ACCESS_BUFFER_H_
#define SOCLIB_COMMON_LINKED_ACCESS_BUFFER_H_

#include <sys/types.h>
#include <stdint.h>
#include <assert.h>
#include <systemc>

namespace soclib { namespace common {

template<typename addr_t, typename id_t>
struct LinkedAccessEntry
{
	addr_t address;
	bool atomic;
    uint32_t m_lfsr;

    LinkedAccessEntry()
    {
        m_lfsr = -1;
    }

	inline void invalidate()
	{
		address = 0;
		atomic = false;
	}
	inline void invalidate(addr_t addr)
	{
        if ( address == addr )
            atomic = false;
	}
    inline void set( addr_t addr )
    {
        /* to avoid livelock, force the atomic access to fail (pseudo-)randomly */
        bool fail = (m_lfsr % (64) == 0);
        m_lfsr = (m_lfsr >> 1) ^ ((-(m_lfsr & 1)) & 0xd0000001);

        address = addr;
        atomic = !fail;
    }
    inline bool is_atomic( addr_t addr )
    {
        return address == addr && atomic;
    }
};

template<typename addr_t, typename id_t>
class LinkedAccessBuffer
{
	typedef LinkedAccessEntry<addr_t, id_t> entry_t;
	entry_t * const m_access;
	const size_t m_n_entry;
public:
	LinkedAccessBuffer( size_t n_entry = 0);
	~LinkedAccessBuffer();

	void clearAll()
    {
        for ( size_t i=0; i<m_n_entry; ++i )
            m_access[i].invalidate();
    }

	void accessDone( addr_t address )
    {
        for ( size_t i=0; i<m_n_entry; ++i )
            m_access[i].invalidate(address);
    }

	void doLoadLinked( addr_t address, id_t id )
    {
        assert(id < m_n_entry && "Access out of bounds");
        m_access[id].set(address);
    }

	bool isAtomic( addr_t address, id_t id ) const
    {
        assert(id < m_n_entry && "Access out of bounds");
        return m_access[id].is_atomic(address);
    }
};

}}

#endif /* SOCLIB_COMMON_LINKED_ACCESS_BUFFER_H_ */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

