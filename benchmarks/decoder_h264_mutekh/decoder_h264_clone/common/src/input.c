/*****************************************************************************
  input.c  -- File performing the access to the nal_buf 
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

/****************************************************************************
  Include section
****************************************************************************/
#include "input.h"


/****************************************************************************
  Global Variables
****************************************************************************/

/* Input buffer Structure :
   First buffer that will store thanks to a fread()
   the whole bitstream containing in the physical file*/
uint8_t *input_structure; /*input buffer structure */
int32_t input_pos; /* this is a variable containing the position of the file pointer
		  during the operation on this input_structure buffer */

/* Ring Buffer Structure :
   After the input structure buffer, data are sent to this Ring buffer ith a memcpy()
   A Nal structure is searched and is sent to a third buffer called Nal_Buffer */
uint8_t ring_buf[RING_BUF_SIZE];
int32_t ring_pos; /* Variable inidcating the position inside the ring buffer */


/* NAL Buffer Structure :
   Third Buffer that store with a memcpy
   the data contained in the Ring_buffer */
uint8_t* *nal_buf; /* For Multislice 1 Nal buffer fo each thread */
int32_t* nal_pos; /* file pointers to move inside the Nal buffer */
int32_t* nal_bit;


int32_t input_size; /* Size of the physical file containing the bitstream */
int32_t input_remain; /* Size remaining inside the input buffer structure */ 


/****************************************************************************
  Static functions
****************************************************************************/
static inline int32_t peek_bits_thread(int32_t bit_count, int32_t thread_ID)
{
  register uint32_t x =
    (nal_buf[thread_ID][nal_pos[thread_ID]] << 24) |
    (nal_buf[thread_ID][nal_pos[thread_ID] + 1] << 16) |
    (nal_buf[thread_ID][nal_pos[thread_ID] + 2] << 8) |
    nal_buf[thread_ID][nal_pos[thread_ID] + 3];

  return (x >> (32 - bit_count - nal_bit[thread_ID])) & ((1 << bit_count)
							 - 1);
}

static inline void step_bits_thread(int32_t bit_count, int32_t thread_ID)
{
  nal_bit[thread_ID] += bit_count;
  nal_pos[thread_ID] += nal_bit[thread_ID] >> 3; /* division entière par 8 */
  nal_bit[thread_ID] &= 7;
}

/****************************************************************************
  Non static functions
****************************************************************************/
int32_t input_peek_bits(int32_t bit_count)
{
  return peek_bits_thread(bit_count, 0);
}

void input_step_bits(int32_t bit_count)
{
  step_bits_thread(bit_count, 0);
}

int32_t input_get_bits(int32_t bit_count)
{
  return input_get_bits_thread(bit_count, 0);
}

int32_t input_get_one_bit()
{
  return input_get_one_bit_thread(0);
}

int32_t input_byte_aligned()
{
  return input_byte_aligned_thread(0);
}

void input_align_to_next_byte()
{
  input_align_to_next_byte_thread(0);
}

int32_t input_get_byte()
{
  return input_get_byte_thread(0);
}


/*THREADS*/
int32_t input_peek_bits_thread(int32_t bit_count, int32_t thread_ID)
{
  return peek_bits_thread(bit_count, thread_ID);
}

void input_step_bits_thread(int32_t bit_count, int32_t thread_ID)
{
  step_bits_thread(bit_count, thread_ID);
}

int32_t input_get_bits_thread(int32_t bit_count, int32_t thread_ID)
{
  int32_t res = peek_bits_thread(bit_count, thread_ID);
  step_bits_thread(bit_count, thread_ID);
  return res;
}

int32_t input_get_one_bit_thread(int32_t thread_ID)
{
  int32_t res = (nal_buf[thread_ID][nal_pos[thread_ID]] >> (7 - nal_bit[thread_ID])) & 1;
  if (++nal_bit[thread_ID] > 7) {
    ++nal_pos[thread_ID];
    nal_bit[thread_ID] = 0;
  }

  //printf("nalpos = %d\n",nal_pos[thread_ID]);
  return res;
}

int32_t input_byte_aligned_thread(int32_t thread_ID)
{
  return (!nal_bit[thread_ID]);
}

void input_align_to_next_byte_thread(int32_t thread_ID)
{
  if (input_byte_aligned_thread(thread_ID))
    return;
  ++nal_pos[thread_ID];
  nal_bit[thread_ID] = 0;
}

int32_t input_get_byte_thread(int32_t thread_ID)
{
  return nal_buf[thread_ID][nal_pos[thread_ID]++];
}
