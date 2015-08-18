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
 * Maintainers: tarik.graba@telecom-paristech.fr
 */

#include "stdio.h"
#include "system.h"
#include "../segmentation.h"

#define IRQ(x) 0x1<<x

/* time function code is in stdio.c */
extern int time();

int fibo(int n) {
    if (n==0)
        return 1;
    else if (n==1)
        return 1;
    else return fibo(n-1) + fibo(n-2);
}

int main(void) {

    printf("Hello from processor %d\n", procnum());
    printf("Start Time %d\n",cpu_cycles());

    // enable irqs
    irq_enable();
    // unmask irq 1
    set_irq_mask(IRQ(1));

    printf("Fibo(1) = %d\n", fibo(1));
    printf("Fibo(2) = %d\n", fibo(2));
    printf("Fibo(3) = %d\n", fibo(3));
    printf("Fibo(4) = %d\n", fibo(4));
    printf("Fibo(5) = %d\n", fibo(5));
    printf("Fibo(6) = %d\n", fibo(6));
    printf("Fibo(7) = %d\n", fibo(7));
    printf("Fibo(8) = %d\n", fibo(8));
    printf("Fibo(9) = %d\n", fibo(9));
    printf("Fibo(10) = %d\n", fibo(10));

    printf("End Time %d\n",cpu_cycles());
//    while(1);
    return 0;
}

void interrupt_handler()
{
    printf( "\n############################\n");
    printf(   " irq recieved @ time : %d\n",cpu_cycles());
    printf(   "############################\n");
    ack_pend_irq(IRQ(1));
}

#undef IRQ
