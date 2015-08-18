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
 *         Nicolas Pouillon <nipo@ssji.net>, 2009
 *
 * Maintainers: nipo
 */

asm(
    ".section        .arm_boot,\"ax\"			\n"

    ".globl arm_vectors				\n\t"
	".type   arm_vectors, %function \n\t"
	"b arm_boot                     \n\t"
	"b arm_undef                    \n\t"
	"b arm_swi                      \n\t"
	"b arm_dabt                     \n\t"
	"nop                            \n\t"
	"b arm_irq                      \n\t"
	"b arm_fiq                      \n\t"
	".size   arm_vectors, .-arm_vectors\n\t"

    ".globl arm_boot				\n\t"
	".type   arm_boot, %function   \n\t"
    "arm_boot:					\n\t"
    "arm_undef:					\n\t"
    "arm_swi:					\n\t"
    "arm_dabt:					\n\t"
    "arm_fiq:					\n\t"

	/* Get CPU num */
	"mrc  p15,0,r4,c15,c12,1     \n\t"
	"lsl  r4, r4, #9             \n\t"
	/* Shift the stack by 512B, set it */
    "ldr  r13, =_stack				\n\t"
	"subs r13, r13, r4              \n\t"

	/* Switch to IRQ32 and set a stack */
	"mrs  r0, cpsr               \n\t"
	"bic  r2, r0, #0x1f          \n\t"
	"orr  r2, r2, #0x12          \n\t"
	"msr  cpsr, r2               \n\t"
    "ldr  r13, =_irq_stack+500+512*3   \n\t"
	"subs r13, r13, r4              \n\t"
	"msr  cpsr, r0               \n\t"
	/* To main */
	"ldr  r12, =main             \n\t"
	"bx   r12 \n\t"
	".size   arm_boot, .-arm_boot     \n\t"

	/* Stupid IRQ handler */
    ".globl arm_irq                    \n\t"
	".type   arm_irq, %function        \n\t"
    "arm_irq:                          \n\t"
    "push   {r0, r1, r2, r3, r12, lr}  \n\t"
    "mov    r0, #1                     \n\t" 
    "ldr    r12, =interrupt_hw_handler \n\t"
	"mov    lr, pc                     \n\t"
    "bx     r12                        \n\t"
    "pop    {r0, r1, r2, r3, r12, lr}  \n\t"
    "subs   pc, r14, #4                \n\t"
	".size   arm_irq, .-arm_irq        \n\t"

	);

char _irq_stack[512*4];
