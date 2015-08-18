/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: tm27.h,v 1.2 2004/04/23 04:47:37 ralf Exp $
 *
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 */

#error

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 *  Define the interrupt mechanism for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#if 1
#define Install_tm27_vector( handler ) \
    (void) set_vector( handler, TX3904_IRQ_SOFTWARE_1, 1 ); \

#define Cause_tm27_intr() \
    asm volatile ( "syscall 0x01" : : );

#define CLOCK_VECTOR TX3904_IRQ_TMR0

#define Clear_tm27_intr() /* empty */

#define Lower_tm27_intr() /* empty */

#else

#endif

#endif
