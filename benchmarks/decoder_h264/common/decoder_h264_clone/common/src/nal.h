/*****************************************************************************
  nal.h -- File retrieving nal 
  - Set the nal structure
  - copy the data from the ring_buf to the nal_buf

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

#ifndef __NAL_H__
#define __NAL_H__

/****************************************************************************
  Include section
****************************************************************************/
#include <stdint.h>
#include "common.h"
#include "input.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
#define RING_MOD  ((RING_BUF_SIZE)-1)
#define HALF_RING (((RING_BUF_SIZE)>>1))

typedef struct _nal_unit 
{
  int32_t NumBytesInNALunit;
  int32_t forbidden_zero_bit;
  int32_t nal_ref_idc;
  int32_t nal_unit_type;
  uint8_t *last_rbsp_byte;
} nal_unit;


/****************************************************************************
  Non static functions
****************************************************************************/
int32_t get_next_nal_unit(nal_unit *nalu);
const char *_str_nal_unit_type(int32_t type);

#endif /*__NAL_H__*/
