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
 * Maintainers: nipo
 */

#include <stdarg.h>
#include "system.h"

#include "../segmentation.h"

#ifdef SIMHELPER_BASE
#include "soclib/simhelper.h"
#endif

#include "stdlib.h"

static void *heap_pointer = 0;
extern void _heap();

static inline char *align(char *ptr)
{
	return (void*)((unsigned long)(ptr+15)&~0xf);
}

void *malloc( size_t sz )
{
	char *rp;
	if ( ! heap_pointer )
		heap_pointer = align((void*)_heap);
	rp = heap_pointer;
	heap_pointer = align(rp+sz);
	return rp;
}

void free(void* ptr)
{
}

void trap()
{
#if defined(__mips__)
# if __mips >= 32
	asm volatile("teq $0, $0");
# else
	asm volatile("break");
# endif
#elif defined(PPC)
	asm volatile("trap");
#elif defined(__sparc__)
       asm volatile("ta 0");
#elif defined(__lm32__)
    asm volatile("break");
#elif defined(__arm__)
	asm volatile("swi 1");
#else
# warning No trap
#endif
}

void exit(int level)
{
#ifdef SIMHELPER_BASE
	soclib_io_set(
		base(SIMHELPER),
		SIMHELPER_END_WITH_RETVAL,
		level);
#else
# warning No simhelper, exit will do a trap and an infinite loop
#endif
	trap();
	while(1);
}

void abort()
{
	exit(1);
}

void *memcpy( void *_dst, void *_src, size_t size )
{
	uint32_t *dst = _dst;
	uint32_t *src = _src;
	if ( ! ((uint32_t)dst & 3) && ! ((uint32_t)src & 3) )
		while (size > 3) {
			*dst++ = *src++;
			size -= 4;
		}

	unsigned char *cdst = (unsigned char*)dst;
	unsigned char *csrc = (unsigned char*)src;

	while (size--) {
		*cdst++ = *csrc++;
	}
	return _dst;
}
