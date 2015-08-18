/*****************************************************************************
  nal_thread.c -- File giving function for Threads for Multislice_management 
  -Double the data structure
  
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
#include "nal_thread.h"

/****************************************************************************
  Global variables and structures
****************************************************************************/
extern int32_t input_remain;
extern uint8_t ring_buf[RING_BUF_SIZE];
extern int32_t ring_pos;
extern uint8_t* *nal_buf;
extern int32_t* nal_pos;
extern int32_t* nal_bit;


/****************************************************************************
  Macros
****************************************************************************/
#define gnn_advance() do {					\
    ring_pos=(ring_pos+1)&RING_MOD;				\
    --input_remain;						\
    if(ring_pos==0) input_read(&ring_buf[HALF_RING],HALF_RING);	\
    if(ring_pos==HALF_RING) input_read(&ring_buf[0],HALF_RING);	\
  } while(0)

/*MultiSlices: We multiply the number of NAL_BUF structure*/
#define gnn_add_segment(end) do {					\
    int32_t size=end-segment_start;						\
    if(size>0) {							\
      memcpy(&nal_buf[mpi->ID][nalu_size],&ring_buf[segment_start],size); \
      nalu_size+=size;							\
    }									\
    segment_start=end&RING_MOD;						\
  } while(0)

/*!
************************************************************************
* \brief
*    Converts Encapsulated Byte Sequence Packets to RBSP
* \param streamBuffer
*    pointer to data stream
* \param end_bytepos
*    size of data stream
* \param begin_bytepos
*    Position after beginning 
************************************************************************/
static int32_t EBSPtoRBSP(uint8_t *streamBuffer, int32_t end_bytepos, int32_t begin_bytepos)
{
  int32_t i, j, count;
  count = 0;

  const int32_t ZEROBYTES_SHORTSTARTCODE = 2; //indicates the number of zero bytes in the short start-code prefix

  if(end_bytepos < begin_bytepos)
    return end_bytepos;
  
  j = begin_bytepos;
  
  for(i = begin_bytepos; i < end_bytepos; i++) 
    { //starting from begin_bytepos to avoid header information
      if(count == ZEROBYTES_SHORTSTARTCODE && streamBuffer[i] == 0x03) 
	{
	  i++;
	  count = 0;
	}
      streamBuffer[j] = streamBuffer[i];
      if(streamBuffer[i] == 0x00)
	count++;
      else
	count = 0;
      j++;
    }
  
  return j;
}

/****************************************************************************
  Non static functions
****************************************************************************/
/*MultiSlice: Another function taking account slice_number
  We give the good data structure according to mpi->ID(=Slice_ID)*/
int32_t get_next_nal_unit_thread(nal_unit * nalu, mode_pred_info * mpi)
{
  int32_t i, segment_start;
  int32_t nalu_size = 0;

  /*search for the next NALU start */
  for (;;) {
    if (input_remain <= 4) {
      return 0;		//scan the ring BUF
    }

    /* Detects the start marker */
    if ((!ring_buf[ring_pos]) &&
	(!ring_buf[(ring_pos + 1) & RING_MOD]) &&
	(!ring_buf[(ring_pos + 2) & RING_MOD]) &&
	(ring_buf[(ring_pos + 3) & RING_MOD] == 1))
      break;
    gnn_advance();
  }

  for (i = 0; i < 4; ++i){
    gnn_advance();
  }
    
  // add bytes to the NALU until the end is found
  segment_start = ring_pos;
    
  while (input_remain) {
    if ((!ring_buf[ring_pos]) &&
	(!ring_buf[(ring_pos + 1) & RING_MOD]) &&
	(!ring_buf[(ring_pos + 2) & RING_MOD])){
      break;}
	
    ring_pos = (ring_pos + 1) & RING_MOD;
    --input_remain;
    if (ring_pos == 0) {
      gnn_add_segment(RING_BUF_SIZE);
      input_read(&ring_buf[HALF_RING], HALF_RING);
    }
    if (ring_pos == HALF_RING) {
      gnn_add_segment(HALF_RING);
      input_read(&ring_buf[0], HALF_RING);
    }
  }
  gnn_add_segment(ring_pos);
    
  if (!nalu_size) {
    return 0;
  }
  // read the NAL unit
  nal_pos[mpi->ID] = 0;
  nal_bit[mpi->ID] = 0;
  nalu->forbidden_zero_bit = input_get_bits_thread(1, mpi->ID);
  nalu->nal_ref_idc = input_get_bits_thread(2, mpi->ID);
  nalu->nal_unit_type = input_get_bits_thread(5, mpi->ID);
  nalu->last_rbsp_byte = &nal_buf[mpi->ID][nalu_size - 1];
  nalu->NumBytesInNALunit = nalu_size;

  EBSPtoRBSP(nal_buf[mpi->ID],nalu_size,0);
  
  return 1;

}

int32_t more_rbsp_data_thread(nal_unit * nalu, mode_pred_info * mpi)
{
  return &nal_buf[mpi->ID][nal_pos[mpi->ID]] <= nalu->last_rbsp_byte;
}


