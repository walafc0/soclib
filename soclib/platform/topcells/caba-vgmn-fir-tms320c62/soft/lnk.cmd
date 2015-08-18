/****************************************************************************/
/*  lnk.cmd   v#####                                                        */
/*  Copyright (c) 1996@%%%%  Texas Instruments Incorporated                 */
/*    Usage:  lnk6x <obj files...>    -o <out file> -m <map file> lnk.cmd   */
/*            cl6x  <src files...> -z -o <out file> -m <map file> lnk.cmd   */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on a C620x/C670x         */
/*                 simulator.  Use it as a guideline.  You will want to     */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/*    Notes: (1)   You must specivy a directory in which rts6x00.lib is     */
/*                 located.  either add a -i"<directory>" line to this      */
/*                 file or use the system environment variable C_DIR to     */
/*                 specify a search path for the libraries.                 */
/*                                                                          */
/*           (2)   If the run-time library you are using is not named       */
/*                 rts6200[e].lib, rts6400[e].lib, or rts6700[e].lib, be    */
/*                 sure to use the correct name here.                       */
/*                                                                          */
/****************************************************************************/
-c
-heap  0x2000
-stack 0x4000

/* Memory Map 1 - the default */
MEMORY
{
        PMEM:   o = 00000020h   l = 0000ffe0h 
        EXT0:   o = 00400000h   l = 01000000h
        EXT1:   o = 01400000h   l = 00400000h
        EXT2:   o = 02000000h   l = 00100000h
        EXT3:   o = 03000000h   l = 00100000h
        BMEM:   o = 04000000h   l = 00100000h 
} 

/* Memory Map 0 */
/* 
MEMORY
{
        PMEM:   o = 00000020h   l = 0000ffe0h 
        EXT0:   o = 00400000h   l = 01000000h
        EXT1:   o = 01400000h   l = 00400000h
        EXT2:   o = 02000000h   l = 00100000h
        EXT3:   o = 03000000h   l = 00100000h
        BMEM:   o = 04000000h   l = 00100000h 
}
*/

SECTIONS
{
    .text:
    { reset.obj (.text)
      main.obj (.text)
      system.obj (.text) 
    }       >       PMEM
    .stack      >       EXT2
    .bss        >       EXT2
    .cinit      >       EXT2
    .cio        >       EXT2 
    .const      >       EXT2
    .data       >       EXT2
    .switch     >       EXT2 
    .sysmem     >       EXT2
    .far        >       EXT2
    .ppdata     >       EXT2
}
