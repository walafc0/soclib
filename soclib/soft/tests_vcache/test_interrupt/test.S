/*
 * Check that we can take an interrupt, and return from it.
 * We use a software-triggerted interrupt.
 */
#include <registers.h>
#include <misc.h>


	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE

	/* reset cop0 status (keep BEV), enbale soft interrupt 0 */
	lui     a0, 0x0040;
	addiu	a0, 0x0101;
	mtc0    a0, COP0_STATUS

	/* reset cause, make sure IV is off */
	mtc0	zero, COP0_CAUSE

	PRINT(startstr)

	/* generate an interrupt */
	mfc0	a0, COP0_CAUSE
	nop
	ori	a0, 0x0100
	mtc0	a0, COP0_CAUSE
	li	a0, MAGIC1
	PRINTX
	PUTCHAR('\n')
	/* we should end here after taking the exeption */
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
	/* clear interrupt */
	mfc0	a0, COP0_CAUSE
	xori	a0, 0x100
	mtc0	a0, COP0_CAUSE
	nop
	eret
	/* we should not end there */
	EXIT(1)

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
