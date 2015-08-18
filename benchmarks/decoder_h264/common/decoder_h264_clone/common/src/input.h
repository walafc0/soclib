/*****************************************************************************
  input.h  -- File performing the access to the nal_buf 
   - Give the structure for the biffer: RING & NAL
   - Give functions for retrieving bits and update the data pointers 
   - Multislices: double the functions for retrieving bit by passing which
     NAL_BUF structure we have to modify

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

#ifndef __INPUT_H__
#define __INPUT_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "common.h"
#include "global.h"

/****************************************************************************
  Variables and structures
****************************************************************************/
#define NAL_BUF_SIZE  65536  // maximum NAL unit size
#define RING_BUF_SIZE  8192  // input ring buffer size, MUST be a power of two!


/****************************************************************************
  Non static functions
****************************************************************************/
int32_t input_open(const char *filename);
void input_rewind();
void file_read(uint8_t *dest); /* fread() in the physical file */
void input_close();

int32_t input_read(uint8_t *dest, int32_t size); /* memcpy */


int32_t output_open(const char *filename);
void output_write(frame *f);
void output_close();

int32_t input_peek_bits(int32_t bit_count);
void input_step_bits(int32_t bit_count);
int32_t input_get_bits(int32_t bit_count);

int32_t input_get_one_bit();

int32_t input_byte_aligned();
void input_align_to_next_byte();
int32_t input_get_byte();

// THREADS
int32_t input_peek_bits_thread(int32_t bit_count,int32_t thread_ID);
void input_step_bits_thread(int32_t bit_count,int32_t thread_ID);
int32_t input_get_bits_thread(int32_t bit_count,int32_t thread_ID);

int32_t input_get_one_bit_thread(int32_t thread_ID);

int32_t input_byte_aligned_thread(int32_t thread_ID);
void input_align_to_next_byte_thread(int32_t thread_ID);
int32_t input_get_byte_thread(int32_t thread_ID);

#endif /*__INPUT_H__*/
