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
 * Maintainer: C. Cunat 
 *
 */

#include "system.h"
#include "stdio.h"
#include "stdlib.h"

#include "../segmentation.h"


/* noncoded_frame.h declares the non coded frame variables */
#include "noncoded_r56_2304.h"
/* encoded_frame.h declares the encoded frame variables */
#include "encoded_r12_2304.h"
#include "encoded_r23A_2304.h"
#include "encoded_r23B_2304.h"
#include "encoded_r34A_2304.h"
#include "encoded_r34B_2304.h"
#include "encoded_r56_2304.h"
#include "encoded_r12_576.h"
#include "encoded_r56_576.h"


#define DATA_TC4200_ENC(x) (* (uint32_t volatile *)(x))


void monitoring_dec(uint32_t monitor);


int main(void)
{
  char expansion_factor_index;
  char rate;
  int i, j;

  int nb_syst_bit;
  int nb_coded_bit;
  unsigned char *ref_frame;

  uint32_t tc4200_enc_word;
  uint32_t tc4200_ref_word;
  int error;

  int test;

  puts("Hello from processor ");
  putchar(procnum() + '0');
  putchar('\n');

  /* Monitoring */
  tc4200_enc_word = DATA_TC4200_ENC(TC4200_ENC_MONITOR); 
  monitoring_dec(tc4200_enc_word);
  

  for (test = 0; test < 8; test ++)
    {
      switch (test) {
        case 0: 
          ref_frame              = encoded_r12_2304;
          expansion_factor_index = 18;
          rate                   = 0;
          nb_syst_bit            = (96 * 12);
          nb_coded_bit           = 96 * 24;
          break;
        case 1:
          ref_frame              = encoded_r23A_2304;
          expansion_factor_index = 18;
          rate                   = 1;
          nb_syst_bit            = (96 * 16);
          nb_coded_bit           = 96 * 24;
          break;
        case 2:
          ref_frame              = encoded_r23B_2304;
          expansion_factor_index = 18;
          rate                   = 2;
          nb_syst_bit            = (96 * 16);
          nb_coded_bit           = 96 * 24;
          break;
        case 3:
          ref_frame              = encoded_r34A_2304;
          expansion_factor_index = 18;
          rate                   = 3;
          nb_syst_bit            = (96 * 18);
          nb_coded_bit           = 96 * 24;
          break;
        case 4:
          ref_frame              = encoded_r34B_2304;
          expansion_factor_index = 18;
          rate                   = 4;
          nb_syst_bit            = (96 * 18);
          nb_coded_bit           = 96 * 24;
          break;
        case 5:
          ref_frame              = encoded_r56_2304;
          expansion_factor_index = 18;
          rate                   = 5;
          nb_syst_bit            = (96 * 20);
          nb_coded_bit           = 96 * 24;
          break;
        case 6:
          ref_frame              = encoded_r12_576;
          expansion_factor_index = 0;
          rate                   = 0;
          nb_syst_bit            = (24 * 12);
          nb_coded_bit           = 24 * 24;
          break;
        case 7:
          ref_frame              = encoded_r56_576;
          expansion_factor_index = 0;
          rate                   = 5;
          nb_syst_bit            = (24 * 20);
          nb_coded_bit           = 24 * 24;
          break;
        default:
          printf("Error ");
          putchar('\n');
          return 1;
      }


      /* Configuration */
      putchar('\n');
      printf("#################");
      putchar('\n');
      puts("processor ");
      putchar(procnum() + '0');
      printf("  New configuration proposed");
      putchar('\n');
      printf("#####");
      putchar('\n');
      
      tc4200_enc_word = (rate << 8) + expansion_factor_index;
      DATA_TC4200_ENC(TC4200_ENC_CONFIG) = tc4200_enc_word;
      
      /**************************************************/
      /* Sending to the encoder the new frame to decode */
      /**************************************************/
      tc4200_enc_word = 0;
      for (i = 0; i < 32; i ++)
        tc4200_enc_word = (tc4200_enc_word << 1) + noncoded_r56_2304[i];
      DATA_TC4200_ENC(TC4200_ENC_D_IN_FIRST) = tc4200_enc_word; 
      
      for (i = 1; i < nb_syst_bit / 32; i ++)
        {
          tc4200_enc_word = 0;
          for (j = 0; j < 32; j ++)
            tc4200_enc_word = (tc4200_enc_word << 1) + 
              noncoded_r56_2304[32 * i + j];
          DATA_TC4200_ENC(TC4200_ENC_D_IN) = tc4200_enc_word;
        }

      
      /* Monitoring */
      tc4200_enc_word = DATA_TC4200_ENC(TC4200_ENC_MONITOR);
      monitoring_dec(tc4200_enc_word);

      printf(" noncoded input frame sent before requesting monitoring");
      putchar('\n');
      printf("#####");
      putchar('\n');

     
  
      /***************************************************/
      /* Reading the decoded frame from the LDPC decoder */
      /***************************************************/
      tc4200_enc_word = DATA_TC4200_ENC(TC4200_ENC_MONITOR);
      monitoring_dec(tc4200_enc_word);
      
      printf(" Decoding verification for test %d", test);
      putchar('\n');
      error = 0;
      for (i = 0; i < nb_coded_bit / 32; i ++)
        {
          tc4200_enc_word = DATA_TC4200_ENC(TC4200_ENC_D_OUT);
          tc4200_ref_word = 0;
          for (j = 0; j < 32; j ++)
            tc4200_ref_word = (tc4200_ref_word << 1) + 
              ref_frame[32 * i + j];
          if (tc4200_enc_word != tc4200_ref_word)
            error ++;
        }
      
      if (error == 0)
        printf(" Successful encoding");
      else
        printf(" %d decoding error", error);
      putchar('\n');      
      printf("#################");
      putchar('\n');      
      putchar('\n');
      
    }
  
  
  return 0;
}



void monitoring_dec(uint32_t monitor)
{
  puts(" Monitoring LDPC decoder \n  idle   : ");
  printf("%d",  monitor & 0x00000001);
  putchar('\n');

}
