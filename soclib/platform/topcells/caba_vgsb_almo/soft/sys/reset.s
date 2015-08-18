#################################################################################
#	File : reset.s
#	Author : Alain Greiner
#	Date : 15/09/2010
#################################################################################
# 	This Rboot code is for a single processor / siggle task application
#	- It initializes the Status Register (SR) 
#	- It defines the stack size  and initializes the stack pointer ($29) 
#	- It initializes the interrupt vector with four ISR addresses:
#              IRQ_IN[0] <= IOC
#              IRQ_IN[1] <= TIMER
#              IRQ_IN[2] <= DMA
#              IRQ_IN[3] <= TTY
#       - It configurates the ICU.
#	- It initializes the EPC register, and jumps to user code in user mode.
#################################################################################
		
	.section .reset,"ax",@progbits

        .func   reset
	.type	reset, %function

	.extern	seg_stack_base
	.extern	seg_data_base
	.extern	seg_icu_base
	.extern	_interrupt_vector
	.extern _isr_tty_get
	.extern _isr_timer0

	.align	2

reset:
       	.set noreorder

# initializes stack pointer
	la	$29,	seg_stack_base
	addiu	$29,	$29,	0x4000	

# initializes interrupt vector
        la      $26,    _interrupt_vector       # interrupt vector address
        la      $27,    _isr_ioc
        sw      $27,    0($26)                  # interrupt_vector[0] <= _isr_ioc
        la      $27,    _isr_timer
        sw      $27,    4($26)                  # interrupt_vector[1] <= _isr_timer
        la      $27,    _isr_dma
        sw      $27,    8($26)                  # interrupt_vector[2] <= _isr_dma
        la      $27,    _isr_tty_get 
        sw      $27,    12($26)                 # interrupt_vector[3] <= _isr_tty_get

# initializes ICU
	la	$26,	seg_icu_base
	li	$27,	0xF			# IRQ_IN[0:3] enabled
	sw	$27,	8($26)			# ICU_MASK_SET 0xF

# initializes SR register
       	li	$26,	0x0000FF13		# IRQ activation
       	mtc0	$26,	$12			

# jump to user's code in user mode
	la	$26,	seg_data_base
        lw      $26,    0($26)			# get the user code entry point
	mtc0	$26,	$14
	eret

	.set reorder

	.endfunc
	.size	reset, .-reset
