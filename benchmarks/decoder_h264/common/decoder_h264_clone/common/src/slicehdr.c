/*****************************************************************************
  slicehdr.c -- File retrieving all the information about the slice 

  Original Author: 
  Martin Fiedler, CHEMNITZ UNIVERSITY OF TECHNOLOGY, 2004-06-01	

  Thales Authors:
  Florian Broekaert, THALES COMMUNICATIONS - AAL, 2006
  Fabien Colas-Bigey, THALES COMMUNICATIONS - AAL, 2008

  Copyright (C) THALES & Martin Fiedler All rights reserved.
 
  This code is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.
   
  This code is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.
   
  You should have received a copy of the GNU General Public License along
  with this code (see file COPYING).  If not, see
  <http://www.gnu.org/licenses/>.
  
  This License does not grant permission to use the name of the copyright
  owner, except only as required above for reproducing the content of
  the copyright notice.
*****************************************************************************/

/****************************************************************************
  Include section
****************************************************************************/
#include "slicehdr.h"


/****************************************************************************
  Non static functions
****************************************************************************/
static void skip_ref_pic_list_reordering(int32_t thread_ID)
{
  int32_t reordering_of_pic_nums_idc;
  int32_t abs_diff_pic_num;
  int32_t long_term_pic_num;

  printf("Warning: I do not support reference picture list reordering.\n"
	 "         Watch out for decoding errors!\n");
  do {
    reordering_of_pic_nums_idc =
      get_unsigned_exp_golomb_thread(thread_ID);
    if (reordering_of_pic_nums_idc == 0
	|| reordering_of_pic_nums_idc == 1)
      abs_diff_pic_num =
	get_unsigned_exp_golomb_thread(thread_ID) + 1;
    else if (reordering_of_pic_nums_idc == 2)
      long_term_pic_num = get_unsigned_exp_golomb_thread(thread_ID);
  }
  while (reordering_of_pic_nums_idc != 3);
}

static void skip_adaptive_ref_pic_marking(int32_t thread_ID)
{
  int32_t memory_management_control_operation;
  int32_t difference_of_pic_nums;
  int32_t long_term_pic_num;
  int32_t long_term_frame_idx;
  int32_t max_long_term_frame_idx;
  printf("Warning: I do not support adaptive reference picture marking.\n"
	 "         Watch out for decoding errors!\n");
  do {
    memory_management_control_operation =
      get_unsigned_exp_golomb_thread(thread_ID);
    if (memory_management_control_operation == 1
	|| memory_management_control_operation == 3)
      difference_of_pic_nums =
	get_unsigned_exp_golomb_thread(thread_ID) + 1;
    if (memory_management_control_operation == 2)
      long_term_pic_num = get_unsigned_exp_golomb_thread(thread_ID);
    if (memory_management_control_operation == 3
	|| memory_management_control_operation == 6)
      long_term_frame_idx =
	get_unsigned_exp_golomb_thread(thread_ID);
    if (memory_management_control_operation == 4)
      max_long_term_frame_idx =
	get_unsigned_exp_golomb_thread(thread_ID) - 1;
  }
  while (memory_management_control_operation != 0);
}


