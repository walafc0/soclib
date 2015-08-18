/*
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 */

#include <bsp.h>
#include <rtems.h>
#include <stdlib.h>
#include <soclib_xicu.h>

void mips_default_isr( int vector );

unsigned int mips_interrupt_number_of_vectors = 32;

#include <rtems/bspIo.h>  /* for printk */

void mips_soclib_icu_init(void)
{
}

void mips_default_isr( int vector )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled isr exception: vector 0x%02x, cause 0x%08X, sr 0x%08X\n",
	  vector, cause, sr );
  rtems_fatal_error_occurred(1);
}

void mips_irq_process(unsigned int v, CPU_Interrupt_frame *frame )
{
  if (_ISR_Vector_table[v])
    _ISR_Vector_table[v](v, frame);
  else
    mips_default_isr(v);
}

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  if ( cause & 0xfc ) {
    uint32_t id = mips_cpu_id();

    while (1) {
      uint32_t s = SOCLIB_XICU_READ( SOCLIB_XICU_BASE, XICU_PRIO, id );

      if (XICU_PRIO_HAS_PTI(s)) {
	mips_irq_process(XICU_PRIO_PTI(s), frame);
	continue;
      }

      if (XICU_PRIO_HAS_HWI(s)) {
	mips_irq_process(XICU_PRIO_HWI(s), frame);
	continue;
      }

      if (XICU_PRIO_HAS_WTI(s)) {
	mips_irq_process(XICU_PRIO_WTI(s), frame);
	continue;
      }

      break;
    }

  }
}


