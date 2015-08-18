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
 * TMS320C6X Instruction Set Simulator for the TMS320C6X processor core
 * developed for the SocLib Projet
 * 
 * Copyright (C) IRISA/INRIA, 2008
 *         Francois Charot <charot@irisa.fr>
 *
 * 
 * Maintainer: charot
 *
 * Functional description:
 * The following files: 
 * 				tms320c62.h
 *              tms320c62.cpp
 *              tms320c62_instructions.cpp
 *              tms320c62_decoding.cpp
 *
 * define the Instruction Set Simulator for the TMS320C62 processor.
 *
 * 
 */

#include "tms320c62.h"

#define SIGN_EXTEND(src,width)   (((src) << (32-width)) >> (32-width))
#define SIGN_EXTEND_LONG(src,width) (((src) << (64-width)) >> (64-width))

#ifndef INT64_MAX
#define INT64_MAX ((0xffffffff < 7) | 0x7f)  // ways to enforce 40-bit long constants
#endif
#ifndef INT64_MIN
#define INT64_MIN (-(0x80000000 << 8))
#endif

namespace soclib {
namespace common {

uint32_t clearbit[32] = { 0xfffffffe, 0xfffffffd, 0xfffffffb, 0xfffffff7,
		0xffffffef, 0xffffffdf, 0xffffffbf, 0xffffff7f, 0xfffffeff, 0xfffffdff,
		0xfffffbff, 0xfffff7ff, 0xffffefff, 0xffffdfff, 0xffffbfff, 0xffff7fff,
		0xfffeffff, 0xfffdffff, 0xfffbffff, 0xfff7ffff, 0xffefffff, 0xffdfffff,
		0xffbfffff, 0xff7fffff, 0xfeffffff, 0xfdffffff, 0xfbffffff, 0xf7ffffff,
		0xefffffff, 0xdfffffff, 0xbfffffff, 0x7fffffff };

uint32_t power2[32] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008,
		0x00000010, 0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200,
		0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
		0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000,
		0x00400000, 0x00800000, 0x01000000, 0x02000000, 0x04000000, 0x08000000,
		0x10000000, 0x20000000, 0x40000000, 0x80000000 };

#define op(x) & Tms320C6xIss::op_##x
#define op4(x, y, z, t) op(x), op(y), op(z), op(t)
#define op2(x, y) op(x), op(y)

Tms320C6xIss::func_t const Tms320C6xIss::l_function_e1[] = { op4(illegal_l_e1, illegal_l_e1, add_l_02_e1, add_l_03_e1), 
      op4(illegal_l_e1, illegal_l_e1, sub_l_06_e1, sub_l_07_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1), 
      op4(illegal_l_e1, illegal_l_e1, ssub_l_0e_e1, ssub_l_0f_e1), 
      op4(illegal_l_e1, illegal_l_e1, sadd_l_12_e1, sadd_l_13_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, sub_l_17_e1), 
      op4(illegal_l_e1, illegal_l_e1, abs_l_1a_e1, illegal_l_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, ssub_l_1f_e1), 
      op4(add_l_20_e1,  add_l_21_e1,  illegal_l_e1, add_l_23_e1),

      op4(sub_l_24_e1,  illegal_l_e1, illegal_l_e1, sub_l_27_e1)		, 
      op4(illegal_l_e1, addu_l_29_e1, illegal_l_e1, addu_l_2b_e1), 
      op4(ssub_l_2c_e1, illegal_l_e1, illegal_l_e1, subu_l_2f_e1), 
      op4(sadd_l_30_e1, sadd_l_31_e1, illegal_l_e1, illegal_l_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, sub_l_37_e1), 
      op4(abs_l_38_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, subu_l_3f_e1), 
      op4(sat_l_40_e1,  illegal_l_e1, illegal_l_e1, illegal_l_e1), 
      op4(cmpgt_l_44_e1,cmpgt_l_45_e1, cmpgt_l_46_e1, cmpgt_l_47_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, subc_l_4b_e1), 
      op4(cmpgtu_l_4c_e1, cmpgtu_l_4d_e1, cmpgtu_l_4e_e1, cmpgtu_l_4f_e1), 
      op4(cmpeq_l_50_e1, cmpeq_l_51_e1, cmpeq_l_52_e1, cmpeq_l_53_e1), 
      op4(cmplt_l_54_e1, cmplt_l_55_e1, cmplt_l_56_e1, cmplt_l_57_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1), 
      op4(cmpltu_l_5c_e1, cmpltu_l_5d_e1, cmpltu_l_5e_e1, cmpltu_l_5f_e1), 
      op4(norm_l_60_e1, illegal_l_e1, illegal_l_e1, norm_l_63_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1), 
      op4(illegal_l_e1, illegal_l_e1, lmbd_l_6a_e1, lmbd_l_6b_e1), 
      op4(illegal_l_e1, illegal_l_e1, xor_l_6e_e1, xor_l_6f_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1), 
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1), 
      op4(illegal_l_e1, illegal_l_e1, and_l_7a_e1, and_l_7b_e1), 
      op4(illegal_l_e1, illegal_l_e1, or_l_7e_e1, or_l_7f_e1), };

		Tms320C6xIss::func_t const Tms320C6xIss::s_function_e1[] = {
			op4(illegal_s_e1, add2_s_01_e1, illegal_s_e1, b_s_03_e1),
			op4(illegal_s_e1, illegal_s_e1, add_s_06_e1, add_s_07_e1),
			op4(illegal_s_e1, illegal_s_e1, xor_s_0a_e1, xor_s_0b_e1),
			op4(illegal_s_e1, b_s_0d_e1, mvc_s_0e_e1, mvu_s_0f_e1),
			op4(illegal_s_e1, sub2_s_11_e1, shl_s_12_e1, shl_s_13_e1),
			op4(illegal_s_e1, illegal_s_e1, sub_s_16_e1, sub_s_17_e1),
			op4(illegal_s_e1, illegal_s_e1, or_s_1a_e1, or_s_1b_e1),
			op4(illegal_s_e1, illegal_s_e1, and_s_1e_e1, and_s_1f_e1),
			op4(illegal_s_e1, illegal_s_e1, sshl_s_22_e1, sshl_s_23_e1),
			op4(shru_s_24_e1, shru_s_25_e1, shru_s_26_e1, shru_s_27_e1),
			op4(illegal_s_e1, illegal_s_e1, illegal_s_e1, extu_s_2b_e1),
			op4(illegal_s_e1, illegal_s_e1, illegal_s_e1, ext_s_2f_e1),
			op4(shl_s_30_e1, shl_s_31_e1, shl_s_32_e1, shl_s_33_e1),
			op4(shr_s_34_e1, shr_s_35_e1, shr_s_36_e1, shr_s_37_e1),
			op4(illegal_s_e1, illegal_s_e1, illegal_s_e1, set_s_3b_e1),
			op4(illegal_s_e1, illegal_s_e1, illegal_s_e1, clr_s_3f_e1),};

		Tms320C6xIss::func_t const Tms320C6xIss::s_immed_function_e1[] = {
			op4(extu_s_immed_00_e1, ext_s_immed_01_e1, set_s_immed_02_e1,clr_s_immed_03_e1), };

    Tms320C6xIss::func_t const Tms320C6xIss::s_mvk_function_e1[] = {
      op2(mvk_s_mvk_00_e1, mvkh_s_mvk_01_e1), };

    Tms320C6xIss::func_t const Tms320C6xIss::s_addk_function_e1[] = {
      op(addk_s_addk_e1) };

    Tms320C6xIss::func_t const Tms320C6xIss::s_bcond_function_e1[] = {
      op(bcond_s_bcond_e1) };

    Tms320C6xIss::func_t const Tms320C6xIss::d_function_e1[] = {
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(add_d_10_e1, sub_d_11_e1, add_d_12_e1, sub_d_13_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(addab_d_30_e1, subab_d_31_e1, addab_d_32_e1, subab_d_33_e1),
      op4(addah_d_34_e1, subah_d_35_e1, addah_d_36_e1, subah_d_37_e1),
      op4(addaw_d_38_e1, subaw_d_39_e1, addaw_d_3a_e1, subaw_d_3b_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1), };

    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstOffset_function_e1[] = {
      op4(ldhu_d_ldstOffset_00_e1, ldbu_d_ldstOffset_01_e1, ldb_d_ldstOffset_02_e1, stb_d_ldstOffset_03_e1),
      op4(ldh_d_ldstOffset_04_e1, sth_d_ldstOffset_05_e1, ldw_d_ldstOffset_06_e1, stw_d_ldstOffset_07_e1), };

    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstBaseROffset_function_e1[] = {
      op4(ldhu_d_ldstBaseROffset_00_e1, ldbu_d_ldstBaseROffset_01_e1, ldb_d_ldstBaseROffset_02_e1, stb_d_ldstBaseROffset_03_e1),
      op4(ldh_d_ldstBaseROffset_04_e1, sth_d_ldstBaseROffset_05_e1, ldw_d_ldstBaseROffset_06_e1, stw_d_ldstBaseROffset_07_e1), };
 
    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstOffset_function_e2[] = {
      op4(ldhu_d_ldstOffset_00_e2, ldbu_d_ldstOffset_01_e2, ldb_d_ldstOffset_02_e2, stb_d_ldstOffset_03_e2),
      op4(ldh_d_ldstOffset_04_e2, sth_d_ldstOffset_05_e2, ldw_d_ldstOffset_06_e2, stw_d_ldstOffset_07_e2), };

    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstBaseROffset_function_e2[] = {
      op4(ldhu_d_ldstBaseROffset_00_e2, ldbu_d_ldstBaseROffset_01_e2, ldb_d_ldstBaseROffset_02_e2, stb_d_ldstBaseROffset_03_e2),
      op4(ldh_d_ldstBaseROffset_04_e2, sth_d_ldstBaseROffset_05_e2, ldw_d_ldstBaseROffset_06_e2, stw_d_ldstBaseROffset_07_e2), };

    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstOffset_function_e3[] = {
      op4(ldhu_d_ldstOffset_00_e3, ldbu_d_ldstOffset_01_e3, ldb_d_ldstOffset_02_e3, stb_d_ldstOffset_03_e3),
      op4(ldh_d_ldstOffset_04_e3, sth_d_ldstOffset_05_e3, ldw_d_ldstOffset_06_e3, stw_d_ldstOffset_07_e3), };

    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstBaseROffset_function_e3[] = {
      op4(ldhu_d_ldstBaseROffset_00_e3, ldbu_d_ldstBaseROffset_01_e3, ldb_d_ldstBaseROffset_02_e3, stb_d_ldstBaseROffset_03_e3),
      op4(ldh_d_ldstBaseROffset_04_e3, sth_d_ldstBaseROffset_05_e3, ldw_d_ldstBaseROffset_06_e3, stw_d_ldstBaseROffset_07_e3), };

    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstOffset_function_e4[] = {
      op4(ldhu_d_ldstOffset_00_e4, ldbu_d_ldstOffset_01_e4, ldb_d_ldstOffset_02_e4, stb_d_ldstOffset_03_e4),
      op4(ldh_d_ldstOffset_04_e4, sth_d_ldstOffset_05_e4, ldw_d_ldstOffset_06_e4, stw_d_ldstOffset_07_e4), };

    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstBaseROffset_function_e4[] = {
      op4(ldhu_d_ldstBaseROffset_00_e4, ldbu_d_ldstBaseROffset_01_e4, ldb_d_ldstBaseROffset_02_e4, stb_d_ldstBaseROffset_03_e4),
      op4(ldh_d_ldstBaseROffset_04_e4, sth_d_ldstBaseROffset_05_e4, ldw_d_ldstBaseROffset_06_e4, stw_d_ldstBaseROffset_07_e4), };

     Tms320C6xIss::func_t const Tms320C6xIss::d_ldstOffset_function_e5[] = {
      op4(ldhu_d_ldstOffset_00_e5, ldbu_d_ldstOffset_01_e5, ldb_d_ldstOffset_02_e5, nothing_to_be_done),
      op4(ldh_d_ldstOffset_04_e5, nothing_to_be_done, ldw_d_ldstOffset_06_e5, nothing_to_be_done), };

    Tms320C6xIss::func_t const Tms320C6xIss::d_ldstBaseROffset_function_e5[] = {
      op4(ldhu_d_ldstBaseROffset_00_e5, ldbu_d_ldstBaseROffset_01_e5, ldb_d_ldstBaseROffset_02_e5, nothing_to_be_done),
      op4(ldh_d_ldstBaseROffset_04_e5, nothing_to_be_done, ldw_d_ldstBaseROffset_06_e5, nothing_to_be_done), };

    Tms320C6xIss::func_t const Tms320C6xIss::m_function_e1[] = {
      op4(illegal_m_e1, mpyh_m_01_e1, smpyh_m_02_e1, mpyhsu_m_03_e1),
      op4(illegal_m_e1, mpyhus_m_05_e1, illegal_m_e1, mpyhu_m_07_e1),
      op4(illegal_m_e1, mpyhl_m_09_e1, smpyhl_m_0a_e1, mpyhslu_m_0b_e1),
      op4(illegal_m_e1, mpyhuls_m_0d_e1, illegal_m_e1, mpyhlu_m_0f_e1),
      op4(illegal_m_e1, mpylh_m_11_e1, smpylh_m_12_e1, mpylshu_m_13_e1),
      op4(illegal_m_e1, mpyluhs_m_15_e1, illegal_m_e1, mpylhu_m_17_e1),
      op4(mpy_m_18_e1, mpy_m_19_e1, smpy_m_1a_e1, mpysu_m_1b_e1),
      op4(illegal_m_e1, mpyus_m_1d_e1, mpysu_m_1e_e1, mpyu_m_1f_e1), };

   Tms320C6xIss::func_t const Tms320C6xIss::m_function_e2[] = {
      op4(illegal_m_e2, mpyh_m_01_e2, smpyh_m_02_e2, mpyhsu_m_03_e2),
      op4(illegal_m_e2, mpyhus_m_05_e2, illegal_m_e2, mpyhu_m_07_e2),
      op4(illegal_m_e2, mpyhl_m_09_e2, smpyhl_m_0a_e2, mpyhslu_m_0b_e2),
      op4(illegal_m_e2, mpyhuls_m_0d_e2, illegal_m_e2, mpyhlu_m_0f_e2),
      op4(illegal_m_e2, mpylh_m_11_e2, smpylh_m_12_e2, mpylshu_m_13_e2),
      op4(illegal_m_e2, mpyluhs_m_15_e2, illegal_m_e2, mpylhu_m_17_e2),
      op4(mpy_m_18_e2,  mpy_m_19_e2, smpy_m_1a_e2, mpysu_m_1b_e2),
      op4(illegal_m_e2, mpyus_m_1d_e2, mpysu_m_1e_e2, mpyu_m_1f_e2), };

#undef op
#define op(x) #x

    const char *Tms320C6xIss::name_l_function_e1[] = {
      op4(illegal_l_e1, illegal_l_e1, add_l_02_e1, add_l_03_e1),
      op4(illegal_l_e1, illegal_l_e1, sub_l_06_e1, sub_l_07_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1),
      op4(illegal_l_e1, illegal_l_e1, ssub_l_0e_e1, ssub_l_0f_e1),
      op4(illegal_l_e1, illegal_l_e1, sadd_l_12_e1, sadd_l_13_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, sub_l_17_e1),
      op4(illegal_l_e1, illegal_l_e1, abs_l_1a_e1, illegal_l_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, ssub_l_1f_e1),
      op4(add_l_20_e1,  add_l_21_e1,  illegal_l_e1, add_l_23_e1),
      op4(sub_l_24_e1,  illegal_l_e1, illegal_l_e1, sub_l_27_e1),
      op4(illegal_l_e1, addu_l_29_e1, illegal_l_e1, addu_l_2b_e1),
      op4(ssub_l_2c_e1, illegal_l_e1, illegal_l_e1, subu_l_2f_e1),
      op4(sadd_l_30_e1, sadd_l_31_e1, illegal_l_e1, illegal_l_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, sub_l_37_e1),
      op4(abs_l_38_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, subu_l_3f_e1),
      op4(sat_l_40_e1,  illegal_l_e1, illegal_l_e1, illegal_l_e1),
      op4(cmpgt_l_44_e1,cmpgt_l_45_e1, cmpgt_l_46_e1, cmpgt_l_47_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, subc_l_4b_e1),
      op4(cmpgtu_l_4c_e1, cmpgtu_l_4d_e1, cmpgtu_l_4e_e1, cmpgtu_l_4f_e1),
      op4(cmpeq_l_50_e1, cmpeq_l_51_e1, cmpeq_l_52_e1, cmpeq_l_53_e1),
      op4(cmplt_l_54_e1, cmplt_l_55_e1, cmplt_l_56_e1, cmplt_l_57_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1),
      op4(cmpltu_l_5c_e1, cmpltu_l_5d_e1, cmpltu_l_5e_e1, cmpltu_l_5f_e1),
      op4(norm_l_60_e1, illegal_l_e1, illegal_l_e1, norm_l_63_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1),
      op4(illegal_l_e1, illegal_l_e1, lmbd_l_6a_e1, lmbd_l_6b_e1),
      op4(illegal_l_e1, illegal_l_e1, xor_l_6e_e1, xor_l_6f_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1),
      op4(illegal_l_e1, illegal_l_e1, illegal_l_e1, illegal_l_e1),
      op4(illegal_l_e1, illegal_l_e1, and_l_7a_e1, and_l_7b_e1),
      op4(illegal_l_e1, illegal_l_e1, or_l_7e_e1, or_l_7f_e1), };

    const char *Tms320C6xIss::name_s_function_e1[] = {
      op4(illegal_s_e1, add2_s_01, illegal_s_e1, b_s_03_e1),
      op4(illegal_s_e1, illegal_s_e1, add_s_06_e1, add_s_07_e1),
      op4(illegal_s_e1, illegal_s_e1, xor_s_0a_e1, xor_s_0b_e1),
      op4(illegal_s_e1, b_s_0d_e1, mvc_s_0e_e1, mvu_s_0f_e1),
      op4(illegal_s_e1, sub2_s_11_e1, shl_s_1_e12_e1, shl_s_13_e1),
      op4(illegal_s_e1, illegal_s_e1, sub_s_16_e1, sub_s_17_e1),
      op4(illegal_s_e1, illegal_s_e1, or_s_1a_e1, or_s_1b_e1),
      op4(illegal_s_e1, illegal_s_e1, and_s_1e_e1, and_s_1f_e1),
      op4(illegal_s_e1, illegal_s_e1, sshl_s_22_e1, sshl_s_23_e1),
      op4(shru_s_24_e1, shru_s_25_e1, shru_s_26_e1, shru_s_27_e1),
      op4(illegal_s_e1, illegal_s_e1, illegal_s_e1, extu_s_2b_e1),
      op4(illegal_s_e1, illegal_s_e1, illegal_s_e1, ext_s_2f_e1),
      op4(shl_s_30_e1, shl_s_31_e1, shl_s_32_e1, shl_s_33_e1),
      op4(shr_s_34_e1, shr_s_35_e1, shr_s_36_e1, shr_s_37_e1),
      op4(illegal_s_e1, illegal_s_e1, illegal_s_e1, set_s_3b_e1),
      op4(illegal_s_e1, illegal_s_e1, illegal_s_e1, clr_s_3f_e1), };

    const char *Tms320C6xIss::name_s_immed_function_e1[] = {
      op4(extu_s_immed_00_e1, ext_s_immed_01_e1, set_s_immed_02_e1, clr_s_immed_03_e1), };

    const char *Tms320C6xIss::name_s_mvk_function_e1[] = {
      op2(mvk_s_mvk_00_e1, mvkh_s_mvk_01_e1), };

    const char *Tms320C6xIss::name_s_addk_function_e1[] = {
      op(addk_s_addk) };

    const char *Tms320C6xIss::name_s_bcond_function_e1[] = {
      op(bcond_s_bcond) };

    const char *Tms320C6xIss::name_d_function_e1[] = {
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(add_d_10_e1, sub_d_11_e1, add_d_12_e1, sub_d_13_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1),
      op4(addab_d_30_e1, subab_d_31_e1, addab_d_32_e1, subab_d_33_e1),
      op4(addah_d_34_e1, subah_d_35_e1, addah_d_36_e1, subah_d_37_e1),
      op4(addaw_d_38_e1, subaw_d_39_e1, addaw_d_3a_e1, subaw_d_3b_e1),
      op4(illegal_d_e1, illegal_d_e1, illegal_d_e1, illegal_d_e1), };

    const char  *Tms320C6xIss::name_d_ldstOffset_function_e1[] = {
      op4(ldhu_d_ldstOffset_00_e1, ldbu_d_ldstOffset_01_e1, ldb_d_ldstOffset_02_e1, stb_d_ldstOffset_03_e1),
      op4(ldh_d_ldstOffset_04_e1, sth_d_ldstOffset_05_e1, ldw_d_ldstOffset_06_e1, stw_d_ldstOffset_07_e1), };

    const char *Tms320C6xIss::name_d_ldstBaseROffset_function_e1[] = {
      op4(ldhu_d_ldstBaseROffset_00_e1, ldbu_d_ldstBaseROffset_01_e1, ldb_d_ldstBaseROffset_02_e1, stb_d_ldstBaseROffset_03_e1),
      op4(ldh_d_ldstBaseROffset_04_e1, sth_dldstBaseROffset__05_e1, ldw_d_ldstBaseROffset_06_e1, stw_d_ldstBaseROffset_07_e1), };

    const char  *Tms320C6xIss::name_d_ldstOffset_function_e2[] = {
      op4(ldhu_d_ldstOffset_00_e2, ldbu_d_ldstOffset_01_e2, ldb_d_ldstOffset_02_e2, stb_d_ldstOffset_03_e2),
      op4(ldh_d_ldstOffset_04_e2, sth_d_ldstOffset_05_e2, ldw_d_ldstOffset_06_e2, stw_d_ldstOffset_07_e2), };

    const char *Tms320C6xIss::name_d_ldstBaseROffset_function_e2[] = {
      op4(ldhu_d_ldstBaseROffset_00_e2, ldbu_d_ldstBaseROffset_01_e2, ldb_d_ldstBaseROffset_02_e2, stb_d_ldstBaseROffset_03_e2),
      op4(ldh_d_ldstBaseROffset_04_e2, sth_dldstBaseROffset__05_e2, ldw_d_ldstBaseROffset_06_e2, stw_d_ldstBaseROffset_07_e2), };

    const char  *Tms320C6xIss::name_d_ldstOffset_function_e3[] = {
      op4(ldhu_d_ldstOffset_00_e3, ldbu_d_ldstOffset_01_e3, ldb_d_ldstOffset_02_e3, stb_d_ldstOffset_03_e3),
      op4(ldh_d_ldstOffset_04_e3, sth_d_ldstOffset_05_e3, ldw_d_ldstOffset_06_e3, stw_d_ldstOffset_07_e3), };

    const char *Tms320C6xIss::name_d_ldstBaseROffset_function_e3[] = {
      op4(ldhu_d_ldstBaseROffset_00_e3, ldbu_d_ldstBaseROffset_01_e3, ldb_d_ldstBaseROffset_02_e3, stb_d_ldstBaseROffset_03_e3),
      op4(ldh_d_ldstBaseROffset_04_e3, sth_dldstBaseROffset__05_e3, ldw_d_ldstBaseROffset_06_e3, stw_d_ldstBaseROffset_07_e3), };

    const char  *Tms320C6xIss::name_d_ldstOffset_function_e4[] = {
      op4(ldhu_d_ldstOffset_00_e4, ldbu_d_ldstOffset_01_e4, ldb_d_ldstOffset_02_e4, stb_d_ldstOffset_03_e4),
      op4(ldh_d_ldstOffset_04_e4, sth_d_ldstOffset_05_e4, ldw_d_ldstOffset_06_e4, stw_d_ldstOffset_07_e4), };

    const char *Tms320C6xIss::name_d_ldstBaseROffset_function_e4[] = {
      op4(ldhu_d_ldstBaseROffset_00_e4, ldbu_d_ldstBaseROffset_01_e4, ldb_d_ldstBaseROffset_02_e4, stb_d_ldstBaseROffset_03_e4),
      op4(ldh_d_ldstBaseROffset_04_e4, sth_dldstBaseROffset__05_e4, ldw_d_ldstBaseROffset_06_e4, stw_d_ldstBaseROffset_07_e4), };

    const char  *Tms320C6xIss::name_d_ldstOffset_function_e5[] = {
      op4(ldhu_d_ldstOffset_00_e5, ldbu_d_ldstOffset_01_e5, ldb_d_ldstOffset_02_e5, nothing_to_be_done),
      op4(ldh_d_ldstOffset_04_e5, nothing_to_be_done, ldw_d_ldstOffset_06_e5, nothing_to_be_done), };

    const char *Tms320C6xIss::name_d_ldstBaseROffset_function_e5[] = {
      op4(ldhu_d_ldstBaseROffset_00_e5, ldbu_d_ldstBaseROffset_01_e5, ldb_d_ldstBaseROffset_02_e5, nothing_to_be_done),
      op4(ldh_d_ldstBaseROffset_04_e5, nothing_to_be_done, ldw_d_ldstBaseROffset_06_e5, nothing_to_be_done), };

    const char *Tms320C6xIss::name_m_function_e1[] = {
      op4(illegal_m_e1, mpyh_m_01_e1, smpyh_m_02_e1, mpyhsu_m_03_e1),
      op4(illegal_m_e1, mpyhus_m_05_e1, illegal_m_e1, mpyhu_m_07_e1),
      op4(illegal_m_e1, mpyhl_m_09_e1, smpyhl_m_0a_e1, mpyhslu_m_0b_e1),
      op4(illegal_m_e1, mpyhuls_m_0d_e1, illegal_m_e1, mpyhlu_m_0f_e1),
      op4(illegal_m_e1, mpylh_m_11_e1, smpylh_m_12_e1, mpylshu_m_13_e1),
      op4(illegal_m_e1, mpyluhs_m_15_e1, illegal_m_e1, mpylhu_m_17_e1),
      op4(mpy_m_18_e1, mpy_m_19_e1, smpy_m_1a_e1, mpysu_m_1b_e1),
      op4(illegal_m_e1, mpyus_m_1d_e1, mpysu_m_1e_e1, mpyu_m_1f_e1), };

    const char *Tms320C6xIss::name_m_function_e2[] = {
      op4(illegal_m_e1, mpyh_m_01_e1, smpyh_m_02_e1, mpyhsu_m_03_e1),
      op4(illegal_m_e1, mpyhus_m_05_e1, illegal_m_e1, mpyhu_m_07_e1),
      op4(illegal_m_e1, mpyhl_m_09_e1, smpyhl_m_0a_e1, mpyhslu_m_0b_e1),
      op4(illegal_m_e1, mpyhuls_m_0d_e1, illegal_m_e1, mpyhlu_m_0f_e1),
      op4(illegal_m_e1, mpylh_m_11_e1, smpylh_m_12_e1, mpylshu_m_13_e1),
      op4(illegal_m_e1, mpyluhs_m_15_e1, illegal_m_e1, mpylhu_m_17_e1),
      op4(mpy_m_18_e1, mpy_m_19_e1, smpy_m_1a_e1, mpysu_m_1b_e1),
      op4(illegal_m_e1, mpyus_m_1d_e1, mpysu_m_1e_e1, mpyu_m_1f_e1), };
#undef op
#undef op4
#undef op2


    void  Tms320C6xIss::run()
    {

      
#if TMS320C62_DEBUG
      std::cout << " start runnning cycle ...." << std::endl;
      switch ((processor_state_fsm) m_run_state) {
	
      case idle_state:
	std::cout << "state: idle" << std::endl;
	break;
	
      case usual_state:
	std::cout << "state: usual" << std::endl;
	break;

      case two_datamemory_ldst_state:
	std::cout << "state: two load/store memory instructions" << std::endl;
	std::cout <<  "e5_phase_stalled:  "<< e5_phase_stalled 
		  <<  " e4_phase_stalled:  "<< e4_phase_stalled 
		  <<  " e3_phase_stalled:  "<< e3_phase_stalled << std::endl;
	break;

      case two_datamemory_ldst_bis_state:
	std::cout << "state: two load/store memory instructions (bis)" << std::endl;
	std::cout <<  "e5_phase_stalled:  "<< e5_phase_stalled 
		  <<  " e4_phase_stalled:  "<< e4_phase_stalled 
		  <<  " e3_phase_stalled:  "<< e3_phase_stalled << std::endl;
	break;

      case flush_pipeline_state:
	std::cout << "state: flush_pipeline_state" << std::endl;
	std::cout <<  "e5_phase_stalled:  "<< e5_phase_stalled 
		  <<  " e4_phase_stalled:  "<< e4_phase_stalled << std::endl;
	break;

      }
#endif

      switch ((processor_state_fsm) m_run_state) {

      case idle_state:
	break;

      case usual_state:
	break;

      case two_datamemory_ldst_state:

	// instruction with 2 load/store encountered in e1_pipeline_phase
	// we switch to the two_datamemory_ldst_bis_state state  in order to add an
	// extra cycle in the pipeline - this extra cycle is used to able the two load/store to be performed
	if (multiple_loadstore_instructions) {
	  m_run_state = two_datamemory_ldst_bis_state;
	  multiple_loadstore_instructions = false;
	  cycle_to_flush_pipeline = 5;

#if TMS320C62_DEBUG
	  std::cout << " register update is postponed " << std::endl;
#endif
	  registerUpdatePostponed = true;
	}

#if TMS320C62_DEBUG
	std::cout << "cycle_to_flush_pipeline:" << cycle_to_flush_pipeline << std::endl;	  
#endif
	
	break;

      case two_datamemory_ldst_bis_state:

	cycle_to_flush_pipeline = cycle_to_flush_pipeline - 1;

	if (end_of_loop) {
	  m_run_state = flush_pipeline_state;
	  end_of_loop = false;
	} 
	else 
	  m_run_state = two_datamemory_ldst_state;

#if TMS320C62_DEBUG
	if (tworeadinst_in_packet)
	  std::cout << " two read instruction in packet " << std::endl;
#endif
 	if (cycle_to_flush_pipeline == 4 && tworeadinst_in_packet)  {
	  registerUpdatePostponed = true;
#if TMS320C62_DEBUG
	  std::cout << " register update is postponed " << std::endl;
#endif
	}
#if TMS320C62_DEBUG
	std::cout << "cycle_to_flush_pipeline:" << cycle_to_flush_pipeline << std::endl;	  
#endif
	if (cycle_to_flush_pipeline == 0)
	  registerUpdatePostponed = false;
	
	break;

      case flush_pipeline_state:

	cycle_to_flush_pipeline = cycle_to_flush_pipeline - 1;

#if TMS320C62_DEBUG
	std::cout << "cycle_to_flush_pipeline:" << cycle_to_flush_pipeline << std::endl;	  
#endif
	if (cycle_to_flush_pipeline == 4) {
	  e5_phase_stalled = true; 
	  e4_phase_stalled = false;
	  e3_phase_stalled = false;
	}
	else if (cycle_to_flush_pipeline == 3) {
	  e5_phase_stalled = false;
	  e4_phase_stalled = true;
	  e3_phase_stalled = false;
	} 
	else if (cycle_to_flush_pipeline == 2) {
	  m_run_state = two_datamemory_ldst_state;
	  registerUpdatePostponed = false;
	  e5_phase_stalled = false;
	  e4_phase_stalled = false;
	  e3_phase_stalled = false;
	}
#if TMS320C62_DEBUG
	std::cout << "cycle_to_flush_pipeline:" << cycle_to_flush_pipeline << std::endl;	  
#endif
	if (cycle_to_flush_pipeline == 0)
	  registerUpdatePostponed = false;
	
	break;
      }
	
 
#if TMS320C62_DEBUG
      switch ((processor_state_fsm) m_run_state) {
	
      case idle_state:
	std::cout << "state: idle" << std::endl;
	break;
	
      case usual_state:
	std::cout << "state: usual" << std::endl;
	break;

      case two_datamemory_ldst_state:
	std::cout << "state: two load/store memory instructions" << std::endl;
	std::cout <<  "e5_phase_stalled:  "<< e5_phase_stalled 
		  <<  " e4_phase_stalled:  "<< e4_phase_stalled 
		  <<  " e3_phase_stalled:  "<< e3_phase_stalled << std::endl;
	break;

      case two_datamemory_ldst_bis_state:
	std::cout << "state: two load/store memory instructions (bis)" << std::endl;
	std::cout <<  "e5_phase_stalled:  "<< e5_phase_stalled 
		  <<  " e4_phase_stalled:  "<< e4_phase_stalled 
		  <<  " e3_phase_stalled:  "<< e3_phase_stalled << std::endl;
	break;

      case flush_pipeline_state:
	std::cout << "state: flush_pipeline_state" << std::endl;
	std::cout <<  "e5_phase_stalled:  "<< e5_phase_stalled 
		  <<  " e4_phase_stalled:  "<< e4_phase_stalled << std::endl;
	break;

      }
#endif

      switch ((processor_state_fsm) m_run_state) {

      case idle_state:
	break;

      case usual_state:
	// usual_state suppose that the data memory allows two Load/store to be performed simultaneously
	Tms320C6xIss::tempRegisterFileUpdate();
	state.PGPS.setStall(state.PGPS.isNextCycleStall());
	state.PSPW.setStall(state.PSPW.isNextCycleStall());
	state.PWPR.setStall(state.PWPR.isNextCycleStall());
	state.PRDP.setStall(state.PRDP.isNextCycleStall());
	state.PGPS.setNextCycleStall(false);
	state.PSPW.setNextCycleStall(false);
	state.PWPR.setNextCycleStall(false);
	state.PRDP.setNextCycleStall(false);
	// we go backwards in the pipeline execution
	Tms320C6xIss::e5_pipeline_phase(); 
	Tms320C6xIss::e4_pipeline_phase();
	Tms320C6xIss::e3_pipeline_phase(); 
	Tms320C6xIss::e2_pipeline_phase();
	Tms320C6xIss::e1_pipeline_phase();
	// decode stage decomposed into 2 phases
	Tms320C6xIss::dc_pipeline_phase();
	Tms320C6xIss::dp_pipeline_phase();
	//fetch stage decomposed into 4 phases
	Tms320C6xIss::pr_pipeline_phase();
	Tms320C6xIss::pw_pipeline_phase();
	Tms320C6xIss::ps_pipeline_phase();
	Tms320C6xIss::pg_pipeline_phase();
	Tms320C6xIss::registerFileUpdate();
	break;

      case two_datamemory_ldst_state:
	// the execution packet contains one data memory store and one data memory load
	// they have to be performed sequentially 
	Tms320C6xIss::tempRegisterFileUpdate();
	state.PGPS.setStall(state.PGPS.isNextCycleStall());
	state.PSPW.setStall(state.PSPW.isNextCycleStall());
	state.PWPR.setStall(state.PWPR.isNextCycleStall());
	state.PRDP.setStall(state.PRDP.isNextCycleStall());
	state.PGPS.setNextCycleStall(false);
	state.PSPW.setNextCycleStall(false);
	state.PWPR.setNextCycleStall(false);
	state.PRDP.setNextCycleStall(false);
	// we go backwards in the pipeline execution
	// execute stage decomposed into 5 phases
	if (!e5_phase_stalled)  
	  Tms320C6xIss::e5_pipeline_phase(); 
 	else e5_phase_stalled = false;
	// cette ligne qui bloque si decommenté	if (!e4_phase_stalled)  
	Tms320C6xIss::e4_pipeline_phase();
	if (!e3_phase_stalled) 
	  Tms320C6xIss::e3_pipeline_phase(); 
	else  { 
	  e3_phase_stalled = false;
	  e4_phase_stalled = true;
	}
	Tms320C6xIss::e2_pipeline_phase();
	Tms320C6xIss::e1_pipeline_phase();
	if (cycle_to_flush_pipeline > 0 && !m_list_reg.empty())
	  Tms320C6xIss::writeRegister();
	// decode stage decomposed into 2 phases
	Tms320C6xIss::dc_pipeline_phase();
	Tms320C6xIss::dp_pipeline_phase();
	//fetch stage decomposed into 4 phases
	Tms320C6xIss::pr_pipeline_phase();
	Tms320C6xIss::pw_pipeline_phase();
	Tms320C6xIss::ps_pipeline_phase();
	Tms320C6xIss::pg_pipeline_phase();
	Tms320C6xIss::registerFileUpdate();
	break;
       
	case two_datamemory_ldst_bis_state:
	  // when there are one store and one load in a execution packet
	  // this state alternate with state two_datamemory_ldst_state
	  Tms320C6xIss::tempRegisterFileUpdate();
	  Tms320C6xIss::e5bis_pipeline_phase();
	  if (!e4_phase_stalled)  
	    Tms320C6xIss::e4bis_pipeline_phase(); 
	  else { 
	    e4_phase_stalled = false;
	    e5_phase_stalled = true;	  
	  }
	  Tms320C6xIss::e3bis_pipeline_phase();
	  Tms320C6xIss::e2bis_pipeline_phase();
	  break;


      case flush_pipeline_state:
	// the execution packet contains one data memory store and one data memory load
	// they have to be performed sequentially 
	Tms320C6xIss::tempRegisterFileUpdate();
	state.PGPS.setStall(state.PGPS.isNextCycleStall());
	state.PSPW.setStall(state.PSPW.isNextCycleStall());
	state.PWPR.setStall(state.PWPR.isNextCycleStall());
	state.PRDP.setStall(state.PRDP.isNextCycleStall());
	state.PGPS.setNextCycleStall(false);
	state.PSPW.setNextCycleStall(false);
	state.PWPR.setNextCycleStall(false);
	state.PRDP.setNextCycleStall(false);
	// we go backwards in the pipeline execution
	// execute stage decomposed into 5 phases
	if (!e5_phase_stalled)  
	  Tms320C6xIss::e5bis_pipeline_phase(); 
	if (!e4_phase_stalled) 
	  Tms320C6xIss::e4_pipeline_phase();
	Tms320C6xIss::e3_pipeline_phase(); 

	Tms320C6xIss::e2_pipeline_phase();
	Tms320C6xIss::e1_pipeline_phase();
	if (cycle_to_flush_pipeline > 0 && !m_list_reg.empty())
	  Tms320C6xIss::writeRegister();
	// decode stage decomposed into 2 phases
	Tms320C6xIss::dc_pipeline_phase();
	Tms320C6xIss::dp_pipeline_phase();
	//fetch stage decomposed into 4 phases
	Tms320C6xIss::pr_pipeline_phase();
	Tms320C6xIss::pw_pipeline_phase();
	Tms320C6xIss::ps_pipeline_phase();
	Tms320C6xIss::pg_pipeline_phase();
	Tms320C6xIss::registerFileUpdate();
	break;
       
     }

#if TMS320C62_DEBUG
      std::cout
	<< m_name << std::hex
	<< " PC: " << r_pc << " branchAddress: " <<m_branchAddress << " branchTaken: " << m_branchTaken 
	<< std::endl
	<< " exceptionSignal: " << std::hex << m_exceptionSignal
	<< std::endl << std::endl << std::dec;
       Tms320C6xIss::dumpRegisterFile();
#endif 

    }

    inline bool Tms320C6xIss::isConditionalInstruction(InstructionState instruction) {
      bool execute = false;
      if ((instruction.ins.common.creg == 0 && instruction.ins.common.z == 0) || 
	  (instruction.ins.common.creg == CREG_B0 && (instruction.ins.common.z == 1) && state.regfile[sideB][0] == 0) ||
	  (instruction.ins.common.creg == CREG_B0 && (instruction.ins.common.z == 0) && state.regfile[sideB][0] != 0) ||
	  (instruction.ins.common.creg == CREG_B1 && (instruction.ins.common.z == 1) && state.regfile[sideB][1] == 0) ||
	  (instruction.ins.common.creg == CREG_B1 && (instruction.ins.common.z == 0) && state.regfile[sideB][1] != 0) ||
	  (instruction.ins.common.creg == CREG_B2 && (instruction.ins.common.z == 1) && state.regfile[sideB][2] == 0) ||
	  (instruction.ins.common.creg == CREG_B2 && (instruction.ins.common.z == 0) && state.regfile[sideB][2] != 0) ||
	  (instruction.ins.common.creg == CREG_A1 && (instruction.ins.common.z == 1) && state.regfile[sideA][1] == 0) ||
	  (instruction.ins.common.creg == CREG_A1 && (instruction.ins.common.z == 0) && state.regfile[sideA][1] != 0) ||
	  (instruction.ins.common.creg == CREG_A2 && (instruction.ins.common.z == 1) && state.regfile[sideA][2] == 0) ||
	  (instruction.ins.common.creg == CREG_A2 && (instruction.ins.common.z == 0) && state.regfile[sideA][2] != 0)  )
	execute = true;
      return execute;
    }
	

    void Tms320C6xIss::pg_pipeline_phase() {
      // first phase of the fetch stage
      // the program address is generated in the CPU
      bool readyToExecute;
      uint32_t new_pc;

      readyToExecute = (state.PG.isBranchFlag() || !state.PGPS.isStall()) ? true : false;
      if (readyToExecute) {
	new_pc = state.PG.isBranchFlag() ? state.PG.getBranchAddress() : state.PG.getPC() + FETCH_SIZE;
	state.PGPS.setPC(new_pc);
	state.PGPS.setBranchFlag(state.PG.isBranchFlag());
	
	state.PG.setPC(new_pc & FETCH_MASK);
	state.PG.setBranchFlag(false);

#if TMS320C62_DEBUG
	std::cout << "  PG PC=" << std::hex << state.PGPS.getPC() << " JumptoPC " << 
	  state.PG.getBranchAddress() << std::dec << " " << 
	  state.PGPS.isBranchFlag() << " " << state.PGPS.isStall() << " "<< state.PGPS.isNextCycleStall() <<
	  std::dec << std::endl;
#endif
      }
      else { // !readyToExecute
	state.PGPS.setBranchFlag(false);	
#if TMS320C62_DEBUG
	std::cout << "  PG PC=" << std::hex << state.PGPS.getPC() << " JumptoPC " << 
	  state.PG.getBranchAddress() << std::dec <<" " << 
	  state.PGPS.isBranchFlag() << " " << state.PGPS.isStall() << " " << state.PGPS.isNextCycleStall() << " stall" <<
	  std::dec << std::endl;
#endif
      }
    }

    void Tms320C6xIss::ps_pipeline_phase() {
      // second phase of the fetch stage
      // the program address is sent to memory
      bool readyToExecute;
 
      readyToExecute = (state.PGPS.isBranchFlag() || !state.PSPW.isStall()) ? true : false;
      if (readyToExecute) {
	state.PSPW.setPC(state.PGPS.getPC()) ;
	state.PSPW.setBranchFlag(state.PGPS.isBranchFlag());

	state.PGPS.setNextCycleStall(state.PSPW.isNextCycleStall());
	state.PGPS.setStall(false);

#if TMS320C62_DEBUG
	std::cout << "  PS PC=" << std::hex << state.PSPW.getPC() << std::dec << " " <<
	  state.PSPW.isBranchFlag() << " " << state.PSPW.isStall() << " " << state.PSPW.isNextCycleStall() <<
	  std::dec << std::endl;
#endif
      }
      else { // !readyToExecute
	state.PGPS.setNextCycleStall(state.PSPW.isNextCycleStall());
#if TMS320C62_DEBUG
	std::cout << "  PS PC=" << std::hex << state.PSPW.getPC() << std::dec << " " <<
	  state.PSPW.isBranchFlag() << " " << state.PSPW.isStall() << " " << state.PSPW.isNextCycleStall() << " stall" <<
	  std::dec << std::endl;
#endif
      }
    }

    void Tms320C6xIss::pw_pipeline_phase() {
      // third phase of the fetch stage
      // a program memory access is performed
      bool readyToExecute;
      InstructionState *inst  = new InstructionState();
     
      readyToExecute = (state.PSPW.isBranchFlag() || !state.PWPR.isStall()) ? true : false;
      if (readyToExecute) {
	state.PWPR.setPC(state.PSPW.getPC()) ;
	state.PWPR.setBranchFlag(state.PSPW.isBranchFlag());
	for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	  inst->setInstruction(m_instruction[i]);	  
	  state.PWPR.setInstInPack(*inst,i);
	}
	state.PSPW.setNextCycleStall(state.PWPR.isNextCycleStall());
	state.PSPW.setStall(false);

#if TMS320C62_DEBUG
	std::cout << "  PW PC=" << std::hex << state.PSPW.getPC() << std::dec << " " <<
	  state.PSPW.isBranchFlag() << " " << state.PWPR.isStall() << " " << state.PWPR.isNextCycleStall() << " ";
	std::cout << std::hex << "               ";
	for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	  inst =  state.PWPR.getInst(i);	  
	  std::cout << inst->getInstruction().ins << "  "; 
	}
	std::cout << std::dec << std::endl;
#endif
      }
      else { // !readyToExecute
	state.PSPW.setNextCycleStall(state.PWPR.isNextCycleStall());
#if TMS320C62_DEBUG
	std::cout << "  PW PC=" << std::hex << state.PSPW.getPC() << std::dec << " " <<
	  state.PSPW.isBranchFlag() << " " << state.PWPR.isStall() << " " << state.PWPR.isNextCycleStall() << " stall" << 
	  std::dec << std::endl;
#endif
      }      
    }

    void Tms320C6xIss::pr_pipeline_phase() {
      // forth phase of the fetch stage
      // the fetch packet is received
      bool readyToExecute;

      readyToExecute = (state.PWPR.isBranchFlag() || !state.PRDP.isStall()) ? true : false;
      if (readyToExecute) {
	state.PRDP.setPC(state.PWPR.getPC()) ;
	state.PRDP.setBranchFlag(state.PWPR.isBranchFlag());
	state.PRDP.setInstPack(state.PWPR.getInstPack());
	
	state.PWPR.setNextCycleStall(state.PRDP.isNextCycleStall());
	state.PWPR.setStall(false);

#if TMS320C62_DEBUG
	InstructionState *inst  = new InstructionState();
	std::cout << "  PR PC=" << std::hex << state.PRDP.getPC() << std::dec << " " <<
	  state.PRDP.isBranchFlag() << " " << state.PRDP.isStall() << " " << state.PRDP.isNextCycleStall() << " ";
	std::cout << std::hex << "               ";
	for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	  inst = state.PRDP.getInst(i);	  
	  std::cout << inst->getInstruction().ins << "  "; 
	}
	std::cout << std::dec << std::endl;
#endif
      }
      else { // !readyToExecute
	state.PWPR.setNextCycleStall(state.PRDP.isNextCycleStall());
#if TMS320C62_DEBUG
	std::cout << "  PR PC=" << std::hex << state.PWPR.getPC() << std::dec << " " <<
	  state.PWPR.isBranchFlag() << " " << state.PRDP.isStall() << " " << state.PRDP.isNextCycleStall() << " ";
	std::cout << std::dec << std::endl;
#endif
      }      
    }

    void Tms320C6xIss::dp_pipeline_phase() {
      // first phase of the decode stage
      // the fetch packets are split into execute packets. An execute packet consists of one instruction or from two to eight
      // parallel instructions
      uint32_t count = 0, count1 = 0, start, end;
      bool multinop_flag = false;

      if (state.DP.getInstLeft() == 0 || state.PRDP.isBranchFlag() ) {

	state.DP.setPC(state.PRDP.getPC());
	state.DP.setBranchFlag(state.PRDP.isBranchFlag());
	state.PRDP.setStall(false);
	state.DP.setInstPack(state.PRDP.getInstPack());

	//take care of branch in a fetch packet
	state.DP.setInstLeft(FETCH_PACKET_SIZE - ((state.DP.getPC() - (state.DP.getPC() & FETCH_MASK)) >> 2));
      }
      
      // break into execution packets and dispatch
      state.DPDC.setPC((state.DP.getPC() & FETCH_MASK) + ((FETCH_PACKET_SIZE - state.DP.getInstLeft()) << 2));
      start = FETCH_PACKET_SIZE - state.DP.getInstLeft();
      for (count = FETCH_PACKET_SIZE - state.DP.getInstLeft(), count1 = 0; count < FETCH_PACKET_SIZE; count++, count1++) {
	
	InstructionState *instState = state.DP.getInst(count);

	if (instState->ins.nop.unit == 0x00 && instState->ins.nop.src > 0) {
	  multinop_flag = true;
	}

	state.DPDC.setInstInPack(*instState, count1);
	state.DP.setInstLeft(state.DP.getInstLeft()-1);

	// break if  next instruction is not parallel to this one
	if (!instState->isParallelInstruction(*instState))
	  break;
      }

      // fill with NOP the rest of the instruction
      InstructionState *instState = new InstructionState();
      instState->setIns(NOP1);	
      for (count = count1 + 1; count < FETCH_PACKET_SIZE; count ++) {
	state.DPDC.setInstInPack(*instState, count);
      }

      // multicycle NOP management
      if (multinop_flag) {
	end = start + count1;
	for (count = start; count <= end; count ++) {
	  InstructionState *instState = state.DP.getInst(count);
	  if (instState->ins.nop.unit == 0x00 && instState->ins.nop.src > 0){
	    instState->ins.nop.src--;
	  }
	  else if (instState->isParallelInstruction(*instState))
	    instState->setIns(NOP1_PARALLEL); 
	  else
	    instState->setIns(NOP1);
	  state.DP.setInstInPack(*instState, count);
	}

	state.DP.setInstLeft(FETCH_PACKET_SIZE - start);
      }

      // stall signal is set if necessary
      if (state.DP.getInstLeft() != 0)
	state.PRDP.setNextCycleStall(true);

#if TMS320C62_DEBUG
      std::cout << "  DP PC=" << std::hex << state.DP.getPC() << std::dec << " " <<
	state.DP.isBranchFlag() << " " << "     instLeft: " << state.DP.getInstLeft() ;
      std::cout << std::hex << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *instState = state.DP.getInst(i);	  
	std::cout << instState->getInstruction().ins << "  "; 
      }
      std::cout << std::dec << std::endl;
