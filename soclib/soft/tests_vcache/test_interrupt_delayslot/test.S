/*
 * Check that we can take an interrupt from the xicu timer0
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
	/* init s0 */
	li	s0, MAGIC2
	/*
	 * interrupt in 46 cycles.
	 * Without cache this takes us to the delay slot of the jump.
	 */
	li	a0, 46
	sw	a0, XICU_PTI_PER(0)(t0)
	/* clear pending interrupt */
	lw	a0, XICU_PTI_ACK(0)(t0)
	/* reset cop0 status (keep BEV), enable interrupt 0 */
	lui     a0, 0x0040;
	addiu	a0, 0x0401;
	mtc0    a0, COP0_STATUS

	nop
	nop
	nop
	j	end
	li 	s0, MAGIC1
	nop
	/* we should not end there */
	EXIT(1)

end:
	PRINT(endstr)
	move	a0, s0
	PRINTX
	PUTCHAR('\n')
	EXIT(0)

	.globl excep
excep:
	.set noreorder
	/* disable timer0 */
	la	t0, XICU_BASE
	sw	zero, XICU_PTI_PER(0)(t0)
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
	eret
	/* we should not end there */
	EXIT(1)

	.rodata:
statusstr: .ascii "status \0"
causestr: .ascii " cause \0"
pcstr: .ascii " pc \0"
badvastr: .ascii " badva \0"
xicustr: .ascii " xicu \0"
startstr: .ascii "start\n\0"
endstr: .ascii "end \0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl evect
evect:
	j	excep
	nop
	.data
myvar:	.word 0
