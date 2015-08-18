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

#include <cassert>

namespace soclib { namespace common {

#define tmpl(x) template<unsigned int NWINDOWS> x Sparcv8Iss<NWINDOWS>
#define func_table(table_name) template <unsigned int NWINDOWS>         \
    typename Sparcv8Iss<NWINDOWS>::func_entry const Sparcv8Iss<NWINDOWS>::table_name[]

#define op(x) {&Sparcv8Iss<NWINDOWS>::op_##x, #x}
#define op4(x, y, z, t) op(x), op(y), op(z), op(t)

// Format2 instructions (branches, sethi, garbage)
func_table(format2_table) = { 
    op4(unimp,  unimp,  branch,  unimp),
    op4(sethi,  unimp,  fbranch, cbranch),
};

func_table(branch_table) = { 
    op4(bn,     be,     ble,    bl ), 
    op4(bleu,   bcs,    bneg,   bvs),
    op4(ba,     bne,    bg,     bge), 
    op4(bgu,    bcc,    bpos,   bvc),
};

func_table(fbranch_table) = { 
    op4(fbn,    fbne,   fblg,   fbul), 
    op4(fbl,    fbug,   fbg,    fbu),
    op4(fba,    fbe,    fbue,   fbge),
    op4(fbuge,  fble,   fbule,  fbo),
};

func_table(cbranch_table) = { 
    op4(cbn,    cb123,  cb12,   cb13),
    op4(cb1,    cb23,   cb2,    cb3),
    op4(cba,    cb0,    cb03,   cb02),
    op4(cb023,  cb01,   cb013,  cb012),
};


// Logical instructions
func_table(logical_table) = {
    op4(add,    and,    or,     xor),
    op4(sub,    andn,   orn,    xnor),
    op4(addx,   unimp,  umul,   smul),
    op4(subx,   unimp,  udiv,   sdiv),

    op4(addcc,  andcc,  orcc,   xorcc),
    op4(subcc,  andncc, orncc,  xnorcc),
    op4(addxcc, unimp,  umulcc, smulcc),
    op4(subxcc, unimp,  udivcc, sdivcc),

    op4(taddcc, tsubcc, taddcctv, tsubcctv),
    op4(mulscc, sll,    srl,    sra),
    op4(rdasr,  rdpsr,  rdwim,  rdtbr),
    op4(unimp,  unimp,  unimp,  unimp),

    op4(wrasr,  wrpsr,  wrwim,  wrtbr),
    op4(fpop1,  fpop2,  cpop1,  cpop2),
    op4(jmpl,   rett,   ticc,   flush),
    op4(save,   restore,unimp,  unimp),
};

func_table(ticc_table) = {
    op4(tn,     te,     tle,    tl),
    op4(tleu,   tcs,    tneg,   tvs),
    op4(ta,     tne,    tg,     tge),
    op4(tgu,    tcc,    tpos,   tvc),
};

func_table(loadstore_table) = {
    op4(ld,     ldub,   lduh,   ldd),
    op4(st,     stb,    sth,    std),
    op4(unimp,  ldsb,   ldsh,   unimp),
    op4(unimp,  ldstub, unimp,  swap),

    op4(lda,    lduba,  lduha,  ldda),
    op4(sta,    stba,   stha,   stda),
    op4(unimp,  ldsba,  ldsha,  unimp),
    op4(unimp,  ldstuba,unimp,  swapa),

    op4(ldf,    ldfsr,  unimp,  lddf),
    op4(stf,    stfsr,  stdfq,  stdf),
    op4(unimp,  unimp,  unimp,  unimp),
    op4(unimp,  unimp,  unimp,  unimp),

    op4(ldc,    ldcsr,  unimp,  lddc),
    op4(stc,    stcse,  stdcq,  stdc),
    op4(unimp,  unimp,  unimp,  unimp),
    op4(casa,   unimp,  unimp,  unimp),
};

#if FPU
func_table(fpop1_table) = {
    op4(fpill,  fmovs,  fpill,  fpill),
    op4(fpill,  fnegs,  fpill,  fpill),
    op4(fpill,  fabss,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fsqrts, fsqrtd, fsqrtq),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fadds,  faddd,  faddq),
    op4(fpill,  fsubs,  fsubd,  fsubq),
    op4(fpill,  fmuls,  fmuld,  fmulq),
    op4(fpill,  fdivs,  fdivd,  fdivq),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fsmuld, fpill,  fpill),
    op4(fpill,  fpill,  fdmulq, fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fitos,  fpill,  fdtos,  fqtos),
    op4(fitod,  fstod,  fpill,  fqtod),
    op4(fitoq,  fstoq,  fdtoq,  fpill),
    op4(fpill,  fstoi,  fdtoi,  fqtoi),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
    op4(fpill,  fpill,  fpill,  fpill),
};

func_table(fpop2_table) = {
    op4(fpill,  fcmps,  fcmpd,  fcmpq),
    op4(fpill,  fcmpes, fcmped, fcmpeq),
};
#endif

#undef op
#undef op4


