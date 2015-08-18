/*****************************************************************************
  slicehdr.h -- File retrieving all the information about the slice 

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

#ifndef __SLICEHDR_H__
#define __SLICEHDR_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "common.h"
#include "input.h"
#include "nal.h"
#include "cavlc.h"
#include "params.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
#define  P_SLICE  0
#define  B_SLICE  1
#define  I_SLICE  2
#define SP_SLICE  3
#define SI_SLICE  4

typedef struct _slice_header {
  int32_t first_mb_in_slice;
  int32_t slice_type;
  int32_t pic_parameter_set_id;
  int32_t frame_num;
  int32_t field_pic_flag;
  int32_t MbaffFrameFlag;
  int32_t PicHeightInMbs;
  int32_t PicHeightInSamples;
  int32_t PicSizeInMbs;
  int32_t bottom_field_flag;
  int32_t idr_pic_id;
  int32_t pic_order_cnt_lsb;
  int32_t delta_pic_order_cnt_bottom;
  int32_t delta_pic_order_cnt[2];
  int32_t redundant_pic_cnt;
  int32_t direct_spatial_mv_pred_flag;
  int32_t num_ref_idx_active_override_flag;
  int32_t num_ref_idx_l0_active;
  int32_t num_ref_idx_l1_active;
  int32_t ref_pic_list_reordering_flag_l0;
  int32_t ref_pic_list_reordering_flag_l1;
  // <dec_ref_pic_marking>
  int32_t no_output_of_prior_pics_flag;
  int32_t long_term_reference_flag;
  int32_t adaptive_ref_pic_marking_mode_flag;
  // </dec_ref_pic_marking>
  int32_t cabac_init_idc;
  int32_t slice_qp_delta;
  int32_t SliceQPy;
  int32_t sp_for_switch_flag;
  int32_t slice_qs_delta;
  int32_t disable_deblocking_filter_idc;
  int32_t slice_alpha_c0_offset_div2;
  int32_t slice_beta_offset_div2;
  int32_t slice_group_change_cycle;
} slice_header;


/****************************************************************************
  Non static functions
****************************************************************************/
void decode_slice_header(slice_header *sh,
                         seq_parameter_set *sps,
                         pic_parameter_set *pps,
                         nal_unit *nalu,
			 int32_t thread_ID);

const char *_str_slice_type(int32_t type);

#endif /*__SLICEHDR_H__*/

