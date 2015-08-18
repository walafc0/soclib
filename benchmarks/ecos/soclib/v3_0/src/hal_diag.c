/*=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:        nickg, dmoseley
// Date:        1998-03-02
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_diag.h>

#include <cyg/hal/hal_intr.h>

#include <cyg/hal/hal_io.h>

//-----------------------------------------------------------------------------
// Select which diag channels to use

//#define CYG_KERNEL_DIAG_LCD
#define CYG_KERNEL_DIAG_SERIAL

/*---------------------------------------------------------------------------*/

#define TTY_SOCLIB_BASE		0xc0c00000

#define TTY_SOCLIB_REG_WRITE    0
#define TTY_SOCLIB_REG_STATUS   4
#define TTY_SOCLIB_REG_READ     8

void hal_diag_led(int x)
{
}

externC void diag_write_string (const char*);

#ifdef CYG_KERNEL_DIAG_SERIAL
extern void cyg_hal_plf_comms_init(void);
extern void cyg_hal_plf_serial_putc(void*, cyg_uint8);
extern cyg_uint8 cyg_hal_plf_serial_getc(void*);
#endif

void hal_diag_init(void)
{
}

void hal_diag_write_char(char c)
{
  HAL_WRITE_UINT8(TTY_SOCLIB_BASE + TTY_SOCLIB_REG_WRITE, c);
}

void hal_diag_read_char(char* _c)
{
  unsigned char c;

  do {
    HAL_READ_UINT8(TTY_SOCLIB_BASE + TTY_SOCLIB_REG_STATUS, c);
  } while (!c);

  HAL_READ_UINT8(TTY_SOCLIB_BASE + TTY_SOCLIB_REG_READ, c);

  *_c = c;
}

/*---------------------------------------------------------------------------*/
/* End of hal_diag.c */
