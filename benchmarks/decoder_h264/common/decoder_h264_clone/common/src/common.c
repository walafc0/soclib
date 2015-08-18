/*****************************************************************************
  common.c -- File performing the memory allocation of the frame and
  containing some common utilities

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
#include "common.h"


/****************************************************************************
  Non static functions
****************************************************************************/
frame *alloc_frame(int32_t width, int32_t height)
{
  frame *f = (frame *)calloc(1, sizeof(frame));
  f->Lwidth = f->Lpitch = width;
  f->Lheight = height;
  f->L = (unsigned char *)malloc(width * height);
  f->Cwidth = f->Cpitch = width >> 1;
  f->Cheight = height >> 1;
  f->C[0] = (unsigned char *)malloc((width * height) >> 2);
  f->C[1] = (unsigned char *)malloc((width * height) >> 2);
  return f;
}

void free_frame(frame * f)
{
  if (!f)
    return;
  if (f->L)
    free(f->L);
  if (f->C[0])
    free(f->C[0]);
  if (f->C[1])
    free(f->C[1]);
  
  free(f);
}

int32_t log_2(int32_t x)
{
  unsigned char cpt = 0;
  unsigned char p2 = 1;

  while (x != 1) {
    if ((x & 1) == 1) {
      p2 = 0;
    }
    x = x >> 1;		//Shift x, as long as x != 1
    cpt++;
  }
  if (p2 == 0) {
    // Add one to cpt to have the number of bits and not the position
    // if MSB pos is 2 then number of bits is 3
    cpt++;
  }
  return cpt;			//The number of shift will give the result of the log2
}
