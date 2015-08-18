/*****************************************************************************
  cavlc.h -- File managing all the cavlc treatment
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

#ifndef __CAVLC_H__
#define __CAVLC_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "common.h"
#include "input.h"

/****************************************************************************
  Variables and structures
****************************************************************************/
typedef struct _code_table_item {
  uint32_t code;
  int32_t bits;
  int32_t data;
} code_table_item;

typedef struct _code_table {
  int32_t count;
  code_table_item *items;
} code_table;


/****************************************************************************
  Non static functions
****************************************************************************/
code_table *init_code_table(code_table_item *items);
void free_code_table(code_table *table);
int32_t get_code(code_table *table);

int32_t get_unsigned_exp_golomb();
int32_t get_signed_exp_golomb();

/*THREAD*/
int32_t get_unsigned_exp_golomb_thread(int32_t thread_ID);
int32_t get_signed_exp_golomb_thread(int32_t thread_ID);
int32_t get_code_thread(code_table *table,int32_t thread_ID);

#endif /*__CAVLC_H__*/