#endif      
    }
    

    void Tms320C6xIss::dc_pipeline_phase() {
      // second phase of the decode stage
      // instructions are decoded in the functional units
      
      state.DCE1.setPC(state.DPDC.getPC());
      state.DCE1.setInstPack(state.DPDC.getInstPack());
      state.DCE1.initState();

#if TMS320C62_DEBUG
      std::cout << "  DC PC=" << std::hex << state.DPDC.getPC() ;
      std::cout << std::hex << "                      ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.DCE1.getInst(i);

	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  "; 
      }
      std::cout << std::dec << std::endl;
#endif    
    }

    
    // first phase of the execute stage
    void Tms320C6xIss::e1_pipeline_phase() {
      bool execute = false;
      uint32_t ldst_instructions = 0;
      uint32_t ldst_really_executed = 0;

      state.E1E2.setPC(state.DCE1.getPC());
      state.E1E2.setInstPack(state.DCE1.getInstPack());

#if TMS320C62_DEBUG
      std::cout << "  E1 PC=" << std::hex << state.DCE1.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E1E2.getInst(i);
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "   ";
      }
      std::cout << std::dec << std::endl;
#endif     

      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState * instState = (state.E1E2.getInst(i));
	
	// number of load store instruction in this packet ?
	if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET || 
	    instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	  switch (ldst_instructions) {
	  case 0:
	    instState->setFirstLoadStore(true);
	    instState->setSecondLoadStore(false);
	    instState->setLoadStorePosition(0);	    
	    break;
	  case 1:
	    instState->setFirstLoadStore(false);
	    instState->setSecondLoadStore(true);
	    instState->setLoadStorePosition(1);	    
	    break;
	  default:
	    instState->setFirstLoadStore(false);
	    instState->setSecondLoadStore(false);
	    break;
	  }
	ldst_instructions++;
	}  
      }
      state.E1E2.setLoadStoreNumber(ldst_instructions);

      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E1E2.getInst(i));
       
	// is it a NOP instruction
	if (instState->ins.nop.unit == 0x00 && instState->ins.nop.src == 0)
	  continue;
	
	// check condition in creg field
	if (isConditionalInstruction(*instState))
	  execute = true;
	else execute = false;
	instState->setExecute(execute);

