/*
 * cache inval test: check that VC_DCACHE_INVAL_PA does the job
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

	/* reset cop0 status (keep BEV) */
	lui	a0, 0x0040;
	mtc0	a0, COP0_STATUS

	la	a0, pte1_a
	srl	a0, a0, 13
	mtc2	a0, VC_PTPR
	nop

	li	a0, VC_TLB_EN_ITLB | VC_TLB_EN_DTLB | VC_TLB_EN_ICACHE | VC_TLB_EN_DCACHE
	mtc2 	a0, VC_TLB_EN

	PRINT(mmustr_a)
	la	t0, testval
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR('\n')
	la	a1, DMA_BASE
	la	a0, testval2
	sw	a0, DMA_SRC(a1)
	la	a0, testval
	sw	a0, DMA_DST(a1)
	li	a0, 4
	sw	a0, DMA_LEN(a1) /* start DMA */
loop:
	lw	a0, DMA_LEN(a1)
	bne	a0, zero, loop;
	nop

	PRINT(mmustr_b)
	la	t0, testval
	lw 	a0, 0(t0);
	PRINTX
	PUTCHAR('\n')

	la      a0, testval
	mtc2	a0, VC_DATA_LO
	mtc2	zero, VC_DATA_HI
	mtc2	zero, VC_DCACHE_INVAL_PA
	nop

	PRINT(mmustr_c)
	la	t0, testval
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
mmustr_a: .ascii "mmu started before DMA \0"
mmustr_b: .ascii "mmu started after DMA \0"
mmustr_c: .ascii "mmu started after FLUSH \0"
startstr: .ascii "start\n\0"

	.org EXCEP_ADDRESS - BOOT_ADDRESS
	.globl evect
evect:
	j	excep
	nop

	.data
data_a:
	.word MAGIC1
testval:
	.word MAGIC2
data_b:
	.word MAGIC3
testval2:
	.word MAGIC4
	.globl pte2_a

pte2_a:
	.align 12
	.word PTE2_V | PTE2_C | PTE2_X
	.word 0x0000 >> 12 /* check real value of data_a */
	.org pte2_a + 4092
	.word 0
	.globl pte1_a
pte1_a:
	.align 13
	.word PTE1_V | PTE1_C | PTE1_W | 0x0 /* map PA 0 at VA 0 */
	.word PTE1_V | PTE1_T | (0x2000 >> 12) /* map PA 0x0 at VA 0x00200000 via pte2_a */
	.org pte1_a + (BOOT_ADDRESS >> 21) * 4
	.word PTE1_V | PTE1_C | PTE1_X | (BOOT_ADDRESS >> 21) /* map PA 0xbfc00000 at VA 0xbfc00000 */
	.org pte1_a + (TTY_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (TTY_BASE >> 21) /* map PA 0xd0200000 at VA 0xd0200000 */
	.org pte1_a + (EXIT_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (EXIT_BASE >> 21) /* map PA 0xe0000000 at VA 0xe0000000 */
	.org pte1_a + (DMA_BASE >> 21) * 4
	.word PTE1_V | PTE1_W | (DMA_BASE >> 21) /* map PA 0xe8000000 at VA 0xe0000000 */
