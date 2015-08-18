#include "soclib/tty.h"
#include "uputs.h"

void uputs(unsigned int addr, const char *str)
{
   volatile char *ttyw = (char *)((TTY_WRITE<<2) + addr);
   while (*str)
      *ttyw = *str++;
}


