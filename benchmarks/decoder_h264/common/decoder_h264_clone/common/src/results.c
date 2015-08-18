/*****************************************************************************
  results.c -- File giving functions to screen infos about sh, pps, mb... 

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
#include "results.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
static int32_t cpt = 1;
static int32_t cpt2 = 1;


/****************************************************************************
  Non static functions
****************************************************************************/
void results_screening_sh(slice_header * sh)
{
  printf("\n--------Slice Header--------\n");
  printf("first_mb_in_slice :%d\n", sh->first_mb_in_slice);
  printf("slice_type :%d == %s\n", sh->slice_type,
	 _str_slice_type(sh->slice_type));
  printf("pic_parameter_set_id :%d\n", sh->pic_parameter_set_id);
  printf("frame_num :%d\n", sh->frame_num);
  printf("field_pic_flag :%d\n", sh->field_pic_flag);
  printf("MbaffFrameFlag :%d\n", sh->MbaffFrameFlag);
  printf("PicHeightInMbs :%d\n", sh->PicHeightInMbs);
  printf("PicHeightInSamples :%d\n", sh->PicHeightInSamples);
  printf("PicSizeInMbs :%d\n", sh->PicSizeInMbs);
  printf("bottom_field_flag :%d\n", sh->bottom_field_flag);
  printf("idr_pic_id :%d\n", sh->idr_pic_id);
  printf("pic_order_cnt_lsb :%d\n", sh->pic_order_cnt_lsb);
  printf("delta_pic_order_cnt_bottom :%d\n",
	 sh->delta_pic_order_cnt_bottom);
  printf("delta_pic_order_cnt[2] :{%d,%d}\n", sh->delta_pic_order_cnt[0],
	 sh->delta_pic_order_cnt[1]);
  printf("redundant_pic_cnt :%d\n", sh->redundant_pic_cnt);
  printf("direct_spatial_mv_pred_flag :%d\n",
	 sh->direct_spatial_mv_pred_flag);

  printf("num_ref_idx_active_override_flag :%d\n",
	 sh->num_ref_idx_active_override_flag);
  printf("num_ref_idx_l0_activ :%d\n", sh->num_ref_idx_l0_active);
  printf("num_ref_idx_l1_active :%d\n", sh->num_ref_idx_l1_active);
  printf("ref_pic_list_reordering_flag_l0 :%d\n",
	 sh->ref_pic_list_reordering_flag_l0);
  printf("ref_pic_list_reordering_flag_l1 :%d\n",
	 sh->ref_pic_list_reordering_flag_l1);
  printf("no_output_of_prior_pics_flag :%d\n",
	 sh->no_output_of_prior_pics_flag);
  printf("long_term_reference_flag :%d\n", sh->long_term_reference_flag);
  printf("adaptive_ref_pic_marking_mode_flag :%d\n",
	 sh->adaptive_ref_pic_marking_mode_flag);
  printf("cabac_init_idc :%d\n", sh->cabac_init_idc);
  printf("slice_qp_delta :%d\n", sh->slice_qp_delta);
  printf("idr_pic_id :%d\n", sh->idr_pic_id);
  printf("SliceQPy :%d\n", sh->SliceQPy);
  printf("sp_for_switch_flag :%d\n", sh->sp_for_switch_flag);
  printf("slice_qs_delta :%d\n", sh->slice_qs_delta);
  printf("disable_deblocking_filter_idc :%d\n",
	 sh->disable_deblocking_filter_idc);
  printf("slice_alpha_c0_offset_div2 :%d\n",
	 sh->slice_alpha_c0_offset_div2);
  printf("slice_beta_offset_div2 :%d\n", sh->slice_beta_offset_div2);
  printf("slice_group_change_cycle :%d\n\n",
	 sh->slice_group_change_cycle);
}

