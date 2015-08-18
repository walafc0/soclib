
/////////////////////////////////////////////////////////////////
//	ADDRESS SPACE SEGMENTATION
//
//	This file must be included in the system.cpp file, 
//	for harware configuration : It is used to build
//	the SOCLIB_SEGMENT_TABLE.
//
//	This file can also be used by the ldscript generator,
//	for embedded software generation.
//	
//	It gives the system integrator the garanty
//	that hardware and software have the same
//	description of the address space segmentation.
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//	reset, and exception segments
//      base address required by MIPS processor
///////////////////////////////////////////////////////////////////////////////////

#define	RESET_BASE	0xBFC00000
#define	RESET_SIZE	0x00010000

#define	EXCEP_BASE	0x80000000
#define	EXCEP_SIZE	0x00010000

///////////////////////////////////////////////////////////////////////////////////
//	global data segment (initialised)
///////////////////////////////////////////////////////////////////////////////////

#define MC0_M_BASE	0x10000000
#define MC0_M_SIZE	0x00400000

#define MC1_M_BASE	0x50000000
#define MC1_M_SIZE	0x00400000

#define MC2_M_BASE	0x91000000
#define MC2_M_SIZE	0x00400000

#define MC3_M_BASE	0xD0000000
#define MC3_M_SIZE	0x00400000

////////////////////////////////////////////////////////////////////////////////
//	System devices (seen by the software)
////////////////////////////////////////////////////////////////////////////////

#define	TTY_BASE	0xC1400000
#define	TTY_SIZE	0x00000100

#define	TIMER_BASE	0xD1400000
#define	TIMER_SIZE	0x00000100

#define	LOCKS_BASE	0xE1400000
#define	LOCKS_SIZE	0x00000100

#define MC0_R_BASE	0x1F400000
#define MC0_R_SIZE	0x00000008

#define MC1_R_BASE	0x5F400000
#define MC1_R_SIZE	0x00000008

#define MC2_R_BASE	0x9F400000
#define MC2_R_SIZE	0x00000008

#define MC3_R_BASE	0xDF400000
#define MC3_R_SIZE	0x00000008

////////////////////////////////////////////////////////////////////////////////
//	System devices (NOT seen by the software)
////////////////////////////////////////////////////////////////////////////////

#define PROC0_BASE	0x01400000
#define PROC0_SIZE	0x00000008

#define PROC1_BASE	0x02400000
#define PROC1_SIZE	0x00000008

#define PROC2_BASE	0x03400000
#define PROC2_SIZE	0x00000008

#define PROC3_BASE	0x04400000
#define PROC3_SIZE	0x00000008

#define PROC4_BASE	0x43400000
#define PROC4_SIZE	0x00000008

#define PROC5_BASE	0x44400000
#define PROC5_SIZE	0x00000008

#define PROC6_BASE	0x45400000
#define PROC6_SIZE	0x00000008

#define PROC7_BASE	0x46400000
#define PROC7_SIZE	0x00000008

#define PROC8_BASE	0xA3400000
#define PROC8_SIZE	0x00000008

#define PROC9_BASE	0xA4400000
#define PROC9_SIZE	0x00000008

#define PROC10_BASE	0xA5400000
#define PROC10_SIZE	0x00000008

#define PROC11_BASE	0xA6400000
#define PROC11_SIZE	0x00000008

#define PROC12_BASE	0xE2400000
#define PROC12_SIZE	0x00000008

#define PROC13_BASE	0xE3400000
#define PROC13_SIZE	0x00000008

#define PROC14_BASE	0xE4400000
#define PROC14_SIZE	0x00000008

#define PROC15_BASE	0xE5400000
#define PROC15_SIZE	0x00000008

#define CLEANUP_OFFSET  0x1F400000
