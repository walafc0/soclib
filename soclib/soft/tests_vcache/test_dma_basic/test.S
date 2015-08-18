/*
 * basic DMA check: check that the DMA engine works and and doens't
 * clobbers memory
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

	PRINT(mmustr_a)

	/* reset cop0 status (keep BEV) */
	lui	a0, 0x0040;
	mtc0	a0, COP0_STATUS

	la	t0, testval
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR(' ')
	la	t0, testval2
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR('\n')
	la	t0, testval
	la	t1, testval3
	lw 	a0, 0(t0);
	sw	a0, 0(t1)

	la	a1, DMA_BASE
	la	a0, tstsrc
	sw	a0, DMA_SRC(a1)
	la	a0, tstdst
	sw	a0, DMA_DST(a1)
	la	a0, 0x18
	sw	a0, DMA_LEN(a1) /* start DMA */
loop:
	lw	a0, DMA_LEN(a1)
	bne	a0, zero, loop;
	nop

	PRINT(mmustr_b)
	la	t0, testval
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR(' ')
	la	t0, testval2
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR(' ')
	la	t0, testval3
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR(' ')
	la	t0, tstsrc
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR(' ')
	la	t0, tstdst
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR('\n')

	/* we should get there */
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
	EXIT(3)

	.rodata:
statusstr: .ascii "status \0"
causestr: .ascii " cause \0"
pcstr: .ascii " pc \0"
badvastr: .ascii " badva \0"
mmustr_a: .ascii "before DMA \0"
mmustr_b: .ascii "after DMA \0"
startstr: .ascii "start\n\0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl evect
evect:
	j	excep
	nop

	.data
testval:
	.word MAGIC2
testval2:
	.word MAGIC4
testval3:
	.word MAGIC4
tstdst:
	.word 0
	.org 0x1000
tstsrc:
	.word MAGIC1

