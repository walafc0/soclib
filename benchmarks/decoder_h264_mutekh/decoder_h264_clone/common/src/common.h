/*****************************************************************************
  common.h -- File performing the memory allocation of the frame and
  containing some common utilities.


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

#ifndef __COMMON_H__
#define __COMMON_H__

/****************************************************************************
  Include section
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
#define DEBUG_MPI 0

#define ExtractSign(x) ((x)>>31)
#define CombineSign(sign,value) ((sign)?(-(value)):(value))

#define CustomClip(i,min,max) (((i)<min)?min:(((i)>max)?max:(i)))
#define Clip(i) CustomClip(i,0,255)


/****************************************************************************
  Non static functions
****************************************************************************/
frame *alloc_frame(int32_t width, int32_t height);
void free_frame(frame *f);

int32_t log_2(int32_t x);

#endif /*__COMMON_H__*/