#if TMS320C62_DEBUG
	Tms320C6xIss::instruction_print(instState->ins.ins); 
#endif
	// if execute condition failed, skip the current instruction
	if (!instState->isExecute())
	  continue;

	// instruction execution
	if (instState->ins.nop.unit == 0 ) {
#if TMS320C62_DEBUG
	  std::cout << " NOP encountered  " << std::endl;
#endif
	}
	else if (instState->ins.idle.unit == 0x7800 ) {
#if TMS320C62_DEBUG
	  std::cout << " IDLE encountered  " << std::endl;
#endif
	}
	else if (instState->ins.l.unit == LUNIT) {
	  func_t func = l_function_e1[instState->ins.l.op];	
	  (this->*func)(instState);
#if TMS320C62_DEBUG
	  std::cout << " execute " << name_l_function_e1[instState->ins.l.op] << std::endl;
#endif	
	}
	else if (instState->ins.s.unit == SUNIT) {
	  func_t func = s_function_e1[instState->ins.s.op];
#if TMS320C62_DEBUG
	  std::cout << " execute " << name_s_function_e1[instState->ins.s.op] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.s_addk.unit == SUNIT_ADDK) {
	  func_t func = s_addk_function_e1[0];
#if TMS320C62_DEBUG
	  std::cout << " execute " << name_s_addk_function_e1[0] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.s_bcond.unit == SUNIT_BCOND) {
	  func_t func = s_bcond_function_e1[0];
#if TMS320C62_DEBUG
	  std::cout << " execute " << name_s_bcond_function_e1[0] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.s_immed.unit == SUNIT_IMMED) {
	  func_t func = s_immed_function_e1[instState->ins.s_immed.op];
 #if TMS320C62_DEBUG
	  std::cout << " execute " <<  name_s_immed_function_e1[instState->ins.s_immed.op] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.s_mvk.unit == SUNIT_MVK) {
	  func_t func = s_mvk_function_e1[instState->ins.s_mvk.op];
#if TMS320C62_DEBUG
	  std::cout << " execute " << name_s_mvk_function_e1[instState->ins.s_mvk.op] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.m.unit == MUNIT) {
	  func_t func = m_function_e1[instState->ins.m.op];
#if TMS320C62_DEBUG
	  std::cout << " execute " << name_m_function_e1[instState->ins.m.op] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.d.unit == DUNIT) {
	  func_t func = d_function_e1[instState->ins.d.op];
#if TMS320C62_DEBUG
	  std::cout << " execute " << name_d_function_e1[instState->ins.d.op] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {
	    func_t func = d_ldstOffset_function_e1[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	    std::cout << " execute " << name_d_ldstOffset_function_e1[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	    (this->*func)(instState);
#if TMS320C62_DEBUG
	    std::cout << std::endl;
	    instState->print();
#endif	
	    ldst_really_executed++;
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	    func_t func = d_ldstBaseROffset_function_e1[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	    std::cout << " execute " << name_d_ldstBaseROffset_function_e1[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	    (this->*func)(instState);
#if TMS320C62_DEBUG
	    std::cout << std::endl;
	    instState->print();
#endif	
	    ldst_really_executed++;
 	}
	else
	  std::cout << "Warning: unknown instruction in E1 pipeline phase @ " << m_exec_cycles <<  " cycle, instruction is " 
		    << std::hex << instState->ins.ins
		    << " PC is " << state.E1E2.getPC() << std::dec << std::endl;
      }
#if TMS320C62_DEBUG
      if (ldst_instructions > 1)
	std::cout << ldst_instructions << " load/store instructions encountered " << std::endl;
#endif
      tworeadinst_in_packet = false;
      int nbread = 0;
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState * instState = (state.E1E2.getInst(i));
	if (instState->isReadInstInPacketExecute()) nbread++;
      }      
      if (nbread > 1) {
	tworeadinst_in_packet = true;
#if TMS320C62_DEBUG
	  std::cout << " two read instruction in packet " << std::endl;	
#endif
      }

      twowriteinst_in_packet = false;
      int nbwrite = 0;
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState * instState = (state.E1E2.getInst(i));
	if (instState->isWriteInstInPacketExecute()) nbwrite++;
      }      
      if (nbwrite > 1) {
	twowriteinst_in_packet = true;
#if TMS320C62_DEBUG
	  std::cout << " two write instruction in packet " << std::endl;	
#endif
       }
      if (ldst_instructions > 1) {
	multiple_loadstore_instructions = true;
	if (ldst_really_executed == 1)
	  end_of_loop = true;
      }
    }


    // second phase of the execute stage
    void Tms320C6xIss::e2_pipeline_phase() {

      state.E2E3.setPC(state.E1E2.getPC());
      state.E2E3.setInstPack(state.E1E2.getInstPack());
      state.E2E3.setLoadStoreNumber(state.E1E2.getLoadStoreNumber());

#if TMS320C62_DEBUG
      std::cout << "  E2 PC=" << std::hex << state.E1E2.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E2E3.getInst(i);	  
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  ";
      }

      std::cout << std::dec << std::endl;
#endif      

      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E2E3.getInst(i));

#if TMS320C62_DEBUG
	//	instState->print();
#endif

	// instruction execution
	if (instState->ins.nop.unit == 0)
	  continue;

	if (!instState->isExecute())
	  continue;

#if TMS320C62_DEBUG
	if(instState->ins.m.unit == MUNIT 
	   || instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET 
	   || instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET)
	   Tms320C6xIss::instruction_print(instState->ins.ins); 
