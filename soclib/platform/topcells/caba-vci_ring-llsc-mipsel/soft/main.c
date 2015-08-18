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
 * Maintainers: abdelmalek.si-merabet@lip6.fr nipo
 */

#include "soclib/timer.h"
#include "system.h"
#include "stdio.h"

#include "../segmentation.h"

extern uint32_t lock;
extern volatile uint32_t count;

int main(void)
{
	const int cpu = procnum();
	int i;

	printf("Cpu %x booted\n", cpu);

	for (i=0; i<100; ++i) {
		lock_lock(&lock);
		printf("Hello from cpu %x\n", cpu);
		lock_unlock(&lock);
	}

	if (atomic_inc(&count) == 4)
		exit(0);
	while(1);
}