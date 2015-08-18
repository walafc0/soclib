/*
 * trap test: check that a we can generate a trap from FPE instruction
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

	/* reset cop0 status (keep BEV), enable cop1 */
	lui	a0, 0x2040;
	mtc0	a0, COP0_STATUS
	/* enable COP1 exceptions */
	li	a0, 0x0f80
	ctc1	a0, $31
#if 0
	li	a0, 0xf000
	ctc1	a0, $26
#endif
	la	a0, div1
	lwc1	$f2, 0(a0)
	la	a0, div2
	lwc1	$f0, 0(a0)
	div.s	$f0, $f2, $f0
	cfc1	a0, $31
	PRINTX
	PUTCHAR('\n')
	EXIT(2)

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
#we should end there
	EXIT(0)

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
	.globl div1
div1:
	.float	10.0
	.globl div2
div2:
	.float	0.0
