/*****************************************************************************
  cavlc.c -- File managing all the cavlc treatment
  -Decode cavlc codes
  -Add-on of functions for multislice management 

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
#include "cavlc.h"


/****************************************************************************
  Non static functions
****************************************************************************/
code_table *init_code_table(code_table_item *items)
{
#if 0
  //  code_table *res = malloc(sizeof(code_table));
  code_table_item *pos;
  int32_t count = 0;
  for (pos = items; pos && pos->code != 0xFFFFFFFF; ++pos)
    ++count;
  res->items = items;
  res->count = count;
  return res;
#else

  code_table *res = (code_table *)malloc(sizeof(code_table));
  int32_t count = 0;

  res->items = items;

  /* count number of elements in items */
  for ( ; items && items->code != 0xFFFFFFFF; ++items) {
    count++;
  }

  res->count = count;
  return res;
#endif
}

void free_code_table(code_table *table)
{
  free(table);
}


int32_t get_code(code_table * table)
{
  return get_code_thread(table, 0);
}

int32_t get_unsigned_exp_golomb()
{
  return get_unsigned_exp_golomb_thread(0);
}

int32_t get_signed_exp_golomb()
{
  return get_signed_exp_golomb_thread(0);
}

/*THREADS version*/
int32_t get_unsigned_exp_golomb_thread(int32_t thread_ID)
{
  int32_t exp;

  for (exp = 0; !input_get_one_bit_thread(thread_ID); ++exp);
  if (exp){
    return (1 << exp) - 1 + input_get_bits_thread(exp, thread_ID);
  }
  else{
    return 0;
  }
}

int32_t get_signed_exp_golomb_thread(int32_t thread_ID)
{
  int32_t code = get_unsigned_exp_golomb_thread(thread_ID);
  return (code & 1) ? (code + 1) >> 1 : -(code >> 1);
}

int32_t get_code_thread(code_table * table, int32_t thread_ID)
{
  uint32_t code = input_peek_bits_thread(24, thread_ID) << 8;
  int32_t min = 0, max = table->count;
  while (max - min > 1) {
    int32_t mid = (min + max) >> 1;
    if (code >= table->items[mid].code)
      min = mid;
    else
      max = mid;
  }
  input_step_bits_thread(table->items[min].bits, thread_ID);
  return table->items[min].data;
}
