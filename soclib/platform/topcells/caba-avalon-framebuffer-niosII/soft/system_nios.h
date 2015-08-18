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
 * Maintainers: irisa
 */


#define get_ctl(x)				\
  ({unsigned int __ctl_x;			\
    __asm__("rdctl %0, ctl"#x:"=r"(__ctl_x));	\
    __ctl_x;})

#define set_ctl(x,val)				\
  ({__asm__("wrctl ctl"#x", %0"::"r"(val));})

static inline int get_ctlRegCount()
{
  return (get_ctl(17));
}

static inline void
irq_disable(void)
{
  __asm__ volatile (
		    ".set noat 					\n"
		    "	rdctl	r1, status			\n"
		    "	ori	r1, r1, 0x1         		\n"
		    "	addi	r1, r1, -1           		\n"
		    "	wrctl	status, r1			\n"
		    );
}

static inline void
irq_enable(void)
{
  __asm__ volatile (
		    ".set noat 					\n"
		    "	rdctl	r1, status			\n"
		    "	ori	r1, r1, 0x1                     \n"
		    "	wrctl	status, r1			\n"
		    ); 
}

static inline int procnum()
{
  return (get_ctl(5));
}
