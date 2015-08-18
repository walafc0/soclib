/*
 * check that a sc without ll does fail
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

	/* make sure myvar is in cache */
	la	s1, myvar 
	lw	a0, 0(s1)
	PRINTX
	PUTCHAR(' ')

	/* now write to myvar with sc, without ll first */
	la	s1, myvar 
1:
	li	a1, MAGIC2
	sc	a1, 0(s1)
	beqz	a1, 2f
	nop
	
	move a0, a1
	PRINTX
	/* we should not get there */
	EXIT(1)

2:
	/* we jump here when the sc failed */
	move a0, a1
	PRINTX
	PUTCHAR('\n');
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
pte1:
	.align 13
	.word PTE1_V | PTE1_W | 0x0 /* map PA 0 at VA 0 */
	.org pte1 + (BOOT_ADDRESS >> 21) * 4
	.word PTE1_V | PTE1_C | PTE1_X | (BOOT_ADDRESS >> 21) /* map PA 0xbfc00000 at VA 0xbfc00000 */
	.org pte1 + (TTY_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (TTY_BASE >> 21) /* map PA 0xd0200000 at VA 0xd0200000 */
	.org pte1 + (EXIT_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (EXIT_BASE >> 21) /* map PA 0xe0000000 at VA 0xe0000000 */
	.org pte1 + 8192
	.word 0

