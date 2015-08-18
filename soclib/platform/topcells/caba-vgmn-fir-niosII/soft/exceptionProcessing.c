/* -*- c++ -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 * 
 * Copyright (C) IRISA/INRIA, 2007
 *         Francois Charot <charot@irisa.fr>
 *
 * 
 * This is the  exception processing for the Nios2 processor.
 *
 * 
 */

/* extern void softwareException (void); */

__asm__(
		".section        .excep, \"xa\"     	\n"
		".globl niosExceptionProcessing 		\n"
		/*     ".type niosExceptionProcessing, @function           \n" */

		".set noat						        \n"
		".set nobreak						    \n"

		"niosExceptionProcessing:  		        \n"

		/*************************************************************
		 start exception processing
		 **************************************************************/

		/*************************************************************
		 save registers
		 **************************************************************/
		"	addi	sp, sp,	-128				\n"
		"	stw	zero,	 0(sp)	            	\n"
		"	stw	at,	 4(sp)		                \n" /* AT reg */
		"	stw	r2,	 8(sp)		                \n" /* Return value regs */
		"	stw	r3,	 12(sp)		                \n" /* Return value regs */
		"	stw	r4,	 16(sp)		                \n" /* Args regs */
		"	stw	r5,	 20(sp)		                \n" /* Args regs */
		"	stw	r6,	 24(sp)		                \n" /* Args regs */
		"	stw	r7,	 28(sp)		                \n" /* Args regs */
		"	stw	r8,	 32(sp)		                \n" /* Caller-Saved regs */
		"	stw	r9,	 36(sp)		                \n" /* Caller-Saved regs */
		"	stw	r10,	 40(sp)		            \n" /* Caller-Saved regs */
		"	stw	r11,	 44(sp)		            \n" /* Caller-Saved regs */
		"	stw	r12,	 48(sp)		            \n" /* Caller-Saved regs */
		"	stw	r13,	 52(sp)		            \n" /* Caller-Saved regs */
		"	stw	r14,	 56(sp)		            \n" /* Caller-Saved regs */
		"	stw	r15,	 60(sp)		            \n" /* Caller-Saved regs */
		"	stw	r16,	 64(sp)		            \n"
		"	stw	r17,	 68(sp)		            \n"
		"	stw	r18,	 72(sp)		            \n"
		"	stw	r19,	 76(sp)		            \n"
		"	stw	r20,	 80(sp)		            \n"
		"	stw	r21,	 84(sp)		            \n"
		"	stw	r22,	 88(sp)		            \n"
		"	stw	r23,	 92(sp)		            \n"
		/* et is not saved, it has already been changed */
		"	stw	bt,	 100(sp)	                \n"
		"	stw	gp,	 104(sp)	                \n"
		"	stw	sp,	 108(sp)	                \n"
		"	stw	fp,	 112(sp)	                \n"
		"	stw	ea,	 116(sp)	                \n"
		"	stw	ba,	 120(sp)	                \n"
		"	stw	ra,	 124(sp)	                \n"

		/*     "	rdctl	r23,	 status		                \n" /\* read status register *\/*/
		/*     "	addi	r22,	 r0, -1	                        \n" /\* clear PIE bit *\/*/
		/*     "	slli	r22,	 r22, 1	                        \n"  */
		/*      "	and	r23,	 r23, r22                       \n"  */
		/*     "	wrctl	status,	 r23		                \n" /\* save it back *\/*/

		"nios_exception_handler:          		\n"
		/* test to see if the exception was a software exception
		 or caused by an external interrupt */
		"	rdctl	r23,	 estatus		    \n" /* read estatus register */
		"	andi	r23,	 r23, 1 	        \n" /* test EPIE bit */
		"	beq	r23,	 zero, softwareException\n"

		/* scanning of the ipending register and finding of the first active IRQ (of highest priority) */
		"	rdctl	r22,	 ipending		    \n" /* read ipending register */
		"	addi	r21,	 r0,0   		    \n" /* set IRQ counter to 0 */
		"	movia   r20,     interrupt_hw_handler  \n" /* address of processing */

		"	addi	r18,	 r0, -1	            \n" /* used to update ienable if necessary */

		"	beq		r22,	 zero, skip_interrupt_hw\n" /* is there interrupt pending ? */

		/*************************************************************
		 process an external hardware exception
		 **************************************************************/

		"interrupt_hw:          		        \n"
		"	andi	r17,	 r22, 0x1	        \n" /* look at  ipending[i] */
		"	beq		r17,	 zero, next_interrupt\n"
		"	add		r4,	 zero,r21               \n" /* parameter of the interrupt_hw_handler function */
		"	callr   r20		                    \n" /* transfer execution to interrupt_hw_handler */

		"next_interrupt:          		        \n"
		"	srli	r22,	 r22,1  		    \n" /* ipending reg is shifted one bit in order */
													/* to look at the next irq entry */
		"	slli	r18,	 r18,1  		    \n" /* update of ienable  */
		/*    "	wrctl	ienable,  r18		                \n"*/

		"	addi    r21,     r21,1  		    \n" /* increment IRQ counter */
		"	cmpeqi	r19,     r21,32  		    \n"
		"	beq	r19,	 zero, interrupt_hw     \n"

		"	addi	r23,	 r0, -1	            \n" /*  */
		"	wrctl	ienable,  r23		        \n"

		/*************************************************************
		 restore the saved registers
		 **************************************************************/
		"skip_interrupt_hw:      		        \n"
		"	ldw	at,	 4(sp)		                \n" /* AT reg */
		"	ldw	r2,	 8(sp)		                \n" /* Return value regs */
		"	ldw	r3,	 12(sp)		                \n" /* Return value regs */
		"	ldw	r4,	 16(sp)		                \n" /* Args regs */
		"	ldw	r5,	 20(sp)		                \n" /* Args regs */
		"	ldw	r6,	 24(sp)		                \n" /* Args regs */
		"	ldw	r7,	 28(sp)		                \n" /* Args regs */
		"	ldw	r8,	 32(sp)		                \n" /* Caller-Saved regs */
		"	ldw	r9,	 36(sp)		                \n" /* Caller-Saved regs */
		"	ldw	r10,	 40(sp)		            \n" /* Caller-Saved regs */
		"	ldw	r11,	 44(sp)		            \n" /* Caller-Saved regs */
		"	ldw	r12,	 48(sp)		            \n" /* Caller-Saved regs */
		"	ldw	r13,	 52(sp)		            \n" /* Caller-Saved regs */
		"	ldw	r14,	 56(sp)		            \n" /* Caller-Saved regs */
		"	ldw	r15,	 60(sp)		            \n" /* Caller-Saved regs */
		"	ldw	r16,	 64(sp)		            \n"
		"	ldw	r17,	 68(sp)		            \n"
		"	ldw	r18,	 72(sp)		            \n"
		"	ldw	r19,	 76(sp)		            \n"
		"	ldw	r20,	 80(sp)		            \n"
		"	ldw	r21,	 84(sp)		            \n"
		"	ldw	r22,	 88(sp)		            \n"
		"	ldw	r23,	 92(sp)		            \n"
		"	ldw	bt,	 100(sp)	                \n"
		"	ldw	gp,	 104(sp)	                \n"
		"	ldw	fp,	 112(sp)	                \n"
		"	ldw	ea,	 116(sp)	                \n"
		"	ldw	ba,	 120(sp)	                \n"
		"	ldw	ra,	 124(sp)	                \n"
		"	addi	sp, sp,	128				    \n"

		/*************************************************************
		 return to the interrupted instruction
		 **************************************************************/

		"	eret            				    \n"
		".set at						        \n"
);

