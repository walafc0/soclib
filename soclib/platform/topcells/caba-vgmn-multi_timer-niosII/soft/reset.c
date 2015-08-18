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
 * This is the reset for the Nios2 processor.
 *
 * 
 */

__asm__(

	".section .entry, \"xa\"             \n"
	".globl _start                        \n"

	"_start :                             \n"
	/* reset PIE bit of status register    */
	"wrctl    status, zero                \n"

	/* disable interruption               */
	"wrctl    ienable, zero               \n"

	/* get cpu ID and adjust stack */
	"rdctl	r16,	 cpuid	             \n" /* read processor identifier */
	"andi      r16, r16, 0x3ff           \n"
	"slli      r16, r16, 10              \n"
	"movia     sp,__alt_stack_pointer    \n"

	"sub       sp, sp, r16               \n"

	/* setup global data pointer */
	"movia     gp,      _gp              \n"

	"movia     r18,     main             \n"
	"jmp       r18                       \n"
);
