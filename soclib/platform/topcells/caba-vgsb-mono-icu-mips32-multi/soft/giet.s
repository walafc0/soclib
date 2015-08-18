##################################################################################
#	File : giet.s
#	Author : Franck Wajsburt & Alain Greiner
#	Date : 2009/11/18
##################################################################################
#	Interruption/Exception/Trap Handler for MIPS32 processor
#	The base address of the segment containing this code
#	MUST be 0x80000000, in order to have the entry point
#	at address 0x80000180 !!!
#	All messages are printed on the TTY defined by the processor ID.
##################################################################################
#	History : 
#	15/09/2009 : The GIET entry point has been modified to comply with
#		the MIPS32 specification : 0x80000180
#	5/10/2009  : The syscall handler has been modified to comply with the
#		MIPS32 specification : the value stored in EPC register is the
#		syscall instruction address => it must be incremented by 4
#		to obtain the return address.
#	15/10/2009 : The Interrupt handler has been modified to comply with the
#		VCI_ICU specification : The IRQ index is obtained by a read
#		to (icu_base_address + 16).	
#	26/10/2009 : The interrupt handler has been modified to support
#		multi-processors architectures with one ICU per processor.
#		Finally, the mfc0 instruction uses now the select parameter
#		to comply with the MIPS32 specification when accessing the
#		processor_id (mtfc0 $x, $15, 1) 
#	8/11/2009 : The syscall handler has been extended to support 32 values
#		for the syscall index, and to enable interrupts when processing 
#		a system call.
#		Five new syscalls have been introduced to access the frame buffer
#		Three new syscalls have been introduced to access the block device
#		The two syscalls associated to the DMA have been removed.
#	18/11/2009 : The syscall handler has been modified to save the SR in
#		the stack before enabling interrupts.
#	15/03/2010 : replace the itoa_print assembler function by the itoa_hex()
#		function defined in the syscalls.c file.
#	10/04/2010 : modify the interrupt handler to use ithe new ICU component
#		supporting several output IRQs.
#		The active IRQ index is obtained as ICU[32*PROC_ID+16].
##################################################################################

	.section .giet,"ax",@progbits
	.align 2                
	.global _interrupt_vector	# makes interrupt_vector an external symbol

	.extern	seg_icu_base
	.extern	seg_tty_base

	.extern	isr_default

	.extern _procid 
	.extern _proctime
	.extern _tty_write
	.extern _tty_read
	.extern _tty_read_irq
	.extern _timer_write
	.extern _timer_read
	.extern _icu_write
	.extern _icu_read
	.extern _gcd_write 
	.extern _gcd_read 
	.extern _locks_read
	.extern _locks_write
	.extern _exit 
	.extern _fb_sync_write 
	.extern _fb_sync_read 
	.extern _fb_write 
	.extern _fb_read 
	.extern _fb_completed
	.extern _ioc_write 
	.extern _ioc_read 
	.extern _ioc_completed
	.extern _itoa_hex

	.ent  giet             
                                                              
################################################################
#	Cause Table (indexed by the Cause register)
################################################################
tab_causes: 
	.word _int_handler   	# 0000 : external interrupt  
	.word _cause_ukn    	# 0001 : undefined exception  
	.word _cause_ukn    	# 0010 : undefined exception
	.word _cause_ukn    	# 0011 : undefined exception
	.word _cause_adel   	# 0100 : illegal address read exception
	.word _cause_ades   	# 0101 : illegal address write exception
	.word _cause_ibe    	# 0110 : instruction bus error exception
	.word _cause_dbe    	# 0111 : data bus error exception
	.word _sys_handler    	# 1000 : system call
	.word _cause_bp     	# 1001 : breakpoint exception
	.word _cause_ri     	# 1010 : illegal codop exception
	.word _cause_cpu    	# 1011 : illegal coprocessor access
	.word _cause_ovf	# 1100 : arithmetic overflow exception    
	.word _cause_ukn    	# 1101 : undefined exception
	.word _cause_ukn    	# 1110 : undefined exception
	.word _cause_ukn    	# 1111 : undefined exception

	.space 320	

################################################################
#	Entry point (at address 0x80000180)
################################################################
giet:
	mfc0  	$27, 	$13		# Cause Register analysis
	lui    	$26, 	0x8000		# $26 <= tab_causes  
	andi  	$27, 	$27, 	0x3c    
	addu  	$26, 	$26, 	$27     
	lw    	$26, 	($26)        
	jr    	$26            		# Jump indexed by CR 
 
