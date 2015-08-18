#####################################################################################
#	file swich.s
# Author : Franck Wajsburt & Alain Greiner
#   Date : 23/11/2009
#####################################################################################
#   The _ctx_switch function performs a context switch between the
#   _current task and another task.
#   It can be used in a multi-processor architecture, with the assuption
#   that the tasks are statically allocated to processors.
#   The max number of processorsi is 4, and the max number of tasks is 4.
#   The scheduling policy is very simple : For each processor, the task index 
#   is incremented,  modulo the number of tasks allocated to the processor.
# 
#   It has no argument, and no return value. 
#   It uses three global variables:
#	- _current_task_array :  an array of 4 task index:
#	  (the task actually running on each processor)
#	- _task_number_array : an array of 4 numbers:
#	  (the number of tasks allocated to each processor)
#       - _task_context_array : an array of 16 task contexts:
#	  (at most 4 processors / each processor can run up to 4 tasks)
#   A task context is an array of 64 words = 256 bytes.
#   It is indexed by m = (proc_id*4 + task_id)
#   It contains copies of the processor registers.
#   As much as possible a register is stored at the index defined by its number 
#   ( for example, $8 is saved in ctx[8]).  
#   The exception are :
#   $0 is not saved since always 0
#   $26, $27 are not saved since not used by the task
#  
#   0*4(ctx) SR    8*4(ctx) $8    16*4(ctx) $16   24*4(ctx) $24   32*4(ctx) EPC
#   1*4(ctx) $1    9*4(ctx) $9    17*4(ctx) $17   25*4(ctx) $25   33*4(ctx) CR      
#   2*4(ctx) $2   10*4(ctx) $10   18*4(ctx) $18   26*4(ctx) LO    34*4(ctx) reserved
#   3*4(ctx) $3   11*4(ctx) $11   19*4(ctx) $19   27*4(ctx) HI    35*4(ctx) reserved
#   4*4(ctx) $4   12*4(ctx) $12   20*4(ctx) $20   28*4(ctx) $28   36*4(ctx) reserved
#   5*4(ctx) $5   13*4(ctx) $13   21*4(ctx) $21   29*4(ctx) $29   37*4(ctx) reserved
#   6*4(ctx) $6   14*4(ctx) $14   22*4(ctx) $22   30*4(ctx) $30   38*4(ctx) reserved
#   7*4(ctx) $7   15*4(ctx) $15   23*4(ctx) $23   31*4(ctx) $31   39*4(ctx) reserved 
#
#   The return address contained in $31 is saved in the _current task context
#   (in the ctx[31] slot), and the function actually returns to the address contained i
#   in the ctx[31] slot of the new task context.
#
#   Caution : This function is intended to be used with periodic interrupts.
#   It can be directly called by the OS, but interrupts must be disabled before calling.
########################################################################################

	.section .ksave

        .global         _task_context_array  	# initialised in reset.s
        .global         _current_task_array  	# initialised in reset.s
        .global         _task_number_array  	# initialised in reset.s

_task_context_array: 	# 16 contexts : indexed by (proc_id*4 + task_id)
	.space	4096
NB_TASKS                = 1;

_current_task_array:	# 4 words : indexed by the proc_id
        .word   0       # _current_task_array[0] <= 0
        .word   0       # _current_task_array[1] <= 0
        .word   0       # _current_task_array[2] <= 0
        .word   0       # _current_task_array[3] <= 0

_task_number_array:	# 4 words : indexed by the proc_id
        .word   1       # _task_number_array[0] <= 1
        .word   1       # _task_number_array[1] <= 1
        .word   1       # _task_number_array[2] <= 1
        .word   1       # _task_number_array[3] <= 1

#########################################################################################

        .section .switch

        .global         _ctx_switch         	# makes it an external symbol

        .align          2

_ctx_switch:

	# test if more than one task on the processor

	mfc0	$26,	$15,	1	
	andi	$26,	$26,	0x7		# $26 <= proc_id
	sll	$26,	$26,	2		# $26 <= 4*proc_id
        la      $27,   _task_number_array  	# $27 <= base address of _task_number_array
	addu	$27,	$27,	$26		# $27 <= _task_number_array + 4*proc_id
        lw	$27,	($27)			# $27 <= task number
	addi	$26,	$27,	-1		# $26 <= _task_number - 1
	bnez	$26,	do_it			# 0 if only one task
	jr	$31				# return
