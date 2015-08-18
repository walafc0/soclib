/*\
 * Boot:
 * Just what is strictly necessary to do some testing.
 * The .org values are specified in the MicroBlaze data-sheet.
\*/
                .section .boot,"ax",@progbits
	.extern __start
	.extern _stack_top
	.globl  __boot
	.ent    __boot

                .org 0x00
__boot:
                bri .Lgo

                .org 0x08
_exception_handler:
                bri _exception_handler

                .org 0x10
_interrupt_handler:
                bri _interrupt_handler

                .org 0x18
_break_handler:
                bri _break_handler

                .org 0x20
_hw_exception_handler:
                bri _hw_exception_handler

                .text
                .org 0x50
.Lgo:
	addik   r1, r0, _stack_top
	bri     __start
_boot_end:
	bri 	_boot_end
	.end	_boot
