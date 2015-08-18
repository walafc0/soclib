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

static const char * const thumb_func_names[64];
static decod_func_t const thumb_func_funcs[4];
static func_t const thumb_funcs[64];
static int8_t const thumb_table_00011XX000[4];
static int8_t const thumb_table_01000XXXXX[32];
static int8_t const thumb_table_10110XXXXX[32];
static int8_t const thumb_table_10111XXXXX[32];
static int8_t const thumb_table_main[32];
static int8_t thumb_func_00011XX000(data_t opcode);
static int8_t thumb_func_01000XXXXX(data_t opcode);
static int8_t thumb_func_10110XXXXX(data_t opcode);
static int8_t thumb_func_10111XXXXX(data_t opcode);
static int8_t thumb_func_main(data_t opcode);
void thumb_ill();
void thumb_adc();
void thumb_and();
void thumb_bic();
void thumb_tst();
void thumb_asr();
void thumb_cmn();
void thumb_cmp();
void thumb_cmp_hi();
void thumb_cpy();
void thumb_eor();
void thumb_lsl();
void thumb_lsr();
void thumb_add_hi();
void thumb_mul();
void thumb_mvn();
void thumb_neg();
void thumb_orr();
void thumb_ror();
void thumb_sbc();
void thumb_lsl_imm5();
void thumb_lsr_imm5();
void thumb_mov_imm8();
void thumb_asr_imm5();
void thumb_add_imm3();
void thumb_sub_imm3();
void thumb_add_reg();
void thumb_sub_reg();
void thumb_sp_add();
void thumb_sp_sub();
void thumb_add_imm8();
void thumb_sub_imm8();
void thumb_add2pc();
void thumb_add2sp();
void thumb_cps_setend();
void thumb_bkpt();
void thumb_bcond_swi();
void thumb_b();
void thumb_b_hi();
void thumb_bl();
void thumb_blx();
void thumb_cmp_imm();
void thumb_pop();
void thumb_push();
void thumb_rev();
void thumb_rev16();
void thumb_revsh();
void thumb_ldst();
void thumb_ldr_pcrel();
void thumb_ldsth_imm5();


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
