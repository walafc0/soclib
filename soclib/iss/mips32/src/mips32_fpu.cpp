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
 *         Noé Girand <noe.girand@polytechnique.org>, 2009
 *
 * $Id: mips32_fpu.cpp 2074 2011-03-09 14:54:56Z kane $
 */

#include "mips32.h"
#include "mips32.hpp"
#include "base_module.h"
#include "static_assert.h"
#include "arithmetics.h"
#include <cmath> // std::cos

#include <strings.h>

namespace soclib { namespace common {

template <>
int32_t Mips32Iss::readFPU<int32_t>(uint8_t fpr)
{
    return r_f[fpr];
}

template <>
float Mips32Iss::readFPU<float>(uint8_t fpr)
{
    union { int32_t i; float t; } v;
    v.i = r_f[fpr];
    return v.t;
}

template <>
double Mips32Iss::readFPU<double>(uint8_t fpr)
{
    union { uint64_t i; double t; } v;
    if (m_little_endian) {
        v.i = ((uint64_t)r_f[fpr+1] << 32) | r_f[fpr];
    } else {
        v.i = ((uint64_t)r_f[fpr] << 32) | r_f[fpr+1];
    }
    return v.t;
}

template <>
void Mips32Iss::storeFPU<int32_t>(uint8_t fpr, int32_t value)
{
    r_f[fpr] = value;
}

template <>
void Mips32Iss::storeFPU<float>(uint8_t fpr, float value)
{
    union { int32_t i; float t; } v;
    v.t = value;
    r_f[fpr] = v.i;
}

template <>
void Mips32Iss::storeFPU<double>(uint8_t fpr, double value)
{
    union { uint64_t i; double t; } v;
    v.t = value;
    if (m_little_endian) {
        r_f[fpr+1] = v.i >> 32;
        r_f[fpr]   = v.i;
    } else {
        r_f[fpr]   = v.i >> 32;
        r_f[fpr+1] = v.i;
    }
}

template <>
void Mips32Iss::storeFPU<int64_t>(uint8_t fpr, int64_t value)
{
    if (m_little_endian) {
        r_f[fpr+1] = value >> 32;
        r_f[fpr]   = value;
    } else {
        r_f[fpr]   = value >> 32;
        r_f[fpr+1] = value;
    }
}


inline void Mips32Iss::CheckFPException()
{
    if (
        ( r_fcsr.cause_e)   |
        ( r_fcsr.cause_v & r_fcsr.enables_v) |
        ( r_fcsr.cause_z & r_fcsr.enables_z) |
        ( r_fcsr.cause_o & r_fcsr.enables_o) |
        ( r_fcsr.cause_u & r_fcsr.enables_u) |
        ( r_fcsr.cause_i & r_fcsr.enables_i)
        ) {
        m_exception = X_FPE;
    }
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

template <>
void Mips32Iss::cop1_do<float>()
{
    func_t func = cop1_s_cod_table[m_ins.fpu_r.func];
    (this->*func)();
}

template <>
void Mips32Iss::cop1_do<double>()
{
    func_t func = cop1_d_cod_table[m_ins.fpu_r.func];
    (this->*func)();
}

template <>
void Mips32Iss::cop1_do<int32_t>()
{
    func_t func = cop1_w_cod_table[m_ins.fpu_r.func];
    (this->*func)();
}

//////////////////////////////////
// Cop1 instructions
//////////////////////////////////

#define cop1(x) &Mips32Iss::cop1_##x
#define cop14(x, y, z, t) cop1(x), cop1(y), cop1(z), cop1(t)

Mips32Iss::func_t const Mips32Iss::cop1_cod_table[]= {
    cop14(mf,   ill,    cf,     ill),
    cop14(mt,   ill,    ct,     ill),
    cop14(bc,   ill,    ill,    ill),
    cop14(ill,  ill,    ill,    ill),
    cop14(do<float>,    do<double>,      ill,    ill),
    cop14(do<int32_t>,    ill,    ill,    ill),
    cop14(ill,  ill,    ill,    ill),
    cop14(ill,  ill,    ill,    ill),
};

#undef cop1

void Mips32Iss::op_cop1()
{
    if (!isCopAccessible(1)) {
        r_cause.ce = 1;
        m_exception = X_CPU;
        return;
    }
    func_t func = cop1_cod_table[m_ins.fpu_r.fmt];
    (this->*func)();
}

void Mips32Iss::cop1_mf()
{
    r_gp[m_ins.fpu_r.ft] = r_f[m_ins.fpu_r.fs];
}

enum {
    FIR = 0,
    FCCR = 25,
    FEXR = 26,
    FENR = 28,
    FCSR = 31,
};

void Mips32Iss::cop1_cf()
{
    switch(m_ins.fpu_r.fs) {
    case FIR:
        r_gp[m_ins.fpu_r.ft] = r_fir.whole;
        break;

    case FCCR: {
        fccr_t r;
        r.whole = 0;
        r.fcc = (r_fcsr.fcc7 << 1) | r_fcsr.fcc1;
        r_gp[m_ins.fpu_r.ft] = r.whole;
        break;
    }

    case FEXR: {
        fexr_t r;
        r.whole = 0;
        r.cause_e = r_fcsr.cause_e;
        r.cause_v = r_fcsr.cause_v;
        r.cause_z = r_fcsr.cause_z;
        r.cause_o = r_fcsr.cause_o;
        r.cause_u = r_fcsr.cause_u;
        r.cause_i = r_fcsr.cause_i;
        r.flags_v = r_fcsr.flags_v;
        r.flags_z = r_fcsr.flags_z;
        r.flags_o = r_fcsr.flags_o;
        r.flags_u = r_fcsr.flags_u;
        r.flags_i = r_fcsr.flags_i;
        r_gp[m_ins.fpu_r.ft] = r.whole;
        break;
    }

    case FENR: {
        Mips32Iss::fenr_t r;
        r.whole = 0;
        r.enables_v = r_fcsr.enables_v;
        r.enables_z = r_fcsr.enables_z;
        r.enables_o = r_fcsr.enables_o;
        r.enables_u = r_fcsr.enables_u;
        r.enables_i = r_fcsr.enables_i;
        r.fs = r_fcsr.fs;
        r.rm = r_fcsr.rm;
        r_gp[m_ins.fpu_r.ft] = r.whole;
        break;
    }

    case FCSR:
        r_gp[m_ins.fpu_r.ft] = r_fcsr.whole;
        break;
    }
}

void Mips32Iss::cop1_mt()
{
    r_f[m_ins.fpu_r.fs] = r_gp[m_ins.fpu_r.ft];
}

void Mips32Iss::cop1_ct()
{
    uint32_t temp = r_gp[m_ins.fpu_r.ft];

    switch(m_ins.fpu_r.fs) {
    case FCCR: {
        fccr_t r;
        r.whole = temp;
        r_fcsr.fcc7 = r.fcc >> 1;
        r_fcsr.fcc1 = r.fcc & 1;
        break;
    }

    case FEXR: {
        fexr_t r;
        r.whole = temp;
        r_fcsr.cause_e = r.cause_e;
        r_fcsr.cause_v = r.cause_v;
        r_fcsr.cause_z = r.cause_z;
        r_fcsr.cause_o = r.cause_o;
        r_fcsr.cause_u = r.cause_u;
        r_fcsr.cause_i = r.cause_i;
        r_fcsr.flags_v = r.flags_v;
        r_fcsr.flags_z = r.flags_z;
        r_fcsr.flags_o = r.flags_o;
        r_fcsr.flags_u = r.flags_u;
        r_fcsr.flags_i = r.flags_i;
        break;
    }

    case FENR: {
        fenr_t r;
        r.whole = temp;
        r_fcsr.enables_v = r.enables_v;
        r_fcsr.enables_z = r.enables_z;
        r_fcsr.enables_o = r.enables_o;
        r_fcsr.enables_u = r.enables_u;
        r_fcsr.enables_i = r.enables_i;
        r_fcsr.fs = r.fs;
        r_fcsr.rm = r.rm;
        break;
    }

    case FCSR:
        r_fcsr.whole = temp;
        break;
    }
}

void Mips32Iss::cop1_bc()
{
    bool likely = m_ins.fpu_bc.nd;
    bool eq_true = m_ins.fpu_bc.tf;

    jump_imm16( FPConditionCode(m_ins.fpu_bc.cc) == eq_true, likely );
}

#define cop1_table                                                     \
    cop14(add,      sub,    mult,   div ),                             \
    cop14(sqrt,     abs,    mov,    neg ),                             \
    cop14(roundl,truncl,  ceill,  floorl),                             \
    cop14(roundw,truncw,  ceilw,  floorw),                             \
    cop14(ill,    movft,   movz,    movn),                             \
    cop14(ill,    recip,  rsqrt,    ill ),                             \
    cop14(ill,      ill,    ill,    ill ),                             \
    cop14(ill,      ill,    ill,    ill ),                             \
    cop14(cvt_s,    cvt_d,  ill,    ill ),                             \
    cop14(cvt_w,    cvt_l,  ill,    ill ),                             \
    cop14(ill,      ill,    ill,    ill ),                             \
    cop14(ill,      ill,    ill,    ill ),                             \
    cop14(c,        c,      c,      c   ),                             \
    cop14(c,        c,      c,      c   ),                             \
    cop14(c,        c,      c,      c   ),                             \
    cop14(c,        c,      c,      c   ),


Mips32Iss::func_t const Mips32Iss::cop1_s_cod_table[]= {
#define cop1(x) &Mips32Iss::cop1_##x<float>
    cop1_table
#undef cop1
};


Mips32Iss::func_t const Mips32Iss::cop1_d_cod_table[]= {
#define cop1(x) &Mips32Iss::cop1_##x<double>
    cop1_table
#undef cop1
};


Mips32Iss::func_t const Mips32Iss::cop1_w_cod_table[]= {
#define cop1(x) &Mips32Iss::cop1_##x<int32_t>
    cop1_table
#undef cop1
};

void Mips32Iss::cop1_ill()
{
    std::cout << name() << " FMT " << m_ins.fpu_r.fmt << " in COP1 is not implemented." << std::endl;
    op_ill();
    r_fcsr.cause_e = 1;
}

template <class T>
void Mips32Iss::cop1_ill()
{
    cop1_ill();
}

template <class T>
void Mips32Iss::cop1_add()
{
    T r = readFPU<T>(m_ins.fpu_r.fs) + readFPU<T>(m_ins.fpu_r.ft);
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_sub()
{
    T r = readFPU<T>(m_ins.fpu_r.fs) - readFPU<T>(m_ins.fpu_r.ft);
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_mult()
{
    T r = readFPU<T>(m_ins.fpu_r.fs) * readFPU<T>(m_ins.fpu_r.ft);
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_div()
{
    T d = readFPU<T>(m_ins.fpu_r.ft);
    if (d!=0) {
        T r = readFPU<T>(m_ins.fpu_r.fs) / d;
        storeFPU<T>(m_ins.fpu_r.fd, r);
    } else {
        r_fcsr.cause_z = 1;
        if (!r_fcsr.enables_z)
            r_fcsr.flags_z = 1;
    }
}

template <class T>
void Mips32Iss::cop1_rsqrt()
{
    T r = (T)((T)1 / std::sqrt(readFPU<T>(m_ins.fpu_r.fs)));
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_sqrt()
{
    T r = (T)std::sqrt(readFPU<T>(m_ins.fpu_r.fs));
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_recip()
{
    T r = (T)1 / readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_abs()
{
    T r = std::abs(readFPU<T>(m_ins.fpu_r.fs));
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_movft()
{
    if ( FPConditionCode(m_ins.fpu_fprc.cc) == !!m_ins.fpu_fprc.tf )
        storeFPU<T>(m_ins.fpu_fprc.fd, readFPU<T>(m_ins.fpu_fprc.fs));
}

template <class T>
void Mips32Iss::cop1_movn()
{
    if ( r_gp[m_ins.fpu_r.ft] != 0 )
        storeFPU<T>(m_ins.fpu_r.fd, readFPU<T>(m_ins.fpu_r.fs));
}

template <class T>
void Mips32Iss::cop1_movz()
{
    if ( r_gp[m_ins.fpu_r.ft] == 0 )
        storeFPU<T>(m_ins.fpu_r.fd, readFPU<T>(m_ins.fpu_r.fs));
}

template <class T>
void Mips32Iss::cop1_mov()
{
    T r = readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_neg()
{
    T r = -readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<T>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_roundw()
{
    int32_t r = (int32_t)(readFPU<T>(m_ins.fpu_r.fs) + (T).5f);
    storeFPU<int32_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_truncw()
{
    int32_t r = (int32_t)readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<int32_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_ceilw()
{
    int32_t r = (int32_t)std::ceil(readFPU<T>(m_ins.fpu_r.fs));
    storeFPU<int32_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_floorw()
{
    int32_t r = (int32_t)std::floor(readFPU<T>(m_ins.fpu_r.fs));
    storeFPU<int32_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_roundl()
{
    int64_t r = (int64_t)(readFPU<T>(m_ins.fpu_r.fs) + (T).5f);
    storeFPU<int64_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_truncl()
{
    int64_t r = (int64_t)readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<int64_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_ceill()
{
    int64_t r = (int64_t)std::ceil(readFPU<T>(m_ins.fpu_r.fs));
    storeFPU<int64_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_floorl()
{
    int64_t r = (int64_t)std::floor(readFPU<T>(m_ins.fpu_r.fs));
    storeFPU<int64_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_cvt_s()
{
    float r = readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<float>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_cvt_d()
{
    double r = readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<double>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_cvt_w()
{
    int32_t r = (int32_t)readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<int32_t>(m_ins.fpu_r.fd, r);
}

template <class T>
void Mips32Iss::cop1_cvt_l()
{
    int64_t r = (int64_t)readFPU<T>(m_ins.fpu_r.fs);
    storeFPU<int64_t>(m_ins.fpu_r.fd, r);
}

/*
template <>
void Mips32Iss::cop1_c<int32_t>()
{
    int32_t cond0, cond1;
    int32_t v_fs = readFPU<int32_t>(m_ins.fpu_c.fs);
    int32_t v_ft = readFPU<int32_t>(m_ins.fpu_c.ft);
    int16_t v_fs0 = v_fs;
    int16_t v_fs1 = v_fs>>16;
    int16_t v_ft0 = v_ft;
    int16_t v_ft1 = v_ft>>16;

    assert((m_ins.fpu_c.cc % 2) == 0);

    cond0 = ((v_fs0 < v_ft0) << 1)
        | ((v_fs0 == v_ft0));

    cond1 = ((v_fs1 < v_ft1) << 1)
        | ((v_fs1 == v_ft1));

    bool res0 = ((m_ins.fpu_c.cond & 7) >> cond0) & 1;
    bool res1 = ((m_ins.fpu_c.cond & 7) >> cond1) & 1;

    if (m_ins.fpu_c.cc==0) {
        r_fcsr.fcc1 = res0;
        r_fcsr.fcc7 = (r_fcsr.fcc7 & ~1) | res1;
    } else {
        int32_t mask0 = 1<<(m_ins.fpu_c.cc-1);
        int32_t mask1 = 1<<(m_ins.fpu_c.cc);
        r_fcsr.fcc7 = (r_fcsr.fcc7 & mask0) | (res0 * mask0);
        r_fcsr.fcc7 = (r_fcsr.fcc7 & mask1) | (res1 * mask1);
    }
}
*/

template <class T>
void Mips32Iss::cop1_c()
{
    T v_fs = readFPU<T>(m_ins.fpu_c.fs);
    T v_ft = readFPU<T>(m_ins.fpu_c.ft);

    bool res;

    if ( std::isnan(v_fs) || std::isnan(v_ft) )
        res = m_ins.fpu_c.cond & 1;
    else if (v_fs == v_ft)
        res = m_ins.fpu_c.cond & 2;
    else if (v_fs < v_ft)
        res = m_ins.fpu_c.cond & 4;
    else
        res = false;

//     std::cout << m_name << " cond " << m_ins.fpu_c.cond << " fs " <<
//         v_fs << " ft " << v_ft << " c " << cond << " res " << res
//               << " dest " << m_ins.fpu_c.cc << std::endl;

    if (m_ins.fpu_c.cc==0) {
        r_fcsr.fcc1 = res;
    } else {
        uint32_t mask = 1<<(m_ins.fpu_c.cc-1);
        r_fcsr.fcc7 &= ~mask;
        if ( res )
            r_fcsr.fcc7 |= mask;
    }
}

void Mips32Iss::op_sdc1_part2()
{
    do_mem_access(m_part_addr, 4, 0, 0, 0, r_f[m_part_data], DATA_WRITE);
    m_microcode_func = NULL;
}


#define check_align(address, align)                                    \
    if ( (address)%(align) ) {                                         \
        m_exception = X_ADEL;                                          \
        r_bar = address;                                               \
        return;                                                        \
    }

void Mips32Iss::op_sdc1()
{
    uint32_t addr =  r_gp[m_ins.fpu_i.base] + sign_ext(m_ins.fpu_i.offset, 16);
    if (!isCopAccessible(1)) {
        r_cause.ce = 1;
        m_exception = X_CPU;
        return;
    }
    check_align(addr, 8);
    do_mem_access(addr, 4, 0, 0, 0, r_f[m_ins.fpu_i.ft], DATA_WRITE);

    m_part_addr = addr+4;
    m_part_data = m_ins.fpu_i.ft+1;
    m_microcode_func = &Mips32Iss::op_sdc1_part2;
}

void Mips32Iss::op_swc1()
{
    uint32_t address =  r_gp[m_ins.fpu_i.base] + sign_ext(m_ins.fpu_i.offset, 16);
    if (!isCopAccessible(1)) {
        r_cause.ce = 1;
        m_exception = X_CPU;
        return;
    }
    check_align(address, 4);
    do_mem_access(address, 4, 0, 0, 0, r_f[m_ins.fpu_i.ft], DATA_WRITE);
}

void Mips32Iss::op_ldc1_part2()
{
    do_mem_access(m_part_addr, 4, 0, &r_f[m_part_data], 0, 0, DATA_READ);
    m_microcode_func = NULL;
}

void Mips32Iss::op_ldc1()
{
    uint32_t addr =  sign_ext(m_ins.fpu_i.offset, 16) + r_gp[m_ins.fpu_i.base];
    if (!isCopAccessible(1)) {
        r_cause.ce = 1;
        m_exception = X_CPU;
        return;
    }
    check_align(addr, 8);
    do_mem_access(addr, 4, 0, &r_f[m_ins.fpu_i.ft], 0, 0, DATA_READ);

    m_part_addr = addr+4;
    m_part_data = m_ins.fpu_i.ft+1;
    m_microcode_func = &Mips32Iss::op_ldc1_part2;
}

void Mips32Iss::op_lwc1()
{
    uint32_t addr = sign_ext(m_ins.fpu_i.offset, 16) + r_gp[m_ins.fpu_i.base];
    if (!isCopAccessible(1)) {
        r_cause.ce = 1;
        m_exception = X_CPU;
        return;
    }
    check_align(addr, 4);
    do_mem_access(addr, 4, 0, &r_f[m_ins.fpu_i.ft], 0, 0, DATA_READ);
}

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
