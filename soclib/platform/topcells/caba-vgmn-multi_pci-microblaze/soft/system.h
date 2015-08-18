#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "soclib/tty.h"

#define TTY_BASE   0x00400000
#define PCI_BASE 0x00500000
#define base(x) (volatile char *)x ## _BASE

#define get_fsl(x)                        \
({unsigned int __fsl_x;                   \
__asm__("get %0, rfsl"#x:"=r"(__fsl_x));  \
__fsl_x;})

//({__asm__("addik r1, r1, %[stackoffset]"::[stackoffset] "i" (v));   

#define set_r1(v)                        \
({__asm__("addk r1, r1, %0"::"r"(v));   \
})

static inline int procnum(void)
{
    return get_fsl(0);
}

static inline void putc(const char x)
{
   soclib_io_write8(
      base(TTY),
      procnum()*TTY_SPAN+TTY_WRITE,
      x);
}

static inline char getc(void)
{
   return soclib_io_read8(
      base(TTY),
      procnum()*TTY_SPAN+TTY_READ);
}

/* Needs swaping bytes !
   The timer seems to love little endian values whereas the MicriBlaze
   sends big endian ones by default.
*/
static inline uint32_t uint32_swap(uint32_t x)
{
    return
        ((x)          << 24 ) |
        ((x & 0xff00) <<  8 ) |
        ((x >>  8) & 0xff00 ) |
        ((x >> 24)          );
}

#endif
