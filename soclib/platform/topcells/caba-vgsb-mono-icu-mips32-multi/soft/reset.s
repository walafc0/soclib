#################################################################################
#	File : reset.s
#	Author : Alain Greiner
#	Date : 15/09/2009
#################################################################################
# 	This is an improved boot code : 
#	- It initializes the Status Register (SR) 
#	- It defines the stack size  and initializes the stack pointer ($29) 
#	- It initializes the interrupt vector with three ISR addresses
#	- It initializes the EPC register, and jumps to the main in user mode.
#################################################################################
		
	.section .reset,"ax",@progbits

	.extern	seg_stack_base
	.extern	seg_data_base
	.extern	main
	.extern _isr_dma
	.extern _isr_tty_get
	.extern _isr_timer0

	.globl  reset	 			# makes reset an external symbol 
	.ent	reset
	.align	2

reset:
       	.set noreorder

# initializes stack pointer
	la	$29,	seg_stack_base
	li	$26,	0x00100000
	addu	$29,	$29,	$26

# initializes interrupt vector
        la      $26,    _interrupt_vector 
        la      $27,    _isr_timer 
        sw      $27,    0($26)                  # interrupt_vector[0] <= _isr_timer
        la      $27,    _isr_tty_get  
        sw      $27,    4($26)                  # interrupt_vector[1] <= _isr_tty_get

# initializes SR register
       	li	$26,	0x0000FF13		# IRQ activation
       	mtc0	$26,	$12			

# jump to main in user mode
	la	$26,	main
	mtc0	$26,	$14
	eret

	.end	reset
	.size	reset, .-reset

	.set reorder
