/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: bspstart.c,v 1.15 2008/07/24 14:52:54 thomas Exp $
 *
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 */

#include <bsp.h>
#include <bsp/bootcard.h>

/*
 *  This method returns the base address and size of the area which
 *  is to be allocated between the RTEMS Workspace and the C Program
 *  Heap.
 */
void bsp_get_work_area(
  void   **work_area_start,
  size_t  *work_area_size,
  void   **heap_start,
  size_t  *heap_size
)
{
  extern int _HeapBase;
  extern int _HeapEnd;
  extern int _WorkspaceBase;
  extern int _ram_end;

  *work_area_start = &_WorkspaceBase;
  *work_area_size = (void*)&_ram_end - (void *)&_WorkspaceBase;
  *heap_start = &_HeapBase;
  *heap_size = (void *)&_HeapEnd - (void *)&_HeapBase;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  extern void mips_soclib_icu_init(void);
  extern void mips_install_isr_entries(void);

  mips_set_sr( 0xff00 );  /* all interrupts unmasked but globally off */
                          /* depend on the IRC to take care of things */
  mips_install_isr_entries();

  mips_soclib_icu_init();
}

/*
 *  Required routine by some gcc run-times.
 */
void clear_cache( void *address, size_t n )
{
}

/* Structure filled in by get_mem_info.  Only the size field is
   actually used (to clear bss), so the others aren't even filled in.  */

struct s_mem
{
  unsigned int size;
  unsigned int icsize;
  unsigned int dcsize;
};

void
get_mem_info (mem)
     struct s_mem *mem;
{
  mem->size = 0x1000000;        /* XXX figure out something here */
}
