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
 * $Id: thumb_decoding_table.cpp 1694 2010-04-05 18:22:46Z nipo $
 *
 */

#include "arm.h"

namespace soclib { namespace common { 

#undef _
#define _(...) &ArmIss::__VA_ARGS__
#undef op
#define op(...) _(thumb_##__VA_ARGS__)

int8_t const ArmIss::thumb_table_main[32] = {
     22,  23,  25,  -3,  24,  43,  32,  33,  -1,  56,  55,  55,  57,  58,  59,  60, 
     63,  63,  61,  62,  34,  35,  -2,  -4,  44,  45,  38,  38,  39,  42,  40,  41, 
};
int8_t ArmIss::thumb_func_main(data_t opcode)
{
    size_t index = ((opcode >> 11) & 0x1f);
    int8_t op = thumb_table_main[index];
    if ( op < 0 ) {
        decod_func_t f = thumb_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::thumb_table_10110XXXXX[32] = {
     30,  30,  31,  31,   0,   0,   0,   0,  48,  49,  50,  51,   0,   0,   0,   0, 
     47,  47,  47,  47,  47,  47,  47,  47,   0,  36,   0,   0,   0,   0,   0,   0, 
};
int8_t ArmIss::thumb_func_10110XXXXX(data_t opcode)
{
    size_t index = ((opcode >> 6) & 0x1f);
    int8_t op = thumb_table_10110XXXXX[index];
    return op;
}
int8_t const ArmIss::thumb_table_10111XXXXX[32] = {
      0,   0,   0,   0,   0,   0,   0,   0,  52,  53,   0,  54,   0,   0,   0,   0, 
     46,  46,  46,  46,  46,  46,  46,  46,  37,  37,  37,  37,   0,   0,   0,   0, 
};
int8_t ArmIss::thumb_func_10111XXXXX(data_t opcode)
{
    size_t index = ((opcode >> 6) & 0x1f);
    int8_t op = thumb_table_10111XXXXX[index];
    return op;
}
int8_t const ArmIss::thumb_table_01000XXXXX[32] = {
      2,  12,  13,  14,   5,   1,  21,  20,   4,  18,   9,   6,  19,  16,   3,  17, 
     15,  15,  15,  15,  10,  10,  10,  10,  11,  11,  11,  11,   7,   7,   8,   8, 
};
int8_t ArmIss::thumb_func_01000XXXXX(data_t opcode)
{
    size_t index = ((opcode >> 6) & 0x1f);
    int8_t op = thumb_table_01000XXXXX[index];
    return op;
}
int8_t const ArmIss::thumb_table_00011XX000[4] = {
     28,  29,  26,  27, 
};
int8_t ArmIss::thumb_func_00011XX000(data_t opcode)
{
    size_t index = ((opcode >> 9) & 0x3);
    int8_t op = thumb_table_00011XX000[index];
    return op;
}
ArmIss::decod_func_t const ArmIss::thumb_func_funcs[4] = {
    _(thumb_func_01000XXXXX), _(thumb_func_10110XXXXX), _(thumb_func_00011XX000), 
    _(thumb_func_10111XXXXX), 
};
ArmIss::func_t const ArmIss::thumb_funcs[64] = {
    op(ill), op(adc), op(and), op(bic), op(tst), op(asr), op(cmn), op(bx_r<false>), 
    op(bx_r<true>), op(cmp), op(cmp_hi), op(cpy), op(eor), op(lsl), op(lsr), 
    op(add_hi), op(mul), op(mvn), op(neg), op(orr), op(ror), op(sbc), op(lsl_imm5), 
    op(lsr_imm5), op(mov_imm8), op(asr_imm5), op(add_imm3), op(sub_imm3), 
    op(add_reg), op(sub_reg), op(sp_add), op(sp_sub), op(add_imm8), op(sub_imm8), 
    op(add2pc), op(add2sp), op(cps_setend), op(bkpt), op(bcond_swi), op(b), 
    op(b_hi), op(bl), op(blx), op(cmp_imm), op(ldstmia<false>), op(ldstmia<true>), 
    op(pop), op(push), op(xt<false,false>), op(xt<false,true>), op(xt<true,false>), 
    op(xt<true,true>), op(rev), op(rev16), op(revsh), op(ldst), op(ldr_pcrel), 
    op(ldst_imm5<false,false>), op(ldst_imm5<true,false>), 
    op(ldst_imm5<false,true>), op(ldst_imm5<true,true>), op(ldst_sprel<false>), 
    op(ldst_sprel<true>), op(ldsth_imm5), 
};
const char * const ArmIss::thumb_func_names[64] = {
    "", "adc", "and", "bic", "tst", "asr", "cmn", "bx_r<false>", "bx_r<true>", 
    "cmp", "cmp_hi", "cpy", "eor", "lsl", "lsr", "add_hi", "mul", "mvn", "neg", 
    "orr", "ror", "sbc", "lsl_imm5", "lsr_imm5", "mov_imm8", "asr_imm5", 
    "add_imm3", "sub_imm3", "add_reg", "sub_reg", "sp_add", "sp_sub", "add_imm8", 
    "sub_imm8", "add2pc", "add2sp", "cps_setend", "bkpt", "bcond_swi", "b", "b_hi", 
    "bl", "blx", "cmp_imm", "stmia", "ldmia", "pop", "push", "sxth", "sxtb", 
    "uxth", "uxtb", "rev", "rev16", "revsh", "ldst", "ldr_pcrel", 
    "store word imm5", "load word imm5", "store byte imm5", "load byte imm5", 
    "store sp rel", "load sp rel", "store half, load half", 
};


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
