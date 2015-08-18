/*
 *
 * SOCLIB_GPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU GPLv2.
 * 
 * SoCLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 * 
 * SOCLIB_GPL_HEADER_END
 *
 * Copyright (c) TurboConcept 2008
 *
 * Maintainer: Christophe Cunat 
 *
 */

#include "system.h"
#include "stdio.h"
#include "stdlib.h"

#include "../segmentation.h"


/* input_frame.h declares the input_frame variable */
#include "input_frame.h"
/* decode_frame.h declares the decoded_frame variable */
#include "decoded_frame.h"


#define DATA_TC4200(x) (* (uint32_t volatile *)(x))


void monitoring_dec(uint32_t monitor);


int main(void)
{
  char expansion_factor_index;
  char rate;
  char itmax;
  char itstop; 
  int i, j;

  uint16_t tc4200_decoded_frame[2304 / 16]; /* tc4200 outputs 16 bits per clock
                                             * cycles */

  uint32_t tc4200_word;
  int error;

  printf("Hello from processor %d\n", procnum());

    
  expansion_factor_index = 18; /* highest possible index */
  rate                   = 5;  /* highest possible rate index */
  itmax                  = 25; /* typical max iteration */
  itstop                 = 1;  /* tc4200 internal stopping criterion set */


  /* Monitoring */
  tc4200_word = DATA_TC4200(TC4200_MONITOR); 
  monitoring_dec(tc4200_word);
  

  /* Configuration */
  tc4200_word = (itstop << 24) + (itmax << 16) +
    (rate << 8) + expansion_factor_index;
  
  DATA_TC4200(TC4200_CONFIG) = tc4200_word;
  
  /**************************************************/
  /* Sending to the decoder the new frame to decode */
  /**************************************************/
  tc4200_word = 0;
  for (i = 0; i < 4; i ++)
    tc4200_word = (tc4200_word << 8) | (input_frame[i] & 0xFF);
  
  
  DATA_TC4200(TC4200_D_IN_FIRST) = tc4200_word;
  

  for (i = 1; i < 2304 / 4; i ++)
    {
      tc4200_word = 0;
      for (j = 0; j < 4; j ++)
        tc4200_word = (tc4200_word << 8) | 
          (input_frame[4 * i + j] & 0xFF);
      /* 4 data are transmitted per word.
       * 2304 input samples for this expansion factor
       */
      DATA_TC4200(TC4200_D_IN) = tc4200_word;
    }
  
  printf("\n#################\nprocessor %d : decoding input frame sent\n#################\n", 
         procnum());

  /* Monitoring */
  tc4200_word = DATA_TC4200(TC4200_MONITOR);
  //printf("%x\n", tc4200_word);
  monitoring_dec(tc4200_word);

  
  /***************************************************/
  /* Reading the decoded frame from the LDPC decoder */
  /***************************************************/
  /* rate index == 5         => rate = 5/6 
   * expansion factor index  => expansion factor = 96
   * => input data are 2304
   * => output data are 1920
   */
  for (i = 0; i < 1920 / 16; i ++)
    {
      tc4200_word = DATA_TC4200(TC4200_D_OUT);
      //printf("%x", tc4200_word);
      tc4200_decoded_frame[i] = (uint16_t) (tc4200_word & 0x0000FFFF);
    }
  
  tc4200_word = DATA_TC4200(TC4200_MONITOR);
  monitoring_dec(tc4200_word);


  error = 0;
  printf(" Decoding verification\n");
  for (i = 0; i < 1920 / 16; i ++)
    {
      int local_data;
      int j;
      local_data = 0;
      for (j = 0; j < 16; j ++)
        local_data = (local_data << 1) + decoded_frame[i * 16 + j];
      if (tc4200_decoded_frame[i] != local_data)
        {
          printf("Decoded word %d erroneous\n", i);
          printf("   expected 0x%04x\n", local_data);
          printf("   received 0x%04x\n", tc4200_decoded_frame[i]);
          error ++;
        }
    }
  
  if (error == 0)
    printf(" Successful decoding");
  else
    printf(" %d decoding error", error);

  putchar('\n');
 
  while (1){
    
  }
  
	return 0;
}



void monitoring_dec(uint32_t monitor)
{
  printf(" Monitoring LDPC decoder \n  idle   : %d\n", monitor & 0x00000001);
  printf("  syndok : %d\n",  (monitor >> 8) & 0x00000001);
  printf("  itdone : %d\n",  (monitor >> 16) & 0x000000FF);  
}