#################################################################
#	System Call Handler
# A system call is handled as a special function call.
# - $2 contains the system call index (< 16).
# - $3 is used to store the syscall address
# - $4, $5, $6, $7 contain the arguments values.
# - The return address (EPC) iand the SR are saved in the stack.
# - Interrupts are enabled before branching to the syscall.
# - All syscalls must return to the syscall handler.
# - $2, $3, $4, $5, $6, $7 as well as $26 & $27 can be modified.
#
# In case of undefined system call, an error message displays 
# the value of EPC on the TTY corresponding to the processor,
# and the user program is killed.
#################################################################
_sys_handler: 
	addiu 	$29, 	$29, 	-8      # stack pointer move
 	mfc0  	$26,  	$12          	# load SR
	sw    	$26, 	0($29) 		# save it in the stack       
	mfc0  	$27, 	$14          	# load EPC
	addiu	$27,	$27,	4	# increment EPC for return address
	sw    	$27, 	4($29) 		# save it in the stack       
 
	andi  	$26,  	$2, 	0x1F    # $26 <= syscall index (i < 32) 
	sll   	$26,  	$26, 	2       # $26 <= index * 4
	la    	$27,  	tab_syscalls	# $27 <= &tab_syscalls[0]
	addu  	$27,  	$27, 	$26     # $27 <= &tab_syscalls[i] 
	lw    	$3,  	0($27)         	# $3  <= syscall address

 	li	$27,	0xFFFFFFED	# Mask for UM & EXL bits
	mfc0	$26,	$12		# $26 <= SR
 	and  	$26,  	$26, 	$27     # UM = 0 / EXL = 0
 	mtc0  	$26,  	$12          	# interrupt enabled
	jalr  	$3                	# jump to the proper syscall
	mtc0	$0,	$12		# interrupt disbled

 	lw    	$26,  	0($29)       	# load SR from stack
 	mtc0  	$26, 	$12         	# restore SR 
	lw	$26,	4($29)        	# load EPC from stack
	mtc0	$26,	$14		# restore EPC
	addiu	$29, 	$29, 	8	# restore stack pointer
	eret   				# exit GIET 

_sys_ukn:				# undefined system call 
	mfc0	$4,	$15, 1		
	andi	$4,	$4,	0x7	# $4 <= processor id
	la    	$5, 	msg_uknsyscall  # $5 <= message address
	li  	$6,	36		# $6 <= message length
	jal   	_tty_write 		# print unknown message

	mfc0	$4,	$15, 1
	andi	$4,	$4,	0x7	# $4 <= processor id
	la    	$5, 	msg_epc         # $5 <= message address
	li  	$6,	8 		# $6 <= message length
	jal   	_tty_write 		# print EPC message

	mfc0  	$4,  	$14 		# $4 <= EPC         
        la	$5,	itoa_buffer 	# $5 <= buffer address
        addiu   $5,	$5,	2	# skip the 0x prefix
	jal	_itoa_hex		# fill the buffer

	mfc0	$4,	$15, 1		
	andi	$4,	$4,	0x7	# $4 <= processor id
	la    	$5, 	itoa_buffer  	# $5 <= buffer address
	li  	$6,	10		# $6 <= buffer length
	jal   	_tty_write 		# print EPC value

	j	_exit 			# end of program

itoa_buffer:  	.ascii "0x00000000"

	.align 2                

#################################################################
# System Call Table (indexed by syscall index)
tab_syscalls: 
	.word _procid			# 0x00 
	.word _proctime			# 0x01 
	.word _tty_write		# 0x02
	.word _tty_read     		# 0x03
	.word _timer_write		# 0x04
	.word _timer_read		# 0x05
	.word _gcd_write		# 0x06
	.word _gcd_read			# 0x07
	.word _icu_write		# 0x08
	.word _icu_read			# 0x09
	.word _tty_read_irq		# 0x0A
	.word _sys_ukn  		# 0x0B
	.word _locks_write		# 0x0C
	.word _locks_read		# 0x0D
	.word _exit 			# 0x0E
	.word _sys_ukn  		# 0x0F
	.word _fb_sync_write		# 0x10
	.word _fb_sync_read		# 0x11
	.word _fb_write 		# 0x12
	.word _fb_read  		# 0x13
	.word _fb_completed  		# 0x14
	.word _ioc_write 		# 0x15
	.word _ioc_read  		# 0x16
	.word _ioc_completed  		# 0x17
	.word _sys_ukn  		# 0x18
	.word _sys_ukn  		# 0x19
	.word _sys_ukn  		# 0x1A
	.word _sys_ukn  		# 0x1B
	.word _sys_ukn  		# 0x1C
	.word _sys_ukn  		# 0x1D
	.word _sys_ukn  		# 0x1E
	.word _sys_ukn  		# 0x1F
  
