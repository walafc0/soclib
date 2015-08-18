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
 * $Id$
 *
 */

static const char * const arm_func_names[87];
static decod_func_t const arm_func_funcs[18];
static func_t const arm_funcs[87];
static int8_t arm_func_0000XXX00001(data_t opcode);
static int8_t arm_func_0000XXX00XX0(data_t opcode);
static int8_t arm_func_0000XXX01XX1(data_t opcode);
static int8_t arm_func_0000XXX10000(data_t opcode);
static int8_t arm_func_0000XXX11XX1(data_t opcode);
static int8_t arm_func_0001XXX00XX0(data_t opcode);
static int8_t arm_func_0001XXX00XX1(data_t opcode);
static int8_t arm_func_0001XXX010X0(data_t opcode);
static int8_t arm_func_0001XXX01XX1(data_t opcode);
static int8_t arm_func_0001XXX10000(data_t opcode);
static int8_t arm_func_0001XXX11XX1(data_t opcode);
static int8_t arm_func_0011XXX00000(data_t opcode);
static int8_t arm_func_01101X110X11(data_t opcode);
static int8_t arm_func_01101X111011(data_t opcode);
static int8_t arm_func_01101XX00111(data_t opcode);
static int8_t arm_func_0110X_X10_X1(data_t opcode);
static int8_t arm_func_0110X_X11XX1(data_t opcode);
static int8_t arm_func_0110X__00XX1(data_t opcode);
static int8_t arm_func_main(data_t opcode);
static int8_t const arm_table_0000XXX00001[8];
static int8_t const arm_table_0000XXX00XX0[32];
static int8_t const arm_table_0000XXX01XX1[32];
static int8_t const arm_table_0000XXX10000[8];
static int8_t const arm_table_0000XXX11XX1[32];
static int8_t const arm_table_0001XXX00XX0[32];
static int8_t const arm_table_0001XXX00XX1[32];
static int8_t const arm_table_0001XXX010X0[16];
static int8_t const arm_table_0001XXX01XX1[32];
static int8_t const arm_table_0001XXX10000[8];
static int8_t const arm_table_0001XXX11XX1[32];
static int8_t const arm_table_0011XXX00000[8];
static int8_t const arm_table_01101X110X11[4];
static int8_t const arm_table_01101X111011[2];
static int8_t const arm_table_01101XX00111[4];
static int8_t const arm_table_0110X_X10_X1[8];
static int8_t const arm_table_0110X_X11XX1[16];
static int8_t const arm_table_0110X__00XX1[8];
static int8_t const arm_table_main[128];
void arm_ill();
void arm_swp();
void arm_adc();
void arm_adcs();
void arm_add();
void arm_adds();
void arm_and();
void arm_ands();
void arm_bic();
void arm_bics();
void arm_eor();
void arm_eors();
void arm_mov();
void arm_movs();
void arm_mvn();
void arm_mvns();
void arm_orr();
void arm_orrs();
void arm_rsb();
void arm_rsbs();
void arm_rsc();
void arm_rscs();
void arm_sbc();
void arm_sbcs();
void arm_sub();
void arm_subs();
void arm_cmns();
void arm_cmps();
void arm_tsts();
void arm_teqs();
void arm_b();
void arm_bl();
void arm_cdp();
void arm_ldc();
void arm_ldstm();
void arm_mcr();
void arm_mla();
void arm_mrc();
void arm_mrs();
void arm_msr();
void arm_mul();
void arm_smlal();
void arm_smull();
void arm_stc();
void arm_swi();
void arm_umull();
void arm_umlal();
void arm_bx();
void arm_blx();
void arm_bkpt();
void arm_clz();
void arm_smla_xy();
void arm_smlaw_y();
void arm_smulw_y();
void arm_smul_xy();
void arm_strex();
void arm_ldrex();
void arm_rev();
void arm_rev16();
void arm_revsh();
void arm_sxtb16();
void arm_uxtb16();
void arm_sxtb();
void arm_uxtb();
void arm_sxth();
void arm_uxth();
void arm_umaal();


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
