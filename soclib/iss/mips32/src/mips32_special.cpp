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
#include "mips32.hpp"
#include "arithmetics.h"

#include <strings.h>

namespace soclib { namespace common {

namespace {
// Avoid duplication of source code, this kind of op
// is easy to bug, and should be easy to debug 
static inline uint32_t sll( uint32_t reg, uint32_t sh )
{
    return reg << sh;
}
static inline uint32_t srl( uint32_t reg, uint32_t sh )
{
    return reg >> sh;
}
static inline uint32_t rotr( uint32_t reg, uint32_t sh )
{
    return (reg >> sh) | (reg << (32-sh));
}
static inline uint32_t sra( uint32_t reg, uint32_t sh )
{
    if ( (int32_t)reg < 0 )
    {
        /* nb: if sh==0, (1<<(32-sh)) is truncated to 0 (32bits casting)
         * which eventually causes the result to be 0xFFFFFFFF */
        /* beside of this conditional-based solution,
         * another solution would be to cast in 64bits:
         * return (reg >> sh) | (~(((unsigned long long int)1<<(32-sh))-1));
         */
        if (sh)
            return (reg >> sh) | (~((1<<(32-sh))-1));
        else
            return reg;
    }
    else
        return reg >> sh;
}
}

void Mips32Iss::special_sll()
{
    // EHB is hidden here if sh == 3 and r[tsd] == 0, ie ins = 0xc0
    // That is ugly !
    if ( m_ins.ins == 0xc0 ) {
        update_mode();
    } else {
        r_gp[m_ins.r.rd] = sll(r_gp[m_ins.i.rt], m_ins.r.sh);
    }
}

void Mips32Iss::special_srl()
{
    if ( m_ins.r.rs&1 )
        r_gp[m_ins.r.rd] = rotr(r_gp[m_ins.i.rt], m_ins.r.sh);
    else
        r_gp[m_ins.r.rd] = srl(r_gp[m_ins.i.rt], m_ins.r.sh);
}

void Mips32Iss::special_sra()
{
    r_gp[m_ins.r.rd] = sra(r_gp[m_ins.i.rt], m_ins.r.sh);
}

void Mips32Iss::special_sllv()
{
    r_gp[m_ins.r.rd] = sll(r_gp[m_ins.i.rt], r_gp[m_ins.i.rs]&0x1f );
}

void Mips32Iss::special_srlv()
{
    if ( m_ins.r.sh&1 )
        r_gp[m_ins.r.rd] = rotr(r_gp[m_ins.i.rt], r_gp[m_ins.i.rs]&0x1f);
    else
        r_gp[m_ins.r.rd] = srl(r_gp[m_ins.i.rt], r_gp[m_ins.i.rs]&0x1f);
}

void Mips32Iss::special_srav()
{
    r_gp[m_ins.r.rd] = sra(r_gp[m_ins.i.rt], r_gp[m_ins.i.rs]&0x1f );
}

void Mips32Iss::special_jr()
{
    if (isPrivDataAddr(r_gp[m_ins.i.rs]) && !isPriviliged()) {
        // TODO error code
        m_exception = X_ADEL;
        return;
    }
    jump(r_gp[m_ins.i.rs], false);
}

void Mips32Iss::special_jalr()
{
    if (isPrivDataAddr(r_gp[m_ins.i.rs]) && !isPriviliged()) {
        // TODO error code
        m_exception = X_ADEL;
        return;
    }
    r_gp[m_ins.r.rd] = r_pc+8;
    jump(r_gp[m_ins.i.rs], false);
}

void Mips32Iss::special_sysc()
{
    m_exception = X_SYS;
}

void Mips32Iss::special_brek()
{
    m_exception = X_BP;
}

void Mips32Iss::special_mfhi()
{
    r_gp[m_ins.r.rd] = r_hi;
}

void Mips32Iss::special_movn()
{
    if ( r_gp[m_ins.i.rt] != 0 )
        r_gp[m_ins.r.rd] = r_gp[m_ins.i.rs];
}

inline bool Mips32Iss::FPConditionCode(uint8_t cc)
{
    bool r;
    if (cc==0)
        r = r_fcsr.fcc1;
    else
        r = 1 & (r_fcsr.fcc7 >> (cc-1));
//     std::cout << m_name << " fp cc " << (int)cc << ": " << r << std::endl;
    return r;
}

void Mips32Iss::special_movtf()
{
    if (!isCopAccessible(1)) {
        m_exception = X_CPU;
        return;
    }

    if ( FPConditionCode(m_ins.fpu_ccri.cc) == !!m_ins.fpu_ccri.tf )
        r_gp[m_ins.fpu_ccri.rd] = r_gp[m_ins.fpu_ccri.rs];
}

void Mips32Iss::special_movz()
{
    if ( r_gp[m_ins.i.rt] == 0 )
        r_gp[m_ins.r.rd] = r_gp[m_ins.i.rs];
}

void Mips32Iss::special_mthi()
{
    r_hi = r_gp[m_ins.i.rs];
}

void Mips32Iss::special_mflo()
{
    r_gp[m_ins.r.rd] = r_lo;
}

void Mips32Iss::special_mtlo()
{
    r_lo = r_gp[m_ins.i.rs];
}

void Mips32Iss::special_mult()
{
    int64_t a = (int32_t)r_gp[m_ins.i.rs];
    int64_t b = (int32_t)r_gp[m_ins.i.rt];
    int64_t res = a*b;
    r_hi = res>>32;
    r_lo = res;
    if (r_gp[m_ins.i.rt])
        setInsDelay( 3 );
}

void Mips32Iss::special_multu()
{
    uint64_t a = r_gp[m_ins.i.rs];
    uint64_t b = r_gp[m_ins.i.rt];
    uint64_t res = a*b;
    r_hi = res>>32;
    r_lo = res;
    if (r_gp[m_ins.i.rt])
        setInsDelay( 3 );
}

void Mips32Iss::special_div()
{
    if ( ! r_gp[m_ins.i.rt] ) {
        r_hi = random();
        r_lo = random();
        return;
    }
    r_hi = (int32_t)r_gp[m_ins.i.rs] % (int32_t)r_gp[m_ins.i.rt];
    r_lo = (int32_t)r_gp[m_ins.i.rs] / (int32_t)r_gp[m_ins.i.rt];
    if (r_gp[m_ins.i.rt])
        setInsDelay( ::soclib::common::clz(r_gp[m_ins.i.rt])+1 );
}

void Mips32Iss::special_divu()
{
    if ( ! r_gp[m_ins.i.rt] ) {
        r_hi = random();
        r_lo = random();
        return;
    }
    r_hi = r_gp[m_ins.i.rs] % r_gp[m_ins.i.rt];
    r_lo = r_gp[m_ins.i.rs] / r_gp[m_ins.i.rt];
    if (r_gp[m_ins.i.rt])
        setInsDelay( ::soclib::common::clz(r_gp[m_ins.i.rt])+1 );
}

void Mips32Iss::special_add()
{
    bool cout, vout;
    uint32_t tmp = add_cv(r_gp[m_ins.i.rs], r_gp[m_ins.i.rt], 0, cout, vout);
    if ( vout )
        m_exception = X_OV;
    else
        r_gp[m_ins.r.rd] = tmp;
}

void Mips32Iss::special_addu()
{
    r_gp[m_ins.r.rd] = r_gp[m_ins.i.rs] + r_gp[m_ins.i.rt];
}

void Mips32Iss::special_sub()
{
    bool cout, vout;
    uint32_t tmp = add_cv(r_gp[m_ins.i.rs], ~r_gp[m_ins.i.rt], 1, cout, vout);
    if ( vout )
        m_exception = X_OV;
    else
        r_gp[m_ins.r.rd] = tmp;
}

void Mips32Iss::special_subu()
{
    r_gp[m_ins.r.rd] = r_gp[m_ins.i.rs] - r_gp[m_ins.i.rt];
}

void Mips32Iss::special_and()
{
    r_gp[m_ins.r.rd] = r_gp[m_ins.i.rs] & r_gp[m_ins.i.rt];
}

void Mips32Iss::special_or()
{
    r_gp[m_ins.r.rd] = r_gp[m_ins.i.rs] | r_gp[m_ins.i.rt];
}

void Mips32Iss::special_xor()
{
    r_gp[m_ins.r.rd] = r_gp[m_ins.i.rs] ^ r_gp[m_ins.i.rt];
}

void Mips32Iss::special_nor()
{
    r_gp[m_ins.r.rd] = ~(r_gp[m_ins.i.rs] | r_gp[m_ins.i.rt]);
}

void Mips32Iss::special_slt()
{
    r_gp[m_ins.r.rd] = (bool)((int32_t)r_gp[m_ins.i.rs] < (int32_t)r_gp[m_ins.i.rt]);
}

void Mips32Iss::special_sltu()
{
    r_gp[m_ins.r.rd] = (bool)(r_gp[m_ins.i.rs] < r_gp[m_ins.i.rt]);
}

void Mips32Iss::special_tlt()
{
    if ((int32_t)r_gp[m_ins.i.rs] < (int32_t)r_gp[m_ins.i.rt])
        m_exception = X_TR;
}

void Mips32Iss::special_tltu()
{
    if (r_gp[m_ins.i.rs] < r_gp[m_ins.i.rt])
        m_exception = X_TR;
}

void Mips32Iss::special_tge()
{
    if ((int32_t)r_gp[m_ins.i.rs] >= (int32_t)r_gp[m_ins.i.rt])
        m_exception = X_TR;
}

void Mips32Iss::special_tgeu()
{
    if (r_gp[m_ins.i.rs] >= r_gp[m_ins.i.rt])
        m_exception = X_TR;
}

void Mips32Iss::special_teq()
{
    if (r_gp[m_ins.i.rs] == r_gp[m_ins.i.rt])
        m_exception = X_TR;
}

void Mips32Iss::special_tne()
{
    if (r_gp[m_ins.i.rs] != r_gp[m_ins.i.rt])
        m_exception = X_TR;
}

void Mips32Iss::special_sync()
{
    do_mem_access(4*XTN_SYNC, 4, 0, NULL, 0, 0, XTN_WRITE);
}

void Mips32Iss::special_ill()
{
    m_exception = X_RI;
}

#define op(x) &Mips32Iss::special_##x
#define op4(x, y, z, t) op(x), op(y), op(z), op(t)

Mips32Iss::func_t const Mips32Iss::special_table[] = {
        op4(  sll,movtf,  srl,  sra),
        op4( sllv,  ill, srlv, srav),

        op4(   jr, jalr, movz, movn),
        op4( sysc, brek,  ill, sync),

        op4( mfhi, mthi, mflo, mtlo),
        op4(  ill,  ill,  ill,  ill),

        op4( mult,multu,  div, divu),
        op4(  ill,  ill,  ill,  ill),

        op4(  add, addu,  sub, subu),
        op4(  and,   or,  xor,  nor),

        op4(  ill,  ill,  slt, sltu),
        op4(  ill,  ill,  ill,  ill),

        op4(  tge, tgeu,  tlt, tltu),
        op4(  teq,  ill,  tne,  ill),

        op4(  ill,  ill,  ill,  ill),
        op4(  ill,  ill,  ill,  ill),
};

#undef op
#undef op4

void Mips32Iss::op_special()
{
    func_t func = special_table[m_ins.r.func];
    (this->*func)();
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
