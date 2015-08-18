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

#include "soclib/xicu.h"
#include "system.h"
#include "stdio.h"

#include "../segmentation.h"

static const int period[4] = {10000, 15000};

static int max_interrupts = 80;

void pti_irq_handler()
{
	printf("Timer wrapped at cycle %d on cpu %d\n", cpu_cycles(), procnum());
	soclib_io_get(
		base(XICU),
		XICU_REG(XICU_PTI_ACK, procnum()));
	soclib_io_set(
		base(XICU),
		XICU_REG(XICU_WTI_REG, procnum()),
		procnum() );
}

void wti_irq_handler()
{
	int left = atomic_add(&max_interrupts, -1);

	int src = soclib_io_get(
		base(XICU),
		XICU_REG(XICU_WTI_REG, procnum()-2));

	printf("IPI received from %d at cycle %d on cpu %d %d interrupts to go\n", src, cpu_cycles(), procnum(), left);

	if ( ! left )
		exit(0);
}

void irq_handler(int irq)
{
	if ( procnum() < 2 )
		pti_irq_handler();
	else
		wti_irq_handler();
}

int main(void)
{
	const int cpu = procnum();

	printf("Hello from processor %d\n", procnum());
	
	set_irq_handler(irq_handler);
	enable_hw_irq(0);
	irq_enable();

	if ( procnum() < 2 ) {
		soclib_io_set(
			base(XICU),
			XICU_REG(XICU_PTI_PER, procnum()),
			period[cpu]);

		soclib_io_set(
			base(XICU),
			XICU_REG(XICU_MSK_PTI, procnum()),
			1<<procnum());
	} else {
		soclib_io_set(
			base(XICU),
			XICU_REG(XICU_MSK_WTI, procnum()),
			1<<(procnum()-2));
	}
	
	while (1)
		pause();
	return 0;
}
