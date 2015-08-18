/*
 *  start.s
 *
 *  Common start code for SPARC.
 *
 *  This is based on the file srt0.s provided with the binary
 *  distribution of the SPARC Instruction Simulator (SIS) found
 *  at ftp://ftp.estec.esa.nl/pub/ws/wsd/erc32.
 *
 *  start.S,v 1.2 2001/04/24 20:49:11 joel Exp
 */

#include "trap.h"
        
#define TRAP(handler)                   \
	mov   %psr, %l0 ;		\
	sethi %hi(handler), %l4 ;	\
	jmp   %l4+%lo(handler);		\
	mov   %tbr, %l3;

#define BAD_TRAP			\
	ta 0;				\
	nop;				\
	nop;				\
	nop;
        
  .text

/* 
 *  The trap table has to be the first code in a boot PROM.  This file
 *  uses an abbreviated trap which has every entry which might be used
 *  before an OS installs its own trap table.
 */

        .global trap_table
trap_table:

        TRAP(hard_reset);                       ! 00 reset trap 
        BAD_TRAP;                               ! 01 instruction access exception
        BAD_TRAP;                               ! 02 illegal instruction
        BAD_TRAP;                               ! 03 privileged instruction
        BAD_TRAP;                               ! 04 fp disabled
        TRAP(window_overflow_trap_handler);     ! 05 window overflow
        TRAP(window_underflow_trap_handler);    ! 06 window underflow
        BAD_TRAP;                               ! 07 memory address not aligned
        BAD_TRAP;                               ! 08 fp exception
        BAD_TRAP;                               ! 09 data access exception
        BAD_TRAP;                               ! 0A tag overflow
        BAD_TRAP;                               ! 0B watchpoint detected
        BAD_TRAP;                               ! 0C undefined
        BAD_TRAP;                               ! 0D undefined
        BAD_TRAP;                               ! 0E undefined
        BAD_TRAP;                               ! 0F undefined
        BAD_TRAP;                               ! 10 undefined

        .rept 15                                ! 11 - 1F Interrupt level handler
        TRAP(user_handler);                  
        .endr

        .rept 224                               ! 20 - FF : undefined 
        BAD_TRAP;
        .endr
        
/*
 *  This is the hard reset code.
 */
        .global hard_reset
hard_reset:
        set     trap_table, %g1                 ! Initialize TBR
        mov     %g1, %tbr

        mov	%psr, %g1                       ! Initialize WIM
	add	%g1, 1, %g2
	and	%g2, 0x7, %g2
	set	1, %g3
	sll	%g3, %g2, %g3
        mov     %g3, %wim
	nop
	nop
	nop

	or	%g1, 0xF20, %g1                 ! enable traps, disable interrupts...
        wr      %g1, %psr		

        nop
        nop
        nop

        set     __stack, %l1                    ! set stack pointer
        add     %l1, %g0, %sp
      	sub	%sp, 16, %sp
	andn	%sp, 0x0f, %sp                  ! align stack on 16-byte boundary

        mov     %sp, %fp                        ! Set frame pointer
        nop

        call    start                           ! call start
        nop
	

	
/*
*  Window overflow trap handler.
*
*  On entry:
*
*    l0 = psr (from trap table)
*    l1 = pc
*    l2 = npc
*/

        .global window_overflow_trap_handler
