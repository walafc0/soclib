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
 * Based on initial work by Frédéric Pétrot and Pierre Guironnet de Massas
 *
 * Maintainers: Alexis Polti
 *
 * $Id$
 */

#include "sparcv8.h"
#include <cstring>
#include <math.h>
#include <stdint.h>
#include <sys/param.h>

#include "soclib_endian.h"

// FPU special numbers denifition :
#define PZEROs                  0x00000000
#define NZEROs                  0x80000000
#define ZEROsMask               0x7FFFFFFF
#define PINFIs                  0x7F800000
#define NINFIs                  0xFF800000
#define INFIsMask               0x7F800000

#define SNaNsMaskE              0x7FC00000 // must be equal to PINFIs
#define NaNsMaskF               0x003FFFFF // must non zero
#define NaNdMaskF               0x0007FFFF // must non zero
#define QNaNsMaskE              0x7FC00000 // must be equal to 0x7FC0000

#define PZEROd                  0x0000000000000000
#define NZEROd                  0x8000000000000000
#define ZEROdMask               0x7FFFFFFFFFFFFFFF
#define PINFId                  0x7FE00000
#define NINFId                  0xFFE00000

#define SNaNdMaskE              0x7FF80000 
#define QNaNdMaskE              0x7FF80000 // must be equal to 0x7FC0000

// Special FPU number tests:
#define Is_ZEROs(x)                                 \
	((x == PZEROs) || (x == NZEROs))

#define Is_SNaNs(x)                                                     \
	(((x & SNaNsMaskE)== 0x7F800000) && (x & NaNsMaskF))

#define Is_QNaNs(x)                                     \
	(((x & QNaNsMaskE)== 0x7FC00000))

#define Is_NaNs(x)                              \
	(Is_SNaNs(x) || Is_QNaNs(x))

#define Is_PINFs(x)                                 \
	((unsigned int)(x) == PINFIs)

#define Is_NINFs(x)                                 \
	((unsigned int)(x) == NINFIs)

#define Is_INFs(x)                              \
	(Is_PINFs(x) || Is_NINFs(x))

#define Is_ZEROd(x)                                 \
	((x == PZEROd) || (x == NZEROd))

#define Is_SNaNd(x,y)                                                   \
	(((x & SNaNdMaskE)== 0x7FF00000) && ((x & NaNdMaskF) | y ))

#define Is_QNaNd(x,y)                                   \
	(((x & SNaNdMaskE)== 0x7FF80000) )

#define Is_NaNd(x,y)                                \
	(Is_SNaNd(x,y) || Is_QNaNd(x,y))

#define Is_PINFd(x,y)                                                   \
	(((unsigned int)(x) == PINFId) && (y == 0x00000000))

#define Is_NINFd(x,y)                                                   \
	(((unsigned int)(x) == NINFId) && (y == 0x00000000))


inline unsigned int only_msb(unsigned int v){
    int cpt = 0;
    unsigned int mask = 0x10000000;
	while (((v & mask) == 0) && (cpt < 32)){
		mask = mask >> 1;
		cpt ++;
	}
	return mask;
}


typedef enum {NONE, S, D, Q} vfloat_type_t;

typedef struct {
    union 
    {
        uint32_t    i;
        float       f;
        double      d;
        struct {
            ENDIAN_BITFIELD(
                uint32_t hw,
                uint32_t lw,
                );
        }  parts;
    };
    vfloat_type_t type;
} vfloat_t;


#define START_FPOP(restype)                                 \
    __attribute__((unused)) uint32_t rs1, rs2, rd;                 \
    __attribute__((unused)) vfloat_t op1, op2, res;                                 \
    unsigned char texc = 0;                                 \
    unsigned char tfcc = 0;                                 \
    bool unfinished = false;                                \
    rs1 = m_ins.format3c.rs1;                               \
    rs2 = m_ins.format3c.rs2;                               \
    rd = m_ins.format3c.rd;                                 \
    op1.d = 0;                                              \
    op2.d = 0;                                              \
    res.d = 0;                                              \
    res.type = restype;                                     \
    setInsDelay(16);

