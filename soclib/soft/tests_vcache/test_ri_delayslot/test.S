/*
 * reserved instruction exception test: check that a reserved instruction
 * in a delay slot is properly handled
 */
#include <registers.h>
#include <misc.h>

	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE
	la	sp, 0x00400000 - 16

	PRINT(startstr)

	/* reset cop0 status (keep BEV) */
	lui	a0, 0x0040;
	mtc0	a0, COP0_STATUS
	nop
	nop
	nop
	j	exit
	.word 0x60000000
	nop
	/* we should not get there, */
	EXIT(2)
	.globl exit
exit:
	move	a0, v0
	PRINTX
	PUTCHAR(' ')
	move	a0, v1
	PRINTX
	PUTCHAR('\n')
	/* we should get there */
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
	la	a0, exit
	mtc0	a0, COP0_EXPC
	li	v0, MAGIC1
	li	v1, MAGIC2
	li	a3, 1
	eret
	nop

#we should not end there
	EXIT(1)

	.rodata:
statusstr: .ascii "status \0"
causestr: .ascii " cause \0"
pcstr: .ascii " pc \0"
badvastr: .ascii " badva \0"
startstr: .ascii "start\n\0"
usrstr: .ascii "usrmode\n\0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl evect
evect:
	j	excep
	nop
	.data
	.word MAGIC1
testval:
	.word MAGIC2
