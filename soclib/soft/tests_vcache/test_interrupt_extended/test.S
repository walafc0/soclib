/*
 * Check interrupt/return with interrup occuring at various points in
 * the software. Cache is enabled to we have on interrupt/cycle
 */
#include <registers.h>
#include <misc.h>
#include <vcache.h>
#include <xicu.h>

	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE

	/* reset cause, make sure IV is off */
	mtc0	zero, COP0_CAUSE

	la	a0, pte1
	srl	a0, a0, 13
	mtc2	a0, VC_PTPR
	nop
	nop

	li	a0, VC_TLB_EN_ITLB | VC_TLB_EN_DTLB | VC_TLB_EN_ICACHE | VC_TLB_EN_DCACHE
	mtc2 	a0, VC_TLB_EN

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
	 * interrupt in INTERRUPT_DELAY cycles.
	 */
	li	a0, INTERRUPT_DELAY
	sw	a0, XICU_PTI_PER(0)(t0)
	/* clear pending interrupt */
	lw	a0, XICU_PTI_ACK(0)(t0)
	/* reset cop0 status (keep BEV), enable interrupt 0 */
	lui     a0, 0x0040;
	addiu	a0, 0x0401;
	mtc0    a0, COP0_STATUS

	li      s0, MAGIC1
	addiu	s0, s0, -1
	addiu	s0, s0, -1
	addiu	s0, s0, -1
	addiu	s0, s0, -1
	nop
	j	end
	addiu	s0, s0, 5
	nop
	/* we should not end there */
	EXIT(1)

end:
	/* disable interrupts */
	mfc0    a0, COP0_STATUS
	xori	a0, a0, 1
	mtc0	a0, COP0_STATUS

	move	a0, s0
	PRINTX
	PUTCHAR('\n')
	EXIT(0)

	.globl excep
excep:
	.set noreorder
	/* save a0 */
	move s1, a0
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
	/* restore a0 */
	move a0, s1
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
pte1:
	.align 13
	.word PTE1_V | PTE1_C | PTE1_W | 0x0 /* map PA 0 at VA 0 */
	.org pte1 + (BOOT_ADDRESS >> 21) * 4
	.word PTE1_V | PTE1_C | PTE1_X | (BOOT_ADDRESS >> 21) /* map PA 0xbfc00000 at VA 0xbfc00000 */
	.org pte1 + (TTY_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (TTY_BASE >> 21) /* map PA 0xd0200000 at VA 0xd0200000 */
	.org pte1 + (XICU_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (XICU_BASE >> 21) /* map PA 0xd2000000 at VA 0xd2000000 */
	.org pte1 + (EXIT_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (EXIT_BASE >> 21) /* map PA 0xe0000000 at VA 0xe0000000 */
	.org pte1 + 8192
	.word 0