#define END_FPOP(rd)                                                    \
    if (m_exception)                                                    \
        return;                                                         \
    if(unfinished) {                                                    \
        m_exception = true;                                             \
        m_exception_cause = TP_FP_EXCEPTION;                            \
        r_fsr.ftt = TP_UNFINISHED_FPOP;                                 \
        return;                                                         \
    }                                                                   \
    if ((texc & r_fsr.tem) != 0) {                                      \
        m_exception = true;                                             \
        m_exception_cause = TP_FP_EXCEPTION;                            \
        r_fsr.ftt = TP_IEEE_754_EXCEPTION;                              \
        r_fsr.cexc = only_msb(texc & r_fsr.tem);                        \
        return;                                                         \
    }                                                                   \
    r_fsr.aexc |= texc;                                                 \
    r_fsr.cexc = texc;                                                  \
    r_fsr.fcc = tfcc;                                                   \
    switch(res.type) {                                                  \
    case S :                                                            \
        r_f[rd] = res.i;                                                \
        break;                                                          \
    case D :                                                            \
        r_f[rd] = res.parts.hw;                                         \
        r_f[rd+1] = res.parts.lw;                                       \
        break;                                                          \
    case Q :                                                            \
        std::cout << name()                                             \
                  << "Quad precision FP operation not supported!"       \
                  << std::endl;                                         \
        break;                                                          \
    default:                                                            \
        break;                                                          \
    }                                                                   


#define ENSURE_ALIGNED(val, byte_count)                                 \
    do {                                                                \
        if (val & (byte_count - 1)) {                                   \
            m_exception = true;                                         \
            m_exception_cause = TP_FP_EXCEPTION;                        \
            r_fsr.ftt = TP_INVALID_FP_REGISTER;                         \
            return;                                                     \
        }                                                               \
    } while(0)



