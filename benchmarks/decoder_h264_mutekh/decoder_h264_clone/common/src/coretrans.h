/*****************************************************************************
  coretrans.h -- File managing the block transformation

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

#ifndef __CORETRANS_H__
#define __CORETRANS_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "common.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
typedef struct _core_block 
{
  int32_t items[16];
} core_block;

#define CoreBlock(b,i,j) (b).items[(j)|((i)<<2)]

#define IntraRoundingMode  3
#define InterRoundingMode  6

#define inverse_core_transform inverse_core_transform_fast


/****************************************************************************
  Non static functions
****************************************************************************/
core_block forward_core_transform(core_block original);
core_block forward_quantize(core_block raw, int32_t quantizer, int32_t rounding_mode);
core_block inverse_quantize(core_block quantized, int32_t quantizer, int32_t without_dc);
core_block inverse_core_transform_slow(core_block coeff);
core_block inverse_core_transform_fast(core_block coeff);
core_block hadamard(core_block coeff);

void direct_ict(core_block coeff, unsigned char *img, int32_t pitch);

void _dump_core_block(core_block block);

#endif /*__CORETRANS_H__*/
