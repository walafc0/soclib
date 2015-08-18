#ifndef __SOCLIB_SPARC_H
#define __SOCLIB_SPARC_H

#define STACK_SIZE 16 * 1024
#define TRAP_STACK_SIZE 4 * 1024
#define NUMBER_OF_REGISTER_WINDOWS 8
#define PSR_INIT   0x10c0       /* Disable traps, set s and ps */
#define TBR_INIT   0

#endif
