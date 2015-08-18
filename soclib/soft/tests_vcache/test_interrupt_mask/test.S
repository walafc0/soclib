/*
 * Check that the interrupt mask in status is properly handled:
 * we generate a software-triggerted interrupt with interrupt enabled but
 * the software interrupt masked. Later we unmask it.
 */
#include <registers.h>
#include <misc.h>


	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE

	/* reset cop0 status (keep BEV), enbale interrupts but mask IM0 */
	lui     a0, 0x0040;
	ori	a0, 0xfe01;
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
	nop
	nop
	nop
	nop
	nop
	nop
	/* unmask the interrupt */
	mfc0	a0, COP0_STATUS
	nop
	ori	a0, 0x0100
	mtc0	a0, COP0_STATUS
	/* we should end here after taking the exeption */
	li	a0, MAGIC2
	PRINTX
	PUTCHAR('\n')
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