window_overflow_trap_handler:

	/*
	*  Calculate new WIM by "rotating" the valid bits in the WIM right 
	*  by one position.  The following shows how the bits move for a SPARC
	*  cpu implementation where NUMBER_OF_REGISTER_WINDOWS is 8.
	*
	*    OLD WIM = 76543210
	*    NEW WIM = 07654321
	*
	*  NOTE: New WIM must be stored in a global register since the
	*        "save" instruction just prior to the load of the wim 
	*        register will result in the local register set changing.
	*/

        mov  %wim, %l3                   ! l3 = WIM
        mov  %g1, %l7                    ! save g1
        srl  %l3, 1, %g1                 ! g1 = WIM >> 1
        sll  %l3, NUMBER_OF_REGISTER_WINDOWS-1 , %l4
                                         ! l4 = WIM << (Number Windows - 1)
        or   %l4, %g1, %g1               ! g1 = (WIM >> 1) | 
                                         !      (WIM << (Number Windows - 1))

        save                             ! Get into window to be saved.
        mov  %g1, %wim                   ! load new WIM
        nop		                 ! 3 slot delay
	nop
	nop
	
 	std     %l0, [%sp + 0 * 4]	 ! save local register set
        std     %l2, [%sp + 2 * 4]
        std     %l4, [%sp + 4 * 4]
        std     %l6, [%sp + 6 * 4]

        std     %i0, [%sp + 8 * 4]	 ! save input register set
        std     %i2, [%sp + 10 * 4]
        std     %i4, [%sp + 12 * 4]
        std     %i6, [%sp + 14 * 4]

        restore                          ! Go back to trap window.
        mov  %l7, %g1                    ! restore g1
        jmp  %l1                         ! Re-execute save.
        rett %l2

/*
*  Window underflow trap handler.
*
*  On entry:
*
*    l0 = psr (from trap table)
*    l1 = pc
*    l2 = npc
*/

        .globl window_underflow_trap_handler
window_underflow_trap_handler:

        /*
         *  Calculate new WIM by "rotating" the valid bits in the WIM left 
         *  by one position.  The following shows how the bits move for a SPARC
         *  cpu implementation where NUMBER_OF_REGISTER_WINDOWS is 8.
         *
         *    OLD WIM = 76543210
         *    NEW WIM = 07654321
         *
         *  NOTE: New WIM must be stored in a global register since the
         *        "save" instruction just prior to the load of the wim 
         *        register will result in the local register set changing.
         */

        mov  %wim, %l3                  ! Calculate new WIM
        sll  %l3, 1, %l4                ! l4 = WIM << 1
        srl  %l3, NUMBER_OF_REGISTER_WINDOWS-1, %l5
                                        ! l5 = WIM >> (Number Windows-1)
        or   %l5, %l4, %l5              ! l5 = (WIM << 1) |
                                        !      (WIM >> (Number Windows-1))
        mov  %l5, %wim                  ! load the new WIM
        nop; nop; nop
        restore                         ! Two restores to get into the
        restore                         ! window to restore
        ldd  [%sp + 0x00], %l0          ! First the local register set
        ldd  [%sp + 0x08], %l2
        ldd  [%sp + 0x10], %l4
        ldd  [%sp + 0x18], %l6
        ldd  [%sp + 0x20], %i0          ! Then the input registers
        ldd  [%sp + 0x28], %i2
        ldd  [%sp + 0x30], %i4
        ldd  [%sp + 0x38], %i6
        save                            ! Get back to the trap window.
        save
        jmp  %l1                        ! Re-execute restore.
        rett  %l2



/*
 * This function is called when any SPARC trap (except window overflow
 * or underflow) occurs.  It makes sure that the invalid register
 * window is still available before jumping into C code.  It will also
 * restore the world if you return from handle_exception.
 *       
 *  We enter this handler from the 4 instructions in the trap table with
 *  the following registers assumed to be set as shown:
 *
 *    l0 = PSR
 *    l1 = PC
 *    l2 = nPC
 *    l3 = tbr
 */
        .globl user_handler
user_handler:
        mov     %psr, %l0
        mov     %wim, %l3

        srl     %l3, %l0, %l4           ! wim >> cwp
        cmp     %l4, 1
        bne     window_fine             ! Branch if not in the invalid window
        nop

        mov     %g1, %l4                ! Save g1, we use it to hold the wim
        srl     %l3, 1, %g1             ! Rotate wim right
        sll     %l3, 8-1, %l5
        or      %l5, %g1, %g1
                
        save    %g0, %g0, %g0           ! Slip into next window
        mov     %g1, %wim               ! Install the new wim

        std     %l0, [%sp + 0 * 4]      ! save L & I registers
        std     %l2, [%sp + 2 * 4]
        std     %l4, [%sp + 4 * 4]
        std     %l6, [%sp + 6 * 4]

        std     %i0, [%sp + 8 * 4]
        std     %i2, [%sp + 10 * 4]
        std     %i4, [%sp + 12 * 4]
        std     %i6, [%sp + 14 * 4]

        restore                         ! Go back to trap window.
        mov     %l4, %g1                ! Restore g1

