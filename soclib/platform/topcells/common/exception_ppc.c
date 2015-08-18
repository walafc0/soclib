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

asm(
    ".section        \".text\",\"ax\",@progbits \n\t"

    ".globl ppc_boot				\n\t"
	".type  ppc_boot, @function   \n\t"
    "ppc_boot:					\n\t"

	"lis   1, special_base@ha \n\t"
	"mtevpr 1 \n\t"

    "lis   9, _stack@ha				\n\t"
    "la   1, _stack@l(9)				\n\t"
	"mfdcr   29,0 \r\n"
    "rlwinm  3,29,12,0,19 \r\n"
	"subf    1,3,1 \r\n"

	"mfmsr 0 \n\t"
	"ori 0, 0, 0x8000 \n\t"
	"mtmsr 0 \n\t"

	"b  main \n\t"
	".size ppc_boot, .-ppc_boot \n\t"
	);

asm(
    ".section        \".ppc_boot\",\"ax\",@progbits			\n\t"

    "b   ppc_boot				\n\t"
	);

#define except_entry_long(name, func, ret_ins)											\
asm(																		\
	".section      \".ppc_special." #name "\",\"ax\",@progbits			\n\t"	\
    ".globl ppc_special_" #name "_entry				\n\t"					\
	".type  ppc_special_" #name "_entry, @function   \n"					\
    "ppc_special_" #name "_entry:					\n\t"					\
    "stwu 1, -16(1)					\n\t"					\
    "stw 0, 8(1)					\n\t"					\
    "mflr 0					\n\t"					\
    "stw 0, 12(1)					\n\t"					\
	"bl " #func "\n\t"											\
    "lwz 0, 12(1)					\n\t"					\
    "mtlr 0					\n\t"					\
    "lwz 0, 8(1)					\n\t"					\
    "addi 1, 1, 16					\n\t"					\
	#ret_ins "\n\t"														\
	".size ppc_special_" #name "_entry, .-ppc_special_" #name "_entry \n\t"	\
	)

#define except_entry_jump(name, dest)											\
asm(																		\
	".section      \".ppc_special." #name "\",\"ax\",@progbits			\n\t"	\
    ".globl ppc_special_" #name "_entry				\n\t"					\
	".type  ppc_special_" #name "_entry, @function   \n"					\
    "ppc_special_" #name "_entry:					\n\t"					\
	"b " #dest "\n\t"											\
	".size ppc_special_" #name "_entry, .-ppc_special_" #name "_entry \n\t"	\
	)

except_entry_long(critical_input, _critical_input, rfci);
except_entry_long(watchdog, _watchdog, rfci);
except_entry_long(debug, _debug, rfci);
except_entry_long(machine_check, _machine_check, rfci);
except_entry_long(instruction_storage, _instruction_storage, rfi);
except_entry_long(program, _program, rfi);
except_entry_long(data_storage, _data_storage, rfi);
except_entry_long(data_tlb_miss, _data_tlb_miss, rfi);
except_entry_long(alignment, _alignment, rfi);
except_entry_long(external, _external, rfi);
except_entry_long(syscall, _syscall, rfi);
except_entry_jump(programmable_interval_timer, ppc_special_programmable_interval_timer_entry);
except_entry_jump(fixed_interval_timer, ppc_special_fixed_interval_timer_entry);
except_entry_long(instruction_tlb_miss, _instruction_tlb_miss, rfi);

void _critical_input()
{
	interrupt_hw_handler(0);
}

void _watchdog()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _debug()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _machine_check()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _instruction_storage()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _program()
{
	printf("Exception: %s\n", __FUNCTION__);
}

void _data_storage()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _data_tlb_miss()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _alignment()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _external()
{
	interrupt_hw_handler(1);
}

void _syscall()
{
	interrupt_sys_handler(0);
}

void _programmable_interval_timer()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _fixed_interval_timer()
{
    printf("Exception: %s\n", __FUNCTION__);
}

void _instruction_tlb_miss()
{
    printf("Exception: %s\n", __FUNCTION__);
}

