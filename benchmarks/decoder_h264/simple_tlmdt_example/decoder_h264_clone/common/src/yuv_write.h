/*****************************************************************************
  yuv_write.h -- File giving function to write an output yuv stream 
  
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

#ifndef __YUV_WRITE_H__
#define __YUV_WRITE_H__

/****************************************************************************
  Include section
****************************************************************************/
#include <stdio.h>
#include "global.h"


/****************************************************************************
  Non static functions
****************************************************************************/
void yuv_write(frame *f, FILE *fileResult);

#endif /*__YUV_WRITE_H__*/
