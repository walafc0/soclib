/////////////////////////////////////////////////////////////////
//	ADDRESS SPACE SEGMENTATION
//
//	This file must be included in the system.cpp file, 
//	for harware configuration : It is used to build
//	the SOCLIB_SEGMENT_TABLE.
//
//	This file is also used by the ldscript generator,
//	for embedded software generation.
//	
//	It gives the system integrator the garanty
//	that hardware and software have the same
//	description of the address space segmentation.
//	
//	The segment names cannot be changed.
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//	text, reset, and exception segments
/////////////////////////////////////////////////////////////////

#define	TEXT_BASE	0x00400000
#define	TEXT_SIZE	0x00050000

/* base address required by MIPS processor */
#define	RESET_BASE	0xBFC00000
#define	RESET_SIZE	0x00010000

/* base address required by MIPS processor */
#define	EXCEP_BASE	0x80000080
#define	EXCEP_SIZE	0x00010000

/////////////////////////////////////////////////////////////////
//	global data segment (initialised)
/////////////////////////////////////////////////////////////////

#define	DATA_BASE	0x10000000
#define	DATA_SIZE	0x00010000

/////////////////////////////////////////////////////////
//	local data segments 
/////////////////////////////////////////////////////////

#define	LOC0_BASE	0x20000000
#define	LOC0_SIZE	0x00001000

#define	LOC1_BASE	0x21000000
#define	LOC1_SIZE	0x00001000

#define	LOC2_BASE	0x22000000
#define	LOC2_SIZE	0x00001000

#define	LOC3_BASE	0x23000000
#define	LOC3_SIZE	0x00001000

//////////////////////////////////////////////////////////
//	System devices
///////////////////////////////////////////////////////////

#define	TTY_BASE	0xC0200000
#define	TTY_SIZE	0x00000258

#define	TIMER_BASE	0xB0200000
#define	TIMER_SIZE	0x00000100

#define	LOCKS_BASE	0xB2200000
#define	LOCKS_SIZE	0x00001000

//////////////////////////////////////////////////////////
//	Test devices
///////////////////////////////////////////////////////////

#define	TEST0_BASE	0x30000000
#define	TEST0_SIZE	0x00000258

#define	TEST1_BASE	0x31000000
#define	TEST1_SIZE	0x00000258

#define	TEST2_BASE	0x32000000
#define	TEST2_SIZE	0x00000258

#define	TEST3_BASE	0x33000000
#define	TEST3_SIZE	0x00000258

#define	TEST4_BASE	0x34000000
#define	TEST4_SIZE	0x00000258

#define	TEST5_BASE	0x35000000
#define	TEST5_SIZE	0x00000258

#define	TEST6_BASE	0x36000000
#define	TEST6_SIZE	0x00000258

#define	TEST7_BASE	0x37000000
#define	TEST7_SIZE	0x00000258

