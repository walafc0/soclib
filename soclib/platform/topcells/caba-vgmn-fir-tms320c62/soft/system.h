/*
 *
 * SOCLIB_GPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU GPLv2.
 * 
 * SoCLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 * 
 * SOCLIB_GPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include <string.h>

#include "tty.h"
#include "soclib_io.h"
#include "../segmentation.h"

#define base(x) (void*)(x##_BASE)

static inline int procnum()
{ return 0;
}

static inline void putc(const char x)
{
	soclib_io_write8(
		base(TTY),
		TTY_WRITE,
		x);
}

static inline char getc()
{
	return soclib_io_read8(
		base(TTY),
		procnum()*TTY_SPAN+TTY_READ);
}

/*

void print_str(const char *s)
{ int i, len = strlen(s);
  for(i=0; i<len; i++)
  { putc(s[i]);
  }
}
*/
#endif
