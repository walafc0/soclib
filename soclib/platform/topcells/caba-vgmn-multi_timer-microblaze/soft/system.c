#include "system.h"
#include "soclib/timer.h"

void uputs(const char *str)
{
   while (*str)
      putc(*str++);
}

void puti(const int i)
{
   if (i>10)
      puti(i/10);
   putc(i%10+'0');
}

void interrupt_handler(void)
{
   soclib_io_set(base(TIMER), procnum()*TIMER_SPAN+TIMER_RESETIRQ, 0);
   uputs("\nInterruption caught on proc "); puti(procnum()); uputs(" at time ");
   puti(soclib_io_get(base(TIMER), TIMER_VALUE)); uputs("\n");
}
