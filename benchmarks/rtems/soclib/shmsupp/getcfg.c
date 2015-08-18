/*  void Shm_Get_configuration( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the Cyclone CVME961.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: getcfg.c,v 1.10 2004/03/31 04:42:37 ralf Exp $
 *
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 */

#include <assert.h>
#include <bsp.h>
#include <rtems.h>
#include "shm_driver.h"
#include <soclib_xicu.h>

#define INTERRUPT 1
#define POLLING   0

shm_config_table BSP_shm_cfgtbl;

void Shm_Get_configuration(
  uint32_t           localnode,
  shm_config_table **shmcfg
)
{
   BSP_shm_cfgtbl.base         = (void*)SOCLIB_SHM_BASE;
   BSP_shm_cfgtbl.length       = SOCLIB_SHM_SIZE;
   BSP_shm_cfgtbl.format       = SHM_LITTLE;

   BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt;

#ifdef NEUTRAL_BIG
   BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#else
   BSP_shm_cfgtbl.convert      = CPU_swap_u32;
#endif

#if (POLLING==1)
   BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
   BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
#else

   assert(localnode == mips_cpu_id());

   BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
   BSP_shm_cfgtbl.Intr.address = SOCLIB_XICU_ADDR( SOCLIB_XICU_BASE, XICU_WTI_REG, mips_cpu_id() );
   BSP_shm_cfgtbl.Intr.value   = 1;
   BSP_shm_cfgtbl.Intr.length  = 4;
#endif

   *shmcfg = &BSP_shm_cfgtbl;

}
