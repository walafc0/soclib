/*****************************************************************************

  timer.h -- Function dealing with the timer 
  

  Authors:
  Pierre-Edouard BEAUCAMPS, THALES COM - AAL, 2009

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

#ifndef __TIMER_H__
#define __TIMER_H__

/****************************************************************************
  Include section
****************************************************************************/
#if defined(CONFIG_DRIVER_TIMER_EMU)
  #include <drivers/timer/emu/timer-emu.h>
#elif defined(CONFIG_DRIVER_TIMER_SOCLIB)
  #include <drivers/icu/soclib-xicu/xicu-soclib.h>
  //#include <drivers/device/timer/soclib/timer-soclib.h>
  #include "../../../soclib_addresses.h"
#endif

#include <stdio.h>
#include <device/device.h>
//#include <device/driver.h>

#include "../common_soclib/src/flags.h"

/****************************************************************************
  Non static functions
****************************************************************************/
void init_timer();
uint32_t get_timer_value();
void set_timer_value(uint32_t value);

#endif /*__TIMER_H__*/
