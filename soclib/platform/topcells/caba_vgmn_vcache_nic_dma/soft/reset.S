/********************************************************************************/
/*	File : reset.S                                                              */
/*	Author : Alain Greiner	                                                    */
/*	Date : 01/11/2012							                                */
/********************************************************************************/
/* This boot code is an adhoc code used to test the MULTI_NIC and CHBUF_DMA     */
/* Processor 0 initializes the NIC and DMA peripherals to build a loop-back:    */
/* For each NIC(i) channel, a dedicated DMA(i) channel transfer the packets     */
/* from the RX CHBUF to the TX CHBUF.        		                            */
/*																				*/
/* Implementation note:															*/
/* The entry point is 0xbfc00000, but the actual boot code starts at address	*/
/* 0xbfc00500, and a minimal boot exception handler is implemented at address	*/
/* 0xbfc0380.									                                */
/********************************************************************************/
		
#include "mips32_registers.h"

#define EXCEP_ORG    		0x380
#define START_ORG    		0x500

	.section .boot,"ax",@progbits
	.align	2
   	.set noreorder

/********************************************************/
/* reset entry point                                    */
/* (address 0xBFC00000 imposed by the hardware)         */
/********************************************************/
boot_reset:
    j	boot_start
	nop

/*******************************************************/
/* handling exceptions in the boot phase               */
/* (address 0xBFC00380 imposed by the hardware         */
/*******************************************************/
	.align	2
    .org	EXCEP_ORG

boot_excep:

    # get the lock protecting TTY0 
    la		k0, boot_tty0_lock
    ll		k1, 0(k0)
    bnez	k1, boot_excep
    li		k1, 1
    sc      k1, 0(k0)
    beqz	k1, boot_excep
    nop

    # display error messages on TTY0  
    la		a0,	boot_error_string
    jal		boot_puts
    nop
    mfc0	a0,	CP0_TIME
	jal		boot_putx
	nop
    la		a0,	boot_lf_string
    jal		boot_puts
    nop

	la		a0,	boot_pid_string
    jal		boot_puts
    nop
    mfc0	k0,	CP0_PROCID
    andi	a0,	k0,	0xFFF
	jal		boot_putx
    nop
    la		a0,	boot_lf_string
    jal		boot_puts
    nop

	la		a0,	boot_epc_string
    jal		boot_puts
    nop
    mfc0	a0,	CP0_EPC
	jal		boot_putx
    nop
    la		a0,	boot_lf_string
    jal		boot_puts
    nop

	la		a0,	boot_cr_string
    jal		boot_puts
    nop
    mfc0	a0,	CP0_CR
	jal		boot_putx
    nop
    la		a0,	boot_lf_string
    jal		boot_puts
    nop

	la		a0,	boot_sr_string
    jal		boot_puts
    nop
    mfc0	a0,	CP0_SR
	jal		boot_putx
    nop
    la		a0,	boot_lf_string
    jal		boot_puts
    nop

	la		a0,	boot_bar_string
    jal		boot_puts
    nop
    mfc0	a0,	CP0_BVAR
	jal		boot_putx
    nop
    la		a0,	boot_lf_string
    jal		boot_puts
    nop

    # release the lock  
    la		k0,	boot_tty0_lock
    li		k1,	0
    sw		k1, 0(k0)

    # exit 
	j	boot_exit
	nop

/*******************************************/
/* actual starting point for the boot code */
/*******************************************/
    .align 	2
    .org	START_ORG

boot_start:

    # get the procid 
    mfc0	k0,	CP0_PROCID
    andi	k0,	k0,	0xFFF	# no more than 4096 processors

    # Only processor 0 does init 
    bne		k0,	zero,	boot_suicide
    nop

    # Processor 0 get a temporary stack 
    la	    sp,	pseg_ram_base
    addiu	sp,	sp,	0x4000				# SP <= pseg_ram_base + 16K 

    # Processor 0 initialises NIC & DMA peripherals
    j	    boot_init
    nop
 
boot_suicide:

    j       boot_exit
    nop

boot_error_string:	.asciiz "\n[BOOT] Fatal Error at cycle "
boot_pid_string:	.asciiz "    PID  = "
boot_sr_string:		.asciiz "    SR   = "
boot_cr_string:		.asciiz "    CR   = "
boot_epc_string:	.asciiz "    EPC  = "
boot_bar_string:	.asciiz "    BAR  = "
boot_lf_string:		.asciiz "\n"

boot_tty0_lock:		.word	0

	.set	reorder


