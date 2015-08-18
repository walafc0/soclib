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
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo
 *
 * $Id: arm_load_store.cpp 2333 2013-05-27 14:05:09Z becoulet $
 */

#include "arm.h"
#include <cassert>
#include "arithmetics.h"

namespace soclib { namespace common {

namespace {
template<typename data_t>
data_t be_to_mask( data_t be )
{
    size_t i;
    data_t ret = 0;
    data_t be_up = (1<<(sizeof(data_t)-1));

    for (i=0; i<sizeof(data_t); ++i) {
        ret <<= 8;
        if ( be_up & be )
            ret |= 0xff;
        be <<= 1;
    }
    return ret;
}
}

void ArmIss::do_mem_access( addr_t address,
							enum DataOperationType operation,
							int byte_count,
							data_t wdata,
							data_t *rdata_dest,
							enum post_memaccess_op_e post_op
	)
{
    int byte_le = address&3;

    if ( r_cpsr.endian )
        wdata = soclib::endian::uint32_swap(wdata) >> (8 * (4-byte_count));

    m_dreq.addr = address & (~3);

    if ( address & (byte_count-1) ) {
        m_exception = EXCEPT_DABT;
        return;
    }

    m_dreq.be = (((1<<byte_count)-1) << byte_le) & 0xf;

    m_dreq.valid = true;
    m_dreq.wdata = wdata << (8 * byte_le);
    m_dreq.type = operation;
    m_dreq.mode = r_bus_mode;

#ifdef SOCLIB_MODULE_DEBUG
    std::cout
        << name()
        << " do_mem_access: " << m_dreq
        << " off: " << byte_le
        << " post_op: " << post_op
        << " dest@: " << rdata_dest
		<< " (gp+" << (int)((uint32_t*)rdata_dest-(uint32_t*)&r_gp[0]) << ")"
        << std::endl;
#endif

    if ( (byte_count + byte_le) > 4 ) {
        dump();
        abort();
    }

    r_mem_byte_le = byte_le;
    r_mem_byte_count = byte_count;
    r_mem_dest_addr = rdata_dest;
    r_mem_post_op = post_op;
}

bool ArmIss::handle_data_response( const struct DataResponse &drsp )
{
	m_dreq.valid = false;

	if ( drsp.error || ! r_mem_dest_addr )
		return false;

	data_t data = drsp.rdata >> (8 * r_mem_byte_le);

#ifdef SOCLIB_MODULE_DEBUG
	std::cout
		<< name() << " handle_data_response"
		<< " data: " << data
        << " dest@: " << r_mem_dest_addr
		<< " (gp+" << (int)((uint32_t*)r_mem_dest_addr-(uint32_t*)&r_gp[0]) << ")"
		<< std::endl;
#endif

	switch ( r_mem_post_op ) {
	case POST_OP_NONE:
		break;
	case POST_OP_WB_UNSIGNED:
		data &= ((uint64_t)1 << 8*r_mem_byte_count) - 1;
		break;
	case POST_OP_WB_SIGNED:
		data = sign_ext(data, 8*r_mem_byte_count);
		break;
	case POST_OP_WB_SWAP_HALFWORDS:
		data = (data << 16) | (data >> 16);
		break;
	case POST_OP_WB_SC:
        data = (data == Iss2::SC_ATOMIC) ? 0 : 1;
		break;
	}

    if ( r_cpsr.endian )
        data = soclib::endian::uint32_swap(data) >> (8 * (4-r_mem_byte_count));

	*r_mem_dest_addr = data;
	return r_mem_dest_addr == &r_gp[15];
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
