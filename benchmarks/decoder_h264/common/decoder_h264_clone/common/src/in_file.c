/*****************************************************************************
  in_file.c  -- File performing the access to the file data 
   - Open, read,rewind, close
   - Store dat into a ring_buf

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
#include "yuv_write.h"
#include "input.h"


/****************************************************************************
  Global variables and structures
****************************************************************************/
extern int32_t input_size;
extern int32_t input_remain;
extern int32_t input_pos;
extern uint8_t ring_buf[RING_BUF_SIZE];
extern int32_t ring_pos;


/****************************************************************************
  Variables and structures
****************************************************************************/
static FILE *input_fd = NULL;
static FILE *output_fd = NULL;


/****************************************************************************
  Non static functions
****************************************************************************/
int32_t input_open(const char *filename)
{
  /*************************************
     FILE Opening
  *************************************/
  printf("  -- INPUT OPEN -- Opening file %s\n",filename);
  if (input_fd) {
    //fprintf(stderr, "input_open: file already opened\n");
    printf("\033[1A  -- INPUT OPEN -- File already opened\n\n");
    return 0;
  }

  if ((input_fd = fopen((char *)filename, "r")) == NULL) {
    printf("\033[1A  -- INPUT OPEN -- Can't open file %s\n", filename);
    //abort();
  }
  else
    printf("\033[1A  -- INPUT OPEN -- File %s opened\033[K\n",filename);

  fseek(input_fd, 0, SEEK_END);
  input_size = ftell(input_fd); /* Give the size of the physical file */
  input_rewind(); /* Reset all the main structures */
  return input_size;
}


int32_t input_read(uint8_t *dest, int32_t size)
{
  int32_t count = fread(dest, 1, size, input_fd);
  input_remain += count;
  return count;

#if 0
  if(input_pos<=input_size) /* Test if there is still data inside the input_structure
			       by comparaison the variable indicating the input_pos */
    {
      memcpy(dest,&input_structure[input_pos],size); /*copy data from the input_structure
						       to the ring buffer */
      //printf("input_remain_count_read:%d\n",size);
      // printf("input_remain_count_read:%d\n",input_remain);
      input_pos += size; /* update the variable file pointer inside the input_structure */
      
      
      input_remain += size; /* Update the variable concerning the number of bit still
			       inside the ring_buffer after this copy of bits */
      
      //printf("input_remain_after_read:%d\n",input_remain);
      return 1;
    }
#endif
}


/* Fread() in the physical file */
void file_read(uint8_t *dest)
{
  /* Copy all the bitstream contained in the physical File to the dest which
     is in this case the input buffer structure */
  fread(dest,sizeof(uint8_t),2*input_size,input_fd);
}

void input_rewind()
{
  if (!input_fd) {
    //fprintf(stderr, "input_rewind called, but no file opened!\n");
    printf("  -- INPUT REWIND -- No file opened!\n");
    return;
  }

  fseek(input_fd, 0, SEEK_SET);

  /* Reset the variables giving the number of bit remaining in the Input buffer structure
     and the position inside the ring_buffer */
  input_remain = 0;
  input_pos = 0;

  /* Copy all the content of the input file into a static structure */
#if 0
  file_read(input_structure);
#endif
  /* Copy the first 8192 bits (=Ring_buf_size) from the
     input_buffer structure to the ring buffer */
  input_read(ring_buf, RING_BUF_SIZE);
  ring_pos = 0;
}

void input_close()
{
  if (!input_fd)
    return;
  fclose(input_fd);
  input_size = 0;
  input_fd = NULL;
  //free(input_structure); /* Free The memory of the input structure buffer */
}

int32_t output_open(const char *filename)
{
  /*************************************
     FILE Opening
  *************************************/
  printf("  -- OUTPUT OPEN -- Opening file %s\n",filename);
  if (output_fd) {
    printf("\033[1A  -- OUTPUT OPEN -- File already opened\n\n");
    return 0;
  }
  if ((output_fd = fopen((char *)filename, "w")) == NULL) {
    printf("\033[1A  -- OUTPUT OPEN -- Can't open file %s\n", filename);
    //abort();
  }
  else
    printf("\033[1A  -- OUTPUT OPEN -- File %s opened\033[K\n",filename);

  fseek(output_fd, 0, SEEK_SET);
  
  return 1;
}

void output_write(frame *f)
{
  yuv_write(f,output_fd);
}

void output_close()
{
  if (!output_fd)
    return;
  fclose(output_fd);
  output_fd = NULL;
}
