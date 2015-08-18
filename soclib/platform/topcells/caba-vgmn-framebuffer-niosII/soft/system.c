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

#include "system.h"
#include "stdio.h"
#include "stdlib.h"

#undef putchar
int putchar(const int x)
{
	return __inline_putchar(x);
}

#undef getchar
int getchar()
{
	return __inline_getchar();
}

int puts(const char *str)
{
	while (*str)
		putchar(*str++);
	putchar('\n');
	return 0;
}

void puti(const int i)
{
	if ( i>=10 )
		puti(i/10);
	putchar(i%10+'0');
}

uint32_t run_cycles()
{
#if defined(__mips__)
# if __mips >= 32
	return get_cp0(9,6);
# else
	return cpu_cycles();
# endif
#elif defined(PPC)
	return dcr_get(3);
#elif defined(__arm__)
	uint32_t ret;
	asm (
		"mrc p15,0,%0,c15,c12,2"
		: "=r"(ret));
	return ret;
#elif defined(NIOS)
	return cpu_cycles();
#else
	return 0;
#endif
}

uint32_t cpu_cycles()
{
#if defined(__mips__)
# if __mips >= 32
	return get_cp0(9, 0);
# else
	return get_cp0(9);
# endif
#elif defined(PPC)
	return dcr_get(284);
#elif defined(__lm32__)
    return get_cc();
#elif defined(__arm__)
	uint32_t ret;
	asm (
		"mrc p15,0,%0,c15,c12,1"
		: "=r"(ret));
	return ret;
#elif defined(NIOS)
	return get_ctlRegCount();
#else
	return 0;
#endif
}

ex_handler_t *user_ex_handler = 0;

void set_ex_handler(ex_handler_t *handler)
{
	user_ex_handler = handler;
}

void interrupt_ex_handler(
	unsigned int type, void *execptr,
	void *dataptr, void *regtable,
	void *stackptr)
{
    if (user_ex_handler)
        user_ex_handler(type, execptr, dataptr, regtable, stackptr);
    else {
        printf("\nException at 0x%x: 0x%x\n", (unsigned int)execptr, type);
        exit(1);
    }
}

sys_handler_t *user_sys_handler = 0;

void set_sys_handler(sys_handler_t *handler)
{
    user_sys_handler = handler;
}

void interrupt_sys_handler(
        unsigned int service, void *execptr,
        void *regtable, void *stackptr)
{
    if (user_sys_handler)
        user_sys_handler(service, execptr, regtable, stackptr);
    else {
        printf("Syscall at 0x%x: 0x%x\n", (unsigned int)execptr, service);
        exit(1);
    }
}

irq_handler_t *user_irq_handler = 0;

void set_irq_handler(irq_handler_t *handler)
{
	user_irq_handler = handler;
}

void enable_hw_irq(unsigned int n)
{
#if __mips__
# if __mips >= 32
	uint32_t status = get_cp0(12, 0);
# else
	uint32_t status = get_cp0(12);
# endif
	status |= 1<<(10+n);
# if __mips >= 32
	set_cp0(12, 0, status);
# else
	set_cp0(12, status);
# endif
#elif PPC
	uint32_t msr;
	asm("mfmsr %0":"=r"(msr));
	if ( n ) {
		// external
		msr |= (1<<(31-16));
	} else {
		// critical
		msr |= (1<<(31-14));
	}
	asm("mtmsr %0"::"r"(msr));
#elif __lm32__
	irq_enable();
#elif __arm__
	// Nothing, there is 1 IRQ line only
#elif __sparc__
       irq_enable();
#elif NIOS
       // NIOSII ienable register is set
	uint32_t enable = get_ctl(3);
	enable |= 1<<n;
	set_ctl(3,enable);
#else
# warning Please implement IRQ enabling for this arch
#endif
}


