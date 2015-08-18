/*
 * Basic MMU checks: we can read mapped addresses, we can't read unmapped
 * addresses
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

	PRINT(startstr)

	la	a0, pte1
	srl	a0, a0, 13
	mtc2	a0, VC_PTPR
	nop

	li	a0, VC_TLB_EN_ITLB | VC_TLB_EN_DTLB | VC_TLB_EN_ICACHE | VC_TLB_EN_DCACHE
	mtc2 	a0, VC_TLB_EN

	PRINT(mmustr)

	/* we can read our magic number at 0x00200000 */
	la	s0, 0x00200000
	move	a0, s0
	PRINTX
	PUTCHAR(':')
	PUTCHAR(' ')
	lw	a0, 0(s0)
	PRINTX
	PUTCHAR('\n')
	/* but not at 0x00000000 */
	la	s0, 0x00000000
	move	a0, s0
	PRINTX
	PUTCHAR(':')
	PUTCHAR(' ')
	lw	a0, 0(s0) /* this should trigger the exception */
	PRINTX

	/* we shouldn't get there */
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

	li	a0, '\n'
	sb	a0, 0(k0)
	EXIT(0)

	.rodata:
statusstr: .ascii "status \0"
causestr: .ascii " cause \0"
pcstr: .ascii " pc \0"
badvastr: .ascii " badva \0"
mmustr:	.ascii "mmu started\n\0"
startstr: .ascii "start\n\0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl evect
evect:
	j	excep
	nop
	.data
testval:
	.word MAGIC1
	.align 13
	.globl pte1
pte1:
	.word 0x0                            /* map PA 0 at VA 0 */
	.word PTE1_V | PTE1_C | PTE1_W | 0x0 /* map PA 0 at VA 0x00200000 */
	.org pte1 + (BOOT_ADDRESS >> 21) * 4
	.word PTE1_V | PTE1_C | PTE1_X | (BOOT_ADDRESS >> 21) /* map PA 0xbfc00000 at VA 0xbfc00000 */
	.org pte1 + (TTY_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (TTY_BASE >> 21) /* map PA 0xd0200000 at VA 0xd0200000 */
	.org pte1 + (EXIT_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (EXIT_BASE >> 21) /* map PA 0xe0000000 at VA 0xe0000000 */
	.org pte1 + 8192
	.word 0

