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

	.data
	.globl __errno
__errno:
	.long 0
	
	
	.text
	.global start
	.global _start
start:
_start:

        /* clear BSS */
1:

        sethi %hi(_bss_start),%g2
        or    %g2,%lo(_bss_start),%g2  ! g2 = start of bss
        sethi %hi(_end),%g3
        or    %g3,%lo(_end),%g3         ! g3 = end of bss
        mov   %g0,%g1                   ! so std has two zeros
zerobss:
        std    %g0,[%g2]
        add    %g2,8,%g2
        cmp    %g2,%g3
        bleu,a zerobss
        nop

        mov     %0, %o2                 ! environ
        mov     %0, %o1                 ! argv
        mov     %0, %o0                 ! argc
        sub     %sp, 0x60, %sp          ! room for boot_card to save args

        call    main    		! call start
        nop
	
	/* call exit from the C library so atexit gets called, and the
         * C++ destructors get run. This calls our exit routine below    
         * when it's done.
         */
        call    exit    		! call exit()
        nop

.globl  _exit
_exit:
        ta    0				! Halt if _main returns
        nop

/* end of file */
