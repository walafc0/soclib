/*****************************************************************************
  block.c -- File managing the block transformation 
  - ICT

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
#include "block.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
static const int32_t LevelScale[6] = { 10, 11, 13, 14, 16, 18 };
static const int32_t ZigZagOrder[] = { 0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15 };


/****************************************************************************
  Non static functions
****************************************************************************/
void enter_luma_block(int32_t *scan, frame * f, int32_t x, int32_t y, int32_t qp,
		      int32_t without_dc)
{
  direct_ict(inverse_quantize(coeff_scan(scan), qp, without_dc),
	     &L_pixel(f, x, y), f->Lpitch);
}

void enter_chroma_block(int32_t *scan, frame * f, int32_t iCbCr, int32_t x, int32_t y,
			int32_t qp, int32_t without_dc)
{
  direct_ict(inverse_quantize(coeff_scan(scan), qp, without_dc),
	     &C_pixel(f, iCbCr, x, y), f->Cpitch);
}

void transform_luma_dc(int32_t *scan, int32_t *out, int32_t qp)
{
  static const int32_t ScanOrder[16] =
    { 0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15 };
  core_block block = hadamard(coeff_scan(scan));
  int32_t scale = LevelScale[qp % 6];
  int32_t i;
  if (qp >= 12)
    for (i = 0; i < 16; ++i)
      out[ScanOrder[i] << 4] =
	(block.items[i] * scale) << (qp / 6 - 2);
  else {
    int32_t round_adj = 1 << (1 - qp / 6);
    for (i = 0; i < 16; ++i)
      out[ScanOrder[i] << 4] =
	(block.items[i] * scale + round_adj) >> (2 - qp / 6);
  }
}

void transform_chroma_dc(int32_t *scan, int32_t qp)
{
  int32_t scale = LevelScale[qp % 6];
  int32_t a = scan[0] + scan[1] + scan[2] + scan[3];
  int32_t b = scan[0] - scan[1] + scan[2] - scan[3];
  int32_t c = scan[0] + scan[1] - scan[2] - scan[3];
  int32_t d = scan[0] - scan[1] - scan[2] + scan[3];
  if (qp >= 6) {
    scan[0] = (a * scale) << (qp / 6 - 1);
    scan[1] = (b * scale) << (qp / 6 - 1);
    scan[2] = (c * scale) << (qp / 6 - 1);
    scan[3] = (d * scale) << (qp / 6 - 1);
  } else {
    scan[0] = (a * scale) >> 1;
    scan[1] = (b * scale) >> 1;
    scan[2] = (c * scale) >> 1;
    scan[3] = (d * scale) >> 1;
  }
}

core_block coeff_scan(int32_t *scan)
{
  core_block res;
  int32_t i;
  for (i = 0; i < 16; ++i)
    res.items[ZigZagOrder[i]] = scan[i];
  return res;
}
