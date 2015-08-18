/*****************************************************************************

  slice.h -- File where all the major steps for decoding are done 
 -decode_slice_data_function call all the others functions
  to manage the decoding process

  Original Author: 
  Martin Fiedler, CHEMNITZ UNIVERSITY OF TECHNOLOGY, 2004-06-01	
  
  Thales Author:
  Florian Broekaert, THALES COM - AAL, 2006-04-07
  Fabien Colas-Bigey THALES COM - AAL, 2008
  Pierre-Edouard BEAUCAMPS, THALES COM - AAL, 2009

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

#ifndef __SLICE_H__
#define __SLICE_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "soclib_addresses.h"
#include <hexo/iospace.h>
#include <hexo/endian.h>

#include "../common/src/mocomp.h"
#include "../common/src/intra_pred.h"
#include "../common/src/residual.h"
#include "../common/src/block.h"
#include "../common/src/nal_thread.h"
#ifdef PRINT_PARAM_SET
  #include "../common/src/results.h"
#endif

#include "../common_soclib/src/flags.h"


/*****************************************************************************************
  Define section of some macros for easier access to the various ModePredInfo structures
*****************************************************************************************/
#define LumaDC_nC		get_luma_nC(mpi,mb_pos_x,mb_pos_y)
#define LumaAC_nC		get_luma_nC(mpi,mb_pos_x+Intra4x4ScanOrder[(i8x8<<2)+i4x4][0],mb_pos_y+Intra4x4ScanOrder[(i8x8<<2)+i4x4][1])
#define ChromaDC_nC		-1
#define ChromaAC_nC		get_chroma_nC(mpi,mb_pos_x+((i4x4&1)<<3),mb_pos_y+((i4x4>>1)<<3),iCbCr)
#define LumaAdjust(val)		set_ModePredInfo_TotalCoeffL(mpi,(mb_pos_x+Intra4x4ScanOrder[(i8x8<<2)+i4x4][0])>>2,(mb_pos_y+Intra4x4ScanOrder[(i8x8<<2)+i4x4][1])>>2, val)
#define ChromaAdjust(val)	set_ModePredInfo_TotalCoeffC(mpi,(mb_pos_x+((i4x4&1)<<3))>>3,(mb_pos_y+((i4x4>>1)<<3))>>3,iCbCr,val)
#define Intra4x4PredMode(i,val)	set_ModePredInfo_Intra4x4PredMode(mpi,(mb_pos_x+Intra4x4ScanOrder[i][0])>>2,(mb_pos_y+Intra4x4ScanOrder[i][1])>>2,val)


/****************************************************************************
  Non static functions
****************************************************************************/
void decode_slice_data(slice_header *sh,
                       seq_parameter_set *sps, pic_parameter_set *pps,
                       nal_unit *nalu,
                       frame *this, frame *ref,
                       mode_pred_info *mpi);
void fb_display_frame(frame *this);

#endif /*__SLICE_H__*/
