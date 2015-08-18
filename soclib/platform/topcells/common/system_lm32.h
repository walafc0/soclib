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
 *         Tarik Graba  <graba@telecom-paristech.fr>
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 */

#ifndef _SYSTEM_LM32_H_
#define _SYSTEM_LM32_H_

static inline uint32_t get_cc(void) 
{
    uint32_t tmp;
    asm volatile (
            "rcsr %0, CC" :"=r"(tmp)
            );
    return tmp;
}

static inline void irq_enable(void)
{
    uint32_t tmp;

    asm volatile ( "rcsr %0, IE\n\t"
            "ori  %0,%0,0x0001\n\t"
            "wcsr IE,%0"
            :"=r"(tmp));

}

static inline void set_irq_mask(uint32_t mask)
{
    asm volatile ("wcsr IM, %0"::"r"(mask));
}

static inline uint32_t get_irq_mask(void)
{
    uint32_t tmp;

    asm volatile ("rcsr %0,IM":"=r"(tmp));
    return tmp;
}

static inline void ack_pend_irq(uint32_t irqs)
{
    asm volatile ("wcsr IP, %0"::"r"(irqs));
}

static inline uint32_t get_pend_irq(void)
{
    uint32_t tmp;

    asm volatile ("rcsr %0,IP":"=r"(tmp));
    return tmp;
}
  
static inline void irq_disable(void)
{
    uint32_t tmp;

    asm volatile ( "rcsr %0, IE\n\t"
            "andi %0,%0,0xFFFE\n\t"
            "wcsr IE,%0"
            :"=r"(tmp));
}

static inline int procnum()
{

    // TODO!!
    return 0;
}

#endif

