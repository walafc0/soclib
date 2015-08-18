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

__asm__(
	".section        .excep,\"xa\",@progbits                   	\n"

	".globl niosExceptionProcessing 	                	\n"
	"niosExceptionProcessing:  		                        \n"
	".set noat						        \n"
	".set nobreak						        \n"


	/*************************************************************
		 start exception processing
	**************************************************************/

	/* save registers  */

	"       addi    sp, sp, -4*32                                   \n"
	"	stw	r1, 1*4(sp)		    	        	\n" /* AT reg */
	"	stw	r2, 2*4(sp)		        	        \n" /* Return value regs */
	"	stw	r3, 3*4(sp)		         	        \n" /* Return value regs */
	"	stw     r4, 4*4(sp)	    		        	\n"  /* args regs */
	"	stw     r5, 5*4(sp)	    		        	\n"  /* args regs */
	"	stw     r6, 6*4(sp)	    		        	\n"  /* args regs */
	"	stw     r7, 7*4(sp)	    		        	\n"  /* args regs */
	"	stw	r8, 8*4(sp)		                	\n" /* Caller-Saved regs */
	"	stw	r9, 9*4(sp)					\n" /* Caller-Saved regs */
	"	stw	r10, 10*4(sp)					\n" /* Caller-Saved regs */
	"	stw	r11, 11*4(sp)			        	\n" /* Caller-Saved regs */
	"	stw	r12, 12*4(sp)			       		\n" /* Caller-Saved regs */
	"	stw	r13, 13*4(sp)			        	\n" /* Caller-Saved regs */
	"	stw	r14, 14*4(sp)			        	\n" /* Caller-Saved regs */
	"	stw	r15, 15*4(sp)			        	\n" /* Caller-Saved regs */
	"       stw     r16, 16*4(sp)       		                \n"
	"       stw     r17, 17*4(sp)  		                        \n"
	"       stw     r18, 18*4(sp)                                   \n"
	"       stw     r19, 19*4(sp)                                   \n"
	"       stw     r20, 20*4(sp)                                   \n"
	"       stw     r21, 21*4(sp)                                   \n"
	"       stw     r22, 22*4(sp)                                   \n"
	"       stw     r23, 23*4(sp)                                   \n"
	"       stw     et,  24*4(sp)                                   \n"
	"       stw     bt,  25*4(sp)                                   \n"
	"       stw     gp,  26*4(sp)                                   \n"
	"       stw     sp,  27*4(sp)                                   \n"
	"       stw     fp,  28*4(sp)                                   \n"
	"       stw     ea,  29*4(sp)                                   \n"
	"       stw     ba,  30*4(sp)                                   \n"
	"	stw     ra,  31*4(sp)	    		        	\n"  /* return address regs */

	/* read and extract cause ¨*/
	"	rdctl	r8, exception					\n" /* read exception register */

	/*  hardware interrupt ?*/
	"	addi	r10, zero, 2					\n"  /* CAUSE = 2 - hardware interrupt */  
	"	beq	r8, r10, interrupt_hw 				\n"  

	/*  syscall exception ? ?*/
	"	addi	r10, zero, 3					\n"  /* CAUSE = 3 - TRAP inst */
	"	beq	r9, r10, interrupt_sys 				\n"      


    

	/************************************************************
		 exeception handling
	**************************************************************/

	"interrupt_ex: 						        \n"

	/* exception function argument */
	"	add	r4, r8, zero					 \n"     /* cause */ 
	"	addi	r5, ea, 4 				         \n"     /* excecution pointer : ea register known about it */ 
	"	mov	r6, r5  					 \n"     /* bad address if any */
	"       add     r7, sp, zero					 \n"     /* register table on stack */ 
	"       add     r8, sp, zero					 \n"     /* stack pointer */ 

	"	stw     r8,  4*4(sp)					 \n"      

	"	movia	r1, interrupt_ex_handler			\n"
	"	callr	r1	          				\n" 
	"	br	return    				        \n"


	/*************************************************************

		 syscall handling

	**************************************************************/

	"interrupt_sys:          					\n"

	"	br	return    				        \n"


	/*************************************************************

		 hardware interrupt handling

	**************************************************************/

	"interrupt_hw:          					\n"
	"	rdctl	r8, ipending					\n" /* read ipending */
	"       mov     r4, r8                                          \n"
	"	movia   r18, interrupt_hw_handler			\n"
	"	callr	r18	          				\n" 

	"return:                         \n"
	/*************************************************************
		 restore the saved registers
	**************************************************************/
	"skip_interrupt_hw:      		        \n"
	"	ldw	at,	 1*4(sp)		                \n" /* AT reg */
	"	ldw	r2,	 2*4(sp)		                \n" /* Return value regs */
	"	ldw	r3,	 3*4(sp)		                \n" /* Return value regs */
	"	ldw	r4,	 4*4(sp)		                \n" /* Args regs */
	"	ldw	r5,	 5*4(sp)		                \n" /* Args regs */
	"	ldw	r6,	 6*4(sp)		                \n" /* Args regs */
	"	ldw	r7,	 7*4(sp)		                \n" /* Args regs */
	"	ldw	r8,	 8*4(sp)		                \n" /* Caller-Saved regs */
	"	ldw	r9,	 9*4(sp)		                \n" /* Caller-Saved regs */
	"	ldw	r10,	 10*4(sp)		            \n" /* Caller-Saved regs */
	"	ldw	r11,	 11*4(sp)		            \n" /* Caller-Saved regs */
	"	ldw	r12,	 12*4(sp)		            \n" /* Caller-Saved regs */
	"	ldw	r13,	 13*4(sp)		            \n" /* Caller-Saved regs */
	"	ldw	r14,	 14*4(sp)		            \n" /* Caller-Saved regs */
	"	ldw	r15,	 15*4(sp)		            \n" /* Caller-Saved regs */
	"	ldw	r16,	 16*4(sp)		            \n"
	"	ldw	r17,	 17*4(sp)		            \n"
	"	ldw	r18,	 18*4(sp)		            \n"
	"	ldw	r19,	 19*4(sp)		            \n"
	"	ldw	r20,	 20*4(sp)		            \n"
	"	ldw	r21,	 21*4(sp)		            \n"
	"	ldw	r22,	 22*4(sp)		            \n"
	"	ldw	r23,	 23*4(sp)		            \n"
	"       ldw     et,      24*4(sp)                                \n"
	"	ldw	bt,	 25*4(sp)	                \n"
	"	ldw	gp,	 26*4(sp)	                \n"
	"	ldw	fp,	 28*4(sp)	                \n"
	"	ldw	ea,	 29*4(sp)	                \n"
	"	ldw	ba,	 30*4(sp)	                \n"
	"	ldw	ra,	 31*4(sp)	                \n"
	"	addi	sp, sp,	4*32				    \n"

	/*************************************************************
		 return to the interrupted instruction
	**************************************************************/

	"	eret            				    \n"       
	".set at						        \n"
	);

