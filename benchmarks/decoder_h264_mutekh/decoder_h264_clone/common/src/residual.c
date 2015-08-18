/*****************************************************************************
  residual.h -- File giving functions to screen infos about sh, pps, mb... 

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
#include "residual.h"
#include "cavlc_tables.h"

/****************************************************************************
  Global variables
****************************************************************************/
static code_table *CoeffTokenCodeTable[4];
static code_table *CoeffTokenCodeTable_ChromaDC;
static code_table *TotalZerosCodeTable_4x4[15];
static code_table *TotalZerosCodeTable_ChromaDC[3];
static code_table *RunBeforeCodeTable[6];


/****************************************************************************
  Non static functions
****************************************************************************/
int32_t residual_block(int32_t *coeffLevel, int32_t maxNumCoeff, int32_t nC, int32_t thread_ID)
{				/*Parsing
				  residual data */
  int32_t coeff_token, TotalCoeff, TrailingOnes;
  int32_t i, suffixLength, zerosLeft, coeffNum;
  int32_t level[16], run[16];

  //printf("nC=%d | ",nC);
  switch (nC) {
  case -1:
    coeff_token =
      get_code_thread(CoeffTokenCodeTable_ChromaDC, thread_ID);
    break;
  case 0:
  case 1:
    coeff_token = get_code_thread(CoeffTokenCodeTable[0], thread_ID);
    break;
  case 2:
  case 3:
    coeff_token = get_code_thread(CoeffTokenCodeTable[1], thread_ID);
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    coeff_token = get_code_thread(CoeffTokenCodeTable[2], thread_ID);
    break;
  default:
    coeff_token = get_code_thread(CoeffTokenCodeTable[3], thread_ID);
    break;
  }

  TotalCoeff = coeff_token >> 2;
  TrailingOnes = coeff_token & 3;

  if (TotalCoeff > 10 && TrailingOnes < 3)
    suffixLength = 1;
  else
    suffixLength = 0;
  //printf("coeff_token=%d TotalCoeff=%d TrailingOnes=%d suffixLength=%d\n",coeff_token,TotalCoeff,TrailingOnes,suffixLength);
  if (!TotalCoeff)
    return 0;

  for (i = 0; i < TotalCoeff; ++i)
    if (i < TrailingOnes)
      level[i] = 1 - 2 * input_get_one_bit_thread(thread_ID);
    else {
      int32_t level_prefix;
      int32_t levelSuffixSize = suffixLength;
      int32_t levelCode;

      for (level_prefix = 0; !input_get_one_bit_thread(thread_ID);
	   ++level_prefix);
      levelCode = level_prefix << suffixLength;
      if (level_prefix == 14 && suffixLength == 0)
	levelSuffixSize = 4;
      else if (level_prefix == 15)
	levelSuffixSize = 12;
      if (levelSuffixSize)
	levelCode +=
	  input_get_bits_thread(levelSuffixSize, thread_ID);
      if (level_prefix == 15 && suffixLength == 0)
	levelCode += 15;
      if (i == TrailingOnes && TrailingOnes < 3)
	levelCode += 2;
      if (levelCode & 1)
	level[i] = (-levelCode - 1) >> 1;
      else
	level[i] = (levelCode + 2) >> 1;
      if (suffixLength == 0)
	suffixLength = 1;
      if (abs(level[i]) > (3 << (suffixLength - 1))
	  && suffixLength < 6)
	++suffixLength;
    }
  //printf("level[] for tot = %d and TRailing = %d =",TotalCoeff,TrailingOnes); for(i=0; i<TotalCoeff; ++i) printf(" %d",level[i]); printf("\n");

  if (TotalCoeff < maxNumCoeff) {
    if (nC < 0)
      zerosLeft =
	get_code_thread(TotalZerosCodeTable_ChromaDC
			[TotalCoeff - 1], thread_ID);
    else
      zerosLeft =
	get_code_thread(TotalZerosCodeTable_4x4[TotalCoeff - 1],
			thread_ID);
  } else
    zerosLeft = 0;
    
  for (i = 0; i < TotalCoeff - 1; ++i) {
    if (zerosLeft > 6) {
      int32_t run_before = 7 - input_get_bits_thread(3, thread_ID);
      if (run_before == 7)
	while (!input_get_one_bit_thread(thread_ID))
	  ++run_before;
      run[i] = run_before;
    } else if (zerosLeft > 0)
      run[i] =
	get_code_thread(RunBeforeCodeTable[zerosLeft - 1],
			thread_ID);
    else
      run[i] = 0;
    zerosLeft -= run[i];
  }
  run[TotalCoeff - 1] = zerosLeft;
  //printf("run[] ="); for(i=0; i<TotalCoeff; ++i) printf(" %d",run[i]); printf("\n");

  coeffNum = -1;
  for (i = TotalCoeff - 1; i >= 0; --i) {
    coeffNum += run[i] + 1;
    coeffLevel[coeffNum] = level[i];
    //printf("coeff = %d\n",coeffLevel[coeffNum]);
  }

  return TotalCoeff;
}

///////////////////////////////////////////////////////////////////////////////

int32_t init_code_tables()
{
  int32_t i;
  for (i = 0; i < 4; ++i)
    CoeffTokenCodeTable[i] = init_code_table(CoeffTokenCodes[i]);
  CoeffTokenCodeTable_ChromaDC =
    init_code_table(CoeffTokenCodes_ChromaDC);
  for (i = 0; i < 15; ++i)
    TotalZerosCodeTable_4x4[i] =
      init_code_table(TotalZerosCodes_4x4[i]);
  for (i = 0; i < 3; ++i)
    TotalZerosCodeTable_ChromaDC[i] =
      init_code_table(TotalZerosCodes_ChromaDC[i]);
  for (i = 0; i < 6; ++i)
    RunBeforeCodeTable[i] = init_code_table(RunBeforeCodes[i]);

  return INIT_CODE_TABLES_OK;
}

int32_t free_code_tables()
{
  int32_t i;
  for (i = 0; i < 4; ++i)
    {
      free_code_table(CoeffTokenCodeTable[i]);}
  free_code_table(CoeffTokenCodeTable_ChromaDC);
  for (i = 0; i < 15; ++i)
    free_code_table(TotalZerosCodeTable_4x4[i]);
  for (i = 0; i < 3; ++i)
    free_code_table(TotalZerosCodeTable_ChromaDC[i]); 
  for (i = 0; i < 6; ++i)
    free_code_table(RunBeforeCodeTable[i]);

  return INIT_CODE_TABLES_KO;

}
