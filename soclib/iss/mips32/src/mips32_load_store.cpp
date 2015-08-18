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
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: nipo
 *
 * $Id$
 */

#include "mips32.h"
#include "base_module.h"
#include "arithmetics.h"

#include <strings.h>

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

void Mips32Iss::do_mem_access( addr_t address,
                               int byte_count,
                               int sign_extend,
                               uint32_t *dest,
                               int dest_byte_in_reg,
                               data_t wdata,
                               enum DataOperationType operation )
{
    if (!isPriviliged() && isPrivDataAddr(address)) {
        r_bar = address;
        m_exception = X_ADEL;
        return;
    }

    int byte_le = address&3;
    assert( (byte_count + byte_le) <= 4 );

    if ( ! m_little_endian ) {
        switch (operation) {
        case Iss2::XTN_READ:
        case Iss2::XTN_WRITE:
            break;
        default:
//        byte_le = (4-byte_count)^byte_le;
            wdata = soclib::endian::uint32_swap(wdata) >> (8 * (4-byte_count));
        }
    }

    m_dreq.addr = address & (~3);
    m_dreq.be = (((1<<byte_count)-1) << byte_le) & 0xf;

    m_dreq.valid = true;
    m_dreq.wdata = wdata << (8 * byte_le);
    m_dreq.type = operation;
    m_dreq.mode = r_bus_mode;

    if ( dest == &r_gp[0] )
        dest = NULL;

#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_DATA) {
        std::cout
            << name()
            << " do_mem_access: " << m_dreq
            << " off: " << byte_le
            << " sign_ext: " << sign_extend
            << " dest: gp+" << dest - &r_gp[0]
            << std::endl;
    }
#endif

    r_mem_byte_le = byte_le;
    r_mem_byte_count = byte_count;
    r_mem_offset_byte_in_reg = dest_byte_in_reg;
    r_mem_do_sign_extend = sign_extend;
    r_mem_dest = dest;
}

bool Mips32Iss::handle_dfetch(const struct DataResponse &rsp)
{
    if ( ! m_dreq.valid ) {
        return true;
    }

    if ( !rsp.valid )
        return false;

#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_DATA) {
        std::cout
            << name()
            << " setData: " << rsp
            << " off: " << r_mem_offset_byte_in_reg
            << " sign_ext: " << r_mem_do_sign_extend
            << " dest: r_gp+" << r_mem_dest - &r_gp[0]
            << std::endl;
    }
#endif

    m_dreq.valid = false;
    if ( rsp.error ) {
        m_exception = X_DBE;
        r_bar = m_dreq.addr;
        return true;
    }

    // We write the  r_gp[i], and we detect a possible data dependency,
    // in order to implement the delayed load behaviour.
    switch (m_dreq.type) {
    case DATA_READ:
    case DATA_LL:
    case DATA_SC:
    case XTN_READ: {
        uint32_t reg_use = curInstructionUsesRegs();
        if ( (reg_use & USE_S && r_mem_dest == &r_gp[m_ins.r.rs]) ||
             (reg_use & USE_T && r_mem_dest == &r_gp[m_ins.r.rt]) )
            m_hazard = true;
        break;
    }
    case DATA_WRITE:
    case XTN_WRITE:
        m_hazard = false;
        break;
    }

    // With destination register == 0, this is a store or a load to r0.
    if ( r_mem_dest == NULL )
        return true;

    data_t data = rsp.rdata;
    int byte_count = r_mem_byte_count;

    data >>= 8*r_mem_byte_le;

    if ( !m_little_endian ) {
        data_t sdata = soclib::endian::uint32_swap(data) >> (8 * (4-byte_count));
        data_t mask = be_to_mask<data_t>((1 << byte_count) - 1);
        data = sdata & mask;
#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_DATA) {
        std::cout
            << name()
            << " BE swapping"
            << " count: " << byte_count
            << " le: " << r_mem_byte_le
            << " orig data: " << rsp.rdata
            << " swapped data: " << sdata
            << " mask: " << mask
            << " data: " << data
            << std::endl;
    }
#endif
    }

    switch (r_mem_do_sign_extend) {
    case 2:
        data = soclib::common::sign_ext(data, 16);
        byte_count = 4;
        break;
    case 1:
        data = soclib::common::sign_ext(data, 8);
        byte_count = 4;
        break;
    case 8:
        data = !data;
        break;
    case -2:
        data = data & 0xffff;
        byte_count = 4;
        break;
    case -1:
        data = data & 0xff;
        byte_count = 4;
        break;
    }

    data_t mask = be_to_mask<data_t>(((1<<byte_count)-1) & 0xf);
    data <<= 8*r_mem_offset_byte_in_reg;
    mask <<= 8*r_mem_offset_byte_in_reg;

    data_t new_data = (data&mask) | (*r_mem_dest&~mask);