do_it:
	# save _current task context 

	mfc0	$26,	$15,	1
	andi	$26,	$26,	0x7		# $26 <= proc_id
	sll	$26,	$26,	2		# $26 <= 4*proc_id
        la      $27,    _current_task_array  	# $27 <= base address of _current_task_array
	addu	$27,	$27,	$26		# $27 <= _current_task_array + 4*proc_id
        lw      $26,    ($27)               	# $26 <= current task index 
        sll     $26,    $26,     8          	# $26 <= 256*task_id
        la      $27,    _task_context_array	# $27 <= base address of context array 
        addu    $27,    $27,     $26        	# $27 <= _task_context_array + 256*task_id
	mfc0	$26,	$15,	1
	andi	$26,	$26,	0x7		# $26 <= proc_id
	sll	$26,	$26,	10		# $26 <= 1024*proc_id
	addu	$27,	$27,	$26		# $27 <= taxk_context_array + 256*(proc_id*4 + task_id)
	
        mfc0    $26,   $12                  	# $26 <= SR
        sw      $26,   0*4($27)             	# ctx[0] <= SR  
        .set noat
        sw      $1,    1*4($27) 		# ctx[1] <= $1 
        .set at
        sw      $2,    2*4($27)			# ctx[2] <= $2			
        sw      $3,    3*4($27)			# ctx[3] <= $3
        sw      $4,    4*4($27)			# ctx[4] <= $4
        sw      $5,    5*4($27)			# ctx[5] <= $5
        sw      $6,    6*4($27)			# ctx[6] <= $6
        sw      $7,    7*4($27)			# ctx[7] <= $7
        sw      $8,    8*4($27)			# ctx[8] <= $8
        sw      $9,    9*4($27)			# ctx[9] <= $9
        sw      $10,   10*4($27)		# ctx[10] <= $10
        sw      $11,   11*4($27)		# ctx[11] <= $11
        sw      $12,   12*4($27)		# ctx[12] <= $12
        sw      $13,   13*4($27)		# ctx[13] <= $13
        sw      $14,   14*4($27)		# ctx[14] <= $14
        sw      $15,   15*4($27)		# ctx[15] <= $15
        sw      $16,   16*4($27)		# ctx[16] <= $16
        sw      $17,   17*4($27)		# ctx[17] <= $17
        sw      $18,   18*4($27)		# ctx[18] <= $18
        sw      $19,   19*4($27)		# ctx[19] <= $19
        sw      $20,   20*4($27)		# ctx[20] <= $20
        sw      $21,   21*4($27)		# ctx[21] <= $21
        sw      $22,   22*4($27)		# ctx[22] <= $22
        sw      $23,   23*4($27)		# ctx[23] <= $23
        sw      $24,   24*4($27)		# ctx[24] <= $24
        sw      $25,   25*4($27)		# ctx[25] <= $25 
        mflo    $26                         	# 
        sw      $26,   26*4($27)            	# ctx[26] <= LO
        mfhi    $26                         	# 
        sw      $26,   27*4($27)            	# ctx[27] <= H1
        sw      $28,   28*4($27)            	# ctx[28] <= $28
        sw      $29,   29*4($27)		# ctx[29] <= $29
        sw      $30,   30*4($27)		# ctx[30] <= $30
        sw      $31,   31*4($27)		# ctx[31] <= $31
        mfc0    $26,   $14                  	#
        sw      $26,   32*4($27)            	# ctx[32] <= EPC   
        mfc0    $26,   $13                  	#
        sw      $26,   33*4($27)            	# ctx[33] <= CR 

	# select  the new task

	mfc0	$15,	$15,	1	
	andi	$15,	$15,	0x7		# $15 <= proc_id
	sll	$16,	$15,	2		# $16 <= 4*proc_id
        la      $17,    _current_task_array  	# $17 <= base address of _current_task_array
	addu	$17,	$17,	$16		# $17 <= _current_task_array + 4*proc_id
        lw      $18,    ($17)               	# $18 <= _current task index 
	la	$19,	_task_number_array	# $19 <= base address of _task_number_array	
	addu	$19,	$19,	$16		# $19 <= _task_number_array + 4*proc_id
	lw	$20,	($19)			# $20 <= max = number of tasks
        addiu   $18,    $18,     1		# $18 <= new task index          
        sub 	$2,     $18,	$20         	# test modulo max
        bne     $2,     $0,     no_wrap
        add     $18,    $0,     $0		# $18 <= new task index
no_wrap:
	sw	$18,    ($17)               	# update _current_task_array
	
	# restore next task context
 
        sll     $19,    $18,	8         	# $19 <= 256*task_id
        la      $27,    _task_context_array	# $27 <= base address of context array 
        addu    $27,    $27,	$19        	# $27 <= _task_context_array + 256*task_id
	sll	$19,	$15,	10		# $19 <= 1024*proc_id
        addu	$27,	$27,	$19		# $27 <= _task_context_array + 256*(proc_id*4 + task_id)

        lw      $26,    0*4($27)  
        mtc0    $26,    $12                 	# restore SR
       .set noat
        lw      $1,     1*4($27)  		# restore $1
       .set at        
        lw      $2,     2*4($27)		# restore $2
        lw      $3,     3*4($27)		# restore $3
        lw      $4,     4*4($27)		# restore $4
        lw      $5,     5*4($27)		# restore $5
        lw      $6,     6*4($27)		# restore $6
        lw      $7,     7*4($27)		# restore $7
        lw      $8,     8*4($27)		# restore $8
        lw      $9,     9*4($27)		# restore $9
        lw      $10,    10*4($27)		# restore $10
        lw      $11,    11*4($27)		# restore $11
        lw      $12,    12*4($27)		# restore $12
        lw      $13,    13*4($27)		# restore $13
        lw      $14,    14*4($27)		# restore $14
        lw      $15,    15*4($27)		# restore $15
        lw      $16,    16*4($27)		# restore $16
        lw      $17,    17*4($27)		# restore $17
        lw      $18,    18*4($27)		# restore $18
        lw      $19,    19*4($27)		# restore $19
        lw      $20,    20*4($27)		# restore $20
        lw      $21,    21*4($27)		# restore $21
        lw      $22,    22*4($27)		# restore $22
        lw      $23,    23*4($27)		# restore $23
        lw      $24,    24*4($27)		# restore $24
        lw      $25,    25*4($27)		# restore $25
        lw      $26,    26*4($27)  
        mtlo    $26                         	# restore LO
        lw      $26,    27*4($27)  	
        mthi    $26                         	# restore HI
        lw      $28,    28*4($27)  		# restore $28
        lw      $29,    29*4($27)		# restore $29
        lw      $30,    30*4($27)		# restore $30
        lw      $31,    31*4($27)		# restore $31
        lw      $26,    32*4($27)           
        mtc0    $26,    $14                 	# restore EPC
        lw      $26,    33*4($27)  
        mtc0    $26,    $13                 	# restore CR
        
        jr      $31                         	# returns to caller

