/*
 * Check interrupt/return with interrup occuring at various points in
 * the software. Cache is enabled to we have on interrupt/cycle
 * expecially, check what happen when an interrupt occurs while
 * the CPU is waiting on the cache to return a bus error.
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
	la	t0, XICU_BASE
	li	a0, INTERRUPT_DELAY
	sw	a0, XICU_PTI_PER(0)(t0)
	/* clear pending interrupt */
	lw	a0, XICU_PTI_ACK(0)(t0)
	/* reset cop0 status (keep BEV), enable interrupt 0 */
	lui     a0, 0x0040;
	addiu	a0, 0x0401;
	mtc0    a0, COP0_STATUS
	nop
	nop

	/* 0x00000000 not mapped yet: we fault, load the MMU and retry */
	la	s0, testval
	lw	s1, 0(s0) /* this should trigger the exception */
	nop
	nop

	/* disable interrupts */
	mfc0    s3, COP0_STATUS
	xori	s3, s3, 1
	mtc0	s3, COP0_STATUS

	move	a0, s0
	PRINTX
	PUTCHAR(':')
	PUTCHAR(' ')
	move	a0, s1
	PRINTX
	PUTCHAR('\n')
	EXIT(0)
	.align 6
	nop
	PRINT(endstr)
	move	a0, s0
	PRINTX
	PUTCHAR(' ')
	move	a0, s1
	PRINTX
	PUTCHAR('\n')
	EXIT(0)
fail:
	EXIT(4)


	.rodata:
statusstr: .ascii "status \0"
causestr: .ascii " cause \0"
pcstr: .ascii " pc \0"
badvastr: .ascii " badva \0"
xicustr: .ascii " xicu \0"
startstr: .ascii "start\n\0"
endstr: .ascii "end \0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl excep
excep:
	.set noreorder
	move	s2, ra
	mfc0	a0, COP0_CAUSE
	andi	a0, a0, 0x7c
	beq	a0, zero, intr
	nop
	/* this is a bus fault */
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
	/* map VA 0 at PA 0: just copy entry for 0x00200000 */
	la	t0, pte1 + 0x00200000
	lw	a0, 4(t0)
	sw	a0, 0(t0)
	move	ra, s2
	eret

intr:
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
	move	ra, s2
	eret
	/* we should not end there */
	EXIT(3)
	.data
myvar:	.word 0
testval:
	.word MAGIC2
pte1:
	.align 13
	.word 0x0                            /* map PA 0 at VA 0 */
	.word PTE1_V | PTE1_C | PTE1_W | 0x0 /* map PA 0 at VA 0x00200000 */
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

