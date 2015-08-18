/*****************************************************************************
  mbmodes.h  --   File decoding and setting the mb structure 

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

#ifndef __MBMODES_H__
#define __MBMODES_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "common.h"
#include "slicehdr.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
#define NA             -1

#define Intra_4x4       0
#define Intra_16x16     1
#define Pred_L0         2
#define Pred_L1         3
#define BiPred          4
#define Direct          5

#define P_L0_16x16      0
#define P_L0_L0_16x8    1
#define P_L0_L0_8x16    2
#define P_8x8           3
#define P_8x8ref0       4
#define I_4x4           5
#define I_16x16_0_0_0   6
#define I_16x16_1_0_0   7
#define I_16x16_2_0_0   8
#define I_16x16_3_0_0   9
#define I_16x16_0_1_0  10
#define I_16x16_1_1_0  11
#define I_16x16_2_1_0  12
#define I_16x16_3_1_0  13
#define I_16x16_0_2_0  14
#define I_16x16_1_2_0  15
#define I_16x16_2_2_0  16
#define I_16x16_3_2_0  17
#define I_16x16_0_0_1  18
#define I_16x16_1_0_1  19
#define I_16x16_2_0_1  20
#define I_16x16_3_0_1  21
#define I_16x16_0_1_1  22
#define I_16x16_1_1_1  23
#define I_16x16_2_1_1  24
#define I_16x16_3_1_1  25
#define I_16x16_0_2_1  26
#define I_16x16_1_2_1  27
#define I_16x16_2_2_1  28
#define I_16x16_3_2_1  29
#define I_PCM          30
#define P_Skip       0xFF

#define P_L0_8x8       0
#define P_L0_8x4       1
#define P_L0_4x8       2
#define P_L0_4x4       3
#define B_Direct_8x8   4

#define IsInter(m)  (((m)>=0 && (m)<5) || (m)==P_Skip)
#define IsIntra(m)  ((m)>=5 && (m)<=I_PCM)


typedef struct _mb_mode {
  int32_t mb_type;
  int32_t NumMbPart;
  int32_t MbPartPredMode[2];
  int32_t Intra16x16PredMode;
  int32_t MbPartWidth;
  int32_t MbPartHeight;
  int32_t CodedBlockPatternChroma;
  int32_t CodedBlockPatternLuma;
} mb_mode;

typedef struct _sub_mb_mode {
  int32_t sub_mb_type;
  int32_t NumSubMbPart;
  int32_t SubMbPredMode;
  int32_t SubMbPartWidth;
  int32_t SubMbPartHeight;
} sub_mb_mode;


/****************************************************************************
  Non static functions
****************************************************************************/
void decode_mb_mode(mb_mode *mb, int32_t slice_type, int32_t raw_mb_type);
void decode_sub_mb_mode(sub_mb_mode *sub, int32_t slice_type, int32_t raw_sub_mb_type);

const char *_str_mb_type(int32_t mb_type);
const char *_str_sub_mb_type(int32_t sub_mb_type);
const char *_str_pred_mode(int32_t pred_mode);
void _dump_mb_mode(mb_mode *mb);

#endif /*__MBMODES_H__*/
