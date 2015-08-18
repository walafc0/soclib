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

#if __mips >= 32
#define get_cp0(x, sel)									\
({unsigned int __cp0_x;								\
__asm__("mfc0 %0, $"#x", "#sel:"=r"(__cp0_x));	\
__cp0_x;})
#define set_cp0(x, sel, val)							\
	do{__asm__ __volatile__("mtc0 %0, $"#x", "#sel::"r"(val));}while(0)
#define get_cp2(x, sel)									\
({unsigned int __cp2_x;								\
__asm__("mfc2 %0, $"#x", "#sel:"=r"(__cp2_x));	\
__cp2_x;})
#define set_cp2(x, sel, val)							\
	do{__asm__ __volatile__("mtc2 %0, $"#x", "#sel::"r"(val));}while(0)
#else
#define get_cp0(x)									\
({unsigned int __cp0_x;								\
__asm__("mfc0 %0, $"#x:"=r"(__cp0_x));	\
__cp0_x;})
#define set_cp0(x, val)							\
	do{__asm__ __volatile__("mtc0 %0, $"#x::"r"(val));}while(0)
#define get_cp2(x)									\
({unsigned int __cp2_x;								\
__asm__("mfc2 %0, $"#x:"=r"(__cp2_x));	\
__cp2_x;})
#define set_cp2(x, val)							\
	do{__asm__ __volatile__("mtc2 %0, $"#x::"r"(val));}while(0)
#endif

static inline void
irq_disable(void)
{
  __asm__ volatile (
		    ".set push			\n"
		    ".set noat			\n"
		    ".set reorder		\n"
#if __mips >= 32 && __mips_isa_rev >= 2
		    "di				\n"
		    "ehb			\n"
#else
		    "mfc0	$1,	$12	\n"
		    "ori	$1,	0x1	\n"
		    "addiu	$1,	-1	\n"
		    ".set noreorder		\n"
		    "mtc0	$1,	$12	\n"
#endif
		    "nop                        \n"
		    "nop                        \n"
		    "nop                        \n"
		    "nop                        \n"
		    //"MTC0_WAIT			\n"
		    ".set pop			\n"
		    );
}

static inline void
irq_enable(void)
{
  __asm__ volatile (
		    ".set push			\n"
		    ".set noat			\n"
		    ".set reorder		\n"
#if __mips >= 32 && __mips_isa_rev >= 2
		    "ei				\n"
		    "ehb			\n"
#else
		    "mfc0	$1,	$12	\n"
		    "ori	$1,	1	\n"
		    "mtc0	$1,	$12	\n"
#endif
		    ".set pop			\n"
		    );
}

static inline int procnum()
{
#if __mips >= 32
  return (get_cp0(15,1)&0x3ff);
#else

    return (get_cp0(15)&0x3ff);
#endif
}

static inline void cache_flush(const void *base, size_t len)
{
    size_t i;
	for (i=0; i<len+16; i+=4){
#if __mips >= 32
		asm volatile(
            " cache %0, %1"
            : : "i" (0x11) , "R" (*(uint8_t*)(base+i)));
#else
		asm volatile("lw $0, 0(%0)"::"r"((uint32_t)base+i));
#endif
	}
}