###################################################################
#	Interrupt Handler
# This simple interrupt handler cannot be interrupted.
# It uses an external ICU component (Interrupt Controler Unit)
# that concentrates up to 32 interrupts lines to a single IRQ
# line that can be connected to any of the 6 MIPS IT inputs.
# The ICU component can route interrupts to 8 processors.
# This component returns the highest priority active interrupt index
# (smaller indexes have the highest priority) taken into account
# only the interrupts allocated to the prper PROC_ID.
# The interrupt handler reads the ICU_IT_VECTOR register,
# using the offset = 32 * PROC_ID + 16.
# This component returns the highest priority interrupt index
# (smaller indexes have the highest priority).
# Any value larger than 31 means "no active interrupt", and
# the default ISR (that does nothing) is executed.
# The interrupt vector (32 ISR addresses array stored at
# _interrupt_vector address) is initialised with the default
# ISR address. The actual ISR addresses are supposed to be written 
# in the interrupt vector array by the boot code.
# Registers $1 to $7, as well as register  $31 and EPC 
# are saved in the interrupted program stack, before calling the 
# Interrupt Service Routine, and can be used by the ISR code.
####################################################################
_int_handler: 
	addiu 	$29,	$29,	-36	# stack space reservation
	.set noat
	sw    	$1,  	0($29)		# save $1     
	.set at
	sw    	$2,  	4($29)		# save $2     
	sw    	$3,  	8($29)		# save $3     
	sw    	$4,  	12($29)		# save $4     
	sw    	$5,  	16($29)		# save $5     
	sw    	$6,  	20($29)		# save $6     
	sw    	$7,  	24($29)		# save $7     
	sw    	$31,  	28($29)		# save $31     
	mfc0	$27,	$14
	sw	$27,	32($29)		# save EPC
	mfc0	$27,	$15,	1	# $27 <= proc_id
	andi	$27,	$27,	0x7	# at most 8 processors
	sll	$27,	$27,	5	# $27 <= proc_id * 32
	la	$26,	seg_icu_base	# $26 <= seg_icu_base 
	add	$26,	$26,	$27	# $26 <= seg_icu_base + 32*proc_id 
	lw	$26,	16($26)		# $26 <= interrupt_index
	srl	$27,	$26,	5
	bne	$27,	$0,	restore # do nothing if index > 31
	sll	$26,	$26,	2	# $26 <= interrupt_index * 4
	la	$27,	_interrupt_vector
	addu	$26,	$26,	$27
	lw    	$26, 	($26)		# read ISR address        
	jalr   	$26            		# call ISR 
restore:
	.set noat
	lw    	$1,  	0($29)    	# restore $1
	.set at
	lw    	$2,  	4($29)    	# restore $2
	lw    	$3,  	8($29)    	# restore $3
	lw    	$4,  	12($29)    	# restore $4
	lw    	$5,  	16($29)    	# restore $5
	lw    	$6,  	20($29)    	# restore $6
	lw    	$7,  	24($29)    	# restore $7
	lw    	$31,  	28($29)    	# restore $31
	lw    	$27,  	32($29)    	# return address (EPC)
	addiu	$29,	$29,	36	# restore stack pointer
	mtc0	$27,	$14		# restore EPC
	eret   				# exit GIET 

# Interrupt Vector Table (indexed by interrupt index)
# 32 words corresponding to 32 ISR addresses 
_interrupt_vector:
	.word isr_default		# ISR 0
	.word isr_default		# ISR 1
	.word isr_default		# ISR 2
	.word isr_default		# ISR 3
	.word isr_default		# ISR 4
	.word isr_default		# ISR 5
	.word isr_default		# ISR 6
	.word isr_default		# ISR 7
	.word isr_default		# ISR 8
	.word isr_default		# ISR 9
	.word isr_default		# ISR 10
	.word isr_default		# ISR 11
	.word isr_default		# ISR 12
	.word isr_default		# ISR 13
	.word isr_default		# ISR 14
	.word isr_default		# ISR 15
	.word isr_default		# ISR 16
	.word isr_default		# ISR 17
	.word isr_default		# ISR 18
	.word isr_default		# ISR 19
	.word isr_default		# ISR 20
	.word isr_default		# ISR 21
	.word isr_default		# ISR 22
	.word isr_default		# ISR 23
	.word isr_default		# ISR 24
	.word isr_default		# ISR 25
	.word isr_default 		# ISR 26
	.word isr_default 		# ISR 27
	.word isr_default 		# ISR 28
	.word isr_default 		# ISR 29
	.word isr_default 		# ISR 30
	.word isr_default 		# ISR 31

# The default ISR is called when no specific ISR has been installed in the
# interrupt vector. It simply displays a message on TTY 0.

