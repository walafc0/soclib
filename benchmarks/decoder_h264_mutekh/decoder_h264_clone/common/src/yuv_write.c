/*****************************************************************************
  yuv_write.c -- File giving function to write an output yuv stream 
  
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
#include "yuv_write.h"


/****************************************************************************
  Non static functions
****************************************************************************/
/*
 * Write the results from the current frame into the
 * result file.
 */
void yuv_write(frame * f, FILE *fileResult)
{
  int32_t nb_L; //number of pixel in luma component
  int32_t nb_C; //number of pixel in chroma component
  
  /* Calculate the number of pixels */
  nb_L = f->Lwidth * f->Lheight;
  nb_C = f->Cwidth * f->Cheight;
  
  /* Copy Luma component into file */
  if (!fwrite(f->L,sizeof(uint8_t),nb_L,fileResult))
    printf("Error writing the output YUV file\n");
  
  /* Copy Chroma components into file */
  if (!fwrite(f->C[0],sizeof(uint8_t),nb_C,fileResult))
    printf("Error writing the output YUV file\n");
  if (!fwrite(f->C[1],sizeof(uint8_t),nb_C,fileResult))
    printf("Error writing the output YUV file\n");

  printf("wrote one image\n");
}
