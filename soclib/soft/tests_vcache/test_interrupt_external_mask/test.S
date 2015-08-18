/*
 * Check that we can mask an external interrupt
 */
#include <registers.h>
#include <misc.h>
#include <xicu.h>

	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE

	/* reset cop0 status (keep BEV), enable interrupt 0 */
	lui     a0, 0x0040;
	ori	a0, 0xc801;
	mtc0    a0, COP0_STATUS

	/* reset cause, make sure IV is off */
	mtc0	zero, COP0_CAUSE

	PRINT(startstr)

	/* program xicu */
	la	t0, XICU_BASE
	/* clear pending interrupt */
	lw	a0, XICU_PTI_ACK(0)(t0)
	/* route PTI0 to irq 0 */
	li	a0, 1 << 0
	sw	a0, XICU_MSK_PTI_E(0)(t0)
	/* interrupt in 10 cycles */
	li	a0, 10
	sw	a0, XICU_PTI_PER(0)(t0)
	/* interrupts still disabled at the MIPS level */
	li	a0, MAGIC1
	PRINTX
	PUTCHAR('\n')
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	/* enable interrupt from timer0 */
	mfc0	a0, COP0_STATUS
	ori	a0, 0x0400
	mtc0	a0, COP0_STATUS

	nop
	nop

1:	j 1b
	nop
	/* we should not end there */
	EXIT(1)

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

	PRINT(xicustr)
	la	t0, XICU_BASE
	lw	a0, XICU_PRIO(0)(t0)
	PRINTX

	PUTCHAR('\n')
	/* clear interrupt */
	mfc0	a0, COP0_CAUSE
	andi	t0, a0, 0xff00
	xor	a0, a0, t0
	mtc0	a0, COP0_CAUSE
	/* we should end there */
	EXIT(0)

	.rodata:
statusstr: .ascii "status \0"
causestr: .ascii " cause \0"
pcstr: .ascii " pc \0"
badvastr: .ascii " badva \0"
xicustr: .ascii " xicu \0"
startstr: .ascii "start\n\0"
eretstr: .ascii "eret\n\0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl evect
evect:
	j	excep
	nop
	.data
myvar:	.word 0
