/*  Shared Memory Lock Routines
 *
 *  This shared memory locked queue support routine need to be
 *  able to lock the specified locked queue.  Interrupts are
 *  disabled while the queue is locked to prevent preemption
 *  and deadlock when two tasks poll for the same lock.
 *  previous level.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: lock.c,v 1.11 2004/04/15 13:26:14 ralf Exp $
 *
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 *  Relies on gcc4 atomic ops
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

/*
 *  Shm_Initialize_lock
 *
 *  Initialize the lock for the specified locked queue.
 */

void Shm_Initialize_lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  lq_cb->lock = LQ_UNLOCKED;
}

/*  void _Shm_Lock( &lq_cb )
 *
 *  This shared memory locked queue support routine locks the
 *  specified locked queue.  It disables interrupts to prevent
 *  a deadlock condition.
 */

void Shm_Lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  uint32_t         isr_level;
  uint32_t         *lockptr = (uint32_t*)&lq_cb->lock;

  rtems_interrupt_disable( isr_level );
    Shm_isrstat = isr_level;

  while (__sync_lock_test_and_set(lockptr, LQ_LOCKED))
    asm volatile("nop");
}

/*
 *  Shm_Unlock
 *
 *  Unlock the lock for the specified locked queue.
 */

void Shm_Unlock(
  Shm_Locked_queue_Control *lq_cb
)
{
  uint32_t         isr_level;
  uint32_t         *lockptr = (uint32_t*)&lq_cb->lock;

  __sync_lock_test_and_set(lockptr, LQ_UNLOCKED);
  isr_level = Shm_isrstat;
  rtems_interrupt_enable( isr_level );
}
