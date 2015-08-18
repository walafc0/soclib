#################################################################################
#	File : reset.s
#	Author : Alain Greiner
#	Date : 28/12/2010
#################################################################################
# 	This is a boot code for a generic multi-clusters architecture
#       (up to 128 clusters / 1 processor per cluster). 
#       There is one ICU, one TTY, one DMA and one stack segment per cluster.
#       The segment base adresses are : base + proc_id*segment_increment
#	- It initializes the Interrupt vector (TTY, DMA & IOC)
#	- It initializes the Status Register (SR) 
#	- It defines the stack size  and initializes the stack pointer ($29) 
#       - It initialises the ICU components
#	- It initializes the EPC register, and jump to the main address
#################################################################################
		
	.section .reset,"ax",@progbits

	.extern	seg_stack_base
	.extern	seg_data_base
	.extern	seg_icu_base
	.extern	segment_increment
	.extern _interrupt_vector
	.extern _isr_tty_get
	.extern _isr_dma
	.extern _isr_ioc
	.extern _isr_timer

	.globl  reset	 			# makes reset an external symbol 
	.ent	reset
	.align	2

reset:
       	.set noreorder

# computes proc_id and increment
    mfc0    $26,    $15,    1
    andi    $10,    $26,    0x3FF	# $10 <= proc_id
    la      $26,    segment_increment
    mult    $26,    $10	
    mflo    $11                 	# $11 <= proc_id * increment

# initializes stack pointer depending on the proc_id
    la      $27,    seg_stack_base
    addu    $27,    $27,    $11		# $27 <= seg_stack_base + proc_id * increment
    li      $26,    0x10000		# $26 <= 64K
    addu    $29,    $27,    $26		# $29 <= seg_stack_base + proc_id * increment + 64K

# initializes interrupt vector
    la      $26,    _interrupt_vector   # interrupt vector address
    la      $27,    _isr_tty_get 
    sw      $27,    0($26)              # interrupt_vector[0] <= _isr_tty_get
    la      $27,    _isr_dma 
    sw      $27,    4($26)              # interrupt_vector[1] <= _isr_dma
    la      $27,    _isr_ioc 
    sw      $27,    8($26)              # interrupt_vector[2] <= _isr_ioc

# initializes ICU depending on the proc_id
    la      $27,    seg_icu_base
    addu    $27,    $27,    $11		# $27 <= seg_icu_base + proc_id * increment
    li      $26,    0x7                 # IRQ[0] IRQ[1] IRQ[2]
    sw      $26,    8($27)              # ICU_MASK_SET

# initializes EPC register
    la	    $26,    main
    mtc0    $26,    $14			# EPC <= main

# initializes SR register
    li	    $26,    0x0000FF13		
    mtc0    $26,    $12			# SR <= user mode / IRQ enable (after eret)

# jumps to main 
    eret

    .end	reset

    .set reorder
