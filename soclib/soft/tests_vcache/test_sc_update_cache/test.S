/*
 * Check that lc also updates the local L1 cache.
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

	/* enable caches */
	li	a0, VC_TLB_EN_ICACHE | VC_TLB_EN_DCACHE
	mtc2 	a0, VC_TLB_EN

	/* fetch data in cache */
	la	s0, myvar
	lw	a0, 0(s0)
	/* write magic value */
	li	a0, MAGIC1
	sw	a0, 0(s0)
	/* ll/sc another value on the same address */
2:
	ll	a0, 0(s0)
	PRINTX
	PUTCHAR(' ')
	li	a0, MAGIC2
	sc	a0, 0(s0)
	beqz	a0, 2b
	/* now check that we got the right value in cache */
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
myvar:	.word 0
