#################################################################################
#	File : reset.s
#	Author : Alain Greiner
#	Date : 28/12/2010
#################################################################################
# 	This is an improved boot code for a clusterized architecture
#       (up to 8 clusters / 1 processor per cluster). 
#       There is one ICU and one stack per cluster.
#       The isegment base adresses are computed as base * proc_id*0x10000000
#	- It initializes the Interrupt vector
#	- It initializes the Status Register (SR) 
#	- It defines the stack size  and initializes the stack pointer ($29) 
#       - It initialises the ICU[i] components
#	- It initializes the EPC register, and jump to the main[i] address
#################################################################################
		
	.section .reset,"ax",@progbits

	.extern	seg_stack_base
	.extern	seg_data_base
	.extern	seg_icu_base
	.extern _interrupt_vector
	.extern _isr_timer
	.extern _isr_tty_get_task0
	.extern _isr_dma
	.extern _isr_ioc

	.globl  reset	 			# makes reset an external symbol 
	.ent	reset
	.align	2

reset:
       	.set noreorder

# initializes stack pointer depending on the proc_id
    la      $27,    seg_stack_base
    mfc0    $26,    $15,    1
    andi    $10,    $26,    0x7		# $10 <= proc_id
    sll     $11,    $26,    28		# $11 <= proc_id * 0x10000000
    addu    $27,    $27,    $11		# $27 <= seg_stack_base + proc_id*0x10000000 
    li	    $26,    0x10000		# $26 <= 64K
    addu    $29,    $27, $26		# $29 <= seg_stack_base + proc_id*0x10000000 + 64K

# initializes interrupt vector
    la      $26,    _interrupt_vector   # interrupt vector address
    la      $27,    _isr_timer          # isr_timer address
    sw      $27,    0($26)              # interrupt_vector[0] <= _isr_timer
    la      $27,    _isr_tty_get_task0  # isr_tty_get_task0 address
    sw      $27,    4($26)              # interrupt_vector[1] <= _isr_tty_get_task0
    la      $27,    _isr_ioc            # isr_ioc address
    sw      $27,    8($26)              # interrupt_vector[2] <= _isr_ioc
    la      $27,    _isr_dma            # isr_dma address
    sw      $27,   12($26)              # interrupt_vector[3] <= _isr_dma

# initializes ICU depending on the proc_id
    la      $27,    seg_icu_base
    addu    $27,    $27,    $11		# $27 <= seg_icu_base + proc_id*0x10000000
    li      $26,    0xF                 # IRQ[0] IRQ[1] IRQ[2] IRQ[3]
    sw      $26,    8($27)              # ICU_MASK_SET

# initializes EPC register depending on the proc_id  
    la	    $26,    tab_main
    sll     $27,    $10,    2		# $27 <= proc_id*4
    addu    $26,    $26,    $27		# $26 <= &tab_main[proc_id]
    lw      $27,    0($26)		# $27 <= tab_main[proc_id]
    mtc0    $27,    $14			# EPC <= tab_main[proc_id]

    la 	    $28, _gp

# initializes SR register
    li	    $26,    0x0000FF13		
    mtc0    $26,    $12			# SR <= user mode / IRQ enable (after eret)

# jumps to main 
    eret

tab_main:
    .word	load
    .word	transpose
    .word	display
    .word	exit
    .word	exit
    .word	exit
    .word	exit
    .word	exit

    .end	reset

    .set reorder