isr_default:
        addiu   $29,    $29,    -4      # get space in stack
        sw      $31,    0($29)          # to save the return address
	mfc0	$4,	$15,	1
	andi	$4,	$4,	0x7	# $4 <= processor id
        la      $5,     msg_default 	# $5 <= string address
	addi	$6,	$0,	36	# $6 <= string length
        jal     _tty_write              # print
        lw      $31,    0($29)          # restore return address
        addiu   $29,    $29,    4       # free space
        jr      $31                     # returns to interrupt handler

############################################################  
#	Exception Handler
# Same code for all fatal exceptions :
# Print the exception type and the values of EPC & BAR
# on the TTY correspondintg to the processor PROCID,
# and the user program is killed.
############################################################
_cause_bp:   
_cause_ukn:  
_cause_ri:   
_cause_ovf:  
_cause_adel: 
_cause_ades: 
_cause_ibe:  
_cause_dbe:  
_cause_cpu:  
	mfc0  	$26,  	$13       	# $26 <= CR
	andi  	$26,  	$26, 	0x3C    # $26 <= _cause_index * 4 
	la    	$27, 	mess_causes	# mess_cause table base address
	addu  	$27,  	$26, 	$27	# pointer on the message base address

	mfc0	$4,	$15,	1
	andi	$4,	$4,	0x7	# $4 <= processor id
	lw    	$5,  	($27)        	# $5 <= message address
	li	$6,	36		# $6 <= message length
	jal   	_tty_write 		# print message cause 

	mfc0	$4,	$15,	1
	andi	$4,	$4,	0x7	# $4 <= processor id
	la    	$5, 	msg_epc         # $5 <= message address
	li	$6,	8		# $6 <= message length
	jal   	_tty_write		# print message EPC

	mfc0  	$4,  	$14          	# $4 <= EPC value
	la	$5,	itoa_buffer	# $5 <= buffer address
	addiu	$5,	$5,	2	# skip 0x prefix
	jal   	_itoa_hex		# fill buffer

	mfc0	$4,	$15,	1
	andi	$4,	$4,	0x7	# $4 <= processor id
	la    	$5, 	itoa_buffer     # $5 <= buffer address
	li	$6,	10		# $6 <= buffer length
	jal   	_tty_write		# print EPC value

	mfc0	$4,	$15,	1
	andi	$4,	$4,	0x7	# $4 <= processor id
	la    	$5, 	msg_bar         # $5 <= mesage address
	li	$6,	8		# $6 <= message length
	jal   	_tty_write		# print message BAR

	mfc0  	$4,  	$8          	# $4 <= BAR value
	la	$5,	itoa_buffer	# $5 <= buffer address
	addiu	$5,	$5,	2	# skip 0x prefix
	jal   	_itoa_hex 		# fill buffer

	mfc0	$4,	$15,	1
	andi	$4,	$4,	0x7	# $4 <= processor id
	la    	$5, 	itoa_buffer     # $5 <= mesage address
	li	$6,	10		# $6 <= message length
	jal   	_tty_write		# print BAR value

	j     	_exit			# end program       

# Exceptions Messages table (indexed by CAUSE)  
mess_causes:   
	.word msg_ukncause
	.word msg_ukncause
	.word msg_ukncause
	.word msg_ukncause
	.word msg_adel
	.word msg_ades
	.word msg_ibe
	.word msg_dbe
	.word msg_ukncause
	.word msg_bp 
	.word msg_ri 
	.word msg_cpu
	.word msg_ovf 
	.word msg_ukncause 
	.word msg_ukncause
	.word msg_ukncause

##################################################################
#	All messages
# Messages length are fixed : 8 or 36 characters...
##################################################################  
msg_bar:       	.asciiz "\nBAR  = "  
msg_epc:       	.asciiz "\nEPC  = "  
msg_default: 	.asciiz "\n\n  !!! Default ISR  !!!           \n"
msg_uknsyscall: .asciiz "\n\n  !!! Undefined System Call !!!  \n"     
msg_ukncause:  	.asciiz "\n\nException : strange unknown cause\n"   
msg_adel:      	.asciiz "\n\nException : illegal read address \n"
msg_ades:      	.asciiz "\n\nException : illegal write address\n"
msg_ibe:       	.asciiz "\n\nException : inst bus error       \n"         
msg_dbe:       	.asciiz "\n\nException : data bus error       \n" 
msg_bp:        	.asciiz "\n\nException : breakpoint           \n"        
msg_ri:        	.asciiz "\n\nException : reserved instruction \n"      
msg_ovf:       	.asciiz "\n\nException : arithmetic overflow  \n"       
msg_cpu:       	.asciiz "\n\nException : illegal coproc access\n"       
		.align 2                
	.end giet

