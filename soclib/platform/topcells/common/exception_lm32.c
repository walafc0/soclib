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
 * Copyright (c) TelecomParisTECH
 *         Tarik Graba <tarik.graba@telecom-paristech.fr>, 2009
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 */


#include "stdlib.h"
#include "stdio.h"

asm(
    ".section .lm32_boot, \"ax\", @progbits      \n"
    /* START OF EXCEPTION HANDLER */
    "    .global     _start                 \n"
    "_start:                                \n"
    "_lm32_reset_:                          \n"
    "    xor  r0, r0, r0                    \n"
    "    bi _crt0                           \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "_breakpoint_handler:                   \n"
    "    bi breakpoint_handler              \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "_instruction_bus_error_handler:        \n"
    "    bi instruction_bus_error_handler   \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "_watchpoint_handler:                   \n"
    "    bi watchpoint_handler              \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "_data_bus_error_handler:               \n"
    "    bi data_bus_error_handler          \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "_divide_by_zero_handler:               \n"
    "    bi divide_by_zero_handler          \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "_interrupt_handler:                    \n"
    "    sw  (sp+0), ra                     \n"
    "    calli _save_all                    \n"
    "    calli interrupt_handler            \n"
    "    bi _restore_all_and_eret           \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "_system_call_handler:                  \n"
    "    bi system_call_handler             \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    "    nop                                \n"
    /* END OF EXCEPTION HANDLER */
    "_crt0:                                   "
    "    wcsr IE,r0                         \n" /* disable interrupts */
    "    wcsr IM,r0                         \n" /* mask all interrupts */
    "    mvhi    r1, hi(_lm32_reset_)       \n"
    "    ori     r1, r1, lo(_lm32_reset_)   \n"
    "    wcsr    EBA, r1                    \n" /* Set Exception base address */
    "    mvhi    r1, hi(main)               \n"
    /* Setup stack and global pointer */
    "    mvhi    sp, hi(_stack)             \n"
    "    ori     sp, sp, lo(_stack)         \n"
    "    ori     r1, r1, lo(main)           \n"
    "    call    r1                         \n" /* branch to main */
    "_exit:                                 \n"
    "_exit_lm32:                            \n"
    "    bi _exit                           \n"
    "_save_all:                             \n"
    "    addi    sp, sp, -56                \n"
    "    sw      (sp+4), r1                 \n"
    "    sw      (sp+8), r2                 \n"
    "    sw      (sp+12), r3                \n"
    "    sw      (sp+16), r4                \n"
    "    sw      (sp+20), r5                \n"
    "    sw      (sp+24), r6                \n"
    "    sw      (sp+28), r7                \n"
    "    sw      (sp+32), r8                \n"
    "    sw      (sp+36), r9                \n"
    "    sw      (sp+40), r10               \n"
    "    /*     ra      */                  \n"
    "    sw      (sp+48), ea                \n"
    "    sw      (sp+52), ba                \n"
    "    /* ra is already in stacK */       \n"
    "    lw      r1, (sp+56)                \n"
    "    sw      (sp+44), r1                \n"
    "    ret                                \n"

    "_restore_all_and_eret:                 \n"
    "    lw      r1, (sp+4)                 \n"
    "    lw      r2, (sp+8)                 \n"
    "    lw      r3, (sp+12)                \n"
    "    lw      r4, (sp+16)                \n"
    "    lw      r5, (sp+20)                \n"
    "    lw      r6, (sp+24)                \n"
    "    lw      r7, (sp+28)                \n"
    "    lw      r8, (sp+32)                \n"
    "    lw      r9, (sp+36)                \n"
    "    lw      r10, (sp+40)               \n"
    "    lw      ra, (sp+44)                \n"
    "    lw      ea, (sp+48)                \n"
    "    lw      ba, (sp+52)                \n"
    "    addi    sp,  sp, 56                \n"
    "    eret                               \n"
    );


/* Dummy IRQ HANDLER */

void interrupt_handler()
{
    printf( "\n############################\n" );
    printf(   " irq recieved @ time : %d\n",cpu_cycles() );
    printf(   "############################\n" );
}

/* Dummy exception handlers */
void breakpoint_handler ()
{
    printf( "\n #####> Exiting on break point !!\n" );
    exit(-1);
}

void instruction_bus_error_handler ()
{
    printf( "\n #####> Exiting on instruction bus error !!\n" );
    exit(-1);
}

void watchpoint_handler ()
{
    printf( "\n #####> Exiting on watch point !!\n" );
    exit(-1);
}

void data_bus_error_handler ()
{
    printf( "\n #####> Exiting on data bus error !!\n" );
    exit(-1);
}

void divide_by_zero_handler ()
{
    printf( "\n #####> Exiting on divide by zero !!\n" );
    exit(-1);
}

void system_call_handler ()
{
    printf( "\n #####> Exiting on syscall !!\n" );
    exit(-1);
}
