/*****************************************************************************
  global.h -- File containing global information used by many blocks of the
              treatment

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

#ifndef _GLOBALS_
#define _GLOBALS_

/****************************************************************************
  Include section
****************************************************************************/
#include <stdint.h>


/****************************************************************************
  Variables section
****************************************************************************/
typedef struct __frame 
{
  int32_t Lwidth,Lheight,Lpitch;
  int32_t Cwidth,Cheight,Cpitch;
  unsigned char *L, *C[2];
} frame;

#define H264_WIDTH(info)  ((info)&0xFFFF)
#define H264_HEIGHT(info) ((info)>>16)
#define L_pixel(f,x,y)   (f->L[(y)*f->Lpitch+(x)])
#define Cr_pixel(f,x,y) (f->C[1][(y)*f->Cpitch+(x)])
#define Cb_pixel(f,x,y) (f->C[0][(y)*f->Cpitch+(x)])
#define C_pixel(f,iCbCr,x,y) (f->C[iCbCr][(y)*f->Cpitch+(x)])

#endif
