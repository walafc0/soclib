/*****************************************************************************

  h264.h/main;h -- Main file which start the core functions
 -Management of the sequence:open, close, rewind
 -Start the decoding process

  Authors:
  Florian Broekaert, THALES COM - AAL, 2006-04-07
  Fabien Colas-Bigey THALES COM - AAL, 2008
  Pierre-Edouard BEAUCAMPS, THALES COM - AAL, 2009

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

#ifndef __H264_H__
#define __H264_H__

/****************************************************************************
  Include section
****************************************************************************/
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#include <mutek/printk.h>

#include "slice.h"
#include "timer.h"

#include "../common/src/yuv_write.h"
#include "../common/src/residual.h"
#include "../common_soclib/src/vfs_file.h"
#include "filter.h"

#include "cpu_type.h"

/****************************************************************************
  Non static functions
****************************************************************************/
int32_t h264_open(char *file_in_name, char *file_out_name,
	      int8_t slice_numbers);
//frame *h264_decode_frame(int32_t verbose);
frame *h264_decode_frame();
void h264_rewind();
void h264_close();

void *slice_process(void *mpi);

#endif /*__H264_H__*/

