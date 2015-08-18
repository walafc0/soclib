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
 *         Dimitri Refauvelet <dimitri.refauvelet@lip6.fr>, 2009
 *
 * Maintainers: dimitri.refauvelet@etu.upmc.fr
 */

#include "system.h"

#include "../segmentation.h"

uint32_t lock, cpt;

int main(void)
{
	const int cpu = procnum();
	int * blackhole = BLACKHOLE_BASE;
	
	lock_lock(&lock);
	printf("Cpu %x booted\n", cpu);
	lock_unlock(&lock);
	
	
	if (cpu == 0 || cpu == 2){
	  lock_lock(&lock);
	  printf("Cpu %x says: Bye Bye, I'm going into the blackhole\n", cpu);
	  lock_unlock(&lock);
	  *blackhole=1;
	  lock_lock(&lock);
	  printf("Cpu %x says: I'm a superCPU and i owned this blackhole\n", cpu);
	  lock_unlock(&lock);
	}
	else{
	  lock_lock(&lock);
	  printf("Cpu %x says: I'm not stupid\n", cpu);
	  lock_unlock(&lock);
	}
	
	while(1){
	  cpt++;
	  if(cpt == 50000){
	    lock_lock(&lock);
	    printf("Cpu %x says: I'm still alive\n", cpu);
	    lock_unlock(&lock);
	    cpt=0;
	  }
	}
}