#endif

	if (instState->ins.m.unit == MUNIT) {
	  func_t func = m_function_e2[instState->ins.m.op];
#if TMS320C62_DEBUG
	  std::cout << " execute " << name_m_function_e2[instState->ins.m.op] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {
	  switch (state.E2E3.getLoadStoreNumber()) {
	  case 1:
	    {
	      // we only have one read/write operation in this packet
	      func_t func = d_ldstOffset_function_e2[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	      std::cout << " execute " << name_d_ldstOffset_function_e2[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	      (this->*func)(instState);
	    }   
	    break;
	  case 2:
	    {
	      // we have two read/write operations in this packet
	      // second read is treated	    
	      if (instState->isSecondLoadStore()) {
		func_t func = d_ldstOffset_function_e2[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
		std::cout << std::endl;
		instState->print();
		std::cout << " execute " << name_d_ldstOffset_function_e2[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
		(this->*func)(instState);
	      }
	      else {
#if TMS320C62_DEBUG
		std::cout << std::endl;
#endif
	      }
	      ;
	    }
	    break;
	  }
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	  switch (state.E2E3.getLoadStoreNumber()) {
	  case 1:
	    {
	      // we only have one read/write operation in this packet
	      func_t func = d_ldstBaseROffset_function_e2[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	      std::cout << " execute " << name_d_ldstBaseROffset_function_e2[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	      (this->*func)(instState);
	    }
	    break;
	  case 2:
	    {
	      // we have two read/write operations in this packet
	      // second read is treated	    
	      if (instState->isSecondLoadStore()) {	    
		func_t func = d_ldstBaseROffset_function_e2[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
		std::cout << std::endl;
		instState->print();
		std::cout << " execute " << name_d_ldstBaseROffset_function_e2[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
		(this->*func)(instState);
	      }
	      else {
#if TMS320C62_DEBUG
		std::cout << std::endl;
#endif
	      }
	      ;
	    }
	    break;
	  }
	}
      }
    }

    // second phase of the execute stage
    // executed again in case of several load/store instruction in the same execute packet
    void Tms320C6xIss::e2bis_pipeline_phase() {

      state.E2E3.setPC(state.E1E2.getPC());
      state.E2E3.setInstPack(state.E1E2.getInstPack());
      state.E2E3.setLoadStoreNumber(state.E1E2.getLoadStoreNumber());

#if TMS320C62_DEBUG
      std::cout << "  E2bis PC=" << std::hex << state.E1E2.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E2E3.getInst(i);	  
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  ";
      }

      std::cout << std::dec << std::endl;
#endif      
   
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E2E3.getInst(i));

	// instruction execution
	if (instState->ins.nop.unit == 0) {
	  continue;
	}
	if (!instState->isExecute())
	  continue;

#if TMS320C62_DEBUG
	if(instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET 
	   || instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET)
	   Tms320C6xIss::instruction_print(instState->ins.ins); 
#endif

	if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {
	  if (instState->isFirstLoadStore()) {
	    func_t func = d_ldstOffset_function_e2[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	    std::cout << std::endl;
	    instState->print();
	    std::cout << " execute " << name_d_ldstOffset_function_e2[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	    (this->*func)(instState);
	    e3_phase_stalled = true; 
	  }
	  else {
#if TMS320C62_DEBUG
	    std::cout << std::endl;
#endif
	  }
	  ;
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	  if (instState->isFirstLoadStore()) {
	    func_t func = d_ldstBaseROffset_function_e2[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	    std::cout <<  std::endl;
	    instState->print();
	    std::cout << " execute " << name_d_ldstBaseROffset_function_e2[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	    (this->*func)(instState);
	    e3_phase_stalled = true; 
	  }
	  else {
#if TMS320C62_DEBUG
	    std::cout << std::endl;
#endif
	  }
	  ;
	}
      }
    }

    // third phase of the execute stage
    void Tms320C6xIss::e3_pipeline_phase() {

      state.E3E4.setPC(state.E2E3.getPC());
      state.E3E4.setInstPack(state.E2E3.getInstPack());
      state.E3E4.setLoadStoreNumber(state.E2E3.getLoadStoreNumber());

#if TMS320C62_DEBUG
      std::cout << "  E3 PC=" << std::hex << state.E2E3.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E3E4.getInst(i);	  
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  "; 
      }
      std::cout << std::dec << std::endl;
#endif      

      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E3E4.getInst(i));

#if TMS320C62_DEBUG
	//	instState->print();
#endif
	// instruction execution
	if (instState->ins.nop.unit == 0 )
	  continue;
 
	if (!instState->isExecute())
	  continue;

#if TMS320C62_DEBUG
	Tms320C6xIss::instruction_print_ld(instState->ins.ins); 
	Tms320C6xIss::instruction_print_st(instState->ins.ins);
#endif

	if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {	  
	  switch (state.E3E4.getLoadStoreNumber()) {
	  case 1:
	    {
	      func_t func = d_ldstOffset_function_e3[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	      std::cout << " execute " << name_d_ldstOffset_function_e3[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	      (this->*func)(instState);
	    }   
	    break;
	  case 2:
	    {
	      if (instState->isFirstLoadStore()) {	    
		func_t func = d_ldstOffset_function_e3[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
		std::cout << " execute " << name_d_ldstOffset_function_e3[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
		(this->*func)(instState);
	      }
	      else {
#if TMS320C62_DEBUG
		std::cout << std::endl;
#endif
	      }
	      ;
	    }
	    break;
	  }
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	  switch (state.E2E3.getLoadStoreNumber()) {
	  case 1:
	    {   
	      func_t func = d_ldstBaseROffset_function_e3[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	      std::cout << " execute " << name_d_ldstBaseROffset_function_e3[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	      (this->*func)(instState);
	    }
	    break;
	  case 2:
	    {
	      if (instState->isFirstLoadStore()) {	    
		func_t func = d_ldstBaseROffset_function_e3[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
		std::cout << " execute " << name_d_ldstBaseROffset_function_e3[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
		(this->*func)(instState);
	      }
	      else {
#if TMS320C62_DEBUG
		std::cout << std::endl;
#endif
	      }
	      ;
	    }
	    break;
	  }
	}
      }
    }
    
    
    // third phase of the execute stage
    void Tms320C6xIss::e3bis_pipeline_phase() {

      state.E3E4.setPC(state.E2E3.getPC());
      state.E3E4.setInstPack(state.E2E3.getInstPack());
      state.E3E4.setLoadStoreNumber(state.E2E3.getLoadStoreNumber());

#if TMS320C62_DEBUG
      std::cout << "  E3bis PC=" << std::hex << state.E2E3.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E3E4.getInst(i);	  
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  "; 
      }
      std::cout << std::dec << std::endl;
#endif      

      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E3E4.getInst(i));

#if TMS320C62_DEBUG
	//	instState->print();
#endif
	// instruction execution
	if (instState->ins.nop.unit == 0 )
	  continue;
 
	if (!instState->isExecute())
	  continue;

#if TMS320C62_DEBUG
	Tms320C6xIss::instruction_print_ld(instState->ins.ins); 
	Tms320C6xIss::instruction_print_st(instState->ins.ins);
#endif

	if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {	  
	  switch (state.E3E4.getLoadStoreNumber()) {
	  case 1:
	    {
	      func_t func = d_ldstOffset_function_e3[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	      std::cout << " execute " << name_d_ldstOffset_function_e3[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	      (this->*func)(instState);
	    }
	    break;
	  case 2:
	    {	    
	      if (instState->isSecondLoadStore()) {	    
		func_t func = d_ldstOffset_function_e3[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
		std::cout << " execute " << name_d_ldstOffset_function_e3[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
		(this->*func)(instState);
	      }
	      else {
#if TMS320C62_DEBUG
		std::cout << std::endl;
#endif
	      }
	      ;
	    }
	    break;
	  }
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	  switch (state.E2E3.getLoadStoreNumber()) {
	  case 1:
	    {
	      func_t func = d_ldstBaseROffset_function_e3[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	      std::cout << " execute " << name_d_ldstBaseROffset_function_e3[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	      (this->*func)(instState);
	    }
	    break;
	  case 2:
	    {
	      if (instState->isSecondLoadStore()) {	    
		func_t func = d_ldstBaseROffset_function_e3[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
		std::cout << " execute " << name_d_ldstBaseROffset_function_e3[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
		(this->*func)(instState);
	      }
	      else {
#if TMS320C62_DEBUG
		std::cout << std::endl;
#endif
	      }
	      ;
	    }
	    break;
	  }
	}
      }
    }
    
    
    void Tms320C6xIss::e4_pipeline_phase() {

      state.E4E5.setPC(state.E3E4.getPC());
      state.E4E5.setInstPack(state.E3E4.getInstPack());
      state.E4E5.setLoadStoreNumber(state.E3E4.getLoadStoreNumber());

#if TMS320C62_DEBUG
      std::cout << "  E4 PC=" << std::hex << state.E3E4.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E4E5.getInst(i);	  
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  "; 
      }
      std::cout << std::dec << std::endl;
#endif  
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E4E5.getInst(i));

#if TMS320C62_DEBUG
	Tms320C6xIss::instruction_print_ld(instState->ins.ins); 
	Tms320C6xIss::instruction_print_st(instState->ins.ins);
#endif
#if TMS320C62_DEBUG
	//	instState->print();
#endif
	// instruction execution
	if (instState->ins.nop.unit == 0 )
	  continue;
 
	if (!instState->isExecute())
	  continue;

	if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {
	    func_t func = d_ldstOffset_function_e4[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	    std::cout << " execute " << name_d_ldstOffset_function_e4[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	    (this->*func)(instState);
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	    func_t func = d_ldstBaseROffset_function_e4[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	    std::cout << " execute " << name_d_ldstBaseROffset_function_e4[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	    (this->*func)(instState);
	}
      }
    }

    void Tms320C6xIss::e4bis_pipeline_phase() {

      state.E4E5.setPC(state.E3E4.getPC());
      state.E4E5.setInstPack(state.E3E4.getInstPack());
      state.E4E5.setLoadStoreNumber(state.E3E4.getLoadStoreNumber());
      
#if TMS320C62_DEBUG
      std::cout << "  E4bis PC=" << std::hex << state.E3E4.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E4E5.getInst(i);	  
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  "; 
      }
      std::cout << std::dec << std::endl;
#endif  
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E4E5.getInst(i));

#if TMS320C62_DEBUG
	Tms320C6xIss::instruction_print_ld(instState->ins.ins); 
	Tms320C6xIss::instruction_print_st(instState->ins.ins);
#endif
#if TMS320C62_DEBUG
	//	instState->print();
#endif
	// instruction execution
	if (instState->ins.nop.unit == 0 )
	  continue;
 
	if (!instState->isExecute())
	  continue;

	if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {
	  if (state.E4E5.getLoadStoreNumber() == 1) {
	    func_t func = d_ldstOffset_function_e4[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	    std::cout << " execute " << name_d_ldstOffset_function_e4[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	    (this->*func)(instState);
	  }
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	  if (state.E4E5.getLoadStoreNumber() == 1) {
	    func_t func = d_ldstBaseROffset_function_e4[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	    std::cout << " execute " << name_d_ldstBaseROffset_function_e4[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	    (this->*func)(instState);
	  }
	}
      }
    }

    void Tms320C6xIss::e5_pipeline_phase() {

#if TMS320C62_DEBUG
      std::cout << "  E5 PC=" << std::hex << state.E4E5.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E4E5.getInst(i);	  
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  "; 
      }
      std::cout << std::dec << std::endl;
#endif      

      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E4E5.getInst(i));

	if (instState->ins.nop.unit == 0 )
	  continue;

#if TMS320C62_DEBUG
	Tms320C6xIss::instruction_print_ld(instState->ins.ins); 
#endif

	if (!instState->isExecute())
	  continue;
	 
	if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {
	  switch (state.E4E5.getLoadStoreNumber()) {
	  case 1:
	    {
	      func_t func = d_ldstOffset_function_e5[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	      instState->print();
	      std::cout << " execute " << name_d_ldstOffset_function_e5[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	      (this->*func)(instState);
	    }
	    break;
	  case 2:
#if TMS320C62_DEBUG
	    std::cout << std::endl;
#endif
	    break;
	  }
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	  switch (state.E4E5.getLoadStoreNumber()) {
	  case 1:
	    {
	      func_t func = d_ldstBaseROffset_function_e5[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	      instState->print();
	      std::cout << " execute " << name_d_ldstBaseROffset_function_e5[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	      (this->*func)(instState);
	    }
	    break;
	  case 2:
#if TMS320C62_DEBUG
	    std::cout << std::endl;
#endif
	    break;
	  }
	}
      }
    }
    
    void Tms320C6xIss::e5bis_pipeline_phase() {

#if TMS320C62_DEBUG
      std::cout << "  E5bis PC=" << std::hex << state.E4E5.getPC() << "   ";
      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {
	InstructionState *inst;
	inst = state.E4E5.getInst(i);	  
	if (inst->getInstruction().ins != 0)
	  std::cout << inst->getInstruction().ins << "  "; 
      }
      std::cout << std::dec << std::endl;
#endif      

      for (int i = 0; i < FETCH_PACKET_SIZE; i++) {

	// execute instruction one after another
	InstructionState * instState = (state.E4E5.getInst(i));

	if (instState->ins.nop.unit == 0 )
	  continue;

#if TMS320C62_DEBUG
	Tms320C6xIss::instruction_print_ld(instState->ins.ins); 
#endif

	if (!instState->isExecute())
	  continue;
	 
	if (instState->ins.d_ldstOffset.unit == DUNIT_LDSTOFFSET) {
	  func_t func = d_ldstOffset_function_e5[instState->ins.d_ldstOffset.op];
#if TMS320C62_DEBUG
	  instState->print();
	  std::cout << " execute " << name_d_ldstOffset_function_e5[instState->ins.d_ldstOffset.op] << std::endl;
#endif	
	  (this->*func)(instState);
	}
	else if (instState->ins.d_ldstBaseROffset.unit == DUNIT_LDSTBASEROFFSET) {
	  func_t func = d_ldstBaseROffset_function_e5[instState->ins.d_ldstBaseROffset.op];
#if TMS320C62_DEBUG
	  instState->print();
	  std::cout << " execute " << name_d_ldstBaseROffset_function_e5[instState->ins.d_ldstBaseROffset.op] << std::endl;
#endif	
	  (this->*func)(instState);
	}
      }
    }
    
    void Tms320C6xIss::registerToBeUpdateLater(uint32_t bank, uint32_t reg, uint32_t value) {

      register_entry regentry;
#if TMS320C62_DEBUG
      std::cout << "register update postponed: " << "register: " << reg;
       if (!bank)
	 std::cout << " bank A" ;
       else std::cout << " bank B";
       std::cout << " (value: " << std::hex << value << ")" << std::dec << std::endl;
#endif
       regentry.bank = bank;
       regentry.reg = reg;
       regentry.value = value;
       m_list_reg.push_front(regentry);
       
#if TMS320C62_DEBUG
       std::cout << "m_list_reg contains " << m_list_reg.size() << " elements:"  << std::endl;
       for (std::list<register_entry>::iterator it = m_list_reg.begin(); it != m_list_reg.end(); ++it)
	 std::cout << "reg: " << (*it).reg << " bank: " << (*it).bank << " value: " << std::hex << (*it).value << std::dec << std::endl;
#endif
    }


    void Tms320C6xIss::writeRegister() {

      register_entry regentry = m_list_reg.back();
#if TMS320C62_DEBUG
       std::cout
	 << m_name << std::hex
	 << " value " << regentry.value << std::dec << " is written to ";
       if (!regentry.bank)
	 std::cout << " bank A" ;
       else std::cout << " bank B";
       std::cout << " register " << regentry.reg << std::endl;      
#endif
       state.regfile_temp[regentry.bank][regentry.reg] = regentry.value;
       m_list_reg.pop_back();
    }


    //
    // registerfile management
    //
void Tms320C6xIss::registerFileUpdate() {

      for (int bank = 0; bank <= 1; bank++)
	for (int i = 0; i < 16; i++)
	  state.regfile[bank][i] = state.regfile_temp[bank][i];
}

void Tms320C6xIss::tempRegisterFileUpdate() {

      for (int bank = 0; bank <= 1; bank++)
	for (int i = 0; i < 16; i++)
	  state.regfile_temp[bank][i] = state.regfile[bank][i];
}


	//
    //
    // .L unit Instruction set
    //
    //

    //
    // E1 phase
    //

    // ABS (.unit) src2 (xsint), dst (sint) -- .unit = .L1 or .L2 op field: 1A pipeline phase E1
    void Tms320C6xIss::op_abs_l_1a_e1(InstructionState * instState) {
      int32_t signedSrc2x, signedDst;
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedDst = abs(signedSrc2x);
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedDst;
    }

    // ABS (.unit) src2 (slong), dst (slong) -- .unit = .L1 or .L2 op field: 1A pipeline phase E1
    void Tms320C6xIss::op_abs_l_38_e1(InstructionState * instState) {
      int64_t signedLongSrc2, signedLongDst;
      uint64_t tmplong;
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      if ( signedLongSrc2 < 0 ) {
	signedLongDst = - signedLongSrc2;
      } else {
	signedLongDst = signedLongSrc2;
      }
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // ADD (.unit) src1 (sint), src2 (xsint), dst (sint) -- .unit = .L1, .L2 op field: 03 pipeline phase E1
    void Tms320C6xIss::op_add_l_03_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1]; 
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedDst = signedSrc1 + signedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedDst;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " add_l_03 operand: " << signedSrc1 << " " <<  signedSrc2x 
         << " add instruction result: " << signedDst << std::dec << " (" << (int) signedDst << ")" << std::endl;
#endif
    }

    // ADD (.unit) src1 (sint), src2 (xsint), dst (slong) -- .unit = .L1, .L2 op field: 23 pipeline phase E1
    void Tms320C6xIss::op_add_l_23_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x; 
      int64_t signedLongDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1]; 
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedLongDst = ((int64_t) signedSrc1) + ((int64_t) signedSrc2x);
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // ADD (.unit) src1 (xsint), src2 (slong), dst (slong) -- .unit = .L1, .L2 op field: 21 pipeline phase E1
    void Tms320C6xIss::op_add_l_21_e1(InstructionState * instState) {
      int32_t signedSrc1x; 
      int64_t signedLongSrc2, signedLongDst;
      uint64_t tmplong;
      signedSrc1x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1]; 
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      signedLongDst = ((int64_t) signedSrc1x) + signedLongSrc2;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // ADD (.unit) src1 (scst5), src2 (xsint), dst (xsint) -- .unit = .L1, .L2 op field: 02 pipeline phase E1
    void Tms320C6xIss::op_add_l_02_e1(InstructionState * instState) {
      int32_t signedCst1, signedSrc2x, signedDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedDst = signedCst1 + signedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedDst;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " add_l_02 operand: " << signedCst1 << " " <<  signedSrc2x 
         << " add instruction result: " << signedDst << std::dec << " (" << (int) signedDst << ")" << std::endl;
#endif
    }

    // ADD (.unit) src1 (scst5), src2 (slong), dst (slong) -- .unit = .L1, .L2 op field: 20 pipeline phase E1
    void Tms320C6xIss::op_add_l_20_e1(InstructionState * instState) {
      int64_t signedLongCst1, signedLongSrc2,  signedLongDst;
      uint64_t tmplong;
      signedLongCst1 = SIGN_EXTEND_LONG((int64_t) instState->ins.l.src1, 5);
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      signedLongDst = signedLongCst1 + signedLongSrc2;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // ADDU (.unit) src1 (usint), src2 (xusint), dst (uslong) -- .unit = .L1, .L2 op field: 2B pipeline phase E1
    void Tms320C6xIss::op_addu_l_2b_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x; 
      uint64_t unsignedLongDst;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1]; 
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedLongDst = ((int64_t) unsignedSrc1) + ((int64_t) unsignedSrc2x);
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = unsignedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = unsignedLongDst;
    }

    // ADDU (.unit) src1 (xusint), src2 (uslong), dst (uslong) -- .unit = .L1, .L2 op field: 29 pipeline phase E1
    void Tms320C6xIss::op_addu_l_29_e1(InstructionState * instState) {
      uint32_t unsignedSrc1x; 
      uint64_t unsignedLongSrc2, unsignedLongDst;
      unsignedSrc1x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      unsignedLongSrc2 = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      unsignedLongSrc2 = unsignedLongSrc2  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      unsignedLongDst = ((int64_t) unsignedSrc1x) + unsignedLongSrc2; 
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = unsignedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = unsignedLongDst;
    }

    // AND (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 7B pipeline phase E1
    void Tms320C6xIss::op_and_l_7b_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedDst;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1];
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = unsignedSrc1 & unsignedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // AND (.unit) src1 (scst5), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 7A pipeline phase E1
    void Tms320C6xIss::op_and_l_7a_e1(InstructionState * instState) {
      int32_t signedCst1; 
      uint32_t unsignedSrc2x, unsignedDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = signedCst1 & unsignedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPEQ (.unit) src1 (sint), src2 (xsint), dst (usint) -- .unit = .L1, .L2 op field: 53 pipeline phase E1
    void Tms320C6xIss::op_cmpeq_l_53_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x; 
      uint32_t unsignedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (signedSrc1 == signedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPEQ (.unit) src1 (scst5), src2 (xsint), dst (usint) -- .unit = .L1, .L2 op field: 52 pipeline phase E1
    void Tms320C6xIss::op_cmpeq_l_52_e1(InstructionState * instState) {
      int32_t signedCst1, signedSrc2x; 
      uint32_t unsignedDst;
      signedCst1 = SIGN_EXTEND((int32_t)instState->ins.l.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (signedCst1 == signedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPEQ (.unit) src1 (xsint), src2 (slong), dst (usint) -- .unit = .L1, .L2 op field: 51 pipeline phase E1
    void Tms320C6xIss::op_cmpeq_l_51_e1(InstructionState * instState) {
      int32_t signedSrc1x; 
      int64_t signedLongSrc2; 
      uint32_t unsignedDst;
      uint64_t tmplong;
      signedSrc1x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      unsignedDst = (signedSrc1x == (signedLongSrc2 & 0xffffffff)) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPEQ (.unit) src1 (scst5), src2 (slong), dst (usint) -- .unit = .L1, .L2 op field: 50 pipeline phase E1
    void Tms320C6xIss::op_cmpeq_l_50_e1(InstructionState * instState) {
      int32_t signedCst1; 
      int64_t signedLongSrc2; 
      uint32_t unsignedDst;
      uint64_t tmplong;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      unsignedDst = (signedCst1 == (signedLongSrc2 & 0xffffffff)) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPGT (.unit) src1 (sint), src2 (xsint), dst (usint) -- .unit = .L1, .L2 op field: 47 pipeline phase E1
    void Tms320C6xIss::op_cmpgt_l_47_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x; 
      uint32_t unsignedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (signedSrc1 > signedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPGT (.unit) src1 (scst5), src2 (xsint), dst (usint) -- .unit = .L1, .L2 op field: 46 pipeline phase E1
    void Tms320C6xIss::op_cmpgt_l_46_e1(InstructionState * instState) {
      int32_t signedCst1, signedSrc2x; 
      uint32_t unsignedDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (signedCst1 > signedSrc2x) ? 1 : 0;
//       std::cout << " cst1: " << signedCst1 << " src2: " << signedSrc2x << " dst: " << unsignedDst << std::endl;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPGT (.unit) src1 (xsint), src2 (slong), dst (usint) -- .unit = .L1, .L2 op field: 45 pipeline phase E1
    void Tms320C6xIss::op_cmpgt_l_45_e1(InstructionState * instState) {
      int32_t signedSrc1x; 
      int64_t signedLongSrc2; 
      uint32_t unsignedDst;
      uint64_t tmplong;
      signedSrc1x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      unsignedDst = (signedSrc1x > signedLongSrc2) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPGT (.unit) src1 (scst5), src2 (slong), dst (usint) -- .unit = .L1, .L2 op field: 44 pipeline phase E1
    void Tms320C6xIss::op_cmpgt_l_44_e1(InstructionState * instState) {
      int32_t signedCst1; 
      int64_t signedLongSrc2; 
      uint32_t unsignedDst;
      uint64_t tmplong;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      unsignedDst = (signedCst1 > signedLongSrc2) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPGTU (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 4f pipeline phase E1
    void Tms320C6xIss::op_cmpgtu_l_4f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedDst;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1];
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (unsignedSrc1 > unsignedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPGTU (.unit) src1 (ucst5), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 4e pipeline phase E1
    void Tms320C6xIss::op_cmpgtu_l_4e_e1(InstructionState * instState) {
      uint32_t unsignedCst1, unsignedSrc2x, unsignedDst;
      unsignedCst1 = instState->ins.l.src1;
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (unsignedCst1 > unsignedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPGTU (.unit) src1 (xusint), src2 (uslong), dst (usint) -- .unit = .L1, .L2 op field: 4d pipeline phase E1
    void Tms320C6xIss::op_cmpgtu_l_4d_e1(InstructionState * instState) {
      uint32_t unsignedSrc1x; 
      uint64_t unsignedLongSrc2; 
      uint32_t unsignedDst;
      unsignedSrc1x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      unsignedLongSrc2 = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      unsignedLongSrc2 = unsignedLongSrc2  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      unsignedDst = (unsignedSrc1x > unsignedLongSrc2) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPGTU (.unit) src1 (ucst5), src2 (uslong), dst (usint) -- .unit = .L1, .L2 op field: 4d pipeline phase E1
    void Tms320C6xIss::op_cmpgtu_l_4c_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      uint64_t unsignedLongSrc2; 
      uint32_t unsignedDst;
      unsignedCst1 = instState->ins.l.src1;
      unsignedLongSrc2 = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      unsignedLongSrc2 = unsignedLongSrc2  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      unsignedDst = (unsignedCst1 > unsignedLongSrc2) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPLT (.unit) src1 (sint), src2 (xsint), dst (usint) -- .unit = .L1, .L2 op field: 57 pipeline phase E1
    void Tms320C6xIss::op_cmplt_l_57_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x;
      uint32_t unsignedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (signedSrc1 < signedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPLT (.unit) src1 (scst5), src2 (xsint), dst (usint) -- .unit = .L1, .L2 op field: 56 pipeline phase E1
    void Tms320C6xIss::op_cmplt_l_56_e1(InstructionState * instState) {
      int32_t signedCst1, signedSrc2x;
      uint32_t unsignedDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (signedCst1 < signedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPLT (.unit) src1 (xsint), src2 (slong), dst (usint) -- .unit = .L1, .L2 op field: 55 pipeline phase E1
    void Tms320C6xIss::op_cmplt_l_55_e1(InstructionState * instState) {
      int32_t signedSrc1x; 
      int64_t signedLongSrc2; 
      uint32_t unsignedDst;
      uint64_t tmplong;
      signedSrc1x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      unsignedDst = (signedSrc1x < signedLongSrc2) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPLT (.unit) src1 (scst5), src2 (slong), dst (usint) -- .unit = .L1, .L2 op field: 55 pipeline phase E1
    void Tms320C6xIss::op_cmplt_l_54_e1(InstructionState * instState) {
      int32_t signedCst1; 
      int64_t signedLongSrc2; 
      uint32_t unsignedDst;
      uint64_t tmplong;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      unsignedDst = (signedCst1 < signedLongSrc2) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPLTU (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 5f pipeline phase E1
    void Tms320C6xIss::op_cmpltu_l_5f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedDst;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1];
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (unsignedSrc1 < unsignedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPLTU (.unit) src1 (ucst5), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 5e pipeline phase E1
    void Tms320C6xIss::op_cmpltu_l_5e_e1(InstructionState * instState) {
      uint32_t unsignedCst1, unsignedSrc2x, unsignedDst;
      unsignedCst1 = instState->ins.l.src1;
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = (unsignedCst1 < unsignedSrc2x) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPLTU (.unit) src1 (xusint), src2 (uslong), dst (usint) -- .unit = .L1, .L2 op field: 5d pipeline phase E1
    void Tms320C6xIss::op_cmpltu_l_5d_e1(InstructionState * instState) {
      uint32_t unsignedSrc1x; 
      uint64_t unsignedLongSrc2; 
      uint32_t unsignedDst;
      unsignedSrc1x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      unsignedLongSrc2 = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      unsignedLongSrc2 = unsignedLongSrc2  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      unsignedDst = (unsignedSrc1x < unsignedLongSrc2) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // CMPLTU (.unit) src1 (ucst5), src2 (uslong), dst (usint) -- .unit = .L1, .L2 op field: 5d pipeline phase E1
    void Tms320C6xIss::op_cmpltu_l_5c_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      uint64_t unsignedLongSrc2; 
      uint32_t unsignedDst;
      unsignedCst1 = instState->ins.l.src1;
      unsignedLongSrc2 = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      unsignedLongSrc2 = unsignedLongSrc2  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      unsignedDst = (unsignedCst1 < unsignedLongSrc2) ? 1 : 0;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }


    // LMBD (.unit) src1 (uint), src2 (xuint), dst (uint) -- .unit = .L1, .L2 op field: 6b pipeline phase E1
    void Tms320C6xIss::op_lmbd_l_6b_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedDst; 
      int32_t tmpcount; 
      uint8_t tmp;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1];
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      tmp = unsignedSrc1 & 0x00000001;
      for (tmpcount = 31 ; tmpcount >= 0 ; tmpcount-- ) {
	if (((unsignedSrc2x >> tmpcount) & 0x00000001) == tmp)
	  break;
      }
      unsignedDst = 31 - tmpcount;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // LMBD (.unit) src1 (cst5), src2 (xuint), dst (uint) -- .unit = .L1, .L2 op field: 6a pipeline phase E1
    void Tms320C6xIss::op_lmbd_l_6a_e1(InstructionState * instState) {
      uint32_t unsignedCst1, unsignedSrc2x, unsignedDst; 
      int32_t tmpcount; 
      uint8_t tmp;
      unsignedCst1 = instState->ins.l.src1;
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      tmp = unsignedCst1 & 0x00000001;
      for (tmpcount = 31 ; tmpcount >= 0 ; tmpcount-- ) {
	if (((unsignedSrc2x >> tmpcount) & 0x00000001) == tmp)
	  break;
      }
      unsignedDst = 31 - tmpcount;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }


    // NORM(.unit) src1 (xsint), dst (uint) -- .unit = .L1, .L2 op field: 63 pipeline phase E1
    void Tms320C6xIss::op_norm_l_63_e1(InstructionState * instState) {
      int32_t signedSrc2x; 
      uint32_t unsignedDst; 
      int32_t tmpcount; 
      uint8_t tmp;
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      tmp = (signedSrc2x >> 31) & 0x00000001;
      tmp = !tmp;
      for (tmpcount = 31 ; tmpcount >= 0 ; tmpcount-- ) {
	if (((signedSrc2x >> tmpcount) & 0x00000001) == tmp)
	  break;
      }
      unsignedDst = 31 - tmpcount - 1;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // NORM (.unit) src1 (slong), dst (uint) -- .unit = .L1, .L2 op field: 60 pipeline phase E1
    void Tms320C6xIss::op_norm_l_60_e1(InstructionState * instState) {
      int64_t signedLongSrc2; 
      uint32_t unsignedDst; 
      int32_t tmpcount; 
      uint8_t tmp;
      uint64_t tmplong;
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      tmp = (signedLongSrc2 >> 39) & 0x00000001;
      tmp = !tmp;
      for (tmpcount = 39 ; tmpcount >= 0 ; tmpcount-- ) {
	if (((signedLongSrc2 >> tmpcount) & 0x00000001) == tmp)
	  break;
      }
//       std::cout << " tmpcount : " << tmpcount <<  std::endl;
      unsignedDst = 39 - tmpcount - 1;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // OR (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 7f pipeline phase E1
    void Tms320C6xIss::op_or_l_7f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedDst;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1];
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = unsignedSrc1 | unsignedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // OR (.unit) src1 (scst5), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 7e pipeline phase E1
    void Tms320C6xIss::op_or_l_7e_e1(InstructionState * instState) {
      int32_t signedCst1;
      uint32_t unsignedSrc2x, unsignedDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = signedCst1 | unsignedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // SADD (.unit) src1 (sint), src2 (xsint), dst (sint) -- .unit = .L1, .L2 op field: 13 pipeline phase E1
    void Tms320C6xIss::op_sadd_l_13_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedDst; 
      int64_t signedLongDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedLongDst = ((int64_t) signedSrc1) + ((int64_t) signedSrc2x);
      if (signedLongDst > 0x7fffffff) {
	signedDst = 0x7fffffff;
	state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200; }
      else if (signedLongDst < -(0x8000000)) {
	signedDst = -(0x80000000);
	state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200; }
      else
	signedDst = (int32_t) signedLongDst;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SADD (.unit) src1 (xsint), src2 (slong), dst (slong) -- .unit = .L1, .L2 op field: 31 pipeline phase E1
    void Tms320C6xIss::op_sadd_l_31_e1(InstructionState * instState) {
      int32_t signedSrc1x; 
      int64_t signedLongSrc2, signedLongDst;
      uint64_t tmplong;
      signedSrc1x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      signedLongDst = ((int64_t) signedSrc1x) + signedLongSrc2;
      if (signedLongDst > INT64_MAX) {
	signedLongDst = INT64_MAX;
	state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200; }
      else if (signedLongDst < INT64_MIN) {
	signedLongDst = INT64_MIN;
	state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200; }
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // SADD (.unit) src1 (scst5), src2 (xsint), dst (sint) -- .unit = .L1, .L2 op field: 12 pipeline phase E1
    void Tms320C6xIss::op_sadd_l_12_e1(InstructionState * instState) {
      int32_t signedCst1, signedSrc2x, signedDst; 
      int64_t signedLongDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedLongDst = ((int64_t) signedCst1) + ((int64_t) signedSrc2x);
      if (signedLongDst > 0x7fffffff) {
	signedDst = 0x7fffffff;			
	state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200; }
      else if (signedLongDst < -(0x8000000)) {
	signedDst = -(0x80000000) ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200; }
      else
	signedDst = (int32_t) signedLongDst;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SADD (.unit) src1 (scst5), src2 (slong), dst (slong) -- .unit = .L1, .L2 op field: 30 pipeline phase E1
    void Tms320C6xIss::op_sadd_l_30_e1(InstructionState * instState) {
      int64_t signedLongCst1, signedLongSrc2, signedLongDst;
      uint32_t tmplong;
      signedLongCst1 = SIGN_EXTEND_LONG((int64_t) instState->ins.l.src1, 5);
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      signedLongDst = signedLongCst1 + signedLongSrc2;
      if (signedLongDst > INT64_MAX) {
	signedLongDst = INT64_MAX;
	state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200; }
      else if (signedLongDst < INT64_MIN) {
	signedLongDst = INT64_MIN;
	state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200; }
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // SAT (.unit) src2 (slong), dst (sint) -- .unit = .L1, .L2 op field: 40 pipeline phase E1
    void Tms320C6xIss::op_sat_l_40_e1(InstructionState * instState) {
      int64_t signedLongSrc2; 
      int32_t signedDst;
      uint64_t tmplong;
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      if (signedLongSrc2 > 0x7fffffff)
	signedDst = 0x7fffffff ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else if (signedLongSrc2 < -(0x80000000))
	signedDst = -(0x80000000) ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else
	signedDst = signedLongSrc2 & 0xffffffff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SSUB (.unit) src1 (sint), src2 (xsint), dst (sint) -- .unit = .L1, .L2 op field: 0f pipeline phase E1
    void Tms320C6xIss::op_ssub_l_0f_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x; 
      int64_t signedLongDst; 
      int32_t signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedLongDst = ((int64_t) signedSrc1) - ((int64_t) signedSrc2x);
      if (signedLongDst > 0x7fffffff)
	signedDst = 0x7fffffff ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else if (signedLongDst < -(0x8000000))
	signedDst = -(0x80000000) ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else
	signedDst = (int32_t) signedLongDst;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SSUB (.unit) src1 (xsint), src2 (sint), dst (sint) -- .unit = .L1, .L2 op field: 1f pipeline phase E1
    void Tms320C6xIss::op_ssub_l_1f_e1(InstructionState * instState) {
      int32_t signedSrc1x, signedSrc2; 
      int64_t signedLongDst; 
      int32_t signedDst;
      signedSrc1x = (int32_t) (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      signedSrc2 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongDst = ((int64_t) signedSrc1x) - ((int64_t) signedSrc2);
      if (signedLongDst > 0x7fffffff)
	signedDst = 0x7fffffff ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else if (signedLongDst < -(0x8000000))
	signedDst = -(0x80000000) ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else
	signedDst = (int32_t) signedLongDst;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SSUB (.unit) src1 (scst5), src2 (xsint), dst (sint) -- .unit = .L1, .L2 op field: 0e pipeline phase E1
    void Tms320C6xIss::op_ssub_l_0e_e1(InstructionState * instState) {
      int32_t signedCst1, signedSrc2x; 
      int64_t signedLongDst; 
      int32_t signedDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedLongDst = ((int64_t) signedCst1) - ((int64_t) signedSrc2x);
      if (signedLongDst > 0x7fffffff)
	signedDst = 0x7fffffff ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else if (signedLongDst < -(0x8000000))
	signedDst = -(0x80000000) ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else
	signedDst = (int32_t) signedLongDst;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SSUB (.unit) src1 (scst5), src2 (slong), dst (slong) -- .unit = .L1, .L2 op field: 2c pipeline phase E1
    void Tms320C6xIss::op_ssub_l_2c_e1(InstructionState * instState) {
      int64_t signedLongCst1, signedLongSrc2, signedLongDst;
      uint64_t tmplong;
      signedLongCst1 = SIGN_EXTEND_LONG((int64_t) instState->ins.l.src1, 5);
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      signedLongDst = signedLongCst1 - signedLongSrc2;
      if (signedLongDst > INT64_MAX)
	signedLongDst = INT64_MAX ,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      else if (signedLongDst < INT64_MIN)
	signedLongDst = INT64_MIN,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // SSUB (.unit) src1 (sint), src2 (xsint), dst (sint) -- .unit = .L1, .L2 op field: 07 pipeline phase E1
    void Tms320C6xIss::op_sub_l_07_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedDst = ((int64_t) signedSrc1) - ((int64_t) signedSrc2x);
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SSUB (.unit) src1 (xsint), src2 (sint), dst (sint) -- .unit = .L1, .L2 op field: 17 pipeline phase E1
    void Tms320C6xIss::op_sub_l_17_e1(InstructionState * instState) {
      int32_t signedSrc1x, signedSrc2, signedDst;
      signedSrc1x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      signedSrc2 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedDst = ((int64_t) signedSrc1x) - ((int64_t) signedSrc2);
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SSUB (.unit) src1 (sint), src2 (xsint), dst (slong) -- .unit = .L1, .L2 op field: 27 pipeline phase E1
    void Tms320C6xIss::op_sub_l_27_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x;
      int64_t signedLongDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedLongDst = ((int64_t) signedSrc1) - ((int64_t) signedSrc2x);
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // SSUB (.unit) src1 (xsint), src2 (sint), dst (slong) -- .unit = .L1, .L2 op field: 37 pipeline phase E1
    void Tms320C6xIss::op_sub_l_37_e1(InstructionState * instState) {
      int32_t signedSrc1x, signedSrc2; 
      int64_t signedLongDst;
      signedSrc1x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      signedSrc2 = (int32_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongDst = ((int64_t) signedSrc1x) - ((int64_t) signedSrc2);
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // SUBU (.unit) src1 (usint), src2 (xusint), dst (uslong) -- .unit = .L1, .L2 op field: 2f pipeline phase E1
    void Tms320C6xIss::op_subu_l_2f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x; 
      uint64_t unsignedLongDst;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1];
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedLongDst = ((uint64_t) unsignedSrc1) - ((uint64_t) unsignedSrc2x);
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = unsignedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = unsignedLongDst;
    }

    // SUBU (.unit) src1 (xusint), src2 (usint), dst (uslong) -- .unit = .L1, .L2 op field: 3f pipeline phase E1
    void Tms320C6xIss::op_subu_l_3f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1x, unsignedSrc2; 
      uint64_t unsignedLongDst;
      unsignedSrc1x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src1];
      unsignedSrc2 = state.regfile[instState->ins.l.s][instState->ins.l.src2];
      unsignedLongDst = ((uint64_t) unsignedSrc1x) - ((uint64_t) unsignedSrc2);
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = unsignedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = unsignedLongDst;
    }

    // SUB (.unit) src1 (scst5), src2 (xsint), dst (sint) -- .unit = .L1, .L2 op field: 06 pipeline phase E1
    void Tms320C6xIss::op_sub_l_06_e1(InstructionState * instState) {
      int32_t signedCst1, signedSrc2x, signedDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      signedDst = signedCst1 - signedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  signedDst;
    }

    // SUB (.unit) src1 (scst5), src2 (slong), dst (slong) -- .unit = .L1, .L2 op field: 24 pipeline phase E1
    void Tms320C6xIss::op_sub_l_24_e1(InstructionState * instState) {
      int64_t signedLongCst1, signedLongSrc2, signedLongDst;
      uint64_t tmplong;
      signedLongCst1 = SIGN_EXTEND_LONG((int64_t) instState->ins.l.src1, 5);
      tmplong = ((uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.l.s][instState->ins.l.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      signedLongDst = signedLongCst1 - signedLongSrc2;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] = signedLongDst;
    }

    // SUBC (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 4b pipeline phase E1
    void Tms320C6xIss::op_subc_l_4b_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedDst;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1];
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      if (((int) (unsignedSrc1 - unsignedSrc2x)) >= 0)
	unsignedDst = ((unsignedSrc1 - unsignedSrc2x) << 1) +1;
      else
	unsignedDst = unsignedSrc1 << 1;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
//       std::cout << " src1: " << unsignedSrc1 << " src2: " << unsignedSrc2x << " dst: " << unsignedDst << std::endl;
    }

    // XOR (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 24 pipeline phase E1
    void Tms320C6xIss::op_xor_l_6f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedDst;
      unsignedSrc1 = state.regfile[instState->ins.l.s][instState->ins.l.src1];
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = unsignedSrc1 ^ unsignedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }

    // XOR (.unit) src1 (scst5), src2 (xusint), dst (usint) -- .unit = .L1, .L2 op field: 24 pipeline phase E1
    void Tms320C6xIss::op_xor_l_6e_e1(InstructionState * instState) {
      int32_t signedCst1; 
      uint32_t unsignedSrc2x, unsignedDst;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.l.src1, 5);
      unsignedSrc2x = state.regfile[instState->ins.l.s ^ instState->ins.l.x][instState->ins.l.src2];
      unsignedDst = signedCst1 ^ unsignedSrc2x;
      state.regfile_temp[instState->ins.l.s][instState->ins.l.dst] =  unsignedDst;
    }


    void Tms320C6xIss::op_illegal_l_e1(InstructionState * instState) {
      std::cerr << "Warning: unknown instruction with opcode "<< std::hex << instState->ins.l.op << std::dec 
		<< " in .L unit in E1 pipeline phase @ " << m_exec_cycles <<  " cycle, instruction is " 
		<< std::hex << instState->ins.ins 
		<< " PC is " << state.DCE1.getPC() << std::dec << std::endl;
    }


    //
    // .S unit InstructionState set
    //
    //

    //
    // E1 phase
    //

    // ADD (.unit) src1 (sint), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 07 pipeline phase E1
    void Tms320C6xIss::op_add_s_07_e1(InstructionState * instState) {
      int32_t signedDst, signedSrc1, signedSrc2x;
      signedSrc1 = (int32_t) state.regfile[instState->ins.s.s][instState->ins.s.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst = signedSrc1 + signedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // ADD (.unit) src1 (scst5), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 06 pipeline phase E1
    void Tms320C6xIss::op_add_s_06_e1(InstructionState * instState) {
      int32_t signedDst, signedCst1, signedSrc2x;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.s.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst = signedCst1 + signedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // ADD2 (.unit) src1 (scst5), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 01 pipeline phase E1
    void Tms320C6xIss::op_add2_s_01_e1(InstructionState * instState) {
      int32_t signedDst, signedSrc1, signedSrc2x;
      signedSrc1 = (int32_t) state.regfile[instState->ins.s.s][instState->ins.s.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst =  ((SLSB16(signedSrc1) + SLSB16(signedSrc2x)) & 0xffff) | ((MSB16(signedSrc1) + MSB16(signedSrc2x)) << 16);
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // AND (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 1f pipeline phase E1
    void Tms320C6xIss::op_and_s_1f_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc1, unsignedSrc2x;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = unsignedSrc1 & unsignedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // AND (.unit) src1 (scst5), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 1e pipeline phase E1
    void Tms320C6xIss::op_and_s_1e_e1(InstructionState * instState) {
      uint32_t unsignedDst, signedCst1, unsignedSrc2x;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.s.src1, 5);
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = signedCst1 & unsignedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // Branch Using a Register B (.unit) src2 .unit = .S2
    void Tms320C6xIss::op_b_s_0d_e1(InstructionState * instState) {
      uint32_t unsignedSrc2x;
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      state.PG.setBranchAddress(unsignedSrc2x);
      state.PG.setBranchFlag(true);
    }

    // Branch Using an Interrupt Return Pointer -- B (.unit) IRP .unit = .S2
    // Branch Using NMI Return Pointer          -- B (.unit) NRP .unit = .S2
    void Tms320C6xIss::op_b_s_03_e1(InstructionState * instState) {
      if (instState->ins.s.src2  == 0x06) {
	// Branch Using an IRP - an Interrupt Return Pointer
	state.PG.setBranchAddress(state.c_regfile[IRP]);
	state.PG.setBranchFlag(true);
	// PGIE is copied to GIE
	state.c_regfile[CSR] = (state.c_regfile[CSR] & 0xfffffffe) | ((state.c_regfile[CSR] >> 1) & 0x00000001);
	//PGPS.branch_delay = 1;
      } else if (instState->ins.s.src2 == 0x07) {
	// Branch using NMI Return Pointer & set NMIE in state.c_regfile[IER]
	state.PG.setBranchAddress(state.c_regfile[NRP]);
	state.PG.setBranchFlag(true);
	state.c_regfile[IER] = state.c_regfile[IER] | 0x00000002;
      }
    }


    // CLR Clear Bit Fields of src2 whose bounds are given by 5-9 & 0-4 of src1 (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 3f pipeline phase E1
    void Tms320C6xIss::op_clr_s_3f_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc1, unsignedSrc2x; 
      int32_t tmpcount;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = unsignedSrc2x;
      for (tmpcount  = ((unsignedSrc1 >> 5) & 0x1f);
	   tmpcount <= (int32_t) ((unsignedSrc1 >> 0) & 0x1f);
	   tmpcount++)
	unsignedDst = unsignedDst & clearbit[tmpcount];
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // EXT Extract & Sign Extend a bit field* (.unit) src1 (usint), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 2f pipeline phase E1
    void Tms320C6xIss::op_ext_s_2f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1;
      int32_t signedDst, signedSrc2x;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst = ( signedSrc2x << ((unsignedSrc1 >> 5) & 0x1f)) >> (unsignedSrc1 & 0x1f);
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // EXT Extract & Zeo Extend a bit field* (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 2f pipeline phase E1
    void Tms320C6xIss::op_extu_s_2b_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc1,  unsignedSrc2x;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = ( unsignedSrc2x << ((unsignedSrc1 >> 5) & 0x1f)) >> (unsignedSrc1 & 0x1f);
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // Move between Control File & Register File (.unit) src2 (usint), dst (usint) -- .unit = .S1, .S2 op field: 0f pipeline phase E1
    void Tms320C6xIss::op_mvu_s_0f_e1(InstructionState * instState) {
      uint32_t unsignedDst;
      unsignedDst = state.c_regfile[instState->ins.s.src2];
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // Move between Control File & Register File (.unit) src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 0e pipeline phase E1
    void Tms320C6xIss::op_mvc_s_0e_e1(InstructionState * instState) {
      uint32_t unsignedSrc2x;
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      state.c_regfile[instState->ins.s.dst] = unsignedSrc2x;
    }

    // Bitwise OR (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 1b pipeline phase E1
    void Tms320C6xIss::op_or_s_1b_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc1, unsignedSrc2x;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = unsignedSrc1 | unsignedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // Bitwise OR (.unit) src1 (scst5), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 1a pipeline phase E1
    void Tms320C6xIss::op_or_s_1a_e1(InstructionState * instState) {
      int32_t signedCst1;
      uint32_t unsignedDst, unsignedSrc2x;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.s.src1, 5);
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = signedCst1 | unsignedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // ET Set Bit Fields of src2 whose bounds are given by 5-9 & 0-4 of src1  (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 3b pipeline phase E1
    void Tms320C6xIss::op_set_s_3b_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc1, unsignedSrc2x; 
      int32_t tmpcount;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = unsignedSrc2x;
      for (tmpcount  = ((unsignedSrc1 >> 5) & 0x1f);
	   tmpcount <= (int32_t) ((unsignedSrc1 >> 0) & 0x1f);
	   tmpcount++)
	unsignedDst = unsignedDst | (0x01 << tmpcount);
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (usint), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 33 pipeline phase E1
    void Tms320C6xIss::op_shl_s_33_e1(InstructionState * instState) {
      uint32_t unsignedSrc1;
      int32_t signedDst, signedSrc2x; 
      uint32_t unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedTmp = unsignedSrc1 & 0x3f;
      unsignedTmp = (unsignedTmp < 40) ? unsignedTmp : 40;
      signedDst = signedSrc2x << unsignedTmp;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (usint), src2 (slong), dst (slong) -- .unit = .S1, .S2 op field: 31 pipeline phase E1
    void Tms320C6xIss::op_shl_s_31_e1(InstructionState * instState) {
      int64_t signedLongSrc2, signedLongDst; 
      uint32_t unsignedSrc1;
      uint32_t unsignedTmp;
      uint32_t tmplong;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      tmplong = ((uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      unsignedTmp = unsignedSrc1 & 0x3f;
      unsignedTmp = (unsignedTmp <= 40) ? unsignedTmp : 40;
      signedLongDst = signedLongSrc2 << unsignedTmp;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] = signedLongDst;
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (usint), src2 (xusint), dst (uslong) -- .unit = .S1, .S2 op field: 13 pipeline phase E1
    void Tms320C6xIss::op_shl_s_13_e1(InstructionState * instState) {
      uint64_t unsignedLongDst; 
      uint32_t unsignedSrc1, unsignedSrc2x; 
      uint32_t unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedTmp = unsignedSrc1 & 0x3f;
      unsignedTmp = (unsignedTmp < 40) ? unsignedTmp : 40;
      unsignedLongDst = ((uint64_t) unsignedSrc2x) << unsignedTmp; 
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst+1] = unsignedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] = unsignedLongDst;
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (ucst5), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 32 pipeline phase E1
    void Tms320C6xIss::op_shl_s_32_e1(InstructionState * instState) {
      uint32_t unsignedCst1;
      int32_t signedDst, signedSrc2x;
      unsignedCst1 = instState->ins.s.src1;
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst = signedSrc2x << unsignedCst1;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " shl operand: " << unsignedCst1 << " " <<  signedSrc2x 
         << " shl instruction result: " << signedDst << std::dec << " (" << (int) signedDst << ")" << std::endl;
#endif
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (ucst5), src2 (slong), dst (slong) -- .unit = .S1, .S2 op field: 30 pipeline phase E1
    void Tms320C6xIss::op_shl_s_30_e1(InstructionState * instState) {
      int64_t signedLongDst, signedLongSrc2; 
      uint32_t unsignedCst1; 
      uint32_t tmplong;
      unsignedCst1 = instState->ins.s.src1;
      tmplong = ((uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      signedLongDst = signedLongSrc2 << unsignedCst1;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] = signedLongDst;
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (ucst5), src2 (xusint), dst (uslong) -- .unit = .S1, .S2 op field: 12 pipeline phase E1
    void Tms320C6xIss::op_shl_s_12_e1(InstructionState * instState) {
      uint64_t unsignedLongDst; 
      uint32_t unsignedCst1, unsignedSrc2x;
      unsignedCst1 = instState->ins.s.src1;
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedLongDst = ((uint64_t) unsignedSrc2x) << unsignedCst1;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst+1] = unsignedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] = unsignedLongDst;
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (usint), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 37 pipeline phase E1
    void Tms320C6xIss::op_shr_s_37_e1(InstructionState * instState) {
      uint32_t  unsignedSrc1 ;
      int32_t signedDst, signedSrc2x; 
      uint32_t unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedTmp = unsignedSrc1 & 0x3f;
      unsignedTmp = (unsignedTmp < 40) ? unsignedTmp : 40;
      signedDst = signedSrc2x >> unsignedTmp;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (usint), src2 (slong), dst (slong) -- .unit = .S1, .S2 op field: 35 pipeline phase E1
    void Tms320C6xIss::op_shr_s_35_e1(InstructionState * instState) {
      int64_t signedLongDst, signedLongSrc2; 
      uint32_t unsignedSrc1;
      uint32_t unsignedTmp;
      uint32_t tmplong;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      tmplong = ((uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      unsignedTmp = unsignedSrc1 & 0x3f;
      unsignedTmp = (unsignedTmp < 40) ? unsignedTmp : 40;
      signedLongDst = signedLongSrc2 >> unsignedTmp;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] = signedLongDst;
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (ucst5), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 36 pipeline phase E1
    void Tms320C6xIss::op_shr_s_36_e1(InstructionState * instState) {
      uint32_t unsignedCst1;
      int32_t signedDst, signedSrc2x;
      unsignedCst1 = instState->ins.s.src1;
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst = signedSrc2x >> unsignedCst1;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " shr operand: " 
	 << unsignedCst1 << " (" << std::dec << (int) unsignedCst1 << ") " << std::hex
	 << signedSrc2x << " (" << std::dec << (int) signedSrc2x << ") " << std::hex
         << " shr instruction result: " << signedDst << std::dec << " (" << (int) signedDst << ")" << std::endl;
#endif
    }

    // SHL Shift Left by amount given in src1  (.unit) src1 (ucst5), src2 (slong), dst (slong) -- .unit = .S1, .S2 op field: 34 pipeline phase E1
    void Tms320C6xIss::op_shr_s_34_e1(InstructionState * instState) {
      int64_t signedLongDst, signedLongSrc2;
      uint32_t unsignedCst1;
      uint32_t tmplong;
      unsignedCst1 = instState->ins.s.src1;
      tmplong = ((uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2+1] & 0xff) << 32;
      tmplong = tmplong  | (uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2];
      signedLongSrc2 = SIGN_EXTEND_LONG((int64_t) tmplong, 40);
      signedLongDst = signedLongSrc2 >> unsignedCst1;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst+1] = signedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] = signedLongDst;
    }

    // SHRU Shift Right by amount given in src1  (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 27 pipeline phase E1
    void Tms320C6xIss::op_shru_s_27_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc1, unsignedSrc2x; 
      uint32_t unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedTmp = unsignedSrc1 & 0x3f;
      unsignedTmp = (unsignedTmp < 40) ? unsignedTmp : 40;
      unsignedDst = unsignedSrc2x >> unsignedTmp;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // SHRU Shift Right by amount given in src1  (.unit) src1 (usint), src2 (uslong), dst (uslong) -- .unit = .S1, .S2 op field: 25 pipeline phase E1
    void Tms320C6xIss::op_shru_s_25_e1(InstructionState * instState) {
      uint64_t unsignedLongDst, unsignedLongSrc2; 
      uint32_t unsignedSrc1; 
      uint32_t unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedLongSrc2 = ((uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2+1] & 0xff) << 32;
      unsignedLongSrc2 = unsignedLongSrc2 | (uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2];
      unsignedTmp = unsignedSrc1 & 0x3f;
      unsignedTmp = (unsignedTmp < 40) ? unsignedTmp : 40;
      unsignedLongDst = unsignedLongSrc2 >> unsignedTmp;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst+1] = unsignedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] = unsignedLongDst;
    }

    // SHRU Shift Right by amount given in src1  (.unit) src1 (ucst5), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 65 pipeline phase E1
    void Tms320C6xIss::op_shru_s_26_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedCst1, unsignedSrc2x;
      unsignedCst1 = instState->ins.s.src1;
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = unsignedSrc2x >> unsignedCst1;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // SHRU Shift Right by amount given in src1  (.unit) src1 (ucst5), src2 (uslong), dst (uslong) -- .unit = .S1, .S2 op field: 24 pipeline phase E1
    void Tms320C6xIss::op_shru_s_24_e1(InstructionState * instState) {
      uint64_t unsignedLongDst, unsignedLongSrc2; 
      uint32_t unsignedCst1;
      unsignedCst1 = instState->ins.s.src1;
      unsignedLongSrc2 = ((uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2+1] & 0xff) << 32;
      unsignedLongSrc2 = unsignedLongSrc2 | (uint64_t) state.regfile[instState->ins.s.s][instState->ins.s.src2];
      unsignedLongDst = unsignedLongSrc2 >> unsignedCst1;
      // long result is written over 2 registers
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst+1] = unsignedLongDst >> 32 & 0x000000ff;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] = unsignedLongDst;
    }

    // SSHL Shift Left with Saturation  (.unit) src1 (usint), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 23 pipeline phase E1
    void Tms320C6xIss::op_sshl_s_23_e1(InstructionState * instState) {
      int32_t signedDst = 0, signedSrc2x; 
      uint32_t unsignedSrc1;
      uint32_t unsignedTmp; uint8_t tmpchar; 
      int32_t tmpcount;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedTmp = unsignedSrc1 & 0x1f;
      tmpchar = signedSrc2x >> 31 & 0x01;
      for(tmpcount=31 ; tmpcount <= (int32_t) (31-unsignedTmp) ; tmpcount--)
	if (((signedSrc2x >> tmpcount) & 0x01) != tmpchar)
	  break;
      if ((uint32_t) tmpcount == (31-unsignedTmp-1))
	signedDst = signedSrc2x << unsignedTmp;
      else if (signedSrc2x > 0)
	signedDst = 0x7fffffff;
      else if (signedSrc2x < 0)
	signedDst = 0x80000000;

      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // SSHL Shift Left with Saturation  (.unit) src1 (ucst5), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 22 pipeline phase E1
    void Tms320C6xIss::op_sshl_s_22_e1(InstructionState * instState) {
      int32_t signedDst = 0, signedSrc2x; 
      uint32_t unsignedCst1; 
      uint32_t unsignedTmp; 
      uint8_t tmpchar; 
      int32_t tmpcount;
      unsignedCst1 = instState->ins.s.src1;
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedTmp = unsignedCst1;
      tmpchar = signedSrc2x >> 31 & 0x01;
      for(tmpcount=31 ; tmpcount <= (int32_t) (31-unsignedTmp) ; tmpcount--)
	if (((signedSrc2x >> tmpcount) & 0x01) != tmpchar)
	  break;
      if ((uint32_t) tmpcount == (31-unsignedTmp-1))
	signedDst = signedSrc2x << unsignedTmp;
      else if (signedSrc2x > 0)
	signedDst = 0x7fffffff;
      else if (signedSrc2x < 0)
	signedDst = 0x80000000;

      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // SUB without saturation  (.unit) src1 (sint), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 17 pipeline phase E1
    void Tms320C6xIss::op_sub_s_17_e1(InstructionState * instState) {
      int32_t signedDst, signedSrc1, signedSrc2x;
      signedSrc1 = (int32_t) state.regfile[instState->ins.s.s][instState->ins.s.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst = signedSrc1 - signedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // SUB without saturation  (.unit) src1 (scst5), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 16 pipeline phase E1
    void Tms320C6xIss::op_sub_s_16_e1(InstructionState * instState) {
      int32_t signedDst, signedCst1, signedSrc2x;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.s.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst = signedCst1 - signedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // SUB2 Subtractions of lower and upper halfs  (.unit) src1 (sint), src2 (xsint), dst (sint) -- .unit = .S1, .S2 op field: 11 pipeline phase E1
    void Tms320C6xIss::op_sub2_s_11_e1(InstructionState * instState) {
      int32_t signedDst, signedSrc1, signedSrc2x;
      signedSrc1 = (int32_t) state.regfile[instState->ins.s.s][instState->ins.s.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      signedDst =  ((SLSB16(signedSrc1) - SLSB16(signedSrc2x)) & 0xffff) |
	((MSB16(signedSrc1) - MSB16(signedSrc2x)) << 16);
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  signedDst;
    }

    // XOR (.unit) src1 (usint), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 0b pipeline phase E1
    void Tms320C6xIss::op_xor_s_0b_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc1, unsignedSrc2x;
      unsignedSrc1 = state.regfile[instState->ins.s.s][instState->ins.s.src1];
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = unsignedSrc1 ^ unsignedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    // XOR (.unit) src1 (scst5), src2 (xusint), dst (usint) -- .unit = .S1, .S2 op field: 0a pipeline phase E1
    void Tms320C6xIss::op_xor_s_0a_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc2x; 
      int32_t signedCst1;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.s.src1, 5);
      unsignedSrc2x = state.regfile[instState->ins.s.s ^ instState->ins.s.x][instState->ins.s.src2];
      unsignedDst = signedCst1 ^ unsignedSrc2x;
      state.regfile_temp[instState->ins.s.s][instState->ins.s.dst] =  unsignedDst;
    }

    void Tms320C6xIss::op_illegal_s_e1(InstructionState * instState) {
      std::cerr << "Warning: unknown instruction with opcode "<< std::hex << instState->ins.s.op << std::dec 
		<< " in .S unit in E1 pipeline phase @ " << m_exec_cycles <<  " cycle, instruction is " 
		<< std::hex << instState->ins.ins 
		<< " PC is " << state.DCE1.getPC() << std::dec << std::endl;
    }


    // ADDK Add Signed 16-Bit Constant to Register ADDK (.unit) cst (cst16), dst (usint) .unit = .S1 or .S2 op field: a pipeline phase E1
    void Tms320C6xIss::op_addk_s_addk_e1(InstructionState * instState) {
      int32_t signedTmp; 
      uint32_t unsignedDst;
      signedTmp = (int32_t) instState->ins.s_addk.cst;
      signedTmp = SIGN_EXTEND(signedTmp,16);
      unsignedDst = state.regfile[instState->ins.s_addk.s][instState->ins.s_addk.dst] + signedTmp;
      state.regfile_temp[instState->ins.s.s][instState->ins.s_addk.dst] =  unsignedDst;
    }

    // Branch Using a Displacement (.unit) label .unit = .S1 or .S2 
    void Tms320C6xIss::op_bcond_s_bcond_e1(InstructionState * instState) {
      int32_t signedTmp;
      signedTmp = (int32_t) instState->ins.s_bcond.cst; 
      signedTmp = SIGN_EXTEND(signedTmp,21); 
      signedTmp = (signedTmp << 2) + (state.DCE1.getPC() & FETCH_MASK);
      state.PG.setBranchAddress(signedTmp);
      state.PG.setBranchFlag(true);
    }

    // CLR Clear Bit Fields of src2 whose bounds are given by csta & cstb (.unit) cst (cst5), dst (usint) .unit = .S1 or .S2 op field: 03 pipeline phase E1
    void Tms320C6xIss::op_clr_s_immed_03_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc2; 
      int32_t tmpCount;
      unsignedSrc2 = state.regfile[instState->ins.s_immed.s][instState->ins.s_immed.src2];
      unsignedDst = unsignedSrc2;
      for (tmpCount  = instState->ins.s_immed.csta;
	   (uint32_t) tmpCount <= instState->ins.s_immed.cstb;
	   tmpCount++)
	unsignedDst = unsignedDst & clearbit[tmpCount];
      state.regfile_temp[instState->ins.s_immed.s][instState->ins.s_immed.dst] =  unsignedDst;
    }

    // EXT Extract & Sign Extend a bit field whose bounds are given by csta & cstb, src2 (sint), dst (sint) unit = .S1 or .S2 op field: 01 pipeline phase E1
    void Tms320C6xIss::op_ext_s_immed_01_e1(InstructionState * instState) {
      int32_t signedDst, signedSrc2;
      signedSrc2 = (int32_t) state.regfile[instState->ins.s_immed.s][instState->ins.s_immed.src2];
      signedDst = signedSrc2 << instState->ins.s_immed.csta;
      signedDst = signedDst >> instState->ins.s_immed.cstb;
      state.regfile_temp[instState->ins.s_immed.s][instState->ins.s_immed.dst] =  signedDst;
    }

    // EXTU Extract & Zero Extend a bit field whose bounds are given by csta & cstb, src2 (usint), dst (usint) unit = .S1 or .S2 op field: 00 pipeline phase E1
    void Tms320C6xIss::op_extu_s_immed_00_e1(InstructionState * instState) {
      uint32_t unsignedDst; 
      int32_t signedSrc2;
      signedSrc2 = (int32_t) state.regfile[instState->ins.s_immed.s][instState->ins.s_immed.src2];
      unsignedDst = signedSrc2 << instState->ins.s_immed.csta;
      unsignedDst = unsignedDst >> instState->ins.s_immed.cstb;
      state.regfile_temp[instState->ins.s_immed.s][instState->ins.s_immed.dst] =  unsignedDst;
    }

    // SET Set Bit Fields  whose bounds are given by csta & cstb, src2 (usint), dst (usint) unit = .S1 or .S2 op field: a pipeline phase E1
    void Tms320C6xIss::op_set_s_immed_02_e1(InstructionState * instState) {
      uint32_t unsignedDst, unsignedSrc2; 
      int32_t tmpcount;
      unsignedSrc2 = state.regfile[instState->ins.s_immed.s][instState->ins.s_immed.src2];
      unsignedDst = unsignedSrc2;
      for (tmpcount  = instState->ins.s_immed.csta;
	   (uint32_t) tmpcount <= instState->ins.s_immed.cstb;
	   tmpcount++)
	unsignedDst = unsignedDst | (0x01 << tmpcount);
      state.regfile_temp[instState->ins.s_immed.s][instState->ins.s_immed.dst] =  unsignedDst;
    }

    // MVK Move a 16bit signed constant into a Register & Sign Extend cst (scst16), dst (sint) unit = .S1 or .S2 op field: a pipeline phase E1
    void Tms320C6xIss::op_mvk_s_mvk_00_e1(InstructionState * instState) {
      int32_t signedDst;
      signedDst = (int32_t) instState->ins.s_mvk.cst;
      signedDst = SIGN_EXTEND(signedDst,16);
      state.regfile_temp[instState->ins.s_mvk.s][instState->ins.s_mvk.dst] =  signedDst;
    }

    // MVK Move a 16bit signed constant into the Upper Bits of a Register  cst (scst16), dst (sint) unit = .S1 or .S2 op field: a pipeline phase E1
    void Tms320C6xIss::op_mvkh_s_mvk_01_e1(InstructionState * instState) {
      int32_t signedDst;
      signedDst = (int32_t) state.regfile[instState->ins.s_mvk.s][instState->ins.s_mvk.dst];
      signedDst = (signedDst & 0x0000ffff) | (instState->ins.s_mvk.cst << 16);
      state.regfile_temp[instState->ins.s_mvk.s][instState->ins.s_mvk.dst] =  signedDst;

    }

    //
    //
    // .M unit InstructionState set
    //
    //

    //
    // E1 phase
    //

    // MPY Integer Multiply 16lsb x 16lsb  src1 (slsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 19 pipeline phase E1
    void Tms320C6xIss::op_mpy_m_19_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];  
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = SLSB16(signedSrc1) * SLSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYU Integer Multiply 16lsb x 16lsb  src1 (slsb16), src2 (xulsb16), dst (usint) unit = .M1 or .M2 op field: 1f pipeline phase E1
    void Tms320C6xIss::op_mpyu_m_1f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.m.s][instState->ins.m.src1]; 
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      unsignedTmp = ULSB16(unsignedSrc1) * ULSB16(unsignedSrc2x);
      instState->setResult(unsignedTmp); // write data in E2 phase
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
	 << " mpy operand: " << unsignedSrc1 << " " <<  unsignedSrc2x
	 << " mpy instruction result: " << unsignedTmp << std::dec << " (" << (int) unsignedTmp << ")" << std::endl;
#endif
    }

    // MPYUS Integer Multiply 16lsb x 16lsb  src1 (ulsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 1d pipeline phase E1
    void Tms320C6xIss::op_mpyus_m_1d_e1(InstructionState * instState) {
      uint32_t unsignedSrc1; 
      int32_t signedSrc2x, signedTmp;
      unsignedSrc1 = state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = ULSB16(unsignedSrc1) * SLSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << unsignedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYSU Integer Multiply 16lsb x 16lsb  src1 (slsb16), src2 (xulsb16), dst (sint) unit = .M1 or .M2 op field: 1b pipeline phase E1
    void Tms320C6xIss::op_mpysu_m_1b_e1(InstructionState * instState) {
      int32_t signedSrc1; 
      uint32_t unsignedSrc2x; 
      int32_t signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = SLSB16(signedSrc1) * ULSB16(unsignedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  unsignedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPY Integer Multiply 16lsb x 16lsb  src1 (scst5), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 1b pipeline phase E1
    void Tms320C6xIss::op_mpy_m_18_e1(InstructionState * instState) {
      int32_t signedCst1, signedSrc2x, signedTmp;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.m.src1, 5);
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = signedCst1 * SLSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedCst1 << " " << signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::cout << std::endl;
#endif
    }

    // MPYSU Integer Multiply 16lsb x 16lsb  src1 (scst5), src2 (xulsb16), dst (sint) unit = .M1 or .M2 op field: 1e pipeline phase E1
    void Tms320C6xIss::op_mpysu_m_1e_e1(InstructionState * instState) {
      int32_t signedCst1; 
      uint32_t unsignedSrc2x; 
      int32_t signedTmp;
      signedCst1 = SIGN_EXTEND((int32_t) instState->ins.m.src1, 5);
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = signedCst1 * ULSB16(unsignedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " <<  signedCst1 << " " << unsignedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::cout << std::endl;
#endif
    }

    // MPYH Integer Multiply 16msb x 16msb   src1 (smsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 01 pipeline phase E1
    void Tms320C6xIss::op_mpyh_m_01_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = MSB16(signedSrc1) * MSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYHU Integer Multiply 16msb x 16msb   src1 (umsb16), src2 (xumsb16), dst (sint) unit = .M1 or .M2 op field: 07 pipeline phase E1
    void Tms320C6xIss::op_mpyhu_m_07_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.m.s][instState->ins.m.src1];
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      unsignedTmp = MSB16(unsignedSrc1) * MSB16(unsignedSrc2x);
      instState->setResult(unsignedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << unsignedSrc1 << " " <<  unsignedSrc2x 
         << " mpy instruction result: " << unsignedTmp << std::dec << " (" << (int) unsignedTmp << ")" << std::endl;
#endif
    }

    // MPYHUS Integer Multiply 16msb x 16msb   src1 (umsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 05 pipeline phase E1
    void Tms320C6xIss::op_mpyhus_m_05_e1(InstructionState * instState) {
      uint32_t unsignedSrc1; 
      int32_t signedSrc2x; int32_t signedTmp;
      unsignedSrc1 = state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = MSB16(unsignedSrc1) * MSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << unsignedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYHSU Integer Multiply 16msb x 16msb   src1 (smsb16), src2 (xumsb16), dst (sint) unit = .M1 or .M2 op field: 03 pipeline phase E1
    void Tms320C6xIss::op_mpyhsu_m_03_e1(InstructionState * instState) {
      int32_t signedSrc1; 
      uint32_t unsignedSrc2x; 
      int32_t signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = MSB16(signedSrc1) * MSB16(unsignedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  unsignedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYHL Integer Multiply 16msb x 16lsb   src1 (smsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 09 pipeline phase E1
    void Tms320C6xIss::op_mpyhl_m_09_e1(InstructionState * instState) {
      int32_t signedSrc1; int32_t signedSrc2x; int32_t signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = MSB16(signedSrc1) * SLSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " 
	 << signedSrc1 << " (" << std::dec << (int) signedSrc1 << ") " << std::hex
	 << signedSrc2x << " (" << std::dec << (int) signedSrc2x << ")" << std::hex
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif 
    }

    // MPYHLU Integer Multiply 16msb x 16lsb   src1 (umsb16), src2 (xulsb16), dst (usint) unit = .M1 or .M2 op field: 0f pipeline phase E1
    void Tms320C6xIss::op_mpyhlu_m_0f_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.m.s][instState->ins.m.src1];
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      unsignedTmp = MSB16(unsignedSrc1) * ULSB16(unsignedSrc2x);
      instState->setResult(unsignedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << unsignedSrc1 << " " <<  unsignedSrc2x 
         << " mpy instruction result: " << unsignedTmp << std::dec << " (" << (int) unsignedTmp << ")" << std::endl;
#endif
    }

    // MPYHULS Integer Multiply 16msb x 16lsb   src1 (umsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 0d pipeline phase E1
    void Tms320C6xIss::op_mpyhuls_m_0d_e1(InstructionState * instState) {
      uint32_t unsignedSrc1; 
      int32_t signedSrc2x; int32_t signedTmp;
      unsignedSrc1 = state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = MSB16(unsignedSrc1) * SLSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << unsignedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYHSLU Integer Multiply 16msb x 16lsb   src1 (smsb16), src2 (smsb16), dst (sint) unit = .M1 or .M2 op field: 0b pipeline phase E1
    void Tms320C6xIss::op_mpyhslu_m_0b_e1(InstructionState * instState) {
      int32_t signedSrc1; 
      uint32_t unsignedSrc2x; 
      int32_t signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = MSB16(signedSrc1) * ULSB16(unsignedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  unsignedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYLH Integer Multiply 16lsb x 16msb   src1 (slsb16), src2 (slsb16), dst (sint) unit = .M1 or .M2 op field: 11 pipeline phase E1
    void Tms320C6xIss::op_mpylh_m_11_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = SLSB16(signedSrc1) * MSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " 
	 << signedSrc1 << " (" << std::dec << (int) signedSrc1 << ") " << std::hex
	 <<  signedSrc2x  <<  "(" << std::dec << (int) signedSrc2x << ")" << std::hex
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYLH Integer Multiply 16lsb x 16msb   src1 (ulsb16), src2 (xumsb16), dst (usint) unit = .M1 or .M2 op field: 17 pipeline phase E1
    void Tms320C6xIss::op_mpylhu_m_17_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, unsignedSrc2x, unsignedTmp;
      unsignedSrc1 = state.regfile[instState->ins.m.s][instState->ins.m.src1];
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      unsignedTmp = ULSB16(unsignedSrc1) * MSB16(unsignedSrc2x);
      instState->setResult(unsignedTmp); // write data in E2 phase  
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << unsignedSrc1 << " " <<  unsignedSrc2x 
         << " mpy instruction result: " << unsignedTmp << std::dec << " (" << (int) unsignedTmp << ")" << std::endl;
#endif
   }

    // MPYLUHS Integer Multiply 16lsb x 16msb   src1 (ulsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 15 pipeline phase E1
    void Tms320C6xIss::op_mpyluhs_m_15_e1(InstructionState * instState) {
      uint32_t unsignedSrc1; 
      int32_t signedSrc2x, signedTmp;
      unsignedSrc1 = state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = ULSB16(unsignedSrc1) * MSB16(signedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << unsignedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // MPYLUHS Integer Multiply 16lsb x 16msb   src1 (16msb), src2 (xumsb16), dst (sint) unit = .M1 or .M2 op field: 13 pipeline phase E1
    void Tms320C6xIss::op_mpylshu_m_13_e1(InstructionState * instState) {
      int32_t signedSrc1; 
      uint32_t unsignedSrc2x; 
      int32_t signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      unsignedSrc2x = state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = SLSB16(signedSrc1) * MSB16(unsignedSrc2x);
      instState->setResult(signedTmp); // write data in E2 phase 
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  unsignedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // SMPY Integer Multiply 16lsb x 16lsb   src1 (slsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 1a pipeline phase E1
    void Tms320C6xIss::op_smpy_m_1a_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = ( SLSB16(signedSrc1) * SLSB16(signedSrc2x) ) << 1;
      if ((uint32_t) signedTmp == 0x80000000)
	signedTmp = 0x7fffffff,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      instState->setResult(signedTmp);
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // SMPYHL Integer Multiply 16msb x 16lsb   src1 (smsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 0a pipeline phase E1
    void Tms320C6xIss::op_smpyhl_m_0a_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = ( MSB16(signedSrc1) * SLSB16(signedSrc2x) ) << 1;
      if ((uint32_t) signedTmp == 0x80000000)
	signedTmp = 0x7fffffff,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      instState->setResult(signedTmp);
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    // SMPYLH Integer Multiply 16lsb x 16msb   src1 (slsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 12 pipeline phase E1
    void Tms320C6xIss::op_smpylh_m_12_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = ( SLSB16(signedSrc1) * MSB16(signedSrc2x) ) << 1;
      if ((uint32_t) signedTmp == 0x80000000)
	signedTmp = 0x7fffffff,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      instState->setResult(signedTmp);
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  signedSrc2x 
	 << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;    
#endif
}

    // SMPYH Integer Multiply 16msb x 16msb   src1 (smsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 12 pipeline phase E1
    void Tms320C6xIss::op_smpyh_m_02_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2x, signedTmp;
      signedSrc1 = (int32_t) state.regfile[instState->ins.m.s][instState->ins.m.src1];
      signedSrc2x = (int32_t) state.regfile[instState->ins.m.s ^ instState->ins.m.x][instState->ins.m.src2];
      signedTmp = (MSB16(signedSrc1) * MSB16(signedSrc2x) ) << 1;
      if ((uint32_t) signedTmp == 0x80000000)
	signedTmp = 0x7fffffff,
	  state.c_regfile[CSR] = state.c_regfile[CSR] | 0x00000200;
      instState->setResult(signedTmp);
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy operand: " << signedSrc1 << " " <<  signedSrc2x 
         << " mpy instruction result: " << signedTmp << std::dec << " (" << (int) signedTmp << ")" << std::endl;
#endif
    }

    void Tms320C6xIss::op_illegal_m_e1(InstructionState * instState) {
      std::cerr << "Warning: unknown instruction with opcode "<< std::hex << instState->ins.m.op << std::dec 
		<< " in .M unit in E1 pipeline phase @ " << m_exec_cycles <<  " cycle, instruction is " 
		<< std::hex << instState->ins.ins 
		<< " PC is " << state.DCE1.getPC() << std::dec << std::endl;
    }

    //
    // E2 phase
    //

    // MPY Integer Multiply 16lsb x 16lsb  src1 (slsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 19 pipeline phase E2
    void Tms320C6xIss::op_mpy_m_19_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif   
    }

    // MPYU Integer Multiply 16lsb x 16lsb  src1 (slsb16), src2 (xulsb16), dst (usint) unit = .M1 or .M2 op field: 1f pipeline phase E2
    void Tms320C6xIss::op_mpyu_m_1f_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYUS Integer Multiply 16lsb x 16lsb  src1 (ulsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 1d pipeline phase E2
    void Tms320C6xIss::op_mpyus_m_1d_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << "(" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif     
}

    // MPYSU Integer Multiply 16lsb x 16lsb  src1 (slsb16), src2 (xulsb16), dst (sint) unit = .M1 or .M2 op field: 1b pipeline phase E1
    void Tms320C6xIss::op_mpysu_m_1b_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << "(" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPY Integer Multiply 16lsb x 16lsb  src1 (scst5), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 1b pipeline phase E2
    void Tms320C6xIss::op_mpy_m_18_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYSU Integer Multiply 16lsb x 16lsb  src1 (scst5), src2 (xulsb16), dst (sint) unit = .M1 or .M2 op field: 1e pipeline phase E2
    void Tms320C6xIss::op_mpysu_m_1e_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYH Integer Multiply 16msb x 16msb   src1 (smsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 01 pipeline phase E2
    void Tms320C6xIss::op_mpyh_m_01_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYHU Integer Multiply 16msb x 16msb   src1 (umsb16), src2 (xumsb16), dst (sint) unit = .M1 or .M2 op field: 07 pipeline phase E2
    void Tms320C6xIss::op_mpyhu_m_07_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYHUS Integer Multiply 16msb x 16msb   src1 (umsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 05 pipeline phase E2
    void Tms320C6xIss::op_mpyhus_m_05_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYHSU Integer Multiply 16msb x 16msb   src1 (smsb16), src2 (xumsb16), dst (sint) unit = .M1 or .M2 op field: 03 pipeline phase E2
    void Tms320C6xIss::op_mpyhsu_m_03_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYHL Integer Multiply 16msb x 16lsb   src1 (smsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 09 pipeline phase E2
    void Tms320C6xIss::op_mpyhl_m_09_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYHLU Integer Multiply 16msb x 16lsb   src1 (umsb16), src2 (xulsb16), dst (usint) unit = .M1 or .M2 op field: 0f pipeline phase E2
    void Tms320C6xIss::op_mpyhlu_m_0f_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYHULS Integer Multiply 16msb x 16lsb   src1 (umsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 0d pipeline phase E2
    void Tms320C6xIss::op_mpyhuls_m_0d_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYHSLU Integer Multiply 16msb x 16lsb   src1 (smsb16), src2 (smsb16), dst (sint) unit = .M1 or .M2 op field: 0b pipeline phase E2
    void Tms320C6xIss::op_mpyhslu_m_0b_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYLH Integer Multiply 16lsb x 16msb   src1 (slsb16), src2 (slsb16), dst (sint) unit = .M1 or .M2 op field: 11 pipeline phase E2
    void Tms320C6xIss::op_mpylh_m_11_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYLH Integer Multiply 16lsb x 16msb   src1 (ulsb16), src2 (xumsb16), dst (usint) unit = .M1 or .M2 op field: 17 pipeline phase E2
    void Tms320C6xIss::op_mpylhu_m_17_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYLUHS Integer Multiply 16lsb x 16msb   src1 (ulsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 15 pipeline phase E2
    void Tms320C6xIss::op_mpyluhs_m_15_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] =  result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // MPYLUHS Integer Multiply 16lsb x 16msb   src1 (16msb), src2 (xumsb16), dst (sint) unit = .M1 or .M2 op field: 13 pipeline phase E2
    void Tms320C6xIss::op_mpylshu_m_13_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // SMPY Integer Multiply 16lsb x 16lsb   src1 (slsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 1a pipeline phase E2
    void Tms320C6xIss::op_smpy_m_1a_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    // SMPYHL Integer Multiply 16msb x 16lsb   src1 (smsb16), src2 (xslsb16), dst (sint) unit = .M1 or .M2 op field: 0a pipeline phase E2
    void Tms320C6xIss::op_smpyhl_m_0a_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif
    }

    // SMPYLH Integer Multiply 16lsb x 16msb   src1 (slsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 12 pipeline phase E2
    void Tms320C6xIss::op_smpylh_m_12_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B"; 
     std::cout << std::endl;
#endif 
    }

    // SMPYH Integer Multiply 16msb x 16msb   src1 (smsb16), src2 (xsmsb16), dst (sint) unit = .M1 or .M2 op field: 12 pipeline phase E2
    void Tms320C6xIss::op_smpyh_m_02_e2(InstructionState * instState) {
      uint32_t result = instState->getResult() & 0xffffffff;
      instState->setResult(result);
      state.regfile_temp[instState->ins.m.s][instState->ins.m.dst] = result;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " mpy instruction result: " << result << std::dec << " (" << (int) result << ")"
         << " written to register " << instState->ins.m.dst;
     if (!instState->ins.m.s)
       std::cout << " bank A" ;
     else std::cout << " bank B";
     std::cout << std::endl;
#endif 
    }

    void Tms320C6xIss::op_illegal_m_e2(InstructionState * instState) {
      std::cerr << "Warning: unknown instruction with opcode "<< std::hex << instState->ins.m.op << std::dec 
		<< " in .M unit in E2 pipeline phase @ " << m_exec_cycles <<  " cycle, instruction is " 
		<< std::hex << instState->ins.ins 
		<< " PC is " << state.E1E2.getPC() << std::dec << std::endl;
    }

    //
    //
    // .D unit InstructionState set
    //
    //

    //
    // E1 phase
    //

    // ADD (.unit) src1 (sint), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 10 pipeline phase E1
    void Tms320C6xIss::op_add_d_10_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src1]; 
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2]; 
      signedDst = signedSrc1 + signedSrc2;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " add operand: " 
	 << signedSrc1 << " (" << std::dec << (int) signedSrc1 << ") " << std::hex
	 << signedSrc2 << " (" << std::dec << (int) signedSrc2 << ") " << std::hex
         << " add instruction result: " << signedDst << std::dec << " (" << (int) signedDst << ")" << std::endl;
#endif 
    }

    // ADD (.unit) src1 (ucst5), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 12 pipeline phase E1
    void Tms320C6xIss::op_add_d_12_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      int32_t signedSrc2, signedDst;
      unsignedCst1 = instState->ins.d.src1;
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2]; 
      signedDst = unsignedCst1 + signedSrc2;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // ADDAB Add Byte with Addressing (.unit) src1 (sint), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 30 pipeline phase E1
    void Tms320C6xIss::op_addab_d_30_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src1]; 
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2]; 
      signedSrc1 = signedSrc1 << 0;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 + signedSrc1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // ADDAH Add Half Word with Addressing (.unit) src1 (sint), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 34 pipeline phase E1
    void Tms320C6xIss::op_addah_d_34_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src1]; 
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2]; 
      signedSrc1 = signedSrc1 << 1;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 + signedSrc1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // ADDAW Add Word with Addressing  (.unit) src1 (sint), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 38 pipeline phase E1
    void Tms320C6xIss::op_addaw_d_38_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src1]; 
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2]; 
      signedSrc1 = signedSrc1 << 2;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 + signedSrc1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // ADDAB Add Byte with Addressing (.unit) src1 (ucsut5), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 32 pipeline phase E1
    void Tms320C6xIss::op_addab_d_32_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      int32_t signedSrc2, signedDst;
      unsignedCst1 = instState->ins.d.src1;
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      unsignedCst1 = unsignedCst1 << 0;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 + unsignedCst1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // ADDAH Add Half Word with Addressing (.unit) src1 (ucsut5), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 32 pipeline phase E1
    void Tms320C6xIss::op_addah_d_36_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      int32_t signedSrc2, signedDst;
      unsignedCst1 = instState->ins.d.src1;
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      unsignedCst1 = unsignedCst1 << 1;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 + unsignedCst1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // ADDAW Add Word with Addressing (.unit) src1 (ucsut5), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 3a pipeline phase E1
    void Tms320C6xIss::op_addaw_d_3a_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      int32_t signedSrc2, signedDst;
      unsignedCst1 = instState->ins.d.src1;
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      unsignedCst1 = unsignedCst1 << 2;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 + unsignedCst1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }


    // SUB without saturation (.unit) src1 (sint), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 11 pipeline phase E1
    void Tms320C6xIss::op_sub_d_11_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src1];
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      signedDst = signedSrc2 - signedSrc1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // SUB without saturation (.unit) src1 (ucst5), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 13 pipeline phase E1
    void Tms320C6xIss::op_sub_d_13_e1(InstructionState * instState) {
      uint32_t unsignedCst1; int32_t signedSrc2; int32_t signedDst;
      unsignedCst1 = instState->ins.d.src1;
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      signedDst = signedSrc2 - unsignedCst1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // SUBAB Subtract Byte with Addressing  (.unit) src1 (sint), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 31 pipeline phase E1
    void Tms320C6xIss::op_subab_d_31_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src1];
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      signedSrc1 = signedSrc1 << 0;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 - signedSrc1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // SSUBAH Subtract Half Word with Addressing   (.unit) src1 (sint), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 31 pipeline phase E1
    void Tms320C6xIss::op_subah_d_35_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src1];
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      signedSrc1 = signedSrc1 << 1;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 - signedSrc1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // SUBAW Subtract Word with Addressing    (.unit) src1 (sint), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 39 pipeline phase E1
    void Tms320C6xIss::op_subaw_d_39_e1(InstructionState * instState) {
      int32_t signedSrc1, signedSrc2, signedDst;
      signedSrc1 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src1];
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      signedSrc1 = signedSrc1 << 2;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 - signedSrc1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // SUBAB Subtract Byte with Addressing    (.unit) src1 (ucsut5), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 39 pipeline phase E1
    void Tms320C6xIss::op_subab_d_33_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      int32_t signedSrc2, signedDst;
      unsignedCst1 = instState->ins.d.src1;
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      unsignedCst1 = unsignedCst1 << 0;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 - unsignedCst1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // SUBAH Subtract Half Word with Addressing    (.unit) src1 (ucsut5), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 37 pipeline phase E1
    void Tms320C6xIss::op_subah_d_37_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      int32_t signedSrc2, signedDst;
      unsignedCst1 = instState->ins.d.src1;
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      unsignedCst1 = unsignedCst1 << 1;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 - unsignedCst1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;

    }

    // SUBAW Subtract Word with Addressing     (.unit) src1 (ucsut5), src2 (sint), dst (sint) -- .unit = .D1, .D2 op field: 37 pipeline phase E1
    void Tms320C6xIss::op_subaw_d_3b_e1(InstructionState * instState) {
      uint32_t unsignedCst1; 
      int32_t signedSrc2, signedDst;
      unsignedCst1 = instState->ins.d.src1;
      signedSrc2 = (int32_t) state.regfile[instState->ins.d.s][instState->ins.d.src2];
      unsignedCst1 = unsignedCst1 << 2;
      if (instState->ins.d.src2 >=4 && instState->ins.d.src2 <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d.src2,instState->ins.d.s)) {
	case 0x1 :
	  // circular addressing with BK0
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }
      signedDst = signedSrc2 - unsignedCst1;
      state.regfile_temp[instState->ins.d.s][instState->ins.d.dst] =  signedDst;
    }

    // Load Byte From Memory With a 15-Bit Unsigned Constant Offset -- LDB (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E1
    void Tms320C6xIss::op_ldb_d_ldstOffset_02_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, address;      
      unsignedSrc1 = state.regfile[sideB][(instState->ins.d_ldstOffset.y == 0)?14:15];
      address = unsignedSrc1 + (instState->ins.d_ldstOffset.ucst15 << 0);
      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
     std::cout
         << m_name << std::hex
         << " load instruction: @" << address << std::dec
         << " (" << " READ_BYTE " << ")"
         << " -> rf" << std::dec << instState->ins.d_ldstOffset.dst
         << std::endl;
#endif    
    }

    // Load Byte From Memory With a 15-Bit Unsigned Constant Offset -- LDBU (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E1
    void Tms320C6xIss::op_ldbu_d_ldstOffset_01_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, address;
      unsignedSrc1 = state.regfile[sideB][(instState->ins.d_ldstOffset.y == 0)?14:15];
      address = unsignedSrc1 + (instState->ins.d_ldstOffset.ucst15 << 0);
      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction: @" << address << std::dec
        << " (" << " READ_BYTE " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstOffset.dst
        << std::endl;
#endif
    }

    // Load Halfword From Memory With a 15-Bit Unsigned Constant Offset -- LDH (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E1
    void Tms320C6xIss::op_ldh_d_ldstOffset_04_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, address;
      unsignedSrc1 = state.regfile[sideB][(instState->ins.d_ldstOffset.y == 0)?14:15];
      address = unsignedSrc1 + (instState->ins.d_ldstOffset.ucst15 << 1);
      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if tms320c62_DEBUG
    std::cout
        << m_name << std::hex
        << " load @" << address << std::dec
        << " (" << " READ_HALF " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstOffset.dst
        << std::endl;
#endif
    }

    // Load Halfword From Memory With a 15-Bit Unsigned Constant Offset -- LDHU (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E1
    void Tms320C6xIss::op_ldhu_d_ldstOffset_00_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, address;
      unsignedSrc1 = state.regfile[sideB][(instState->ins.d_ldstOffset.y == 0)?14:15];
      address = unsignedSrc1 + (instState->ins.d_ldstOffset.ucst15 << 1);
      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction: @" << address << std::dec
        << " (" << " READ_HALF " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstOffset.dst
        << std::endl;
#endif
    }

    // Load Word From Memory With a 15-Bit Unsigned Constant Offset -- LDW (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E1
    void Tms320C6xIss::op_ldw_d_ldstOffset_06_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, address;
      unsignedSrc1 = state.regfile[sideB][(instState->ins.d_ldstOffset.y == 0)?14:15];
      address = unsignedSrc1 + (instState->ins.d_ldstOffset.ucst15 << 2);
      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
	<< " load instruction: @" << address << std::dec
//	<< " load instruction: @" << instState->getAddress(instState->getLoadStorePosition()) << std::dec
        << " (" << " READ_WORD " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstOffset.dst
        << std::endl;
#endif
    }

    // Store Byte to Memory With a 15-Bit Unsigned Constant Offset -- STB (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E1
    void Tms320C6xIss::op_stb_d_ldstOffset_03_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, address, data;
      unsignedSrc1 = state.regfile[sideB][(instState->ins.d_ldstOffset.y == 0)?14:15];
      address = unsignedSrc1 + (instState->ins.d_ldstOffset.ucst15 << 0);
      data = state.regfile[instState->ins.d_ldstOffset.s][instState->ins.d_ldstOffset.dst] & 0x000000ff;
      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setData(data, instState->getLoadStorePosition());
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction: @" << address 
        << ": " << data << std::dec
        << " (" << " WRITE_BYTE " << ")"
        << std::endl;
#endif
   }

    // Store Halfword to Memory With a 15-Bit Unsigned Constant Offset -- STH (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E1
    void Tms320C6xIss::op_sth_d_ldstOffset_05_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, address, data;
      unsignedSrc1 = state.regfile[sideB][(instState->ins.d_ldstOffset.y == 0)?14:15];
      address = unsignedSrc1 + (instState->ins.d_ldstOffset.ucst15 << 1);
      data = state.regfile[instState->ins.d_ldstOffset.s][instState->ins.d_ldstOffset.dst] & 0x0000ffff;
      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setData(data, instState->getLoadStorePosition());
      instState->setWriteInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction: @" << address 
        << ": " << data << std::dec
        << " (" << " WRITE_HALF " << ")"
        << std::endl;
#endif
    }

    // Store Word to Memory With a 15-Bit Unsigned Constant Offset -- STW (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E1
    void Tms320C6xIss::op_stw_d_ldstOffset_07_e1(InstructionState * instState) {
      uint32_t unsignedSrc1, address, data;
      unsignedSrc1 = state.regfile[sideB][(instState->ins.d_ldstOffset.y == 0)?14:15];
      address = unsignedSrc1 + (instState->ins.d_ldstOffset.ucst15 << 2);
      data = (state.regfile[instState->ins.d_ldstOffset.s][instState->ins.d_ldstOffset.dst]);
      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setData(data, instState->getLoadStorePosition());
      instState->setWriteInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction: @" << address 
        << ": " << data << std::dec
        << " (" << " WRITE_WORD " << ")"
        << std::endl;
#endif
    }

    // Load Byte From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E1
    // LDB (.unit) *+baseR[offsetR], dst or LDB (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldb_d_ldstBaseROffset_02_e1(InstructionState * instState) {
      uint32_t unsignedCst1, address = 0; int32_t signedSrc1; int32_t signedSrc2;
      signedSrc1 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.offsetR];
      signedSrc2 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR];
      unsignedCst1 = instState->ins.d_ldstBaseROffset.offsetR;
      signedSrc1 = signedSrc1 << 0;
      unsignedCst1 = unsignedCst1 << 0;

      if (instState->ins.d_ldstBaseROffset.baseR >=4 && instState->ins.d_ldstBaseROffset.baseR <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d_ldstBaseROffset.baseR,instState->ins.d_ldstBaseROffset.y)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }

      switch (instState->ins.d_ldstBaseROffset.mode) {
      case 0x0 :
	address = (signedSrc2 - unsignedCst1);
	break;
      case 0x1 :
	address = (signedSrc2 + unsignedCst1);
	break;
      case 0x4 :
	address = (signedSrc2 - signedSrc1);
	break;
      case 0x5 :
	address = signedSrc2 + signedSrc1;
	break;
      case 0x8 : // predecrement
	signedSrc2 = signedSrc2 - unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0x9 : // preincrement
	signedSrc2 = signedSrc2 + unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xa : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xb : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xc : // predecrement
	signedSrc2 = signedSrc2 - signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xd : // preincrement
	signedSrc2 = signedSrc2 + signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xe : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xf : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      default:
	break;
      }

      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction: @" << address << std::dec
        << " (" << " READ_BYTE " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstBaseROffset.dst
        << std::endl;
#endif    
}

    // Load Byte From Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E1
    // LDBU (.unit) *+baseR[offsetR], dst or LDBU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldbu_d_ldstBaseROffset_01_e1(InstructionState * instState) {
      uint32_t unsignedCst1, address = 0; int32_t signedSrc1; int32_t signedSrc2;
      signedSrc1 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.offsetR];
      signedSrc2 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR];
      unsignedCst1 = instState->ins.d_ldstBaseROffset.offsetR;
      signedSrc1 = signedSrc1 << 0;
      unsignedCst1 = unsignedCst1 << 0;

      if (instState->ins.d_ldstBaseROffset.baseR >=4 && instState->ins.d_ldstBaseROffset.baseR <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d_ldstBaseROffset.baseR,instState->ins.d_ldstBaseROffset.y)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }

      switch (instState->ins.d_ldstBaseROffset.mode) {
      case 0x0 :
	address = (signedSrc2 - unsignedCst1);
	break;
      case 0x1 :
	address = (signedSrc2 + unsignedCst1);
	break;
      case 0x4 :
	address = (signedSrc2 - signedSrc1);
	break;
      case 0x5 :
	address = signedSrc2 + signedSrc1;
	break;
      case 0x8 : // predecrement
	signedSrc2 = signedSrc2 - unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0x9 : // preincrement
	signedSrc2 = signedSrc2 + unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xa : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xb : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xc : // predecrement
	signedSrc2 = signedSrc2 - signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xd : // preincrement
	signedSrc2 = signedSrc2 + signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xe : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xf : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      default:
	break;
      }

      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction: @" << address << std::dec
        << " (" << " READ_BYTE " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstBaseROffset.dst
        << std::endl;
#endif
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E1
    // LDH (.unit) *+baseR[offsetR], dst   LDH (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldh_d_ldstBaseROffset_04_e1(InstructionState * instState) {
      uint32_t unsignedCst1, address = 0; int32_t signedSrc1; int32_t signedSrc2;
      signedSrc1 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.offsetR];
      signedSrc2 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR];
      unsignedCst1 = instState->ins.d_ldstBaseROffset.offsetR;
      signedSrc1 = signedSrc1 << 1;
      unsignedCst1 = unsignedCst1 << 1;

      if (instState->ins.d_ldstBaseROffset.baseR >=4 && instState->ins.d_ldstBaseROffset.baseR <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d_ldstBaseROffset.baseR,instState->ins.d_ldstBaseROffset.y)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }

      switch (instState->ins.d_ldstBaseROffset.mode) {
      case 0x0 :
	address = (signedSrc2 - unsignedCst1);
	break;
      case 0x1 :
	address = (signedSrc2 + unsignedCst1);
	break;
      case 0x4 :
	address = (signedSrc2 - signedSrc1);
	break;
      case 0x5 :
	address = signedSrc2 + signedSrc1;
	break;
      case 0x8 : // predecrement
	signedSrc2 = signedSrc2 - unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0x9 : // preincrement
	signedSrc2 = signedSrc2 + unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xa : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xb : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xc : // predecrement
	signedSrc2 = signedSrc2 - signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xd : // preincrement
	signedSrc2 = signedSrc2 + signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xe : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xf : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      default:
	break;
      }

      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction: @" << address << std::dec
        << " (" << " READ_HALF " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstBaseROffset.dst
        << std::endl;
#endif    
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E1
    // LDHU (.unit) *+baseR[offsetR], dst   LDHU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldhu_d_ldstBaseROffset_00_e1(InstructionState * instState) {
      uint32_t unsignedCst1, address = 0; int32_t signedSrc1; int32_t signedSrc2;
      signedSrc1 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.offsetR];
      signedSrc2 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR];
      unsignedCst1 = instState->ins.d_ldstBaseROffset.offsetR;
      signedSrc1 = signedSrc1 << 1;
      unsignedCst1 = unsignedCst1 << 1;

      if (instState->ins.d_ldstBaseROffset.baseR >=4 && instState->ins.d_ldstBaseROffset.baseR <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d_ldstBaseROffset.baseR,instState->ins.d_ldstBaseROffset.y)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }

      switch (instState->ins.d_ldstBaseROffset.mode) {
      case 0x0 :
	address = (signedSrc2 - unsignedCst1);
	break;
      case 0x1 :
	address = (signedSrc2 + unsignedCst1);
	break;
      case 0x4 :
	address = (signedSrc2 - signedSrc1);
	break;
      case 0x5 :
	address = signedSrc2 + signedSrc1;
	break;
      case 0x8 : // predecrement
	signedSrc2 = signedSrc2 - unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0x9 : // preincrement
	signedSrc2 = signedSrc2 + unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xa : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xb : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xc : // predecrement
	signedSrc2 = signedSrc2 - signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xd : // preincrement
	signedSrc2 = signedSrc2 + signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xe : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xf : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      default:
	break;
      }

      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction: @" << address << std::dec
        << " (" << " READ_HALF " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstBaseROffset.dst
        << std::endl;
#endif
    }

    // Load Word From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E1
    // LDW (.unit) *+baseR[offsetR], dst    -- LDW (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldw_d_ldstBaseROffset_06_e1(InstructionState * instState) {
      uint32_t unsignedCst1, address = 0; int32_t signedSrc1; int32_t signedSrc2;
      signedSrc1 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.offsetR];
      signedSrc2 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR];
      unsignedCst1 = instState->ins.d_ldstBaseROffset.offsetR;

      signedSrc1 = signedSrc1 << 2;
      unsignedCst1 = unsignedCst1 << 2;

      if (instState->ins.d_ldstBaseROffset.baseR >=4 && instState->ins.d_ldstBaseROffset.baseR <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d_ldstBaseROffset.baseR,instState->ins.d_ldstBaseROffset.y)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }

      switch (instState->ins.d_ldstBaseROffset.mode) {
      case 0x0 :
	address = (signedSrc2 - unsignedCst1);
	break;
      case 0x1 :
	address = (signedSrc2 + unsignedCst1);
	break;
      case 0x4 :
	address = (signedSrc2 - signedSrc1);
	break;
      case 0x5 :
	address = signedSrc2 + signedSrc1;
	break;
      case 0x8 : // predecrement
	signedSrc2 = signedSrc2 - unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0x9 : // preincrement
	signedSrc2 = signedSrc2 + unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xa : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xb : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xc : // predecrement
	signedSrc2 = signedSrc2 - signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xd : // preincrement
	signedSrc2 = signedSrc2 + signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xe : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xf : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      default:
	break;
      }

      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setReadInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
	<< " load instruction: @" << instState->getAddress(instState->getLoadStorePosition()) << std::dec
//         << " load instruction: @" << address << std::dec
        << " (" << " READ_WORD " << ")"
        << " -> rf" << std::dec << instState->ins.d_ldstBaseROffset.dst
        << std::endl;
#endif
    }

    // Store Byte to Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E1
    // STB (.unit) src, *+baseR[offsetR] -- STB (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_stb_d_ldstBaseROffset_03_e1(InstructionState * instState) {
      uint32_t unsignedCst1, data, address = 0; int32_t signedSrc1; int32_t signedSrc2;
      signedSrc1 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.offsetR];
      signedSrc2 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR];
      unsignedCst1 = instState->ins.d_ldstBaseROffset.offsetR;
      data = state.regfile[instState->ins.d_ldstBaseROffset.s][instState->ins.d_ldstBaseROffset.dst] & 0x000000ff;

      signedSrc1 = signedSrc1 << 0;
      unsignedCst1 = unsignedCst1 << 0;

      if (instState->ins.d_ldstBaseROffset.baseR >=4 && instState->ins.d_ldstBaseROffset.baseR <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d_ldstBaseROffset.baseR,instState->ins.d_ldstBaseROffset.y)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }

      switch (instState->ins.d_ldstBaseROffset.mode) {
      case 0x0 :
	address = (signedSrc2 - unsignedCst1);
	break;
      case 0x1 :
	address = (signedSrc2 + unsignedCst1);
	break;
      case 0x4 :
	address = (signedSrc2 - signedSrc1);
	break;
      case 0x5 :
	address = signedSrc2 + signedSrc1;
	break;
      case 0x8 : // predecrement
	signedSrc2 = signedSrc2 - unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0x9 : // preincrement
	signedSrc2 = signedSrc2 + unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xa : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xb : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xc : // predecrement
	signedSrc2 = signedSrc2 - signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xd : // preincrement
	signedSrc2 = signedSrc2 + signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xe : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xf : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      default:
	break;
      }

      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setData(data, instState->getLoadStorePosition());
      instState->setWriteInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction: @" << address 
        << ": " << data << std::dec
        << " (" << " WRITE_BYTE " << ")"
        << std::endl;
#endif
    }

    // Store Halfword to Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E1
    // STH (.unit) src, *+baseR[offsetR] -- STH (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_sth_d_ldstBaseROffset_05_e1(InstructionState * instState) {
      uint32_t unsignedCst1, data, address = 0; int32_t signedSrc1; int32_t signedSrc2;
      signedSrc1 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.offsetR];
      signedSrc2 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR];
      unsignedCst1 = instState->ins.d_ldstBaseROffset.offsetR;
      data = state.regfile[instState->ins.d_ldstBaseROffset.s][instState->ins.d_ldstBaseROffset.dst] & 0x0000ffff;

      signedSrc1 = signedSrc1 << 1;
      unsignedCst1 = unsignedCst1 << 1;

      if (instState->ins.d_ldstBaseROffset.baseR >=4 && instState->ins.d_ldstBaseROffset.baseR <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d_ldstBaseROffset.baseR,instState->ins.d_ldstBaseROffset.y)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }

      switch (instState->ins.d_ldstBaseROffset.mode) {
      case 0x0 :
	address = (signedSrc2 - unsignedCst1);
	break;
      case 0x1 :
	address = (signedSrc2 + unsignedCst1);
	break;
      case 0x4 :
	address = (signedSrc2 - signedSrc1);
	break;
      case 0x5 :
	address = signedSrc2 + signedSrc1;
	break;
      case 0x8 : // predecrement
	signedSrc2 = signedSrc2 - unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0x9 : // preincrement
	signedSrc2 = signedSrc2 + unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xa : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xb : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xc : // predecrement
	signedSrc2 = signedSrc2 - signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xd : // preincrement
	signedSrc2 = signedSrc2 + signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xe : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xf : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      default:
	break;
      }

      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setData(data, instState->getLoadStorePosition());
      instState->setWriteInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction: @" << address
        << ": " << data  << std::dec
        << " (" << " WRITE_HALF " << ")"
        << std::endl;
#endif
    }

    // Store Word to Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E1
    // STW (.unit) src, *+baseR[offsetR]  -- STW (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_stw_d_ldstBaseROffset_07_e1(InstructionState * instState) {
      uint32_t unsignedCst1, data, address = 0; int32_t signedSrc1; int32_t signedSrc2;
      signedSrc1 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.offsetR];
      signedSrc2 = state.regfile[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR];
      unsignedCst1 = instState->ins.d_ldstBaseROffset.offsetR;
      data = state.regfile[instState->ins.d_ldstBaseROffset.s][instState->ins.d_ldstBaseROffset.dst];

      signedSrc1 = signedSrc1 << 2;
      unsignedCst1 = unsignedCst1 << 2;

      if (instState->ins.d_ldstBaseROffset.baseR >=4 && instState->ins.d_ldstBaseROffset.baseR <= 7) {
	// use of circular addressing ?
	switch (ADDRESSING_MODE(instState->ins.d_ldstBaseROffset.baseR,instState->ins.d_ldstBaseROffset.y)) {
	case 0x1 :
	  // circular addressing with BK0
	  signedSrc1 = signedSrc1 % power2[BK0+1];
	  unsignedCst1 = unsignedCst1 % power2[BK0+1];
	  break;
	case 0x2 :
	  // circular addressing with BK1
	  signedSrc1 = signedSrc1 % power2[BK1+1];
	  unsignedCst1 = unsignedCst1 % power2[BK1+1];
	  break;
	default :
	  break;
	}
      }

      switch (instState->ins.d_ldstBaseROffset.mode) {
      case 0x0 :
	address = (signedSrc2 - unsignedCst1);
	break;
      case 0x1 :
	address = (signedSrc2 + unsignedCst1);
	break;
      case 0x4 :
	address = (signedSrc2 - signedSrc1);
	break;
      case 0x5 :
	address = signedSrc2 + signedSrc1;
	break;
      case 0x8 : // predecrement
	signedSrc2 = signedSrc2 - unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0x9 : // preincrement
	signedSrc2 = signedSrc2 + unsignedCst1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xa : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xb : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + unsignedCst1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xc : // predecrement
	signedSrc2 = signedSrc2 - signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xd : // preincrement
	signedSrc2 = signedSrc2 + signedSrc1;
	address = (signedSrc2);
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xe : //postdecrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 - signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      case 0xf : //postincrement
	address = (signedSrc2);
	signedSrc2 = signedSrc2 + signedSrc1;
	state.regfile_temp[instState->ins.d_ldstBaseROffset.y][instState->ins.d_ldstBaseROffset.baseR] = signedSrc2;
	break;
      default:
	break;
      }

      instState->setAddress(address, instState->getLoadStorePosition());
      instState->setData(data, instState->getLoadStorePosition());
      instState->setWriteInstInPacket(true);
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction: @" << address 
        << ": " << data << std::dec
        << " (" << " WRITE_WORD " << ")"
        << std::endl;
#endif
    }


    //
    // E2 phase
    // this phase is used to transmit the data_from_mem data to E2 phase 
    //

    // Load Byte From Memory With a 15-Bit Unsigned Constant Offset -- LDB (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E2
    void Tms320C6xIss::op_ldb_d_ldstOffset_02_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_BYTE;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstOffset.dst;
      r_mem_unsigned = false;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex 
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
    }

    // Load Byte From Memory With a 15-Bit Unsigned Constant Offset -- LDBU (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E2
    void Tms320C6xIss::op_ldbu_d_ldstOffset_01_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_BYTE;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstOffset.dst;
      r_mem_unsigned = false;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
      }

    // Load Halfword From Memory With a 15-Bit Unsigned Constant Offset -- LDH (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E2
    void Tms320C6xIss::op_ldh_d_ldstOffset_04_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_HALF;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstOffset.dst;
      r_mem_unsigned = false;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
    }

    // Load Halfword From Memory With a 15-Bit Unsigned Constant Offset -- LDHU (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E2
    void Tms320C6xIss::op_ldhu_d_ldstOffset_00_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_HALF;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstOffset.dst;
      r_mem_unsigned = true;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
       }

    // Load Word From Memory With a 15-Bit Unsigned Constant Offset -- LDW (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E2
    void Tms320C6xIss::op_ldw_d_ldstOffset_06_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_WORD;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstOffset.dst;
      r_mem_unsigned = false;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
    }

    // Store Byte to Memory With a 15-Bit Unsigned Constant Offset -- STB (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E2
    void Tms320C6xIss::op_stb_d_ldstOffset_03_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = WRITE_BYTE;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_wdata = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction request with @" << r_mem_addr << " is sent to memory (data: " << r_mem_wdata << ")"
        << std::dec << std::endl;
#endif
    }

    // Store Halfword to Memory With a 15-Bit Unsigned Constant Offset -- STH (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E2
    void Tms320C6xIss::op_sth_d_ldstOffset_05_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = WRITE_HALF;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_wdata = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction request with @" << r_mem_addr << " is sent to memory (data: " << r_mem_wdata << ")"
        << std::dec << std::endl;
#endif
       }

    // Store Word to Memory With a 15-Bit Unsigned Constant Offset -- STW (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E2
    void Tms320C6xIss::op_stw_d_ldstOffset_07_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = WRITE_WORD;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_wdata = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction request with @" << r_mem_addr << " is sent to memory (data: " << r_mem_wdata << ")"
        << std::dec << std::endl;
#endif
    }

    // Load Byte From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E2
    // LDB (.unit) *+baseR[offsetR], dst or LDB (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldb_d_ldstBaseROffset_02_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_BYTE;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstBaseROffset.dst;
      r_mem_unsigned = false;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset
    // LDBU (.unit) *+baseR[offsetR], dst or LDBU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldbu_d_ldstBaseROffset_01_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_HALF;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstBaseROffset.dst;
      r_mem_unsigned = false;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E2
    // LDH (.unit) *+baseR[offsetR], dst   LDH (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldh_d_ldstBaseROffset_04_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_HALF;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstBaseROffset.dst;
      r_mem_unsigned = false;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E2
    // LDHU (.unit) *+baseR[offsetR], dst   LDHU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldhu_d_ldstBaseROffset_00_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_HALF;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstBaseROffset.dst;
      r_mem_unsigned = false;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
    }

    // Load Word From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E2
    // LDW (.unit) *+baseR[offsetR], dst    -- LDW (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldw_d_ldstBaseROffset_06_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = READ_WORD;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_dest = instState->ins.d_ldstBaseROffset.dst;
      r_mem_unsigned = true;
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " load instruction request with @" << r_mem_addr << " is sent to memory "
        << std::dec << std::endl;
#endif
    }

    // Store Byte to Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E2
    // STB (.unit) src, *+baseR[offsetR] -- STB (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_stb_d_ldstBaseROffset_03_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = WRITE_BYTE;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_wdata = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction request with @" << r_mem_addr << " is sent to memory (data: " << r_mem_wdata << ")"
        << std::dec << std::endl;
