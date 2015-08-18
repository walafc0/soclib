/*
 *  This file implements a benchmark timer using a soclib timer.
 *
 *  NOTE: On the simulator, the count directly reflects instructions.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: timer.c,v 1.10 2008/09/05 04:59:23 ralf Exp $
 *
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 */

#include <assert.h>
#include <bsp.h>
#include <soclib_xicu.h>

bool benchmark_timer_find_average_overhead;

void benchmark_timer_initialize(void)
{
  SOCLIB_XICU_WRITE( SOCLIB_XICU_BASE, XICU_PTI_PER, 1, -1 );
}

int benchmark_timer_read(void)
{
  uint32_t          total = (uint32_t)-1 - SOCLIB_XICU_READ( SOCLIB_XICU_BASE, XICU_PTI_VAL, 1 );

  return total;          /* in one microsecond units */
}

void benchmark_timer_disable_subtracting_average_overhead( bool find_flag )
{
}

