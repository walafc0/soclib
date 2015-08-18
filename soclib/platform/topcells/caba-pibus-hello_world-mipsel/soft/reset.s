  .extern     _stack
  .extern     _gp

  .text
  .align 2

   li         $26,0x0000FF15
   mtc0       $26,$12

   la         $2,     _edata
   la         $3,     _end
   li         $4,     0

   /* set bss to zero */
loop:
   beq        $2, $3, endloop
   nop
   sw         $4, 0($2)
   addiu      $2, $2, 4
   j          loop
endloop:
   la         $sp,    _stack - 4
   la         $28,    _gp
   la         $27,    main
   j          $27
   nop
