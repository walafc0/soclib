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
 *         Alexandre Becoulet <alexandre.becoulet@free.fr>, 2013
 *         Nicolas Pouillon <nipo@ssji.net>, 2013
 *
 * Maintainers: nipo becoulet
 *
 * $Id$
 *
 */

#include "arm.h"

namespace soclib { namespace common { 

#undef _
#define _(...) &ArmIss::__VA_ARGS__
#undef op
#define op(...) _(arm_uncond_##__VA_ARGS__)

int8_t const ArmIss::arm_uncond_table_main[128] = {
      0,   0,   0,   0,   0,   0,   0,   0, -17,  -6,  -9, -12,   0,   0,   0,   0, 
      2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2, 
      3,   3,   3,   3, -32, -24, -16, -11,   0,   0,   0,   0,  -8,  -7, -29, -13, 
      0,   0,   0,   0,  -5,   0, -35,   0,   0,   0,   0,   0, -18,   0,  -3,   0, 
    -30, -14,  -1, -21, -28, -25,  -2, -33, -22, -10, -31, -15, -34, -19,  -4, -26, 
     13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13, 
    -20, -20, -20, -20, -23, -23, -23, -23,  15,  15,  15,  15,  14,  14,  14,  14, 
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
};
int8_t ArmIss::arm_uncond_func_main(data_t opcode)
{
    size_t index = ((opcode >> 4) & 0x1)
        | ((opcode >> 6) & 0x2)
        | ((opcode >> 18) & 0x4)
        | ((opcode >> 21) & 0x78);
    int8_t op = arm_uncond_table_main[index];
    if ( op < 0 ) {
        decod_func_t f = arm_uncond_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_uncond_table_1001_X_00__0[2] = {
      0,  11, 
};
int8_t ArmIss::arm_uncond_func_1001_X_00__0(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1001_X_00__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0110__X11__0[2] = {
    -27,   0, 
};
int8_t ArmIss::arm_uncond_func_0110__X11__0(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0110__X11__0[index];
    if ( op < 0 ) {
        decod_func_t f = arm_uncond_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_uncond_table_1000_X_00__0[2] = {
      0,  11, 
};
int8_t ArmIss::arm_uncond_func_1000_X_00__0(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1000_X_00__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0100__X10__0[2] = {
    -27,   0, 
};
int8_t ArmIss::arm_uncond_func_0100__X10__0(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0100__X10__0[index];
    if ( op < 0 ) {
        decod_func_t f = arm_uncond_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_uncond_table_0100__X10__1[2] = {
    -27,   0, 
};
int8_t ArmIss::arm_uncond_func_0100__X10__1(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0100__X10__1[index];
    if ( op < 0 ) {
        decod_func_t f = arm_uncond_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_uncond_table_0111__X11__0[2] = {
      6,   0, 
};
int8_t ArmIss::arm_uncond_func_0111__X11__0(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0111__X11__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0001XXX00_X0[16] = {
      1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
};
int8_t ArmIss::arm_uncond_func_0001XXX00_X0(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x1)
        | ((opcode >> 20) & 0xe);
    int8_t op = arm_uncond_table_0001XXX00_X0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1001_X_11__1[2] = {
     12,   0, 
};
int8_t ArmIss::arm_uncond_func_1001_X_11__1(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1001_X_11__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0110__X10__0[2] = {
    -27,   0, 
};
int8_t ArmIss::arm_uncond_func_0110__X10__0(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0110__X10__0[index];
    if ( op < 0 ) {
        decod_func_t f = arm_uncond_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_uncond_table_1100XXX10000[8] = {
      0,  14,  17,  14,  14,  14,  14,  14, 
};
int8_t ArmIss::arm_uncond_func_1100XXX10000(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x7);
    int8_t op = arm_uncond_table_1100XXX10000[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1001_X_01__1[2] = {
      0,  11, 
};
int8_t ArmIss::arm_uncond_func_1001_X_01__1(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1001_X_01__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1000_X_10__0[2] = {
     12,   0, 
};
int8_t ArmIss::arm_uncond_func_1000_X_10__0(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1000_X_10__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1000_X_11__0[2] = {
     12,   0, 
};
int8_t ArmIss::arm_uncond_func_1000_X_11__0(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1000_X_11__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1001_X_01__0[2] = {
      0,  11, 
};
int8_t ArmIss::arm_uncond_func_1001_X_01__0(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1001_X_01__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0001XXX01_X0[16] = {
      1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
};
int8_t ArmIss::arm_uncond_func_0001XXX01_X0(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x1)
        | ((opcode >> 20) & 0xe);
    int8_t op = arm_uncond_table_0001XXX01_X0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1001_X_00__1[2] = {
      0,  11, 
};
int8_t ArmIss::arm_uncond_func_1001_X_00__1(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1001_X_00__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1001_X_10__0[2] = {
     12,   0, 
};
int8_t ArmIss::arm_uncond_func_1001_X_10__0(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1001_X_10__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1000_X_01__0[2] = {
      0,  11, 
};
int8_t ArmIss::arm_uncond_func_1000_X_01__0(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1000_X_01__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_01000X010000[2] = {
      4,   5, 
};
int8_t ArmIss::arm_uncond_func_01000X010000(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_01000X010000[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0101XXX10XX1[32] = {
      6,   6,   6,   6,   0,   0,   0,   0,   6,   6,   6,   6,   7,   0,   9,   0, 
      6,   6,   6,   6,   0,   0,   0,   0,   6,   6,   6,   6,   0,   0,   0,   0, 
};
int8_t ArmIss::arm_uncond_func_0101XXX10XX1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_uncond_table_0101XXX10XX1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1001_X_11__0[2] = {
     12,   0, 
};
int8_t ArmIss::arm_uncond_func_1001_X_11__0(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1001_X_11__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0101__X11__1[2] = {
      6,   0, 
};
int8_t ArmIss::arm_uncond_func_0101__X11__1(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0101__X11__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0101XXX10XX0[32] = {
      6,   6,   6,   6,   0,   0,   0,   0,   6,   6,   6,   6,   0,   0,   8,  10, 
      6,   6,   6,   6,   0,   0,   0,   0,   6,   6,   6,   6,   0,   0,   0,   0, 
};
int8_t ArmIss::arm_uncond_func_0101XXX10XX0(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x3)
        | ((opcode >> 19) & 0x1c);
    int8_t op = arm_uncond_table_0101XXX10XX0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0111__X10__0[2] = {
      6,   0, 
};
int8_t ArmIss::arm_uncond_func_0111__X10__0(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0111__X10__0[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0001XXX00_X1[16] = {
      1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
};
int8_t ArmIss::arm_uncond_func_0001XXX00_X1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x1)
        | ((opcode >> 20) & 0xe);
    int8_t op = arm_uncond_table_0001XXX00_X1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0100__X11__0[2] = {
    -27,   0, 
};
int8_t ArmIss::arm_uncond_func_0100__X11__0(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0100__X11__0[index];
    if ( op < 0 ) {
        decod_func_t f = arm_uncond_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_uncond_table_1100XXX00000[8] = {
      0,  15,  16,  15,  15,  15,  15,  15, 
};
int8_t ArmIss::arm_uncond_func_1100XXX00000(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x7);
    int8_t op = arm_uncond_table_1100XXX00000[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1000_X_10__1[2] = {
     12,   0, 
};
int8_t ArmIss::arm_uncond_func_1000_X_10__1(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1000_X_10__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0100__X11__1[2] = {
    -27,   0, 
};
int8_t ArmIss::arm_uncond_func_0100__X11__1(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0100__X11__1[index];
    if ( op < 0 ) {
        decod_func_t f = arm_uncond_func_funcs[~(ssize_t)op];
        return (*f)(opcode);
    }
    return op;
}
int8_t const ArmIss::arm_uncond_table_1000_X_01__1[2] = {
      0,  11, 
};
int8_t ArmIss::arm_uncond_func_1000_X_01__1(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1000_X_01__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1000_X_11__1[2] = {
     12,   0, 
};
int8_t ArmIss::arm_uncond_func_1000_X_11__1(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1000_X_11__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1001_X_10__1[2] = {
     12,   0, 
};
int8_t ArmIss::arm_uncond_func_1001_X_10__1(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1001_X_10__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0001XXX01_X1[16] = {
      1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
};
int8_t ArmIss::arm_uncond_func_0001XXX01_X1(data_t opcode)
{
    size_t index = ((opcode >> 5) & 0x1)
        | ((opcode >> 20) & 0xe);
    int8_t op = arm_uncond_table_0001XXX01_X1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_1000_X_00__1[2] = {
      0,  11, 
};
int8_t ArmIss::arm_uncond_func_1000_X_00__1(data_t opcode)
{
    size_t index = ((opcode >> 22) & 0x1);
    int8_t op = arm_uncond_table_1000_X_00__1[index];
    return op;
}
int8_t const ArmIss::arm_uncond_table_0101__X11__0[2] = {
      6,   0, 
};
int8_t ArmIss::arm_uncond_func_0101__X11__0(data_t opcode)
{
    size_t index = ((opcode >> 21) & 0x1);
    int8_t op = arm_uncond_table_0101__X11__0[index];
    return op;
}
ArmIss::decod_func_t const ArmIss::arm_uncond_func_funcs[35] = {
    _(arm_uncond_func_1000_X_01__0), _(arm_uncond_func_1000_X_11__0), 
    _(arm_uncond_func_0111__X11__0), _(arm_uncond_func_1001_X_11__0), 
    _(arm_uncond_func_0110__X10__0), _(arm_uncond_func_0001XXX00_X1), 
    _(arm_uncond_func_0101XXX10XX1), _(arm_uncond_func_0101XXX10XX0), 
    _(arm_uncond_func_0001XXX01_X0), _(arm_uncond_func_1001_X_00__1), 
    _(arm_uncond_func_0100__X11__1), _(arm_uncond_func_0001XXX01_X1), 
    _(arm_uncond_func_0101__X11__1), _(arm_uncond_func_1000_X_00__1), 
    _(arm_uncond_func_1001_X_01__1), _(arm_uncond_func_0100__X11__0), 
    _(arm_uncond_func_0001XXX00_X0), _(arm_uncond_func_0111__X10__0), 
    _(arm_uncond_func_1001_X_10__1), _(arm_uncond_func_1100XXX00000), 
    _(arm_uncond_func_1000_X_01__1), _(arm_uncond_func_1001_X_00__0), 
    _(arm_uncond_func_1100XXX10000), _(arm_uncond_func_0100__X10__1), 
    _(arm_uncond_func_1000_X_10__1), _(arm_uncond_func_1001_X_11__1), 
    _(arm_uncond_func_01000X010000), _(arm_uncond_func_1000_X_10__0), 
    _(arm_uncond_func_0101__X11__0), _(arm_uncond_func_1000_X_00__0), 
    _(arm_uncond_func_1001_X_01__0), _(arm_uncond_func_0100__X10__0), 
    _(arm_uncond_func_1000_X_11__1), _(arm_uncond_func_1001_X_10__0), 
    _(arm_uncond_func_0110__X11__0), 
};
ArmIss::func_t const ArmIss::arm_uncond_funcs[18] = {
    op(ill), op(cps_setend), op(avsimd_data), op(avsimd_ldst), op(nop), op(pli), 
    op(pld), op(clrex), op(dsb), op(dmb), op(isb), op(srs), op(rfe), op(blx), 
    op(ldci), op(stci), op(mcrr), op(mrrc), 
};
const char * const ArmIss::arm_uncond_func_names[18] = {
    "", "cps_setend", "avsimd_data", "avsimd_ldst", "nop", "pli", "pld", "clrex", 
    "dsb", "dmb", "isb", "srs", "rfe", "blx", "ldci", "stci", "mcrr", "mrrc", 
};


}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
