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
 * $Id: thumb_instructions.cpp 1694 2010-04-05 18:22:46Z nipo $
 *
 */

#include <cassert>

#include "arm.h"
#include "arithmetics.h"
#include "soclib_endian.h"

namespace soclib { namespace common {

using namespace soclib::common;

void ArmIss::thumb_get_reghi(uint8_t &rm, uint8_t &rd)
{
    rm = m_thumb_op.hilo.rm + (m_thumb_op.hilo.h2<<3);
    rd = m_thumb_op.hilo.rd + (m_thumb_op.hilo.h1<<3);
}

void ArmIss::thumb_ill()
{
    m_exception = EXCEPT_UNDEF;
}

template<bool link>
void ArmIss::thumb_bx_r()
{
    uint8_t rm, rd;
    thumb_get_reghi(rm, rd);

    addr_t dest = r_gp[rm];
    if ( rm == 15 )
        dest += 2;

    if ( link )
        r_gp[14] = r_gp[15] | 1;

    r_cpsr.thumb = dest & 1;
	r_gp[15] = dest & ~1;
}

void ArmIss::thumb_b()
{
    r_gp[15] += 2 + (sign_ext(m_thumb_op.imm11.imm, 11) << 1);
}

void ArmIss::thumb_b_hi()
{
    r_gp[14] = r_gp[15] + (sign_ext(m_thumb_op.imm11.imm, 11) << 12) + 2;
}

void ArmIss::thumb_bl()
{
    addr_t target = r_gp[14];
    r_gp[14] = r_gp[15] | 1;
    r_gp[15] = target + (m_thumb_op.imm11.imm << 1);
}

void ArmIss::thumb_blx()
{
    addr_t target = r_gp[14];
    r_gp[14] = r_gp[15] | 1;
    r_gp[15] = (target + (m_thumb_op.imm11.imm << 1)) & ~3;
    r_cpsr.thumb = 0;
}

void ArmIss::thumb_cpy()
{
    uint8_t rm, rd;
    uint32_t addend = 0;
    thumb_get_reghi(rm, rd);
    if ( rm == 15 )
        addend += 2;

    r_gp[rd] = r_gp[rm] + addend;
}

void ArmIss::thumb_add_hi()
{
    uint8_t rm, rd;
    uint32_t addend = 0;
    thumb_get_reghi(rm, rd);
    if ( rm == 15 )
        addend += 2;
    if ( rd == 15 )
        addend += 2;

    r_gp[rd] += r_gp[rm] + addend;
}

void ArmIss::thumb_mul()
{
    data_t res = r_gp[m_thumb_op.reg2.rm] * r_gp[m_thumb_op.reg2.rd];

    r_gp[m_thumb_op.reg2.rd] = res;
    r_cpsr.zero = !res;
    r_cpsr.sign = ((int32_t)res) < 0;
}

void ArmIss::thumb_sp_add()
{
    r_gp[0xd] += m_thumb_op.imm7.imm << 2;
}

void ArmIss::thumb_sp_sub()
{
    r_gp[0xd] -= m_thumb_op.imm7.imm << 2;
}

void ArmIss::thumb_add2pc()
{
    r_gp[m_thumb_op.imm8.rd] = r_gp[15] + m_thumb_op.imm8.imm * 4;
}

void ArmIss::thumb_add2sp()
{
    r_gp[m_thumb_op.imm8.rd] = r_gp[13] + m_thumb_op.imm8.imm * 4;
}

void ArmIss::thumb_cps_setend()
{
    if ( m_thumb_op.cps.is_cps ) {
        ArmMode cur_mode = psr_to_mode[r_cpsr.mode];
        if ( cur_mode == MOD_USER32 )
            return;

        // We dont support 'a' bit
//         if ( m_thumb_op.cps.a )
//             r_cpsr.a = m_thumb_op.cps.imod;
        if ( m_thumb_op.cps.i )
            r_cpsr.irq_disabled = m_thumb_op.cps.imod;
        if ( m_thumb_op.cps.f )
            r_cpsr.fiq_disabled = m_thumb_op.cps.imod;
    } else {
        // 101101100101E000;

        assert(! "Unsupported operation");
    }
}

void ArmIss::thumb_bkpt()
{
    m_exception = EXCEPT_SWI;
}

static const char * const cond_code[16] = {
	"EQ","NE","CS","CC","MI","PL","VS","VC",
	"HI","LS","GE","LT","GT","LE","AL","NV",
};

static const char * const flag_code[16] = {
	"    ","   V","  C ","  CV",
	" Z  "," Z V"," ZC "," ZCV",
	"-   ","-  V","- C ","- CV",
	"-Z  ","-Z V","-ZC ","-ZCV",
};

void ArmIss::thumb_bcond_swi()
{
    if ( m_thumb_op.cond.cond == 0xf ) {
        m_exception = EXCEPT_SWI;
    } else {
        uint16_t cond_word = cond_table[m_thumb_op.cond.cond];
#ifdef SOCLIB_MODULE_DEBUG
        std::cout
            << m_name << " bcond"
            << " [" << flag_code[r_cpsr.flags] << "]"
            << " + " << cond_code[m_thumb_op.cond.cond];
#endif
        if ( (cond_word >> r_cpsr.flags) & 1 ) {
            r_gp[15] += 2 + (sign_ext(m_thumb_op.cond.imm, 8) << 1);
#ifdef SOCLIB_MODULE_DEBUG
            std::cout << " taken: next_pc = " << r_gp[15] << std::endl;
        } else {
            std::cout << " not taken" << std::endl;
#endif
        }
    }
}

template<bool load>
void ArmIss::thumb_ldstmia()
{
    m_opcode.ins = 0xe8800000;
    m_opcode.bdt.load_store = load;
    m_opcode.bdt.rn = m_thumb_op.ldstm.rn;
    m_opcode.bdt.write_back =
        load
        ? !((m_thumb_op.ldstm.regs >> m_thumb_op.ldstm.rn) & 1)
        : 1;
    m_opcode.bdt.reg_list = m_thumb_op.ldstm.regs;

    arm_ldstm();
}

void ArmIss::thumb_pop()
{
    m_opcode.ins = 0xe8bd0000;

    m_opcode.bdt.reg_list = m_thumb_op.push_pop.regs
        | (m_thumb_op.push_pop.r << 15);

    arm_ldstm();
}

void ArmIss::thumb_push()
{
    m_opcode.ins = 0xe92d0000;

    m_opcode.bdt.reg_list = m_thumb_op.push_pop.regs
        | (m_thumb_op.push_pop.r << 14);

    arm_ldstm();
}

template<bool unsigned_, bool byte>
void ArmIss::thumb_xt()
{
    if ( unsigned_ )
        r_gp[m_thumb_op.reg2.rd] =
            r_gp[m_thumb_op.reg2.rm] &
            (byte ? 0xff : 0xffff);
    else
        r_gp[m_thumb_op.reg2.rd] =
            sign_ext(r_gp[m_thumb_op.reg2.rm],
                     (byte ? 8 : 16));
}

void ArmIss::thumb_rev()
{
    r_gp[m_thumb_op.reg2.rd] = 
        soclib::endian::uint32_swap(r_gp[m_thumb_op.reg2.rm]);
}

void ArmIss::thumb_rev16()
{
    data_t rm = soclib::endian::uint32_swap(r_gp[m_thumb_op.reg2.rm]);
    r_gp[m_thumb_op.reg2.rd] = (rm<<16) | (rm>>16);
}

void ArmIss::thumb_revsh()
{
    uint16_t rm = soclib::endian::uint16_swap(r_gp[m_thumb_op.reg2.rm]);
    r_gp[m_thumb_op.reg2.rd] = sign_ext(rm, 16);
}

void ArmIss::thumb_ldst()
{
	addr_t addr = r_gp[m_thumb_op.ldst.rn]
        + r_gp[m_thumb_op.ldst.rm];

    switch (m_thumb_op.ldst.mode) {
    case 0: // str
        do_mem_access(addr, DATA_WRITE, 4,
                      r_gp[m_thumb_op.ldst.rd],
                      NULL, POST_OP_NONE );
        break;
    case 1: // strh
        do_mem_access(addr, DATA_WRITE, 2,
                      r_gp[m_thumb_op.ldst.rd],
                      NULL, POST_OP_NONE );
        break;
    case 2: // strb
        do_mem_access(addr, DATA_WRITE, 1,
                      r_gp[m_thumb_op.ldst.rd],
                      NULL, POST_OP_NONE );
        break;
    case 3: // ldrsb
		do_mem_access(addr, DATA_READ, 1,
                      0, &r_gp[m_thumb_op.ldst.rd],
                      POST_OP_WB_SIGNED );
        break;
    case 4: // ldr
		do_mem_access(addr, DATA_READ, 4,
                      0, &r_gp[m_thumb_op.ldst.rd],
                      POST_OP_WB_UNSIGNED );
        break;
    case 5: // ldrh
		do_mem_access(addr, DATA_READ, 2,
                      0, &r_gp[m_thumb_op.ldst.rd],
                      POST_OP_WB_UNSIGNED );
        break;
    case 6: // ldrb
		do_mem_access(addr, DATA_READ, 1,
                      0, &r_gp[m_thumb_op.ldst.rd],
                      POST_OP_WB_UNSIGNED );
        break;
    case 7: // ldrsh
		do_mem_access(addr, DATA_READ, 2,
                      0, &r_gp[m_thumb_op.ldst.rd],
                      POST_OP_WB_SIGNED );
        break;
    }
}

void ArmIss::thumb_ldr_pcrel()
{
    do_mem_access(((r_gp[15] + 2) & ~3) + (m_thumb_op.imm8.imm << 2),
                  DATA_READ,
                  4,
                  0,
                  &r_gp[m_thumb_op.imm8.rd],
                  POST_OP_WB_UNSIGNED );
}

template<bool load, bool byte>
void ArmIss::thumb_ldst_imm5()
{
	addr_t addr = r_gp[m_thumb_op.imm5.rn];

	int byte_count = byte ? 1 : 4;

    addr += m_thumb_op.imm5.imm * byte_count;

#ifdef SOCLIB_MODULE_DEBUG
    std::cout
        << m_name
        << " ldst_imm5, imm: " <<  m_thumb_op.imm5.imm
        << ", rn: " <<  m_thumb_op.imm5.rn
        << '(' << r_gp[m_thumb_op.imm5.rn]
        << "), addr: " << addr << std::endl;
#endif

	if ( load )
        do_mem_access(addr, DATA_READ,
                      byte_count, 0,
                      &r_gp[m_thumb_op.imm5.rd], POST_OP_WB_UNSIGNED );
    else
        do_mem_access(addr, DATA_WRITE,
                      byte_count, r_gp[m_thumb_op.imm5.rd],
                      NULL, POST_OP_NONE );
}

template<bool load>
void ArmIss::thumb_ldst_sprel()
{
    if ( load )
        do_mem_access(r_gp[0xd] + (m_thumb_op.imm8.imm << 2),
                      DATA_READ,
                      4,
                      0,
                      &r_gp[m_thumb_op.imm8.rd],
                      POST_OP_WB_UNSIGNED );
    else
        do_mem_access(r_gp[0xd] + (m_thumb_op.imm8.imm << 2),
                      DATA_WRITE,
                      4,
                      r_gp[m_thumb_op.imm8.rd],
                      NULL,
                      POST_OP_NONE );
}

void ArmIss::thumb_ldsth_imm5()
{
	addr_t addr = r_gp[m_thumb_op.imm5.rn]
        + (m_thumb_op.imm5.imm << 1);

	if ( m_thumb_op.imm5.decod0 & 1 ) // load
        do_mem_access(addr, DATA_READ,
                      2, 0,
                      &r_gp[m_thumb_op.imm5.rd], POST_OP_WB_UNSIGNED );
    else
        do_mem_access(addr, DATA_WRITE,
                      2, r_gp[m_thumb_op.imm5.rd],
                      NULL, POST_OP_NONE );
}




void ArmIss::thumb_lsl()
{
    m_opcode.ins = 0xe1b00000;
    m_opcode.dp.rd = m_thumb_op.reg2.rd;
    m_opcode.dp.rm = m_thumb_op.reg2.rd;
    m_opcode.dp.shift_code = (m_thumb_op.reg2.rm << 4) | 1;

    arm_movs();
}

void ArmIss::thumb_lsr()
{
    m_opcode.ins = 0xe1b00000;
    m_opcode.dp.rd = m_thumb_op.reg2.rd;
    m_opcode.dp.rm = m_thumb_op.reg2.rd;
    m_opcode.dp.shift_code = (m_thumb_op.reg2.rm << 4) | 3;

    arm_movs();
}

void ArmIss::thumb_ror()
{
    m_opcode.ins = 0xe1b00000;
    m_opcode.dp.rd = m_thumb_op.reg2.rd;
    m_opcode.dp.rm = m_thumb_op.reg2.rd;
    m_opcode.dp.shift_code = (m_thumb_op.reg2.rm << 4) | 7;

    arm_movs();
}

void ArmIss::thumb_lsl_imm5()
{
    m_opcode.ins = 0xe1b00000;
    m_opcode.dp.rd = m_thumb_op.imm5.rd;
    m_opcode.dp.rm = m_thumb_op.imm5.rn;
    m_opcode.dp.shift_code = m_thumb_op.imm5.imm << 3;

    arm_movs();
}

void ArmIss::thumb_lsr_imm5()
{
    m_opcode.ins = 0xe1b00000;
    m_opcode.dp.rd = m_thumb_op.imm5.rd;
    m_opcode.dp.rm = m_thumb_op.imm5.rn;
    m_opcode.dp.shift_code = (m_thumb_op.imm5.imm << 3) | 2;

    arm_movs();
}

void ArmIss::thumb_mov_imm8()
{
    r_gp[m_thumb_op.imm8.rd] = m_thumb_op.imm8.imm;
    r_cpsr.sign = 0;
    r_cpsr.zero = !m_thumb_op.imm8.imm;
}

void ArmIss::thumb_asr_imm5()
{
    m_opcode.ins = 0xe1b00000;
    m_opcode.dp.rd = m_thumb_op.imm5.rd;
    m_opcode.dp.rm = m_thumb_op.imm5.rn;
    m_opcode.dp.shift_code = (m_thumb_op.imm5.imm << 3) | 5;

    arm_movs();
}

void ArmIss::thumb_add_imm3()
{
    m_opcode.ins = 0xe2900000;
    m_opcode.dp.rn = m_thumb_op.reg3.rn;
    m_opcode.dp.rd = m_thumb_op.reg3.rd;
    m_opcode.dp.rm = m_thumb_op.reg3.rm;

	arm_adds();
}

void ArmIss::thumb_sub_imm3()
{
    m_opcode.ins = 0xe2500000;
    m_opcode.dp.rn = m_thumb_op.reg3.rn;
    m_opcode.dp.rd = m_thumb_op.reg3.rd;
    m_opcode.dp.rm = m_thumb_op.reg3.rm;

	arm_subs();
}

void ArmIss::thumb_add_imm8()
{
    m_opcode.ins = 0xe2900000;
    m_opcode.dp.rn = m_thumb_op.imm8.rd;
    m_opcode.dp.rd = m_thumb_op.imm8.rd;
    m_opcode.rot.immval = m_thumb_op.imm8.imm;

	arm_adds();
}

void ArmIss::thumb_sub_imm8()
{
    m_opcode.ins = 0xe2500000;
    m_opcode.dp.rn = m_thumb_op.imm8.rd;
    m_opcode.dp.rd = m_thumb_op.imm8.rd;
    m_opcode.rot.immval = m_thumb_op.imm8.imm;

	arm_subs();
}

void ArmIss::thumb_cmp_hi()
{
    uint8_t rm, rn;
    thumb_get_reghi(rm, rn);

    m_opcode.ins = 0xe1500000;
    m_opcode.dp.rn = rn;
    m_opcode.dp.rm = rm;

	arm_cmps();
}

void ArmIss::thumb_cmp_imm()
{
    m_opcode.ins = 0xe3500000;
    m_opcode.dp.rn = m_thumb_op.imm8.rd;
    m_opcode.rot.immval = m_thumb_op.imm8.imm;

	arm_cmps();
}

void ArmIss::thumb_asr()
{
    m_opcode.ins = 0xe1b00000;
    m_opcode.dp.rd = m_thumb_op.reg2.rd;
    m_opcode.dp.rm = m_thumb_op.reg2.rd;
    m_opcode.dp.shift_code = (m_thumb_op.reg2.rm << 4) | 5;

    arm_movs();
}

void ArmIss::thumb_add_reg()
{
    m_opcode.ins = 0xe0900000;
    m_opcode.dp.rn = m_thumb_op.reg3.rn;
    m_opcode.dp.rd = m_thumb_op.reg3.rd;
    m_opcode.dp.rm = m_thumb_op.reg3.rm;

	arm_adds();
}

void ArmIss::thumb_sub_reg()
{
    m_opcode.ins = 0xe0500000;
    m_opcode.dp.rn = m_thumb_op.reg3.rn;
    m_opcode.dp.rd = m_thumb_op.reg3.rd;
    m_opcode.dp.rm = m_thumb_op.reg3.rm;

	arm_subs();
}

template void ArmIss::thumb_ldst_imm5<true , false>();
template void ArmIss::thumb_ldst_imm5<true , true >();
template void ArmIss::thumb_ldst_imm5<false, false>();
template void ArmIss::thumb_ldst_imm5<false, true >();
template void ArmIss::thumb_ldst_sprel<false>();
template void ArmIss::thumb_ldst_sprel<true >();
template void ArmIss::thumb_xt<true , false>();
template void ArmIss::thumb_xt<true , true >();
template void ArmIss::thumb_xt<false, false>();
template void ArmIss::thumb_xt<false, true >();
template void ArmIss::thumb_ldstmia<false>();
template void ArmIss::thumb_ldstmia<true >();
template void ArmIss::thumb_bx_r<false>();
template void ArmIss::thumb_bx_r<true >();

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
