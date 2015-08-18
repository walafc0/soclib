/***************************************************
 *
 * LM32 startup code and exceptions handlers
 * This is based on the exception hnadler exemple
 * which can be found in the :
 *       "LatticeMico32 Processor Reference Manual"
 * And the u-boot start.S from Theobroma Systems  
 *        <www.theobroma-systems.com>
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 *
 ***************************************************/

/* ALL EXCEPTION HANDLERS ARE 8word (32bytes) long 
 * AND MUST BE ALIGNED
 */

/*
        System call convention:
        - System call number in register r8
        - Return value in r1 ( and r2 only if 64-bit value)
*/

.section .boot, "ax", @progbits

/* START OF EXCEPTION HANDLER */
    .global     _start
_start:
_reset_handler:
    xor  r0, r0, r0
    wcsr IE,r0 /* disable interrupts */
    wcsr IM,r0 /* mask all interrupts */
    mvhi    r1, hi(_reset_handler) 
    ori     r1, r1, lo(_reset_handler)
    wcsr    EBA, r1 /* Set Exception base address */
    bi _crt0
    nop
_breakpoint_handler:
    bi _reset_handler
    nop
    nop
    nop
    nop
    nop
    nop
    nop
_instruction_bus_error_handler:
    bi _reset_handler
    nop
    nop
    nop
    nop
    nop
    nop
    nop
_watchpoint_handler:
    bi _reset_handler
    nop
    nop
    nop
    nop
    nop
    nop
    nop
_data_bus_error_handler:
    bi _reset_handler
    nop
    nop
    nop
    nop
    nop
    nop
    nop
_divide_by_zero_handler:
    bi _reset_handler
    nop
    nop
    nop
    nop
    nop
    nop
    nop
_interrupt_handler:
    sw  (sp+0), ra   
    calli _save_all
    calli interrupt_handler
    bi _restore_all_and_eret
    nop
    nop
    nop
    nop
_system_call_handler:
    bi _reset_handler
    nop
    nop
    nop
    nop
    nop
    nop
    nop

/* END OF EXCEPTION HANDLER */

_save_all:
    addi    sp, sp, -56
    sw      (sp+4), r1
    sw      (sp+8), r2
    sw      (sp+12), r3
    sw      (sp+16), r4
    sw      (sp+20), r5
    sw      (sp+24), r6
    sw      (sp+28), r7
    sw      (sp+32), r8
    sw      (sp+36), r9
    sw      (sp+40), r10
    /*     ra      */
    sw      (sp+48), ea
    sw      (sp+52), ba
    /* ra is already in stacK */
    lw      r1, (sp+56)
    sw      (sp+44), r1
    ret

_restore_all_and_eret:
    lw      r1, (sp+4)
    lw      r2, (sp+8) 
    lw      r3, (sp+12) 
    lw      r4, (sp+16) 
    lw      r5, (sp+20) 
    lw      r6, (sp+24) 
    lw      r7, (sp+28) 
    lw      r8, (sp+32) 
    lw      r9, (sp+36) 
    lw      r10, (sp+40)
    lw      ra, (sp+44)
    lw      ea, (sp+48)
    lw      ba, (sp+52)
    addi    sp,  sp, 56 
    eret
