/*****************************************************************************
  params.h -- File retrieving all the information about sps and pps
  -Decode sps & pps
  -Check all the unsupported features

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
#ifndef __PARAMS_H__
#define __PARAMS_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "common.h"
#include "input.h"
//#include "nal.h"
#include "cavlc.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
typedef struct _seq_parameter_set {
  int32_t profile_idc;
  int32_t constraint_set0_flag;
  int32_t constraint_set1_flag;
  int32_t constraint_set2_flag;
  int32_t reserved_zero_5bits;
  int32_t level_idc;
  int32_t seq_parameter_set_id;
  int32_t log2_max_frame_num;
  int32_t MaxFrameNum;
  int32_t pic_order_cnt_type;
  int32_t log2_max_pic_order_cnt_lsb;
  int32_t MaxPicOrderCntLsb;
  int32_t delta_pic_order_always_zero_flag;
  int32_t offset_for_non_ref_pic;
  int32_t offset_for_top_to_bottom_field;
  int32_t num_ref_frames_in_pic_order_cnt_cycle;
  int32_t offset_for_ref_frame[256];
  int32_t num_ref_frames;
  int32_t gaps_in_frame_num_value_allowed_flag;
  int32_t PicWidthInMbs;
  int32_t PicWidthInSamples;
  int32_t PicHeightInMapUnits;
  int32_t PicSizeInMapUnits;
  int32_t FrameHeightInMbs;
  int32_t FrameHeightInSamples;
  int32_t frame_mbs_only_flag;
  int32_t mb_adaptive_frame_field_flag;
  int32_t direct_8x8_inference_flag;
  int32_t frame_cropping_flag;
  int32_t frame_crop_left_offset;
  int32_t frame_crop_right_offset;
  int32_t frame_crop_top_offset;
  int32_t frame_crop_bottom_offset;
  int32_t vui_parameters_present_flag;
} seq_parameter_set;

typedef struct _pic_parameter_set {
  int32_t pic_parameter_set_id;
  int32_t seq_parameter_set_id;
  int32_t entropy_coding_mode_flag;
  int32_t pic_order_present_flag;
  int32_t num_slice_groups;
  int32_t slice_group_map_type;
  int32_t run_length[8];
  int32_t top_left[8];
  int32_t bottom_right[8];
  int32_t slice_group_change_direction_flag;
  int32_t SliceGroupChangeRate;
  int32_t pic_size_in_map_units;
  int32_t slice_group_id[8192];
  int32_t num_ref_idx_l0_active;
  int32_t num_ref_idx_l1_active;
  int32_t weighted_pred_flag;
  int32_t weighted_bipred_idc;
  int32_t pic_init_qp;
  int32_t pic_init_qs;
  int32_t chroma_qp_index_offset;
  int32_t deblocking_filter_control_present_flag;
  int32_t constrained_intra_pred_flag;
  int32_t redundant_pic_cnt_present_flag;
} pic_parameter_set;


/****************************************************************************
  Non static functions
****************************************************************************/
void decode_seq_parameter_set(seq_parameter_set *sps);
void decode_pic_parameter_set(pic_parameter_set *pps);
int32_t check_unsupported_features(seq_parameter_set *sps, pic_parameter_set *pps);

#endif /*__PARAMS_H__*/
