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
 * $Id: arm_instructions.cpp 2333 2013-05-27 14:05:09Z becoulet $
 *
 */

#include <cassert>

#include "arm.h"
#include "arithmetics.h"
#include "soclib_endian.h"

namespace soclib { namespace common {

using namespace soclib::common;

void ArmIss::arm_bx()
{
	if (!cond_eval())
		return;

    addr_t dest = r_gp[m_opcode.brx.rn];
	r_gp[15] = dest & ~1;
    r_cpsr.thumb = dest & 1;
}

void ArmIss::arm_blx()
{
	if (!cond_eval())
		return;

    addr_t dest = r_gp[m_opcode.brx.rn];
    r_gp[14] = r_gp[15];
	r_gp[15] = dest & ~1;
    r_cpsr.thumb = dest & 1;
}

void ArmIss::arm_b()
{
	if (!cond_eval())
		return;

	r_gp[15] += sign_ext(m_opcode.brl.offset << 2, 26) + 4;
}

void ArmIss::arm_bl()
{
	if (!cond_eval())
		return;

    r_gp[14] = r_gp[15];
	r_gp[15] += sign_ext(m_opcode.brl.offset << 2, 26) + 4;
}

void ArmIss::arm_cdp()
{

}

void ArmIss::arm_ill()
{
    m_exception = EXCEPT_UNDEF;
}

void ArmIss::arm_ldc()
{

}

void ArmIss::do_microcoded_ldstm_user()
{
	assert( m_microcode_opcode.bdt.reg_list );

	int reg_to_xfer = ctz(m_microcode_opcode.bdt.reg_list);
	m_microcode_opcode.bdt.reg_list &= ~(1<<reg_to_xfer);
    data_t *reg = &r_gp[reg_to_xfer];

    if ( (r_cpsr.mode == MOD_PSR_FIQ32) &&
         (reg_to_xfer >= 8) &&
         (reg_to_xfer <= 12) )
        reg = &r_r8_r12[MOD_USER32][reg_to_xfer-8];
    if ( (reg_to_xfer >= 13) &&
         (reg_to_xfer <= 14) )
        reg = &r_r13_r14[MOD_USER32][reg_to_xfer-13];

	if ( m_microcode_opcode.bdt.reg_list == 0 )
		m_microcode_func = NULL;

	if ( m_microcode_opcode.bdt.load_store ) {
		do_mem_access(m_microcode_status.bdt.base_address,
					  DATA_READ, 4, 0, reg, POST_OP_WB_UNSIGNED );
	} else {
		do_mem_access(m_microcode_status.bdt.base_address,
					  DATA_WRITE, 4, *reg, NULL, POST_OP_NONE );
	}
    m_microcode_status.bdt.base_address += 4;
    if ( reg_to_xfer == 15 && m_microcode_opcode.bdt.load_store )
        cpsr_update(r_spsr[psr_to_mode[r_cpsr.mode]]);
}

void ArmIss::do_microcoded_ldstm()
{
	assert( m_microcode_opcode.bdt.reg_list );

	int reg_to_xfer = ctz(m_microcode_opcode.bdt.reg_list);
	m_microcode_opcode.bdt.reg_list &= ~(1<<reg_to_xfer);

	if ( m_microcode_opcode.bdt.reg_list == 0 )
		m_microcode_func = NULL;

	if ( m_microcode_opcode.bdt.load_store ) {
		do_mem_access(m_microcode_status.bdt.base_address,
					  DATA_READ, 4, 0, &r_gp[reg_to_xfer], POST_OP_WB_UNSIGNED );
	} else {
		do_mem_access(m_microcode_status.bdt.base_address,
					  DATA_WRITE, 4, r_gp[reg_to_xfer], NULL, POST_OP_NONE );
	}
    m_microcode_status.bdt.base_address += 4;
}

void ArmIss::do_sleep()
{
    if ( m_irq_in ) {
#if defined(SOCLIB_MODULE_DEBUG)
    std::cout << name() << " out of sleep" << std::endl;
#endif
        m_microcode_func = NULL;
    }
}

void ArmIss::arm_ldstm()
{
	if (!cond_eval() || !m_opcode.bdt.reg_list)
		return;

	m_microcode_opcode = m_opcode;

	addr_t offset = popcount(m_opcode.bdt.reg_list) * 4;
	addr_t base_address = r_gp[m_opcode.bdt.rn];
    addr_t wbck_address = base_address;

    if ( m_opcode.bdt.up_down ) {
        wbck_address += offset;
    } else {
        wbck_address -= offset;
        base_address = wbck_address;
    }
    if ( m_opcode.bdt.pre_post == m_opcode.bdt.up_down )
        base_address += 4;

    if ( base_address & 0x3 ) {
        m_exception = EXCEPT_DABT;
        return;
    }

	m_microcode_status.bdt.base_address = base_address;
    if ( m_opcode.bdt.force_user ) {
        m_microcode_func = &ArmIss::do_microcoded_ldstm_user;
        do_microcoded_ldstm_user();
    } else {
        m_microcode_func = &ArmIss::do_microcoded_ldstm;
        m_ldstm_sp_offset = 0;
        if ( m_opcode.bdt.write_back ) {
            if ( m_opcode.bdt.rn == 13 && m_opcode.bdt.up_down )
                m_ldstm_sp_offset = offset;
            r_gp[m_opcode.bdt.rn] = wbck_address;
        }
        do_microcoded_ldstm();
    }
}

// to coproc
void ArmIss::arm_mcr()
{
	if (!cond_eval())
		return;

#ifdef SOCLIB_MODULE_DEBUG
    std::cout << name()
              << " mcr " << m_opcode.coproc.crn
              << ", " << m_opcode.coproc.opcode1
              << ", " << m_opcode.coproc.crm
              << ", " << m_opcode.coproc.opcode2
              << ": " << r_gp[m_opcode.coproc.rd];
#endif

    if ( ! coproc_put(
             m_opcode.coproc.cp_no,
             m_opcode.coproc.crn,
             m_opcode.coproc.opcode1,
             m_opcode.coproc.crm,
             m_opcode.coproc.opcode2,
             r_gp[m_opcode.coproc.rd] ) ) {
#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << "error" << std::endl;
#endif
        return arm_ill();
    } else {
#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << "ok" << std::endl;
#endif
    }
}

// from coproc
void ArmIss::arm_mrc()
{
	if (!cond_eval())
		return;

#if defined(SOCLIB_MODULE_DEBUG)
    std::cout << name()
              << " mrc " << m_opcode.coproc.crn
              << ", " << m_opcode.coproc.opcode1
              << ", " << m_opcode.coproc.crm
              << ", " << m_opcode.coproc.opcode2
              << ": ";
#endif

    if ( ! coproc_get(
             m_opcode.coproc.cp_no,
             m_opcode.coproc.crn,
             m_opcode.coproc.opcode1,
             m_opcode.coproc.crm,
             m_opcode.coproc.opcode2,
             r_gp[m_opcode.coproc.rd] ) ) {
#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << "error" << std::endl;
#endif
        return arm_ill();
    } else {
#if defined(SOCLIB_MODULE_DEBUG)
        std::cout << "ok: " << r_gp[m_opcode.coproc.rd] << std::endl;
#endif
    }
}

void ArmIss::arm_mrs()
{
	if (!cond_eval())
		return;

    if ( m_opcode.dp.rn != 0xf )
        return arm_ill();

	if (m_opcode.ms.p)
		r_gp[m_opcode.ms.rd] = r_spsr[psr_to_mode[r_cpsr.mode]].whole;
	else
		r_gp[m_opcode.ms.rd] = r_cpsr.whole;
}

void ArmIss::arm_msr()
{
	if (!cond_eval())
		return;

    if ( m_opcode.ms.p && r_cpsr.mode == MOD_PSR_USER32 )
        return arm_ill();

    ArmMode cur_mode = psr_to_mode[r_cpsr.mode];
    assert(cur_mode < MOD_Count);

	psr_t oldval = m_opcode.ms.p
        ? r_spsr[cur_mode]
        : r_cpsr;

	psr_t newval;

    data_t psr_mask =
        (m_opcode.ms.f * 0xff000000) |
        (m_opcode.ms.s * 0x00ff0000) |
        (m_opcode.ms.x * 0x0000ff00) |
        (m_opcode.ms.c * 0x000000ff);

    if (r_cpsr.mode == MOD_PSR_USER32)
        psr_mask &= 0xf0000000;

    data_t tmp = m_opcode.ms.i
		? (m_opcode.rot.immval << m_opcode.rot.rotate)
		: r_gp[m_opcode.ms.rm];
   
    newval.whole = (~psr_mask & oldval.whole) | (psr_mask & tmp);

#ifdef SOCLIB_MODULE_DEBUG
	std::cout
		<< name()
        << " msr p: " << m_opcode.ms.p
		<< " " << std::hex << r_cpsr.whole << " -> " << newval.whole
        << " (mask: " << psr_mask << ")"
        << " (new: " << tmp << ")"
		<< std::endl;
#endif

	if (m_opcode.ms.p) {
		r_spsr[cur_mode] = newval;
    } else {
        cpsr_update(newval);
    }
}

void ArmIss::arm_clz()
{
	if (!cond_eval())
		return;

    data_t d = r_gp[m_opcode.dp.rm];
    if ( d )
        r_gp[m_opcode.dp.rd] = soclib::common::clz<data_t>(d);
    else
        r_gp[m_opcode.dp.rd] = 32;
}

void ArmIss::arm_stc()
{

}

template<size_t byte_count, bool pre, bool load, bool signed_>
void ArmIss::arm_ldstrh()
{
	if (!cond_eval())
		return;

	addr_t addr = r_gp[m_opcode.sdth.rn];

	if ( m_opcode.sdth.rn == 15 )
		addr += 4;

	addr_t offset = m_opcode.sdth.immediate
        ? (m_opcode.sdth.im_up << 4) | m_opcode.sdth.rm
        : r_gp[m_opcode.sdth.rm];

	offset *= -1 + 2 * m_opcode.sdth.up_down;

	if (pre)
		addr += offset;

	if ( load ) {
		do_mem_access( addr, DATA_READ, byte_count, 0, &r_gp[m_opcode.sdth.rd],
					   signed_ ? POST_OP_WB_SIGNED : POST_OP_WB_UNSIGNED );
	} else {
		data_t value = r_gp[m_opcode.sdth.rd];
		if ( m_opcode.sdth.rd == 15 )
			value += 8;
		do_mem_access(addr, DATA_WRITE, byte_count, value, NULL, POST_OP_NONE );
	}

	if (!pre || m_opcode.sdth.write_back) {
		if (!pre) {
			addr += offset;
            if ( m_opcode.sdth.rn == 13 )
                m_ldstm_sp_offset = 4;
        }
		r_gp[m_opcode.sdth.rn] = addr;
	}
}

template<bool reg, bool pre, bool load>
void ArmIss::arm_ldstr()
{
	if (!cond_eval())
		return;

	addr_t addr = r_gp[m_opcode.sdt.rn];

	if ( m_opcode.sdt.rn == 15 )
		addr += 4;

	addr_t offset = reg
		? arm_shifter_shift<false>()
		: m_opcode.sdt.offset;

	offset *= -1 + 2 * m_opcode.sdt.up_down;

	if (pre)
		addr += offset;

	int byte_count = 4 - m_opcode.sdt.byte_word * 3;

	if ( load ) {
        if ( __builtin_expect( byte_count == 4 && (addr & 3) == 2, false ) )
            do_mem_access(addr & ~3, DATA_READ, 4, 0, &r_gp[m_opcode.sdt.rd], POST_OP_WB_SWAP_HALFWORDS );
        else
            do_mem_access(addr, DATA_READ, byte_count, 0, &r_gp[m_opcode.sdt.rd], POST_OP_WB_UNSIGNED );
	} else {
		data_t value = r_gp[m_opcode.sdt.rd];
		if ( m_opcode.sdt.rd == 15 )
			value += 8;
		do_mem_access(addr, DATA_WRITE, byte_count, value, NULL, POST_OP_NONE );
	}

	if (!pre || m_opcode.sdt.write_back) {
		if (!pre) {
			addr += offset;
            if ( m_opcode.sdt.rn == 13 )
                m_ldstm_sp_offset = 4;
        }
		r_gp[m_opcode.sdt.rn] = addr;
	}
}

void ArmIss::arm_swi()
{
	if (!cond_eval())
		return;

    m_exception = EXCEPT_SWI;
}

void ArmIss::arm_bkpt()
{
	if (!cond_eval())
		return;

    m_exception = EXCEPT_SWI;
}

void ArmIss::do_microcoded_swp_ll()
{
	do_mem_access(m_microcode_status.swp.address, DATA_LL,
				  4, 0, &m_microcode_status.swp.tmp_data,
				  POST_OP_WB_UNSIGNED );
	m_microcode_func = &ArmIss::do_microcoded_swp_sc;
}

void ArmIss::do_microcoded_swp_sc()
{
	data_t wdata;
	if ( m_microcode_opcode.swp.b ) {
		// TODO : support big-endian bus mode

		int offset = (8 * (m_microcode_status.swp.address & 3));
		data_t mask = ~(0xff << offset);
		data_t interleave = (0xff & r_gp[m_microcode_opcode.swp.rm]) << offset;
		wdata = interleave | (mask & m_microcode_status.swp.tmp_data);

		r_gp[m_microcode_opcode.swp.rd] = (m_microcode_status.swp.tmp_data >> offset) & 0xff;
	} else {
		wdata  = r_gp[m_opcode.swp.rm];

		r_gp[m_microcode_opcode.swp.rd] = m_microcode_status.swp.tmp_data;
        if ( wdata == m_microcode_status.swp.tmp_data ) {
            m_microcode_func = NULL;
            return;
        }
	}
	do_mem_access(m_microcode_status.swp.address, DATA_SC,
				  4, wdata, &m_microcode_status.swp.tmp_data,
				  POST_OP_WB_UNSIGNED );
	m_microcode_func = &ArmIss::do_microcoded_swp_decide;
}

void ArmIss::do_microcoded_swp_decide()
{
	if ( m_microcode_status.swp.tmp_data != Iss2::SC_ATOMIC ) {
		do_microcoded_swp_ll();
	} else {
		m_microcode_func = NULL;
	}
}

void ArmIss::arm_swp()
{
	if (!cond_eval())
		return;

	m_microcode_opcode = m_opcode;
	m_microcode_status.swp.address = r_gp[m_opcode.swp.rn];
	do_microcoded_swp_ll();
}

void ArmIss::arm_strex()
{
	if (!cond_eval())
		return;

	do_mem_access(r_gp[m_opcode.atomic.rn], DATA_SC,
				  4, r_gp[m_opcode.atomic.rm],
                  &r_gp[m_opcode.atomic.rd],
				  POST_OP_WB_SC );
}

void ArmIss::arm_ldrex()
{
	if (!cond_eval())
		return;

	do_mem_access(r_gp[m_opcode.atomic.rn], DATA_LL,
				  4, 0, &r_gp[m_opcode.atomic.rd],
				  POST_OP_WB_UNSIGNED );
}

ArmIss::data_t ArmIss::x_get_rot() const
{
    size_t shift = m_opcode.dp.shift_code >> 3;
    data_t data = r_gp[m_opcode.dp.rm];
    return (data >> shift) | (data << (32-shift));
}

void ArmIss::arm_rev()
{
	if (!cond_eval())
		return;

    r_gp[m_opcode.dp.rd] = soclib::endian::uint32_swap(r_gp[m_opcode.dp.rm]);
}

void ArmIss::arm_sxtb()
{
	if (!cond_eval())
		return;

    data_t rm = x_get_rot() & 0xff;
    r_gp[m_opcode.dp.rd] = sign_ext(rm, 8);
}

void ArmIss::arm_uxtb()
{
	if (!cond_eval())
		return;

    data_t rm = x_get_rot() & 0xff;
    r_gp[m_opcode.dp.rd] = rm;
}

void ArmIss::arm_rev16()
{
	if (!cond_eval())
		return;

    data_t rm = soclib::endian::uint32_swap(r_gp[m_opcode.dp.rm]);
    r_gp[m_opcode.dp.rd] = (rm<<16) | (rm>>16);
}

void ArmIss::arm_uxtb16()
{
	if (!cond_eval())
		return;

    data_t rm = x_get_rot();
    rm &= 0x00ff00ff;
    r_gp[m_opcode.dp.rd] = rm;
}

void ArmIss::arm_sxth()
{
	if (!cond_eval())
		return;

    data_t rm = x_get_rot();
    r_gp[m_opcode.dp.rd] = sign_ext(rm, 16);
}

void ArmIss::arm_uxth()
{
	if (!cond_eval())
		return;

    data_t rm = x_get_rot();
    r_gp[m_opcode.dp.rd] = rm & 0xffff;
}

void ArmIss::arm_revsh()
{
	if (!cond_eval())
		return;

    uint16_t rm = soclib::endian::uint16_swap(r_gp[m_opcode.dp.rm]);
    r_gp[m_opcode.dp.rd] = sign_ext(rm, 16);
}

void ArmIss::arm_sxtb16()
{
	if (!cond_eval())
		return;

    data_t rm = x_get_rot();
    rm &= 0x00ff00ff;
    rm |= (rm & 0x00000080) ? 0x0000ff00 : 0;
    r_gp[m_opcode.dp.rd] = sign_ext(rm, 24);
}


template void ArmIss::arm_ldstrh<2, false, false, false>();
template void ArmIss::arm_ldstrh<1, false, false, true>();
template void ArmIss::arm_ldstrh<2, false, false, true>();
template void ArmIss::arm_ldstrh<2, true, false, false>();
template void ArmIss::arm_ldstrh<1, true, false, true>();
template void ArmIss::arm_ldstrh<2, true, false, true>();
template void ArmIss::arm_ldstrh<2, false, true, false>();
template void ArmIss::arm_ldstrh<1, false, true, true>();
template void ArmIss::arm_ldstrh<2, false, true, true>();
template void ArmIss::arm_ldstrh<2, true, true, false>();
template void ArmIss::arm_ldstrh<1, true, true, true>();
template void ArmIss::arm_ldstrh<2, true, true, true>();

template void ArmIss::arm_ldstr<false, false, false>();
template void ArmIss::arm_ldstr<false, true, false>();
template void ArmIss::arm_ldstr<true, false, false>();
template void ArmIss::arm_ldstr<true, true, false>();
template void ArmIss::arm_ldstr<false, false, true>();
template void ArmIss::arm_ldstr<false, true, true>();
template void ArmIss::arm_ldstr<true, false, true>();
template void ArmIss::arm_ldstr<true, true, true>();


void ArmIss::arm_uncond_ill()
{
    arm_ill();
}

void ArmIss::arm_uncond_cps_setend()
{
    if (m_opcode.setend.is_setend) {
        r_cpsr.endian = m_opcode.setend.be;

    } else {  // cps
        arm_ill();
    }
}

void ArmIss::arm_uncond_avsimd_data()
{
    arm_ill();
}

void ArmIss::arm_uncond_avsimd_ldst()
{
    arm_ill();
}

void ArmIss::arm_uncond_nop()
{
}

void ArmIss::arm_uncond_pli()
{
    arm_ill();
}

void ArmIss::arm_uncond_pld()
{
    arm_ill();
}

void ArmIss::arm_uncond_clrex()
{
    arm_ill();
}

void ArmIss::arm_uncond_dsb()
{
    arm_ill();
}

void ArmIss::arm_uncond_dmb()
{
    arm_ill();
}

void ArmIss::arm_uncond_isb()
{
    arm_ill();
}

void ArmIss::arm_uncond_srs()
{
    arm_ill();
}

void ArmIss::arm_uncond_rfe()
{
    arm_ill();
}

void ArmIss::arm_uncond_blx()
{
    arm_ill();
}

void ArmIss::arm_uncond_ldci()
{
    arm_ill();
}

void ArmIss::arm_uncond_stci()
{
    arm_ill();
}

void ArmIss::arm_uncond_mcrr()
{
    arm_ill();
}

void ArmIss::arm_uncond_mrrc()
{
    arm_ill();
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
