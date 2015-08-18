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
 * Copyright (c) Telecom ParisTech
 *         Alexis Polti <polti@telecom-paristech.fr>
 *
 * Maintainers: Alexis Polti
 *
 * $Id$
 */

#include "sparcv8.h"
#include <cstring>
#include "arithmetics.h"
#include <cassert>

namespace soclib { namespace common {

#define tmpl(x) template<unsigned int NWINDOWS> x Sparcv8Iss<NWINDOWS>

#define MSB(r) ((((int32_t) r) < 0) ? 1 : 0)

#define GET_LOGIC_OPERANDS(ins, op1, op2)                                     \
    do {                                                                \
        op1 = GPR(ins.format3a.rs1);                                    \
        if (ins.format3a.i)                                             \
            op2 = (uint32_t)sign_ext(ins.format3b.imm, 13);             \
        else                                                            \
            op2 = GPR(ins.format3a.rs2);                                \
    } while(0)

#define SET_CCL(r)                                                      \
	do {                                                                \
		r_psr.n = (MSB(r) == 1);                                        \
		r_psr.z = (r == 0);                                             \
		r_psr.v = 0;                                                    \
		r_psr.c = 0;                                                    \
	} while (0)


#define SET_CCA(op1, op2, r)                                            \
	do {                                                                \
		r_psr.n = (MSB(r) == 1);                                        \
		r_psr.z = (r == 0);                                             \
		r_psr.v = (MSB(op1) == MSB(op2)) && (MSB(op1) != MSB(r));       \
		r_psr.c = ((uint32_t)r) < ((uint32_t)op1);                      \
	} while (0)


#define SET_CCS(op1,op2,r)                                              \
	do {                                                                \
		r_psr.n = (MSB(r) == 1);                                        \
		r_psr.z = (r == 0);                                             \
		r_psr.v = ((MSB(op1) != MSB(op2)) && (MSB(op1) != MSB(r)));     \
        r_psr.c = ((uint32_t)r) > ((uint32_t)op1);                      \
	} while (0)


#define SET_CCM(r)                                                      \
	do {                                                                \
		r_psr.n = (MSB(r) == 1);                                        \
		r_psr.z = (r == 0);                                             \
		r_psr.v = 0;                                                    \
        r_psr.c = 0;                                                    \
	} while (0)


#define SET_CCD(r, ov)                                                  \
	do {                                                                \
		r_psr.n = (MSB(r) == 1);                                        \
		r_psr.z = (r == 0);                                             \
		r_psr.v = ov ? 1 : 0;                                           \
        r_psr.c = 0;                                                    \
	} while (0)


#define ENSURE_PRIVILEDGED_MODE()                                       \
    do {                                                                \
        if (r_psr.s != 1) {                                             \
            m_exception = true;                                         \
            m_exception_cause = TP_PRIVILEGED_INSTRUCTION;              \
            return;                                                     \
        }                                                               \
    } while(0)


tmpl(void)::op_sethi()
{
    GPR(m_ins.sethi.rd) = (m_ins.sethi.imm << 10);
}

tmpl(void)::op_and()
{
    uint32_t op1, op2;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    GPR(m_ins.format3a.rd) = op1 & op2;
}

tmpl(void)::op_or()
{
    uint32_t op1, op2;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    GPR(m_ins.format3a.rd) = op1 | op2;
}

tmpl(void)::op_xor()
{
    uint32_t op1, op2;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    GPR(m_ins.format3a.rd) = op1 ^ op2;
}

tmpl(void)::op_andn()
{
    uint32_t op1, op2;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    GPR(m_ins.format3a.rd) = op1 & ~op2;
}

tmpl(void)::op_orn()
{
    uint32_t op1, op2;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    GPR(m_ins.format3a.rd) = op1 | ~op2;
}

tmpl(void)::op_xnor()
{
    uint32_t op1, op2;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    GPR(m_ins.format3a.rd) = op1 ^ ~op2;
}

tmpl(void)::op_andcc()
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 & op2;
    SET_CCL(res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_orcc()
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 | op2;
    SET_CCL(res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_xorcc()
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 ^ op2;
    SET_CCL(res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_andncc()
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 & ~op2;
    SET_CCL(res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_orncc()
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 | ~op2;
    SET_CCL(res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_xnorcc()
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 ^ ~op2;
    SET_CCL(res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_sll()
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    op2 = op2 & 0x1f;
    res = op1 << op2;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_srl()
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    op2 = op2 & 0x1f;
    res = op1 >> op2;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_sra()
{
    int32_t op1;
    uint32_t op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    op2 = op2 & 0x1f;
    res = op1 >> op2;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_add()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_addx()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2 + r_psr.c;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_addcc()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2;
    SET_CCA(op1, op2, res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_addxcc()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2 + r_psr.c;
    SET_CCA(op1, op2, res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_taddcc()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2;
    SET_CCA(op1, op2, res);
    if ((op1 & 0x3) && (op2 & 0x3))
        r_psr.v = 1;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_taddcctv()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2;
    if (((MSB(op1) == MSB(op2)) && (MSB(op1) != MSB(res))) || ((op1 & 0x3) && (op2 & 0x3))) {
        m_exception = true;
        m_exception_cause = TP_TAG_OVERFLOW;
        return;
    }
    else
        SET_CCA(op1, op2, res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_sub()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 - op2;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_subx()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 - op2 - r_psr.c;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_subcc()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 - op2;
    SET_CCS(op1, op2, res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_subxcc()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 - op2 - r_psr.c;
    SET_CCS(op1, op2, res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_tsubcc()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 - op2;
    SET_CCS(op1, op2, res);
    if ((op1 & 0x3) && (op2 & 0x3))
        r_psr.v = 1;
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_tsubcctv()
{
    int32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 - op2;
    if (((MSB(op1) != MSB(op2)) && (MSB(op1) != MSB(res))) || ((op1 & 0x3) && (op2 & 0x3))) {
        m_exception = true;
        m_exception_cause = TP_TAG_OVERFLOW;
        return;
    }
    else
        SET_CCS(op1, op2, res);
    GPR(m_ins.format3a.rd) = res;    
}

tmpl(void)::op_mulscc()
{
    uint32_t op1, op1_saved, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    op1_saved = op1;
    op1 = ((r_psr.n ^ r_psr.v) << 31) | ((op1 >> 1) & 0x7fffffff);
    op2 = ((r_y & 0x1) == 1) ? op2 : 0;
    res = op1 + op2;
    GPR(m_ins.format3a.rd) = res;    
    SET_CCA(op1, op2, res);
    r_y = (op1_saved << 31) | ((r_y >> 1) & 0x7fffffff);
    setInsDelay(4);
}

tmpl(void)::op_umul()
{
    uint64_t op1, op2;
    uint64_t res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 * op2;
    GPR(m_ins.format3a.rd) = res; 
    res >>= 32;
    r_y = res;
    setInsDelay(4);
}

tmpl(void)::op_smul()
{
    int64_t op1, op2;
    int64_t res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 * op2;
    GPR(m_ins.format3a.rd) = res; 
    res >>= 32;
    r_y = res;
    setInsDelay(4);
}

tmpl(void)::op_umulcc()
{
    uint64_t op1, op2;
    uint64_t res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 * op2;
    GPR(m_ins.format3a.rd) = res; 
    SET_CCM(res);
    res >>= 32;
    r_y = res;
    setInsDelay(4);
}

tmpl(void)::op_smulcc()
{
    int64_t op1, op2;
    int64_t res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 * op2;
    GPR(m_ins.format3a.rd) = res; 
    SET_CCM(res);
    res >>= 32;
    r_y = res;
    setInsDelay(4);
}

tmpl(void)::op_udiv()
{
    uint32_t op1, op2, res;
    uint64_t a, b, temp_64;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    if (op2 == 0) {
        m_exception = true;
        m_exception_cause = TP_DIVISION_BY_ZERO;
        return;
    }

    a = r_y;
    a = (a << 32) | op1;;
    b = op2;
    temp_64 = a / b;

    if ((temp_64 >> 32) != 0) 
        // Result overflowed 32 bits; return largest appropriate integer
        res = 0xffffffff;
    else
        // Result didn't overflow, return quotient
        res= temp_64;

    GPR(m_ins.format3a.rd) = res; 
    setInsDelay(31);
}

tmpl(void)::op_sdiv()
{
    int32_t op1, op2, res;
    int64_t a, b, temp_64;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    if (op2 == 0) {
        m_exception = true;
        m_exception_cause = TP_DIVISION_BY_ZERO;
        return;
    }

    a = r_y;
    a = (a << 32) | op1;;
    b = op2;
    temp_64 = a / b;

    if (((temp_64 >> 32) != 0) && (((temp_64 >> 31) & 0x1ffffffffLL) == 0x1ffffffffLL)) 
        // Result overflowed 32 bits; return largest appropriate integer
        res = (temp_64 > 0) ? 0x7FFFFFFF : 0xFFFFFFFF;
    else
        // Result didn't overflow, return quotient
        res= temp_64;
    
    GPR(m_ins.format3a.rd) = res; 
    setInsDelay(31);
}

tmpl(void)::op_udivcc()
{
    uint32_t op1, op2, res;
    uint64_t a, b, temp_64;
    bool V;

    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    if (op2 == 0) {
        m_exception = true;
        m_exception_cause = TP_DIVISION_BY_ZERO;
        return;
    }

    a = r_y;
    a = (a << 32) | op1;;
    b = op2;
    temp_64 = a / b;
    V = (temp_64 >> 32) != 0;

    if (V) 
        // Result overflowed 32 bits; return largest appropriate integer
        res = 0xffffffff;
    else
        // Result didn't overflow, return quotient
        res= temp_64;

    GPR(m_ins.format3a.rd) = res; 
    SET_CCD(res, V);
    setInsDelay(31);
}

tmpl(void)::op_sdivcc()
{
    int32_t op1, op2, res;
    int64_t a, b, temp_64;
    bool V;

    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    if (op2 == 0) {
        m_exception = true;
        m_exception_cause = TP_DIVISION_BY_ZERO;
        return;
    }

    a = r_y;
    a = (a << 32) | op1;;
    b = op2;
    temp_64 = a / b;
    V = ((temp_64 >> 32) != 0) && (((temp_64 >> 31) & 0x1ffffffffLL) == 0x1ffffffffLL);

    if (V) 
        // Result overflowed 32 bits; return largest appropriate integer
        res = (temp_64 > 0) ? 0x7FFFFFFF : 0xFFFFFFFF;
    else
        // Result didn't overflow, return quotient
        res= temp_64;
    
    GPR(m_ins.format3a.rd) = res; 
    SET_CCD(res, V);
    setInsDelay(31);
}

tmpl(void)::op_rdasr() 
{
    uint32_t op1;
    op1 = m_ins.format3a.rs1;
    
    switch (op1) {
    case 0 : 
        // RDY
        GPR(m_ins.format3a.rd) = r_y; 
        break;

    case 1 ... 14 :
        // RDASR
        ENSURE_PRIVILEDGED_MODE();
        break;

    case 15 :
        // STBAR of garbage
        ENSURE_PRIVILEDGED_MODE();
        if (m_ins.format3a.rd == 0) {
            // STBAR : don't do anything
        }
        else {
            m_exception = true;
            m_exception_cause = TP_ILLEGAL_INSTRUCTION;
        }
        break;
        
    case 16 : 
        // RDASR
        ENSURE_PRIVILEDGED_MODE();
        GPR(m_ins.format3a.rd) = m_ident; 
        break;

    case 17 :
        // LEON3 cpu id
        GPR(m_ins.format3a.rd) = m_ident << 28;
        break;

    default :
        m_exception = true;
        m_exception_cause = TP_ILLEGAL_INSTRUCTION;
    }
}

tmpl(void)::op_rdtbr() 
{
    ENSURE_PRIVILEDGED_MODE();
    GPR(m_ins.format3a.rd) = r_tbr.whole; 
}

tmpl(void)::op_rdpsr() 
{
    ENSURE_PRIVILEDGED_MODE();
    GPR(m_ins.format3a.rd) = r_psr.whole; 
}

tmpl(void)::op_rdwim() 
{
    ENSURE_PRIVILEDGED_MODE();
    GPR(m_ins.format3a.rd) = r_wim & ~(0xffffffff << NWINDOWS); 
}

tmpl(void)::op_wrasr() 
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 ^ op2;

    switch (m_ins.format3a.rd) {
    case 0 : 
        // WRY
        r_y = res;
        break;

    case 1 ... 15 :
        ENSURE_PRIVILEDGED_MODE();
        break;

    case 19: // enter irq wait state
        ENSURE_PRIVILEDGED_MODE();
        m_wait_irq = true;
        break;

    default :
        m_exception = true;
        m_exception_cause = TP_ILLEGAL_INSTRUCTION;
    }
}

tmpl(void)::op_wrtbr() 
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 ^ op2;

    ENSURE_PRIVILEDGED_MODE();
    r_tbr.whole = res & 0xfffff000;
}

tmpl(void)::op_wrpsr() 
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 ^ op2;
    ENSURE_PRIVILEDGED_MODE();
    
    // Ensure new CWP points to implemented window, else trap
    if ((res & 0x1f) >= NWINDOWS) {
        m_exception = true;
        m_exception_cause = TP_ILLEGAL_INSTRUCTION;
        return;
    }
    
    // Bits impl, ver, reserved, ec, and ef are read-only
    r_psr_write.whole = (r_psr.whole & 0xff0ff000) | (res & 0x00f00fff);
    r_psr_delay = 3;
}

tmpl(void)::op_wrwim() 
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 ^ op2;

    ENSURE_PRIVILEDGED_MODE();
    r_wim = res & ~(0xffffffff << NWINDOWS);
}

tmpl(void)::op_save() 
{
    uint32_t op1, op2, res;
    uint32_t cwp;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2;

    cwp = (r_psr.cwp-1) % NWINDOWS;
    if ((r_wim & (1 << cwp)) != 0) {
        m_exception = true;
        m_exception_cause = TP_WINDOW_OVERFLOW;
        return;
    }
    r_psr.cwp = cwp;
    GPR(m_ins.format3a.rd) = res; 
}

tmpl(void)::op_restore() 
{
    uint32_t op1, op2, res;
    uint32_t cwp;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2;

    cwp = (r_psr.cwp+1) % NWINDOWS;
    if ((r_wim & (1 << cwp)) != 0) {
        m_exception = true;
        m_exception_cause = TP_WINDOW_UNDERFLOW;
        return;
    }
    r_psr.cwp = cwp;
    GPR(m_ins.format3a.rd) = res; 
}

tmpl(void)::op_call() 
{
    m_next_pc = r_pc + (m_ins.call.disp << 2);
    GPR(15) = r_pc; 
}

tmpl(void)::op_jmpl() 
{
    uint32_t op1, op2, res;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2;

    if ((res & 0x3) != 0) {
        m_exception = true;
        m_exception_cause = TP_MEM_ADDRESS_NOT_ALIGNED;
        return;
    }

    m_next_pc = res;
    GPR(m_ins.format3a.rd) = r_pc; 
}

tmpl(void)::op_rett() 
{
    uint32_t op1, op2, res;
    uint32_t cwp;
    GET_LOGIC_OPERANDS(m_ins, op1, op2);
    res = op1 + op2;

    if (r_psr.et == 1) {
        if (r_psr.s == 1)
            m_exception_cause = TP_ILLEGAL_INSTRUCTION;
        else
            m_exception_cause = TP_PRIVILEGED_INSTRUCTION;
        m_exception = true;
        return;
    }
    
    ENSURE_PRIVILEDGED_MODE();
    
    cwp = (r_psr.cwp+1) % NWINDOWS;
    if ((r_wim & (1 << cwp)) != 0) {
        m_exception = true;
        m_exception_cause = TP_WINDOW_UNDERFLOW;
        return;
    }

    if ((res & 0x3) != 0) {
        m_exception = true;
        m_exception_cause = TP_MEM_ADDRESS_NOT_ALIGNED;
        return;
    }
    
    // Restore CWP, restore S bit, set ET bit
    r_psr.cwp = cwp;
    r_psr.s = r_psr.ps;
    r_psr.et = 1;

    // Delayed branch to new address
    m_next_pc = res;
}

#define op(x) tmpl(void)::op_##x()                          \
    {                                                       \
        uint32_t op1, op2, trap;                    \
        GET_LOGIC_OPERANDS(m_ins, op1, op2);                      \
                                                            \
        if(!evaluate_icc(m_ins.branches.cond))              \
            return;                                         \
                                                            \
        trap = (op1 + op2) & 0x7f;                          \
                                                            \
        if (!m_exception) {                                 \
            m_exception = true;                             \
            m_exception_cause = TP_TRAP_INSTRUCTION(trap);  \
        }                                                   \
        setInsDelay(4);                                     \
    }                                                       

#define op4(x, y, z, t) op(x); op(y); op(z); op(t);
op4(tn,     te,     tle,    tl);
op4(tleu,   tcs,    tneg,   tvs);
op4(ta,     tne,    tg,     tge);
op4(tgu,    tcc,    tpos,   tvc);

#undef op4
#undef op

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

