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
 * $Id: arm_alu.cpp 2333 2013-05-27 14:05:09Z becoulet $
 *
 */

#include "arm.h"
#include "arithmetics.h"
#include <cassert>

namespace soclib { namespace common {

/* update_carry must be true for AND, EOR, TST, TEQ, ORR, MOV, BIC, MVN */
template <bool update_carry>
uint32_t ArmIss::arm_shifter()
{
  uint32_t result;

  // rotate case
  if (m_opcode.dp.i) {
    uint32_t imm = m_opcode.rot.immval;
    uint8_t rotate = 2 * m_opcode.rot.rotate;

#ifdef SOCLIB_MODULE_DEBUG
    std::cout
        << "Arm shifter<" << update_carry << ">"
        << " i: " << m_opcode.dp.i
        << " imm: " << m_opcode.rot.immval
        << " rotate: " << m_opcode.rot.rotate;
#endif

    result = (imm >> rotate) | (imm << (32 - rotate));
  } else {

#ifdef SOCLIB_MODULE_DEBUG
  std::cout
      << "Arm shifter_shift<" << update_carry << ">"
      << " code: " << m_opcode.dp.shift_code
      << " rm: " << m_opcode.dp.rm
      << " *rm: " << r_gp[m_opcode.dp.rm];
#endif

      result = arm_shifter_shift<update_carry>();
  }

#ifdef SOCLIB_MODULE_DEBUG
  std::cout
      << " result: " << result << std::endl;
#endif

  return result;
}

template uint32_t ArmIss::arm_shifter<false>();

template <bool update_carry>
uint32_t ArmIss::arm_shifter_shift()
{
    // shift case
    uint32_t value = r_gp[m_opcode.dp.rm];

    if ( m_opcode.dp.rm == 15 ) {
        value += 4 - 2 * r_cpsr.thumb;
        if ( m_opcode.dp.i )
            value += 4 - 2 * r_cpsr.thumb;
    }

    uint8_t shift_code = m_opcode.dp.shift_code;

    if (!shift_code)
        return value;

    uint8_t shift;
    if ( shift_code & 1 ) {
        shift = r_gp[shift_code >> 4] & 0xff;
        if ( (shift_code >> 4) == 15 )
            shift += 8;
        if ( !shift )
            return value;
    } else {
        shift = shift_code >> 3;
    }

    switch (shift_code & 6)
    {
    case 0:         // shift logical left
        if (!shift)      // 0 means 32
            shift = 32;

        if (shift >= 32) {
            if (update_carry)
                r_cpsr.carry = shift == 32 ? value : 0;
            return 0;

        } else {
            if (update_carry)
                r_cpsr.carry = value << (shift - 1);
            return value << shift;
        }

    case 2:         // shift logical right
        if (!shift)      // 0 means 32
            shift = 32;

        if (shift >= 32) {
            if (update_carry)
                r_cpsr.carry = shift == 32 ? value >> 31 : 0;
            return 0;

        } else {
            if (update_carry)
                r_cpsr.carry = value >> (shift - 1);
            return value >> shift;
        }

    case 4:         // shift arith right
        if (!shift)      // 0 means 32
            shift = 32;

        if (shift > 32) {
            if (update_carry)
                r_cpsr.carry = value >> 31;
            return -1 * r_cpsr.carry;

        } else {
            if (update_carry)
                r_cpsr.carry = value >> (shift - 1);
            return (int32_t)value >> shift;
        }

    case 6:         // rotate right
        if (shift) {
            shift &= 0x1f;

            if (shift == 0) {
                if (update_carry)
                    r_cpsr.carry = value >> 31;
                return value;

            } else {
                if (update_carry)
                    r_cpsr.carry = value >> (shift - 1);
                return (value >> shift) | (value << (32 - shift));
            }

        } else {            // rotate extended
            uint32_t carry = r_cpsr.carry << 31;
            if (update_carry)
                r_cpsr.carry = value;
            return (value >> 1) | carry;
        }

    }

    abort();
}

template uint32_t ArmIss::arm_shifter_shift<false>();

#define ARM_DATA_LOGICAL_INS_S(n, op, write_result)                     \
                                                                        \
    ARM_OPS_PROTO(ArmIss::arm_##n##s)                                    \
    {                                                                   \
        if (!cond_eval())                                            \
            return;                                                     \
                                                                        \
        uint32_t shifted = (m_opcode.dp.rd == 15)                       \
            ? arm_shifter<false>()                                      \
            : arm_shifter<true>();                                      \
                                                                        \
        uint32_t op1 = r_gp[m_opcode.dp.rn];                            \
        if (m_opcode.dp.rn == 15) {                                     \
            op1 += 4 - 2 * r_cpsr.thumb;                                                   \
            if ( m_opcode.dp.i && (m_opcode.dp.shift_code & 0x10) )     \
                op1 += 4 - 2 * r_cpsr.thumb;                                               \
        }                                                               \
                                                                        \
        data_t res = arm_logical_##op(op1, shifted);                    \
                                                                        \
        if (write_result)                                               \
            r_gp[m_opcode.dp.rd] = res;                                 \
                                                                        \
        if (m_opcode.dp.rd == 15) {                                     \
            ArmMode cur_mode = psr_to_mode[r_cpsr.mode];               \
            assert(cur_mode < MOD_Count);                               \
            r_gp[m_opcode.dp.rd] = res & ~1;                            \
            cpsr_update(r_spsr[cur_mode]);                              \
            r_cpsr.thumb = res & 1;                                     \
        } else {                                                        \
            r_cpsr.zero = !res;                                         \
            r_cpsr.sign = res >> 31;                                    \
        }                                                               \
    }

#define ARM_DATA_LOGICAL_INS(n, op)                                     \
                                                                        \
    ARM_DATA_LOGICAL_INS_S(n, op, true)                                 \
                                                                        \
    ARM_OPS_PROTO(ArmIss::arm_##n)                                       \
    {                                                                   \
        if (!cond_eval())                                            \
            return;                                                     \
                                                                        \
        uint32_t op1 = r_gp[m_opcode.dp.rn];                            \
        if (m_opcode.dp.rn == 15) {                                     \
            op1 += 4 - 2 * r_cpsr.thumb;                                                   \
            if ( m_opcode.dp.i && (m_opcode.dp.shift_code & 0x10) )     \
                op1 += 4 - 2 * r_cpsr.thumb;                                               \
        }                                                               \
                                                                        \
        uint32_t res = arm_logical_##op(op1, arm_shifter<false>());     \
                                                                        \
        r_gp[m_opcode.dp.rd] = res;                                     \
    }

static inline uint32_t arm_logical_and(uint32_t a, uint32_t b)
{
  return a & b;
}

static inline uint32_t arm_logical_or(uint32_t a, uint32_t b)
{
  return a | b;
}

static inline uint32_t arm_logical_eor(uint32_t a, uint32_t b)
{
  return a ^ b;
}

static inline uint32_t arm_logical_mov(uint32_t a, uint32_t b)
{
  return b;
}

static inline uint32_t arm_logical_bic(uint32_t a, uint32_t b)
{
  return a & ~b;
}

static inline uint32_t arm_logical_mvn(uint32_t a, uint32_t b)
{
  return ~b;
}

ARM_DATA_LOGICAL_INS(and, and)
ARM_DATA_LOGICAL_INS(eor, eor)
ARM_DATA_LOGICAL_INS_S(tst, and, false)
ARM_DATA_LOGICAL_INS_S(teq, eor, false)
ARM_DATA_LOGICAL_INS(orr, or)
ARM_DATA_LOGICAL_INS(mov, mov)
ARM_DATA_LOGICAL_INS(bic, bic)
ARM_DATA_LOGICAL_INS(mvn, mvn)


#define ARM_DATA_ARITH_INS_S(n, op, write_result)                       \
                                                                        \
    ARM_OPS_PROTO(ArmIss::arm_##n##s)                                    \
    {                                                                   \
        if (!cond_eval())                                            \
            return;                                                     \
                                                                        \
        bool cout = r_cpsr.carry, vout;                                 \
                                                                        \
        uint32_t op1 = r_gp[m_opcode.dp.rn];                            \
        if (m_opcode.dp.rn == 15) {                                     \
            op1 += 4 - 2 * r_cpsr.thumb;                                                   \
            if ( m_opcode.dp.i && (m_opcode.dp.shift_code & 0x10) )     \
                op1 += 4 - 2 * r_cpsr.thumb;                                               \
        }                                                               \
                                                                        \
        uint32_t res = arm_arith_##op(op1, arm_shifter<false>(), cout, vout); \
                                                                        \
        if (write_result)                                               \
            r_gp[m_opcode.dp.rd] = res;                                 \
                                                                        \
        if (m_opcode.dp.rd == 15) {                                     \
            ArmMode cur_mode = psr_to_mode[r_cpsr.mode];               \
            assert(cur_mode < MOD_Count);                               \
            r_gp[m_opcode.dp.rd] = res & ~1;                            \
            cpsr_update(r_spsr[cur_mode]);                              \
            r_cpsr.thumb = res & 1;                                     \
        } else {                                                        \
            r_cpsr.carry = cout;                                        \
            r_cpsr.overflow = vout;                                     \
            r_cpsr.sign = res >> 31;                                    \
            r_cpsr.zero = !res;                                         \
        }                                                               \
    }

#define ARM_DATA_ARITH_INS(n, op)                                       \
                                                                        \
    ARM_DATA_ARITH_INS_S(n, op, true)                                   \
                                                                        \
    ARM_OPS_PROTO(ArmIss::arm_##n)                                       \
    {                                                                   \
        if (!cond_eval())                                            \
            return;                                                     \
                                                                        \
        bool cout = r_cpsr.carry, vout;                                 \
                                                                        \
        uint32_t op1 = r_gp[m_opcode.dp.rn];                            \
        if (m_opcode.dp.rn == 15) {                                     \
            op1 += 4 - 2 * r_cpsr.thumb;                                                   \
            if ( m_opcode.dp.i && (m_opcode.dp.shift_code & 0x10) )         \
                op1 += 4 - 2 * r_cpsr.thumb;                                               \
        }                                                               \
                                                                        \
        uint32_t res = arm_arith_##op(op1, arm_shifter<false>(), cout, vout); \
                                                                        \
        r_gp[m_opcode.dp.rd] = res;                                     \
    }

static inline uint32_t arm_arith_add(uint32_t a, uint32_t b, bool &cout, bool &vout)
{
  return add_cv(a, b, 0, cout, vout);
}

static inline uint32_t arm_arith_adc(uint32_t a, uint32_t b, bool &cout, bool &vout)
{
  return add_cv(a, b, cout, cout, vout);
}

static inline uint32_t arm_arith_sub(uint32_t a, uint32_t b, bool &cout, bool &vout)
{
  return add_cv(a, ~b, 1, cout, vout);
}

static inline uint32_t arm_arith_rsb(uint32_t a, uint32_t b, bool &cout, bool &vout)
{
  return add_cv(b, ~a, 1, cout, vout);
}

static inline uint32_t arm_arith_sbc(uint32_t a, uint32_t b, bool &cout, bool &vout)
{
  return add_cv(a, ~b, cout, cout, vout);
}

static inline uint32_t arm_arith_rsc(uint32_t a, uint32_t b, bool &cout, bool &vout)
{
  return add_cv(b, ~a, cout, cout, vout);
}

static inline uint32_t arm_arith_neg(uint32_t a, uint32_t b, bool &cout, bool &vout)
{
  return add_cv(0, ~a, 1, cout, vout);
}

ARM_DATA_ARITH_INS(sub, sub)
ARM_DATA_ARITH_INS(rsb, rsb)
ARM_DATA_ARITH_INS(add, add)
ARM_DATA_ARITH_INS(adc, adc)
ARM_DATA_ARITH_INS(sbc, sbc)
ARM_DATA_ARITH_INS(rsc, rsc)
ARM_DATA_ARITH_INS_S(cmp, sub, false)
ARM_DATA_ARITH_INS_S(cmn, add, false)




void ArmIss::arm_mul()
{
    if (!cond_eval())
        return;

    data_t res =
        r_gp[m_opcode.mul.rm] * r_gp[m_opcode.mul.rs];

    r_gp[m_opcode.mul.rd] = res;
    if (m_opcode.mul.setcond) {
        r_cpsr.zero = !res;
        r_cpsr.sign = ((int32_t)res) < 0;
    }
}

void ArmIss::arm_smul_xy()
{
    if (!cond_eval())
        return;

    int16_t x = r_gp[m_opcode.mul.rm] >> (16*m_opcode.mul.x);
    int16_t y = r_gp[m_opcode.mul.rs] >> (16*m_opcode.mul.y);
    data_t res = (int32_t)x * (int32_t)y;

    r_gp[m_opcode.mul.rd] = res;
}

void ArmIss::arm_smla_xy()
{
    if (!cond_eval())
        return;

    int16_t x = r_gp[m_opcode.mul.rm] >> (16*m_opcode.mul.x);
    int16_t y = r_gp[m_opcode.mul.rs] >> (16*m_opcode.mul.y);
    data_t res = (int32_t)x * (int32_t)y;

    r_gp[m_opcode.mul.rd] = res + r_gp[m_opcode.mul.rn];
}

void ArmIss::arm_smlaw_y()
{
    if (!cond_eval())
        return;

    int16_t y = r_gp[m_opcode.mul.rs] >> (16*m_opcode.mul.y);
    uint64_t res64 = (int64_t)y * (int64_t)(int32_t)r_gp[m_opcode.mul.rm];
    uint32_t res = (uint32_t)(res64>>16);

    r_gp[m_opcode.mul.rd] = res + r_gp[m_opcode.mul.rn];
}

void ArmIss::arm_smulw_y()
{
    if (!cond_eval())
        return;

    int16_t y = r_gp[m_opcode.mul.rs] >> (16*m_opcode.mul.y);
    uint64_t res64 = (int64_t)y * (int64_t)(int32_t)r_gp[m_opcode.mul.rm];
    uint32_t res = (uint32_t)(res64>>16);

    r_gp[m_opcode.mul.rd] = res;
}

void ArmIss::arm_mla()
{
    if (!cond_eval())
        return;

    data_t res =
        r_gp[m_opcode.mul.rm] * r_gp[m_opcode.mul.rs]
        + r_gp[m_opcode.mul.rn];

    r_gp[m_opcode.mul.rd] = res;
    if (m_opcode.mul.setcond) {
        r_cpsr.zero = !res;
        r_cpsr.sign = ((int32_t)res) < 0;
    }
}

void ArmIss::arm_umaal()
{
    if (!cond_eval())
        return;

    uint64_t res = 
        (uint64_t)r_gp[m_opcode.mul.rm] * (uint64_t)r_gp[m_opcode.mul.rs]
        + r_gp[m_opcode.mul.rd]
        + r_gp[m_opcode.mul.rn];

    r_gp[m_opcode.mul.rn] = res;
    r_gp[m_opcode.mul.rd] = res >> 32;
}

void ArmIss::arm_umull()
{
    if (!cond_eval())
        return;

    uint64_t res = 
        (uint64_t)r_gp[m_opcode.mul.rm] * (uint64_t)r_gp[m_opcode.mul.rs];

    r_gp[m_opcode.mul.rn] = res;
    r_gp[m_opcode.mul.rd] = res >> 32;
    if (m_opcode.mul.setcond) {
        r_cpsr.zero = !res;
        r_cpsr.sign = ((int64_t)res) < 0;
    }
}

void ArmIss::arm_umlal()
{
    if (!cond_eval())
        return;

    uint64_t res =
        (uint64_t)r_gp[m_opcode.mul.rm] * (uint64_t)r_gp[m_opcode.mul.rs]
        + (((uint64_t)r_gp[m_opcode.mul.rd] << 32) | r_gp[m_opcode.mul.rn]);
    
    r_gp[m_opcode.mul.rn] = res;
    r_gp[m_opcode.mul.rd] = res >> 32;
    if (m_opcode.mul.setcond) {
        r_cpsr.zero = !res;
        r_cpsr.sign = ((int64_t)res) < 0;
    }
}

void ArmIss::arm_smull()
{
    if (!cond_eval())
        return;

    int64_t res =
        (int64_t)(int32_t)r_gp[m_opcode.mul.rm] * (int64_t)(int32_t)r_gp[m_opcode.mul.rs];

    r_gp[m_opcode.mul.rn] = res;
    r_gp[m_opcode.mul.rd] = res >> 32;
    if (m_opcode.mul.setcond) {
        r_cpsr.zero = !res;
        r_cpsr.sign = ((int64_t)res) < 0;
    }
}

void ArmIss::arm_smlal()
{
    if (!cond_eval())
        return;

    int64_t res =
        ((int64_t)(int32_t)r_gp[m_opcode.mul.rm] * (int64_t)(int32_t)r_gp[m_opcode.mul.rs])
        + (int64_t)(((uint64_t)r_gp[m_opcode.mul.rd] << 32) | r_gp[m_opcode.mul.rn]);
    
    r_gp[m_opcode.mul.rn] = res;
    r_gp[m_opcode.mul.rd] = res >> 32;
    if (m_opcode.mul.setcond) {
        r_cpsr.zero = !res;
        r_cpsr.sign = ((int64_t)res) < 0;
    }
}


#define THUMB_DATA_ARITH_INS_S(n, op, write_result)                 \
                                                                    \
ARM_OPS_PROTO(ArmIss::thumb_##n)                                    \
{                                                                   \
    bool cout = r_cpsr.carry, vout;                                 \
                                                                    \
    uint32_t op1 = r_gp[m_thumb_op.reg2.rd];                        \
    uint32_t op2 = r_gp[m_thumb_op.reg2.rm];                        \
                                                                    \
    uint32_t res = arm_arith_##op(op1, op2, cout, vout);            \
                                                                    \
    if (write_result)                                               \
        r_gp[m_thumb_op.reg2.rd] = res;                             \
                                                                    \
    r_cpsr.carry = cout;                                            \
    r_cpsr.overflow = vout;                                         \
    r_cpsr.sign = res >> 31;                                        \
    r_cpsr.zero = !res;                                             \
}

#define THUMB_DATA_LOGICAL_INS_S(n, op, write_result)               \
                                                                    \
ARM_OPS_PROTO(ArmIss::thumb_##n)                                    \
{                                                                   \
    uint32_t op1 = r_gp[m_thumb_op.reg2.rd];                        \
    uint32_t op2 = r_gp[m_thumb_op.reg2.rm];                        \
                                                                    \
    data_t res = arm_logical_##op(op1, op2);                        \
                                                                    \
    if (write_result)                                               \
        r_gp[m_thumb_op.reg2.rd] = res;                             \
                                                                    \
    r_cpsr.sign = res >> 31;                                        \
    r_cpsr.zero = !res;                                             \
}

THUMB_DATA_ARITH_INS_S(adc, adc, true)
THUMB_DATA_LOGICAL_INS_S(tst, and, false)
THUMB_DATA_LOGICAL_INS_S(and, and, true)
THUMB_DATA_LOGICAL_INS_S(bic, bic, true)
THUMB_DATA_ARITH_INS_S(cmn, add, false)
THUMB_DATA_ARITH_INS_S(cmp, sub, false)
THUMB_DATA_LOGICAL_INS_S(eor, eor, true)
THUMB_DATA_LOGICAL_INS_S(mvn, mvn, true)
THUMB_DATA_ARITH_INS_S(neg, neg, true)
THUMB_DATA_LOGICAL_INS_S(orr, or, true)
THUMB_DATA_ARITH_INS_S(sbc, sbc, true)


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
