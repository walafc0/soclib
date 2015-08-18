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

#ifndef __RESIDUAL_H__
#define __RESIDUAL_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "common.h"
#include "input.h"
#include "cavlc.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
#define INIT_CODE_TABLES_KO  0
#define INIT_CODE_TABLES_OK  1


/****************************************************************************
  Non static functions
****************************************************************************/
int32_t init_code_tables();
int32_t free_code_tables();
int32_t residual_block(int32_t *coeffLevel, int32_t maxNumCoeff, int32_t nC,int32_t thread_ID);

#endif /*__RESIDUAL_H__*/