void results_screening_sps(seq_parameter_set * sps)
{
  printf("\n--------Seq_Parameter_Set--------\n");
  printf("profile_idc :%d\n", sps->profile_idc);
  printf("constraint_set0_flag :%d\n", sps->constraint_set0_flag);
  printf("constraint_set1_flag :%d\n", sps->constraint_set1_flag);
  printf("constraint_set2_flag :%d\n", sps->constraint_set2_flag);
  printf("reserved_zero_5bits :%d\n", sps->reserved_zero_5bits);
  printf("level_idc :%d\n", sps->level_idc);
  printf("seq_parameter_set_id :%d\n", sps->seq_parameter_set_id);
  printf("log2_max_frame_num :%d\n", sps->log2_max_frame_num);
  printf("MaxFrameNum :%d\n", sps->MaxFrameNum);
  printf("pic_order_cnt_type :%d\n", sps->pic_order_cnt_type);
  printf("log2_max_pic_order_cnt_lsb :%d\n",
	 sps->log2_max_pic_order_cnt_lsb);
  printf("MaxPicOrderCntLsb :%d\n", sps->MaxPicOrderCntLsb);
  printf("delta_pic_order_always_zero_flag :%d\n",
	 sps->delta_pic_order_always_zero_flag);
  printf("offset_for_non_ref_pic :%d\n", sps->offset_for_non_ref_pic);
  printf("offset_for_top_to_bottom_field :%d\n",
	 sps->offset_for_top_to_bottom_field);
  printf("num_ref_frames_in_pic_order_cnt_cycle :%d\n",
	 sps->num_ref_frames_in_pic_order_cnt_cycle);
  printf("offset_for_ref_frame[255] :%d\n",
	 sps->offset_for_ref_frame[255]);
  printf("num_ref_frames :%d\n", sps->num_ref_frames);
  printf("gaps_in_frame_num_value_allowed_flag :%d\n",
	 sps->gaps_in_frame_num_value_allowed_flag);
  printf("PicWidthInMbs :%d\n", sps->PicWidthInMbs);
  printf("PicWidthInSamples :%d\n", sps->PicWidthInSamples);
  printf("PicHeightInMapUnits :%d\n", sps->PicHeightInMapUnits);
  printf("PicSizeInMapUnits :%d\n", sps->PicSizeInMapUnits);
  printf("FrameHeightInMbs :%d\n", sps->FrameHeightInMbs);
  printf("FrameHeightInSamples :%d\n", sps->FrameHeightInSamples);
  printf("frame_mbs_only_flag :%d\n", sps->frame_mbs_only_flag);
  printf("mb_adaptive_frame_field_flag :%d\n",
	 sps->mb_adaptive_frame_field_flag);
  printf("direct_8x8_inference_flag :%d\n",
	 sps->direct_8x8_inference_flag);
  printf("frame_cropping_flag :%d\n", sps->frame_cropping_flag);
  printf("frame_crop_left_offset :%d\n", sps->frame_crop_left_offset);
  printf("frame_crop_right_offset :%d\n", sps->frame_crop_right_offset);
  printf("frame_crop_top_offset :%d\n", sps->frame_crop_top_offset);
  printf("frame_crop_bottom_offset :%d\n",
	 sps->frame_crop_bottom_offset);
  printf("vui_parameters_present_flag :%d\n",
	 sps->vui_parameters_present_flag);
}

