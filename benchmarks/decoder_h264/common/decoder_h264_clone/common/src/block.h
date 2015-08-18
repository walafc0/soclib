/*****************************************************************************
  block.h -- File managing the block transformation
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

#ifndef __BLOCK_H__
#define __BLOCK_H__


/****************************************************************************
  Include section
****************************************************************************/
#include "coretrans.h"
#include "global.h"


/****************************************************************************
  Non static functions
****************************************************************************/
void enter_luma_block(int32_t *scan, frame *f, int32_t x, int32_t y, int32_t qp, int32_t without_dc);
void enter_chroma_block(int32_t *scan, frame *f, int32_t iCbCr, int32_t x, int32_t y, int32_t qp, int32_t without_dc);

void transform_luma_dc(int32_t *scan, int32_t *out, int32_t qp);
void transform_chroma_dc(int32_t *scan, int32_t qp);

core_block coeff_scan(int32_t *scan);

#endif /*__BLOCK_H__*/
