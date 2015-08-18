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
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors :
 * 
 * History :
 *
 * Comment :
 *
 */

#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned int t_uint32;
typedef int          t_int32;

typedef unsigned short t_uint16;
typedef short          t_int16;

typedef unsigned char t_uint8;
typedef char          t_int8;

#ifdef __cplusplus
typedef bool t_bit;
#endif /* #ifdef __cplusplus */

#if !defined(NULL)
#define NULL 0
#endif /*#if !defined(NULL) */

#if defined(F2_API_CPU_SYSTEMC)

// For a DATA type transfer, in the packet header, this defines the packet direction of the wormhole algorithm
typedef enum { NORTH=0, EAST=1, SOUTH=2, WEST=3, RES=4 } anoc_dir;

#endif /* #if defined(F2_API_CPU_SYSTEMC) */

#define ANOC_PATH_LENGTH 9      // length of the path_to_target information (equiv. to max number of node through the network)

#endif /*#ifndef _TYPES_H_*/