#endif
    }

    // Store Halfword to Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E2 
    // STH (.unit) src, *+baseR[offsetR] -- STH (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_sth_d_ldstBaseROffset_05_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = WRITE_HALF;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_wdata = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction request with @" << r_mem_addr << " is sent to memory (data: " << r_mem_wdata << ")"
        << std::dec << std::endl;
#endif
    }

    // Store Word to Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E2
    // STW (.unit) src, *+baseR[offsetR]  -- STW (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_stw_d_ldstBaseROffset_07_e2(InstructionState * instState) {
      r_mem_req = true;
      r_mem_type = WRITE_WORD;
      r_mem_addr = instState->getAddress(instState->getLoadStorePosition());
      r_mem_wdata = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " store instruction request with @" << r_mem_addr << " is sent to memory (data: " << r_mem_wdata << ")"
        << std::dec << std::endl;
#endif
    }


    //
    // E3 phase
    //

    // Load Byte From Memory With a 15-Bit Constant Offset -- LDB (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E3
    void Tms320C6xIss::op_ldb_d_ldstOffset_02_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Load Byte From Memory With a 15-Bit Unsigned Constant Offset -- LDBU (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E3
    void Tms320C6xIss::op_ldbu_d_ldstOffset_01_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Load Halfword From Memory With a 15-Bit Constant Offset -- LDH (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E3
    void Tms320C6xIss::op_ldh_d_ldstOffset_04_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Load Halfword From Memory With a 15-Bit Unsigned Constant Offset -- LDHU (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E3
    void Tms320C6xIss::op_ldhu_d_ldstOffset_00_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Load Word From Memory With a 15-Bit Unsigned Offset -- LDW (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E3
    void Tms320C6xIss::op_ldw_d_ldstOffset_06_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
      std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Store Byte to Memory With a 15-Bit Unsigned Constant Offset -- STB (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E3
    void Tms320C6xIss::op_stb_d_ldstOffset_03_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::endl;
#endif
    }

    // Store Halfword to Memory With a 15-Bit Unsigned Constant Offset -- STH (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E3
    void Tms320C6xIss::op_sth_d_ldstOffset_05_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    }

    // Store Word to Memory With a 15-Bit Unsigned Constant Offset -- STW (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E3
    void Tms320C6xIss::op_stw_d_ldstOffset_07_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value stored in memory : " << instState->getData(instState->getLoadStorePosition())
        << std::dec << std::endl;
#endif
    }

    // Load Byte From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E3
    // LDB (.unit) *+baseR[offsetR], dst or LDB (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldb_d_ldstBaseROffset_02_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
      std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }
    
    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E3
    // LDBU (.unit) *+baseR[offsetR], dst or LDBU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldbu_d_ldstBaseROffset_01_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E3
    // LDH (.unit) *+baseR[offsetR], dst   LDH (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldh_d_ldstBaseROffset_04_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E3
    // LDHU (.unit) *+baseR[offsetR], dst   LDHU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldhu_d_ldstBaseROffset_00_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
        << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Load Word From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E3
    // LDW (.unit) *+baseR[offsetR], dst    -- LDW (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldw_d_ldstBaseROffset_06_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
     std::cout
       << m_name << std::hex
       << " value read at @" << instState->getAddress(instState->getLoadStorePosition()) << "(" <<data_from_mem << ")"
       << std::dec << std::endl;
#endif
    data_from_mem_e3[instState->getLoadStorePosition()] = data_from_mem;
    }

    // Store Byte to Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E3
    // STB (.unit) src, *+baseR[offsetR] -- STB (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_stb_d_ldstBaseROffset_03_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value stored in memory : " << instState->getData(instState->getLoadStorePosition())
        << std::dec << std::endl;
