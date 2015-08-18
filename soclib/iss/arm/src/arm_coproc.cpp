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
 *         Alexandre Becoulet <alexandre.becoulet@free.fr>, 2009
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo becoulet
 *
 * $Id: arm_coproc.cpp 2344 2013-05-29 15:08:56Z becoulet $
 *
 */

#include "arm.h"
#include "arithmetics.h"

namespace soclib { namespace common {

#define REG_ID(crn, op1, crm, op2) ((((crn)&0xf)<<11)|(((op1)&0x7)<<7)|(((crm)&0xf)<<3)|(((op2)&0x7)<<0))

bool ArmIss::coproc_put(
	unsigned int cp_no,
	unsigned int crn,
	unsigned int opcode1,
	unsigned int crm,
	unsigned int opcode2,
	data_t val )
{
    if ( (cp_no != 15) || (opcode1 != 0) )
        return false;

    switch (REG_ID(crn, opcode1, crm, opcode2)) {
    case REG_ID(1, 0, 0, 0):
        r_sctlr.whole = val;
        return true;
    case REG_ID(7, 0, 0, 4):
#if defined(SOCLIB_MODULE_DEBUG)
    std::cout << name() << " sleeping" << std::endl;
#endif
        m_microcode_func = &ArmIss::do_sleep;
        return true;
    case REG_ID(7, 0, 6, 1):
		do_mem_access( XTN_DCACHE_INVAL*4, XTN_WRITE, 4, val, NULL, POST_OP_NONE );
        return true;
    case REG_ID(7, 0, 10, 4):
    do_mem_access(4*XTN_SYNC, XTN_READ, 4, 0, NULL, POST_OP_NONE);
        return true;
    case REG_ID(13, 0, 0, 2):
        m_tls_regs[0] = val;
    return true;
    case REG_ID(13, 0, 0, 3):
        if ( r_cpsr.mode == MOD_PSR_USER32 )
            return false;
        m_tls_regs[1] = val;
    return true;
    case REG_ID(13, 0, 0, 4):
        if ( r_cpsr.mode == MOD_PSR_USER32 )
            return false;
        m_tls_regs[2] = val;
    return true;
    default:
        break;
    }
    return false;
}

bool ArmIss::coproc_get(
	unsigned int cp_no,
	unsigned int crn,
	unsigned int opcode1,
	unsigned int crm,
	unsigned int opcode2,
	data_t &val )
{
    if ( (cp_no != 15) )
        return false;

    switch (REG_ID(crn, opcode1, crm, opcode2)) {
    case REG_ID(0, 0, 0, 0):
        val = 0x9d170000;
    return true;
    case REG_ID(1, 0, 0, 0):
        val = r_sctlr.whole;
    return true;
    case REG_ID(0, 0, 0, 1):
		val = m_cache_info.whole;
    return true;
    case REG_ID(15, 0, 12, 1):
        val = m_cycle_count;
    return true;
    case REG_ID(15, 0, 12, 2):
        val = m_run_count;
    return true;
    case REG_ID(6, 0, 0, 0):
        val = m_exception_dptr;
    return true;
    case REG_ID(6, 0, 0, 2):
        val = m_exception_pc;
    return true;
    case REG_ID(13, 0, 0, 2):
        val = m_tls_regs[0];
    return true;
    case REG_ID(13, 0, 0, 3):
        val = m_tls_regs[1];
    return true;
    case REG_ID(13, 0, 0, 4):
        if ( r_cpsr.mode == MOD_PSR_USER32 )
            return false;
        val = m_tls_regs[2];
        return true;
    case REG_ID(0, 0, 0, 5):
        val = m_ident;
    return true;
    default:
        break;
    }
    return false;
}

void ArmIss::setCacheInfo( const struct CacheInfo &info )
{
	data_t icache_size = info.icache_line_size*info.icache_assoc*info.icache_n_lines;
	m_cache_info.icache_len = std::min(
		uint32_log2(info.icache_line_size/8), 0);
	m_cache_info.icache_size = uint32_log2(icache_size/512);
	m_cache_info.icache_assoc = uint32_log2(info.icache_assoc);

	data_t dcache_size = info.dcache_line_size*info.dcache_assoc*info.dcache_n_lines;
	m_cache_info.dcache_len = std::min(
		uint32_log2(info.dcache_line_size/8), 0);
	m_cache_info.dcache_size = uint32_log2(dcache_size/512);
	m_cache_info.dcache_assoc = uint32_log2(info.dcache_assoc);
}

}}

