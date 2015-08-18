/////////////////////////////////////////////////////////////////
//	ADDRESS SPACE SEGMENTATION
//
//	This file must be included in the top.cc  file, 
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
//	
//	Those three segments are mandatory
//	The base adresses are imposed by the 
//	NIOS processor architecture
/////////////////////////////////////////////////////////////////

#define	RESET_BASE	0x00802000
#define	RESET_SIZE	0x00001000 

#define	TEXT_BASE	0x01000000
#define	TEXT_SIZE	0x00080000

#define	EXCEP_BASE	0x00800820
#define	EXCEP_SIZE	0x00001000

/////////////////////////////////////////////////////////////////
//	global data segment (initialised)
/////////////////////////////////////////////////////////////////

#define	DATA_BASE	0x02000000
#define	DATA_SIZE	0x00080000

/////////////////////////////////////////////////////////
//	local data segments 
//	
/////////////////////////////////////////////////////////

/*#define	LOC_BASE	0x20000000
  #define	LOC_SIZE	0x00100000 */


//////////////////////////////////////////////////////////
//	System TTY and private TTYs
//	
//	- The system TTY is used by the printf() function.
//	This segment must always be defined, 
//	as it is used by the Mutek kernel. 
///////////////////////////////////////////////////////////

#define	TTY_BASE	0xC0200000
#define	TTY_SIZE	0x00000256

/////////////////////////////////////////////////////////
//	System Peripherals
//
//	Those segments must be defined for MUTEK,
//	even if the corresponding peripherals
//	are not instanciated in the hardware platform
/////////////////////////////////////////////////////////

#define	TIMER_BASE	0xB0200000
#define	TIMER_SIZE	0x00000100

#define	ITC_BASE	0xB1200000
#define	ITC_SIZE	0x00001000

#define	LOC0_BASE	0xB2200000
#define	LOC0_SIZE	0x00001000

#define	DFT_BASE	0xB3200000
#define	DFT_SIZE	0x00001000

#define FB_WIDTH 320
#define FB_HEIGHT 200

#define	FB_BASE	0xB4200000
#define	FB_SIZE	FB_WIDTH*FB_HEIGHT*4

#define FB_MODE 256


