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
 *         Alexandre Becoulet <alexandre.becoulet@free.fr>, 2010
 *         Nicolas Pouillon <nipo@ssji.net>, 2010
 *
 * Maintainers: nipo becoulet
 *
 * $Id: arm_decoding_table.cpp 1815 2010-06-25 11:31:20Z nipo $
 *
 */

#include "arm.h"

namespace soclib { namespace common { 

#undef _
#define _(...) &ArmIss::__VA_ARGS__
#undef op
#define op(...) _(arm_##__VA_ARGS__)

int8_t const ArmIss::arm_table_main[128] = {
    -18, -17, -17,  -5,  -4,  -4,  -4, -13, -12,  -1, -11, -10,  -6,  -6,  -6,  -7, 
    -17, -17, -17, -17,  -4,  -4,  -4,  -4,  -9,  -9,  -9,  -9,  -6,  -6,  -6,  -6, 
     46,  46,  46,  46,  47,  47,  47,  47,  48,  48,  48,  48,  49,  49,  49,  49, 
     50,  -2,  50,   0,  51,  -3,  51,  -8,  52,   0,  52,   0,  53,   0,  53,   0, 
     54,  54,  54,  54,  54,  54,  54,  54,  54,  54,  54,  54,  54,  54,  54,  54, 
     30,  30,  30,  30,  30,  30,  30,  30,  31,  31,  31,  31,  31,  31,  31,  31, 
     63,  63,  63,  63,  33,  33,  33,  33,  63,  63,  63,  63,  33,  33,  33,  33, 
     32,  55,  32,  55,  32,  57,  32,  57,  64,  64,  64,  64,  64,  64,  64,  64, 
};
int8_t ArmIss::arm_func_main(data_t opcode)
{
    size_t index = ((opcode >> 4) & 0x1)
        | ((opcode >> 6) & 0x2)
        | ((opcode >> 18) & 0x4)
        | ((opcode >> 21) & 0x78);
    int8_t op = arm_table_main[index];
    if ( op < 0 ) {
        decod_func_t f = arm_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_table_0000XXX10000[8] = {
      7,  11,  25,  19,   5,   3,  23,  21, 
};
int8_t ArmIss::arm_func_0000XXX10000(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x7);
    int8_t op = arm_table_0000XXX10000[index];
    return op;
}
int8_t const ArmIss::arm_table_0000XXX01XX1[32] = {
     60,  34,  38,  42,  56,  34,  38,  42,  86,  34,  38,  42,   0,  34,  38,  42, 
     65,  34,  38,  42,  66,  34,  38,  42,  62,  34,  38,  42,  61,  34,  38,  42, 
};
int8_t ArmIss::arm_func_0000XXX01XX1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_table_0000XXX01XX1[index];
    return op;
}
int8_t const ArmIss::arm_table_0001XXX00XX1[32] = {
      0,   0,   0,   0,  67,  68,   0,  69,   0,   0,   0,   0,  70,   0,   0,   0, 
     16,  16,  16,  16,  12,  12,  12,  12,   8,   8,   8,   8,  14,  14,  14,  14, 
};
int8_t ArmIss::arm_func_0001XXX00XX1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_table_0001XXX00XX1[index];
    return op;
}
int8_t const ArmIss::arm_table_0001XXX010X0[16] = {
     71,  71,  72,  73,   0,   0,  74,  74,  16,  16,  12,  12,   8,   8,  14,  14, 
};
int8_t ArmIss::arm_func_0001XXX010X0(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x1)
        | ((opcode >> 20) & 0xe);
    int8_t op = arm_table_0001XXX010X0[index];
    return op;
}
int8_t const ArmIss::arm_table_0011XXX00000[8] = {
      0,  59,   0,  59,  16,  12,   8,  14, 
};
int8_t ArmIss::arm_func_0011XXX00000(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x7);
    int8_t op = arm_table_0011XXX00000[index];
    return op;
}
int8_t const ArmIss::arm_table_0000XXX11XX1[32] = {
     60,  35,  39,  43,  56,  35,  39,  43,   0,  35,  39,  43,   0,  35,  39,  43, 
     65,  35,  39,  43,  66,  35,  39,  43,  62,  35,  39,  43,  61,  35,  39,  43, 
};
int8_t ArmIss::arm_func_0000XXX11XX1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_table_0000XXX11XX1[index];
    return op;
}
int8_t const ArmIss::arm_table_0001XXX00XX0[32] = {
     58,   0,   0,   0,  59,   0,   0,   0,  58,   0,   0,   0,  59,   0,   0,   0, 
     16,  16,  16,  16,  12,  12,  12,  12,   8,   8,   8,   8,  14,  14,  14,  14, 
};
int8_t ArmIss::arm_func_0001XXX00XX0(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_table_0001XXX00XX0[index];
    return op;
}
int8_t const ArmIss::arm_table_0000XXX00001[8] = {
      6,  10,  24,  18,   4,   2,  22,  20, 
};
int8_t ArmIss::arm_func_0000XXX00001(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x7);
    int8_t op = arm_table_0000XXX00001[index];
    return op;
}
int8_t const ArmIss::arm_table_0110X__00XX1[8] = {
      0,   0,   0,   0,   0,   0,   0, -15, 
};
int8_t ArmIss::arm_func_0110X__00XX1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 21) & 0x4);
    int8_t op = arm_table_0110X__00XX1[index];
    if ( op < 0 ) {
        decod_func_t f = arm_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_table_0110X_X11XX1[16] = {
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, -16,   0,   0, 
};
int8_t ArmIss::arm_func_0110X_X11XX1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x4)
        | ((opcode >> 20) & 0x8);
    int8_t op = arm_table_0110X_X11XX1[index];
    if ( op < 0 ) {
        decod_func_t f = arm_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_table_0001XXX11XX1[32] = {
      0,  37,  41,  45,   0,  37,  41,  45,   0,  37,  41,  45,   0,  37,  41,  45, 
     76,  37,  41,  45,   0,  37,  41,  45,   0,  37,  41,  45,   0,  37,  41,  45, 
};
int8_t ArmIss::arm_func_0001XXX11XX1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_table_0001XXX11XX1[index];
    return op;
}
int8_t const ArmIss::arm_table_0001XXX01XX1[32] = {
      1,  36,  40,  44,   0,  36,  40,  44,   1,  36,  40,  44,   0,  36,  40,  44, 
     75,  36,  40,  44,   0,  36,  40,  44,   0,  36,  40,  44,   0,  36,  40,  44, 
};
int8_t ArmIss::arm_func_0001XXX01XX1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_table_0001XXX01XX1[index];
    return op;
}
int8_t const ArmIss::arm_table_0000XXX00XX0[32] = {
      6,   6,   6,   6,  10,  10,  10,  10,  24,  24,  24,  24,  18,  18,  18,  18, 
      4,   4,   4,   4,   2,   2,   2,   2,  22,  22,  22,  22,  20,  20,  20,  20, 
};
int8_t ArmIss::arm_func_0000XXX00XX0(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_table_0000XXX00XX0[index];
    return op;
}
int8_t const ArmIss::arm_table_0110X_X10_X1[8] = {
      0,   0,   0,   0,   0,   0,   0, -14, 
};
int8_t ArmIss::arm_func_0110X_X10_X1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x1)
        | ((opcode >> 20) & 0x2)
        | ((opcode >> 21) & 0x4);
    int8_t op = arm_table_0110X_X10_X1[index];
    if ( op < 0 ) {
        decod_func_t f = arm_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_table_0001XXX10000[8] = {
     28,  29,  27,  26,  17,  13,   9,  15, 
};
int8_t ArmIss::arm_func_0001XXX10000(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x7);
    int8_t op = arm_table_0001XXX10000[index];
    return op;
}
int8_t const ArmIss::arm_table_01101XX00111[4] = {
     80,  82,  81,  83, 
};
int8_t ArmIss::arm_func_01101XX00111(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x3);
    int8_t op = arm_table_01101XX00111[index];
    return op;
}
int8_t const ArmIss::arm_table_01101X110X11[4] = {
     77,  84,   0,  85, 
};
int8_t ArmIss::arm_func_01101X110X11(data_t opcode)
{
    size_t index = ((opcode >> 6) & 0x1)
        | ((opcode >> 21) & 0x2);
    int8_t op = arm_table_01101X110X11[index];
    return op;
}
int8_t const ArmIss::arm_table_01101X111011[2] = {
     78,  79, 
};
int8_t ArmIss::arm_func_01101X111011(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_table_01101X111011[index];
    return op;
}
ArmIss::decod_func_t const ArmIss::arm_func_funcs[18] = {
    _(arm_func_0001XXX00XX1), _(arm_func_0110X__00XX1), _(arm_func_0110X_X10_X1), 
    _(arm_func_0000XXX10000), _(arm_func_0000XXX01XX1), _(arm_func_0001XXX10000), 
    _(arm_func_0001XXX11XX1), _(arm_func_0110X_X11XX1), _(arm_func_0011XXX00000), 
    _(arm_func_0001XXX01XX1), _(arm_func_0001XXX010X0), _(arm_func_0001XXX00XX0), 
    _(arm_func_0000XXX11XX1), _(arm_func_01101X110X11), _(arm_func_01101XX00111), 
    _(arm_func_01101X111011), _(arm_func_0000XXX00001), _(arm_func_0000XXX00XX0), 
};
ArmIss::func_t const ArmIss::arm_funcs[87] = {
    op(ill), op(swp), op(adc), op(adcs), op(add), op(adds), op(and), op(ands), 
    op(bic), op(bics), op(eor), op(eors), op(mov), op(movs), op(mvn), op(mvns), 
    op(orr), op(orrs), op(rsb), op(rsbs), op(rsc), op(rscs), op(sbc), op(sbcs), 
    op(sub), op(subs), op(cmns), op(cmps), op(tsts), op(teqs), op(b), op(bl), 
    op(cdp), op(ldc), op(ldstrh<2,false,false,false>), 
    op(ldstrh<2,false,true,false>), op(ldstrh<2,true,false,false>), 
    op(ldstrh<2,true,true,false>), op(ldstrh<1,false,false,true>), 
    op(ldstrh<1,false,true,true>), op(ldstrh<1,true,false,true>), 
    op(ldstrh<1,true,true,true>), op(ldstrh<2,false,false,true>), 
    op(ldstrh<2,false,true,true>), op(ldstrh<2,true,false,true>), 
    op(ldstrh<2,true,true,true>), op(ldstr<false,false,false>), 
    op(ldstr<false,false,true>), op(ldstr<false,true,false>), 
    op(ldstr<false,true,true>), op(ldstr<true,false,false>), 
    op(ldstr<true,false,true>), op(ldstr<true,true,false>), 
    op(ldstr<true,true,true>), op(ldstm), op(mcr), op(mla), op(mrc), op(mrs), 
    op(msr), op(mul), op(smlal), op(smull), op(stc), op(swi), op(umull), op(umlal), 
    op(bx), op(blx), op(bkpt), op(clz), op(smla_xy), op(smlaw_y), op(smulw_y), 
    op(smul_xy), op(strex), op(ldrex), op(rev), op(rev16), op(revsh), op(sxtb16), 
    op(uxtb16), op(sxtb), op(uxtb), op(sxth), op(uxth), op(umaal), 
};
const char * const ArmIss::arm_func_names[87] = {
    "", "swpb, swp", "adc", "adcs", "add", "adds", "and", "ands", "bic", "bics", 
    "eor", "eors", "mov", "movs", "mvn", "mvns", "orr", "orrs", "rsb", "rsbs", 
    "rsc", "rscs", "sbc", "sbcs", "sub", "subs", "cmn", "cmp", "tst", "teq", "b", 
    "bl", "cdp", "ldc", "strhu", "ldrhu", "strhu pre", "ldrhu pre", "strb", "ldrb", 
    "strb pre", "ldrb pre", "strh", "ldrh", "strh pre", "ldrh pre", "str imm", 
    "ldr imm", "str imm pre", "ldr imm pre", "str reg", "ldr reg", "str reg pre", 
    "ldr reg pre", "stm, ldm", "mcr", "mla, mlas", "mrc", "mrs", "msr (imm), msr", 
    "mul, muls", "smlals, smlal", "smull, smulls", "stc", "swi", "umulls, umull", 
    "umlal, umlals", "bx", "blx", "bkpt", "clz", "smlabt, smlatb, smlabb, smlatt", 
    "smlawb, smlawt", "smulwb, smulwt", "smultt, smulbb, smultb, smulbt", "strex", 
    "ldrex", "rev", "rev16", "revsh", "sxtb16", "uxtb16", "sxtb", "uxtb", "sxth", 
    "uxth", "umaal", 
};


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
