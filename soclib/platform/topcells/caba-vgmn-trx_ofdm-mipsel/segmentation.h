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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors : Ivan MIRO-PANADES
 * 
 * History :
 *
 * Comment :
 *
 */

#define	TEXT_BASE	0x00400000
#define	TEXT_SIZE	0x00050000

#define	RESET_BASE	0xBFC00000
#define	RESET_SIZE	0x00000200

#define	EXCEP_BASE	0x80000000
#define	EXCEP_SIZE	0x00010000

#define	DATA_BASE	0x10000000
#define	DATA_SIZE	0x00020000

#define	TTY_BASE	0xC0200000
#define	TTY_SIZE	0x00000010

#define	MWMR_BASE	0xA0200000
#define	MWMR_SIZE	0x00001000

#define	MWMRd_BASE	0x20200000
#define	MWMRd_SIZE	0x00003000

#define	LOCKS_BASE	0x30200000
#define	LOCKS_SIZE	0x00000100

