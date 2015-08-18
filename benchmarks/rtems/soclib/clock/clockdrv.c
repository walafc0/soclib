/*
 *  Instantiate the clock driver shell.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: clockdrv.c,v 1.8 2006/11/17 22:41:41 joel Exp $
 *
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 */

#include <rtems.h>
#include <bsp.h>
#include <soclib_xicu.h>


#define CLOCK_DRIVER_USE_FAST_IDLE

#define Clock_driver_support_at_tick() \
  SOCLIB_XICU_READ( SOCLIB_XICU_BASE, XICU_PTI_ACK, CLOCK_VECTOR );

/*
 *  500000 clicks per tick ISR is HIGHLY arbitrary
 */

#define CLICKS 500000

#define Clock_driver_support_install_isr( _new, _old )			\
  do {									\
    uint32_t   _clicks = CLICKS;					\
    _old = set_vector( _new, CLOCK_VECTOR, 1 );				\
    SOCLIB_XICU_WRITE( SOCLIB_XICU_BASE, XICU_PTI_VAL, CLOCK_VECTOR, _clicks );	\
    SOCLIB_XICU_WRITE( SOCLIB_XICU_BASE, XICU_PTI_PER, CLOCK_VECTOR, _clicks );    \
    SOCLIB_XICU_WRITE( SOCLIB_XICU_BASE, XICU_MSK_PTI_ENABLE, /* cpu0 */ 0, 1 << CLOCK_VECTOR ); \
  } while(0)

#define Clock_driver_support_initialize_hardware()

#define Clock_driver_support_shutdown_hardware() \
  SOCLIB_XICU_WRITE( SOCLIB_XICU_BASE, XICU_MSK_PTI_DISABLE, 0, 1 << CLOCK_VECTOR );

#include "../../../shared/clockdrv_shell.c"

