/*****************************************************************************
  intra_pred.h  -- File performing the intra prediction 

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

#ifndef __INTRA_PRED_H__
#define __INTRA_PRED_H__

/****************************************************************************
  Include section
****************************************************************************/
//#include "common.h"
#include "mode_pred.h"
//#include "mbmodes.h"
//#include "global.h"


/****************************************************************************
  Non static functions
****************************************************************************/
void Intra_4x4_Dispatch(frame *f, mode_pred_info *mpi, int32_t x, int32_t y, int32_t luma4x4BlkIdx);
void Intra_16x16_Dispatch(frame *f, mode_pred_info *mpi, int32_t mode, int32_t x, int32_t y, int32_t constrained_intra_pred);
void Intra_Chroma_Dispatch(frame *f, mode_pred_info *mpi, int32_t mode, int32_t x, int32_t y, int32_t constrained_intra_pred);

#endif /*__INTRA_PRED_H__*/
