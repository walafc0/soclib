/*****************************************************************************

  filter.h -- File managing the deblocking filter process

  Original Author: 
  Joel Porquet, THALES COM - AAL, 2006-07-01
  Fabien Colas-Bigey THALES COM - AAL, 2008

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

#ifndef _FILTER_
#define _FILTER_

/****************************************************************************
  Include section
****************************************************************************/
#include "../common/src/mode_pred.h"
#include "../common_soclib/src/flags.h"


/****************************************************************************
  Non static functions
****************************************************************************/
void filter_slice (slice_header *sh,
		   seq_parameter_set *sps, pic_parameter_set *pps,
		   frame *this, mode_pred_info *mpi);

#endif