#endif
    }

    // Store Halfword to Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E3
    // STH (.unit) src, *+baseR[offsetR] -- STH (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_sth_d_ldstBaseROffset_05_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value stored in memory : " << instState->getData(instState->getLoadStorePosition())
        << std::dec << std::endl;
#endif
    }

    // Store Word to Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E3
    // STW (.unit) src, *+baseR[offsetR]  -- STW (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_stw_d_ldstBaseROffset_07_e3(InstructionState * instState) {
#if TMS320C62_DEBUG
    std::cout
        << m_name << std::hex
        << " value stored in memory : " << instState->getData(instState->getLoadStorePosition())
        << std::dec << std::endl;
#endif
    }


    //
    // E4 phase
    //

    // Load Byte From Memory With a 15-Bit Constant Offset -- LDB (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E4
    void Tms320C6xIss::op_ldb_d_ldstOffset_02_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Load Byte From Memory With a 15-Bit Unsigned Constant Offset -- LDBU (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E4
    void Tms320C6xIss::op_ldbu_d_ldstOffset_01_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Load Halfword From Memory With a 15-Bit Constant Offset -- LDH (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E4
    void Tms320C6xIss::op_ldh_d_ldstOffset_04_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Load Halfword From Memory With a 15-Bit Unsigned Constant Offset -- LDHU (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E4
    void Tms320C6xIss::op_ldhu_d_ldstOffset_00_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Load Word From Memory With a 15-Bit Unsigned Offset -- LDW (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E4
    void Tms320C6xIss::op_ldw_d_ldstOffset_06_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Store Byte to Memory With a 15-Bit Unsigned Constant Offset -- STB (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E4
    void Tms320C6xIss::op_stb_d_ldstOffset_03_e4(InstructionState * instState) {
    }

    // Store Halfword to Memory With a 15-Bit Unsigned Constant Offset -- STH (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E4
    void Tms320C6xIss::op_sth_d_ldstOffset_05_e4(InstructionState * instState) {
    }

    // Store Word to Memory With a 15-Bit Unsigned Constant Offset -- STW (.unit) src, *+B14/B15[ucst15] -- .unit = .D2  pipeline phase E4
    void Tms320C6xIss::op_stw_d_ldstOffset_07_e4(InstructionState * instState) {
    }

    // Load Byte From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E4
    // LDB (.unit) *+baseR[offsetR], dst or LDB (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldb_d_ldstBaseROffset_02_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }
    
    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E4
    // LDBU (.unit) *+baseR[offsetR], dst or LDBU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldbu_d_ldstBaseROffset_01_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E4
    // LDH (.unit) *+baseR[offsetR], dst   LDH (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldh_d_ldstBaseROffset_04_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E4
    // LDHU (.unit) *+baseR[offsetR], dst   LDHU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldhu_d_ldstBaseROffset_00_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Load Word From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E4
    // LDW (.unit) *+baseR[offsetR], dst    -- LDW (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldw_d_ldstBaseROffset_06_e4(InstructionState * instState) {
      instState->setData(data_from_mem_e3[instState->getLoadStorePosition()], instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value received at CPU boundary: "  << data_from_mem_e3[instState->getLoadStorePosition()] << std::dec 
	  << std::endl;
#endif
    }

    // Store Byte to Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E4
    // STB (.unit) src, *+baseR[offsetR] -- STB (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_stb_d_ldstBaseROffset_03_e4(InstructionState * instState) {
    }

    // Store Halfword to Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E4
    // STH (.unit) src, *+baseR[offsetR] -- STH (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_sth_d_ldstBaseROffset_05_e4(InstructionState * instState) {
    }

    // Store Word to Memory With a 5-Bit Unsigned Constant Offset or Register Offset  -- .unit = .D1 or .D2  pipeline phase E4
    // STW (.unit) src, *+baseR[offsetR]  -- STW (.unit) src, *+baseR[ucst5]
    void Tms320C6xIss::op_stw_d_ldstBaseROffset_07_e4(InstructionState * instState) {
    }


    //
    // E5 phase
    //

    // Load Byte From Memory With a 15-Bit Unsigned Constant Offset -- LDB (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E5
    void Tms320C6xIss::op_ldb_d_ldstOffset_02_e5(InstructionState * instState) {
      if (!registerUpdatePostponed) {
	state.regfile_temp[instState->ins.d_ldstOffset.s][instState->ins.d_ldstOffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
	if (!instState->ins.d_ldstOffset.s)
	  std::cout << " bank A" ;
	else std::cout << " bank B";
	std::cout << " register " << instState->ins.d_ldstOffset.dst << std::endl;
#endif
      }
      else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstOffset.s,instState->ins.d_ldstOffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Byte From Memory With a 15-Bit Unsigned Constant Offset -- LDBU (.unit) *+B14/B15[ucst15], dst -- .unit = .D2  pipeline phase E5
    void Tms320C6xIss::op_ldbu_d_ldstOffset_01_e5(InstructionState * instState) {
      if (!registerUpdatePostponed) {
	state.regfile_temp[instState->ins.d_ldstOffset.s][instState->ins.d_ldstOffset.dst] = instState->getData(instState->getLoadStorePosition()); 
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
	if (!instState->ins.d_ldstOffset.s)
	  std::cout << " bank A" ;
	else std::cout << " bank B";
	std::cout << " register " << instState->ins.d_ldstOffset.dst << std::endl;
#endif
      }
      else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstOffset.s,instState->ins.d_ldstOffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Halfword From Memory With a 15-Bit Unsigned Constant Offset -- LDH (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E5
    void Tms320C6xIss::op_ldh_d_ldstOffset_04_e5(InstructionState * instState) {
      if (!registerUpdatePostponed) {
	state.regfile_temp[instState->ins.d_ldstOffset.s][instState->ins.d_ldstOffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
	if (!instState->ins.d_ldstOffset.s)
	  std::cout << " bank A" ;
	else std::cout << " bank B";
	std::cout << " register " << instState->ins.d_ldstOffset.dst << std::endl;
#endif
      }
      else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstOffset.s,instState->ins.d_ldstOffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Halfword From Memory With a 15-Bit Unsigned Constant Offset -- LDHU (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E5
    void Tms320C6xIss::op_ldhu_d_ldstOffset_00_e5(InstructionState * instState) {
      if (!registerUpdatePostponed) {
	state.regfile_temp[instState->ins.d_ldstOffset.s][instState->ins.d_ldstOffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
	if (!instState->ins.d_ldstBaseROffset.s)
	  std::cout << " bank A" ;
	else std::cout << " bank B";
	std::cout << " register " << instState->ins.d_ldstBaseROffset.dst << std::endl;
#endif
      }
      else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstOffset.s,instState->ins.d_ldstOffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Word From Memory With a 15-Bit Unsigned Constant Offset -- LDW (.unit) *+B14/B15[ucst15], dst  -- .unit = .D2  pipeline phase E5
    void Tms320C6xIss::op_ldw_d_ldstOffset_06_e5(InstructionState * instState) {
      if (!registerUpdatePostponed) {
	state.regfile_temp[instState->ins.d_ldstOffset.s][instState->ins.d_ldstOffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
	std::cout
	  << m_name << std::hex
	  << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
	if (!instState->ins.d_ldstBaseROffset.s)
	  std::cout << " bank A" ;
	else std::cout << " bank B";
	std::cout << " register " << instState->ins.d_ldstBaseROffset.dst << std::endl;
#endif 
      }
      else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstOffset.s,instState->ins.d_ldstOffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Byte From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E5
    // LDB (.unit) *+baseR[offsetR], dst or LDB (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldb_d_ldstBaseROffset_02_e5(InstructionState * instState) {
     if (!registerUpdatePostponed) {
       state.regfile_temp[instState->ins.d_ldstBaseROffset.s][instState->ins.d_ldstBaseROffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
       std::cout
	 << m_name << std::hex
	 << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
       if (!instState->ins.d_ldstBaseROffset.s)
	 std::cout << " bank A" ;
       else std::cout << " bank B";
       std::cout << " register " << instState->ins.d_ldstBaseROffset.dst << std::endl;
#endif 
     }
     else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstBaseROffset.s,instState->ins.d_ldstBaseROffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset
    // LDBU (.unit) *+baseR[offsetR], dst or LDBU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldbu_d_ldstBaseROffset_01_e5(InstructionState * instState) {
     if (!registerUpdatePostponed) {
       state.regfile_temp[instState->ins.d_ldstBaseROffset.s][instState->ins.d_ldstBaseROffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
       std::cout
	 << m_name << std::hex
	 << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
       if (!instState->ins.d_ldstBaseROffset.s)
	 std::cout << " bank A" ;
       else std::cout << " bank B";
       std::cout << " register " << instState->ins.d_ldstBaseROffset.dst << std::endl;
#endif 
     }
     else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstBaseROffset.s,instState->ins.d_ldstBaseROffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E5
    // LDH (.unit) *+baseR[offsetR], dst   LDH (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldh_d_ldstBaseROffset_04_e5(InstructionState * instState) {
     if (!registerUpdatePostponed) {
       state.regfile_temp[instState->ins.d_ldstBaseROffset.s][instState->ins.d_ldstBaseROffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
       std::cout
	 << m_name << std::hex
	 << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
       if (!instState->ins.d_ldstBaseROffset.s)
	 std::cout << " bank A" ;
       else std::cout << " bank B";
       std::cout << " register " << instState->ins.d_ldstBaseROffset.dst << std::endl;
#endif 
     }
     else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstBaseROffset.s,instState->ins.d_ldstBaseROffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Halfword From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E1
    // LDHU (.unit) *+baseR[offsetR], dst   LDHU (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldhu_d_ldstBaseROffset_00_e5(InstructionState * instState) {
     if (!registerUpdatePostponed) {
       state.regfile_temp[instState->ins.d_ldstBaseROffset.s][instState->ins.d_ldstBaseROffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
       std::cout
	 << m_name << std::hex
	 << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
       if (!instState->ins.d_ldstBaseROffset.s)
	 std::cout << " bank A" ;
       else std::cout << " bank B";
       std::cout << " register " << instState->ins.d_ldstBaseROffset.dst << std::endl;    
#endif  
     }
     else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstBaseROffset.s,instState->ins.d_ldstBaseROffset.dst,instState->getData(instState->getLoadStorePosition()));
    }

    // Load Word From Memory With a 5-Bit Unsigned Constant Offset or Register Offset -- .unit = .D1 or .D2  pipeline phase E1
    // LDW (.unit) *+baseR[offsetR], dst    -- LDW (.unit) *+baseR[ucst5], dst
    void Tms320C6xIss::op_ldw_d_ldstBaseROffset_06_e5(InstructionState * instState) {
     if (!registerUpdatePostponed) {
       state.regfile_temp[instState->ins.d_ldstBaseROffset.s][instState->ins.d_ldstBaseROffset.dst] = instState->getData(instState->getLoadStorePosition());
#if TMS320C62_DEBUG
       std::cout
	 << m_name << std::hex
	 << " value " << instState->getData(instState->getLoadStorePosition()) << std::dec << " is written to ";
       if (!instState->ins.d_ldstBaseROffset.s)
	 std::cout << " bank A" ;
       else std::cout << " bank B";
       std::cout << " register " << instState->ins.d_ldstBaseROffset.dst << std::endl;    
#endif    
     }
     else Tms320C6xIss::registerToBeUpdateLater(instState->ins.d_ldstBaseROffset.s,instState->ins.d_ldstBaseROffset.dst,instState->getData(instState->getLoadStorePosition()));
    }


    void Tms320C6xIss::op_nothing_to_be_done(InstructionState * instState) {    }
    

    void Tms320C6xIss::op_illegal_d_e1(InstructionState * instState) {
      std::cerr << "Warning: unknown instruction with opcode "<< std::hex << instState->ins.d.op << std::dec 
		<< " in .D unit in E1 pipeline phase @ " << m_exec_cycles <<  " cycle, instruction is " 
		<< std::hex << instState->ins.ins
		<< " PC is " << state.DCE1.getPC() << std::dec << std::endl;
    }
	}}

      
// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
 
