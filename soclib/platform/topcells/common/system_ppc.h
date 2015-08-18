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
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 *
 * Maintainers: nipo
 */


#define dcr_get(x)					\
({unsigned int __val;				\
__asm__("mfdcr %0, "#x:"=r"(__val));\
__val;})

#define spr_get(x)					\
({unsigned int __val;				\
__asm__("mfspr %0, "#x:"=r"(__val));\
__val;})

static inline void
irq_disable(void)
{
	uint32_t tmp;

	asm volatile (
		"mfmsr %0		\n\t"
		"and %0, %0, %1		\n\t"
		"mtmsr %0		\n\t"
		: "=r" (tmp)
		: "r" (~0x8000)
		);
}

static inline void
irq_enable(void)
{
	uint32_t tmp;

	asm volatile (
		"mfmsr %0		\n\t"
		"ori %0, %0, 0x8000	\n\t"
		"mtmsr %0		\n\t"
		: "=r" (tmp)
		);
}

static inline int procnum()
{
    return dcr_get(0);
}

static inline void cache_flush(void *base, size_t len)
{
    size_t i;
	for (i=0; i<len+16; i+=4){
        asm volatile (
            "dcbi 0, %0"
            :
            : "r" ((uint32_t)base+i)
            : "memory"
		);
	}
}
