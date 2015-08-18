/*
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
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: nipo
 */

#define	TEXT_BASE	0x00400000
#define	TEXT_SIZE	0x00050000

#define	PPC_SPECIAL_BASE	0x00500000
#define	PPC_SPECIAL_SIZE	0x00010000

#define	PPC_BOOT_BASE	0xffffff80
#define	PPC_BOOT_SIZE	0x00000080

#define	DATA_BASE	0x10000000
#define	DATA_SIZE	0x00020000

#define	TTY_BASE	0xC0200000
#define	TTY_SIZE	0x00000258

#define	TIMER_BASE	0xB0200000
#define	TIMER_SIZE	0x00000100

#define	ITC_BASE	0xB1200000
#define	ITC_SIZE	0x00001000
