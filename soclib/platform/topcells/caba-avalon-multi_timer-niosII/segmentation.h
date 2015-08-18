/* -*- c++ -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 * 
 * Copyright (C) IRISA/INRIA, 2007
 *         Francois Charot <charot@irisa.fr>
 *
 * Address Space segmentation 
 */

/////////////////////////////////////////////////////////////////
//	text, reset, and exception segments
//	
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

#define	LOC0_BASE	0xD0200000
#define	LOC0_SIZE	0x00001000

#define	LOC1_BASE	0xD0210000
#define	LOC1_SIZE	0x00001000

#define	LOC2_BASE	0xD0220000
#define	LOC2_SIZE	0x00001000

#define	LOC3_BASE	0xD0230000
#define	LOC3_SIZE	0x00001000


//////////////////////////////////////////////////////////
//	System TTY and private TTYs
//	
//	- The system TTY is used by the printf() function.
//
///////////////////////////////////////////////////////////

#define	TTY_BASE	0xC0200000
#define	TTY_SIZE	0x00000258

/////////////////////////////////////////////////////////
//	System Peripherals
//
/////////////////////////////////////////////////////////

#define	TIMER_BASE	0xB0200000
#define	TIMER_SIZE	0x00000100

#define	LOCKS_BASE	0xB2200000
#define	LOCKS_SIZE	0x00001000