window_fine:
        sethi   %hi(in_trap_handler), %l4
        ld      [%lo(in_trap_handler) + %l4], %l5
        tst     %l5
        bg      recursive_trap
        inc     %l5

        /* use the stack we set in the linker script */
        sethi   %hi(__trap_stack), %l6
        or      %l6,%lo(__trap_stack),%l6
        mov     %l6, %sp                ! set the stack pointer

recursive_trap:
        st      %l5, [%lo(in_trap_handler) + %l4]

        sub     %sp,(16+1+6+1+72)*4,%sp ! Make room for input & locals
                                        ! + hidden arg + arg spill
                                        ! + doubleword alignment
                                        ! + registers[72] local var

        std     %g0, [%sp + (24 + 0) * 4] ! registers[Gx]
        std     %g2, [%sp + (24 + 2) * 4]
        std     %g4, [%sp + (24 + 4) * 4]
        std     %g6, [%sp + (24 + 6) * 4]

        std     %i0, [%sp + (24 + 8) * 4] ! registers[Ox]
        std     %i2, [%sp + (24 + 10) * 4]
        std     %i4, [%sp + (24 + 12) * 4]
        std     %i6, [%sp + (24 + 14) * 4]
                                         ! F0->F31 not implemented
        mov     %y, %l4
        mov     %tbr, %l5
        st      %l4, [%sp + (24 + 64) * 4] ! Y
        st      %l0, [%sp + (24 + 65) * 4] ! PSR
        st      %l3, [%sp + (24 + 66) * 4] ! WIM
        st      %l5, [%sp + (24 + 67) * 4] ! TBR
        st      %l1, [%sp + (24 + 68) * 4] ! PC
        st      %l2, [%sp + (24 + 69) * 4] ! NPC
                                         ! CPSR and FPSR not implemented

        mov     %tbr, %o0               ! Prepare Interrupt number
        srl     %o0, 4, %o0
        and     %o0, 0xff, %o0

        or      %l0, 0xf20, %l4
        mov     %l4, %psr               ! Turn on traps, disable interrupts

        call    interrupt_hw_handler
        add     %sp, 24 * 4, %o0        ! Pass address of registers

/* Reload all of the registers that aren't on the stack */

        ld      [%sp + (24 + 1) * 4], %g1  ! registers[Gx]
        ldd     [%sp + (24 + 2) * 4], %g2
        ldd     [%sp + (24 + 4) * 4], %g4
        ldd     [%sp + (24 + 6) * 4], %g6

        ldd     [%sp + (24 + 8) * 4], %i0  ! registers[Ox]
        ldd     [%sp + (24 + 10) * 4], %i2
        ldd     [%sp + (24 + 12) * 4], %i4
        ldd     [%sp + (24 + 14) * 4], %i6

        ldd     [%sp + (24 + 64) * 4], %l0 ! Y & PSR
        ldd     [%sp + (24 + 68) * 4], %l2 ! PC & NPC

        restore                         ! Ensure that previous window is valid
        save    %g0, %g0, %g0           !  by causing a window_underflow trap

        mov     %l0, %y
        mov     %l1, %psr               ! Make sure that traps are disabled
                                        ! for rett

        sethi   %hi(in_trap_handler), %l4
        ld      [%lo(in_trap_handler) + %l4], %l5
        dec     %l5
        st      %l5, [%lo(in_trap_handler) + %l4]
        
        jmpl    %l2, %g0                ! Restore old PC
        rett    %l3                     ! Restore old nPC


        .data
        .align  4
in_trap_handler:
        .word   0

        
/* end of file */
