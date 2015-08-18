/*****************************************************************************
  mocomp.h  -- File performing the inter prediction 

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

#ifndef __MOCOMP_H__
#define __MOCOMP_H__

/****************************************************************************
  Include section
****************************************************************************/
//#include "common.h"
#include "mode_pred.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
typedef struct L_MC_temp_block {
  int32_t p[9][9];
} L_MC_temp_block;

typedef struct C_MC_temp_block {
  int32_t p[3][3];
} C_MC_temp_block;

#define Filter(E,F,G,H,I,J) Clip1(((E)-5*(F)+20*(G)+20*(H)-5*(I)+(J)+16)>>5)
#define iffrac(x,y) if(frac==(y<<2)+x)
#define Mix(a,b) (((a)+(b)+1)>>1)


/****************************************************************************
  Non static functions
****************************************************************************/
void MotionCompensateTB(frame *cur, frame *ref, int32_t org_x, int32_t org_y,
			int16_t mvx, int16_t mvy);

void MotionCompensateMB(frame *cur, frame *ref, mode_pred_info *mpi,
                        int32_t org_x, int32_t org_y);

#endif /*__MOCOMP_H__*/