void interrupt_hw_handler(unsigned int irq)
{
#if __sparc__
	printf("Exception: %s irq %d\n", __FUNCTION__, irq);
	if ( user_irq_handler )
		user_irq_handler(irq);
#elif __lm32__
	int i;

  	printf("Exception: %s irq %d\n", __FUNCTION__, irq);

	for (i=0; i<32;++i) {
		if (irq&1)
			break;
		irq>>=1;
	}

	if ( user_irq_handler )
		user_irq_handler(i);
	else
		exit(1);
#elif NIOS
	int i;

	printf("Exception: %s irq %d\n", __FUNCTION__, irq);

	for (i=0; i<32;++i) {
		if (irq&1)
			break;
		irq>>=1;
	}

	if ( user_irq_handler )
		user_irq_handler(i);
	else
		exit(1);
#else
	int i;

//	printf("Exception: %s irq %d\n", __FUNCTION__, irq);

	for (i=0; i<6;++i) {
		if (irq&1)
			break;
		irq>>=1;
	}

	if ( user_irq_handler )
		user_irq_handler(i);
	else
		exit(1);
#endif
}


static inline uint32_t ll( uint32_t *addr )
{
	uint32_t ret;
#if __mips__
	__asm__ __volatile__("ll %0, 0(%1)":"=r"(ret):"p"(addr));
#elif PPC
	__asm__ __volatile__("lwarx %0, 0, %1":"=r"(ret):"p"(addr));
#elif NIOS
	__asm__ __volatile__("custom  0, %0, %1, zero":"=r"(ret):"r"(addr));
#else
#warning TODO : implement ll for this CPU !
	ret = *(volatile uint32_t*)addr;
#endif
	return ret;
}

/*
 * Store conditional, return 0 when succes
 */
static inline uint32_t sc( uint32_t *addr, uint32_t value )
{
	uint32_t ret;
#if __mips__
	__asm__ __volatile__("sc %0, 0(%1)":"=r"(ret):"p"(addr), "0"(value):"memory");
	ret = !ret;
#elif PPC
	ret = 0;
	__asm__ __volatile__("stwcx. %2, 0, %1    \n\t"
						 "mfcr   %0    \n\t"
						 :"=r"(ret)
						 :"p"(addr), "r"(value)
						 :"memory");
	ret = ! (ret&0x20000000);
#elif NIOS
	__asm__ __volatile__("custom  1, %0, %1, zero":"=r"(ret):"r"(addr), "0"(value):"memory");
	ret = !ret;
#else
#warning TODO : implement sc for this CPU !
	*(volatile uint32_t*)addr = value;
	ret = 0;
#endif
	return ret;
}

uint32_t atomic_add( uint32_t *addr, uint32_t delta )
{
#if 1
	uint32_t val, failed;
	do {
		val = ll(addr);
		val += delta;
		failed = sc(addr, val);
	} while (failed);
	return val;
#else
	volatile uint32_t *a = addr;
	uint32_t oldval;

	do {
		oldval = *a;
	} while ( ! __sync_bool_compare_and_swap(a, oldval, oldval+delta) );

	return oldval;
#endif
}

void lock_lock( uint32_t *lock )
{
#if 1
	uint32_t failed;
	do {
		while ( ll(lock) != 0 )
			;
		failed = sc(lock, 1);
	} while (failed);
#elif defined(__mips__)
	__asm__ __volatile__(
		".set push        \n\t"
		".set noreorder   \n\t"
		".set noat        \n\t"
		"1:               \n\t"
		"ll    $2, 0(%0)  \n\t"
		"bnez  $2, 1b     \n\t"
		"ori   $1, $0, 1  \n\t"
		"sc    $1, 0(%0)  \n\t"
		"beqz  $1, 1b     \n\t"
		"nop              \n\t"
		"2:               \n\t"
		".set pop         \n\t"
		:
		: "p"(lock), "m"(*lock)
		: "$1", "$2"
		);
#elif PPC
	uint32_t tmp;
	asm volatile(
		"1:			\n"
		"	lwarx	%0,0,%1         \n"
		"   cmpi    0, 0, %0, 0           \n"
		"   bne		1b				\n"
		"	ori		%0,%0,1		\n"
		"   stwcx.	%0,0,%1		\n"
		"	bne-	1b		\n"
		"2:                \n"
		: "=&r" (tmp)
		: "p" (lock), "m" (*lock)
		);
#else
#warning TODO : implement lock_lock for this arch !
#endif
}

void lock_unlock( uint32_t *lock )
{
	*lock = 0;
}

void pause()
{
#if defined(__mips__)
# if __mips >= 32
	__asm__ __volatile__("wait");
# else
#  warning No pause for this architecture
# endif
#elif PPC
	asm volatile("nap");
#elif NIOS
	asm volatile("nop");
#else
#  warning No pause for this architecture
#endif
}