namespace soclib { namespace common {

#define tmpl(x) template<unsigned int NWINDOWS> x Sparcv8Iss<NWINDOWS>

tmpl(void)::op_fitos() 
{
    START_FPOP(S);
    setInsDelay(2);
    res.f = (float) r_f[rs2];
    END_FPOP(rd);
}

tmpl(void)::op_fitod() 
{
    START_FPOP(D);
    setInsDelay(2);
    res.d = (double) r_f[rs2];
    END_FPOP(rd);
} 

tmpl(void)::op_fstoi() 
{
    START_FPOP(S);
    setInsDelay(2);
    if (Is_SNaNs(r_f[rs2])) 
        texc = IEEE754_INVALID;
    op2.i = r_f[rs2];
    res.i = (int)op2.f;
    END_FPOP(rd);
}

tmpl(void)::op_fdtoi()
{
    START_FPOP(S);
    ENSURE_ALIGNED(rs2, 2);
    setInsDelay(2);
    if (Is_SNaNd(r_f[rs2], r_f[rs2 + 1]))
        texc = IEEE754_INVALID;
    op2.parts.hw =  r_f[rs2];
    op2.parts.lw =  r_f[rs2 + 1];
    res.i = (int)op2.d;
    END_FPOP(rd);
}

tmpl(void)::op_fstod() 
{
    START_FPOP(D);
    ENSURE_ALIGNED(rd, 2);
    setInsDelay(2);
    if (Is_SNaNs(r_f[rs2])) 
        texc = IEEE754_INVALID;
    op2.i =  r_f[rs2];
    res.d = (double)op2.f;
    END_FPOP(rd);
}

tmpl(void)::op_fdtos()
{
    START_FPOP(S);
    ENSURE_ALIGNED(rd, 2);
    setInsDelay(2);
    if (Is_SNaNd(r_f[rs2], r_f[rs2 + 1]))
        texc = IEEE754_INVALID;
    op2.parts.hw =  r_f[rs2];
    op2.parts.lw =  r_f[rs2 + 1];
    res.f = (float)op2.d;
    END_FPOP(rd);
}

tmpl(void)::op_fmovs() 
{
    START_FPOP(S);
    setInsDelay(2);
    res.i = r_f[rs2]; 
    END_FPOP(rd);
}

tmpl(void)::op_fnegs() 
{
    START_FPOP(S);
    setInsDelay(2);
    res.i = r_f[rs2] ^ 0x80000000;
    END_FPOP(rd);
}

tmpl(void)::op_fabss() 
{
    START_FPOP(S);
    setInsDelay(2);
    res.i = r_f[rs2] & 0x7FFFFFFF;
    END_FPOP(rd);
}

tmpl(void)::op_fsqrts()
{
    START_FPOP(S);
    op2.i = r_f[rs2];
    if (op2.f < 0){
        // What should we do : don't finish or raise INVALID exception ???
        // Hmmm.. do both, see later :)
        texc = IEEE754_INVALID;
        unfinished = true; 
    }
    res.f = sqrt(op2.f);
    END_FPOP(rd);
}

tmpl(void)::op_fsqrtd()
{
    START_FPOP(D);
    ENSURE_ALIGNED(rd, 2);
    ENSURE_ALIGNED(rs2, 2);
    op2.parts.hw = r_f[rs2];
    op2.parts.lw = r_f[rs2+1];
    if (op2.d < 0){
        // What should we do : don't finish or raise INVALID exception ???
        // Hmmm.. do both, see later :)
        texc = IEEE754_INVALID;
        unfinished = true; 
    }
    res.d = sqrt(op2.d);
    printf("ALEXIS : op2 = %f, res=%f\n", op2.d, res.d);
    END_FPOP(rd);
}

tmpl(void)::op_fadds()
{
    START_FPOP(S);
    op1.i = r_f[rs1];
    op2.i = r_f[rs2];
    // inf - inf : NV exception !
    if ((Is_PINFs(r_f[rs1]) && Is_NINFs(r_f[rs2])) || 
        (Is_PINFs(r_f[rs2]) && Is_NINFs(r_f[rs1])))
        texc = IEEE754_INVALID;
    res.f = op1.f + op2.f;
    END_FPOP(rd);
}

tmpl(void)::op_faddd()
{
    START_FPOP(D);
    ENSURE_ALIGNED(rd, 2);
    ENSURE_ALIGNED(rs1, 2);
    ENSURE_ALIGNED(rs2, 2);
    op1.parts.hw = r_f[rs1];
    op1.parts.lw = r_f[rs1+1];
    op2.parts.hw = r_f[rs2];
    op2.parts.lw = r_f[rs2+1];

    if ((Is_PINFd(r_f[rs1], r_f[rs1 + 1]) && Is_NINFd(r_f[rs2], r_f[rs2 + 1])) ||
        (Is_PINFd(r_f[rs2], r_f[rs2 + 1]) && Is_NINFd(r_f[rs1], r_f[rs1 +1])))
        texc = IEEE754_INVALID; 
    res.d = op1.d + op2.d;
    END_FPOP(rd);
}

tmpl(void)::op_fsubs()
{
    START_FPOP(S);
    op1.i = r_f[rs1];
    op2.i = r_f[rs2];
    // inf - inf : NV exception !
    if ((Is_PINFs(r_f[rs1]) && Is_NINFs(r_f[rs2])) || 
        (Is_PINFs(r_f[rs2]) && Is_NINFs(r_f[rs1])))
        texc = IEEE754_INVALID;
    res.f = op1.f - op2.f;
    END_FPOP(rd);
}

tmpl(void)::op_fsubd()
{
    START_FPOP(D);
    ENSURE_ALIGNED(rd, 2);
    ENSURE_ALIGNED(rs1, 2);
    ENSURE_ALIGNED(rs2, 2);
    op1.parts.hw = r_f[rs1];
    op1.parts.lw = r_f[rs1+1];
    op2.parts.hw = r_f[rs2];
    op2.parts.lw = r_f[rs2+1];

    if ((Is_PINFd(r_f[rs1], r_f[rs1 + 1]) && Is_NINFd(r_f[rs2], r_f[rs2 + 1])) ||
        (Is_PINFd(r_f[rs2], r_f[rs2 + 1]) && Is_NINFd(r_f[rs1], r_f[rs1 +1])))
        texc = IEEE754_INVALID; 
    res.d = op1.d - op2.d;
    END_FPOP(rd);
}

tmpl(void)::op_fmuls()
{
    START_FPOP(S);
    op1.i = r_f[rs1];
    op2.i = r_f[rs2];
    // inf - inf : NV exception !
    if (Is_SNaNs(r_f[rs1]) || Is_SNaNs(r_f[rs2]))
        texc = IEEE754_INVALID;
    res.f = op1.f * op2.f;
    END_FPOP(rd);
}

tmpl(void)::op_fmuld()
{
    START_FPOP(D);
    ENSURE_ALIGNED(rd, 2);
    ENSURE_ALIGNED(rs1, 2);
    ENSURE_ALIGNED(rs2, 2);
    op1.parts.hw = r_f[rs1];
    op1.parts.lw = r_f[rs1+1];
    op2.parts.hw = r_f[rs2];
    op2.parts.lw = r_f[rs2+1];

    if (Is_SNaNd(r_f[rs1], r_f[rs1 + 1]) || Is_SNaNd(r_f[rs2], r_f[rs2 + 1]))
        texc = IEEE754_INVALID; 
    res.d = op1.d * op2.d;
    END_FPOP(rd);
}

tmpl(void)::op_fsmuld()
{
    START_FPOP(D);
    ENSURE_ALIGNED(rd, 2);
    op1.i = r_f[rs1];
    op2.i = r_f[rs2];

    if ((Is_SNaNs(r_f[rs1]))||(Is_SNaNs(r_f[rs2]))) 
        texc = IEEE754_INVALID;
    res.d = op1.f * op2.f;
    END_FPOP(rd);
}

tmpl(void)::op_fdivs()
{
    START_FPOP(S);
    op1.i = r_f[rs1];
    op2.i = r_f[rs2];

    if (Is_SNaNs(r_f[rs2]))
        texc = IEEE754_INVALID;
    // 0/0 does not raise an exception
    if ((op2.f == 0) && (op1.f!=0))
        texc = IEEE754_DIVBYZERO; 
    if (op2.f == 0)
        res.f = op1.f / op2.f;
    END_FPOP(rd);
}

tmpl(void)::op_fdivd()
{
    START_FPOP(D);
    ENSURE_ALIGNED(rd, 2);
    ENSURE_ALIGNED(rs1, 2);
    ENSURE_ALIGNED(rs2, 2);
    op1.parts.hw = r_f[rs1];
    op1.parts.lw = r_f[rs1+1];
    op2.parts.hw = r_f[rs2];
    op2.parts.lw = r_f[rs2+1];

    if (Is_SNaNd(r_f[rs1], r_f[rs1 + 1]) || Is_SNaNd(r_f[rs2], r_f[rs2 + 1]))
        texc = IEEE754_INVALID;
    if ((op2.d == 0) && (op1.d!=0))
        texc = IEEE754_DIVBYZERO; 
    if (op2.f == 0)
        res.d = op1.d / op2.d;
    END_FPOP(rd);
}

tmpl(void)::op_fcmps()
{
    START_FPOP(NONE);
    op1.i = r_f[rs1];
    op2.i = r_f[rs2];

    if (Is_SNaNs(r_f[rs1]) || Is_SNaNs(r_f[rs2])){
        texc = IEEE754_INVALID;
        tfcc = 0x3;
    } 
    else if (op1.f == op2.f)
        tfcc = 0;
    else if (op1.f < op2.f)
        tfcc = 0x1;
    else if (op1.f > op2.f)
        tfcc = 0x2;
    else
        tfcc = 0x3;

    END_FPOP(rd);
}

tmpl(void)::op_fcmpd()
{
    START_FPOP(NONE);
    ENSURE_ALIGNED(rs1, 2);
    ENSURE_ALIGNED(rs2, 2);
    op1.parts.hw = r_f[rs1];
    op1.parts.lw = r_f[rs1+1];
    op2.parts.hw = r_f[rs2];
    op2.parts.lw = r_f[rs2+1];

    if (Is_SNaNd(r_f[rs1], r_f[rs1 + 1]) || Is_SNaNd(r_f[rs2], r_f[rs2 + 1])){
        texc = IEEE754_INVALID;
        tfcc = 0x3;
    } 
    else if (op1.d == op2.d)
        tfcc = 0;
    else if (op1.d < op2.d)
        tfcc = 0x1;
    else if (op1.d > op2.d)
        tfcc = 0x2;
    else
        tfcc = 0x3;

    END_FPOP(rd);
}


tmpl(void)::op_fcmpes()
{
    op_fcmps();
}

tmpl(void)::op_fcmped()
{
    op_fcmpd();
}

tmpl(void)::op_fcmpeq()
{
    op_fcmpq();
}

tmpl(void)::op_fpill()
{
    op_fcmpq();
}


// These instructions are not implemented (quad precision, no real hardware
// implement them...)
#define op(x) tmpl(void)::op_##x() {                        \
        m_exception = true;                                 \
        m_exception_cause = TP_FP_EXCEPTION;                \
        r_fsr.ftt = TP_UNIMPLEMENTED_FPOP;                  \
    }

#define op4(x, y, z, t) op(x); op(y); op(z); op(t);
op(fdivq);
op4(fcmpq, fitoq, fqtoi, fstoq);
op4(fdtoq, fqtos, fqtod, fsqrtq);
op4(faddq, fsubq, fmulq, fdmulq);
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

