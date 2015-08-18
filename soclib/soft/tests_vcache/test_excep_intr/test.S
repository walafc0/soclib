/*
 * Check interrupt while in trap handler.
 * Cache is enabled so we have one instruction/cycle
 */
#include <registers.h>
#include <misc.h>
#include <vcache.h>
#include <xicu.h>

#define DOLOAD_ADDR 0x80000000

	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE
	la	sp, 0x00200000 - 16

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
	nop
	nop
	nop
	nop
	nop

	nop
	move	a1, zero
	la	s0, DOLOAD_ADDR
	jalr	s0
	nop
	j	end
	nop
	/* we should not end there */
	EXIT(1)

end:
	PRINT(endstr)
	move	a0, a1
	PRINTX
	PUTCHAR(' ')
	la	a0, myvar
	lw	a0, 0(a0)
	PRINTX
	PUTCHAR('\n')
	EXIT(0)

	.globl excep
excep:
	.set noreorder
        /* don't clobber a0 and ra */
	addiu   sp, sp, -8
	sw      a0, 4(sp)
	sw      ra, 8(sp)

	PRINT(statusstr)
	mfc0	a0, COP0_STATUS
	PRINTX

	PRINT(causestr)
	mfc0	a0, COP0_CAUSE
	move	s0, a0
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
	/* map VA DOLOAD_ADDR at PA 0xbfc01000 */
	la	t0, pte22 + VADDR_TO_PTE2I(DOLOAD_ADDR) * 8
	la	a0, (0xbfc01000 >> PTE2_SHIFT)
	sw	a0, 4(t0)
	la	a0, (PTE2_V | PTE2_C | PTE2_X)
	sw	a0, 0(t0)
	mtc2	zero, VC_DCACHE_FLUSH /* flush the whole data cache */

	andi	a0, s0, 0xff00
	beq	a0, zero, notintr
	nop

	/* disable timer0 */
	la	t0, XICU_BASE
	sw	zero, XICU_PTI_PER(0)(t0)
notintr:
	la	a1, myvar
	la	a0, 0(a1)
	beq	a0, zero, noex
	nop
	EXIT(2)
noex:
	li	a0, 1
	sw	a0, 0(a1)
	lw	a0, 4(sp)
	lw	ra, 8(sp)
	addiu	sp, sp, 8
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
	/*
	 * we use a ldscript trick here, to load this function at
	 * the appropriate address
	 */
	.section .text2, "ax"
	.globl doload
doload:
	la	s0, testval
	jr	ra
	lw	a1, 0(s0) /* this should trigger the exception */
	/* we should not get there */
	EXIT(1)
	nop
	.data
myvar:	.word 0
testval:
	.word MAGIC2
	.globl pte2
pte2:
        .align 12
        .word PTE2_V | PTE2_C | PTE2_X
        .word BOOT_ADDRESS >> 12
        .org pte2 + 4092
	.globl pte22
pte22:
        .align 12
        .word 0
        .word 0
        .org pte22 + 4092
        .globl pte1
pte1:
	.align 13
	.word PTE1_V | PTE1_C | PTE1_W | 0x0 /* map PA 0 at VA 0 */
	.org pte1 + (DOLOAD_ADDR >> 21) * 4
        .word PTE1_V | PTE1_T | (0x2000 >> 12) /* map VA DOLOAD_ADDR with 4k page: check real address of PTE22 !!! */
	.org pte1 + (BOOT_ADDRESS >> 21) * 4
        .word PTE1_V | PTE1_T | (0x1000 >> 12) /* map PA 0xbfc00000 at VA 0xbfc00000 with 4k page: check real address of PTE2 !!! */
	.org pte1 + (TTY_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (TTY_BASE >> 21) /* map PA 0xd0200000 at VA 0xd0200000 */
	.org pte1 + (XICU_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (XICU_BASE >> 21) /* map PA 0xd2000000 at VA 0xd2000000 */
	.org pte1 + (EXIT_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (EXIT_BASE >> 21) /* map PA 0xe0000000 at VA 0xe0000000 */
	.org pte1 + 8192
	.word 0

