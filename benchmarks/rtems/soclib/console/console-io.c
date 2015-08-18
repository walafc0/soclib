/*
 *  Logic based on the jmr3904-io.c file in newlib 1.8.2
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: console-io.c,v 1.10 2004/04/21 16:01:36 ralf Exp $
 *
 *  Modified by Alexandre Becoulet for the SoCLib BSP
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <soclib_xicu.h>

/* external prototypes for monitor interface routines */

#define READ_UINT8( _register_, _value_ ) \
        ((_value_) = *((volatile unsigned char *)(_register_)))

#define WRITE_UINT8( _register_, _value_ ) \
        (*((volatile unsigned char *)(_register_)) = (_value_))

#define TTY_SOCLIB_REG_WRITE	0
#define TTY_SOCLIB_REG_STATUS	4
#define TTY_SOCLIB_REG_READ	8

/*
 *  Eventually console-polled.c should hook to this better.
 */

rtems_isr TTY_isr( rtems_vector_number vector )
{
}

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 *
 */

void console_initialize_hardware(void)
{
#if 0
  SOCLIB_XICU_WRITE( SOCLIB_XICU_BASE, XICU_MSK_HWI_ENABLE, /* cpu0 */ 0, 1 << TTY_VECTOR );
  set_vector( &TTY_isr, TTY_VECTOR, 1 );
#endif
}

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
  unsigned short disr;

  WRITE_UINT8(TTY_SOCLIB_BASE + TTY_SOCLIB_REG_WRITE, ch);
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
  unsigned char c;
  unsigned char st;

  READ_UINT8 (TTY_SOCLIB_BASE + TTY_SOCLIB_REG_STATUS, st);
  if (st) {
    READ_UINT8 (TTY_SOCLIB_BASE + TTY_SOCLIB_REG_READ, c);
    return (char) c;
  }
  return -1;
}

#include <rtems/bspIo.h>

static void soclib_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = soclib_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;