/****************************************************************************
  Non static functions
****************************************************************************/
/*+1 parameter for the multislice management*/
void decode_slice_header(slice_header * sh,
			 seq_parameter_set * sps,
			 pic_parameter_set * pps,
			 nal_unit * nalu, int32_t thread_ID)
{
  memset((void *) sh, 0, sizeof(slice_header));
  sh->first_mb_in_slice = get_unsigned_exp_golomb_thread(thread_ID);
  sh->slice_type = get_unsigned_exp_golomb_thread(thread_ID) % 5;
  sh->pic_parameter_set_id = get_unsigned_exp_golomb_thread(thread_ID);
  sh->frame_num =
    input_get_bits_thread(sps->log2_max_frame_num, thread_ID);
  if (!sps->frame_mbs_only_flag) {
    sh->field_pic_flag = input_get_one_bit_thread(thread_ID);
    if (sh->field_pic_flag)
      sh->bottom_field_flag = input_get_one_bit_thread(thread_ID);
  }
  sh->MbaffFrameFlag = (sps->mb_adaptive_frame_field_flag
			&& !sh->field_pic_flag);
  sh->PicHeightInMbs = sps->FrameHeightInMbs / (1 + sh->field_pic_flag);
  sh->PicHeightInSamples = (sh->PicHeightInMbs) << 4;
  sh->PicSizeInMbs = sps->PicWidthInMbs * sh->PicHeightInMbs;
  if (nalu->nal_unit_type == 5)
    sh->idr_pic_id = get_unsigned_exp_golomb_thread(thread_ID);
  if (sps->pic_order_cnt_type == 0) {
    sh->pic_order_cnt_lsb =
      input_get_bits_thread(sps->log2_max_pic_order_cnt_lsb,
			    thread_ID);
    if (pps->pic_order_present_flag && !sh->field_pic_flag)
      sh->delta_pic_order_cnt_bottom =
	get_signed_exp_golomb_thread(thread_ID);
  }
  if (sps->pic_order_cnt_type == 1
      && !sps->delta_pic_order_always_zero_flag) {
    sh->delta_pic_order_cnt[0] =
      get_signed_exp_golomb_thread(thread_ID);
    if (pps->pic_order_present_flag && !sh->field_pic_flag)
      sh->delta_pic_order_cnt[1] =
	get_signed_exp_golomb_thread(thread_ID);
  }
  if (pps->redundant_pic_cnt_present_flag)
    sh->redundant_pic_cnt = get_unsigned_exp_golomb_thread(thread_ID);
  if (sh->slice_type == B_SLICE)
    sh->direct_spatial_mv_pred_flag =
      input_get_one_bit_thread(thread_ID);
  if (sh->slice_type == P_SLICE || sh->slice_type == B_SLICE
      || sh->slice_type == SP_SLICE) {
    sh->num_ref_idx_active_override_flag =
      input_get_one_bit_thread(thread_ID);
    if (sh->num_ref_idx_active_override_flag) {
      sh->num_ref_idx_l0_active =
	get_unsigned_exp_golomb_thread(thread_ID) + 1;
      if (sh->slice_type == B_SLICE)
	sh->num_ref_idx_l1_active =
	  get_unsigned_exp_golomb_thread(thread_ID) + 1;
    }
  }
  // ref_pic_list_reordering()
  if (sh->slice_type != I_SLICE && sh->slice_type != SI_SLICE) {
    sh->ref_pic_list_reordering_flag_l0 =
      input_get_one_bit_thread(thread_ID);
    if (sh->ref_pic_list_reordering_flag_l0)
      skip_ref_pic_list_reordering(thread_ID);
  }
  if (sh->slice_type == B_SLICE) {
    sh->ref_pic_list_reordering_flag_l1 =
      input_get_one_bit_thread(thread_ID);
    if (sh->ref_pic_list_reordering_flag_l1)
      skip_ref_pic_list_reordering(thread_ID);
  }
  if ((pps->weighted_pred_flag
       && (sh->slice_type == P_SLICE || sh->slice_type == SP_SLICE))
      || (pps->weighted_bipred_idc == 1 && sh->slice_type == B_SLICE)) {
    printf("sorry, I _really_ do not support weighted prediction!\n");
    //exit(1);
    abort();
  }
  if (nalu->nal_ref_idc != 0) {
    // dec_ref_pic_marking()
    if (nalu->nal_unit_type == 5) {
      sh->no_output_of_prior_pics_flag =
	input_get_one_bit_thread(thread_ID);
      sh->long_term_reference_flag =
	input_get_one_bit_thread(thread_ID);
    } else {
      sh->adaptive_ref_pic_marking_mode_flag =
	input_get_one_bit_thread(thread_ID);
      if (sh->adaptive_ref_pic_marking_mode_flag)
	skip_adaptive_ref_pic_marking(thread_ID);
    }
  }
  if (pps->entropy_coding_mode_flag && sh->slice_type != I_SLICE
      && sh->slice_type != SI_SLICE)
    sh->cabac_init_idc = get_unsigned_exp_golomb_thread(thread_ID);
  sh->slice_qp_delta = get_signed_exp_golomb_thread(thread_ID);
  sh->SliceQPy = pps->pic_init_qp + sh->slice_qp_delta;
  if (sh->slice_type == SP_SLICE || sh->slice_type == SI_SLICE) {
    if (sh->slice_type == SP_SLICE)
      sh->sp_for_switch_flag = input_get_one_bit_thread(thread_ID);
    sh->slice_qs_delta = get_signed_exp_golomb_thread(thread_ID);
  }
  if (pps->deblocking_filter_control_present_flag) {
    sh->disable_deblocking_filter_idc =
      get_unsigned_exp_golomb_thread(thread_ID);
    if (sh->disable_deblocking_filter_idc != 1) {
      sh->slice_alpha_c0_offset_div2 =
	get_signed_exp_golomb_thread(thread_ID);
      sh->slice_beta_offset_div2 =
	get_signed_exp_golomb_thread(thread_ID);
    }
  }
  if (pps->num_slice_groups > 1 && pps->slice_group_map_type >= 3
      && pps->slice_group_map_type <= 5) {

    /*Modif Read correct number of bits according to the JVT norm paper */
    sh->slice_group_change_cycle =
      input_get_bits_thread(log_2
			    (sps->PicSizeInMapUnits /
			     pps->SliceGroupChangeRate + 1),
			    thread_ID);
    //printf("sh->slice_group_change_cycle = %d\n",sh->slice_group_change_cycle);

  }
}

const char *_str_slice_type(int32_t type)
{
  switch (type) {
  case P_SLICE:
  case P_SLICE + 5:
    return "P-Slice";
  case B_SLICE:
  case B_SLICE + 5:
    return "B-Slice";
  case I_SLICE:
  case I_SLICE + 5:
    return "I-Slice";
  case SP_SLICE:
  case SP_SLICE + 5:
    return "SP-Slice";
  case SI_SLICE:
  case SI_SLICE + 5:
    return "SI-Slice";
  }
  return "Illegal Slice";
}

