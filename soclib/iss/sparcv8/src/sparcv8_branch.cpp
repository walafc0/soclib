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
#include <cassert>
#include "arithmetics.h"

namespace soclib { namespace common {

#define tmpl(x) template<unsigned int NWINDOWS> x Sparcv8Iss<NWINDOWS>

tmpl(void)::op_unimp()
{
    std::cout << "OP_UNIMP : Illegal instruction !" << std::endl;
    m_exception = true;
    m_exception_cause = TP_UNIMPLEMENTED_INSTRUCTION;
}


#define GET_BR_OPERANDS(ins, cancel, addr)                   \
    do {                                                  \
        cancel = ins.branches.a;                          \
        addr = (uint32_t)sign_ext(ins.branches.disp, 22)*4; \
    } while(0)

  
#define op(x) tmpl(void)::op_##x() {                                    \
        std::cout <<  #x << " : no coprocessor!" << std::endl;          \
        m_exception = true;                                             \
        m_exception_cause = TP_CP_DISABLED;                             \
    }
#define op4(x, y, z, t) op(x); op(y); op(z); op(t);
  op4(cbn,   cb123, cb12,  cb13);
  op4(cb1,   cb23,  cb2,   cb3);
  op4(cba,   cb0,   cb03,  cb02);
  op4(cb023, cb01,  cb013, cb012);
#undef op
#undef op4



#define op(x) tmpl(void)::op_##x() {            \
    uint32_t cancel, addr;                      \
    GET_BR_OPERANDS(m_ins, cancel, addr);          \
    if(!evaluate_icc(m_ins.branches.cond))      \
        m_cancel_next_ins = cancel;             \
    else                                        \
        m_next_pc = r_pc + addr;                \
    }
#define op4(x, y, z, t) op(x); op(y); op(z); op(t);
#define op3(x, y, z) op(x); op(y); op(z);
  op4(bn,     be,     ble,    bl ); 
  op4(bleu,   bcs,    bneg,   bvs);
  op3(        bne,    bg,     bge);
  op4(bgu,    bcc,    bpos,   bvc);
#undef op
#undef op4
#undef op3


tmpl(void)::op_ba()
{
    uint32_t cancel, addr;
    GET_BR_OPERANDS(m_ins, cancel, addr);

    m_cancel_next_ins = cancel;
    m_next_pc = r_pc + addr;
}

#define ENSURE_EF_SET()                                                 \
    do {                                                                \
        if (r_psr.ef != 1) {                                            \
            m_exception = true;                                         \
            m_exception_cause = TP_FP_DISABLED;                         \
            return;                                                     \
        }                                                               \
    } while(0)

#define FPU_E   (r_fsr.fcc == 0x0) 
#define FPU_L   (r_fsr.fcc == 0x1)  
#define FPU_G   (r_fsr.fcc == 0x2)  
#define FPU_U   (r_fsr.fcc == 0x3)  

tmpl(bool)::evaluate_ficc(unsigned char cond) {
    switch(cond) {
    case 0: return false;
    case 1: return FPU_L | FPU_G | FPU_U;
    case 2: return FPU_L | FPU_G;
    case 3: return FPU_L | FPU_U;
    case 4: return FPU_L;
    case 5: return FPU_G | FPU_U;
    case 6: return FPU_G;
    case 7: return FPU_U;
    case 8: return true;
    case 9: return FPU_E;
    case 10: return FPU_E | FPU_U;
    case 11: return FPU_E | FPU_G;
    case 12: return FPU_E | FPU_G | FPU_U;
    case 13: return FPU_E | FPU_L;
    case 14: return FPU_E | FPU_L | FPU_U;
    case 15: return FPU_E | FPU_L | FPU_G;
    default : 
        assert(false);
        return false;
    }
}


#if FPU
#define op(x) tmpl(void)::op_##x() {            \
        uint32_t cancel, addr;                  \
        GET_BR_OPERANDS(m_ins, cancel, addr);      \
        if(!evaluate_ficc(m_ins.branches.cond)) \
            m_cancel_next_ins = cancel;         \
        else                                    \
            m_next_pc = r_pc + addr;            \
    }
#define op4(x, y, z, t) op(x); op(y); op(z); op(t);
#define op3(x, y, z) op(x); op(y); op(z); 
  op4(fbn,   fbne,   fblg,   fbul);
  op4(fbl,   fbug,   fbg,    fbu);
  op3(       fbe,    fbue,   fbge);
  op4(fbuge, fble,   fbule,  fbo);
#undef op3
#undef op4
#undef op

tmpl(void)::op_fba()
{
    uint32_t cancel, addr;
    GET_BR_OPERANDS(m_ins, cancel, addr);

    m_cancel_next_ins = cancel;
    m_next_pc = r_pc + addr;
}
#else
#define op(x) tmpl(void)::op_##x() {                                    \
        std::cout <<  #x << " : no floating point unit!" << std::endl;  \
        m_exception = true;                                             \
        m_exception_cause = TP_FP_DISABLED;                             \
    }
#define op4(x, y, z, t) op(x); op(y); op(z); op(t);
  op4(fbn,   fbne,   fblg,   fbul);
  op4(fbl,   fbug,   fbg,    fbu);
  op4(fba,   fbe,    fbue,   fbge);
  op4(fbuge, fble,   fbule,  fbo);
#undef op
#undef op4
#endif


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

