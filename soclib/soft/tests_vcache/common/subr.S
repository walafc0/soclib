#include <registers.h>
/* test common subroutines

	.text
/*
 * void print(char *): print a string to the multitty.
 * assumes k0 points to the tty base.
 */
	.globl print
print:
	.set noreorder
	move	t0, a0
1:
	lb	t1, 0(t0);
	beq	t1, zero, end
	nop
	sb	t1, 0(k0)
	addiu	t0, t0, 1
	j	1b
	nop
end:
	jr	ra
	nop

/*
 * void printx(uint32_t): print a number in hex to the multitty
 * assumes points to the tty base.
 */
	.globl printx
printx:
	.set noreorder
	move	t0, a0
	li	t1, '0'
	sb	t1, 0(k0)
	li	t1, 'x'
	sb	t1, 0(k0)
	li	t1, 8
1:
	lui	t2, 0xf000;
	and	t2, t0, t2
	srl	t2, t2, 28
	slti	t3, t2, 0xa
	beq	t3, zero, 2f /* in range A-F */
	nop
	addi	t2, t2, '0'
	j	3f
	nop
2:
	addi    t2, t2, 'A' - 0xa
3:
	sb      t2, 0(k0)
	addi	t1, t1, -1
	bgtz	t1, 1b
	sll	t0, t0, 4
	jr	ra
	nop
