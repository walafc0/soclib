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

#ifndef USER_H_
#define USER_H_

#include "soclib_io.h"
#include "soclib/tty.h"
#include "../segmentation.h"

#include "stdint.h"

void lock_lock( uint32_t * );
void lock_unlock( uint32_t * );
uint32_t atomic_add( uint32_t *addr, uint32_t delta );

static inline uint32_t atomic_inc( uint32_t *addr )
{
	return atomic_add(addr, 1);
}

#define base(x) (void*)(x##_BASE)

int puts(const char *);
void puti(const int i);

uint32_t run_cycles();
uint32_t cpu_cycles();

typedef void irq_handler_t(int);
void set_irq_handler(irq_handler_t *handler);

void enable_hw_irq(unsigned int n);

typedef void sys_handler_t(unsigned int, void*, void*, void*);
void set_sys_handler(sys_handler_t *handler);

typedef void ex_handler_t(unsigned int, void*, void*, void*, void*);
void set_ex_handler(ex_handler_t *handler);

#if defined(__mips__)
# include "system_mips.h"
#elif defined(PPC) /* __mips__ */
# include "system_ppc.h"
#elif defined(__lm32__) /* __mips__ PPC */
# include "system_lm32.h"
#elif defined(__sparc__) /* __mips__ PPC __lm32__ */
# include "system_sparc.h"
#elif defined(__arm__)
# include "system_arm.h"
#else /* __mips__ PPC __lm32__ __sparc__ */
# error no mips no ppc no lm32 no arm
#endif

void pause();

#define putchar __inline_putchar
static inline int putchar(const int x)
{
#ifdef MMU
#if defined(__lm32__)
#error MMU support for lm32 not yet implemented...
#elif defined(__sparc__)
#error MMU support for sparc not yet implemented...
#endif

	uint32_t tlb_mode = get_cp2(1, 0);

	if (tlb_mode & 4)
	{

	soclib_io_write8(
		base(V_TTY),
#if TTY_SIZE > 0x10
		procnum()*TTY_SPAN+
#endif
		TTY_WRITE,
		x);
	}
	else
	{
	soclib_io_write8(
		base(TTY),
#if TTY_SIZE > 0x10
		procnum()*TTY_SPAN+
#endif
		TTY_WRITE,
		x);

	}
#else
	soclib_io_write8(
		base(TTY),
#if TTY_SIZE > 0x10
		procnum()*TTY_SPAN+
#endif
		TTY_WRITE,
		x);
#endif	//end define MMU
	return x;
}

#define getchar __inline_getchar
static inline int getchar()
{
#ifdef MMU
#if defined(__lm32__)
#error MMU support for lm32 not yet implemented...
#endif
	uint32_t tlb_mode = get_cp2(1, 0);

	if (tlb_mode & 4)
	{
	return soclib_io_read8(
		base(V_TTY),
#if TTY_SIZE > 0x10
		procnum()*TTY_SPAN+
#endif
		TTY_READ);
	}
	else
	{
	return soclib_io_read8(
		base(TTY),
#if TTY_SIZE > 0x10
		procnum()*TTY_SPAN+
#endif
		TTY_READ);
	}
#else
	return soclib_io_read8(
		base(TTY),
#if TTY_SIZE > 0x10
		procnum()*TTY_SPAN+
#endif
		TTY_READ);
#endif	// end define MMU
}

#endif
