/*
 * Check that on return from exception the modified EPC is handled.
 * We use a load to an unaligned address to trigger the exception.
 */
#include <registers.h>
#include <misc.h>

	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE

	PRINT(startstr)

	/* reset cop0 status (keep BEV) */
	lui	a0, 0x0040;
	mtc0	a0, COP0_STATUS

	li	a0, 1
	lw	a0, 0(a0) /* fault */

	/* we shouldn't get there */
	EXIT(1)

	.globl exret

exret:
	PRINT(eretstr)
	EXIT(0)

	.globl excep
excep:
	.set noreorder
	PRINT(statusstr)
	mfc0	a0, COP0_STATUS
	PRINTX

	PRINT(causestr)
	mfc0	a0, COP0_CAUSE
	PRINTX

	PRINT(pcstr)
	mfc0	a0, COP0_EXPC
	PRINTX

	PRINT(badvastr)
	mfc0	a0, COP_0_BADVADDR
	PRINTX

	PUTCHAR('\n')
	la	a0, exret
	mtc0	a0, COP0_EXPC
	eret

	.rodata:
statusstr: .ascii "status \0"
causestr: .ascii " cause \0"
pcstr: .ascii " pc \0"
badvastr: .ascii " badva \0"
startstr: .ascii "start\n\0"
eretstr: .ascii "eret\n\0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl evect
evect:
	j	excep
	nop
	.data
myvar:	.word 0
