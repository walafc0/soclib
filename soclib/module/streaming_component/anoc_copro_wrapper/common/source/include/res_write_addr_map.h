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
 * Comment : Ressource address mapping
 *
 */

#ifndef _RES_ADDR_MAP_H_
#define _RES_ADDR_MAP_H_

//-----------------------------------------
// CORE GENERAL ADDR MAP
#define CORE_BEGIN          (1<<20) // bit 21
#define CORE_MASK           (1<<18) // bit 19
// Core address
// bits  21  |  20  19  |  18 - - - 01   00 |
//      CORE |  CORE_ID |      ADDRESS      |

#define CORE_ADDR_NUM_CFG          0

//-----------------------------------------
// NETWORK INTERFACE ADDR MAP

#define PAGE_SHIFT 8 // 8 address bits for 1 page
#define PAGE_SIZE (1<<PAGE_SHIFT) // page_size=2^page_shift 

#define NB_PAGE_NAME 10
static std::string PAGE_NAME[NB_PAGE_NAME] = {
  "ITM",
  "AMR",
  "LPM",
  "MTM",
  "CTM_1",
  "CTM_2",
  "CFM",
  "IDM",
  "ODM",
  "CSD"
};

#define ITM_PAGE   0
#define AMR_PAGE   1
#define LPM_PAGE   2
#define MTM_PAGE   3
#define CTM_PAGE_1 4
#define CTM_PAGE_2 5
#define CFM_PAGE   6
#define IDM_PAGE   7
#define ODM_PAGE   8
#define CSD_PAGE   9
 
#define IDM_FIFO_SIZE_ADDR 252 // specific address in IDM page to write initial released size for input FIFOs

#define MTM_TASK_SIZE    32

#define CTM_1_TASK_SIZE  32    
#define CTM_1_CTX_ACTIVE_POS 31  
#define CTM_1_CTX_BASE_ADDR_POS 0
#define CTM_1_CTX_BASE_ADDR_SIZE 16

#define CTM_1_LOOP_REG_ADDR 255

#define CFM_ICC_CFG_BASE_ADDR   2
#define CFM_OCC_CFG_BASE_ADDR   3
#define CFM_CORE0_CFG_BASE_ADDR 4
#define CFM_CORE1_CFG_BASE_ADDR 5
#define CFM_CORE2_CFG_BASE_ADDR 6
#define CFM_CORE3_CFG_BASE_ADDR 7

//------------------

#endif /* _RES_ADDR_MAP_H_ */
