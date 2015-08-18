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
 * Copyright (c) Telecom ParisTech
 *         Alexis Polti <polti@telecom-paristech.fr>
 *
 * Maintainers: Alexis Polti
 */

#ifndef _SYSTEM_SPARC_H_
#define _SYSTEM_SPARC_H_

static inline void
irq_enable(void)
{
	uint32_t tmp;

	asm volatile (
		"rd %%psr, %0 \n\t"
		"or %0, %1, %0 \n\t"
		"wr %0, 0, %%psr \n\t"
		: "=r" (tmp)
		: "r" (0xf00)
		);
}

static inline void
irq_set_pil(unsigned char level)
{
	uint32_t tmp;

	asm volatile (
		"rd %%psr, %0 \n\t"
		"and %0, %2, %0 \n\t"
		"sll %1, 8, %1 \n\t"
		"or %0, %1, %0 \n\t"
		"wr %0, 0, %%psr \n\t"
		: "=r&" (tmp)
		: "r" (level), "r" (~0xf00)
		);
}

static inline void
irq_disable(void)
{
	uint32_t tmp;

	asm volatile (
		"rd %%psr, %0 \n\t"
		"and %0, %1, %0 \n\t"
		"wr %0, 0, %%psr \n\t"
		: "=r" (tmp)
		: "r" (~0xf00)
		);
}

static inline int procnum()
{
	uint32_t tmp;

	// IDENT is stored in ASR16
	asm volatile (
		"rd %%asr16, %0"
		: "=r"(tmp)
		);
			
    return tmp;
}

#endif

