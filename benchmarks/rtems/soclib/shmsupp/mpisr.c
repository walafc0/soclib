/*
 *
 *  NOTE: This routine is not used when in polling mode.  Either
 *        this routine OR Shm_clockisr is used in a particular system.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: mpisr.c,v 1.10 2004/03/31 04:42:37 ralf Exp $
 *
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>
#include <soclib_xicu.h>


rtems_isr Shm_isr_mvme136()
{
  Shm_Interrupt_count += 1;
  rtems_multiprocessing_announce();
  SOCLIB_XICU_READ( SOCLIB_XICU_BASE, XICU_WTI_REG, mips_cpu_id() );
}

/*  void _Shm_setvec( )
 *
 *  This driver routine sets the SHM interrupt vector to point to the
 *  driver's SHM interrupt service routine.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 */

void Shm_setvec()
{
  /* may need to disable intr */
  uint32_t id = mips_cpu_id();
  SOCLIB_XICU_WRITE( SOCLIB_XICU_BASE, XICU_MSK_WTI_ENABLE, IPI_VECTOR + id, 1 << id );
  set_vector( Shm_isr_mvme136, IPI_VECTOR + id, 1 );
}

