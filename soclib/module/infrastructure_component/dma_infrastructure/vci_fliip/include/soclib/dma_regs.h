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
 * Copyright (c) CITI/INSA, 2009
 * 
 * Authors:
 * 	Ludovic L'Hours <ludovic.lhours@insa-lyon.fr>
 * 	Antoine Fraboulet <antoine.fraboulet@insa-lyon.fr>
 * 	Tanguy Risset <tanguy.risset@insa-lyon.fr>
 * 
 */

#ifndef DMA_REGS_H
#define DMA_REGS_H

enum {
	FIFO_0_REG,
	FIFO_1_REG,
	FIFO_2_REG,
	FIFO_3_REG,
	FIFO_4_REG,
	FIFO_5_REG,
	FIFO_6_REG,
	FIFO_7_REG,
	FIFO_8_REG,
	FIFO_9_REG,
	FIFO_10_REG,
	FIFO_11_REG,
	FIFO_12_REG,
	FIFO_13_REG,
	FIFO_14_REG,
	FIFO_15_REG,

	DMA_MEM_REG,
	DMA_INFO_REG,
	DMA_PHASE_REG,
	DMA_LENGTH_REG,
	DMA_READ_LOOP_REG,
	DMA_WRITE_LOOP_REG,
	DMA_RESET_REG,
	DMA_START_REG,

	CTRL_ADDR_REG,
	CTRL_DATA_REG,
	CTRL_RESET_REG,
	CTRL_START_REG,

	USER_0_REG,
	USER_1_REG,
	USER_2_REG,
	USER_4_REG,
};

enum {
	PORT_SHIFT = 0,
	PORT_BITS  = 8,
};

enum {
	DMA_MEM_CONST  = 0,
	DMA_MEM_CONTIG = 1,
};

enum {
	DMA_READ  = 0,
	DMA_WRITE = 1,
};

#define DMA_LOOP_COUNT_STRIDE(count, stride) ((count&0xFFFF) | (stride<<16))
#define DMA_INFO(inout, mode, fifo) ( (((inout)&1)<< 10) | (((mode) & 0x3) << 8) | ((fifo) & 0xFF) )

#define SUPER(x) (x)
#define PHASE(x) ((x)|0x40000000)
#define PATTERN(x) ((x)|0x80000000)

#define INPUT_OUTPUT_PATTERN(i,o) ((i)<<16|(o))
#define LENGTH_COUNT(a,b) ((a)<<16|(b))

#endif /* DMA_REGS_H */
