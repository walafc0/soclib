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

static const char * const arm_uncond_func_names[18];
static decod_func_t const arm_uncond_func_funcs[35];
static func_t const arm_uncond_funcs[18];
static int8_t arm_uncond_func_0001XXX00_X0(data_t opcode);
static int8_t arm_uncond_func_0001XXX00_X1(data_t opcode);
static int8_t arm_uncond_func_0001XXX01_X0(data_t opcode);
static int8_t arm_uncond_func_0001XXX01_X1(data_t opcode);
static int8_t arm_uncond_func_01000X010000(data_t opcode);
static int8_t arm_uncond_func_0100__X10__0(data_t opcode);
static int8_t arm_uncond_func_0100__X10__1(data_t opcode);
static int8_t arm_uncond_func_0100__X11__0(data_t opcode);
static int8_t arm_uncond_func_0100__X11__1(data_t opcode);
static int8_t arm_uncond_func_0101XXX10XX0(data_t opcode);
static int8_t arm_uncond_func_0101XXX10XX1(data_t opcode);
static int8_t arm_uncond_func_0101__X11__0(data_t opcode);
static int8_t arm_uncond_func_0101__X11__1(data_t opcode);
static int8_t arm_uncond_func_0110__X10__0(data_t opcode);
static int8_t arm_uncond_func_0110__X11__0(data_t opcode);
static int8_t arm_uncond_func_0111__X10__0(data_t opcode);
static int8_t arm_uncond_func_0111__X11__0(data_t opcode);
static int8_t arm_uncond_func_1000_X_00__0(data_t opcode);
static int8_t arm_uncond_func_1000_X_00__1(data_t opcode);
static int8_t arm_uncond_func_1000_X_01__0(data_t opcode);
static int8_t arm_uncond_func_1000_X_01__1(data_t opcode);
static int8_t arm_uncond_func_1000_X_10__0(data_t opcode);
static int8_t arm_uncond_func_1000_X_10__1(data_t opcode);
static int8_t arm_uncond_func_1000_X_11__0(data_t opcode);
static int8_t arm_uncond_func_1000_X_11__1(data_t opcode);
static int8_t arm_uncond_func_1001_X_00__0(data_t opcode);
static int8_t arm_uncond_func_1001_X_00__1(data_t opcode);
static int8_t arm_uncond_func_1001_X_01__0(data_t opcode);
static int8_t arm_uncond_func_1001_X_01__1(data_t opcode);
static int8_t arm_uncond_func_1001_X_10__0(data_t opcode);
static int8_t arm_uncond_func_1001_X_10__1(data_t opcode);
static int8_t arm_uncond_func_1001_X_11__0(data_t opcode);
static int8_t arm_uncond_func_1001_X_11__1(data_t opcode);
static int8_t arm_uncond_func_1100XXX00000(data_t opcode);
static int8_t arm_uncond_func_1100XXX10000(data_t opcode);
static int8_t arm_uncond_func_main(data_t opcode);
static int8_t const arm_uncond_table_0001XXX00_X0[16];
static int8_t const arm_uncond_table_0001XXX00_X1[16];
static int8_t const arm_uncond_table_0001XXX01_X0[16];
static int8_t const arm_uncond_table_0001XXX01_X1[16];
static int8_t const arm_uncond_table_01000X010000[2];
static int8_t const arm_uncond_table_0100__X10__0[2];
static int8_t const arm_uncond_table_0100__X10__1[2];
static int8_t const arm_uncond_table_0100__X11__0[2];
static int8_t const arm_uncond_table_0100__X11__1[2];
static int8_t const arm_uncond_table_0101XXX10XX0[32];
static int8_t const arm_uncond_table_0101XXX10XX1[32];
static int8_t const arm_uncond_table_0101__X11__0[2];
static int8_t const arm_uncond_table_0101__X11__1[2];
static int8_t const arm_uncond_table_0110__X10__0[2];
static int8_t const arm_uncond_table_0110__X11__0[2];
static int8_t const arm_uncond_table_0111__X10__0[2];
static int8_t const arm_uncond_table_0111__X11__0[2];
static int8_t const arm_uncond_table_1000_X_00__0[2];
static int8_t const arm_uncond_table_1000_X_00__1[2];
static int8_t const arm_uncond_table_1000_X_01__0[2];
static int8_t const arm_uncond_table_1000_X_01__1[2];
static int8_t const arm_uncond_table_1000_X_10__0[2];
static int8_t const arm_uncond_table_1000_X_10__1[2];
static int8_t const arm_uncond_table_1000_X_11__0[2];
static int8_t const arm_uncond_table_1000_X_11__1[2];
static int8_t const arm_uncond_table_1001_X_00__0[2];
static int8_t const arm_uncond_table_1001_X_00__1[2];
static int8_t const arm_uncond_table_1001_X_01__0[2];
static int8_t const arm_uncond_table_1001_X_01__1[2];
static int8_t const arm_uncond_table_1001_X_10__0[2];
static int8_t const arm_uncond_table_1001_X_10__1[2];
static int8_t const arm_uncond_table_1001_X_11__0[2];
static int8_t const arm_uncond_table_1001_X_11__1[2];
static int8_t const arm_uncond_table_1100XXX00000[8];
static int8_t const arm_uncond_table_1100XXX10000[8];
static int8_t const arm_uncond_table_main[128];
void arm_uncond_ill();
void arm_uncond_cps_setend();
void arm_uncond_avsimd_data();
void arm_uncond_avsimd_ldst();
void arm_uncond_nop();
void arm_uncond_pli();
void arm_uncond_pld();
void arm_uncond_clrex();
void arm_uncond_dsb();
void arm_uncond_dmb();
void arm_uncond_isb();
void arm_uncond_srs();
void arm_uncond_rfe();
void arm_uncond_blx();
void arm_uncond_ldci();
void arm_uncond_stci();
void arm_uncond_mcrr();
void arm_uncond_mrrc();


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
