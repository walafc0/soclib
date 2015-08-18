/*
 * Basic sanity checks:
 *   we can run a program
 *   we can write to ram
 *   we can write to the tty
 *   we can make the simulator exit
 */
#include <registers.h>
#include <misc.h>

	.text
	.globl  _start
_start:
	.set noreorder
	la	k0, TTY_BASE
	la	k1, EXIT_BASE

	la	t0, value
	lw	t0, 0(t0)
	la	t1, myvar
	sw	t0, 0(t1)

	la	a0, string
	jal	print
	nop

	la	t1, myvar
	lw	a0, 0(t1)
	jal	printx
	nop

	li	a0, '\n'
	sb	a0, 0(k0)
	EXIT(0)

	.rodata:
value:	.word 0x1234abcd
string: .ascii "hello world \0"

	.data
myvar:	.word 0
