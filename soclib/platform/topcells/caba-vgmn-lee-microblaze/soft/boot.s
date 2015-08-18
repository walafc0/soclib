/* Boot:
   Normal boot approach, for all programs but the bootstrapping. 
   Author: Frédéric Pétrot <Frederic.Petrot@imag.fr>
/* Section:
   I define here several sections in order to be able to map them
   easilly in memory */
  .section .reset,"ax",@progbits
	.extern _init
	.extern _interrupt_handler
	.extern _stack
	.globl  _boot
	.ent    _boot

                .org 0x00
_boot:
                bri _start1

                .org 0x08
_exception_handler:
                bri _exception_handler

                .org 0x10
_it:
                bri _interrupt_handler

                .org 0x18
_break_handler:
                bri _break_handler

                .org 0x20
_hw_exception_handler:
                bri _hw_exception_handler

                .text
                /* 0x300 :
                 * This offset is required to leave room for the
                 * bootloader. */
                .org 0x300
_start1:
	addik   r1, r0, _stack
	bri     _init
_boot_end:
	bri 	_boot_end
	.end	_boot