#ifdef SOCLIB_MODULE_DEBUG
    if (m_debug_mask & MIPS32_DEBUG_DATA) {
        std::cout
            << name()
            << " setData: " << rsp
            << " off: " << r_mem_offset_byte_in_reg
            << " count: " << r_mem_byte_count
            << " le: " << r_mem_byte_le
            << " old: " << *r_mem_dest
            << " from_mem: " << rsp.rdata
            << " mask: " << mask
            << " new_data: " << new_data
            << std::endl;
    }
#endif
    *r_mem_dest = new_data;

    return true;
}

/* except: L stands for Load, S stands for store */
#define check_align(address, align, except) \
    if ( (address)%(align) ) {              \
        r_bar = address;                    \
        m_exception = X_ADE##except;        \
        return;                             \
    }

// Loads

void Mips32Iss::op_lb()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    do_mem_access(address, 1, 1, &r_gp[m_ins.i.rt], 0, 0, DATA_READ);
}

void Mips32Iss::op_lbu()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    do_mem_access(address, 1, -1, &r_gp[m_ins.i.rt], 0, 0, DATA_READ);
}

void Mips32Iss::op_lh()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    check_align(address, 2, L);
    do_mem_access(address, 2, 2, &r_gp[m_ins.i.rt], 0, 0, DATA_READ);
}

void Mips32Iss::op_lhu()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    check_align(address, 2, L);
    do_mem_access(address, 2, -2, &r_gp[m_ins.i.rt], 0, 0, DATA_READ);
}

void Mips32Iss::op_lw()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    check_align(address, 4, L);
    do_mem_access(address, 4, 0, &r_gp[m_ins.i.rt], 0, 0, DATA_READ);
}

// Stores

void Mips32Iss::op_sb()
{
    uint32_t tmp = r_gp[m_ins.i.rt]&0xff;
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    do_mem_access(address, 1, 0, NULL, 0, tmp, DATA_WRITE);
}

void Mips32Iss::op_sh()
{
    uint32_t tmp = r_gp[m_ins.i.rt]&0xffff;
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    check_align(address, 2, S);
    do_mem_access(address, 2, 0, NULL, 0, tmp, DATA_WRITE);
}

void Mips32Iss::op_sw()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    check_align(address, 4, S);
    do_mem_access(address, 4, 0, NULL, 0, r_gp[m_ins.i.rt], DATA_WRITE);
}

// Unaligned accesses

void Mips32Iss::op_lwl()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    uint32_t w = address&3;
    int dest_byte = m_little_endian ?
        (3 - w):
        (w);
    int byte_count = m_little_endian ?
        (1 + w):
        (4 - w);
    if ( m_little_endian )
        address &= ~3;
    do_mem_access(address, byte_count, 0, &r_gp[m_ins.i.rt], dest_byte, 0, DATA_READ);
}

void Mips32Iss::op_swl()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    uint32_t w = address&3;
    int byte_count = m_little_endian ?
        (1 + w):
        (4 - w);
    if ( m_little_endian )
        address &= ~3;
    data_t data = r_gp[m_ins.i.rt];
    data >>= 8*(4 - byte_count);
    do_mem_access(address, byte_count, 0, NULL, 0, data, DATA_WRITE);
}

void Mips32Iss::op_lwr()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    uint32_t w = address&3;
    int byte_count = m_little_endian ?
        (4 - w):
        (1 + w);
    if ( ! m_little_endian )
        address &= ~3;
    do_mem_access(address, byte_count, 0, &r_gp[m_ins.i.rt], 0, 0, DATA_READ);
}

void Mips32Iss::op_swr()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    uint32_t w = address&3;
    int byte_count = m_little_endian ?
        (4 - w):
        (1 + w);
    if ( ! m_little_endian )
        address &= ~3;
    do_mem_access(address, byte_count, 0, NULL, 0, r_gp[m_ins.i.rt], DATA_WRITE);
}

// Atomic accesses

void Mips32Iss::op_ll()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    check_align(address, 4, L);
    do_mem_access(address, 4, 0, &r_gp[m_ins.i.rt], 0, 0, DATA_LL);
}

void Mips32Iss::op_sc()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    check_align(address, 4, S);
    do_mem_access(address, 4, 8, &r_gp[m_ins.i.rt], 0, r_gp[m_ins.i.rt], DATA_SC);
}

// Prefetch

void Mips32Iss::op_pref()
{
    uint32_t address =  r_gp[m_ins.i.rs] + sign_ext(m_ins.i.imd, 16);
    // ignored hint field
    // uint32_t hint = m_ins.i.rt;
    do_mem_access(4*XTN_DCACHE_PREFETCH, 4, false, NULL, 0, address, XTN_WRITE);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