void results_screening_pps(pic_parameter_set * pps)
{
  printf("\n--------Pic_Parameter_Set--------\n");
  printf("pic_parameter_set_id :%d\n", pps->pic_parameter_set_id);
  printf("seq_parameter_set_id :%d\n", pps->seq_parameter_set_id);
  printf("entropy_coding_mode_flag :%d\n",
	 pps->entropy_coding_mode_flag);
  printf("pic_order_present_flag :%d\n", pps->pic_order_present_flag);
  printf("num_slice_groups :%d\n", pps->num_slice_groups);
  printf("slice_group_map_type :%d\n", pps->slice_group_map_type);
  printf("run_length[8] :{%d,%d,%d,%d,%d,%d,%d,%d}\n",
	 pps->run_length[0], pps->run_length[1], pps->run_length[2],
	 pps->run_length[3], pps->run_length[4], pps->run_length[5],
	 pps->run_length[6], pps->run_length[7]);
  printf("top_left[8] :{%d,%d,%d,%d,%d,%d,%d,%d}\n", pps->top_left[0],
	 pps->top_left[1], pps->top_left[2], pps->top_left[3],
	 pps->top_left[4], pps->top_left[5], pps->top_left[6],
	 pps->top_left[7]);
  printf("bottom_right[8] :{%d,%d,%d,%d,%d,%d,%d,%d}\n",
	 pps->bottom_right[0], pps->bottom_right[1],
	 pps->bottom_right[2], pps->bottom_right[3],
	 pps->bottom_right[4], pps->bottom_right[5],
	 pps->bottom_right[6], pps->bottom_right[7]);
  printf("slice_group_change_direction_flag :%d\n",
	 pps->slice_group_change_direction_flag);
  printf("SliceGroupChangeRate :%d\n", pps->SliceGroupChangeRate);
  printf("pic_size_in_map_units :%d\n", pps->pic_size_in_map_units);
  printf("slice_group_id[8192] :%d\n", pps->slice_group_id[0]);
  printf("num_ref_idx_l0_active :%d\n", pps->num_ref_idx_l0_active);
  printf("num_ref_idx_l1_active :%d\n", pps->num_ref_idx_l1_active);
  printf("weighted_pred_flag :%d\n", pps->weighted_pred_flag);
  printf("weighted_bipred_idc :%d\n", pps->weighted_bipred_idc);
  printf("pic_init_qp :%d\n", pps->pic_init_qp);
  printf("pic_init_qs :%d\n", pps->pic_init_qs);
  printf("chroma_qp_index_offset :%d\n", pps->chroma_qp_index_offset);
  printf("deblocking_filter_control_present_flag :%d\n",
	 pps->deblocking_filter_control_present_flag);
  printf("constrained_intra_pred_flag :%d\n",
	 pps->constrained_intra_pred_flag);
  printf("redundant_pic_cnt_present_flag :%d\n",
	 pps->redundant_pic_cnt_present_flag);
}

void results_screening_nalu(nal_unit * nalu)
{
  printf("\n--------Nal_Unit-------- number :%i\n", cpt);
  printf("NumBytesInNALunit :%d\n", nalu->NumBytesInNALunit);
  printf("forbidden_zero_bit :%d\n", nalu->forbidden_zero_bit);
  printf("nal_ref_idc :%d\n", nalu->nal_ref_idc);
  printf("nal_unit_type :%d == %s\n", nalu->nal_unit_type,
	 _str_nal_unit_type(nalu->nal_unit_type));
  printf("last_rbsp_byte :%i\n", (int32_t) *(nalu->last_rbsp_byte));	//PB
  cpt++;
}

void results_screening_mb(mb_mode * mb)
{
  printf("\n-------- Macro Bloc -------- number :%i\n", cpt2);
  printf("mb_type :%d\n", mb->mb_type);
  printf("NumMbPart :%d\n", mb->NumMbPart);
  printf("MbPartPredMode[2] : {%d,%d}\n", mb->MbPartPredMode[0],
	 mb->MbPartPredMode[1]);
  printf("Intra16x16PredMode :%d\n", mb->Intra16x16PredMode);
  printf("MbPartWidth :%d\n", mb->MbPartWidth);
  printf("MbPartHeight :%d\n", mb->MbPartHeight);
  printf("CodedBlockPatternChroma :%d\n", mb->CodedBlockPatternChroma);
  printf("CodedBlockPatternLuma :%d\n", mb->CodedBlockPatternLuma);
  cpt2++;
}
