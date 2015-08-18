/*
 * Check that the MMU doens't clobber the OS bits in PTEs, and interraction
 * with ll/sc
 */
#include <registers.h>
#include <misc.h>
#include <vcache.h>

	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE

	/* reset cop0 status (keep BEV) */
	lui	a0, 0x0040;
	mtc0	a0, COP0_STATUS

	la	a0, pte1
	srl	a0, a0, 13
	mtc2	a0, VC_PTPR
	nop

	li	a0, VC_TLB_EN_ITLB | VC_TLB_EN_DTLB | VC_TLB_EN_ICACHE | VC_TLB_EN_DCACHE
	mtc2 	a0, VC_TLB_EN

	/* do a swap of pte2[0] to make the entry valid */
	la	s0, pte2 + 0x00200000
1:	la	a1, PTE2_V | PTE2_C | PTE2_W | PTE2_os
	ll	a0, 0(s0)
	sc	a1, 0(s0)
	beqz	a1, 1b
	nop
	PRINTX
	PUTCHAR(' ')
	/* now write to myvar via pte2[0] */
	li	a0, MAGIC2
	la	s1, myvar + 0x00200000
	sw	a0, 0(s1)
	/*
	 * and invalidate pte2[0], again using a swap
	 * check that we get the same value with lw and ll
	 */
	lw	s1, 0(s0)
2:
	move	a1, zero
	ll	a0, 0(s0)
	sc	a1, 0(s0)
	beqz	a1, 2b
	PRINTX
	PUTCHAR(' ')
	move	a0, s1
	PRINTX
	PUTCHAR(' ')
	/* finally check what lw tells us now */
	lw	a0, 0(s0)
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
	/* we should not get there */
	EXIT(1)

	.rodata:
statusstr: .ascii "status \0"
causestr: .ascii " cause \0"
pcstr: .ascii " pc \0"
badvastr: .ascii " badva \0"
value:	.word 0x1234abcd
string: .ascii "hello world \0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl evect
evect:
	j	excep
	nop

	.data
myvar:	.word MAGIC1
	.globl pte1
pte2:
	.align 12
	.word 0	/* invalid for now */
	.word 0x0000 >> 12 /* check real value of myvar */
	.word PTE2_V | PTE2_C | PTE2_W | PTE2_os /* map pte2 too */
	.word 0x1000 >> 12 /* check real address of pte2 */
	.org pte2 + 4092
	.word 0
	.globl pte1
pte1:
	.align 13
	.word PTE1_V | PTE1_C | PTE1_W | 0x0 /* map PA 0 at VA 0 */
	.word PTE1_V | PTE1_T | (0x1000 >> 12) /* map PA 0x0 at VA 0x00200000 via pte2 */
	.org pte1 + (BOOT_ADDRESS >> 21) * 4
	.word PTE1_V | PTE1_C | PTE1_X | (BOOT_ADDRESS >> 21) /* map PA 0xbfc00000 at VA 0xbfc00000 */
	.org pte1 + (TTY_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (TTY_BASE >> 21) /* map PA 0xd0200000 at VA 0xd0200000 */
	.org pte1 + (EXIT_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (EXIT_BASE >> 21) /* map PA 0xe0000000 at VA 0xe0000000 */
	.org pte1 + 8192
	.word 0