tmpl(void)::run()
{
    switch(m_ins.call.op) {
    case 0 : {
        // This is a format2 instruction : sethi, branch, branch_fp, branch_cp, unimp or garbage...
        //   - op2  : tells wether it's a sethi, branch, branch_cp, ...
        //   - cond : tells which particular branch it is
        void (Sparcv8Iss::*func)() = Sparcv8Iss::format2_table[m_ins.branches.op2].func;
        (this->*func)();
        break;
    }

    case 1 :
        // This is a format1 instruction : CALL
        this->op_call();
        break;

    case 2 : {
        // This is one of the logical instructions (format3)
        void (Sparcv8Iss::*func)()  = Sparcv8Iss::logical_table[m_ins.format3.op3].func;
        (this->*func)();
        break;
    }

    case 3 :
        // This is one of the load/store instructions (format3)
        void (Sparcv8Iss::*func)()   = Sparcv8Iss::loadstore_table[m_ins.format3.op3].func;
        (this->*func)();
        break;
    }
}

tmpl(void)::op_branch()
{
    void (Sparcv8Iss::*func)() = Sparcv8Iss::branch_table[m_ins.branches.cond].func;
    (this->*func)();
}

tmpl(void)::op_fbranch()
{
    void (Sparcv8Iss::*func)() = Sparcv8Iss::fbranch_table[m_ins.branches.cond].func;
    (this->*func)();
}

tmpl(void)::op_cbranch()
{
    void (Sparcv8Iss::*func)() = Sparcv8Iss::cbranch_table[m_ins.branches.cond].func;
    (this->*func)();
}

tmpl(void)::op_ticc()
{
    void (Sparcv8Iss::*func)() = Sparcv8Iss::ticc_table[m_ins.branches.cond].func;
    (this->*func)();
}

#if FPU
tmpl(void)::op_fpop1()
{
    void (Sparcv8Iss::*func)() = Sparcv8Iss::fpop1_table[m_ins.format3c.opf].func;
    (this->*func)();
}

tmpl(void)::op_fpop2()
{
    void (Sparcv8Iss::*func)() = Sparcv8Iss::fpop2_table[m_ins.format3c.opf-0x50].func;
    (this->*func)();
}
#else
tmpl(void)::op_fpop1()
{
    m_exception = true;
    m_exception_cause = TP_FP_DISABLED;
}

tmpl(void)::op_fpop2()
{
    m_exception = true;
    m_exception_cause = TP_FP_DISABLED;
}
#endif


tmpl(bool)::evaluate_icc(unsigned char cond) {
    switch(cond) {
    case 0: return false;
    case 1: return r_psr.z;
    case 2: return (r_psr.z | (r_psr.n ^ r_psr.v));
    case 3: return (r_psr.n ^ r_psr.v);
    case 4: return (r_psr.c | r_psr.z);
    case 5: return r_psr.c;
    case 6: return r_psr.n;
    case 7: return r_psr.v;
    case 8: return true;
    case 9: return !r_psr.z;
    case 10: return !(r_psr.z | (r_psr.n ^ r_psr.v));
    case 11: return !(r_psr.n ^ r_psr.v);
    case 12: return !(r_psr.c | r_psr.z);
    case 13: return !r_psr.c;
    case 14: return !r_psr.n;
    case 15: return !r_psr.v;
    default : 
        assert(false);
        return false;
    }
}

tmpl(uint32_t)::get_absolute_dest_reg(ins_t ins) const
{
    switch(ins.call.op) {
    case 0 : {
        // Format2 instruction : sethi, branch, branch_fp, branch_cp, unimp or garbage...
        if (ins.branches.op2 == 4)
            // SETHI
            return RR(ins.sethi.rd);
        else
            return 0;
        break;
    }

    case 1 :
        // This is a format1 instruction : CALL
        return 0;
        break;

    case 2 : 
    case 3 :
        // This is one of the logical instructions (format3)
        return RR(ins.format3.rd);
        break;
    }
    return 0;
}


tmpl(std::string)::get_ins_name(void) const
{
    switch(m_ins.call.op) {
    case 0 :
        // This is a format2 instruction : sethi, branch, branch_fp, branch_cp, unimp or garbage...
        //   - op2  : tells wether it's a sethi, branch, branch_cp, ...
        //   - cond : tells which particular branch it is
        // Treat "sethi , 0" as special case (nop)
        switch(m_ins.branches.op2) {
        case 4:
            if ((m_ins.sethi.rd == 0) && (m_ins.sethi.imm == 0))
                return "nop";
            else 
                return "sethi";
            break;
        case 2:
            return this->branch_table[m_ins.branches.cond].name;
            break;
        case 6:
            return this->fbranch_table[m_ins.branches.cond].name;
            break;
        case 7:
            return this->cbranch_table[m_ins.branches.cond].name;
            break;
        default:
            return "unimp";
            break;
        }
        break;

    case 1 :
        // This is a format1 instruction : CALL
        return "call";
        break;

    case 2 :
        // This is one of the logical instructions (format 3)
        // Special case for ticc and fpops
        if (m_ins.format3.op3 == 58)
            return this->ticc_table[m_ins.branches.cond].name;
#if FPU
        if (m_ins.format3.op3 == 52)
            return this->fpop1_table[m_ins.format3c.opf].name;
        if (m_ins.format3.op3 == 53)
            return this->fpop2_table[m_ins.format3c.opf-0x50].name;
#endif
        return this->logical_table[m_ins.format3.op3].name;
        break;

    case 3 :
        // This is one of the load/store instructions (format 3)
        return this->loadstore_table[m_ins.format3.op3].name;
        break;
    }
    return "Unable to get current instruction's name.";
}

}}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

