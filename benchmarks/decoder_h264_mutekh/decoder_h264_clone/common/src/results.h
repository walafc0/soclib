/*****************************************************************************
  results.h -- File giving functions to screen infos about sh, pps, mb... 

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

#ifndef __RESULTS_H__
#define __RESULTS_H__

/****************************************************************************
  Include section
****************************************************************************/
#include "params.h"
#include "nal.h"
#include "slicehdr.h"
#include "common.h"
#include "mode_pred.h"
#include "mbmodes.h"
#include "mode_pred.h"


/****************************************************************************
  Non static functions
****************************************************************************/
void results_screening_sh(slice_header *sh);
void results_screening_sps(seq_parameter_set *sps);
void results_screening_pps(pic_parameter_set *pps);
void results_screening_nalu(nal_unit *nalu);
void results_screening_mb(mb_mode *mb);

#endif
