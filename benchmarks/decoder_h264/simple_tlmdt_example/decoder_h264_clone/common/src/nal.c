/*****************************************************************************
  nal.c -- File retrieving nal 
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

/****************************************************************************
  Include section
****************************************************************************/
#include "nal.h"

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
#define gnn_advance() do { \
          ring_pos=(ring_pos+1)&RING_MOD; \
          --input_remain; \
          if(ring_pos==0) input_read(&ring_buf[HALF_RING],HALF_RING); \
          if(ring_pos==HALF_RING) input_read(&ring_buf[0],HALF_RING); \
        } while(0)

/*MultiSlices: We double the number of NAL_BUF structure*/

#define gnn_add_segment(end) do { \
          int32_t size=end-segment_start; \
          if(size>0) { \
            memcpy(&nal_buf[0][nalu_size],&ring_buf[segment_start],size); \
            nalu_size+=size; \
          } \
          segment_start=end&RING_MOD; \
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
int32_t get_next_nal_unit(nal_unit * nalu)
{
  int32_t i, segment_start;
  int32_t nalu_size = 0;

  // search for the next NALU start
  for (;;) {
    if (input_remain <= 4)
      return 0;		//scan the ring BUF
    
    if ((!ring_buf[ring_pos]) &&
	(!ring_buf[(ring_pos + 1) & RING_MOD]) &&
	(!ring_buf[(ring_pos + 2) & RING_MOD]) &&
	(ring_buf[(ring_pos + 3) & RING_MOD] == 1))
      break;
    gnn_advance();
  }

  for (i = 0; i < 4; ++i)
    gnn_advance();
  
  // add bytes to the NALU until the end is found
  segment_start = ring_pos;

  while (input_remain) {
    if ((!ring_buf[ring_pos]) &&
	(!ring_buf[(ring_pos + 1) & RING_MOD]) &&
	(!ring_buf[(ring_pos + 2) & RING_MOD]))
      break;
    
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

  if (!nalu_size)
    return 0;

  // read the NAL unit
  nal_pos[0] = 0;
  nal_bit[0] = 0;
  nalu->forbidden_zero_bit = input_get_bits(1);
  nalu->nal_ref_idc = input_get_bits(2);
  nalu->nal_unit_type = input_get_bits(5);
  nalu->last_rbsp_byte = &nal_buf[0][nalu_size - 1];
  nalu->NumBytesInNALunit = nalu_size;
    
  EBSPtoRBSP(nal_buf[0],nalu_size,0);

  return 1;
}

const char * _str_nal_unit_type(int32_t type)
{
  switch (type) {
  case 1:
    return "Coded slice of a non-IDR picture";
  case 2:
    return "Coded slice data partition A";
  case 3:
    return "Coded slice data partition B";
  case 4:
    return "Coded slice data partition C";
  case 5:
    return "Coded slice of an IDR picture";
  case 6:
    return "Supplemental enhancement information (SEI)";
  case 7:
    return "Sequence parameter set";
  case 8:
    return "Picture parameter set";
  case 9:
    return "Access unit delimiter";
  case 10:
    return "End of sequence";
  case 11:
    return "End of stream";
  case 12:
    return "Filler data";
  default:
    if (type && (type < 24))
      return "Reserved";
  }
  return "Unspecified";
}


///////////////////////////////////////////////////////////////////////////////

#ifdef BUILD_TESTS

int32_t _test_nal(int32_t argc, int8_t *argv[])
{
  nal_unit unit;
  int32_t count;

  if (!input_open("../streams/in.264"))
    return 1;

  for (count = 1; get_next_nal_unit(&unit); ++count) {
    printf("%d: count=%d zero=%d ref_idc=%d type: %s\n", count,
	   unit.NumBytesInNALunit, unit.forbidden_zero_bit,
	   unit.nal_ref_idc, _str_nal_unit_type(unit.nal_unit_type));
  }

  input_close();
  return 0;
}

#endif
