/*
 * syscall test: check that a syscall generates an exception, and we can
 * return 
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
	/* switch to user mode */
	mfc0	a0, COP0_STATUS
	ori	a0, 0x10
	mtc0	a0, COP0_STATUS
	nop
	nop
	nop
	PRINT(usrstr)
	syscall
	bne a3, zero, exit
	nop
	/* we should not get there, a3 is not 0 */
	EXIT(2)
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
	mfc0	a0, COP0_EXPC
	addiu	a0, a0, 4
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
